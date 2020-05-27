#include "utils_gui.h"
#include <irrlicht.h>
#include <ICursorControl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include "game_config.h"
#include "text_types.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include "logging.h"
#elif defined(__ANDROID__)
#include "Android/COSAndroidOperator.h"
class android_app;
namespace porting {
extern android_app* app_global;
}
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#elif defined(__APPLE__)
#include "osx_menu.h"
#endif

namespace ygo {

irr::IrrlichtDevice* GUIUtils::CreateDevice(GameConfig* configs) {
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = configs->antialias;
#ifndef __ANDROID__
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if(configs->use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
#endif
		params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<irr::u32>(1024 * configs->dpi_scale, 640 * configs->dpi_scale);
	params.Vsync = configs->vsync;
#else
	if(gGameConfig->use_d3d) {
		params.DriverType = irr::video::EDT_OGLES1;
	} else {
		params.DriverType = irr::video::EDT_OGLES2;
	}
	params.PrivateData = porting::app_global;
	params.Bits = 24;
	params.ZBufferBits = 16;
	params.AntiAlias = 0;
	params.WindowSize = irr::core::dimension2du(0, 0);
#endif
	irr::IrrlichtDevice* device = irr::createDeviceEx(params);
	if(!device) {
		throw std::runtime_error("Failed to create Irrlicht Engine device!");
	}
	auto driver = device->getVideoDriver();
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
	irr::IOSOperator* Operator = new irr::COSAndroidOperator();
	device->getGUIEnvironment()->setOSOperator(Operator);
	Operator->drop();
	if(driver->queryFeature(irr::video::EVDF_TEXTURE_NPOT)) {
		driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	} else {
		driver->setTextureCreationFlag(irr::video::ETCF_ALLOW_NON_POWER_2, true);
		driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	}
#else
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
#endif
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	device->setWindowCaption(L"Project Ignis: EDOPro");
	device->setResizable(true);
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON hBigIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	HWND hWnd = reinterpret_cast<HWND>(driver->getExposedVideoData().D3D9.HWnd);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (long)hSmallIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (long)hBigIcon);
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

void GUIUtils::TakeScreenshot(irr::IrrlichtDevice* device)
{
	const auto driver = device->getVideoDriver();
	const auto image = driver->createScreenShot();
	if (image) { // In theory this should never be NULL
		auto now = std::time(nullptr);
		path_string filename(fmt::format(EPRO_TEXT("screenshots/EDOPro {:%Y-%m-%d %H-%M-%S}.png"), *std::localtime(&now)).c_str());
		if (!driver->writeImageToFile(image, filename.c_str()))
			device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);
		image->drop();
	}
}

void GUIUtils::ToggleFullscreen(irr::IrrlichtDevice* device, bool& fullscreen) {
#ifdef _WIN32
	static WINDOWPLACEMENT nonFullscreenSize;
	static LONG nonFullscreenStyle;
	static constexpr LONG_PTR fullscreenStyle = WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	static const std::vector<RECT> monitors = []() {
		std::vector<RECT> ret;
		EnumDisplayMonitors(0, 0, [](HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) -> BOOL {
			auto monitors = reinterpret_cast<std::vector<RECT>*>(pData);
			monitors->push_back(*lprcMonitor);
			return TRUE;
		}, (LPARAM)&ret);
		return ret;
	}();
	fullscreen = !fullscreen;
	const auto driver = device->getVideoDriver();
	HWND hWnd = reinterpret_cast<HWND>(driver->getExposedVideoData().D3D9.HWnd);
	RECT clientSize = {};
	if(fullscreen) {
		GetWindowPlacement(hWnd, &nonFullscreenSize);
		nonFullscreenStyle = GetWindowLong(hWnd, GWL_STYLE);
		static RECT curSize;
		GetWindowRect(hWnd, &curSize);
		for(const auto& rect : monitors) {
			POINT windowCenter = { (curSize.left + (curSize.right - curSize.left) / 2), (curSize.top + (curSize.bottom - curSize.top) / 2) };
			if(PtInRect(&rect, windowCenter)) {
				clientSize = rect;
				break;
			}
		}
		if(!SetWindowLongPtr(hWnd, GWL_STYLE, fullscreenStyle))
			ErrorLog("Could not change window style.");

		const auto width = clientSize.right - clientSize.left;
		const auto height = clientSize.bottom - clientSize.top;

		SetWindowPos(hWnd, HWND_TOP, clientSize.left, clientSize.top, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	} else {
		SetWindowPlacement(hWnd, &nonFullscreenSize);
		SetWindowLongPtr(hWnd, GWL_STYLE, nonFullscreenStyle);
	}
#elif defined(__linux__) && !defined(__ANDROID__)
	struct {
		unsigned long   flags;
		unsigned long   functions;
		unsigned long   decorations;
		long			inputMode;
		unsigned long   status;
	} hints = {};
	Display* display = XOpenDisplay(NULL);;
	Window window;
	static bool wasHorizontalMaximized = false, wasVerticalMaximized = false;
	Window child;
	int revert;
	fullscreen = !fullscreen;
	XGetInputFocus(display, &window, &revert);

	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", false);
	Atom max_horz = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
	Atom max_vert = XInternAtom(display, "_NET_WM_STATE_MAXIMIZED_VERT", false);

	auto checkMaximized = [&]() {
		long maxLength = 1024;
		Atom actualType;
		int actualFormat;
		unsigned long i, numItems, bytesAfter;
		unsigned char *propertyValue = NULL;
		if(XGetWindowProperty(display, window, wm_state,
							  0l, maxLength, false, XA_ATOM, &actualType,
							  &actualFormat, &numItems, &bytesAfter,
							  &propertyValue) == Success) {
			Atom* atoms = (Atom *)propertyValue;
			for(i = 0; i < numItems; ++i) {
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
#elif defined(__APPLE__)
	EDOPRO_ToggleFullScreen();
#endif
}

}
