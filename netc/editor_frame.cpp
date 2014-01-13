#include <wx/filename.h>
#include <wx/clipbrd.h>
#include <wx/utils.h>
#include "editor_frame.h"
#include "editor_canvas.h"
#include "game_frame.h"
#include "filter_frame.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"

namespace ygopro
{

    EditorFrame* editorFrame = nullptr;

    EditorFrame::EditorFrame(int sx, int sy) : wxFrame(nullptr, wxID_ANY, "YGOpro Deck Editor", wxDefaultPosition, wxSize(sx, sy)) {
        
        wxMenuBar* menu_bar = new wxMenuBar;
        wxMenu* m_deck = new wxMenu();
        m_deck->Append(ID_DECK_LOAD, wxT("Load\tCTRL+D"));
        m_deck->Append(ID_DECK_SAVE, wxT("Save\tCTRL+S"));
        m_deck->Append(ID_DECK_SAVEAS, wxT("Save As\tCTRL+SHIFT+S"));
        m_deck->Append(ID_DECK_LOADSTR, wxT("Load From String\tCTRL+SHIFT+V"));
        m_deck->Append(ID_DECK_SAVESTR, wxT("Save To String\tCTRL+SHIFT+C"));
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckLoad, this, ID_DECK_LOAD);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSave, this, ID_DECK_SAVE);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSaveAs, this, ID_DECK_SAVEAS);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckLoadString, this, ID_DECK_LOADSTR);
        m_deck->Bind(wxEVT_MENU, &EditorFrame::OnDeckSaveString, this, ID_DECK_SAVESTR);
        
        wxMenu* m_tool = new wxMenu;
        m_tool->Append(ID_TOOL_CLEAR, wxT("Clear\tCTRL+E"));
        m_tool->Append(ID_TOOL_SORT, wxT("Sort\tCTRL+R"));
        m_tool->Append(ID_TOOL_SHUFFLE, wxT("Shuffle\tCTRL+T"));
        m_tool->Append(ID_TOOL_SCREENSHOT, wxT("Screenshot\tCTRL+P"));
        m_tool->Append(ID_TOOL_SCREENSHOT_SV, wxT("Save Screenshot\tCTRL+SHIFT+P"));
        m_tool->Append(ID_TOOL_SEARCH, wxT("Search\tCTRL+F"));
        m_tool->Append(ID_TOOL_BROWSER, wxT("Open in Browser\tCTRL+O"));
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolClear, this, ID_TOOL_CLEAR);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolSort, this, ID_TOOL_SORT);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolShuffle, this, ID_TOOL_SHUFFLE);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolScreenshot, this, ID_TOOL_SCREENSHOT);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolScreenshotSV, this, ID_TOOL_SCREENSHOT_SV);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolSearch, this, ID_TOOL_SEARCH);
        m_tool->Bind(wxEVT_MENU, &EditorFrame::OnToolOpenBrowser, this, ID_TOOL_BROWSER);

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

        card_image = new wxStaticBitmap(this, wxID_ANY, wxBitmap(177, 254));
        card_info = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(240, 400), wxRE_READONLY | wxRE_MULTILINE);
        card_info->Bind(wxEVT_TEXT_URL, &EditorFrame::OnUrlClicked, this, wxID_ANY);
        card_info->GetCaret()->Hide();
        int wx_gl_attribs[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
        editor_canvas = new wxEditorCanvas(this, wxID_ANY, wx_gl_attribs);

        wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
        sz->AddSpacer(5);
        sz->Add(card_image, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->AddSpacer(5);
        sz->Add(card_info, 1, wxEXPAND | wxALL, 5);
        wxBoxSizer *sz2 = new wxBoxSizer(wxHORIZONTAL);
        sz2->Add(sz);
        sz2->Add(editor_canvas, 1, wxEXPAND | wxALL);
        SetSizer(sz2);
        
        editorFrame = this;
        SetCardInfo(0);
        editor_canvas->SetFocus();

        TextureInfo& st = imageMgr.textures["star"];
        if(st.src) {
            int x1 = st.src->t_width * st.lx;
            int y1 = st.src->t_height * st.ly;
            int x2 = st.src->t_width * st.rx;
            int y2 = st.src->t_height * st.ry;
            star_img = st.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
        }
    }

    EditorFrame::~EditorFrame() {
        if(filterFrame)
            delete filterFrame;
    }

    void EditorFrame::SetCardInfo(unsigned int code) {
        if(code == 0) {
            TextureInfo& sv = imageMgr.textures["sleeve1"];
            if(sv.src == nullptr)
                return;
            card_info->Clear();
            int x1 = sv.src->t_width * sv.lx;
            int y1 = sv.src->t_height * sv.ly;
            int x2 = sv.src->t_width * sv.rx;
            int y2 = sv.src->t_height * sv.ry;
            card_image->SetBitmap(wxBitmap(sv.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1))));
        } else {
            wxImage card_img;
            wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), code);
            if(wxFileExists(file) && card_img.LoadFile(file)) {
                card_image->SetBitmap(wxBitmap(card_img));
            } else {
                TextureInfo& sv = imageMgr.textures["unknown"];
                if(sv.src == nullptr)
                    return;
                int x1 = sv.src->t_width * sv.lx;
                int y1 = sv.src->t_height * sv.ly;
                int x2 = sv.src->t_width * sv.rx;
                int y2 = sv.src->t_height * sv.ry;
                card_image->SetBitmap(wxBitmap(sv.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1))));
            }
            card_info->Clear();
            CardData* cd = dataMgr[code];
            if(cd == nullptr)
                return;
            wxString card_name = cd->name;
            wxRichTextAttr attr;
            card_info->SetDefaultStyle(attr);
            card_info->BeginFontSize(16);
            card_info->BeginBold();
            card_info->WriteText(card_name);
            card_info->EndBold();
            card_info->EndFontSize();
            card_info->Newline();
            if(cd->type & 0x1) {
                for(unsigned int i = 0; i < cd->level; ++i)
                    card_info->WriteImage(star_img);
                card_info->Newline();
                card_info->BeginTextColour(wxColour(180, 140, 40));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ") + dataMgr.GetAttributeString(cd->attribute) + wxT("/") + dataMgr.GetRaceString(cd->race);
                card_info->WriteText(infostr);
                card_info->EndTextColour();
                card_info->Newline();
                card_info->BeginTextColour(wxColour(64, 64, 64));
                if(cd->attack >= 0 && cd->defence >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:%d / DEF:%d"), cd->attack, cd->defence));
                else if(cd->attack >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:%d / DEF:????"), cd->attack));
                else if(cd->defence >= 0)
                    card_info->WriteText(wxString::Format(wxT("ATK:???? / DEF:%d"), cd->defence));
                else
                    card_info->WriteText(wxString::Format(wxT("ATK:???? / DEF:????")));
                card_info->EndTextColour();
                card_info->Newline();
            } else {
                if(cd->type & 0x2)
                    card_info->BeginTextColour(wxColour(0, 192, 128));
                else
                    card_info->BeginTextColour(wxColour(250, 32, 192));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ");
                card_info->WriteText(infostr);
                card_info->EndTextColour();
                card_info->Newline();
            }
            card_info->Newline();
            wxString card_text = cd->texts;
            wxString parse_begin = stringCfg["parse_begin"];
            wxString parse_end = stringCfg["parse_end"];
            size_t start = 0, begin = 0, end = 0, length = card_text.length();
            while(start < length) {
                begin = card_text.find_first_of(parse_begin, start);
                if(begin == -1) {
                    card_info->WriteText(card_text.substr(start));
                    break;
                }
                end = card_text.find_first_of(parse_end, begin + 1);
                while(end != -1 && card_text[end - 1] == wxT('\\'))
                    end = card_text.find_first_of(parse_end, end + 1);
                if(end == -1) {
                    card_info->WriteText(card_text.substr(start));
                    break;
                }
                if(begin > start)
                    card_info->WriteText(card_text.substr(start, begin - start));
                card_info->BeginTextColour(wxColour(0, 0, 255));
                card_info->BeginURL(card_text.substr(begin + 1, end - begin - 1));
                card_info->WriteText(card_text.substr(begin, end - begin + 1));
                card_info->EndURL();
                card_info->EndTextColour();
                start = end + 1;
            }
            card_info->Newline();
            if(cd->setcode) {
                card_info->Newline();
                card_info->WriteText(wxT("Category: "));
                wxString setname = dataMgr.GetSetCode(cd->setcode & 0xffff);
                card_info->BeginTextColour(wxColour(0, 0, 255));
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("] "));
                    card_info->EndURL();
                }
                setname = dataMgr.GetSetCode((cd->setcode >> 16) & 0xffff);
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("]"));
                    card_info->EndURL();
                }
                setname = dataMgr.GetSetCode((cd->setcode >> 32) & 0xffff);
                if(!setname.IsEmpty()) {
                    card_info->BeginURL(setname);
                    card_info->WriteText(wxT("[") + setname + wxT("]"));
                    card_info->EndURL();
                }
                card_info->EndTextColour();
            }
        }
    }
    
    void EditorFrame::AddCard(unsigned int code, unsigned int pos) {
        editor_canvas->GetDeck().InsertCard(code, pos, -1, false);
        editor_canvas->Refresh();
    }
    
    void EditorFrame::OnDeckLoad(wxCommandEvent& evt) {
        wxFileDialog fd(this, wxT("Choose"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if(fd.ShowModal() != wxID_OK)
            return;
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(fd.GetPath())) {
            editor_canvas->ClearDeck();
            editor_canvas->GetDeck() = tempdeck;
            current_file = fd.GetPath();
            SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
            editor_canvas->Refresh();
        } else {
            wxMessageDialog(this, wxT("Cannot load deck file."));
        }
    }

    void EditorFrame::OnDeckSave(wxCommandEvent& evt) {
        if(current_file.IsEmpty())
            OnDeckSaveAs(evt);
        else
            editor_canvas->GetDeck().SaveToFile(current_file);
    }

    void EditorFrame::OnDeckSaveAs(wxCommandEvent& evt) {
        wxFileDialog fd(this, wxT("Save deck file as"), wxT(""), wxT(""), wxT("YGO Deck File |*.ydk"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if(fd.ShowModal() != wxID_OK)
            return;
        editor_canvas->GetDeck().SaveToFile(fd.GetPath());
        current_file = fd.GetPath();
        SetTitle(wxT("YGOpro Deck Editor (") + current_file + wxT(")"));
    }

    void EditorFrame::OnDeckLoadString(wxCommandEvent& evt) {
        wxTextDataObject tdo;
        wxTheClipboard->Open();
        wxTheClipboard->GetData(tdo);
        wxTheClipboard->Close();
        wxString deck_string = tdo.GetText();
        DeckData tempdeck;
        if(deck_string.Left(6) == wxT("ydk://") && tempdeck.LoadFromString(deck_string.Right(deck_string.Length() - 6))) {
            editor_canvas->ClearDeck();
            editor_canvas->GetDeck() = tempdeck;
            editor_canvas->Refresh();
        } else {
            wxMessageDialog(this, wxString("Load from string failed."), wxT("Error"), wxICON_ERROR).ShowModal();
        }
    }

    void EditorFrame::OnDeckSaveString(wxCommandEvent& evt) {
        wxString deck_string = editor_canvas->GetDeck().SaveToString();
        deck_string = wxT("ydk://") + deck_string;
        wxTheClipboard->Open();
        wxTheClipboard->SetData(new wxTextDataObject(deck_string));
        wxTheClipboard->Close();
        editor_canvas->Refresh();
        wxMessageDialog(this, wxString("The deck has been successfully copied to clipboard."), wxT("Information"), wxICON_INFORMATION).ShowModal();
    }

    void EditorFrame::OnToolClear(wxCommandEvent& evt) {
        editor_canvas->ClearDeck();
        editor_canvas->Refresh();
    }

    void EditorFrame::OnToolSort(wxCommandEvent& evt) {
        editor_canvas->GetDeck().Sort();
        editor_canvas->Refresh();
    }

    void EditorFrame::OnToolShuffle(wxCommandEvent& evt) {
        editor_canvas->GetDeck().Shuffle();
        editor_canvas->Refresh();
    }

    void EditorFrame::OnToolScreenshot(wxCommandEvent& evt) {
        editor_canvas->SaveScreenshot(wxT(""), true);
        wxMessageDialog(this, wxString("Screen has been copied to clipboard."), wxT("Information"), wxICON_INFORMATION).ShowModal();
    }

    void EditorFrame::OnToolScreenshotSV(wxCommandEvent& evt) {
        time_t t = time(0);
        wxString path = commonCfg["screenshot_path"];
        if(*path.rbegin() != wxT('/'))
            path.Append(wxT('/'));
        if(!wxFileName::DirExists(path) && !wxFileName::Mkdir(path)) {
            wxMessageDialog(this, wxString("Cannot create folder ") + path, wxT("Error"), wxICON_ERROR).ShowModal();
            return;
        }
        path += wxString::Format(wxT("%d.png"), (int)t);
        editor_canvas->SaveScreenshot(path);
        wxMessageDialog(this, wxString("File has been successful saved to:\n") + path, wxT("Information"), wxICON_INFORMATION).ShowModal();
    }

    void EditorFrame::OnToolSearch(wxCommandEvent& evt) {
        if(filterFrame)
            filterFrame->Raise();
        else {
            filterFrame = new FilterFrame(600, 500);
            filterFrame->Center();
            filterFrame->Show();
        }
    }

    void EditorFrame::OnToolOpenBrowser(wxCommandEvent& evt) {
        wxString neturl = commonCfg["deck_neturl"];
        wxString deck_string = editor_canvas->GetDeck().SaveToString();
        neturl.Replace("{amp}", wxT("&"));
        neturl.Replace("{deck}", deck_string);
        if(current_file.IsEmpty())
            neturl.Replace("{name}", wxT(""));
        else {
            wxFileName fn(current_file);
            neturl.Replace("{name}", fn.GetName());
        }
        wxLaunchDefaultBrowser(neturl);
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
        limitRegulationMgr.SetLimitRegulation(id);
        limitRegulationMgr.GetDeckCardLimitCount(editor_canvas->GetDeck());
        editor_canvas->Refresh();
    }
    
    void EditorFrame::OnUrlClicked(wxTextUrlEvent& evt) {
        wxString url = evt.GetString();
        std::string strurl = (wxT("setname_") + url).ToStdString();
        FilterCondition fc;
        if(stringCfg.Exists(strurl))
            fc.setcode = stringCfg[strurl];
        if(filterFrame)
            filterFrame->Raise();
        else {
            filterFrame = new FilterFrame(600, 500);
            filterFrame->Center();
            filterFrame->Show();
        }
        filterFrame->FilterCards(fc, url);
    }

}
