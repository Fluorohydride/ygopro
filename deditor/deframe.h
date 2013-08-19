#ifndef _DEFRAME_H_
#define _DEFRAME_H_

#include "wx/frame.h"

namespace ygopro
{

	class DEFrame : public wxFrame
	{
	public:
		DEFrame(int sx, int sy);
		~DEFrame();

		

	protected:
		DECLARE_EVENT_TABLE()
	};

	extern DEFrame* deFrame;
}

#endif //_GAME_H_
