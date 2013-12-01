#ifndef _DEFRAME_H_
#define _DEFRAME_H_

#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/frame.h"
#include "wx/glcanvas.h"

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
        };

    public:
        EditorFrame(int sx, int sy);
        ~EditorFrame();

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

    private:
        wxAuiManager m_auiManager;
        wxString current_file;
        wxEditorCanvas* editor_canvas;
    };

    extern EditorFrame* editorFrame;
}

#endif //_GAME_H_
