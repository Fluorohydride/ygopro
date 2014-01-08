#ifndef _EDITOR_FRAME_H_
#define _EDITOR_FRAME_H_

#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/glcanvas.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/textctrl.h"

namespace ygopro
{

    class wxEditorCanvas;
    
    class EditorFrame : public wxFrame {

        enum {
            ID_DECK_LOAD = wxID_HIGHEST + 1,
            ID_DECK_SAVE,
            ID_DECK_SAVEAS,
            ID_DECK_LOADSTR,
            ID_DECK_SAVESTR,
            ID_TOOL_CLEAR,
            ID_TOOL_SORT,
            ID_TOOL_SHUFFLE,
            ID_TOOL_SCREENSHOT,
            ID_TOOL_SCREENSHOT_SV,
            ID_TOOL_SEARCH,
            ID_TOOL_BROWSER,
            ID_REGULATION,
            ID_REGULATION_END = ID_REGULATION + 100,
            ID_MENUEND,
            ID_RTC_CARD,
        };

    public:
        EditorFrame(int sx, int sy);
        ~EditorFrame();

        void SetCardInfo(unsigned int code);
        void AddCard(unsigned int code, unsigned int pos);
        
        void OnDeckLoad(wxCommandEvent& evt);
        void OnDeckSave(wxCommandEvent& evt);
        void OnDeckSaveAs(wxCommandEvent& evt);
        void OnDeckLoadString(wxCommandEvent& evt);
        void OnDeckSaveString(wxCommandEvent& evt);

        void OnToolClear(wxCommandEvent& evt);
        void OnToolSort(wxCommandEvent& evt);
        void OnToolShuffle(wxCommandEvent& evt);
        void OnToolScreenshot(wxCommandEvent& evt);
        void OnToolScreenshotSV(wxCommandEvent& evt);
        void OnToolSearch(wxCommandEvent& evt);
        void OnToolOpenBrowser(wxCommandEvent& evt);

        void OnRegulationChange(wxCommandEvent& evt);
        
        void OnUrlClicked(wxTextUrlEvent& evt);
        
    private:
        wxString current_file;
        wxEditorCanvas* editor_canvas;
        wxStaticBitmap* card_image;
        wxRichTextCtrl* card_info;
        wxImage star_img;
    };

    extern EditorFrame* editorFrame;
}

#endif //_EDITOR_FRAME_H_
