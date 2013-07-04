#ifndef _DUEL_CANVAS_H_
#define _DUEL_CANVAS_H_

#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "frame_controler.h"

namespace ygopro
{
	class wxDuelCanvas : public wxGLCanvas, public FrameControler {

	private:
		wxGLContext* glcontext;

	public:
		wxDuelCanvas(wxFrame* parent, int id, int* args);
		virtual ~wxDuelCanvas();

		void drawScene();

		// events
		void eventResized(wxSizeEvent& evt);
		void eventRender(wxPaintEvent& evt);
		void eventMouseMoved(wxMouseEvent& evt);
		void eventMouseWheelMoved(wxMouseEvent& evt);
		void eventMouseDown(wxMouseEvent& evt);
		void eventMouseReleased(wxMouseEvent& evt);
		void eventMouseLeftWindow(wxMouseEvent& evt);
		void eventKeyPressed(wxKeyEvent& evt);
		void eventKeyReleased(wxKeyEvent& evt);

		DECLARE_EVENT_TABLE()
	};

}

#endif
