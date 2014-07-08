#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <wx/wx.h>

#include "xml_config.h"
#include "../common/random.h"

namespace ygopro
{
    
    class GameScene {
    public:
        GameScene();
        ~GameScene();
        
        void SetCardInfo(unsigned int code);
        void AddCard(unsigned int code, unsigned int pos);
        void StopViewRegulation() { view_regulation = 0; }
        
    protected:
    private:
        wxString current_file;
        int view_regulation = 0;
    };
/*
    class wxGameCanvas;
    
    class GameFrame : public wxFrame {

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
            ID_TOOL_SHOWEXCLUSIVE,
            ID_REG_VIEW1,
            ID_REG_VIEW2,
            ID_REG_VIEW3,
            ID_REGULATION,
            ID_REGULATION_END = ID_REGULATION + 100,
            ID_MENUEND,
            ID_RTC_CARD,
        };

    public:
        GameFrame(int sx, int sy);
        ~GameFrame();

        void SetCardInfo(unsigned int code);
        void AddCard(unsigned int code, unsigned int pos);
        void StopViewRegulation() { view_regulation = 0; }
        
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
        void OnToolSwitchExclusive(wxCommandEvent& evt);
        void OnToolViewRegulation(wxCommandEvent& evt);

        void OnRegulationChange(wxCommandEvent& evt);
        
        void OnUrlClicked(wxTextUrlEvent& evt);
        
    private:
        wxString current_file;
        wxGameCanvas* game_canvas;
        wxStaticBitmap* card_image;
        wxRichTextCtrl* card_info;
        wxImage star_img;
        int view_regulation = 0;
    };
*/
//    extern GameFrame* mainFrame;
    extern Random globalRandom;
	extern CommonConfig commonCfg;
    extern CommonConfig stringCfg;
}

#endif //_GAME_SCENE_H_
