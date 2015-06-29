//#include "buildin/common.h"
//
//#include "ocgcore/common.h"
//
//#include "../scene_mgr.h"
//#include "duel_scene.h"
//#include "duel_scene_handler.h"
//#include "duel_command.h"
//
//namespace ygopro
//{
//    
//    DuelCmdWait::DuelCmdWait(double tm) {
//        end_time = SceneMgr::Get().GetGameTime() + tm;
//    }
//
//    bool DuelCmdWait::Handle(std::shared_ptr<DuelScene> pscene) {
//        return SceneMgr::Get().GetGameTime() > end_time;
//    }
//
//    DuelCmdMsg::DuelCmdMsg(uint8_t msg_type, BufferUtil& reader) {
//        this->msg_type = msg_type;
//        this->reader = &reader;
//    }
//    
//    bool DuelCmdMsg::Handle(std::shared_ptr<DuelScene> pscene) {
//        std::static_pointer_cast<DuelSceneHandler>(pscene->GetSceneHandler())->SolveMessage(msg_type, *reader);
//        return true;
//    }
//    
//    DuelCmdRefreshPos::DuelCmdRefreshPos(std::shared_ptr<FieldCard> ptr, bool update, float tm) {
//        pcard = ptr;
//        this->update = update;
//        this->update_tm = tm;
//    }
//    
//    bool DuelCmdRefreshPos::Handle(std::shared_ptr<DuelScene> pscene) {
//        pscene->RefreshPos(pcard, update, update_tm);
//        return true;
//    }
//    
//    DuelCmdMove::DuelCmdMove(std::shared_ptr<FieldCard> ptr, uint32_t code, int32_t con, int32_t loc, int32_t seq, int32_t subs, uint32_t reason) {
//        pcard = ptr;
//        this->code = code;
//        this->con = con;
//        this->loc = loc;
//        this->seq = seq;
//        this->subs = subs;
//        this->reason = reason;
//    }
//    
//    bool DuelCmdMove::Handle(std::shared_ptr<DuelScene> pscene) {
//        float tm = ((pcard->loc & 0x43) && (loc & 0x30)) ? 1.0f : 0.5f;
//        pscene->MoveCard(pcard, con, loc, seq, subs, false, tm);
//        return true;
//    }
//    
//    DuelCmdDraw::DuelCmdDraw(uint32_t pl, uint32_t data) {
//        this->playerid = pl;
//        this->data = data;
//    }
//    
//    bool DuelCmdDraw::Handle(std::shared_ptr<DuelScene> pscene) {
//        pscene->DrawCard(playerid, data);
//        return true;
//    }
//    
//}
