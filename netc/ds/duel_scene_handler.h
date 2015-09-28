#ifndef _DUEL_SCENE_HANDLER_H_
#define _DUEL_SCENE_HANDLER_H_

#include "utils/command.h"
#include "utils/bufferutil.h"

#include "../scene_mgr.h"

namespace ygopro
{
    
    class DuelScene;
    class FieldBlock;
    class FieldCard;
    class InfoPanel;
    
    class DuelSceneHandler : public SceneHandler {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual void BeginHandler();
        virtual bool UpdateHandler();
        virtual void MouseMove(int32_t x, int32_t y);
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseWheel(float deltax, float deltay);
        virtual void KeyDown(int32_t key, int32_t mods);
        virtual void KeyUp(int32_t key, int32_t mods);
        
        
        int32_t SolveMessage(uint8_t msg_type, BufferUtil& reader);
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        
        bool btnDown[2] = {false};
        v2i btnPos[2];
        v2i click_pos = {0, 0};
        int64_t show_info_time = 0;
        std::weak_ptr<FieldBlock> pre_block;
        std::weak_ptr<FieldCard> pre_card;
        std::shared_ptr<InfoPanel> info_panel;
    };
    
}

#endif
