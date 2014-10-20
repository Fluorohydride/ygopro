#include "../../common/common.h"

#include "../sungui.h"
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
        return true;
    }
    
    void BuildSceneHandler::BeginHandler() {
        auto pnl = sgui::SGPanel::Create(nullptr, {10, 5}, {0, 35});
        pnl->SetSize({-20, 35}, {1.0f, 0.0f});
        pnl->eventKeyDown.Bind([this](sgui::SGWidget& sender, sgui::KeyEvent evt)->bool {
            build_scene.lock()->GetInputHandler()->KeyDown(evt);
            return true;
        });
        auto rpnl = sgui::SGPanel::Create(nullptr, {0, 0}, {200, 60});
        rpnl->SetPosition({0, 40}, {0.795f, 0.0f});
        rpnl->SetSize({-10, 60}, {0.205f, 0.0f});
        rpnl->eventKeyDown.Bind([this](sgui::SGWidget& sender, sgui::KeyEvent evt)->bool {
            build_scene.lock()->GetInputHandler()->KeyDown(evt);
            return true;
        });
        auto icon_label = sgui::SGIconLabel::Create(pnl, {10, 7}, std::wstring(L"\ue08c").append(stringCfg["eui_msg_new_deck"]));
        deck_label = icon_label;
        auto menu_deck = sgui::SGButton::Create(pnl, {250, 5}, {70, 25});
        menu_deck->SetText(stringCfg["eui_menu_deck"], 0xff000000);
        menu_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            PopupMenu::Begin(SceneMgr::Get().GetMousePosition(), 100, [this](int id){
                OnMenuDeck(id);
            })
            .AddButton(stringCfg["eui_deck_load"])
            .AddButton(stringCfg["eui_deck_save"])
            .AddButton(stringCfg["eui_deck_saveas"])
            .AddButton(stringCfg["eui_deck_loadstr"])
            .AddButton(stringCfg["eui_deck_savestr"])
            .End();
            return true;
        });
        auto menu_tool = sgui::SGButton::Create(pnl, {325, 5}, {70, 25});
        menu_tool->SetText(stringCfg["eui_menu_tool"], 0xff000000);
        menu_tool->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            PopupMenu::Begin(SceneMgr::Get().GetMousePosition(), 100, [this](int id){
                OnMenuTool(id);
            })
            .AddButton(stringCfg["eui_tool_sort"])
            .AddButton(stringCfg["eui_tool_shuffle"])
            .AddButton(stringCfg["eui_tool_clear"])
            .AddButton(stringCfg["eui_tool_browser"])
            .End();
            return true;
        });
        auto menu_list = sgui::SGButton::Create(pnl, {400, 5}, {70, 25});
        menu_list->SetText(stringCfg["eui_menu_list"], 0xff000000);
        menu_list->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            PopupMenu::Begin(SceneMgr::Get().GetMousePosition(), 100, [this](int id){
                OnMenuList(id);
            })
            .AddButton(stringCfg["eui_list_forbidden"])
            .AddButton(stringCfg["eui_list_limit"])
            .AddButton(stringCfg["eui_list_semilimit"])
            .End();
            return true;
        });
        auto menu_search = sgui::SGButton::Create(pnl, {475, 5}, {70, 25});
        menu_search->SetText(stringCfg["eui_menu_search"], 0xff000000);
        menu_search->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            filter_dialog->Show(SceneMgr::Get().GetMousePosition());
            filter_dialog->SetOKCallback([this](const FilterCondition fc, int lmt)->void {
                Search(fc, lmt);
            });
            return true;
        });
        auto limit_reg = sgui::SGComboBox::Create(pnl, {550, 2}, {150, 30});
        auto& lrs = LimitRegulationMgr::Get().GetLimitRegulations();
        for(unsigned int i = 0; i < lrs.size(); ++i)
            limit_reg->AddItem(lrs[i].name, 0xff000000);
        limit_reg->SetSelection(0);
        limit_reg->eventSelChange.Bind([this](sgui::SGWidget& sender, int index)->bool {
            auto v = view_regulation;
            build_scene.lock()->ChangeRegulation(index, view_regulation);
            view_regulation = v;
            return true;
        });
        auto show_ex = sgui::SGCheckbox::Create(pnl, {710, 7}, {100, 30});
        show_ex->SetText(stringCfg["eui_show_exclusive"], 0xff000000);
        show_ex->SetChecked(true);
        show_ex->eventCheckChange.Bind([this](sgui::SGWidget& sender, bool check)->bool {
            build_scene.lock()->ChangeExclusive(check);
            return true;
        });
        auto lblres = sgui::SGLabel::Create(rpnl, {0, 10}, L"");
        lblres->SetPosition({0, 10}, {-1.0, 0.0f});
        label_result = lblres;
        auto lblpage = sgui::SGLabel::Create(rpnl, {0, 33}, L"");
        lblpage->SetPosition({0, 33}, {-1.0, 0.0f});
        label_page = lblpage;
        auto btn1 = sgui::SGButton::Create(rpnl, {10, 35}, {15, 15});
        auto btn2 = sgui::SGButton::Create(rpnl, {170, 35}, {15, 15});
        btn2->SetPosition({-30, 35}, {1.0f, 0.0f});
        btn1->SetTextureRect({136, 74, 15, 15}, {136, 90, 15, 15}, {136, 106, 15, 15});
        btn2->SetTextureRect({154, 74, 15, 15}, {154, 90, 15, 15}, {154, 106, 15, 15});
        btn1->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            ResultPrevPage();
            return true;
        });
        btn2->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            ResultNextPage();
            return true;
        });
    }
    
    void BuildSceneHandler::ViewRegulation(int limit) {
        build_scene.lock()->ViewRegulation(limit);
        view_regulation = limit + 1;
        current_file.clear();
        deck_edited = false;
        std::wstring title = L"\ue07a";
        if(limit == 0)
            title.append(stringCfg["eui_list_forbidden"]);
        else if(limit == 1)
            title.append(stringCfg["eui_list_limit"]);
        else
            title.append(stringCfg["eui_list_semilimit"]);
        SetDeckLabel(title, 0xff000000);
    }
    
    void BuildSceneHandler::SetDeckEdited() {
        if(!deck_edited) {
            if(current_file.length() == 0)
                SetDeckLabel(std::wstring(L"\ue08c").append(stringCfg["eui_msg_new_deck"]).append(L"*"), 0xff000000);
            else
                SetDeckLabel(std::wstring(L"\ue08c").append(current_file).append(L"*"), 0xff000000);
            deck_edited = true;
        }
        view_regulation = 0;
    }
    
    void BuildSceneHandler::OnMenuDeck(int id) {
        switch(id) {
            case 0:
                file_dialog->Show(stringCfg["eui_msg_deck_load"], commonCfg["deck_path"], L".ydk");
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
                    file_dialog->Show(stringCfg["eui_msg_deck_save"], commonCfg["deck_path"], L".ydk");
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
                file_dialog->Show(stringCfg["eui_msg_deck_save"], commonCfg["deck_path"], L".ydk");
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
                MessageBox::ShowOK(L"", stringCfg["eui_msg_deck_tostr_ok"]);
            }
                break;
            default:
                break;
        }
    }
    
    void BuildSceneHandler::OnMenuTool(int id) {
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
                std::wstring neturl = commonCfg["deck_neturl"];
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
    
    void BuildSceneHandler::OnMenuList(int id) {
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
    
    void BuildSceneHandler::ShowCardInfo(unsigned int code) {
        int w = 700;
        int h = 320;
        auto scene_size = SceneMgr::Get().GetSceneSize();
        info_panel->ShowInfo(code, {scene_size.x / 2 - w / 2, scene_size.y / 2 - h / 2}, {w, h});
    }
    
    void BuildSceneHandler::HideCardInfo() {
        info_panel->Destroy();
    }
    
    void BuildSceneHandler::SetDeckLabel(const std::wstring& str, unsigned int cl) {
        if(!deck_label.expired())
            deck_label.lock()->SetText(str, 0xff000000);
    }
    
    void BuildSceneHandler::ResultPrevPage() {
        if(result_page == 0)
            return;
        result_page--;
        std::array<CardData*, 10> new_results;
        for(int i = 0; i < 10; ++i) {
            if((size_t)(result_page * 10 + i) < search_result.size())
                new_results[i] = search_result[result_page * 10 + i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int pageall = (search_result.size() == 0) ? 0 : (search_result.size() - 1) / 10 + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr->SetText(s, 0xff000000);
        }
    }
    
    void BuildSceneHandler::ResultNextPage() {
        if((size_t)(result_page * 10 + 10) >= search_result.size())
            return;
        result_page++;
        std::array<CardData*, 10> new_results;
        for(int i = 0; i < 10; ++i) {
            if((size_t)(result_page * 10 + i) < search_result.size())
                new_results[i] = search_result[result_page * 10 + i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int pageall = (search_result.size() == 0) ? 0 : (search_result.size() - 1) / 10 + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr->SetText(s, 0xff000000);
        }
    }
    
    void BuildSceneHandler::Search(const FilterCondition& fc, int lmt) {
        if(lmt == 0)
            search_result = DataMgr::Get().FilterCard(fc);
        else
            search_result = LimitRegulationMgr::Get().FilterCard(lmt - 1, fc);
        std::sort(search_result.begin(), search_result.end(), CardData::card_sort);
        result_page = 0;
        std::array<CardData*, 10> new_results;
        for(int i = 0; i < 10; ++i) {
            if((size_t)(i) < search_result.size())
                new_results[i] = search_result[i];
            else
                new_results[i] = nullptr;
        }
        build_scene.lock()->RefreshSearchResult(new_results);
        auto ptr = label_result.lock();
        if(ptr != nullptr) {
            std::wstring s = stringCfg["eui_filter_count"];
            std::wstring ct = To<std::wstring>(To<std::string>("%ld", search_result.size()));
            size_t pos = s.find(L"{count}");
            if(pos != std::wstring::npos)
                s.replace(pos, 7, ct);
            ptr->SetText(s, 0xff000000);
        }
        auto ptr2 = label_page.lock();
        if(ptr2 != nullptr) {
            int pageall = (search_result.size() == 0) ? 0 : (search_result.size() - 1) / 10 + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr2->SetText(s, 0xff000000);
        }
    }
    
}
