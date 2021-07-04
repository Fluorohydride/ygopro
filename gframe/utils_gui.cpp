#include "utils_gui.h"
#include <irrlicht.h>
#include <ICursorControl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include "utils.h"
#include "game_config.h"
#include "text_types.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include "logging.h"
#include "Base64.h"
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
#include "IrrlichtCommonIncludes1.9/CCursorControl.h"
using CCursorControl = irr::CIrrDeviceWin32::CCursorControl;
#else
#include "IrrlichtCommonIncludes/CCursorControl.h"
using CCursorControl = irr::CCursorControl;
#endif
#elif defined(__ANDROID__)
#include "Android/porting_android.h"
#elif defined(__APPLE__)
#import <CoreFoundation/CoreFoundation.h>
#include "osx_menu.h"
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dlfcn.h>
#include <memory>
namespace {
using Bool_v = int;
using Atom_v = unsigned long;
using Display_v = void;
using Window_v = unsigned long;
using Status_v = int;
using XEvent_v = void;
struct X11Helper {
	X11Helper();
	~X11Helper();
	void* LibX11{ nullptr };
	Display_v* (*XOpenDisplay)(const char*) { nullptr };
	int (*XGetInputFocus)(Display_v*, Window_v*, int*) { nullptr };
	Atom_v(*XInternAtom)(Display_v*, const char*, Bool_v) { nullptr };
	int (*XGetWindowProperty)(Display_v*, Window_v, Atom_v, long, long, Bool_v, Atom_v, Atom_v*, int*, unsigned long*, unsigned long*, unsigned char**) { nullptr };
	int (*XFree)(void*) { nullptr };
	Status_v(*XSendEvent)(Display_v*, Window_v, Bool_v, long, XEvent_v*) { nullptr };
	int (*XChangeProperty)(Display_v*, Window_v, Atom_v, Atom_v, int, int, const unsigned char*, int) { nullptr };
	int (*XMapWindow)(Display_v*, Window_v) { nullptr };
	int (*XFlush)(Display_v*) { nullptr };
};
}

static std::unique_ptr<X11Helper> X11{ nullptr };
#endif

namespace ygo {

#ifdef _WIN32
static HWND GetWindowHandle(irr::video::IVideoDriver* driver) {
	switch(driver->getDriverType()) {
#if !(IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	case irr::video::EDT_DIRECT3D8:
		return static_cast<HWND>(driver->getExposedVideoData().D3D8.HWnd);
#endif
	case irr::video::EDT_DIRECT3D9:
		return static_cast<HWND>(driver->getExposedVideoData().D3D9.HWnd);
	case irr::video::EDT_OPENGL:
#if (IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	case irr::video::EDT_OGLES1:
	case irr::video::EDT_OGLES2:
#endif
		return static_cast<HWND>(driver->getExposedVideoData().OpenGLWin32.HWnd);
	default:
		break;
	}
	return nullptr;
}
#endif

irr::IrrlichtDevice* GUIUtils::CreateDevice(GameConfig* configs) {
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = configs->antialias;
	params.Vsync = configs->vsync;
	params.DriverType = configs->driver_type;
#if (IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	params.OGLES2ShaderPath = EPRO_TEXT("BUNDLED");
	params.WindowResizable = true;
#endif
#ifndef __ANDROID__
	params.WindowSize = irr::core::dimension2d<irr::u32>((irr::u32)(1024 * configs->dpi_scale), (irr::u32)(640 * configs->dpi_scale));
#else
	params.PrivateData = porting::app_global;
	params.Bits = 24;
	params.ZBufferBits = 16;
	params.AntiAlias = 0;
	params.WindowSize = irr::core::dimension2du(0, 0);
#endif
	irr::IrrlichtDevice* device = irr::createDeviceEx(params);
	if(!device)
		throw std::runtime_error("Failed to create Irrlicht Engine device!");
	const auto driver = device->getVideoDriver();
	if(!driver)
		throw std::runtime_error("Failed to create video driver!");
#ifdef __ANDROID__
	auto filesystem = device->getFileSystem();
	// The Android assets file-system does not know which sub-directories it has (blame google).
	// So we have to add all sub-directories in assets manually. Otherwise we could still open the files,
	// but existFile checks will fail (which are for example needed by getFont).
	for(int i = 0; i < filesystem->getFileArchiveCount(); ++i) {
		auto archive = filesystem->getFileArchive(i);
		if(archive->getType() == irr::io::EFAT_ANDROID_ASSET) {
			archive->addDirectoryToFileList("media/");
			break;
		}
	}
	device->getGUIEnvironment()->setOSOperator(Utils::OSOperator);
	if(!driver->queryFeature(irr::video::EVDF_TEXTURE_NPOT))
		driver->setTextureCreationFlag(irr::video::ETCF_ALLOW_NON_POWER_2, true);
#endif
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	device->setWindowCaption(L"Project Ignis: EDOPro");
#if !(IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	device->setResizable(true);
#endif
#ifdef _WIN32
	auto hInstance = static_cast<HINSTANCE>(GetModuleHandle(nullptr));
	auto hSmallIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	auto hBigIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR));
	auto hWnd = GetWindowHandle(driver);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(hSmallIcon));
	SendMessage(hWnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(hBigIcon));
	if(gGameConfig->windowStruct.size()) {
		auto winstruct = base64_decode(gGameConfig->windowStruct);
		if(winstruct.size() == sizeof(WINDOWPLACEMENT))
			SetWindowPlacement(hWnd, reinterpret_cast<WINDOWPLACEMENT*>(winstruct.data()));
	}
#elif defined(__APPLE__)
	if(gGameConfig->windowStruct.size())
		EDOPRO_SetWindowRect(driver->getExposedVideoData().OpenGLOSX.Window, gGameConfig->windowStruct.data());
#endif
	device->getLogger()->setLogLevel(irr::ELL_ERROR);
#if defined(__linux__) && !defined(__ANDROID__)
	X11 = std::make_unique<X11Helper>();
#endif
	return device;
}

void GUIUtils::ChangeCursor(irr::IrrlichtDevice* device, /*irr::gui::ECURSOR_ICON*/ int _icon) {
#ifndef __ANDROID__
	auto icon = static_cast<irr::gui::ECURSOR_ICON>(_icon);
	auto cursor = device->getCursorControl();
	if (cursor->getActiveIcon() != icon) {
		cursor->setActiveIcon(icon);
	}
#endif
}

bool GUIUtils::TakeScreenshot(irr::IrrlichtDevice* device) {
	const auto driver = device->getVideoDriver();
	const auto image = driver->createScreenShot();
	if(!image)
		return false;
	const auto now = std::time(nullptr);
	const auto filename = fmt::format(EPRO_TEXT("screenshots/EDOPro {:%Y-%m-%d %H-%M-%S}.png"), *std::localtime(&now));
	auto written = driver->writeImageToFile(image, { filename.data(), static_cast<irr::u32>(filename.size()) });
	if(!written)
		device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);
	image->drop();
	return written;
}

#ifdef _WIN32
//gcc on mingw can't convert lambda to __stdcall function
static BOOL CALLBACK callback(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) {
	auto monitors = reinterpret_cast<std::vector<RECT>*>(pData);
	monitors->push_back(*lprcMonitor);
	return TRUE;
}
#endif

void GUIUtils::ToggleFullscreen(irr::IrrlichtDevice* device, bool& fullscreen) {
#ifdef _WIN32
	static WINDOWPLACEMENT nonFullscreenSize;
	static LONG_PTR nonFullscreenStyle;
	static constexpr LONG_PTR fullscreenStyle = WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	static const auto monitors = [] {
		std::vector<RECT> ret;
		EnumDisplayMonitors(0, 0, callback, reinterpret_cast<LPARAM>(&ret));
		return ret;
	}();
	fullscreen = !fullscreen;
	const auto driver = device->getVideoDriver();
	auto hWnd = GetWindowHandle(driver);
	if(fullscreen) {
		GetWindowPlacement(hWnd, &nonFullscreenSize);
		nonFullscreenStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
		RECT curSize{};
		GetWindowRect(hWnd, &curSize);
		const POINT windowCenter = { (curSize.left + (curSize.right - curSize.left) / 2), (curSize.top + (curSize.bottom - curSize.top) / 2) };
		for(const auto& rect : monitors) {
			if(PtInRect(&rect, windowCenter)) {
				curSize = rect;
				break;
			}
		}
		if(!SetWindowLongPtr(hWnd, GWL_STYLE, fullscreenStyle))
			ErrorLog("Could not change window style.");

		const auto width = curSize.right - curSize.left;
		const auto height = curSize.bottom - curSize.top;

		SetWindowPos(hWnd, HWND_TOP, curSize.left, curSize.top, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	} else {
		SetWindowPlacement(hWnd, &nonFullscreenSize);
		SetWindowLongPtr(hWnd, GWL_STYLE, nonFullscreenStyle);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	static_cast<CCursorControl*>(device->getCursorControl())->updateBorderSize(fullscreen, true);
#elif defined(__linux__) && !defined(__ANDROID__)
	if(!X11->LibX11)
		return;
	struct {
		unsigned long   flags;
		unsigned long   functions;
		unsigned long   decorations;
		long			inputMode;
		unsigned long   status;
	} hints{};
	auto display = X11->XOpenDisplay(nullptr);
	Window window;
	static bool wasHorizontalMaximized = false, wasVerticalMaximized = false;
	int revert;
	fullscreen = !fullscreen;
	X11->XGetInputFocus(display, &window, &revert);

	Atom wm_state = X11->XInternAtom(display, "_NET_WM_STATE", false);
	Atom max_horz = X11->XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
	Atom max_vert = X11->XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);

	auto checkMaximized = [&]() {
		const long maxLength = 1024;
		Atom actualType;
		int actualFormat;
		unsigned long numItems, bytesAfter;
		unsigned char* propertyValue = nullptr;
		wasVerticalMaximized = false;
		wasHorizontalMaximized = false;
		if(X11->XGetWindowProperty(display, window, wm_state,
							  0l, maxLength, false, XA_ATOM, &actualType,
							  &actualFormat, &numItems, &bytesAfter,
							  &propertyValue) == Success) {
			Atom* atoms = (Atom*)propertyValue;
			for(unsigned long i = 0; i < numItems; ++i) {
				if(atoms[i] == max_vert) {
					wasVerticalMaximized = true;
				} else if(atoms[i] == max_horz) {
					wasHorizontalMaximized = true;
				}
			}
			X11->XFree(propertyValue);
		}
	};
	if(fullscreen)
		checkMaximized();
	if(!wasHorizontalMaximized && !wasVerticalMaximized) {
		XEvent xev = {};
		xev.type = ClientMessage;
		xev.xclient.window = window;
		xev.xclient.message_type = wm_state;
		xev.xclient.format = 32;
		xev.xclient.data.l[0] = fullscreen ? 1 : 0;
		int i = 1;
		if(!wasHorizontalMaximized)
			xev.xclient.data.l[i++] = max_horz;
		if(!wasVerticalMaximized)
			xev.xclient.data.l[i++] = max_vert;
		if(i == 2)
			xev.xclient.data.l[i] = 0;
		X11->XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);
	}

	Atom property = X11->XInternAtom(display, "_MOTIF_WM_HINTS", true);
	hints.flags = 2;
	hints.decorations = fullscreen ? 0 : 1;
	X11->XChangeProperty(display, window, property, property, 32, PropModeReplace, (unsigned char*)&hints, 5);
	X11->XMapWindow(display, window);
	X11->XFlush(display);
#elif defined(__APPLE__)
	(void)fullscreen;
	EDOPRO_ToggleFullScreen();
#endif
}

void GUIUtils::ShowErrorWindow(epro::stringview context, epro::stringview message) {
#ifdef _WIN32
	MessageBox(nullptr, Utils::ToPathString(message).data(), Utils::ToPathString(context).data(), MB_OK | MB_ICONERROR);
#elif defined (__APPLE__)
	CFStringRef header_ref = CFStringCreateWithCString(nullptr, context.data(), context.size());
	CFStringRef message_ref = CFStringCreateWithCString(nullptr, message.data(), message.size());

	CFOptionFlags result;  //result code from the message box

	//launch the message box
	CFUserNotificationDisplayAlert(
		0, // no timeout
		kCFUserNotificationStopAlertLevel, //change it depending message_type flags ( MB_ICONASTERISK.... etc.)
		nullptr, //icon url, use default, you can change it depending message_type flags
		nullptr, //not used
		nullptr, //localization of strings
		header_ref, //header text 
		message_ref, //message text
		nullptr, //default "ok" text in button
		nullptr, //alternate button title
		nullptr, //other button title, null--> no other button
		&result //response flags
	);

	//Clean up the strings
	CFRelease(header_ref);
	CFRelease(message_ref);
#elif defined(__ANDROID__)
	porting::showErrorDialog(context, message);
#else
	auto pid = vfork();
	if(pid == 0) {
		execl("/usr/bin/kdialog", "kdialog", "--title", context.data(), "--error", message.data());
		execl("/usr/bin/zenity", "zenity", "--title", context.data(), "--error", message.data());
		_exit(EXIT_FAILURE);
	} else if(pid > 0)
		(void)waitpid(pid, nullptr, 0);
#endif
}

std::string GUIUtils::SerializeWindowPosition(irr::IrrlichtDevice* device) {
#ifdef _WIN32
	auto hWnd = GetWindowHandle(device->getVideoDriver());
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hWnd, &wp);
	return base64_encode<std::string>(reinterpret_cast<uint8_t*>(&wp), sizeof(wp));
#elif defined (__APPLE__)
	return EDOPRO_GetWindowRect(device->getVideoDriver()->getExposedVideoData().OpenGLOSX.Window);
#else
	return std::string{};
#endif
}

}

#if defined(__linux__) && ! defined(__ANDROID__)
#define LOAD_LIBRARY(name) do {\
name = (decltype(name))dlsym(LibX11, #name);\
if(!name){\
	dlclose(LibX11);\
	LibX11=nullptr;\
	return;\
}\
} while(0)

X11Helper::X11Helper() {
	LibX11 = dlopen("libX11.so", RTLD_LAZY); //libx11 has almost 2000 exported symbols
											 //there's no point in using RTDL_NOW
	if(!LibX11)
		LibX11 = dlopen("libX11.so.6", RTLD_LAZY);
	LOAD_LIBRARY(XOpenDisplay);
	LOAD_LIBRARY(XGetInputFocus);
	LOAD_LIBRARY(XInternAtom);
	LOAD_LIBRARY(XGetWindowProperty);
	LOAD_LIBRARY(XFree);
	LOAD_LIBRARY(XSendEvent);
	LOAD_LIBRARY(XChangeProperty);
	LOAD_LIBRARY(XMapWindow);
	LOAD_LIBRARY(XFlush);
}
#undef LOAD_LIBRARY

X11Helper::~X11Helper() {
	if(LibX11)
		dlclose(LibX11);
}
#endif
