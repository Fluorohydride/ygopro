#include "editor_frame.h"
#include "editor_canvas.h"
#include "image_mgr.h"

namespace ygopro
{

	EditorFrame* editorFrame = nullptr;

	EditorFrame::EditorFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
        m_auiManager.SetManagedWindow(this);
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        m_deck->Append(ID_DECK_LOAD, wxT("Load\tCTRL+D"));
        m_deck->Append(ID_DECK_SAVE, wxT("Save\tCTRL+S"));
        m_deck->Append(ID_DECK_SAVEAS, wxT("Save As\tCTRL+A"));
        Connect(ID_DECK_LOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrame::OnDeckLoad));
        Connect(ID_DECK_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrame::OnDeckSave));
        Connect(ID_DECK_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorFrame::OnDeckSaveAs));
        
        wxMenu* m_tool = new wxMenu;
        m_tool->Append(ID_TOOL_CLEAR, wxT("&Clear\tCTRL+E"))->SetCheckable(true);
        m_tool->Append(ID_TOOL_SORT, wxT("&Sort\tCTRL+R"));
        m_tool->Append(ID_TOOL_SHUFFLE, wxT("&Shuffle\tCTRL+T"));
        m_tool->Append(ID_TOOL_SCREENSHOT, wxT("&Screenshot\tCTRL+P"));
        m_tool->Append(ID_TOOL_SEARCH, wxT("&Search\tCTRL+F"));
        
        menu_bar->Append(m_deck, wxT("Deck"));
        menu_bar->Append(m_tool, wxT("Tools"));
        SetMenuBar(menu_bar);

        wxStaticBitmap* bmpCardImage = new wxStaticBitmap(this, wxID_ANY, wxBitmap(177, 254));
		wxTextCtrl* textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                              wxTE_MULTILINE |	wxTE_LEFT | wxTE_BESTWRAP);
        int wx_gl_attribs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
        wxEditorCanvas* glcanvas = new wxEditorCanvas(this, wxID_ANY, wx_gl_attribs);
        
        m_auiManager.AddPane(textCtrl, wxAuiPaneInfo().Name(wxT("Text")).Caption(wxT("Card Information")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(bmpCardImage, wxAuiPaneInfo().Name(wxT("Image")).Caption(wxT("Card Image")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(glcanvas, wxAuiPaneInfo().CenterPane());

        m_auiManager.Update();
	}

	EditorFrame::~EditorFrame() {
        m_auiManager.UnInit();
	}

	void EditorFrame::OnDeckLoad(wxCommandEvent& evt) {
        
	}
    
    void EditorFrame::OnDeckSave(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnDeckSaveAs(wxCommandEvent& evt) {
    }
    
}
