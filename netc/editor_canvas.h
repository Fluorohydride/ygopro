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

	private:
		wxGLContext* glcontext;
		unsigned int glwidth;
		unsigned int glheight;
        TextureInfo* t_buildbg;
        TextureInfo* t_deckbg;
        TextureInfo* t_font;
        TextureInfo* t_limits[3];
        DeckData current_deck;
        
	public:
		wxEditorCanvas(wxFrame* parent, int id, int* args);
		virtual ~wxEditorCanvas();

        DeckData& GetDeck() { return current_deck; }
		void ClearDeck();
        void SaveScreenshot(const wxString& file, bool clipboard = false);
        
        void drawString(const char* str, int size, unsigned int color, float lx, float ly, float rx, float ry, bool limit);
        void drawNumber(int number, unsigned int color, float lx, float ly, float rx, float ry);
		void drawScene();

		// events
		void eventResized(wxSizeEvent& evt);
		void eventRender(wxPaintEvent& evt);
		void eventMouseMoved(wxMouseEvent& evt);
		void eventMouseWheelMoved(wxMouseEvent& evt);
		void eventMouseDown(wxMouseEvent& evt);
		void eventMouseReleased(wxMouseEvent& evt);
		void eventMouseLeftWindow(wxMouseEvent& evt);

		DECLARE_EVENT_TABLE()
	};

}

#endif
