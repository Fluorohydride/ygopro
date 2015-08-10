#include "utils/common.h"

#include "../sgui.h"
#include "../card_data.h"
#include "../scene_mgr.h"
#include "../gui_extra.h"
#include "build_scene.h"
#include "build_scene_handler.h"
#include "build_input_handler.h"

namespace ygopro
{
    
    BuildInputHandler::BuildInputHandler(std::shared_ptr<BuildScene> pscene) {
        build_scene = pscene;
        hover_pos = std::make_pair(0, 0);
        click_pos = std::make_pair(0, 0);
    }
    
    bool BuildInputHandler::UpdateInput() {
        if(show_info_begin) {
            auto pscene = build_scene.lock();
            auto now = SceneMgr::GetSingleton().GetSysClock();
            if(now - show_info_time >= 500) {
                show_info = true;
                show_info_begin = false;
                click_pos.first = 0;
                pscene->ShowSelectedInfo(hover_pos.first, hover_pos.second);
                sgui::SGGUIRoot::GetSingleton().event_mouse_up.Bind([this](sgui::SGWidget& sender, int32_t btn, int32_t mods, int32_t x, int32_t y)->bool {
                    if(btn == GLFW_MOUSE_BUTTON_LEFT) {
                        show_info = false;
                        show_info_begin = false;
                        build_scene.lock()->HideCardInfo();
                        sgui::SGGUIRoot::GetSingleton().event_mouse_move.Remove(this);
                        sgui::SGGUIRoot::GetSingleton().event_mouse_up.Remove(this);
                    }
                    return true;
                }, this);
                sgui::SGGUIRoot::GetSingleton().event_mouse_move.Bind([this](sgui::SGWidget& sender, int32_t x, int32_t y)->bool {
                    MouseMove(x, y);
                    return true;
                }, this);
            }
        }
        return true;
    }
    
    void BuildInputHandler::MouseMove(int32_t x, int32_t y) {
        std::shared_ptr<DeckCardData> dcd = nullptr;
        auto pscene = build_scene.lock();
        auto pre = hover_obj.lock();
        auto hov = pscene->GetHoverPos(x, y);
        if(hov.first == 4) {
            pscene->SetCurrentSelection(hov.second, show_info);
        } else {
            dcd = pscene->GetCard(hov.first, hov.second);
            pscene->SetCurrentSelection(-1, false);
            if(dcd && show_info)
                pscene->ShowCardInfo(dcd->data->code);
        }
        if(dcd != pre) {
            if(pre) {
                auto ptr = std::static_pointer_cast<BuilderCard>(pre->extra);
                ptr->hl.Reset(0.0f);
            }
            if(dcd) {
                auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
                ptr->hl.SetAnimator(std::make_shared<LerpAnimator<float, TGenPeriodicRet>>(0.2f, 0.8f, SceneMgr::Get().GetGameTime(), 1.0));
                pscene->AddUpdatingCard(dcd);
            }
        }
        hover_pos = hov;
        hover_obj = dcd;
    }
    
    void BuildInputHandler::MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
        click_pos = hover_pos;
        if(button == GLFW_MOUSE_BUTTON_LEFT) {
            show_info_begin = true;
            show_info_time = SceneMgr::Get().GetGameTime();
        }
    }
    
    void BuildInputHandler::MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT)
            show_info_begin = false;
        if(hover_pos != click_pos)
            return;
        auto pscene = build_scene.lock();
        click_pos.first = 0;
        int32_t pos = hover_pos.first;
        int32_t index = hover_pos.second;
        if(pos > 0 && pos < 4) {
            if(index < 0)
                return;
            if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                pscene->MoveCard(pos, index);
            } else {
                pscene->RemoveCard(pos, index);
            }
        } else if(pos == 4) {
            pscene->InsertSearchResult(index, evt.button != GLFW_MOUSE_BUTTON_LEFT);
        }
    }
    
    void BuildInputHandler::MouseWheel(float deltax, float deltay) {
        
    }
    
    void BuildInputHandler::KeyDown(int32_t key, int32_t mods) {
        auto pscene = build_scene.lock();
        switch(evt.key) {
            case GLFW_KEY_1:
                if(evt.mods & GLFW_MOD_CONTROL)
                    pscene->SortDeck();
                else if(evt.mods & GLFW_MOD_ALT)
                    pscene->GetSceneHandler<BuildSceneHandler>()->ViewRegulation(0);
                break;
            case GLFW_KEY_2:
                if(evt.mods & GLFW_MOD_CONTROL)
                    pscene->ShuffleDeck();
                else if(evt.mods & GLFW_MOD_ALT)
                    pscene->GetSceneHandler<BuildSceneHandler>()->ViewRegulation(1);
                break;
            case GLFW_KEY_3:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    pscene->ClearDeck();
                    pscene->SetDeckDirty();
                } else if(evt.mods & GLFW_MOD_ALT)
                    pscene->GetSceneHandler<BuildSceneHandler>()->ViewRegulation(2);
                break;
            case GLFW_KEY_C:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    std::string deck_string;
                    deck_string.append("ydk://").append(build_scene.lock()->SaveDeckToString());
                    glfwSetClipboardString(nullptr, deck_string.c_str());
                    MessageBox::ShowOK(L"", stringCfg["eui_msg_deck_tostr_ok"]);
                }
                break;
            case GLFW_KEY_V:
                if(evt.mods & GLFW_MOD_CONTROL) {
                    auto pscene = build_scene.lock();
                    std::string deck_string = glfwGetClipboardString(nullptr);
                    pscene->LoadDeckFromString(deck_string);
                    pscene->GetSceneHandler<BuildSceneHandler>()->StopViewRegulation();
                }
                break;
            default:
                break;
        }
    }
    
    void BuildInputHandler::KeyUp(int32_t key, int32_t mods) {
        
    }
    
}
