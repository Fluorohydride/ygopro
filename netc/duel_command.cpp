#include "../common/common.h"

#include "scene_mgr.h"
#include "duel_scene.h"
#include "duel_command.h"

namespace ygopro
{
    
    DuelCommandWait::DuelCommandWait(double tm) {
        end_time = SceneMgr::Get().GetGameTime() + tm;
    }
    
    bool DuelCommandWait::Handle(DuelScene* pscene) {
        return SceneMgr::Get().GetGameTime() > end_time;
    }
    
    bool DuelMessageMove::Handle(DuelScene* pscene) {
        auto moving_card = pscene->GetCard(pdata[0], pdata[1], pdata[2], pdata[3]);
        if(moving_card)
            pscene->MoveCard(moving_card, pdata[4], pdata[5], pdata[6], pdata[7], false, 1.0);
        return true;
    }
    
}
