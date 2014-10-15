#ifndef _DUEL_INPUT_HANDLER_H_
#define _DUEL_INPUT_HANDLER_H_

#include "../scene_mgr.h"

namespace ygopro
{
        
    class DuelScene;
    
    class DuelSceneHandler : public SceneHandler {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual void BeginHandler();
    };
    
}

#endif
