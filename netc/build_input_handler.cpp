#include "../common/common.h"

#include "sungui.h"
#include "scene_mgr.h"
#include "build_scene.h"
#include "build_input_handler.h"

namespace ygopro
{
    
    BuildInputHandler::BuildInputHandler(std::shared_ptr<BuildScene> pscene) {
        build_scene = pscene;
    }
    
    void BuildInputHandler::Update() {
        if(show_info_begin) {
            auto pscene = build_cene.lock();
            double now = SceneMgr::Get().GetGameTime();
            if(now - show_info_time >= 0.5) {
                show_info = true;
                show_info_begin = false;
                click_pos.first = 0;
                auto pos = hover_pos.first;
                if(pos > 0 && pos < 4) {
                    auto dcd = pscene->GetCard(pos, hover_pos.second);
                    if(dcd != nullptr)
                        pscene->ShowCardInfo(dcd->data->code);
                } else if(pos == 4) {
                    auto index = hover_pos.second;
                    if((size_t)(result_page * 10 + index) < search_result.size())
                        pscene->ShowCardInfo(search_result[result_page * 10 + index]->code);
                }
                sgui::SGGUIRoot::GetSingleton().eventMouseButtonUp.Bind([this](sgui::SGWidget& sender, sgui::MouseButtonEvent evt)->bool {
                    if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                        show_info = false;
                        show_info_begin = false;
                        info_panel->Destroy();
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
    }
    
    void BuildInputHandler::MouseMove(sgui::MouseMoveEvent evt) {
        std::shared_ptr<DeckCardData> dcd = nullptr;
        auto pscene = build_cene.lock();
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
        auto pscene = build_cene.lock();
        click_pos.first = 0;
        int pos = hover_pos.first;
        if(pos > 0 && pos < 4) {
            int index = hover_pos.second;
            if(index < 0)
                return;
            auto dcd = pscene->GetCard(pos, index);
            if(dcd == nullptr)
                return;
            if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
                if(pos == 1) {
                    auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.main_deck[index]->extra);
                    ptr->hl.Reset(0.0f);
                    current_deck.side_deck.push_back(current_deck.main_deck[index]);
                    current_deck.main_deck.erase(current_deck.main_deck.begin() + index);
                } else if(pos == 2) {
                    auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[index]->extra);
                    ptr->hl.Reset(0.0f);
                    current_deck.side_deck.push_back(current_deck.extra_deck[index]);
                    current_deck.extra_deck.erase(current_deck.extra_deck.begin() + index);
                } else if(dcd->data->type & 0x802040) {
                    auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[index]->extra);
                    ptr->hl.Reset(0.0f);
                    current_deck.extra_deck.push_back(current_deck.side_deck[index]);
                    current_deck.side_deck.erase(current_deck.side_deck.begin() + index);
                } else {
                    auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[index]->extra);
                    ptr->hl.Reset(0.0f);
                    current_deck.main_deck.push_back(current_deck.side_deck[index]);
                    current_deck.side_deck.erase(current_deck.side_deck.begin() + index);
                }
                current_deck.CalCount();
                RefreshParams();
                RefreshAllIndex();
                UpdateAllCard();
                SetDeckDirty();
                MouseMove({SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y});
            } else {
                if(update_status == 1)
                    return;
                update_status = 1;
                unsigned int code = dcd->data->code;
                auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
                v2f dst = ptr->pos.Get() + v2f{card_size.x / 2, -card_size.y / 2};
                v2f dsz = {0.0f, 0.0f};
                ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->pos.Get(), dst, SceneMgr::Get().GetGameTime(), 0.2));
                ptr->size.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->size.Get(), dsz, SceneMgr::Get().GetGameTime(), 0.2));
                if(!ptr->updateing) {
                    ptr->updateing = true;
                    updating_cards.push_back(dcd);
                }
                ptr->show_limit = false;
                ptr->show_exclusive = false;
                build_timer.RegisterEvent([pos, index, code, this]() {
                    if(current_deck.RemoveCard(pos, index)) {
                        ImageMgr::Get().UnloadCardTexture(code);
                        RefreshParams();
                        RefreshAllIndex();
                        UpdateAllCard();
                        SetDeckDirty();
                        MouseMove({SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y});
                        update_status = 0;
                    }
                }, 0.2, 0, false);
            }
        } else if(pos == 4) {
            int index = hover_pos.second;
            if((size_t)(result_page * 10 + index) >= search_result.size())
                return;
            auto data = search_result[result_page * 10 + index];
            std::shared_ptr<DeckCardData> ptr;
            if(evt.button == GLFW_MOUSE_BUTTON_LEFT)
                ptr = current_deck.InsertCard(1, -1, data->code, true);
            else
                ptr = current_deck.InsertCard(3, -1, data->code, true);
            if(ptr != nullptr) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(data->code);
                exdata->show_exclusive = show_exclusive;
                auto mpos = SceneMgr::Get().GetMousePosition();
                exdata->pos = (v2f){(float)mpos.x / scene_size.x * 2.0f - 1.0f, 1.0f - (float)mpos.y / scene_size.y * 2.0f};
                exdata->size = card_size;
                exdata->hl = 0.0f;
                ptr->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
                RefreshParams();
                RefreshAllIndex();
                UpdateAllCard();
                SetDeckDirty();
            }
        }
    }
    
    void BuildInputHandler::MouseWheel(sgui::MouseWheelEvent evt) {
        
    }
    
    void BuildInputHandler::KeyDown(sgui::KeyEvent evt) {
        auto pscene = build_cene.lock();
        switch(evt.key) {
            case GLFW_KEY_1:
                if(evt.mods & GLFW_MOD_ALT)
                    pscene->ViewRegulation(0);
                break;
            case GLFW_KEY_2:
                if(evt.mods & GLFW_MOD_ALT)
                    pscene->ViewRegulation(1);
                break;
            case GLFW_KEY_3:
                if(evt.mods & GLFW_MOD_ALT)
                    pscene->ViewRegulation(2);
                break;
            case GLFW_KEY_C:
                if(evt.mods & GLFW_MOD_CONTROL)
                    pscene->SaveDeckToClipboard();
                break;
            case GLFW_KEY_V:
                if(evt.mods & GLFW_MOD_CONTROL)
                    pscene->LoadDeckFromClipboard();
                break;
            default:
                break;
        }
    }
    
    void BuildInputHandler::KeyUp(sgui::KeyEvent evt) {
        
    }
    
}
