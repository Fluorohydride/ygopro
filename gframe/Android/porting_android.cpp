#ifndef __ANDROID__
#error This file may only be compiled for android!
#endif

#include "porting_android.h"

#include <android_native_app_glue.h>
#include <jni.h>
#include <irrlicht.h>
#include <vector>
#include <unistd.h>
#include "../log.h"
#include "../bufferio.h"
#include "../sound_manager.h"
#include "../game_config.h"
#include "../game.h"

#define JPARAMS(...)  "(" __VA_ARGS__ ")"
#define JARRAY(...) "[" __VA_ARGS__
#define JSTRING "Ljava/lang/String;"
#define JINT "I"
#define JVOID "V"
#define JBYTE "B"
#define JBOOL "Z"

namespace {

std::string JstringtoCA(JNIEnv* env, const jstring& jnistring) {
	if(!jnistring)
		return "";

	const auto stringClass = env->GetObjectClass(jnistring);
	const auto getBytes = env->GetMethodID(stringClass, "getBytes", JPARAMS(JSTRING)JARRAY(JBYTE));
	jstring UTF8_STRING = env->NewStringUTF("UTF-8");
	const auto stringJbytes = static_cast<jbyteArray>(env->CallObjectMethod(jnistring, getBytes, UTF8_STRING));

	size_t length = (size_t)env->GetArrayLength(stringJbytes);
	jbyte* pBytes = env->GetByteArrayElements(stringJbytes, nullptr);

	std::string ret{ (char*)pBytes, length };
	env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

	env->DeleteLocalRef(stringJbytes);
	env->DeleteLocalRef(stringClass);
	env->DeleteLocalRef(UTF8_STRING);
	return ret;
}

inline std::wstring JstringtoCW(JNIEnv* env, const jstring& jnistring) {
	return BufferIO::DecodeUTF8(JstringtoCA(env, jnistring));
}

//calls: Charset.forName("UTF-8").decode(bb).toString()
jstring NewJavaString(JNIEnv* env, epro::stringview string) {
	jobject bb = env->NewDirectByteBuffer((void*)string.data(), string.size());

	jclass cls_Charset = env->FindClass("java/nio/charset/Charset");
	jmethodID mid_Charset_forName = env->GetStaticMethodID(cls_Charset, "forName", JPARAMS(JSTRING)"Ljava/nio/charset/Charset;");
	jstring UTF8_STRING = env->NewStringUTF("UTF-8");
	jobject charset = env->CallStaticObjectMethod(cls_Charset, mid_Charset_forName, UTF8_STRING);

	jmethodID mid_Charset_decode = env->GetMethodID(cls_Charset, "decode", JPARAMS("Ljava/nio/ByteBuffer;")"Ljava/nio/CharBuffer;");
	jobject cb = env->CallObjectMethod(charset, mid_Charset_decode, bb);

	jclass cls_CharBuffer = env->FindClass("java/nio/CharBuffer");
	jmethodID mid_CharBuffer_toString = env->GetMethodID(cls_CharBuffer, "toString", JPARAMS()JSTRING);
	jstring ret = static_cast<jstring>(env->CallObjectMethod(cb, mid_CharBuffer_toString));

	env->DeleteLocalRef(cls_Charset);
	env->DeleteLocalRef(cls_CharBuffer);
	env->DeleteLocalRef(charset);
	env->DeleteLocalRef(cb);
	env->DeleteLocalRef(bb);
	env->DeleteLocalRef(UTF8_STRING);

	return ret;
}

std::mutex* queued_messages_mutex = nullptr;
std::atomic_bool error_dialog_returned{ true };
std::deque<std::function<void()>>* events = nullptr;
std::unique_ptr<std::unique_lock<std::mutex>> mainGameMutex = nullptr;
}

extern "C" {
	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_putMessageBoxResult(
		JNIEnv* env, jclass thiz, jstring textString, jboolean send_enter) {
		if(porting::app_global->userData) {
			queued_messages_mutex->lock();
			events->emplace_back([send_enter,text=JstringtoCW(env, textString)](){
				auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
				auto irrenv = device->getGUIEnvironment();
				auto element = irrenv->getFocus();
				if(element && element->getType() == irr::gui::EGUIET_EDIT_BOX) {
					auto editbox = static_cast<irr::gui::IGUIEditBox*>(element);
					editbox->setText(text.data());
					irrenv->removeFocus(editbox);
					irrenv->setFocus(editbox->getParent());
					irr::SEvent changeEvent;
					changeEvent.EventType = irr::EET_GUI_EVENT;
					changeEvent.GUIEvent.Caller = editbox;
					changeEvent.GUIEvent.Element = 0;
					changeEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_CHANGED;
					editbox->getParent()->OnEvent(changeEvent);
					if(send_enter) {
						irr::SEvent enterEvent;
						enterEvent.EventType = irr::EET_GUI_EVENT;
						enterEvent.GUIEvent.Caller = editbox;
						enterEvent.GUIEvent.Element = 0;
						enterEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_ENTER;
						editbox->getParent()->OnEvent(enterEvent);
					}
				}
			});
			queued_messages_mutex->unlock();
		}
	}

	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_putComboBoxResult(
		JNIEnv* env, jclass thiz, jint index) {
		if(porting::app_global->userData) {
			queued_messages_mutex->lock();
			events->emplace_back([index](){
				auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
				auto irrenv = device->getGUIEnvironment();
				auto element = irrenv->getFocus();
				if(element && element->getType() == irr::gui::EGUIET_COMBO_BOX) {
					auto combobox = static_cast<irr::gui::IGUIComboBox*>(element);
					combobox->setSelected(index);
					irr::SEvent changeEvent;
					changeEvent.EventType = irr::EET_GUI_EVENT;
					changeEvent.GUIEvent.Caller = combobox;
					changeEvent.GUIEvent.Element = 0;
					changeEvent.GUIEvent.EventType = irr::gui::EGET_COMBO_BOX_CHANGED;
					combobox->getParent()->OnEvent(changeEvent);
				}
			});
			queued_messages_mutex->unlock();
		}
	}

	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_errorDialogReturn(
		JNIEnv* env, jclass thiz) {
		error_dialog_returned = true;
	}
}

namespace porting {

std::string internal_storage = "";
std::string working_directory = "";

android_app* app_global = nullptr;
JNIEnv*      jnienv = nullptr;
jclass       nativeActivity;

std::vector<std::string> GetExtraParameters() {
	std::vector<std::string> ret;
	ret.push_back("");//dummy arg 0

	jobject me = app_global->activity->clazz;

	jclass acl = jnienv->GetObjectClass(me); //class pointer of NativeActivity
	jmethodID giid = jnienv->GetMethodID(acl, "getIntent", JPARAMS()"Landroid/content/Intent;");
	jobject intent = jnienv->CallObjectMethod(me, giid); //Got our intent

	jnienv->DeleteLocalRef(acl);

	jclass icl = jnienv->GetObjectClass(intent); //class pointer of Intent
	jmethodID gseid = jnienv->GetMethodID(icl, "getStringArrayExtra", JPARAMS(JSTRING)JARRAY(JSTRING));
	jnienv->DeleteLocalRef(icl);

	auto argstring = jnienv->NewStringUTF("ARGUMENTS");

	jobjectArray stringArrays = (jobjectArray)jnienv->CallObjectMethod(intent, gseid, argstring);

	jnienv->DeleteLocalRef(argstring);
	jnienv->DeleteLocalRef(intent);

	int size = jnienv->GetArrayLength(stringArrays);

	for(int i = 0; i < size; ++i) {
		jstring string = (jstring)jnienv->GetObjectArrayElement(stringArrays, i);
		ret.push_back(JstringtoCA(jnienv, string));
		jnienv->DeleteLocalRef(string);
	}
	jnienv->DeleteLocalRef(stringArrays);
	return ret;
}

jclass findClass(std::string classname, JNIEnv* env = nullptr) {
	env = env ? env : jnienv;
	if(env == nullptr)
		return 0;

	jclass nativeactivity = env->FindClass("android/app/NativeActivity");
	jmethodID getClassLoader = env->GetMethodID(nativeactivity, "getClassLoader", JPARAMS()"Ljava/lang/ClassLoader;");
	jobject cls = env->CallObjectMethod(app_global->activity->clazz, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass = env->GetMethodID(classLoader, "loadClass", JPARAMS(JSTRING)"Ljava/lang/Class;");
	jstring strClassName = env->NewStringUTF(classname.c_str());
	auto ret = static_cast<jclass>(env->CallObjectMethod(cls, findClass, strClassName));

	jnienv->DeleteLocalRef(nativeactivity);
	jnienv->DeleteLocalRef(cls);
	jnienv->DeleteLocalRef(classLoader);
	jnienv->DeleteLocalRef(strClassName);

	return ret;
}

void initAndroid() {
	jnienv = nullptr;
	JavaVM *jvm = app_global->activity->vm;
	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "Edopro NativeThread";
	lJavaVMAttachArgs.group = nullptr;
	if(jvm->AttachCurrentThread(&jnienv, &lJavaVMAttachArgs) == JNI_ERR) {
		LOGE("Couldn't attach current thread");
		exit(-1);
	}
	nativeActivity = findClass("io/github/edo9300/edopro/EpNativeActivity");
	if(!nativeActivity){
		LOGE("Couldn't retrieve nativeActivity");
		exit(-1);
	}
}

void cleanupAndroid() {
	JavaVM *jvm = app_global->activity->vm;
	jvm->DetachCurrentThread();
}

void displayKeyboard(bool pShow) {	
	// Retrieves NativeActivity.
	jobject lNativeActivity = app_global->activity->clazz;
	jclass ClassNativeActivity = jnienv->GetObjectClass(lNativeActivity);

	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = jnienv->FindClass("android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE =
		jnienv->GetStaticFieldID(ClassContext,
								  "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject INPUT_METHOD_SERVICE =
		jnienv->GetStaticObjectField(ClassContext,
									  FieldINPUT_METHOD_SERVICE);

	jnienv->DeleteLocalRef(ClassContext);

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = jnienv->FindClass(
		"android/view/inputmethod/InputMethodManager");
	jmethodID MethodGetSystemService = jnienv->GetMethodID(
		ClassNativeActivity, "getSystemService", JPARAMS(JSTRING)"Ljava/lang/Object;");
	jobject lInputMethodManager = jnienv->CallObjectMethod(
		lNativeActivity, MethodGetSystemService,
		INPUT_METHOD_SERVICE);

	jnienv->DeleteLocalRef(INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = jnienv->GetMethodID(
		ClassNativeActivity, "getWindow", JPARAMS()"Landroid/view/Window;");
	jobject lWindow = jnienv->CallObjectMethod(lNativeActivity,
												MethodGetWindow);
	jclass ClassWindow = jnienv->FindClass(
		"android/view/Window");
	jmethodID MethodGetDecorView = jnienv->GetMethodID(
		ClassWindow, "getDecorView", JPARAMS()"Landroid/view/View;");
	jobject lDecorView = jnienv->CallObjectMethod(lWindow,
												   MethodGetDecorView);

	jnienv->DeleteLocalRef(lWindow);
	jnienv->DeleteLocalRef(ClassWindow);

	jint lFlags = 0;
	if(pShow) {
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = jnienv->GetMethodID(
			ClassInputMethodManager, "showSoftInput", JPARAMS("Landroid/view/View;" JINT)JBOOL);
		jboolean lResult = jnienv->CallBooleanMethod(
			lInputMethodManager, MethodShowSoftInput,
			lDecorView, lFlags);
	} else {
		// Runs lWindow.getViewToken()
		jclass ClassView = jnienv->FindClass(
			"android/view/View");
		jmethodID MethodGetWindowToken = jnienv->GetMethodID(
			ClassView, "getWindowToken", JPARAMS()"Landroid/os/IBinder;");

		jnienv->DeleteLocalRef(ClassView);

		jobject lBinder = jnienv->CallObjectMethod(lDecorView,
													MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = jnienv->GetMethodID(
			ClassInputMethodManager, "hideSoftInputFromWindow",
			JPARAMS("Landroid/os/IBinder;" JINT)JBOOL);
		jboolean lRes = jnienv->CallBooleanMethod(
			lInputMethodManager, MethodHideSoftInput,
			lBinder, lFlags);

		jnienv->DeleteLocalRef(lBinder);
	}

	jnienv->DeleteLocalRef(ClassNativeActivity);
	jnienv->DeleteLocalRef(lInputMethodManager);
	jnienv->DeleteLocalRef(ClassInputMethodManager);
	jnienv->DeleteLocalRef(lDecorView);
}

void showInputDialog(epro::path_stringview current) {
	jmethodID showdialog = jnienv->GetMethodID(nativeActivity, "showDialog", JPARAMS(JSTRING)JVOID);

	if(showdialog == 0) {
		assert("porting::showInputDialog unable to find java show dialog method" == 0);
	}

	jstring jcurrent = NewJavaString(jnienv, current);

	jnienv->CallVoidMethod(app_global->activity->clazz, showdialog, jcurrent);

	jnienv->DeleteLocalRef(jcurrent);
}

void showComboBox(const std::vector<std::string>& list) {
	jmethodID showbox = jnienv->GetMethodID(nativeActivity, "showComboBox", JPARAMS(JARRAY(JSTRING))JVOID);

	jsize len = list.size();
	jobjectArray jlist = jnienv->NewObjectArray(len, jnienv->FindClass("java/lang/String"), 0);

	for(int i = 0; i < list.size(); i++) {
		auto jstring = NewJavaString(jnienv, list[i]);
		jnienv->SetObjectArrayElement(jlist, i, jstring);
		jnienv->DeleteLocalRef(jstring);
	}

	jnienv->CallVoidMethod(app_global->activity->clazz, showbox, jlist);

	jnienv->DeleteLocalRef(jlist);
}

bool transformEvent(const irr::SEvent & event, bool& stopPropagation) {
	static irr::core::position2di m_pointer = irr::core::position2di(0, 0);
	auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
	switch(event.EventType) {
		case irr::EET_MOUSE_INPUT_EVENT: {
			if(event.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
				auto hovered = ygo::mainGame->env->getRootGUIElement()->getElementFromPoint({ event.MouseInput.X, event.MouseInput.Y });
				if(hovered && hovered->isEnabled()) {
					if(hovered->getType() == irr::gui::EGUIET_EDIT_BOX) {
						bool retval = hovered->OnEvent(event);
						if(retval)
							ygo::mainGame->env->setFocus(hovered);
						if(ygo::gGameConfig->native_keyboard) {
							porting::displayKeyboard(true);
						} else {
							porting::showInputDialog(BufferIO::EncodeUTF8(((irr::gui::IGUIEditBox *)hovered)->getText()));
						}
						stopPropagation = retval;
						return retval;
					}
				}
			}
			break;
		}
		case irr::EET_KEY_INPUT_EVENT: {
			if(ygo::gGameConfig->native_keyboard && event.KeyInput.Key == irr::KEY_RETURN) {
				porting::displayKeyboard(false);
			}
			break;
		}
		case irr::EET_SYSTEM_EVENT: {
			stopPropagation = false;
			switch(event.SystemEvent.AndroidCmd.Cmd) {
				case APP_CMD_PAUSE: {
					ygo::mainGame->SaveConfig();
					ygo::gSoundManager->PauseMusic(true);
					if(mainGameMutex == nullptr)
						mainGameMutex = std::unique_ptr<std::unique_lock<std::mutex>>(new std::unique_lock<std::mutex>(ygo::mainGame->gMutex));
					break;
				}
				case APP_CMD_GAINED_FOCUS:
				case APP_CMD_LOST_FOCUS: {
					stopPropagation = true;
					break;
				}
				case APP_CMD_RESUME: {
					ygo::gSoundManager->PauseMusic(false);
					mainGameMutex = nullptr;
					break;
				}
				default: break;
			}
			return true;
		}
									
		/*
		* partial implementation from https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/src/gui/modalMenu.cpp#L116
		* with this patch applied to the engine https://github.com/minetest/minetest/blob/02a23892f94d3c83a6bdc301defc0e7ade7e1c2b/build/android/patches/irrlicht-touchcount.patch
		*/
		case irr::EET_TOUCH_INPUT_EVENT: {
			irr::SEvent translated;
			memset(&translated, 0, sizeof(irr::SEvent));
			translated.EventType = irr::EET_MOUSE_INPUT_EVENT;

			translated.MouseInput.X = event.TouchInput.X;
			translated.MouseInput.Y = event.TouchInput.Y;
			translated.MouseInput.Control = false;

			switch(event.TouchInput.touchedCount) {
				case 1: {
					switch(event.TouchInput.Event) {
						case irr::ETIE_PRESSED_DOWN:
							m_pointer = irr::core::position2di(event.TouchInput.X, event.TouchInput.Y);
							translated.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
							translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;
							irr::SEvent hoverEvent;
							hoverEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
							hoverEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
							hoverEvent.MouseInput.X = event.TouchInput.X;
							hoverEvent.MouseInput.Y = event.TouchInput.Y;
							device->postEventFromUser(hoverEvent);
							break;
						case irr::ETIE_MOVED:
							m_pointer = irr::core::position2di(event.TouchInput.X, event.TouchInput.Y);
							translated.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
							translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;
							break;
						case irr::ETIE_LEFT_UP:
							translated.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
							translated.MouseInput.ButtonStates = 0;
							// we don't have a valid pointer element use last
							// known pointer pos
							translated.MouseInput.X = m_pointer.X;
							translated.MouseInput.Y = m_pointer.Y;
							break;
						default:
							stopPropagation = true;
							return true;
					}
					break;
				}
				case 2: {
					if(event.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
						translated.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
						translated.MouseInput.ButtonStates = irr::EMBSM_LEFT | irr::EMBSM_RIGHT;
						translated.MouseInput.X = m_pointer.X;
						translated.MouseInput.Y = m_pointer.Y;
						device->postEventFromUser(translated);

						translated.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
						translated.MouseInput.ButtonStates = irr::EMBSM_LEFT;

						device->postEventFromUser(translated);
					}
					return true;
				}
				case 3: {
					if(event.TouchInput.Event == irr::ETIE_LEFT_UP) {
						translated.EventType = irr::EET_KEY_INPUT_EVENT;
						translated.KeyInput.Control = true;
						translated.KeyInput.PressedDown = false;
						translated.KeyInput.Key = irr::KEY_KEY_O;
						device->postEventFromUser(translated);
					}
					return true;
				}
				default:
					return true;
			}

			bool retval = device->postEventFromUser(translated);

			if(event.TouchInput.Event == irr::ETIE_LEFT_UP) {
				m_pointer = irr::core::position2di(0, 0);
			}
			stopPropagation = retval;
			return true;
		}
		default: break;
	}
	return false;
}

int getLocalIP() {
	jmethodID getIP = jnienv->GetMethodID(nativeActivity, "getLocalIpAddress", JPARAMS()JINT);
	if(getIP == 0) {
		assert("porting::getLocalIP unable to find java getLocalIpAddress method" == 0);
	}
	int value = jnienv->CallIntMethod(app_global->activity->clazz, getIP);
	return value;
}

#define JAVAVOIDSTRINGMETHOD(name)\
void name(epro::path_stringview arg) {\
jmethodID name = jnienv->GetMethodID(nativeActivity, #name, JPARAMS(JSTRING)JVOID);\
if(name == 0) assert("porting::" #name " unable to find java " #name " method" == 0);\
jstring jargs = NewJavaString(jnienv, arg);\
jnienv->CallVoidMethod(app_global->activity->clazz, name, jargs);\
jnienv->DeleteLocalRef(jargs);\
}

JAVAVOIDSTRINGMETHOD(launchWindbot)
JAVAVOIDSTRINGMETHOD(addWindbotDatabase)
JAVAVOIDSTRINGMETHOD(installUpdate)
JAVAVOIDSTRINGMETHOD(openUrl)
JAVAVOIDSTRINGMETHOD(openFile)
JAVAVOIDSTRINGMETHOD(shareFile)

void showErrorDialog(epro::stringview context, epro::stringview message) {
	jmethodID showDialog = jnienv->GetMethodID(nativeActivity, "showErrorDialog", JPARAMS(JSTRING JSTRING)JVOID);
	if(showDialog == 0)
		assert("porting::showErrorDialog unable to find java showErrorDialog method" == 0);
	jstring jcontext = NewJavaString(jnienv, context);
	jstring jmessage = NewJavaString(jnienv, message);

	error_dialog_returned = false;

	jnienv->CallVoidMethod(app_global->activity->clazz, showDialog, jcontext, jmessage);

	jnienv->DeleteLocalRef(jcontext);
	jnienv->DeleteLocalRef(jmessage);

	//keep parsing events so that the activity is drawn properly
	int Events = 0;
	int ident = 0;
	android_poll_source* source = 0;
	while(!error_dialog_returned &&
		ALooper_pollAll(-1, nullptr, &Events, (void**)&source) >= 0 &&
		app_global->destroyRequested == 0) {
		if(source != NULL)
			source->process(app_global, source);
	}
}

void setTextToClipboard(epro::wstringview text) {
	jmethodID setClip = jnienv->GetMethodID(nativeActivity, "setClipboard", JPARAMS(JSTRING)JVOID);
	if(setClip == 0)
		assert("porting::setTextToClipboard unable to find java setClipboard method" == 0);
	jstring jargs = NewJavaString(jnienv, BufferIO::EncodeUTF8(text));

	jnienv->CallVoidMethod(app_global->activity->clazz, setClip, jargs);

	jnienv->DeleteLocalRef(jargs);
}

const wchar_t* getTextFromClipboard() {
	static std::wstring text;
	jmethodID getClip = jnienv->GetMethodID(nativeActivity, "getClipboard", JPARAMS()JSTRING);
	if(getClip == 0)
		assert("porting::getTextFromClipboard unable to find java getClipboard method" == 0);
	jstring js_clip = (jstring)jnienv->CallObjectMethod(app_global->activity->clazz, getClip);
	text = JstringtoCW(jnienv, js_clip);

	jnienv->DeleteLocalRef(js_clip);
	return text.c_str();
}

void dispatchQueuedMessages() {
	auto& _events = *events;
	std::unique_lock<std::mutex> lock(*queued_messages_mutex);
	while(!_events.empty()) {
		const auto event = _events.front();
		_events.pop_front();
		lock.unlock();
		event();
		lock.lock();
	}
}

}

extern int main(int argc, char *argv[]);

void android_main(android_app *app) {
	int retval = 0;
	porting::app_global = app;
	porting::initAndroid();
	porting::internal_storage = app->activity->internalDataPath;
	std::mutex _queued_messages_mutex;
	queued_messages_mutex = &_queued_messages_mutex;
	std::deque<std::function<void()>> _events;
	events=&_events;

	auto strparams = porting::GetExtraParameters();
	std::vector<const char*> params;

	for(auto& param : strparams) {
		params.push_back(param.c_str());
	}
	//no longer needed after ndk 15c
	//app_dummy();
	retval = main(params.size(), (char**)params.data());

	porting::cleanupAndroid();
	exit(retval);
}