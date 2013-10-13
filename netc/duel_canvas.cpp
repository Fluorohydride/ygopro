#include "image_mgr.h"
#include "duel_canvas.h"

namespace ygopro
{
	BEGIN_EVENT_TABLE(wxDuelCanvas, wxGLCanvas)
		EVT_SIZE(wxDuelCanvas::eventResized)
		EVT_PAINT(wxDuelCanvas::eventRender)
		EVT_MOTION(wxDuelCanvas::eventMouseMoved)
		EVT_MOUSEWHEEL(wxDuelCanvas::eventMouseWheelMoved)
		EVT_LEFT_DOWN(wxDuelCanvas::eventMouseDown)
		EVT_LEFT_UP(wxDuelCanvas::eventMouseReleased)
		EVT_LEAVE_WINDOW(wxDuelCanvas::eventMouseLeftWindow)
		EVT_KEY_DOWN(wxDuelCanvas::eventKeyPressed)
		EVT_KEY_UP(wxDuelCanvas::eventKeyReleased)
	END_EVENT_TABLE()

	wxDuelCanvas::wxDuelCanvas(wxFrame* parent, int id, int* args): wxGLCanvas(parent, id, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
		glcontext = new wxGLContext(this);
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        t_duelbg = &imageMgr.textures["duelng"];
        t_field = &imageMgr.textures["field"];
        t_avatar1 = &imageMgr.textures["avatar1"];
	}

	wxDuelCanvas::~wxDuelCanvas() {
		delete glcontext;
	}

	void wxDuelCanvas::eventResized(wxSizeEvent& evt) {
		glwidth = evt.GetSize().GetWidth();
		glheight = evt.GetSize().GetHeight();
		glViewport(0, 0, glwidth, glheight);
		Refresh();
	}

	void wxDuelCanvas::eventRender(wxPaintEvent& evt) {
		if(!IsShown())
			return;
		wxGLCanvas::SetCurrent(*glcontext);
		wxPaintDC(this);
		drawScene();
		SwapBuffers();
	}

	void wxDuelCanvas::eventMouseMoved(wxMouseEvent& evt){

	}

	void wxDuelCanvas::eventMouseWheelMoved(wxMouseEvent& evt){
		
	}

	void wxDuelCanvas::eventMouseDown(wxMouseEvent& evt){

	}

	void wxDuelCanvas::eventMouseReleased(wxMouseEvent& evt){

	}

	void wxDuelCanvas::eventMouseLeftWindow(wxMouseEvent& evt){

	}

	void wxDuelCanvas::eventKeyPressed(wxKeyEvent& evt){

	}

	void wxDuelCanvas::eventKeyReleased(wxKeyEvent& evt){

	}

}
