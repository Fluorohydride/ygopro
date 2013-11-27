#ifndef _DEFRAME_H_
#define _DEFRAME_H_

#include <gl/glew.h>
#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/frame.h"
#include "wx/glcanvas.h"

namespace ygopro
{

    class wxEditorCanvas;
    
	class EditorFrame : public wxFrame
	{

		enum
		{
			ID_DECK_LOAD = wxID_HIGHEST + 1,
            ID_DECK_SAVE,
            ID_DECK_SAVEAS,
            ID_TOOL_CLEAR,
            ID_TOOL_SORT,
            ID_TOOL_SHUFFLE,
            ID_TOOL_SCREENSHOT,
            ID_TOOL_SEARCH,
            
            ID_MENUEND,
            ID_BANLISTSTART = wxID_HIGHEST + 100,
		};

	public:
		EditorFrame(int sx, int sy);
		~EditorFrame();

        void OnDeckLoad(wxCommandEvent& evt);
        void OnDeckSave(wxCommandEvent& evt);
        void OnDeckSaveAs(wxCommandEvent& evt);
        
        DECLARE_EVENT_TABLE()
        
    private:
        wxAuiManager m_auiManager;
        wxString current_file;
        wxEditorCanvas* editor_canvas;
	};

	extern EditorFrame* editorFrame;
}

#endif //_GAME_H_
