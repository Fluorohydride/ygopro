#include "utils/common.h"

#include "../sgui.h"
#include "../gui_extra.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    DuelSceneHandler::DuelSceneHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
        info_panel = std::make_shared<InfoPanel>();
    }
    
    void DuelSceneHandler::BeginHandler() {
//        proto_handler->BeginProto();
    }
    
    bool DuelSceneHandler::UpdateHandler() {
//        auto pscene = duel_scene.lock();
//        if(cur_commands.IsEmpty()) {
//            auto cmd = proto_handler->GetCommand();
//            if(cmd)
//                cur_commands.PushCommand(cmd);
//        } else {
//            while(!cur_commands.IsEmpty()) {
//                auto cmd = cur_commands.PullCommand();
//                bool finished = cmd->Handle(pscene);
//                if(finished)
//                    cur_commands.PopCommand();
//                else
//                    break;
//            }
//            if(cur_commands.IsEmpty())
//                proto_handler->EndCommand();
//        }
        if(show_info_time) {
            auto pscene = duel_scene.lock();
            auto now = SceneMgr::Get().GetSysClock();
            if(now - show_info_time >= 500) {
                show_info_time = 0;
                auto mp = SceneMgr::Get().GetMousePosition();
                auto cpos = pscene->GetHoverCardPos(pscene->GetHoverPos(mp.x, mp.y));
                auto ptr = pscene->GetHoverCard(cpos.x, cpos.y, cpos.z);
                if(ptr && ptr->code)
                    info_panel->ShowInfo(ptr->code);
            }
        }
        return true;
    }

    void DuelSceneHandler::MouseMove(int32_t x, int32_t y) {
        auto pscene = duel_scene.lock();
        auto scene_size = SceneMgr::Get().GetSceneSize();
        if(btnDown[0]) {
            float ratex = (float)(x - btnPos[0].x) / scene_size.x * 2.0f;
            float ratey = (float)(y - btnPos[0].y) / scene_size.y * 2.0f;
            btnPos[0] = {x, y};
            pscene->UpdateViewOffset({ratex, ratey});
            show_info_time = 0;
        }
        if(btnDown[1]) {
            float rate = (float)(y - btnPos[1].y) / scene_size.y * 3.1415926f * 0.5f;
            btnPos[1] = {x, y};
            pscene->UpdateViewAngle(rate);
        }
        auto pblock = pre_block.lock();
        auto pcard = pre_card.lock();
        auto hp = pscene->GetHoverPos(x, y);
        std::shared_ptr<FieldBlock> hover_block = pscene->GetFieldBlock(hp.x, hp.y);
        auto cpos = pscene->GetHoverCardPos(pscene->GetHoverPos(x, y));
        auto hover_card = pscene->GetHoverCard(cpos.x, cpos.y, cpos.z);
        if(pblock != hover_block) {
            if(pblock) {
                pblock->SetHL(0);
                SceneMgr::Get().RemoveAction(pblock.get());
            }
            if(hover_block) {
                auto act = std::make_shared<LerpAnimator<int64_t, FieldBlock>>(0, hover_block, [](FieldBlock* fb, double t)->bool {
                    uint32_t alpha = (uint32_t)((t * 0.6f + 0.2f) * 255);
                    fb->SetHL((alpha << 24) | 0xffffff);
                    return true;
                }, std::make_shared<TGenPeriodicRet<int64_t>>(1000));
                SceneMgr::Get().PushAction(act, hover_block.get());
            }
            pre_block = hover_block;
        }
        if(pcard != hover_card) {
            if(pcard && pcard->card_loc == 0x2) {
                auto curoff = pcard->yoffset;
                auto act = std::make_shared<LerpAnimator<int64_t, FieldCard>>(0, pcard, [curoff](FieldCard* fb, double t)->bool {
                    fb->SetYOffset(curoff * (1.0 - t));
                    return true;
                }, std::make_shared<TGenMove<int64_t>>(curoff / 0.2f * 500, 0.01));
                SceneMgr::Get().RemoveAction(pcard.get(), 2);
                SceneMgr::Get().PushAction(act, pcard.get(), 2);
            }
            if(hover_card && hover_card->card_loc == 0x2) {
                auto act = std::make_shared<LerpAnimator<int64_t, FieldCard>>(0, hover_card, [](FieldCard* fb, double t)->bool {
                    fb->SetYOffset(0.2f * t);
                    return true;
                }, std::make_shared<TGenMove<int64_t>>(500, 0.01));
                SceneMgr::Get().PushAction(act, hover_card.get(), 2);
            }
            if(hover_card && hover_card->code && info_panel->IsOpen())
                show_info_time = SceneMgr::Get().GetSysClock() - 200;
            pre_card = hover_card;
        }
    }

    void DuelSceneHandler::MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(button < 2) {
            btnDown[button] = true;
            btnPos[button] = {x, y};
            if(button == 0)
                show_info_time = SceneMgr::Get().GetSysClock();
        }
    }

    void DuelSceneHandler::MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(button < 2) {
            btnDown[button] = false;
            if(button == 0)
                show_info_time = 0;
        }
    }

    void DuelSceneHandler::MouseWheel(float deltax, float deltay) {
        auto pscene = duel_scene.lock();
        pscene->UpdateViewRadius(deltay / 30.0f);
        pscene->UpdateParams();
    }
    
    void DuelSceneHandler::KeyDown(int32_t key, int32_t mods) {

    }
    
    void DuelSceneHandler::KeyUp(int32_t key, int32_t mods) {
        
    }
    
}