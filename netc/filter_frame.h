#ifndef _FILTER_FRAME_H_
#define _FILTER_FRAME_H_

#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/textctrl.h"
#include "wx/combobox.h"
#include <vector>

namespace ygopro
{
    struct CardData;
    struct FilterCondition;
    
    class FilterFrame : public wxFrame {
        
    public:
        FilterFrame(int sx, int sy);
        ~FilterFrame();
        
        void FilterCards(const FilterCondition& fc, const wxString& filter_text);
        void ShowPage(unsigned int page);
        void ValueCheck(const wxString& val, int& min, int& max);
        void OnClear(wxCommandEvent& evt);
        void OnSearch(wxCommandEvent& evt);
        void OnPrev(wxCommandEvent& evt);
        void OnNext(wxCommandEvent& evt);
        void OnCmdClicked(wxTextUrlEvent& evt);
        void OnMenu(wxCommandEvent& evt);
        void OnTypeSelected(wxCommandEvent& evt);
        
    private:
        unsigned int page = 0;
        wxImage star_img;
        wxStaticText* page_info;
        wxButton* prev_page;
        wxButton* next_page;
        wxTextCtrl* filter_att1[4];
        wxComboBox* filter_att2[5];
        wxRichTextCtrl* search_result;
        wxStatusBar* result_info;
        std::vector<CardData*> vector_results;
    };
    
    extern FilterFrame* filterFrame;
}

#endif //_FILTER_FRAME_H_
