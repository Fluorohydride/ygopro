#ifndef _BUILD_INPUT_HANDLER_H_
#define _BUILD_INPUT_HANDLER_H_

#include "../scene_mgr.h"

namespace ygopro
{
    class BuildScene;
    struct DeckCardData;
    
    class BuildInputHandler : public InputHandler {
    public:
        BuildInputHandler(std::shared_ptr<BuildScene> pscene);
        virtual bool UpdateInput();
        virtual void MouseMove(int32_t x, int32_t y);
        virtual void MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y);
        virtual void MouseWheel(int32_t deltax, int32_t deltay);
        virtual void KeyDown(int32_t key, int32_t mods);
        virtual void KeyUp(int32_t key, int32_t mods);
        
    protected:
        std::weak_ptr<BuildScene> build_scene;
        std::pair<int32_t, int32_t> hover_pos;
        std::pair<int32_t, int32_t> click_pos;
        std::weak_ptr<DeckCardData> hover_obj;
        bool show_info = false;
        bool show_info_begin = false;
        int64_t show_info_time = 0;
    };
    
}

#endif
