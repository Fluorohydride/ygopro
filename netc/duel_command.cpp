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
    
    DuelCommandMove::DuelCommandMove(unsigned int f, unsigned int t) {
        
    }
    
    bool DuelCommandMove::Handle(DuelScene* pscene) {
        auto pcard = pscene->GetCard(0, 1, 0, 0);
        if(pcard)
            pscene->MoveCard(pcard, 0, 2, 0, 1, false, 1.0);
        return true;
    }
    
}
