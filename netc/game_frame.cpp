#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "duel_canvas.h"
#include "game_frame.h"

namespace ygopro
{

	GameFrame* mainFrame = nullptr;
	Random globalRandom;
	CommonConfig commonCfg;

	BEGIN_EVENT_TABLE(GameFrame, wxFrame)
	END_EVENT_TABLE()

	GameFrame::GameFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro", wxDefaultPosition, wxSize(sx, sy)) {

		int wx_gl_attribs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
 		wxDuelCanvas* glcanvas = new wxDuelCanvas(this, wxID_ANY, wx_gl_attribs);
		wxSizer *s = new wxBoxSizer(wxVERTICAL);
		s->Add(glcanvas, 1, wxEXPAND);
		SetSizer(s);
	}

	GameFrame::~GameFrame() {

	}

	void GameFrame::HandleMessage(unsigned int msg, unsigned int size, void* data) {

	}

	

}
