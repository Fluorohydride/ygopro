#ifndef _DUEL_INPUT_HANDLER_H_
#define _DUEL_INPUT_HANDLER_H_

#include "../scene_mgr.h"

namespace ygopro
{
    class DuelScene;
    struct DeckCardData;
    struct FieldBlock;
    struct FieldCard;
    
    class DuelInputHandler : public InputHandler {
    public:
        DuelInputHandler(std::shared_ptr<DuelScene> pscene);
        virtual bool UpdateInput();
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void MouseWheel(sgui::MouseWheelEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        bool btnDown[2] = {false};
        v2i btnPos[2];
        std::weak_ptr<FieldBlock> pre_block;
        std::weak_ptr<FieldCard> pre_card;
        v2i click_pos = {0, 0};
    };
    
}

#endif
