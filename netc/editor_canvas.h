#ifndef _EDITOR_CANVAS_H_
#define _EDITOR_CANVAS_H_

#include "wx/wx.h"
#include "wx/glcanvas.h"
#include "card_data.h"
#include <vector>

namespace ygopro
{
    
    struct TextureInfo;
    struct DeckData;
    
    struct EditorCardData {
        CardData* cd;
        TextureInfo* ti;
        
        bool deck_sort(EditorCardData* c1, EditorCardData* c2) {
            CardData* p1 = c1->cd;
            CardData* p2 = c2->cd;
            if((p1->type & 0x7) != (p2->type & 0x7))
                return (p1->type & 0x7) < (p2->type & 0x7);
            if((p1->type & 0x7) == 1) {
                int type1 = (p1->type & 0x8020c0) ? (p1->type & 0x8020c1) : (p1->type & 0x31);
                int type2 = (p2->type & 0x8020c0) ? (p2->type & 0x8020c1) : (p2->type & 0x31);
                if(type1 != type2)
                    return type1 < type2;
                if(p1->level != p2->level)
                    return p1->level > p2->level;
                if(p1->attack != p2->attack)
                    return p1->attack > p2->attack;
                if(p1->defence != p2->defence)
                    return p1->defence > p2->defence;
                else return p1->code < p2->code;
            }
            if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
                return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
            return p1->code < p2->code;
        }
    };
    
	class wxEditorCanvas : public wxGLCanvas {

	private:
		wxGLContext* glcontext;
		unsigned int glwidth;
		unsigned int glheight;
        TextureInfo* t_buildbg;
        std::vector<EditorCardData> main_deck;
        std::vector<EditorCardData> extra_deck;
        std::vector<EditorCardData> side_deck;
        
	public:
		wxEditorCanvas(wxFrame* parent, int id, int* args);
		virtual ~wxEditorCanvas();

        void setDeck(DeckData* deck);
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
