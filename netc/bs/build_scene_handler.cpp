#include "utils/common.h"

#include "../config.h"
#include "../sgui.h"
#include "../card_data.h"
#include "build_scene.h"
#include "build_scene_handler.h"
#include "build_input_handler.h"

namespace ygopro
{
    
    BuildSceneHandler::BuildSceneHandler(std::shared_ptr<BuildScene> pscene) {
        build_scene = pscene;
        file_dialog = std::make_shared<FileDialog>();
        filter_dialog = std::make_shared<FilterDialog>();
        info_panel = std::make_shared<InfoPanel>();
    }
    
    bool BuildSceneHandler::UpdateEvent() {
        UpdateActionTime(SceneMgr::Get().GetSysClock());
        return true;
    }
    
    void BuildSceneHandler::BeginHandler() {
        InitActionTime(SceneMgr::Get().GetSysClock());
        auto pnl_build = LoadDialogAs<sgui::SGPanel>("build panel");
        if(pnl_build) {
            pnl_build->event_key_down += [this](sgui::SGWidget& sender, int32_t key, int32_t mods)->bool {
                build_scene.lock()->GetInputHandler()->KeyDown(key, mods);
                return true;
            };
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
            auto menu_search = pnl_build->FindWidgetAs<sgui::SGTextButton>("menu search");
            if(menu_search) {
                menu_search->event_click += [this](sgui::SGWidget& sender)->bool {
                    filter_dialog->Show(SceneMgr::Get().GetMousePosition());
                    filter_dialog->SetOKCallback([this](const FilterCondition fc, int32_t lmt)->void {
                        Search(fc, lmt);
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
                    build_scene.lock()->ChangeRegulation(index, view_regulation);
                    view_regulation = v;
                    return true;
                };
            }
        }
        auto pnl_result = LoadDialogAs<sgui::SGPanel>("result panel");
        if(pnl_result) {
            pnl_result->event_key_down += [this](sgui::SGWidget& sender, int32_t key, int32_t mods)->bool {
                build_scene.lock()->GetInputHandler()->KeyDown(key, mods);
                return true;
            };
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
    }
    
    void BuildSceneHandler::ViewRegulation(int32_t limit) {
        build_scene.lock()->ViewRegulation(limit);
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
                        build_scene.lock()->LoadDeckFromFile(deck_file);
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
                        if(build_scene.lock()->SaveDeckToFile(deck_file)) {
                            current_file = deck_file;
                            deck_edited = false;
                            SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                        }
                    });
                } else {
                    if(build_scene.lock()->SaveDeckToFile(current_file)) {
                        deck_edited = false;
                        SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                    }
                }
                break;
            case 2:
                file_dialog->Show(To<std::wstring>(stringCfg["eui_msg_deck_save"].to_string()), To<std::wstring>(commonCfg["deck_path"].to_string()), L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck_file)->void {
                    if(build_scene.lock()->SaveDeckToFile(deck_file)) {
                        current_file = deck_file;
                        deck_edited = false;
                        SetDeckLabel(std::wstring(L"\ue08c").append(current_file), 0xff000000);
                    }
                });
                break;
            case 3: {
                std::string deck_string = glfwGetClipboardString(nullptr);
                build_scene.lock()->LoadDeckFromString(deck_string);
                view_regulation = 0;
            }
                break;
            case 4: {
                std::string deck_string;
                deck_string.append("ydk://").append(build_scene.lock()->SaveDeckToString());
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
                build_scene.lock()->SortDeck();
                break;
            case 1:
                build_scene.lock()->ShuffleDeck();
                break;
            case 2:
                build_scene.lock()->ClearDeck();
                build_scene.lock()->SetDeckDirty();
                break;
            case 3: {
                std::wstring neturl = To<std::wstring>(commonCfg["deck_neturl"].to_string());
                std::wstring deck_string = To<std::wstring>(build_scene.lock()->SaveDeckToString());
                auto ntpos = neturl.find(L"{amp}");
                while(ntpos != std::wstring::npos) {
                    neturl.replace(ntpos, 5, L"&");
                    ntpos = neturl.find(L"{amp}");
                }
                ntpos = neturl.find(L"{deck}");
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
    
    void BuildSceneHandler::ShowCardInfo(uint32_t code) {
        int32_t w = 700;
        int32_t h = 320;
        auto scene_size = SceneMgr::Get().GetSceneSize();
        info_panel->ShowInfo(code, {scene_size.x / 2 - w / 2, scene_size.y / 2 - h / 2});
    }
    
    void BuildSceneHandler::HideCardInfo() {
        info_panel->Destroy();
    }
    
    void BuildSceneHandler::SetDeckLabel(const std::wstring& str, uint32_t cl) {
        if(!deck_label.expired())
            deck_label.lock()->GetTextUI()->SetText(str, 0xff000000);
    }
    
    void BuildSceneHandler::ResultPrevPage() {
        if(result_page == 0)
            return;
        result_page--;
        std::array<CardData*, 10> new_results;
        for(int32_t i = 0; i < 10; ++i) {
            if((size_t)(result_page * 10 + i) < search_result.size())
                new_results[i] = search_result[result_page * 10 + i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / 10) + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr->GetTextUI()->SetText(s, 0xff000000);
        }
    }
    
    void BuildSceneHandler::ResultNextPage() {
        if((size_t)(result_page * 10 + 10) >= search_result.size())
            return;
        result_page++;
        std::array<CardData*, 10> new_results;
        for(int32_t i = 0; i < 10; ++i) {
            if((size_t)(result_page * 10 + i) < search_result.size())
                new_results[i] = search_result[result_page * 10 + i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / 10) + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
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
        std::array<CardData*, 10> new_results;
        for(int32_t i = 0; i < 10; ++i) {
            if((size_t)(i) < search_result.size())
                new_results[i] = search_result[i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_result.lock();
        if(ptr != nullptr) {
            std::wstring s = To<std::wstring>(stringCfg["eui_filter_count"].to_string());
            std::wstring ct = To<std::wstring>(To<std::string>("%ld", search_result.size()));
            size_t pos = s.find(L"{count}");
            if(pos != std::wstring::npos)
                s.replace(pos, 7, ct);
            ptr->GetTextUI()->SetText(s, 0xff000000);
        }
        auto ptr2 = label_page.lock();
        if(ptr2 != nullptr) {
            int32_t pageall = (search_result.size() == 0) ? 0 : (int32_t)((search_result.size() - 1) / 10) + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr2->GetTextUI()->SetText(s, 0xff000000);
        }
    }
    
}
