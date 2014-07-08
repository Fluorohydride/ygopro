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
    /*
    struct CardData;
    struct FilterCondition;
    
    class FilterFrame : public wxFrame {
        
    public:
        FilterFrame();
        ~FilterFrame();
        
        void FilterCards(const wxString& filter_text);
        void ShowPage(unsigned int page);
        void ValueCheck(const wxString& val, int& min, int& max);
        void OnClear(wxCommandEvent& evt);
        void OnSearch(wxCommandEvent& evt);
        void OnPrev(wxCommandEvent& evt);
        void OnNext(wxCommandEvent& evt);
        void OnImageHover(wxMouseEvent& evt);
        void OnImageLeave(wxMouseEvent& evt);
        void OnImageLClick(wxMouseEvent& evt);
        void OnImageRClick(wxMouseEvent& evt);
        void OnHoverTimer(wxTimerEvent& evt);
        void OnKeywordEnter(wxCommandEvent& evt);
        void OnTypeSelected(wxCommandEvent& evt);
        
    private:
        unsigned int page = 0;
        wxTimer hover_timer;
        wxImage star_img;
        wxImage card_unknown;
        wxImage card_sleeve;
        wxStaticText* page_info;
        wxButton* prev_page;
        wxButton* next_page;
        wxTextCtrl* filter_att1[4];
        wxComboBox* filter_att2[5];
        wxStatusBar* result_info;
        std::vector<CardData*> vector_results;
        wxStaticBitmap* thumbs[15];
    };
    
    extern FilterFrame* filterFrame;
     */
}

#endif //_FILTER_FRAME_H_
