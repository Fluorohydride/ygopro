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
    
    DuelCommandMove::DuelCommandMove(std::shared_ptr<FieldCard> pcard, int side, int loc, int seq, int pos) {
        moving_card = pcard;
        to[0] = side;
        to[1] = loc;
        to[2] = seq;
        to[3] = pos;
    }
    
    bool DuelCommandMove::Handle(DuelScene* pscene) {
        if(moving_card)
            pscene->MoveCard(moving_card, to[0], to[1], to[2], to[3], false, 1.0);
        return true;
    }
    
}
