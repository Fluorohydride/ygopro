#include "utils_gui.h"
#include <IrrlichtDevice.h>
#include <ICursorControl.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include "text_types.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <vector>
#include "logging.h"
#elif defined(__linux__) && !defined(__ANDROID__)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#elif defined(__APPLE__)
#include "osx_menu.h"
#endif

namespace ygo {

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
		path_string filename(fmt::format(EPRO_TEXT("screenshots/ygopro_{:%Y-%m-%d %H-%M-%S}.png"), *std::localtime(&now)).c_str());
		if (!driver->writeImageToFile(image, filename.c_str()))
			device->getLogger()->log(L"Failed to take screenshot.", irr::ELL_WARNING);
		image->drop();
	}
}

void GUIUtils::ToggleFullscreen(irr::IrrlichtDevice* device, bool& fullscreen) {
#ifdef _WIN32
	static RECT nonFullscreenSize;
	static std::vector<RECT> monitors;
	static bool maximized = false;
	if(monitors.empty()) {
		EnumDisplayMonitors(0, 0, [](HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData) -> BOOL {
			auto monitors = reinterpret_cast<std::vector<RECT>*>(pData);
			monitors->push_back(*lprcMonitor);
			return TRUE;
		}, (LPARAM)&monitors);
	}
	fullscreen = !fullscreen;
	const auto driver = device->getVideoDriver();
	HWND hWnd = reinterpret_cast<HWND>(driver->getExposedVideoData().D3D9.HWnd);
	LONG_PTR style = WS_POPUP;
	RECT clientSize = {};
	if(fullscreen) {
		if(GetWindowLong(hWnd, GWL_STYLE) & WS_MAXIMIZE) {
			maximized = true;
		}
		GetWindowRect(hWnd, &nonFullscreenSize);
		style = WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		for(auto& rect : monitors) {
			POINT windowCenter = { (nonFullscreenSize.left + (nonFullscreenSize.right - nonFullscreenSize.left) / 2), (nonFullscreenSize.top + (nonFullscreenSize.bottom - nonFullscreenSize.top) / 2) };
			if(PtInRect(&rect, windowCenter)) {
				clientSize = rect;
				break;
			}
		}
	} else {
		style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		clientSize = nonFullscreenSize;
		if(maximized) {
			style |= WS_MAXIMIZE;
			maximized = false;
		}
	}

	if(!SetWindowLongPtr(hWnd, GWL_STYLE, style))
		ErrorLog("Could not change window style.");

	const auto width = clientSize.right - clientSize.left;
	const auto height = clientSize.bottom - clientSize.top;

	SetWindowPos(hWnd, HWND_TOP, clientSize.left, clientSize.top, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
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
