#include "filter_frame.h"
#include "game_frame.h"
#include "game_canvas.h"
#include "card_data.h"
#include "image_mgr.h"
#include "wx/tokenzr.h"

namespace ygopro
{
    FilterFrame* filterFrame = nullptr;
    
    FilterFrame::FilterFrame() : wxFrame(nullptr, wxID_ANY, stringCfg["eui_filter_title"], wxDefaultPosition, wxSize(680, 480), wxDEFAULT_FRAME_STYLE) {
        
        TextureInfo& st = imageMgr.textures["star"];
        if(st.src) {
            int x1 = st.src->t_width * st.lx;
            int y1 = st.src->t_height * st.ly;
            int x2 = st.src->t_width * st.rx;
            int y2 = st.src->t_height * st.ry;
            star_img = st.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
        }
        TextureInfo& sv = imageMgr.textures["unknown"];
        if(sv.src) {
            int x1 = sv.src->t_width * sv.lx;
            int y1 = sv.src->t_height * sv.ly;
            int x2 = sv.src->t_width * sv.rx;
            int y2 = sv.src->t_height * sv.ry;
            card_unknown = sv.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
            card_unknown.Rescale(88, 127, wxIMAGE_QUALITY_BICUBIC);
        }
        TextureInfo& sl = imageMgr.textures["sleeve1"];
        if(sl.src) {
            int x1 = sl.src->t_width * sl.lx;
            int y1 = sl.src->t_height * sl.ly;
            int x2 = sl.src->t_width * sl.rx;
            int y2 = sl.src->t_height * sl.ry;
            card_sleeve = sl.src->img.GetSubImage(wxRect(x1, y1, x2 - x1, y2 - y1));
            card_sleeve.Rescale(88, 127, wxIMAGE_QUALITY_BICUBIC);
        }
        
        hover_timer.SetOwner(this);
        Bind(wxEVT_TIMER, &FilterFrame::OnHoverTimer, this);
        wxStaticText* stkey = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_keyword"], wxDefaultPosition, wxDefaultSize);
        filter_att1[0] = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        filter_att1[0]->Bind(wxEVT_TEXT_ENTER, &FilterFrame::OnKeywordEnter, this, wxID_ANY);
        filter_att1[0]->SetToolTip(stringCfg["eui_filter_tooltip"]);
        wxStaticText* st1 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_attack"], wxDefaultPosition, wxDefaultSize);
        filter_att1[1] = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        filter_att1[1]->Disable();
        wxStaticText* st2 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_defence"], wxDefaultPosition, wxDefaultSize);
        filter_att1[2] = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        filter_att1[2]->Disable();
        wxStaticText* st3 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_star"], wxDefaultPosition, wxDefaultSize);
        filter_att1[3] = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
        filter_att1[3]->Disable();
        wxStaticText* st4 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_type"], wxDefaultPosition, wxDefaultSize);
        filter_att2[0] = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[0]->Append(stringCfg["eui_filter_na"], (void*)0);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x1), (void*)0x1);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x2), (void*)0x2);
        filter_att2[0]->Append(dataMgr.GetTypeString2(0x4), (void*)0x4);
        filter_att2[0]->SetSelection(0);
        filter_att2[0]->Bind(wxEVT_COMBOBOX, &FilterFrame::OnTypeSelected, this, wxID_ANY);
        filter_att2[1] = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[1]->Append(stringCfg["eui_filter_na"], (void*)0);
        filter_att2[1]->SetSelection(0);
        filter_att2[1]->Disable();
        wxStaticText* st6 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_attribute"], wxDefaultPosition, wxDefaultSize);
        filter_att2[2] = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[2]->Append(stringCfg["eui_filter_na"], (void*)0);
        for(unsigned int att = 1; att != 0x80; att <<= 1)
            filter_att2[2]->Append(dataMgr.GetAttributeString(att), (void*)(long)att);
        filter_att2[2]->SetSelection(0);
        filter_att2[2]->Disable();
        wxStaticText* st7 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_race"], wxDefaultPosition, wxDefaultSize);
        filter_att2[3] = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[3]->Append(stringCfg["eui_filter_na"], (void*)0);
        for(unsigned int rac = 1; rac != 0x800000; rac <<= 1)
            filter_att2[3]->Append(dataMgr.GetRaceString(rac), (void*)(long)rac);
        filter_att2[3]->SetSelection(0);
        filter_att2[3]->Disable();
        wxStaticText* st8 = new wxStaticText(this, wxID_ANY, stringCfg["eui_filter_limit"], wxDefaultPosition, wxDefaultSize);
        filter_att2[4] = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
        filter_att2[4]->Append(stringCfg["eui_filter_na"], (void*)0);
        filter_att2[4]->Append(stringCfg["pool_limit0"]);
        filter_att2[4]->Append(stringCfg["pool_limit1"]);
        filter_att2[4]->Append(stringCfg["pool_limit2"]);
        filter_att2[4]->Append(stringCfg["pool_ocg"]);
        filter_att2[4]->Append(stringCfg["pool_tcg"]);
        filter_att2[4]->SetSelection(0);
        wxButton* btnSearch = new wxButton(this, wxID_ANY, stringCfg["eui_filter_search"], wxDefaultPosition, wxDefaultSize);
        btnSearch->Bind(wxEVT_BUTTON, &FilterFrame::OnSearch, this, wxID_ANY);
        wxButton* btnClear = new wxButton(this, wxID_ANY, stringCfg["eui_filter_clear"], wxDefaultPosition, wxDefaultSize);
        btnClear->Bind(wxEVT_BUTTON, &FilterFrame::OnClear, this, wxID_ANY);
        prev_page = new wxButton(this, wxID_ANY, wxT("<<"), wxDefaultPosition, wxDefaultSize);
        prev_page->Bind(wxEVT_BUTTON, &FilterFrame::OnPrev, this, wxID_ANY);
        next_page = new wxButton(this, wxID_ANY, wxT(">>"), wxDefaultPosition, wxDefaultSize);
        next_page->Bind(wxEVT_BUTTON, &FilterFrame::OnNext, this, wxID_ANY);
        page_info = new wxStaticText(this, wxID_ANY, wxT(" 1 / 1"), wxDefaultPosition, wxDefaultSize);
        result_info = new wxStatusBar(this, wxID_ANY);
        SetStatusBar(result_info);
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
        wxBoxSizer *szV = new wxBoxSizer(wxVERTICAL);
        szV->AddSpacer(10);
        szV->Add(sz);
        szV->AddSpacer(5);
        szV->Add(btnSearch, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        szV->AddSpacer(5);
        szV->Add(btnClear, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        szV->AddSpacer(5);
        wxFlexGridSizer* szImg = new wxFlexGridSizer(5);
        for(int i = 0; i < 15; ++i) {
            thumbs[i] = new wxStaticBitmap(this, wxID_ANY, wxBitmap(card_sleeve), wxDefaultPosition, wxSize(88, 127));
            thumbs[i]->Bind(wxEVT_ENTER_WINDOW, &FilterFrame::OnImageHover, this, wxID_ANY);
            thumbs[i]->Bind(wxEVT_LEAVE_WINDOW, &FilterFrame::OnImageLeave, this, wxID_ANY);
            thumbs[i]->Bind(wxEVT_LEFT_DOWN, &FilterFrame::OnImageLClick, this, wxID_ANY);
            thumbs[i]->Bind(wxEVT_RIGHT_DOWN, &FilterFrame::OnImageRClick, this, wxID_ANY);
            thumbs[i]->SetClientData(nullptr);
            szImg->Add(thumbs[i]);
        }
        szImg->Add(prev_page, 0, wxALIGN_LEFT, 5);
        szImg->AddSpacer(5);
        szImg->Add(page_info, 0, wxALIGN_CENTER_VERTICAL, 5);
        szImg->AddSpacer(5);
        szImg->Add(next_page, 0, wxALIGN_RIGHT, 5);
        wxBoxSizer *szH = new wxBoxSizer(wxHORIZONTAL);
        szH->AddSpacer(5);
        szH->Add(szImg);
        szH->AddSpacer(5);
        szH->Add(szV, 1, wxEXPAND | wxALL);
        szH->AddSpacer(5);
        SetSizer(szH);

    }
    
    FilterFrame::~FilterFrame() {
        filterFrame = nullptr;
    }
    
    void FilterFrame::FilterCards(const wxString &filter_text) {
        FilterCondition fc;
        std::string strurl = (wxT("setname_") + filter_text).ToStdString();
        filter_att1[0]->Clear();
        if(stringCfg.Exists(strurl)) {
            filter_att1[0]->AppendText(wxT("#"));
            fc.setcode = stringCfg[strurl];
        }
        filter_att1[0]->AppendText(filter_text);
        vector_results = dataMgr.FilterCard(fc, filter_text, true);
        page = 0;
        ShowPage(0);
        wxString ct = wxString::Format(wxT("%d"), vector_results.size());
        wxString msg = stringCfg["eui_filter_count"];
        msg.Replace(wxT("{count}"), ct);
        result_info->SetStatusText(msg);
    }
    
    void FilterFrame::ShowPage(unsigned int page) {
        size_t maxpage = vector_results.empty() ? 0 : ((vector_results.size() - 1) / 15);
        if(page > maxpage)
            page = (unsigned int)maxpage;
        for(unsigned int i = 0; i < 15; ++i) {
            if(page * 15 + i < vector_results.size()) {
                auto& cd = vector_results[page * 15 + i];
                wxImage card_img;
                wxString file = wxString::Format("%s/%d.jpg", ((const std::string&)commonCfg["image_path"]).c_str(), cd->code);
                if(wxFileExists(file) && card_img.LoadFile(file)) {
                    card_img.Rescale(88, 127, wxIMAGE_QUALITY_BICUBIC);
                    thumbs[i]->SetBitmap(wxBitmap(card_img));
                } else {
                    thumbs[i]->SetBitmap(wxBitmap(card_unknown));
                }
                thumbs[i]->SetClientData((void*)(long)cd->code);
            } else {
                thumbs[i]->SetBitmap(wxBitmap(card_sleeve));
                thumbs[i]->SetClientData(nullptr);
            }
        }
        if(page > 0)
            prev_page->Enable();
        else
            prev_page->Disable();
        if(page * 15 + 15 < vector_results.size())
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
            return;
        }
        wxStringTokenizer tk(val, wxT("-"), wxTOKEN_STRTOK);
        if(tk.CountTokens() == 1) {
            long v = 0;
            tk.GetNextToken().ToLong(&v);
            min = max = (int)v;
        } else {
            long v = 0;
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
        filter_att1[1]->Disable();
        filter_att1[2]->Disable();
        filter_att1[3]->Disable();
    }
    
    void FilterFrame::OnSearch(wxCommandEvent& evt) {
        FilterCondition fc;
        wxString fstr = filter_att1[0]->GetValue();
        if(static_cast<int>(commonCfg["full_width_space"])) {
            fstr.Replace(wxT(" "), stringCfg["full_width_space"]);
            filter_att1[0]->SetLabelText(fstr);
        }
        bool check_desc = true;
        if(fstr[0] == wxT('#')) {
            std::string setstr = (wxT("setname_") + fstr.SubString(1, -1)).ToStdString();
            if(stringCfg.Exists(setstr))
                fc.setcode = stringCfg[setstr];
        } else if(fstr[0] == wxT('@')) {
            long code = 0;
            fstr.SubString(1, -1).ToLong(&code);
            fc.code = code;
            fstr = wxEmptyString;
        }
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
        if(l == 0)
            vector_results = dataMgr.FilterCard(fc, fstr, check_desc);
        else
            vector_results = limitRegulationMgr.FilterCard(l - 1, fc, fstr, check_desc);
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
    
    void FilterFrame::OnImageHover(wxMouseEvent& evt) {
        void* data = static_cast<wxStaticBitmap*>(evt.GetEventObject())->GetClientData();
        unsigned int code = (unsigned int)(long)data;
        if(code == 0)
            return;
        int delay = (int)commonCfg["hover_info_delay"];
        if(delay == 0)
            mainFrame->SetCardInfo(code);
        else {
            hover_timer.Stop();
            hover_timer.SetClientData(evt.GetEventObject());
            hover_timer.StartOnce(delay);
        }
    }

    void FilterFrame::OnImageLeave(wxMouseEvent& evt) {
        wxStaticBitmap* ctrl = static_cast<wxStaticBitmap*>(hover_timer.GetClientData());
        if(ctrl != evt.GetEventObject())
            return;
        hover_timer.Stop();
    }
    
    void FilterFrame::OnImageLClick(wxMouseEvent& evt) {
        void* data = static_cast<wxStaticBitmap*>(evt.GetEventObject())->GetClientData();
        unsigned int code = (unsigned int)(long)data;
        mainFrame->SetCardInfo((unsigned int)code);
        mainFrame->AddCard(code, 1);
    }
    
    void FilterFrame::OnImageRClick(wxMouseEvent& evt) {
        void* data = static_cast<wxStaticBitmap*>(evt.GetEventObject())->GetClientData();
        unsigned int code = (unsigned int)(long)data;
        mainFrame->SetCardInfo((unsigned int)code);
        mainFrame->AddCard(code, 3);
    }
    
    void FilterFrame::OnHoverTimer(wxTimerEvent& evt) {
        wxStaticBitmap* ctrl = static_cast<wxStaticBitmap*>(hover_timer.GetClientData());
        unsigned int code = (unsigned int)(long)ctrl->GetClientData();
        mainFrame->SetCardInfo(code);
    }
    
    void FilterFrame::OnKeywordEnter(wxCommandEvent& evt) {
        OnSearch(evt);
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
                filter_att2[1]->Append(stringCfg["eui_filter_na"], (void*)0);
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
                filter_att2[1]->Append(stringCfg["eui_filter_na"], (void*)0);
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
                filter_att2[1]->Append(stringCfg["eui_filter_na"], (void*)0);
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
