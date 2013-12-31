#ifndef _FILTER_FRAME_H_
#define _FILTER_FRAME_H_

#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/textctrl.h"

namespace ygopro
{
    
    class FilterFrame : public wxFrame {
        
        enum {
            ID_FILTER_TEXT = wxID_HIGHEST + 1,
        };
        
    public:
        FilterFrame(int sx, int sy, const wxString& init_text = wxT(""));
        ~FilterFrame();
        
        void FilterCards(const wxString& filter_text);
        
        void OnCmdClicked(wxTextUrlEvent& evt);
        
    private:
        wxRichTextCtrl* search_result;
    };
    
    extern FilterFrame* filterFrame;
}

#endif //_FILTER_FRAME_H_
