#include "../../common/common.h"

#include "../sungui.h"
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
            double now = SceneMgr::Get().GetGameTime();
            if(now - show_info_time >= 0.5) {
                show_info = true;
                show_info_begin = false;
                click_pos.first = 0;
                pscene->ShowSelectedInfo(hover_pos.first, hover_pos.second);
                sgui::SGGUIRoot::GetSingleton().eventMouseButtonUp.Bind([this](sgui::SGWidget& sender, sgui::MouseButtonEvent evt)->bool {
                    if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                        show_info = false;
                        show_info_begin = false;
                        build_scene.lock()->HideCardInfo();
                        sgui::SGGUIRoot::GetSingleton().eventMouseMove.Reset();
                        sgui::SGGUIRoot::GetSingleton().eventMouseButtonUp.Reset();
                    }
                    return true;
                });
                sgui::SGGUIRoot::GetSingleton().eventMouseMove.Bind([this](sgui::SGWidget& sender, sgui::MouseMoveEvent evt)->bool {
                    MouseMove(evt);
                    return true;
                });
            }
        }
        return true;
    }
    
    void BuildInputHandler::MouseMove(sgui::MouseMoveEvent evt) {
        std::shared_ptr<DeckCardData> dcd = nullptr;
        auto pscene = build_scene.lock();
        auto pre = hover_obj.lock();
        auto hov = pscene->GetHoverPos(evt.x, evt.y);
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
    
    void BuildInputHandler::MouseButtonDown(sgui::MouseButtonEvent evt) {
        click_pos = hover_pos;
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
            show_info_begin = true;
            show_info_time = SceneMgr::Get().GetGameTime();
        }
    }
    
    void BuildInputHandler::MouseButtonUp(sgui::MouseButtonEvent evt) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT)
            show_info_begin = false;
        if(hover_pos != click_pos)
            return;
        auto pscene = build_scene.lock();
        click_pos.first = 0;
        int pos = hover_pos.first;
        int index = hover_pos.second;
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
    
    void BuildInputHandler::MouseWheel(sgui::MouseWheelEvent evt) {
        
    }
    
    void BuildInputHandler::KeyDown(sgui::KeyEvent evt) {
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
    
    void BuildInputHandler::KeyUp(sgui::KeyEvent evt) {
        
    }
    
}
