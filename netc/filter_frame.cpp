#include "filter_frame.h"

namespace ygopro
{
    FilterFrame* filterFrame = nullptr;
    
    FilterFrame::FilterFrame(int sx, int sy, const wxString& init_text) : wxFrame(nullptr, wxID_ANY, "Card Filter", wxDefaultPosition, wxSize(sx, sy)) {
        wxStaticText* stkey = new wxStaticText(this, wxID_ANY, wxT("Keyword"), wxDefaultPosition, wxDefaultSize);
        wxTextCtrl* txkey = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200, -1));
        wxStaticText* st1 = new wxStaticText(this, wxID_ANY, wxT("Attack"), wxDefaultPosition, wxDefaultSize);
        wxTextCtrl* tx1 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        wxStaticText* st2 = new wxStaticText(this, wxID_ANY, wxT("Defence"), wxDefaultPosition, wxDefaultSize);
        wxTextCtrl* tx2 = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize);
        search_result = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxRE_READONLY | wxRE_MULTILINE);
        search_result->Bind(wxEVT_TEXT_URL, &FilterFrame::OnCmdClicked, this, wxID_ANY);
        search_result->GetCaret()->Hide();
        
        wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
        sz->AddSpacer(5);
        sz->Add(stkey, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->Add(txkey, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->Add(st1, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->Add(tx1, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->Add(st2, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        sz->Add(tx2, 0, wxALIGN_CENTER_HORIZONTAL, 5);
        wxBoxSizer *sz2 = new wxBoxSizer(wxHORIZONTAL);
        sz2->Add(sz);
        sz2->Add(search_result, 1, wxEXPAND | wxALL);
        SetSizer(sz2);
        
        if(init_text.size())
            FilterCards(init_text);
    }
    
    FilterFrame::~FilterFrame() {
        filterFrame = nullptr;
    }
    
    void FilterFrame::FilterCards(const wxString &filter_text) {
        
    }
    
    void FilterFrame::OnCmdClicked(wxTextUrlEvent& evt) {
        
    }
    
}
