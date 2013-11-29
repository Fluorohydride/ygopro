#include "editor_frame.h"
#include "editor_canvas.h"
#include "game_frame.h"
#include "image_mgr.h"
#include "deck_data.h"
#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/richtext/richtextctrl.h>
#include <sys/time.h>

namespace ygopro
{

	EditorFrame* editorFrame = nullptr;

	EditorFrame::EditorFrame(int sx, int sy): wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
        m_auiManager.SetManagedWindow(this);
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        m_deck->Append(ID_DECK_LOAD, wxT("Load\tCTRL+D"));
        m_deck->Append(ID_DECK_SAVE, wxT("Save\tCTRL+S"));
        m_deck->Append(ID_DECK_SAVEAS, wxT("Save As\tCTRL+SHIFT+S"));
        m_deck->Append(ID_DECK_LOADSTR, wxT("Load From String\tCTRL+V"));
        m_deck->Append(ID_DECK_SAVESTR, wxT("Save To String\tCTRL+C"));
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckLoad, this, ID_DECK_LOAD);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSave, this, ID_DECK_SAVE);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSaveAs, this, ID_DECK_SAVEAS);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckLoadString, this, ID_DECK_LOADSTR);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSaveString, this, ID_DECK_SAVESTR);
        
        wxMenu* m_tool = new wxMenu;
        m_tool->Append(ID_TOOL_CLEAR, wxT("&Clear\tCTRL+E"));
        m_tool->Append(ID_TOOL_SORT, wxT("&Sort\tCTRL+R"));
        m_tool->Append(ID_TOOL_SHUFFLE, wxT("&Shuffle\tCTRL+T"));
        m_tool->Append(ID_TOOL_SCREENSHOT, wxT("&Screenshot\tCTRL+P"));
        m_tool->Append(ID_TOOL_SEARCH, wxT("&Search\tCTRL+F"));
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolClear, this, ID_TOOL_CLEAR);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolSort, this, ID_TOOL_SORT);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolShuffle, this, ID_TOOL_SHUFFLE);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolScreenshot, this, ID_TOOL_SCREENSHOT);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolSearch, this, ID_TOOL_SEARCH);
        
        wxMenu* m_limit = new wxMenu;
        auto& lrs = limitRegulationMgr.GetLimitRegulations();
        for(unsigned int i = 0; i < lrs.size(); ++i) {
            m_limit->Append(ID_REGULATION + i, lrs[i].name, wxT("Check to choose this regulation."), true);
            m_limit->Bind(wxEVT_MENU, &EditorFrame::OnRegulationChange, this, ID_REGULATION + i);
        }
        if(lrs.size() > 0)
            m_limit->GetMenuItems()[0]->Check(true);
        
        menu_bar->Append(m_deck, wxT("Deck"));
        menu_bar->Append(m_tool, wxT("Tools"));
        menu_bar->Append(m_limit, wxT("Limit Regulation"));
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
            limitRegulationMgr.GetDeckCardLimitCount(editor_canvas->getDeck());
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
        wxFileDialog fd(this, wxT("Save deck file as"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if(fd.ShowModal() != wxID_OK)
            return;
        editor_canvas->getDeck().SaveToFile(fd.GetPath());
        current_file = fd.GetPath();
        SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
    }
    
    void EditorFrame::OnDeckLoadString(wxCommandEvent& evt) {
        
    }
    
    void EditorFrame::OnDeckSaveString(wxCommandEvent& evt) {
        wxString deck_string = editor_canvas->getDeck().SaveToString();
        wxTheClipboard->Open();
        wxTheClipboard->SetData(new wxTextDataObject(deck_string));
        wxTheClipboard->Close();
        wxMessageDialog(this, wxString("The deck has been successfully copied to clipboard."), wxT("Information"), wxICON_INFORMATION).ShowModal();
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
        time_t t = time(0);
        wxString path = static_cast<const std::string&>(commonCfg["screenshot_path"]);
        if(*path.rbegin() != wxT('/'))
            path.Append(wxT('/'));
        if(!wxFileName::DirExists(path) && !wxFileName::Mkdir(path)) {
            wxMessageDialog(this, wxString("Cannot create folder ") + path, wxT("Error"), wxICON_ERROR).ShowModal();
            return;
        }
        path += wxString::Format(wxT("%d.png"), t);
        editor_canvas->saveScreenshot(path);
        wxMessageDialog(this, wxString("File has been successful saved to:\n") + path, wxT("Information"), wxICON_INFORMATION).ShowModal();
    }
    
    void EditorFrame::OnToolSearch(wxCommandEvent& evt) {
    }
    
    void EditorFrame::OnRegulationChange(wxCommandEvent& evt) {
        auto& regs = GetMenuBar()->GetMenu(2)->GetMenuItems();
        if(regs.GetCount() == 0)
            return;
        bool change = true;
        int id = evt.GetId() - ID_REGULATION;
        for(size_t i = 0; i < regs.GetCount(); ++i) {
            auto mi = regs[i];
            if(mi->IsCheck() && id == i)
                change = false;
            mi->Check(id == i);
        }
    }
}
