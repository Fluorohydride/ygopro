#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "wx/ribbon/bar.h"
#include "wx/ribbon/buttonbar.h"
#include "wx/ribbon/gallery.h"
#include "wx/ribbon/toolbar.h"
#include "deframe.h"

namespace ygopro
{

	DEFrame* deFrame = nullptr;

	BEGIN_EVENT_TABLE(DEFrame, wxFrame)
	END_EVENT_TABLE()

	DEFrame::DEFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
		wxRibbonBar* m_ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRIBBON_BAR_DEFAULT_STYLE | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS);
		wxRibbonPage* ribbon_page = new wxRibbonPage(m_ribbon, wxID_ANY, wxT("Menu"));

		wxRibbonPanel *deck_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Deck"));
		wxRibbonButtonBar *deck_bar = new wxRibbonButtonBar(deck_panel);
		deck_bar->AddButton(wxID_ANY, wxT("Load"), wxBitmap(48, 48));
		deck_bar->AddButton(wxID_ANY, wxT("Save"), wxBitmap(48, 48));
		deck_bar->AddButton(wxID_ANY, wxT("Save As"), wxBitmap(48, 48));
		wxRibbonPanel *tool_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Tools"));
		wxRibbonButtonBar *tool_bar = new wxRibbonButtonBar(tool_panel);
		tool_bar->AddButton(wxID_ANY, wxT("Clear"), wxBitmap(48, 48));
		tool_bar->AddButton(wxID_ANY, wxT("Sort"), wxBitmap(48, 48));
		tool_bar->AddButton(wxID_ANY, wxT("Shuffle"), wxBitmap(48, 48));
		tool_bar->AddButton(wxID_ANY, wxT("Screenshot"), wxBitmap(48, 48));
		wxRibbonPanel *filter_panel = new wxRibbonPanel(ribbon_page, wxID_ANY, wxT("Filter"));
		wxRibbonButtonBar *filter_bar = new wxRibbonButtonBar(filter_panel);
		filter_bar->AddButton(wxID_ANY, wxT("Filter"), wxBitmap(48, 48));
		filter_bar->AddDropdownButton(wxID_ANY, wxT("Ban lists"), wxBitmap(48, 48));
		m_ribbon->Realize();

		wxSizer* s1 = new wxBoxSizer(wxVERTICAL);
		wxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
		s1->Add(m_ribbon, 0, wxEXPAND);
		s1->Add(s2, 1, wxEXPAND);
		SetSizer(s1);
	}

	DEFrame::~DEFrame() {

	}

}
