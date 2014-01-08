#include "filter_frame.h"
#include "editor_frame.h"
#include "editor_canvas.h"
#include "card_data.h"
#include "image_mgr.h"
#include "wx/tokenzr.h"

namespace ygopro
{
    FilterFrame* filterFrame = nullptr;
    
    FilterFrame::FilterFrame(int sx, int sy) : wxFrame(nullptr, wxID_ANY, "Card Filter", wxDefaultPosition, wxSize(sx, sy)) {
        wxStaticText* stkey = new wxStaticText(this, wxID_ANY, wxT("Keyword"), wxDefaultPosition, wxDefaultSize);
        filter_att1[0] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st1 = new wxStaticText(this, wxID_ANY, wxT("Attack"), wxDefaultPosition, wxDefaultSize);
        filter_att1[1] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st2 = new wxStaticText(this, wxID_ANY, wxT("Defence"), wxDefaultPosition, wxDefaultSize);
        filter_att1[2] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st3 = new wxStaticText(this, wxID_ANY, wxT("Star"), wxDefaultPosition, wxDefaultSize);
        filter_att1[3] = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st4 = new wxStaticText(this, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize);
        filter_att2[0] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[0]->Append(wxT("---"), (void*)0);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x1), (void*)0x1);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x21), (void*)0x21);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x81), (void*)0x81);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x41), (void*)0x41);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x2001), (void*)0x20001);
        filter_att2[0]->Append(dataMgr.GetTypeString(0x800001), (void*)0x800001);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x2), (void*)0x2);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x4), (void*)0x4);
        filter_att2[0]->SetSelection(0);
        wxStaticText* st5 = new wxStaticText(this, wxID_ANY, wxT("Subtype"), wxDefaultPosition, wxDefaultSize);
        filter_att2[1] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[1]->Append(wxT("---"), (void*)0);
        filter_att2[1]->SetSelection(0);
        wxStaticText* st6 = new wxStaticText(this, wxID_ANY, wxT("Attribute"), wxDefaultPosition, wxDefaultSize);
        filter_att2[2] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[2]->Append(wxT("---"), (void*)0);
        for(unsigned int att = 1; att != 0x80; att <<= 1)
            filter_att2[2]->Append(dataMgr.GetAttributeString(att), (void*)(long)att);
        filter_att2[2]->SetSelection(0);
        wxStaticText* st7 = new wxStaticText(this, wxID_ANY, wxT("Race"), wxDefaultPosition, wxDefaultSize);
        filter_att2[3] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[3]->Append(wxT("---"), (void*)0);
        for(unsigned int rac = 1; rac != 0x800000; rac <<= 1)
            filter_att2[3]->Append(dataMgr.GetRaceString(rac), (void*)(long)rac);
        filter_att2[3]->SetSelection(0);
        wxStaticText* st8 = new wxStaticText(this, wxID_ANY, wxT("Limit"), wxDefaultPosition, wxDefaultSize);
        filter_att2[4] = new wxComboBox(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
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
        search_result->GetCaret()->Hide();
        prev_page->Disable();
        next_page->Disable();
        
        wxFlexGridSizer *sz = new wxFlexGridSizer(2);
        sz->Add(stkey, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att1[0], 0, wxALIGN_LEFT, 5);
        sz->Add(st1, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att1[1], 0, wxALIGN_LEFT, 5);
        sz->Add(st2, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att1[2], 0, wxALIGN_LEFT, 5);
        sz->Add(st3, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att1[3], 0, wxALIGN_LEFT, 5);
        sz->Add(st4, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att2[0], 0, wxALIGN_LEFT, 5);
        sz->Add(st5, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att2[1], 0, wxALIGN_LEFT, 5);
        sz->Add(st6, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att2[2], 0, wxALIGN_LEFT, 5);
        sz->Add(st7, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att2[3], 0, wxALIGN_LEFT, 5);
        sz->Add(st8, 0, wxALIGN_LEFT, 5);
        sz->Add(filter_att2[4], 0, wxALIGN_LEFT, 5);
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
        
        SetTransparent(240);
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
        
    }
    
    void FilterFrame::OnSearch(wxCommandEvent& evt) {
        FilterCondition fc;
        wxString fstr = filter_att1[0]->GetValue();
        ValueCheck(filter_att1[1]->GetValue(), fc.atkmin, fc.atkmax);
        ValueCheck(filter_att1[2]->GetValue(), fc.defmin, fc.defmax);
        ValueCheck(filter_att1[3]->GetValue(), fc.lvmin, fc.lvmax);
        vector_results.clear();
        dataMgr.FilterCard(fc, fstr, vector_results);
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
    
}
