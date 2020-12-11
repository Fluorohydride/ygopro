#ifndef __ANDROID__
#error This file may only be compiled for android!
#endif

#include "porting_android.h"

#include <android_native_app_glue.h>
#include <jni.h>
#include <irrlicht.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <unistd.h>
#include "../log.h"
#include "../bufferio.h"
#include "../sound_manager.h"
#include "../game_config.h"
#include "../game.h"

#define JPARAMS(...)  "(" __VA_ARGS__ ")"
#define JSTRING "Ljava/lang/String;"
#define JINT "I"
#define JVOID "V"

#define JstringtoC(type, rettype, ...)\
std::rettype JstringtoC##type(JNIEnv* env, const jstring& jnistring) {\
	const size_t len = env->GetStringUTFLength(jnistring);\
	const char* text = env->GetStringUTFChars(jnistring, nullptr);\
	std::rettype res __VA_ARGS__({text, len});\
	env->ReleaseStringUTFChars(jnistring, text);\
	return res;\
}

JstringtoC(W, wstring, = BufferIO::DecodeUTF8s)
JstringtoC(A, string)

extern "C" {
	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_putMessageBoxResult(
		JNIEnv* env, jclass thiz, jstring textString, jboolean send_enter) {
		if(porting::app_global->userData) {
			auto device = static_cast<irr::IrrlichtDevice*>(porting::app_global->userData);
			auto irrenv = device->getGUIEnvironment();
			auto element = irrenv->getFocus();
			if(element && element->getType() == irr::gui::EGUIET_EDIT_BOX) {
				auto editbox = static_cast<irr::gui::IGUIEditBox*>(element);
				editbox->setText(JstringtoCW(env, textString).c_str());
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
		}
	}

	JNIEXPORT void JNICALL Java_io_github_edo9300_edopro_EpNativeActivity_putComboBoxResult(
		JNIEnv* env, jclass thiz, jint index) {
		if(porting::app_global->userData) {
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
		}
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
	jmethodID giid = jnienv->GetMethodID(acl, "getIntent", "()Landroid/content/Intent;");
	jobject intent = jnienv->CallObjectMethod(me, giid); //Got our intent

	jclass icl = jnienv->GetObjectClass(intent); //class pointer of Intent
	jmethodID gseid = jnienv->GetMethodID(icl, "getStringArrayExtra", "(Ljava/lang/String;)[Ljava/lang/String;");

	jobjectArray stringArrays = (jobjectArray)jnienv->CallObjectMethod(intent, gseid, jnienv->NewStringUTF("ARGUMENTS"));

	int size = jnienv->GetArrayLength(stringArrays);

	for(int i = 0; i < size; ++i) {
		jstring string = (jstring)jnienv->GetObjectArrayElement(stringArrays, i);
		ret.push_back(JstringtoCA(jnienv, string));
	}
	return ret;
}

jclass findClass(std::string classname, JNIEnv* env = nullptr) {
	env = env ? env : jnienv;
	if(env == 0) {
		return 0;
	}
	jclass nativeactivity = env->FindClass("android/app/NativeActivity");
	jmethodID getClassLoader =
		env->GetMethodID(nativeactivity, "getClassLoader",
						 "()Ljava/lang/ClassLoader;");
	jobject cls =
		env->CallObjectMethod(app_global->activity->clazz, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass =
		env->GetMethodID(classLoader, "loadClass",
						 "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName =
		env->NewStringUTF(classname.c_str());
	return (jclass)env->CallObjectMethod(cls, findClass, strClassName);
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
	// Attaches the current thread to the JVM.
	jint lResult;
	jint lFlags = 0;

	JavaVM* lJavaVM = app_global->activity->vm;
	JNIEnv* lJNIEnv = app_global->activity->env;

	JavaVMAttachArgs lJavaVMAttachArgs;
	lJavaVMAttachArgs.version = JNI_VERSION_1_6;
	lJavaVMAttachArgs.name = "NativeThread";
	lJavaVMAttachArgs.group = nullptr;

	lResult = lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
	if(lResult == JNI_ERR) {
		return;
	}

	// Retrieves NativeActivity.
	jobject lNativeActivity = app_global->activity->clazz;
	jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE =
		lJNIEnv->GetStaticFieldID(ClassContext,
								  "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
	jobject INPUT_METHOD_SERVICE =
		lJNIEnv->GetStaticObjectField(ClassContext,
									  FieldINPUT_METHOD_SERVICE);

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = lJNIEnv->FindClass(
		"android/view/inputmethod/InputMethodManager");
	jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(
		ClassNativeActivity, "getSystemService",
		"(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lInputMethodManager = lJNIEnv->CallObjectMethod(
		lNativeActivity, MethodGetSystemService,
		INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = lJNIEnv->GetMethodID(
		ClassNativeActivity, "getWindow",
		"()Landroid/view/Window;");
	jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity,
												MethodGetWindow);
	jclass ClassWindow = lJNIEnv->FindClass(
		"android/view/Window");
	jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(
		ClassWindow, "getDecorView", "()Landroid/view/View;");
	jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow,
												   MethodGetDecorView);

	if(pShow) {
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = lJNIEnv->GetMethodID(
			ClassInputMethodManager, "showSoftInput",
			"(Landroid/view/View;I)Z");
		jboolean lResult = lJNIEnv->CallBooleanMethod(
			lInputMethodManager, MethodShowSoftInput,
			lDecorView, lFlags);
	} else {
		// Runs lWindow.getViewToken()
		jclass ClassView = lJNIEnv->FindClass(
			"android/view/View");
		jmethodID MethodGetWindowToken = lJNIEnv->GetMethodID(
			ClassView, "getWindowToken", "()Landroid/os/IBinder;");
		jobject lBinder = lJNIEnv->CallObjectMethod(lDecorView,
													MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = lJNIEnv->GetMethodID(
			ClassInputMethodManager, "hideSoftInputFromWindow",
			"(Landroid/os/IBinder;I)Z");
		jboolean lRes = lJNIEnv->CallBooleanMethod(
			lInputMethodManager, MethodHideSoftInput,
			lBinder, lFlags);
	}

	// Finished with the JVM.
	lJavaVM->DetachCurrentThread();
}

/**
 * show text input dialog in java
 * @param acceptButton text to display on accept button
 * @param hint hint to show
 * @param current initial value to display
 * @param editType type of texfield
 * (1==multiline text input; 2==single line text input; 3=password field)
 */
void showInputDialog(epro::path_stringview current) {
	jmethodID showdialog = jnienv->GetMethodID(nativeActivity, "showDialog", JPARAMS(JSTRING)JVOID);

	if(showdialog == 0) {
		assert("porting::showInputDialog unable to find java show dialog method" == 0);
	}

	jstring jcurrent = jnienv->NewStringUTF(current.data());

	jnienv->CallVoidMethod(app_global->activity->clazz, showdialog, jcurrent);
}

void showComboBox(const std::vector<std::string>& list) {
	jmethodID showbox = jnienv->GetMethodID(nativeActivity, "showComboBox",
											"([Ljava/lang/String;)V");

	jstring str;
	jobjectArray jlist = 0;
	jsize len = list.size();

	jlist = jnienv->NewObjectArray(len, jnienv->FindClass("java/lang/String"), 0);

	for(int i = 0; i < list.size(); i++) {
		str = jnienv->NewStringUTF(list[i].c_str());
		jnienv->SetObjectArrayElement(jlist, i, str);
	}

	jnienv->CallVoidMethod(app_global->activity->clazz, showbox, jlist);
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
							porting::showInputDialog(BufferIO::EncodeUTF8s(((irr::gui::IGUIEditBox *)hovered)->getText()));
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
					break;
				}
				case APP_CMD_GAINED_FOCUS:
				case APP_CMD_LOST_FOCUS: {
					stopPropagation = true;
					break;
				}
				case APP_CMD_RESUME: {
					ygo::gSoundManager->PauseMusic(false);
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
jstring jargs = jnienv->NewStringUTF(arg.data());\
jnienv->CallVoidMethod(app_global->activity->clazz, name, jargs);\
}

JAVAVOIDSTRINGMETHOD(launchWindbot)
JAVAVOIDSTRINGMETHOD(installUpdate)
JAVAVOIDSTRINGMETHOD(openUrl)
JAVAVOIDSTRINGMETHOD(openFile)

void setTextToClipboard(epro::wstringview text) {
	jmethodID setClip = jnienv->GetMethodID(nativeActivity, "setClipboard", JPARAMS(JSTRING)JVOID);
	if(setClip == 0)
		assert("porting::setTextToClipboard unable to find java setClipboard method" == 0);
	jstring jargs = jnienv->NewStringUTF(BufferIO::EncodeUTF8s(text).data());

	jnienv->CallVoidMethod(app_global->activity->clazz, setClip, jargs);
}

const wchar_t* getTextFromClipboard() {
	static std::wstring text;
	jmethodID getClip = jnienv->GetMethodID(nativeActivity, "getClipboard", JPARAMS()JSTRING);
	if(getClip == 0)
		assert("porting::getTextFromClipboard unable to find java getClipboard method" == 0);
	jstring js_clip = (jstring)jnienv->CallObjectMethod(app_global->activity->clazz, getClip);
	text = JstringtoCW(jnienv, js_clip);
	return text.c_str();
}

}

extern int main(int argc, char *argv[]);

void android_main(android_app *app) {
	int retval = 0;
	porting::app_global = app;
	porting::initAndroid();
	porting::internal_storage = app->activity->internalDataPath;

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