#include "../../common/common.h"

#include "../../ocgcore/common.h"

#include "../scene_mgr.h"
#include "duel_scene.h"
#include "duel_scene_handler.h"
#include "duel_command.h"

namespace ygopro
{
    
    DuelCommandWait::DuelCommandWait(double tm) {
        end_time = SceneMgr::Get().GetGameTime() + tm;
    }

    bool DuelCommandWait::Handle(std::shared_ptr<DuelScene> pscene) {
        return SceneMgr::Get().GetGameTime() > end_time;
    }

    DuelCommandMsg::DuelCommandMsg(uint8_t msg_type, BufferUtil& reader) {
        this->msg_type = msg_type;
        this->reader = &reader;
    }
    
    bool DuelCommandMsg::Handle(std::shared_ptr<DuelScene> pscene) {
        std::static_pointer_cast<DuelSceneHandler>(pscene->GetSceneHandler())->SolveMessage(msg_type, *reader);
        return true;
    }
    
}
