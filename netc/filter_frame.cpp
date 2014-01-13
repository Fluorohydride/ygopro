#include "filter_frame.h"
#include "editor_frame.h"
#include "game_frame.h"
#include "editor_canvas.h"
#include "card_data.h"
#include "image_mgr.h"
#include "wx/tokenzr.h"

namespace ygopro
{
    FilterFrame* filterFrame = nullptr;
    
    FilterFrame::FilterFrame(int sx, int sy) : wxFrame(nullptr, wxID_ANY, "Card Filter", wxDefaultPosition, wxSize(sx, sy), wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP) {
        wxStaticText* stkey = new wxStaticText(this, wxID_ANY, wxT("Keyword"), wxDefaultPosition, wxDefaultSize);
        filter_att1[0] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st1 = new wxStaticText(this, wxID_ANY, wxT("Attack"), wxDefaultPosition, wxDefaultSize);
        filter_att1[1] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        filter_att1[1]->Disable();
        wxStaticText* st2 = new wxStaticText(this, wxID_ANY, wxT("Defence"), wxDefaultPosition, wxDefaultSize);
        filter_att1[2] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        filter_att1[2]->Disable();
        wxStaticText* st3 = new wxStaticText(this, wxID_ANY, wxT("Star"), wxDefaultPosition, wxDefaultSize);
        filter_att1[3] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        filter_att1[3]->Disable();
        wxStaticText* st4 = new wxStaticText(this, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize);
        filter_att2[0] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[0]->Append(wxT("---"), (void*)0);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x1), (void*)0x1);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x2), (void*)0x2);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x4), (void*)0x4);
        filter_att2[0]->SetSelection(0);
        filter_att2[0]->Bind(wxEVT_COMBOBOX, &FilterFrame::OnTypeSelected, this, wxID_ANY);
        filter_att2[1] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[1]->Append(wxT("---"), (void*)0);
        filter_att2[1]->SetSelection(0);
        filter_att2[1]->Disable();
        wxStaticText* st6 = new wxStaticText(this, wxID_ANY, wxT("Attribute"), wxDefaultPosition, wxDefaultSize);
        filter_att2[2] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[2]->Append(wxT("---"), (void*)0);
        for(unsigned int att = 1; att != 0x80; att <<= 1)
            filter_att2[2]->Append(dataMgr.GetAttributeString(att), (void*)(long)att);
        filter_att2[2]->SetSelection(0);
        filter_att2[2]->Disable();
        wxStaticText* st7 = new wxStaticText(this, wxID_ANY, wxT("Race"), wxDefaultPosition, wxDefaultSize);
        filter_att2[3] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[3]->Append(wxT("---"), (void*)0);
        for(unsigned int rac = 1; rac != 0x800000; rac <<= 1)
            filter_att2[3]->Append(dataMgr.GetRaceString(rac), (void*)(long)rac);
        filter_att2[3]->SetSelection(0);
        filter_att2[3]->Disable();
        wxStaticText* st8 = new wxStaticText(this, wxID_ANY, wxT("Limit"), wxDefaultPosition, wxDefaultSize);
        filter_att2[4] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[4]->Append(wxT("---"), (void*)0);
        filter_att2[4]->Append(stringCfg["pool_limit0"]);
        filter_att2[4]->Append(stringCfg["pool_limit1"]);
        filter_att2[4]->Append(stringCfg["pool_limit2"]);
        filter_att2[4]->Append(stringCfg["pool_ocg"]);
        filter_att2[4]->Append(stringCfg["pool_tcg"]);
        filter_att2[4]->SetSelection(0);
        wxButton* btnSearch = new wxButton(this, wxID_ANY, wxT("Search"), wxDefaultPosition, wxDefaultSize);
        btnSearch->Bind(wxEVT_BUTTON, &FilterFrame::OnSearch, this, wxID_ANY);
        wxButton* btnClear = new wxButton(this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize);
        btnClear->Bind(wxEVT_BUTTON, &FilterFrame::OnClear, this, wxID_ANY);
        prev_page = new wxButton(this, wxID_ANY, wxT("<<"), wxDefaultPosition, wxDefaultSize);
        prev_page->Bind(wxEVT_BUTTON, &FilterFrame::OnPrev, this, wxID_ANY);
        next_page = new wxButton(this, wxID_ANY, wxT(">>"), wxDefaultPosition, wxDefaultSize);
        next_page->Bind(wxEVT_BUTTON, &FilterFrame::OnNext, this, wxID_ANY);
        page_info = new wxStaticText(this, wxID_ANY, wxT(" 1 / 1"), wxDefaultPosition, wxDefaultSize);
        result_info = new wxStatusBar(this, wxID_ANY);
        SetStatusBar(result_info);
        search_result = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY | wxRE_MULTILINE);
        search_result->Bind(wxEVT_TEXT_URL, &FilterFrame::OnCmdClicked, this, wxID_ANY);
        prev_page->Disable();
        next_page->Disable();
        
        wxFlexGridSizer *sz = new wxFlexGridSizer(2);
        sz->Add(stkey, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att1[0], 1, wxEXPAND | wxALL, 2);
        sz->Add(st8, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att2[4], 1, wxEXPAND | wxALL, 2);
        sz->Add(st4, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att2[0], 1, wxEXPAND | wxALL, 2);
        sz->AddSpacer(5);
        sz->Add(filter_att2[1], 1, wxEXPAND | wxALL, 2);
        sz->Add(st6, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att2[2], 1, wxEXPAND | wxALL, 2);
        sz->Add(st7, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att2[3], 1, wxEXPAND | wxALL, 2);
        sz->Add(st1, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att1[1], 1, wxEXPAND | wxALL, 2);
        sz->Add(st2, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att1[2], 1, wxEXPAND | wxALL, 2);
        sz->Add(st3, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 2);
        sz->Add(filter_att1[3], 1, wxEXPAND | wxALL, 2);
        wxBoxSizer *szP = new wxBoxSizer(wxHORIZONTAL);
        szP->SetMinSize(0, 10000);
        szP->Add(prev_page, 1, wxALIGN_BOTTOM, 5);
        szP->AddSpacer(10);
        szP->Add(page_info, 0, wxALIGN_BOTTOM | wxALIGN_CENTER, 5);
        szP->AddSpacer(10);
        szP->Add(next_page, 1, wxALIGN_BOTTOM, 5);
        wxBoxSizer *szV = new wxBoxSizer(wxVERTICAL);
        szV->AddSpacer(10);
        szV->Add(sz);
        szV->AddSpacer(5);
        szV->Add(btnClear, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        szV->AddSpacer(5);
        szV->Add(btnSearch, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        szV->Add(szP, 1, wxEXPAND | wxALL);
        szV->AddSpacer(5);
        wxBoxSizer *szH = new wxBoxSizer(wxHORIZONTAL);
        szH->AddSpacer(5);
        szH->Add(search_result, 1, wxEXPAND | wxALL);
        szH->AddSpacer(5);
        szH->Add(szV);
        szH->AddSpacer(5);
        SetSizer(szH);
        
        TextureInfo& st = imageMgr.textures["star"];
        if(st.src) {
            int x1 = st.src->t_width * st.lx;
            int y1 = st.src->t_height * st.ly;
            int x2 = st.src->t_width * st.rx;
            int y2 = st.src->t_height * st.ry;
            star_img = st.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
        }
        
        SetTransparent(224);
    }
    
    FilterFrame::~FilterFrame() {
        filterFrame = nullptr;
    }
    
    void FilterFrame::FilterCards(const FilterCondition& fc, const wxString &filter_text) {
        vector_results.clear();
        dataMgr.FilterCard(fc, filter_text, vector_results);
        page = 0;
        ShowPage(0);
        result_info->SetStatusText(wxString::Format(wxT("%d cards found. "), vector_results.size()));
    }
    
    void FilterFrame::ShowPage(unsigned int page) {
        size_t maxpage = vector_results.empty() ? 0 : ((vector_results.size() - 1) / 10);
        if(page > maxpage)
            page = (unsigned int)maxpage;
        search_result->Clear();
        for(unsigned int i = 0; i < 10 && page * 10 + i < vector_results.size(); ++i) {
            auto& cd = vector_results[page * 10 + i];
            wxString card_name = cd->name;
            wxRichTextAttr attr;
            search_result->SetDefaultStyle(attr);
            search_result->BeginFontSize(16);
            search_result->BeginBold();
            search_result->BeginTextColour(wxColour(0, 0, 128));
            search_result->BeginURL(wxString::Format(wxT("/%d"), cd->code));
            search_result->WriteText(card_name);
            search_result->EndURL();
            search_result->EndTextColour();
            search_result->EndBold();
            search_result->EndFontSize();
            search_result->WriteText(wxT("  "));
            search_result->BeginTextColour(wxColour(0, 0, 255));
            search_result->BeginURL(wxString::Format(wxT("+%d"), cd->code));
            search_result->WriteText(wxT("[MAIN]"));
            search_result->EndURL();
            search_result->WriteText("  ");
            search_result->BeginURL(wxString::Format(wxT("-%d"), cd->code));
            search_result->WriteText(wxT("[SIDE]"));
            search_result->EndURL();
            search_result->EndTextColour();
            search_result->Newline();
            if(cd->type & 0x1) {
                for(unsigned int i = 0; i < cd->level; ++i)
                    search_result->WriteImage(star_img);
                search_result->Newline();
                search_result->BeginTextColour(wxColour(180, 140, 40));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ") + dataMgr.GetAttributeString(cd->attribute) + wxT("/") + dataMgr.GetRaceString(cd->race);
                search_result->WriteText(infostr);
                search_result->EndTextColour();
                search_result->Newline();
                search_result->BeginTextColour(wxColour(64, 64, 64));
                if(cd->attack >= 0 && cd->defence >= 0)
                    search_result->WriteText(wxString::Format(wxT("ATK:%d / DEF:%d"), cd->attack, cd->defence));
                else if(cd->attack >= 0)
                    search_result->WriteText(wxString::Format(wxT("ATK:%d / DEF:????"), cd->attack));
                else if(cd->defence >= 0)
                    search_result->WriteText(wxString::Format(wxT("ATK:???? / DEF:%d"), cd->defence));
                else
                    search_result->WriteText(wxString::Format(wxT("ATK:???? / DEF:????")));
                search_result->EndTextColour();
                search_result->Newline();
            } else {
                if(cd->type & 0x2)
                    search_result->BeginTextColour(wxColour(0, 192, 128));
                else
                    search_result->BeginTextColour(wxColour(250, 32, 192));
                wxString infostr = wxT("[") + dataMgr.GetTypeString(cd->type) + wxT("] ");
                search_result->WriteText(infostr);
                search_result->EndTextColour();
                search_result->Newline();
            }
            search_result->Newline();
        }
        if(page > 0)
            prev_page->Enable();
        else
            prev_page->Disable();
        if(page * 10 + 10 < vector_results.size())
            next_page->Enable();
        else
            next_page->Disable();
        page_info->SetLabel(wxString::Format(wxT("%d / %d"), page + 1, maxpage + 1));
    }
    
    void FilterFrame::ValueCheck(const wxString& val, int& min, int& max) {
        if(val.Length() == 0)
            return;
        if(val[0] == wxT('?')) {
            min = max = -2;
        }
        wxStringTokenizer tk(val, wxT("-"), wxTOKEN_STRTOK);
        if(tk.CountTokens() == 1) {
            long v;
            tk.GetNextToken().ToLong(&v);
            min = max = (int)v;
        } else {
            long v;
            tk.GetNextToken().ToLong(&v);
            min = (int)v;
            tk.GetNextToken().ToLong(&v);
            max = (int)v;
        }
    }
    
    void FilterFrame::OnClear(wxCommandEvent& evt) {
        filter_att2[0]->SetSelection(0);
        filter_att2[1]->Disable();
        filter_att2[2]->Disable();
        filter_att2[3]->Disable();
        filter_att2[4]->SetSelection(0);
        filter_att1[0]->Clear();
        filter_att1[1]->Disable();
        filter_att1[2]->Disable();
        filter_att1[3]->Disable();
    }
    
    void FilterFrame::OnSearch(wxCommandEvent& evt) {
        FilterCondition fc;
        wxString fstr = filter_att1[0]->GetValue();
        fc.type = (unsigned int)(long)filter_att2[0]->GetClientData(filter_att2[0]->GetSelection());
        fc.subtype = (unsigned int)(long)filter_att2[1]->GetClientData(filter_att2[1]->GetSelection());
        if(fc.type == 0x1) {
            fc.attribute = (unsigned int)(long)filter_att2[2]->GetClientData(filter_att2[2]->GetSelection());
            fc.race = (unsigned int)(long)filter_att2[3]->GetClientData(filter_att2[3]->GetSelection());
            ValueCheck(filter_att1[1]->GetValue(), fc.atkmin, fc.atkmax);
            ValueCheck(filter_att1[2]->GetValue(), fc.defmin, fc.defmax);
            ValueCheck(filter_att1[3]->GetValue(), fc.lvmin, fc.lvmax);
        }
        int l = 0;
        switch(filter_att2[4]->GetSelection()) {
            case 0: break;
            case 1: l = 1; break;
            case 2: l = 2; break;
            case 3: l = 3; break;
            case 4: fc.pool = 1; break;
            case 5: fc.pool = 2; break;
        }
        vector_results.clear();
        if(l == 0)
            dataMgr.FilterCard(fc, fstr, vector_results);
        else
            limitRegulationMgr.FilterCard(l - 1, fc, fstr, vector_results);
        page = 0;
        ShowPage(0);
        result_info->SetStatusText(wxString::Format(wxT("%d cards found. "), vector_results.size()));
    }
    
    void FilterFrame::OnPrev(wxCommandEvent& evt) {
        if(page > 0) {
            page --;
            ShowPage(page);
        }
    }
    
    void FilterFrame::OnNext(wxCommandEvent& evt) {
        size_t maxpage = vector_results.empty() ? 0 : ((vector_results.size() - 1) / 10);
        if(page < maxpage) {
            page++;
            ShowPage(page);
        }
    }
    
    void FilterFrame::OnCmdClicked(wxTextUrlEvent& evt) {
        wxString url = evt.GetString();
        long code;
        url.Right(url.length() - 1).ToLong(&code);
        if(url[0] == wxT('+'))
            editorFrame->AddCard((unsigned int)code, 1);
        else if(url[0] == wxT('-'))
            editorFrame->AddCard((unsigned int)code, 3);
        editorFrame->SetCardInfo((unsigned int)code);
    }

    void FilterFrame::OnTypeSelected(wxCommandEvent& evt) {
        switch(filter_att2[0]->GetSelection()) {
            case 0:
                filter_att2[1]->Disable();
                filter_att2[2]->Disable();
                filter_att2[3]->Disable();
                filter_att1[1]->Disable();
                filter_att1[2]->Disable();
                filter_att1[3]->Disable();
                break;
            case 1:
                filter_att2[1]->Enable();
                filter_att2[1]->Clear();
                filter_att2[1]->Append(wxT("---"), (void*)0);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x10), (void*)0x10);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x20), (void*)0x20);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x40), (void*)0x40);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x80), (void*)0x80);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x2000), (void*)0x2000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x800000), (void*)0x800000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x200), (void*)0x200);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x400), (void*)0x400);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x800), (void*)0x800);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x1000), (void*)0x1000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x200000), (void*)0x200000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x400000), (void*)0x400000);
                filter_att2[1]->SetSelection(0);
                filter_att2[2]->Enable();
                filter_att2[3]->Enable();
                filter_att1[1]->Enable();
                filter_att1[2]->Enable();
                filter_att1[3]->Enable();
                break;
            case 2:
                filter_att2[1]->Enable();
                filter_att2[1]->Clear();
                filter_att2[1]->Append(wxT("---"), (void*)0);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x10), (void*)0);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x10000), (void*)0x10000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x20000), (void*)0x20000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x40000), (void*)0x40000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x80000), (void*)0x80000);
                filter_att2[1]->SetSelection(0);
                filter_att2[2]->Disable();
                filter_att2[3]->Disable();
                filter_att1[1]->Disable();
                filter_att1[2]->Disable();
                filter_att1[3]->Disable();
                break;
            case 3:
                filter_att2[1]->Enable();
                filter_att2[1]->Clear();
                filter_att2[1]->Append(wxT("---"), (void*)0);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x10), (void*)0);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x20000), (void*)0x20000);
                filter_att2[1]->Append(dataMgr.GetTypeString2(0x100000), (void*)0x100000);
                filter_att2[1]->SetSelection(0);
                filter_att2[2]->Disable();
                filter_att2[3]->Disable();
                filter_att1[1]->Disable();
                filter_att1[2]->Disable();
                filter_att1[3]->Disable();
                break;
            default:
                break;
        }
    }
}
