#ifndef _DUEL_INPUT_HANDLER_H_
#define _DUEL_INPUT_HANDLER_H_

#include "../scene_mgr.h"

namespace ygopro
{
    class DuelScene;
    struct DeckCardData;
    
    class DuelInputHandler : public InputHandler {
    public:
        DuelInputHandler(std::shared_ptr<DuelScene> pscene);
        virtual bool Update();
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void MouseWheel(sgui::MouseWheelEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
    protected:
    };
    
}

#endif
