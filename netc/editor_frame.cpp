#include "editor_frame.h"
#include "editor_canvas.h"
#include "image_mgr.h"
#include <wx/richtext/richtextctrl.h>

namespace ygopro
{
    
    BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
        EVT_MENU(ID_DECK_LOAD, EditorFrame::OnDeckLoad)
        EVT_MENU(ID_DECK_SAVE, EditorFrame::OnDeckSave)
        EVT_MENU(ID_DECK_SAVEAS, EditorFrame::OnDeckSaveAs)
        EVT_MENU(ID_DECK_LOADSTR, EditorFrame::OnDeckLoadString)
        EVT_MENU(ID_DECK_SAVESTR, EditorFrame::OnDeckSaveString)
        EVT_MENU(ID_TOOL_CLEAR, EditorFrame::OnToolClear)
        EVT_MENU(ID_TOOL_SORT, EditorFrame::OnToolSort)
        EVT_MENU(ID_TOOL_SHUFFLE, EditorFrame::OnToolShuffle)
        EVT_MENU(ID_TOOL_SCREENSHOT, EditorFrame::OnToolScreenshot)
        EVT_MENU(ID_TOOL_SEARCH, EditorFrame::OnToolSearch)
	END_EVENT_TABLE()

	EditorFrame* editorFrame = nullptr;

	EditorFrame::EditorFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
        m_auiManager.SetManagedWindow(this);
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        m_deck->Append(ID_DECK_LOAD, wxT("Load\tCTRL+D"));
        m_deck->Append(ID_DECK_SAVE, wxT("Save\tCTRL+S"));
        m_deck->Append(ID_DECK_SAVEAS, wxT("Save As\tCTRL+A"));
        m_deck->Append(ID_DECK_LOADSTR, wxT("Load From String\tCTRL+SHIFT+D"));
        m_deck->Append(ID_DECK_SAVESTR, wxT("Save To String\tCTRL+SHIFT+S"));
        
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
		wxRichTextCtrl* textCtrl = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                              wxTE_MULTILINE |	wxTE_LEFT | wxTE_BESTWRAP | wxTE_READONLY);
        int wx_gl_attribs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
        editor_canvas= new wxEditorCanvas(this, wxID_ANY, wx_gl_attribs);
        
        wxPanel* infoPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(200, 600));
        wxBoxSizer *sz=new wxBoxSizer(wxVERTICAL);
        sz->AddSpacer(5);
        sz->Add(bmpCardImage, 0, wxALIGN_CENTER_HORIZONTAL);
        sz->AddSpacer(5);
        sz->Add(textCtrl, 1, wxEXPAND | wxALL);
        infoPanel->SetSizer(sz);
        
        m_auiManager.AddPane(infoPanel, wxAuiPaneInfo().Name(wxT("Text")).Caption(wxT("Card Information")).Left()
                             .CloseButton(false).Movable(false).Floatable(false).CaptionVisible(false));
        m_auiManager.AddPane(editor_canvas, wxAuiPaneInfo().CenterPane());
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
        if(editor_canvas->getDeck().LoadFromFile(fd.GetPath())) {
            current_file = fd.GetPath();
            SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
            deckMgr.GetDeckCardLimitCount(editor_canvas->getDeck());
            editor_canvas->Refresh();
        } else {
            wxMessageDialog(this, wxT("Cannot load deck file."));
        }
	}
    
    void EditorFrame::OnDeckSave(wxCommandEvent& evt) {
        if(current_file.IsEmpty())
            OnDeckSaveAs(evt);
        else
            editor_canvas->getDeck().SaveToFile(current_file);
    }
    
    void EditorFrame::OnDeckSaveAs(wxCommandEvent& evt) {
        wxFileDialog fd(this, wxT("Choose"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if(fd.ShowModal() != wxID_OK)
            return;
        editor_canvas->getDeck().SaveToFile(fd.GetPath());
        current_file = fd.GetPath();
        SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
    }
    
    void EditorFrame::OnDeckLoadString(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnDeckSaveString(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnToolClear(wxCommandEvent& evt) {
        editor_canvas->getDeck().Clear();
        editor_canvas->Refresh();
    }
    
    void EditorFrame::OnToolSort(wxCommandEvent& evt) {
        editor_canvas->getDeck().Sort();
        editor_canvas->Refresh();
    }
    
    void EditorFrame::OnToolShuffle(wxCommandEvent& evt) {
        editor_canvas->getDeck().Shuffle();
        editor_canvas->Refresh();
    }
    
    void EditorFrame::OnToolScreenshot(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnToolSearch(wxCommandEvent& evt) {
    }
    
}
