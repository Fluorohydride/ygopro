#ifndef G_IME_H
#define G_IME_H

#include "config.h"

namespace ygo {

#ifdef _WIN32

class G_IME{
public:
	void Init(irr::video::SExposedVideoData&);
	static LRESULT CALLBACK CharHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	
private:
	static HIMC hIMC;
	static HWND hWindow;
	static HHOOK hHook;
};

#else

class G_IME{
	void Init(irr::video::SExposedVideoData&);
};

#endif

}

#endif //G_IME_H
