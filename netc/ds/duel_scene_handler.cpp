#include "../../common/common.h"

#include "../sungui.h"
#include "duel_command.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    DuelSceneHandler::DuelSceneHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
    }
    
    void DuelSceneHandler::UpdateEvent() {
        auto pscene = duel_scene.lock();
        PullEvent();
        do {
            auto cmd = duel_commands.PullCommand();
            if(cmd == nullptr)
                break;
            if(!cmd->Handle(pscene))
                break;
            duel_commands.PopCommand();
        } while (duel_commands.IsEmpty());
    }
    
    void DuelSceneHandler::BeginHandler() {
        
    }
    
}