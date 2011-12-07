#include "g_ime.h"

namespace ygo {

#ifdef _WIN32

HIMC G_IME::hIMC;
HWND G_IME::hWindow;
HHOOK G_IME::hHook;
	
void G_IME::Init(irr::video::SExposedVideoData& vdata) {
	hWindow = (HWND)vdata.OpenGLWin32.HWnd;
	hHook = SetWindowsHookEx(WH_GETMESSAGE, CharHookProc, NULL, GetCurrentThreadId());
	hIMC = ImmGetContext(hWindow);
	ImmAssociateContext(hWindow, hIMC);
}
LRESULT CALLBACK G_IME::CharHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if(nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	if(!ImmIsIME(GetKeyboardLayout(0)))
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	MSG* msg = (MSG*)lParam;
	switch(msg->message) {
	case WM_IME_CHAR: {
		break;
	}
/*	case WM_CHAR: {
		switch(msg->wParam) {
		case VK_RETURN:
			break;
		default: {
			input_count ++;
			if(msg->wParam > 128) {
				if(input_count == 1)
					input_buffer[1] = msg->wParam;
				else if(input_count == 2) {
					input_buffer[0] = msg->wParam;
					input_count = 0;
				}
			} else {
				input_count = 0;
			}
			return true;
		}
		break;
		}
	}*/
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
#else

void G_IME::Init(irr::video::SExposedVideoData&) {

}

#endif

}
