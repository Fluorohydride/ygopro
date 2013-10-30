#include "editor_frame.h"
#include "editor_canvas.h"
#include "image_mgr.h"
#include <wx/wfstream.h>

namespace ygopro
{
    
    BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
        EVT_MENU(ID_DECK_LOAD, EditorFrame::OnDeckLoad)
        EVT_MENU(ID_DECK_SAVE, EditorFrame::OnDeckSave)
        EVT_MENU(ID_DECK_SAVEAS, EditorFrame::OnDeckSaveAs)
	END_EVENT_TABLE()

	EditorFrame* editorFrame = nullptr;

	EditorFrame::EditorFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
        m_auiManager.SetManagedWindow(this);
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        m_deck->Append(ID_DECK_LOAD, wxT("Load\tCTRL+D"));
        m_deck->Append(ID_DECK_SAVE, wxT("Save\tCTRL+S"));
        m_deck->Append(ID_DECK_SAVEAS, wxT("Save As\tCTRL+A"));
        
        wxMenu* m_tool = new wxMenu;
        m_tool->Append(ID_TOOL_CLEAR, wxT("&Clear\tCTRL+E"));
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
        
#ifdef _WIN32
        m_auiManager.AddPane(bmpCardImage, wxAuiPaneInfo().Name(wxT("Image")).Caption(wxT("Card Image")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(textCtrl, wxAuiPaneInfo().Name(wxT("Text")).Caption(wxT("Card Information")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(glcanvas, wxAuiPaneInfo().CenterPane());
#else
        m_auiManager.AddPane(textCtrl, wxAuiPaneInfo().Name(wxT("Text")).Caption(wxT("Card Information")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(bmpCardImage, wxAuiPaneInfo().Name(wxT("Image")).Caption(wxT("Card Image")).Left()
                             .CloseButton(false).Movable(false).Floatable(false));
        m_auiManager.AddPane(glcanvas, wxAuiPaneInfo().CenterPane());
#endif
        m_auiManager.Update();
        editorFrame = this;
	}

	EditorFrame::~EditorFrame() {
        m_auiManager.UnInit();
	}

	void EditorFrame::OnDeckLoad(wxCommandEvent& evt) {
        wxFileDialog fd(this, wxT("Choose"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if(fd.ShowModal() != wxID_OK)
            return;
        wxFileInputStream deck_file(fd.GetPath());
        if(!deck_file.IsOk())
            wxMessageDialog(this, wxT("Cannot load deck file."));
        current_file = fd.GetPath();
        SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
	}
    
    void EditorFrame::OnDeckSave(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnDeckSaveAs(wxCommandEvent& evt) {
        wxFileDialog fd(this, wxT("Choose"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        fd.ShowModal();
    }
    
}
