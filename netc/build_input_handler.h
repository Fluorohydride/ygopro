#ifndef _BUILD_INPUT_HANDLER_H_
#define _BUILD_INPUT_HANDLER_H_

#include "scene_mgr.h"

namespace ygopro
{
    class BuildScene;
    struct DeckCardData;
    
    class BuildInputHandler {
    public:
        BuildInputHandler(std::shared_ptr<BuildScene> pscene);
        virtual bool Update();
        virtual void MouseMove(sgui::MouseMoveEvent evt);
        virtual void MouseButtonDown(sgui::MouseButtonEvent evt);
        virtual void MouseButtonUp(sgui::MouseButtonEvent evt);
        virtual void MouseWheel(sgui::MouseWheelEvent evt);
        virtual void KeyDown(sgui::KeyEvent evt);
        virtual void KeyUp(sgui::KeyEvent evt);
        
    protected:
        std::weak_ptr<BuildScene> build_cene;
        std::pair<int, int> hover_pos;
        std::pair<int, int> click_pos;
        std::weak_ptr<DeckCardData> hover_obj;
        bool show_info = false;
        bool show_info_begin = false;
        double show_info_time = 0.0;
    };
    
}

#endif
