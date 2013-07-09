#include "game_window.h"
#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "duel_canvas.h"

namespace ygopro
{

	GameFrame* mainFrame = nullptr;
	Random globalRandom;
	CommonConfig commonCfg;

	BEGIN_EVENT_TABLE(GameFrame, wxFrame)
	END_EVENT_TABLE()

	GameFrame::GameFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro", wxDefaultPosition, wxSize(sx, sy)) {
		wxRibbonBar* m_ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS);
		wxRibbonPage* ribbon_page = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Menu"));

		wxRibbonPanel *config_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Config"), wxBitmap(32, 32));
		wxRibbonButtonBar *config_bar = new wxRibbonButtonBar(config_panel);
		config_bar->AddButton(wxID_ANY, wxT("Edit Config"), wxBitmap(32, 32));
		config_bar->AddButton(wxID_ANY, wxT("Edit Show"), wxBitmap(32, 32));
		config_bar->AddButton(wxID_ANY, wxT("Edit Timing"), wxBitmap(32, 32));
		wxRibbonPanel *replay_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Replay"), wxBitmap(32, 32));
		wxRibbonButtonBar *replay_bar = new wxRibbonButtonBar(replay_panel);
		replay_bar->AddButton(wxID_ANY, wxT("Play"), wxBitmap(32, 32), wxBitmap(16, 16));
		replay_bar->AddButton(wxID_ANY, wxT("Next Step"), wxBitmap(32, 32), wxBitmap(16, 16));
		replay_bar->AddButton(wxID_ANY, wxT("Pause"), wxBitmap(32, 32), wxBitmap(16, 16));
		replay_bar->AddButton(wxID_ANY, wxT("Resume"), wxBitmap(32, 32), wxBitmap(16, 16));
		wxRibbonPanel *tool_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Tools"), wxBitmap(32, 32));
		wxRibbonButtonBar *tool_bar = new wxRibbonButtonBar(tool_panel);
		tool_bar->AddButton(wxID_ANY, wxT("Screenshot"), wxBitmap(32, 32), wxBitmap(16, 16));
		m_ribbon->Realize();
		int wx_gl_attribs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
		wxDuelCanvas* glcanvas = new wxDuelCanvas(this, wxID_ANY, wx_gl_attribs);
		wxSizer *s = new wxBoxSizer(wxVERTICAL);
		s->Add(m_ribbon, 0, wxEXPAND);
		s->Add(glcanvas, 1, wxEXPAND);
		SetSizer(s);
	}

	GameFrame::~GameFrame() {

	}

	void GameFrame::HandleMessage(unsigned int msg, unsigned int size, void* data) {

	}

	

}
