#include "../../common/common.h"

#include "../sungui.h"
#include "duel_command.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    DuelSceneHandler::DuelSceneHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
    }
    
    bool DuelSceneHandler::UpdateEvent() {
        auto pscene = duel_scene.lock();
        if(!proto_handler->IsEmpty()) {
            do {
                auto cmd = proto_handler->PullCommand();
                if(cmd == nullptr)
                    break;
                if(!cmd->Handle(pscene))
                    break;
                proto_handler->PopCommand();
            } while (proto_handler->IsEmpty());
        } else
            proto_handler->GetProto();
        return !proto_handler->ProtoEnd();
    }
    
    void DuelSceneHandler::BeginHandler() {
        proto_handler->BeginProto();
    }
    
    int DuelProtoHandler::MessageToCmd(unsigned int sz) {
        return 0;
    }
    
}