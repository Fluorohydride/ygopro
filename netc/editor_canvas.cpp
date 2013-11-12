#include "image_mgr.h"
#include "editor_canvas.h"
#include "card_data.h"
#include "deck_data.h"
#include "image_mgr.h"

namespace ygopro
{
	BEGIN_EVENT_TABLE(wxEditorCanvas, wxGLCanvas)
		EVT_SIZE(wxEditorCanvas::eventResized)
		EVT_PAINT(wxEditorCanvas::eventRender)
		EVT_MOTION(wxEditorCanvas::eventMouseMoved)
		EVT_MOUSEWHEEL(wxEditorCanvas::eventMouseWheelMoved)
		EVT_LEFT_DOWN(wxEditorCanvas::eventMouseDown)
		EVT_LEFT_UP(wxEditorCanvas::eventMouseReleased)
		EVT_LEAVE_WINDOW(wxEditorCanvas::eventMouseLeftWindow)
	END_EVENT_TABLE()

	wxEditorCanvas::wxEditorCanvas(wxFrame* parent, int id, int* args): wxGLCanvas(parent, id, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
        glcontext = new wxGLContext(this);
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        t_buildbg = &imageMgr.textures["buildbg"];
	}

	wxEditorCanvas::~wxEditorCanvas() {
		delete glcontext;
	}

	void wxEditorCanvas::eventResized(wxSizeEvent& evt) {
		glwidth = evt.GetSize().GetWidth();
		glheight = evt.GetSize().GetHeight();
		glViewport(0, 0, glwidth, glheight);
		Refresh();
	}

	void wxEditorCanvas::eventRender(wxPaintEvent& evt) {
		if(!IsShown())
			return;
		wxGLCanvas::SetCurrent(*glcontext);
		wxPaintDC(this);
        imageMgr.InitTextures();
		drawScene();
		SwapBuffers();
	}

	void wxEditorCanvas::eventMouseMoved(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseWheelMoved(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseDown(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseReleased(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseLeftWindow(wxMouseEvent& evt){

	}

    void wxEditorCanvas::setDeck(DeckData* deck) {
        for(auto pcard : deck->main_deck) {
            EditorCardData cd;
            cd.cd = pcard;
            cd.ti = &imageMgr.GetCardTexture(cd.cd->code);
            main_deck.push_back(cd);
        }
        for(auto pcard : deck->extra_deck) {
            EditorCardData cd;
            cd.cd = pcard;
            cd.ti = &imageMgr.GetCardTexture(cd.cd->code);
            extra_deck.push_back(cd);
        }
        for(auto pcard : deck->side_deck) {
            EditorCardData cd;
            cd.cd = pcard;
            cd.ti = &imageMgr.GetCardTexture(cd.cd->code);
            side_deck.push_back(cd);
        }
    }
    
	void wxEditorCanvas::drawScene() {

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glShadeModel(GL_SMOOTH);

		//draw background
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, t_buildbg->tex());
		glBegin(GL_QUADS);
		{
			glTexCoord2f(t_buildbg->lx, t_buildbg->ly);glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(t_buildbg->lx, t_buildbg->ry);glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(t_buildbg->rx, t_buildbg->ry);glVertex2f(1.0f, -1.0f);
			glTexCoord2f(t_buildbg->rx, t_buildbg->ly);glVertex2f(1.0f, 1.0f);
		}
		glEnd();
		glFlush();
	}

}
