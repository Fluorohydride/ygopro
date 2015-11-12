#include "utils/common.h"

#include "../config.h"
#include "../sgui.h"
#include "../card_data.h"
#include "build_scene.h"
#include "build_scene_handler.h"

namespace ygopro
{
    
    BuildSceneHandler::BuildSceneHandler(BuildScene* pscene) {
        build_scene = pscene;
        file_dialog = std::make_shared<FileDialog>();
        filter_dialog = std::make_shared<FilterDialog>();
        info_panel = std::make_shared<InfoPanel>();
        hover_pos = {CardLocation::Null, 0};
        click_pos = {CardLocation::Null, 0};
    }
    
    BuildSceneHandler::~BuildSceneHandler() {
    }
    
    void BuildSceneHandler::BeginHandler() {
        auto pnl_build = LoadDialogAs<sgui::SGPanel>("build panel");
        if(pnl_build) {
            auto res = pnl_build->FindWidget("deck name");
            deck_label = res ? res->CastPtr<sgui::SGLabel>() : nullptr;
            auto menu_deck = pnl_build->FindWidgetAs<sgui::SGTextButton>("menu deck");
            if(menu_deck) {
                menu_deck->event_click += [this](sgui::SGWidget& sender)->bool {
                    PopupMenu::Load("menu deck", SceneMgr::Get().GetMousePosition(), [this](int32_t id) {
                        OnMenuDeck(id);
                    });
                    return true;
                };
            }
            auto menu_tool = pnl_build->FindWidgetAs<sgui::SGTextButton>("menu tool");
            if(menu_tool) {
                menu_tool->event_click += [this](sgui::SGWidget& sender)->bool {
                    PopupMenu::Load("menu tool", SceneMgr::Get().GetMousePosition(), [this](int32_t id) {
                        OnMenuTool(id);
                    });
                    return true;
                };
            }
            auto menu_list = pnl_build->FindWidgetAs<sgui::SGTextButton>("menu list");
            if(menu_list) {
                menu_list->event_click += [this](sgui::SGWidget& sender)->bool {
                    PopupMenu::Load("menu list", SceneMgr::Get().GetMousePosition(), [this](int32_t id){
                        OnMenuList(id);
                    });
                    return true;
                };
            }
            auto limit_reg = pnl_build->FindWidgetAs<sgui::SGComboBox>("limit reg");
            if(limit_reg) {
                auto& lrs = LimitRegulationMgr::Get().GetLimitRegulations();
                for(uint32_t i = 0; i < lrs.size(); ++i)
                    limit_reg->AddItem(lrs[i].name, 0xff000000);
                limit_reg->SetSelection(0);
                limit_reg->event_sel_change += [this](sgui::SGWidget& sender, int32_t index)->bool {
                    auto v = view_regulation;
                    build_scene->ChangeRegulation(index, view_regulation);
                    view_regulation = v;
                    return true;
                };
            }
        }
        auto pnl_result = LoadDialogAs<sgui::SGPanel>("search panel");
        if(pnl_result) {
            auto menu_search = pnl_result->FindWidgetAs<sgui::SGTextButton>("menu search");
            if(menu_search) {
                menu_search->event_click += [this](sgui::SGWidget& sender)->bool {
                    filter_dialog->Show(SceneMgr::Get().GetMousePosition());
                    filter_dialog->SetOKCallback([this](const FilterCondition fc, int32_t lmt)->void {
                        Search(fc, lmt);
                    });
                    return true;
                };
            }
            auto res = pnl_result->FindWidget("result label");
            label_result = res ? res->CastPtr<sgui::SGLabel>() : nullptr;
            res = pnl_result->FindWidget("page label");
            label_page = res ? res->CastPtr<sgui::SGLabel>() : nullptr;
            auto btn_prev = pnl_result->FindWidgetAs<sgui::SGTextButton>("prev page");
            if(btn_prev) {
                btn_prev->event_click += [this](sgui::SGWidget& sender)->bool {
                    ResultPrevPage();
                    return true;
                };
            }
            auto btn_next = pnl_result->FindWidgetAs<sgui::SGTextButton>("next page");
            if(btn_next) {
                btn_next->event_click += [this](sgui::SGWidget& sender)->bool {
                    ResultNextPage();
                    return true;
                };
            }
        }
        sgui::SGGUIRoot::GetSingleton().event_key_down.Bind([this](sgui::SGWidget& sender, int32_t key, int32_t mods)->bool {
            KeyDown(key, mods);
            return true;
        }, this);
        v2i rc = sgui::SGJsonUtil::ConvertV2i(layoutCfg["search count"], 0);
        page_count = rc.x * rc.y;
    }
    
    void BuildSceneHandler::EndHandler() {
        sgui::SGGUIRoot::GetSingleton().event_key_down.Remove(this);
    }
    
    bool BuildSceneHandler::UpdateHandler() {
        if(show_info_time) {
            auto now = SceneMgr::Get().GetSysClock();
            if(now - show_info_time >= 500) {
                show_info_time = 0;
                auto mp = SceneMgr::Get().GetMousePosition();
                auto cur = build_scene->GetHoverPos(mp.x, mp.y);
                click_pos.first = CardLocation::Null;
                if(cur == hover_pos) {
                    auto ptr = build_scene->GetCard(hover_pos.first, hover_pos.second);
                    if(ptr)
                        info_panel->ShowInfo(ptr->data->code);
                }
            }
        }
        return true;
    }
    
    void BuildSceneHandler::MouseMove(int32_t x, int32_t y) {
        auto pre = hover_obj.lock();
        auto hov = build_scene->GetHoverPos(x, y);
        auto dcd = build_scene->GetCard(hov.first, hov.second);
        if(dcd != pre) {
            if(pre) {
                if(hover_pos.first == CardLocation::Result)
                    build_scene->HighlightCancel();
                pre->builder_card->SetHL(0);
                SceneMgr::Get().RemoveAction(pre->builder_card.get(), 1);
            }
            if(dcd) {
                auto ptr = dcd->builder_card;
                ptr->hl = 0x00ffffff;
                auto act = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(0, ptr, [](BuilderCard* bc, double t)->bool {
                    uint32_t alpha = (uint32_t)((t * 0.6f + 0.2f) * 255);
                    bc->SetHL((alpha << 24) | 0xffffff);
                    return true;
                }, std::make_shared<TGenPeriodicRet<int64_t>>(1000));
                SceneMgr::Get().PushAction(act, ptr.get(), 1);
                if(info_panel->IsOpen())
                    show_info_time = SceneMgr::Get().GetSysClock() - 200;
                if(hov.first == CardLocation::Result)
                    build_scene->HighlightCode(dcd->data->GetRawCode());
            }
        }
        hover_pos = hov;
        hover_obj = dcd;
    }
    
    void BuildSceneHandler::MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
        click_pos = hover_pos;
        if(button == GLFW_MOUSE_BUTTON_LEFT)
            show_info_time = SceneMgr::Get().GetSysClock();
    }
    
    void BuildSceneHandler::MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(button == GLFW_MOUSE_BUTTON_LEFT)
            show_info_time = 0;
        if(hover_pos != click_pos)
            return;
        click_pos.first = CardLocation::Null;
        CardLocation pos = hover_pos.first;
        int32_t index = hover_pos.second;
        if(pos < CardLocation::Result) {
            if(index < 0)
                return;
            if(button == GLFW_MOUSE_BUTTON_LEFT) {
                build_scene->MoveCard(pos, index);
                SetDeckEdited();
                MouseMove(SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y);
            } else {
                PopupMenu::Load("menu build popup", SceneMgr::Get().GetMousePosition(), [this, pos, index](int32_t id) {
                    switch(id) {
                        case 0:
                            build_scene->RemoveCard(pos, index);
                            SetDeckEdited();
                            break;
                        case 1: {
                            auto pcard = build_scene->GetCard(pos, index);
                            if(pcard) {
                                build_scene->InsertCardFromPos(pcard->data->code, pcard->builder_card->pos, pos == CardLocation::Side);
                                SetDeckEdited();
                                auto pt = SceneMgr::Get().GetMousePosition();
                                MouseMove(pt.x, pt.y);
                            }
                            break;
                        }
                    }
                });
            }
        } else if(pos == CardLocation::Result) {
            build_scene->InsertSearchResult(index, button != GLFW_MOUSE_BUTTON_LEFT);
        }
    }
    
    void BuildSceneHandler::MouseWheel(float deltax, float deltay) {
        
    }
    
    void BuildSceneHandler::KeyDown(int32_t key, int32_t mods) {
        switch(key) {
            case GLFW_KEY_1:
                if(mods & GLFW_MOD_CONTROL)
                    build_scene->SortDeck();
                else if(mods & GLFW_MOD_ALT)
                    ViewRegulation(0);
                break;
            case GLFW_KEY_2:
                if(mods & GLFW_MOD_CONTROL)
                    build_scene->ShuffleDeck();
                else if(mods & GLFW_MOD_ALT)
                    ViewRegulation(1);
                break;
            case GLFW_KEY_3:
                if(mods & GLFW_MOD_CONTROL) {
                    build_scene->ClearDeck();
                    SetDeckEdited();
                } else if(mods & GLFW_MOD_ALT)
                    ViewRegulation(2);
                break;
            case GLFW_KEY_C:
                if(mods & GLFW_MOD_CONTROL) {
                    std::string deck_string;
                    deck_string.append("ydk://").append(build_scene->SaveDeckToString());
                    glfwSetClipboardString(nullptr, deck_string.c_str());
                    MessageBox::ShowOK(L"", To<std::wstring>(stringCfg["eui_msg_deck_tostr_ok"].to_string()), nullptr);
                }
                break;
            case GLFW_KEY_V:
                if(mods & GLFW_MOD_CONTROL) {
                    std::string deck_string = glfwGetClipboardString(nullptr);
                    if(build_scene->LoadDeckFromString(deck_string)) {
                        SetDeckEdited();
                        StopViewRegulation();
                    }                    
                }
                break;
            default:
                break;
        }
    }
    
    void BuildSceneHandler::KeyUp(int32_t key, int32_t mods) {
        
    }
    
    void BuildSceneHandler::ViewRegulation(int32_t limit) {
        build_scene->ViewRegulation(limit);
        view_regulation = limit + 1;
        current_file.clear();
        deck_edited = false;
        std::wstring title = L"\ue07a";
        if(limit == 0)
            title.append(To<std::wstring>(stringCfg["eui_list_forbidden"].to_string()));
        else if(limit == 1)
            title.append(To<std::wstring>(stringCfg["eui_list_limit"].to_string()));
        else
            title.append(To<std::wstring>(stringCfg["eui_list_semilimit"].to_string()));
        SetDeckLabel(title, 0xff000000);
    }
    
    void BuildSceneHandler::SetDeckEdited() {
        if(!deck_edited) {
            if(current_file.length() == 0)
                SetDeckLabel(std::wstring(L"\ue08c").append(To<std::wstring>(stringCfg["eui_msg_new_deck"].to_string())).append(L"*"), 0xff000000);
            else
                SetDeckLabel(std::wstring(L"\ue08c").append(current_file).append(L"*"), 0xff000000);
            deck_edited = true;
        }
        view_regulation = 0;
    }
    
    void BuildSceneHandler::OnMenuDeck(int32_t id) {
        switch(id) {
            case 0:
                file_dialog->Show(To<std::wstring>(stringCfg["eui_msg_deck_load"].to_string()), To<std::wstring>(commonCfg["deck_path"].to_string()), L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck_file)->void {
                    if(deck_edited || deck_file != current_file) {
                        build_scene->LoadDeckFromFile(deck_file);
                        current_file = deck_file;
                        deck_edited = false;
                        view_regulation = 0;
                        SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                    }
                });
                break;
            case 1:
                if(current_file.length() == 0) {
                    file_dialog->Show(To<std::wstring>(stringCfg["eui_msg_deck_save"].to_string()), To<std::wstring>(commonCfg["deck_path"].to_string()), L".ydk");
                    file_dialog->SetOKCallback([this](const std::wstring& deck_file)->void {
                        if(build_scene->SaveDeckToFile(deck_file)) {
                            current_file = deck_file;
                            deck_edited = false;
                            SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                        }
                    });
                } else {
                    if(build_scene->SaveDeckToFile(current_file)) {
                        deck_edited = false;
                        SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                    }
                }
                break;
            case 2:
                file_dialog->Show(To<std::wstring>(stringCfg["eui_msg_deck_save"].to_string()), To<std::wstring>(commonCfg["deck_path"].to_string()), L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck_file)->void {
                    if(build_scene->SaveDeckToFile(deck_file)) {
                        current_file = deck_file;
                        deck_edited = false;
                        SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                    }
                });
                break;
            case 3: {
                std::string deck_string = glfwGetClipboardString(nullptr);
                build_scene->LoadDeckFromString(deck_string);
                view_regulation = 0;
            }
                break;
            case 4: {
                std::string deck_string;
                deck_string.append("ydk://").append(build_scene->SaveDeckToString());
                glfwSetClipboardString(nullptr, deck_string.c_str());
                MessageBox::ShowOK(L"", To<std::wstring>(stringCfg["eui_msg_deck_tostr_ok"].to_string()), nullptr);
            }
                break;
            default:
                break;
        }
    }
    
    void BuildSceneHandler::OnMenuTool(int32_t id) {
        switch(id) {
            case 0:
                build_scene->SortDeck();
                break;
            case 1:
                build_scene->ShuffleDeck();
                break;
            case 2:
                build_scene->ClearDeck();
                SetDeckEdited();
                break;
            case 3: {
                std::wstring neturl = To<std::wstring>(commonCfg["deck_neturl"].to_string());
                std::wstring deck_string = To<std::wstring>(build_scene->SaveDeckToString());
                auto ntpos = neturl.find(L"{deck}");
                if(ntpos != std::wstring::npos)
                    neturl.replace(ntpos, 6, deck_string);
                ntpos = neturl.find(L"{name}");
                if(ntpos != std::wstring::npos) {
                    auto pos = current_file.find_last_of(L'/');
                    if(pos == std::wstring::npos)
                        neturl.replace(ntpos, 6, L"");
                    else
                        neturl.replace(ntpos, 6, current_file, pos + 1, std::wstring::npos);
                }
#ifdef _WIN32
                std::string url = "start \"\" \"";
#else
                std::string url = "open \"";
#endif
                url.append(To<std::string>(neturl)).append("\"");
                std::system(url.c_str());
                break;
            }
            default:
                break;
        }
    }
    
    void BuildSceneHandler::OnMenuList(int32_t id) {
        switch(id) {
            case 0:
                ViewRegulation(0);
                break;
            case 1:
                ViewRegulation(1);
                break;
            case 2:
                ViewRegulation(2);
                break;
            default:
                break;
        }
    }
    
    void BuildSceneHandler::SetDeckLabel(const std::wstring& str, uint32_t cl) {
        if(!deck_label.expired())
            deck_label.lock()->GetTextUI()->SetText(str, 0xff000000);
    }
    
    void BuildSceneHandler::ResultPrevPage() {
        if(result_page == 0)
            return;
        result_page--;
        std::vector<CardData*> new_results;
        new_results.resize(page_count);
        for(int32_t i = 0; i < page_count; ++i) {
            if((size_t)(result_page * page_count + i) < search_result.size())
                new_results[i] = search_result[result_page * page_count + i];
            else
                new_results[i] = nullptr;
        }
        build_scene->UpdateResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / page_count) + 1;
            std::wstring s = To<std::wstring>(L"%d/%d", result_page + 1, pageall);
            ptr->GetTextUI()->SetText(s, 0xff000000);
        }
    }
    
    void BuildSceneHandler::ResultNextPage() {
        if((size_t)(result_page * page_count + page_count) >= search_result.size())
            return;
        result_page++;
        std::vector<CardData*> new_results;
        new_results.resize(page_count);
        for(int32_t i = 0; i < page_count; ++i) {
            if((size_t)(result_page * page_count + i) < search_result.size())
                new_results[i] = search_result[result_page * page_count + i];
            else
                new_results[i] = nullptr;
        }
        build_scene->UpdateResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / page_count) + 1;
            std::wstring s = To<std::wstring>(L"%d/%d", result_page + 1, pageall);
            ptr->GetTextUI()->SetText(s, 0xff000000);
        }
    }
    
    void BuildSceneHandler::Search(const FilterCondition& fc, int32_t lmt) {
        if(lmt == 0)
            search_result = DataMgr::Get().FilterCard(fc);
        else
            search_result = LimitRegulationMgr::Get().FilterCard(lmt - 1, fc);
        std::sort(search_result.begin(), search_result.end(), CardData::card_sort);
        result_page = 0;
        std::vector<CardData*> new_results;
        new_results.resize(page_count);
        for(int32_t i = 0; i < page_count; ++i) {
            if((size_t)(i) < search_result.size())
                new_results[i] = search_result[i];
            else
                new_results[i] = nullptr;
        }
        build_scene->UpdateResult(new_results);
        auto ptr = label_result.lock();
        if(ptr != nullptr) {
            std::wstring s = To<std::wstring>(stringCfg["eui_filter_count"].to_string());
            std::wstring ct = To<std::wstring>(L"%ld", search_result.size());
            size_t pos = s.find(L"{count}");
            if(pos != std::wstring::npos)
                s.replace(pos, 7, ct);
            ptr->GetTextUI()->SetText(s, 0xff000000);
        }
        auto ptr2 = label_page.lock();
        if(ptr2 != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / page_count) + 1;
            std::wstring s = To<std::wstring>(L"%d/%d", result_page + 1, pageall);
            ptr2->GetTextUI()->SetText(s, 0xff000000);
        }
    }
    
}
