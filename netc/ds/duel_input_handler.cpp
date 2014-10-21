#include "../../common/common.h"

#include "../sungui.h"
#include "../card_data.h"
#include "../scene_mgr.h"

#include "duel_scene.h"
#include "duel_input_handler.h"

namespace ygopro
{
    
    DuelInputHandler::DuelInputHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
    }
    
    bool DuelInputHandler::UpdateInput() {
        return true;
    }
    
    void DuelInputHandler::MouseMove(sgui::MouseMoveEvent evt) {
        auto pscene = duel_scene.lock();
        auto scene_size = SceneMgr::Get().GetSceneSize();
        auto& vparam = pscene->GetViewParams();
        bool update_param = false;
        if(btnDown[0]) {
            float ratex = (float)(evt.x - btnPos[0].x) / scene_size.x * 2.0f;
            float ratey = (float)(evt.y - btnPos[0].y) / scene_size.y * 2.0f;
            vparam.xoffset += ratex;
            vparam.yoffset -= ratey;
            btnPos[0] = {evt.x, evt.y};
            update_param = true;
        }
        if(btnDown[1]) {
            float rate = (float)(evt.y - btnPos[1].y) / scene_size.y;
            vparam.angle += 3.1415926f * 0.5f * rate;
            if(vparam.angle < 0.0f)
                vparam.angle = 0.0f;
            if(vparam.angle > 3.1415926f * 0.5f)
                vparam.angle = 3.1415926f * 0.5f;
            btnPos[1] = {evt.x, evt.y};
            update_param = true;
        }
        if(update_param)
            pscene->UpdateParams();
        auto pblock = pre_block.lock();
        auto pcard = pre_card.lock();
        auto hp = pscene->GetHoverPos(evt.x, evt.y);
        std::shared_ptr<FieldBlock> hover_block = pscene->GetFieldBlock(hp.x, hp.y);
        std::shared_ptr<FieldCard> hover_card = pscene->GetFieldCard(hp.x, hp.y);
        if(pblock != hover_block) {
            if(pblock)
                pblock->hl.Reset(0.0f);
            if(hover_block) {
                hover_block->hl.SetAnimator(std::make_shared<LerpAnimator<float, TGenPeriodicRet>>(0.2f, 0.8f, SceneMgr::Get().GetGameTime(), 1.0));
                pscene->AddUpdateBlock(hover_block);
            }
            pre_block = hover_block;
        }
        if(pcard != hover_card) {
            if(pcard) {
                pcard->dy.SetAnimator(std::make_shared<LerpAnimator<float, TGenMove>>(pcard->dy.Get(), 0.0f, SceneMgr::Get().GetGameTime(), 0.5, 10));
                pscene->AddUpdateCard(pcard);
            }
            if(hover_card) {
                hover_card->dy.SetAnimator(std::make_shared<LerpAnimator<float, TGenMove>>(0.0f, 0.2f, SceneMgr::Get().GetGameTime(), 0.5, 10));
                pscene->AddUpdateCard(hover_card);
            }
            pre_card = hover_card;
        }
    }
    
    void DuelInputHandler::MouseButtonDown(sgui::MouseButtonEvent evt) {
        if(evt.button < 2) {
            btnDown[evt.button] = true;
            btnPos[evt.button] = {evt.x, evt.y};
        }
    }
    
    void DuelInputHandler::MouseButtonUp(sgui::MouseButtonEvent evt) {
        if(evt.button < 2) {
            btnDown[evt.button] = false;
        }
    }
    
    void DuelInputHandler::MouseWheel(sgui::MouseWheelEvent evt) {
        auto pscene = duel_scene.lock();
        auto& vparam = pscene->GetViewParams();
        vparam.radius += evt.deltay / 30.0f;
        if(vparam.radius < 1.0f)
            vparam.radius = 1.0f;
        if(vparam.radius > 50.0f)
            vparam.radius = 50.0f;
        pscene->UpdateParams();
    }
    
    void DuelInputHandler::KeyDown(sgui::KeyEvent evt) {

    }
    
    void DuelInputHandler::KeyUp(sgui::KeyEvent evt) {
        
    }
    
}
