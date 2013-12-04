#ifndef _EDITOR_CANVAS_H_
#define _EDITOR_CANVAS_H_

#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "deck_data.h"
#include <vector>

namespace ygopro
{
    
    struct TextureInfo;
    
	class wxEditorCanvas : public wxGLCanvas {

        enum {
            ID_POP_ADDCARD = wxID_HIGHEST + 1,
            ID_POP_DELCARD,
            ID_POP_TOSIDE,
            ID_POP_TOMAIN,
        };

	private:
		wxGLContext* glcontext;
        wxTimer hover_timer;
		unsigned int glwidth;
		unsigned int glheight;
        TextureInfo* t_buildbg;
        TextureInfo* t_deckbg;
        TextureInfo* t_font;
        TextureInfo* t_hmask;
        TextureInfo* t_limits[3];
        DeckData current_deck;
        short hover_field;
        short hover_index;
        short click_field;
        short click_index;
        unsigned int hover_code;
        
	public:
		wxEditorCanvas(wxFrame* parent, int id, int* args);
		virtual ~wxEditorCanvas();

        DeckData& GetDeck() { return current_deck; }
		void ClearDeck();
        void SaveScreenshot(const wxString& file, bool clipboard = false);
        
        void DrawString(const char* str, int size, unsigned int color, float lx, float ly, float rx, float ry, bool limit);
        void DrawNumber(int number, unsigned int color, float lx, float ly, float rx, float ry);
		void DrawScene();

		// events
		void EventResized(wxSizeEvent& evt);
		void EventRender(wxPaintEvent& evt);
		void EventMouseMoved(wxMouseEvent& evt);
		void EventMouseWheelMoved(wxMouseEvent& evt);
		void EventMouseDown(wxMouseEvent& evt);
		void EventMouseReleased(wxMouseEvent& evt);
		void EventMouseLeftWindow(wxMouseEvent& evt);
        void OnHoverTimer(wxTimerEvent& evt);
        void OnPopupMenu(wxCommandEvent& evt);

	};

}

#endif
