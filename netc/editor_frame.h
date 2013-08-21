#ifndef _DEFRAME_H_
#define _DEFRAME_H_

#include <gl/glew.h>
#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/glcanvas.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"

namespace ygopro
{

	class EditorFrame : public wxFrame
	{

		enum
		{
			ID_RIBBON_BANLIST = wxID_HIGHEST + 1,
		};

	public:
		EditorFrame(int sx, int sy);
		~EditorFrame();

	void OnBanListDropdown(wxRibbonButtonBarEvent& evt);

	protected:
		DECLARE_EVENT_TABLE()
	};

	extern EditorFrame* editorFrame;
}

#endif //_GAME_H_
