#include "utils_gui.h"
#include <irrlicht.h>
#include <ICursorControl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include "config.h"
#include "utils.h"
#include "game_config.h"
#include "text_types.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include "logging.h"
#include "Base64.h"
#if !(IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
#include "IrrlichtCommonIncludes/CCursorControl.h"
using CCursorControl = irr::CCursorControl;
#endif
#elif defined(__ANDROID__)
#include "Android/porting_android.h"
#elif defined(EDOPRO_MACOS)
#import <CoreFoundation/CoreFoundation.h>
#include "osx_menu.h"
#elif defined(__linux__)
#if !(IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif
#include <unistd.h>
#include <sys/wait.h>
#include <memory>

static inline bool try_guess_wayland() {
	const char* env = getenv("XDG_SESSION_TYPE");
	return env == nullptr || env != "x11"_sv;
}
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

static inline irr::video::E_DRIVER_TYPE getDefaultDriver(irr::E_DEVICE_TYPE device_type) {
	(void)device_type;
#if defined(__ANDROID__)
	return irr::video::EDT_OGLES2;
#elif defined(__linux__) && (IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	if(device_type == irr::E_DEVICE_TYPE::EIDT_WAYLAND)
		return irr::video::EDT_OGLES2;
	return irr::video::EDT_OPENGL;
#elif defined(_WIN32) && defined(IRR_COMPILE_WITH_DX9_DEV_PACK)
	return irr::video::EDT_DIRECT3D9;
#else
	return irr::video::EDT_OPENGL;
#endif
}

irr::IrrlichtDevice* GUIUtils::CreateDevice(GameConfig* configs) {
	irr::SIrrlichtCreationParameters params{};
	params.AntiAlias = configs->antialias;
#if defined(__linux__) && !defined(__ANDROID__) && (IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	if(configs->useWayland == 2) {
		if(!try_guess_wayland())
			configs->useWayland = 0;
	} else if(configs->useWayland == 1 && try_guess_wayland()) {
		params.DeviceType = irr::E_DEVICE_TYPE::EIDT_WAYLAND;
		fmt::print("You're using the wayland device backend.\n"
				   "Keep in mind that it's still experimental and might be unstable.\n"
				   "If you are getting any major issues, or the game doesn't start,\n"
				   "you can manually disable this option from the system.conf file by toggling the useWayland option.\n"
				   "Feel free to report any issues you encounter.\n");
	}
	// This correspond to the program's class name, used by window managers and
	// desktop environments to group multiple instances with their desktop file
	char class_name[] = "EDOPro";
	params.PrivateData = class_name;
#endif
	params.Vsync = configs->vsync;
	if(configs->driver_type == irr::video::EDT_COUNT)
		params.DriverType = getDefaultDriver(params.DeviceType);
	else
		params.DriverType = configs->driver_type;
#if (IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9)
	params.OGLES2ShaderPath = EPRO_TEXT("BUNDLED");
	params.WindowResizable = true;
#if defined(EDOPRO_MACOS)
	params.UseIntegratedGPU = configs->useIntegratedGpu > 0;
#endif
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
		if(winstruct.size() == sizeof(WINDOWPLACEMENT)) {
			WINDOWPLACEMENT wp;
			memcpy(&wp, winstruct.data(), sizeof(WINDOWPLACEMENT));
			if(wp.length == sizeof(WINDOWPLACEMENT))
				SetWindowPlacement(hWnd, &wp);
		}
	}
#elif defined(EDOPRO_MACOS)
	if(gGameConfig->windowStruct.size())
		EDOPRO_SetWindowRect(driver->getExposedVideoData().OpenGLOSX.Window, gGameConfig->windowStruct.data());
#endif
	device->getLogger()->setLogLevel(irr::ELL_ERROR);
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
	(void)fullscreen;
#ifdef EDOPRO_MACOS
	EDOPRO_ToggleFullScreen();
#elif defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
#if IRRLICHT_VERSION_MAJOR==1 && IRRLICHT_VERSION_MINOR==9
	device->toggleFullscreen(!std::exchange(fullscreen, !fullscreen));
#else
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
#else
	// If irrlicht 1.8 is being used, x11 is always hard linked
	struct {
		unsigned long   flags;
		unsigned long   functions;
		unsigned long   decorations;
		long			inputMode;
		unsigned long   status;
	} hints{};
	auto display = XOpenDisplay(nullptr);
	Window window;
	static bool wasHorizontalMaximized = false, wasVerticalMaximized = false;
	int revert;
	fullscreen = !fullscreen;
	XGetInputFocus(display, &window, &revert);

	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", false);
	Atom max_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
	Atom max_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);

	auto checkMaximized = [&]() {
		const long maxLength = 1024;
		Atom actualType;
		int actualFormat;
		unsigned long numItems, bytesAfter;
		unsigned char* propertyValue = nullptr;
		wasVerticalMaximized = false;
		wasHorizontalMaximized = false;
		if(XGetWindowProperty(display, window, wm_state,
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
			XFree(propertyValue);
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
		XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);
	}

	Atom property = XInternAtom(display, "_MOTIF_WM_HINTS", true);
	hints.flags = 2;
	hints.decorations = fullscreen ? 0 : 1;
	XChangeProperty(display, window, property, property, 32, PropModeReplace, (unsigned char*)&hints, 5);
	XMapWindow(display, window);
	XFlush(display);
#endif
#endif
#endif //EDOPRO_MACOS
}

void GUIUtils::ShowErrorWindow(epro::stringview context, epro::stringview message) {
#ifdef _WIN32
	MessageBox(nullptr, Utils::ToPathString(message).data(), Utils::ToPathString(context).data(), MB_OK | MB_ICONERROR);
#elif defined (EDOPRO_MACOS)
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
#elif defined(__linux__)
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
#elif defined (EDOPRO_MACOS)
	return EDOPRO_GetWindowRect(device->getVideoDriver()->getExposedVideoData().OpenGLOSX.Window);
#else
	return std::string{};
#endif
}

}
