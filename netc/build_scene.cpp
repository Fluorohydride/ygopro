#include "../common/common.h"

#include "glbase.h"
#include "sungui.h"
#include "image_mgr.h"
#include "card_data.h"
#include "deck_data.h"
#include "build_scene.h"

namespace ygopro
{
    
    BuildScene::BuildScene() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &deck_buffer);
        glGenBuffers(1, &back_buffer);
        glGenBuffers(1, &misc_buffer);
        glGenBuffers(1, &result_buffer);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 256 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, misc_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 33 * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, result_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * 10 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__FILE__, __LINE__);
        std::vector<unsigned short> index;
        index.resize(256 * 4 * 6);
        for(int i = 0; i < 256 * 4; ++i) {
            index[i * 6] = i * 4;
            index[i * 6 + 1] = i * 4 + 2;
            index[i * 6 + 2] = i * 4 + 1;
            index[i * 6 + 3] = i * 4 + 1;
            index[i * 6 + 4] = i * 4 + 2;
            index[i * 6 + 5] = i * 4 + 3;
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 256 * 4 * 6, &index[0], GL_STATIC_DRAW);
        glGenVertexArrays(1, &deck_vao);
        glGenVertexArrays(1, &back_vao);
        glGenVertexArrays(1, &misc_vao);
        glGenVertexArrays(1, &result_vao);
        for(int i = 0; i < 4; ++i) {
            switch(i) {
                case 0: glBindVertexArray(deck_vao); glBindBuffer(GL_ARRAY_BUFFER, deck_buffer); break;
                case 1: glBindVertexArray(back_vao); glBindBuffer(GL_ARRAY_BUFFER, back_buffer); break;
                case 2: glBindVertexArray(misc_vao); glBindBuffer(GL_ARRAY_BUFFER, misc_buffer); break;
                case 3: glBindVertexArray(result_vao); glBindBuffer(GL_ARRAY_BUFFER, result_buffer); break;
                default: break;
            }
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), 0);
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::color_offset);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glbase::v2ct), (const GLvoid*)glbase::v2ct::tex_offset);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            glBindVertexArray(0);
        }
        GLCheckError(__FILE__, __LINE__);
        limit[0] = ImageMgr::Get().GetTexture("limit0");
        limit[1] = ImageMgr::Get().GetTexture("limit1");
        limit[2] = ImageMgr::Get().GetTexture("limit2");
        pool[0] = ImageMgr::Get().GetTexture("pool_ocg");
        pool[1] = ImageMgr::Get().GetTexture("pool_tcg");
        pool[2] = ImageMgr::Get().GetTexture("pool_ex");
        hmask = ImageMgr::Get().GetTexture("cmask");
        file_dialog = std::make_shared<FileDialog>();
        filter_dialog = std::make_shared<FilterDialog>();
        info_panel = std::make_shared<InfoPanel>();
    }
    
    BuildScene::~BuildScene() {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &deck_buffer);
        glDeleteBuffers(1, &back_buffer);
        glDeleteBuffers(1, &misc_buffer);
        glDeleteBuffers(1, &result_buffer);
        glDeleteVertexArrays(1, &deck_vao);
        glDeleteVertexArrays(1, &back_vao);
        glDeleteVertexArrays(1, &misc_vao);
        glDeleteVertexArrays(1, &result_vao);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::Activate() {
        view_regulation = 0;
        hover_pos = std::make_pair(0, 0);
        click_pos = std::make_pair(0, 0);
        search_result.clear();
        result_page = 0;
        current_deck.Clear();
        build_timer.Init(SceneMgr::Get().GetGameTime());
        auto pnl = sgui::SGPanel::Create(nullptr, {10, 5}, {0, 35});
        pnl->SetSize({-20, 35}, {1.0f, 0.0f});
        pnl->eventKeyDown.Bind([this](sgui::SGWidget& sender, sgui::KeyEvent evt)->bool {
            KeyDown(evt);
            return true;
        });
        auto rpnl = sgui::SGPanel::Create(nullptr, {0, 0}, {200, 60});
        rpnl->SetPosition({0, 40}, {0.795f, 0.0f});
        rpnl->SetSize({-10, 60}, {0.205f, 0.0f});
        rpnl->eventKeyDown.Bind([this](sgui::SGWidget& sender, sgui::KeyEvent evt)->bool {
            KeyDown(evt);
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
            ChangeRegulation(index);
            return true;
        });
        auto show_ex = sgui::SGCheckbox::Create(pnl, {710, 7}, {100, 30});
        show_ex->SetText(stringCfg["eui_show_exclusive"], 0xff000000);
        show_ex->SetChecked(true);
        show_ex->eventCheckChange.Bind([this](sgui::SGWidget& sender, bool check)->bool {
            ChangeExclusive(check);
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
        RefreshAllCard();
    }
    
    bool BuildScene::Update() {
        double tm = SceneMgr::Get().GetGameTime();
        build_timer.UpdateTime(tm);
        UpdateBackGround();
        UpdateCard();
        UpdateMisc();
        UpdateResult();
        UpdateInfo();
        return true;
    }
    
    void BuildScene::Draw() {
        glViewport(0, 0, scene_size.x, scene_size.y);
        auto& shader = glbase::Shader::GetDefaultShader();
        shader.Use();
        shader.SetParam1i("texid", 0);
        // background
        ImageMgr::Get().GetRawBGTexture()->Bind();
        glBindVertexArray(back_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        // miscs
        ImageMgr::Get().GetRawMiscTexture()->Bind();
        glBindVertexArray(misc_vao);
        glDrawElements(GL_TRIANGLES, 33 * 6, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__FILE__, __LINE__);
        // cards
        ImageMgr::Get().GetRawCardTexture()->Bind();
        // result
        if(result_show_size) {
            glBindVertexArray(result_vao);
            glDrawElements(GL_TRIANGLES, result_show_size * 24, GL_UNSIGNED_SHORT, 0);
            GLCheckError(__FILE__, __LINE__);
        }
        // deck
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz > 0) {
            glBindVertexArray(deck_vao);
            glDrawElements(GL_TRIANGLES, deck_sz * 24, GL_UNSIGNED_SHORT, 0);
        }
        GLCheckError(__FILE__, __LINE__);
        glBindVertexArray(0);
        shader.Unuse();
    }
    
    void BuildScene::SetSceneSize(v2i sz) {
        scene_size = sz;
        float yrate = 1.0f - 40.0f / sz.y;
        card_size = {0.2f * yrate * sz.y / sz.x, 0.29f * yrate};
        icon_size = {0.08f * yrate * sz.y / sz.x, 0.08f * yrate};
        minx = 50.0f / sz.x * 2.0f - 1.0f;
        maxx = 0.541f;
        main_y_spacing = 0.3f * yrate;
        offsety[0] = (0.92f + 1.0f) * yrate - 1.0f;
        offsety[1] = (-0.31f + 1.0f) * yrate - 1.0f;
        offsety[2] = (-0.64f + 1.0f) * yrate - 1.0f;
        max_row_count = (maxx - minx - card_size.x) / (card_size.x * 1.1f);
        if(max_row_count < 10)
            max_row_count = 10;
        main_row_count = max_row_count;
        if((int)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (current_deck.main_deck.size() - 1) / 4 + 1;
        dx[0] = (maxx - minx - card_size.x) / (main_row_count - 1);
        int rc1 = std::max((int)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc1 - 1);
        int rc2 = std::max((int)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc2 - 1);
        UpdateAllCard();
        update_misc = true;
        update_result = true;
        GLCheckError(__FILE__, __LINE__);
    }
    
    recti BuildScene::GetScreenshotClip() {
        int maxx = (int)(scene_size.x * 0.795f) - 1;
        return {0, 40, maxx, scene_size.y - 40};
    }
    
    void BuildScene::MouseMove(sgui::MouseMoveEvent evt) {
        std::shared_ptr<DeckCardData> dcd = nullptr;
        auto pre = hover_obj.lock();
        auto hov = GetHoverPos(evt.x, evt.y);
        if(hov.first == 4) {
            if(hov.second != current_sel_result) {
                current_sel_result = hov.second;
                update_result = true;
            }
            if(show_info && (size_t)(result_page * 10 + current_sel_result) < search_result.size())
                ShowCardInfo(search_result[result_page * 10 + current_sel_result]->code);
        } else {
            dcd = GetCard(hov.first, hov.second);
            if(current_sel_result >= 0) {
                current_sel_result = -1;
                update_result = true;
            }
            if(dcd && show_info)
               ShowCardInfo(dcd->data->code);
        }
        if(dcd != pre) {
            if(pre) {
                auto ptr = std::static_pointer_cast<BuilderCard>(pre->extra);
                ptr->hl.Reset(0.0f);
            }
            if(dcd) {
                auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
                ptr->hl.SetAnimator(std::make_shared<LerpAnimator<float, TGenPeriodicRet>>(0.2f, 0.8f, SceneMgr::Get().GetGameTime(), 1.0));
                if(!ptr->updateing) {
                    ptr->updateing = true;
                    updating_cards.push_back(dcd);
                }
            }
        }
        hover_pos = hov;
        hover_obj = dcd;
    }
    
    void BuildScene::MouseButtonDown(sgui::MouseButtonEvent evt) {
        click_pos = hover_pos;
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT) {
            show_info_begin = true;
            show_info_time = SceneMgr::Get().GetGameTime();
        }
    }
    
    void BuildScene::MouseButtonUp(sgui::MouseButtonEvent evt) {
        if(evt.button == GLFW_MOUSE_BUTTON_LEFT)
            show_info_begin = false;
        if(hover_pos != click_pos)
            return;
        click_pos.first = 0;
        int pos = hover_pos.first;
        if(pos > 0 && pos < 4) {
            int index = hover_pos.second;
            if(index < 0)
                return;
            auto dcd = GetCard(pos, index);
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
    
    void BuildScene::MouseWheel(sgui::MouseWheelEvent evt) {
        
    }
    
    void BuildScene::KeyDown(sgui::KeyEvent evt) {
        switch(evt.key) {
            case GLFW_KEY_1:
                if(evt.mods & GLFW_MOD_ALT)
                    ViewRegulation(0);
                break;
            case GLFW_KEY_2:
                if(evt.mods & GLFW_MOD_ALT)
                    ViewRegulation(1);
                break;
            case GLFW_KEY_3:
                if(evt.mods & GLFW_MOD_ALT)
                    ViewRegulation(2);
                break;
            case GLFW_KEY_C:
                if(evt.mods & GLFW_MOD_CONTROL)
                    SaveDeckToClipboard();
                break;
            case GLFW_KEY_V:
                if(evt.mods & GLFW_MOD_CONTROL)
                    LoadDeckFromClipboard();
                break;
            default:
                break;
        }
    }
    
    void BuildScene::KeyUp(sgui::KeyEvent evt) {
        
    }
    
    void BuildScene::ShowCardInfo(unsigned int code) {
        int w = 700;
        int h = 320;
        info_panel->ShowInfo(code, {scene_size.x / 2 - w / 2, scene_size.y / 2 - h / 2}, {w, h});
    }
    
    void BuildScene::ClearDeck() {
        if(current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size() == 0)
            return;
        for(auto& dcd : current_deck.main_deck)
            ImageMgr::Get().UnloadCardTexture(dcd->data->code);
        for(auto& dcd : current_deck.extra_deck)
            ImageMgr::Get().UnloadCardTexture(dcd->data->code);
        for(auto& dcd : current_deck.side_deck)
            ImageMgr::Get().UnloadCardTexture(dcd->data->code);
        current_deck.Clear();
        SetDeckDirty();
    }
    
    void BuildScene::SortDeck() {
        current_deck.Sort();
        RefreshAllIndex();
        UpdateAllCard();
    }
    
    void BuildScene::ShuffleDeck() {
        current_deck.Shuffle();
        RefreshAllIndex();
        UpdateAllCard();
    }
    
    void BuildScene::SetDeckDirty() {
        if(!deck_edited) {
            if(current_file.length() == 0)
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(stringCfg["eui_msg_new_deck"]).append(L"*"), 0xff000000);
            else
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(current_file).append(L"*"), 0xff000000);
            deck_edited = true;
        }
        update_misc = true;
        view_regulation = 0;
    }
    
    void BuildScene::LoadDeckFromFile(const std::wstring& file) {
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(file)) {
            ClearDeck();
            current_deck = tempdeck;
            current_file = file;
            deck_edited = false;
            update_misc = true;
            view_regulation = 0;
            for(auto& dcd : current_deck.main_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.side_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            if(!deck_label.expired())
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(current_file), 0xff000000);
        }
        RefreshAllCard();
    }
    
    void BuildScene::LoadDeckFromClipboard() {
        DeckData tempdeck;
        std::string deck_string = glfwGetClipboardString(nullptr);
        if(deck_string.find("ydk://") == 0 && tempdeck.LoadFromString(deck_string.substr(6))) {
            ClearDeck();
            current_deck = tempdeck;
            SetDeckDirty();
            view_regulation = 0;
            for(auto& dcd : current_deck.main_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.side_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                exdata->show_exclusive = show_exclusive;
                dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
        }
        RefreshAllCard();
    }
    
    void BuildScene::SaveDeckToFile(const std::wstring& file) {
        auto deckfile = file;
        if(deckfile.find(L".ydk") != deckfile.length() - 4)
            deckfile.append(L".ydk");
        current_deck.SaveToFile(deckfile);
        current_file = deckfile;
        deck_edited = false;
        if(!deck_label.expired())
            deck_label.lock()->SetText(std::wstring(L"\ue08c").append(current_file), 0xff000000);
    }
    
    void BuildScene::SaveDeckToClipboard() {
        auto str = current_deck.SaveToString();
        std::string deck_string;
        deck_string.append("ydk://").append(str);
        glfwSetClipboardString(nullptr, deck_string.c_str());
        MessageBox::ShowOK(L"", stringCfg["eui_msg_deck_tostr_ok"]);
    }
    
    void BuildScene::OnMenuDeck(int id) {
        switch(id) {
            case 0:
                file_dialog->Show(stringCfg["eui_msg_deck_load"], commonCfg["deck_path"], L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                    if(deck_edited || deck != current_file)
                        LoadDeckFromFile(deck);
                });
                break;
            case 1:
                if(current_file.length() == 0) {
                    file_dialog->Show(stringCfg["eui_msg_deck_save"], commonCfg["deck_path"], L".ydk");
                    file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                        SaveDeckToFile(deck);
                    });
                } else
                    SaveDeckToFile(current_file);
                break;
            case 2:
                file_dialog->Show(stringCfg["eui_msg_deck_save"], commonCfg["deck_path"], L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                    SaveDeckToFile(deck);
                });
                break;
            case 3:
                LoadDeckFromClipboard();
                break;
            case 4:
                SaveDeckToClipboard();
                break;
            default:
                break;
        }
    }
    
    void BuildScene::OnMenuTool(int id) {
        switch(id) {
            case 0:
                SortDeck();
                break;
            case 1:
                ShuffleDeck();
                break;
            case 2:
                ClearDeck();
                SetDeckDirty();
                break;
            case 3: {
                std::wstring neturl = commonCfg["deck_neturl"];
                std::wstring deck_string = To<std::wstring>(current_deck.SaveToString());
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
    
    void BuildScene::OnMenuList(int id) {
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
    
    void BuildScene::UpdateBackGround() {
        if(!update_bg)
            return;
        update_bg = false;
        auto ti = ImageMgr::Get().GetTexture("bg");
        std::array<glbase::v2ct, 4> verts;
        glbase::FillVertex(&verts[0], {-1.0f, 1.0f}, {2.0f, -2.0f}, ti);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * verts.size(), &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::UpdateCard() {
        double tm = SceneMgr::Get().GetGameTime();
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        for(auto iter = updating_cards.begin(); iter != updating_cards.end();) {
            auto cur = iter++;
            auto dcd = (*cur).lock();
            if(dcd == nullptr) {
                updating_cards.erase(cur);
                continue;
            }
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
            bool up = ptr->pos.NeedUpdate();
            ptr->pos.Update(tm);
            ptr->size.Update(tm);
            ptr->hl.Update(tm);
            if(up)
                RefreshCardPos(dcd);
            else
                RefreshHL(dcd);
            if(!ptr->pos.NeedUpdate() && !ptr->size.NeedUpdate() && !ptr->hl.NeedUpdate()) {
                ptr->updateing = false;
                updating_cards.erase(cur);
            }
        }
    }
    
    void BuildScene::UpdateAllCard() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.main_deck[i]->extra);
            ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenMove>>(ptr->pos.Get(), cpos, SceneMgr::Get().GetGameTime(), 1.0, 10));
            if(!ptr->updateing) {
                ptr->updateing = true;
                updating_cards.push_back(current_deck.main_deck[i]);
            }
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[1] * i, offsety[1]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[i]->extra);
            ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenMove>>(ptr->pos.Get(), cpos, SceneMgr::Get().GetGameTime(), 1.0, 10));
            if(!ptr->updateing) {
                ptr->updateing = true;
                updating_cards.push_back(current_deck.extra_deck[i]);
            }
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[2] * i, offsety[2]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[i]->extra);
            ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenMove>>(ptr->pos.Get(), cpos, SceneMgr::Get().GetGameTime(), 1.0, 10));
            if(!ptr->updateing) {
                ptr->updateing = true;
                updating_cards.push_back(current_deck.side_deck[i]);
            }
        }
    }
    
    void BuildScene::RefreshParams() {
        main_row_count = max_row_count;
        if((int)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (current_deck.main_deck.size() - 1) / 4 + 1;
        dx[0] = (maxx - minx - card_size.x) / (main_row_count - 1);
        int rc1 = std::max((int)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc1 - 1);
        int rc2 = std::max((int)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc2 - 1);
    }
    
    void BuildScene::RefreshAllCard() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        RefreshParams();
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        unsigned int index = 0;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.main_deck[i]->extra);
            ptr->buffer_index = index++;
            ptr->pos = cpos;
            ptr->size = card_size;
            RefreshCardPos(current_deck.main_deck[i]);
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[1] * i, offsety[1]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[i]->extra);
            ptr->buffer_index = index++;
            ptr->pos = cpos;
            ptr->size = card_size;
            RefreshCardPos(current_deck.extra_deck[i]);
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[2] * i, offsety[2]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[i]->extra);
            ptr->buffer_index = index++;
            ptr->pos = cpos;
            ptr->size = card_size;
            RefreshCardPos(current_deck.side_deck[i]);
        }
    }
    
    void BuildScene::RefreshAllIndex() {
        unsigned int index = 0;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.main_deck[i]->extra);
            ptr->buffer_index = index++;
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[i]->extra);
            ptr->buffer_index = index++;
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[i]->extra);
            ptr->buffer_index = index++;
        }
    }
    
    void BuildScene::RefreshCardPos(std::shared_ptr<DeckCardData> dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        std::array<glbase::v2ct, 16> verts;
        auto& pos = ptr->pos.Get();
        auto& sz = ptr->size.Get();
        glbase::FillVertex(&verts[0], ptr->pos, {sz.x, -sz.y}, ptr->card_tex);
        unsigned int cl = (((unsigned int)((float)ptr->hl * 255) & 0xff) << 24) | 0xffffff;
        glbase::FillVertex(&verts[4], ptr->pos, {sz.x, -sz.y}, hmask, cl);
        if((ptr->show_limit) && dcd->limit < 3) {
            auto& lti = limit[dcd->limit];
            glbase::FillVertex(&verts[8], pos + v2f{-0.01f, 0.01f}, {icon_size.x, -icon_size.y}, lti);
        }
        if((ptr->show_exclusive) && dcd->data->pool != 3) {
            float px = pos.x + sz.x / 2.0f - icon_size.x * 0.75f;
            auto& pti = (dcd->data->pool == 1) ? pool[0] : pool[1];
            glbase::FillVertex(&verts[12], {px, pos.y - sz.y + icon_size.y * 0.75f - 0.01f}, {icon_size.x * 1.5f, -icon_size.y * 0.75f}, pti);
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * ptr->buffer_index * 16, sizeof(glbase::v2ct) * 16, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::UpdateMisc() {
        if(!update_misc)
            return;
        update_misc = false;
        std::array<glbase::v2ct, 33 * 4> verts;
        auto msk = ImageMgr::Get().GetTexture("mmask");
        auto nbk = ImageMgr::Get().GetTexture("numback");
        float yrate = 1.0f - 40.0f / scene_size.y;
        float lx = 10.0f / scene_size.x * 2.0f - 1.0f;
        float rx = 0.5625f;
        float y0 = (0.95f + 1.0f) * yrate - 1.0f;
        float y1 = (offsety[0] - main_y_spacing * 3 - card_size.y + offsety[1]) / 2;
        float y2 = (offsety[1] - card_size.y + offsety[2]) / 2;
        float y3 = offsety[2] - card_size.y - 0.03f * yrate;
        float nw = 60.0f / scene_size.x;
        float nh = 60.0f / scene_size.y;
        float nx = 15.0f / scene_size.x * 2.0f - 1.0f;
        float ndy = 64.0f / scene_size.y;
        float th = 120.0f / scene_size.y;
        float my = offsety[0] - main_y_spacing * 3 - card_size.y + th;
        float ey = offsety[1] - card_size.y + th;
        float sy = offsety[2] - card_size.y + th;
        auto numblock = [&nw, &nh, &nbk](glbase::v2ct* v, v2f pos, unsigned int cl1, unsigned int cl2, int val) {
            glbase::FillVertex(&v[0], {pos.x, pos.y}, {nw, -nh}, nbk, cl1);
            if(val >= 10) {
                glbase::FillVertex(&v[4], {pos.x + nw * 0.1f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + (val % 100) / 10), cl2);
                glbase::FillVertex(&v[8], {pos.x + nw * 0.5f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + val % 10), cl2);
            } else
                glbase::FillVertex(&v[4], {pos.x + nw * 0.3f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + val), cl2);
        };
        glbase::FillVertex(&verts[0], {lx, y0}, {rx - lx, y1 - y0}, msk, 0xc0ffffff);
        glbase::FillVertex(&verts[4], {lx, y1}, {rx - lx, y2 - y1}, msk, 0xc0c0c0c0);
        glbase::FillVertex(&verts[8], {lx, y2}, {rx - lx, y3 - y2}, msk, 0xc0808080);
        glbase::FillVertex(&verts[12], {nx, my}, {nw, -th}, ImageMgr::Get().GetTexture("main_t"), 0xff80ffff);
        glbase::FillVertex(&verts[16], {nx, ey}, {nw, -th}, ImageMgr::Get().GetTexture("extra_t"), 0xff80ffff);
        glbase::FillVertex(&verts[20], {nx, sy}, {nw, -th}, ImageMgr::Get().GetTexture("side_t"), 0xff80ffff);
        numblock(&verts[24], {nx, offsety[0] - ndy * 0}, 0xf03399ff, 0xff000000, current_deck.mcount);
        numblock(&verts[36], {nx, offsety[0] - ndy * 1}, 0xf0a0b858, 0xff000000, current_deck.scount);
        numblock(&verts[48], {nx, offsety[0] - ndy * 2}, 0xf09060bb, 0xff000000, current_deck.tcount);
        numblock(&verts[60], {nx, offsety[0] - ndy * 3}, 0xf0b050a0, 0xff000000, current_deck.fuscount);
        numblock(&verts[72], {nx, offsety[0] - ndy * 4}, 0xf0ffffff, 0xff000000, current_deck.syncount);
        numblock(&verts[84], {nx, offsety[0] - ndy * 5}, 0xf0000000, 0xffffffff, current_deck.xyzcount);
        numblock(&verts[96], {nx, my + card_size.y - th}, 0x80ffffff, 0xff000000, current_deck.main_deck.size());
        numblock(&verts[108], {nx, ey + card_size.y - th}, 0x80ffffff, 0xff000000, current_deck.extra_deck.size());
        numblock(&verts[120], {nx, sy + card_size.y - th}, 0x80ffffff, 0xff000000, current_deck.side_deck.size());
        glBindBuffer(GL_ARRAY_BUFFER, misc_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * 33 * 4, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::UpdateResult() {
        if(!update_result)
            return;
        update_result = false;
        result_show_size = 0;
        if((size_t)(result_page * 10) >= search_result.size())
            return;
        float left = 0.59f;
        float right = 1.0f - 10.0f / scene_size.x * 2.0f;
        float width = (right - left) / 2.0f;
        float top = 1.0f - 110.0f / scene_size.y * 2.0f;
        float down = 10.0f / scene_size.y * 2.0f - 1.0f;
        float height = (top - down) / 5.0f;
        float cheight = height * 0.9f;
        float cwidth = cheight / 2.9f * 2.0f * scene_size.y / scene_size.x;
        if(cwidth >= (right - left) / 2.0f) {
            cwidth = (right - left) / 2.0f;
            cheight = cwidth * 2.9f / 2.0f * scene_size.x / scene_size.y;
        }
        float offy = (height - cheight) * 0.5f;
        float iheight = 0.08f / 0.29f * cheight;
        float iwidth = iheight * scene_size.y / scene_size.x;
        std::array<glbase::v2ct, 160> verts;
        for(int i = 0; i < 10 ; ++i) {
            if((size_t)(i + result_page * 10) >= search_result.size())
                continue;
            result_show_size++;
            auto pvert = &verts[i * 16];
            unsigned int cl = (i == current_sel_result) ? 0xc0ffffff : 0xc0000000;
            glbase::FillVertex(&pvert[0], {left + (i % 2) * width, top - (i / 2) * height}, {width, -height}, hmask, cl);
            CardData* pdata = search_result[i + result_page * 10];
            glbase::FillVertex(&pvert[4], {left + (i % 2) * width + width / 2 - cwidth / 2, top - (i / 2) * height - offy}, {cwidth, -cheight}, result_tex[i]);
            unsigned int lmt = LimitRegulationMgr::Get().GetCardLimitCount(pdata->code);
            if(lmt < 3) {
                glbase::FillVertex(&pvert[8], {left + (i % 2) * width + width / 2 - cwidth / 2 - 0.01f, top - (i / 2) * height - offy + 0.01f},
                                   {iwidth, -iheight}, limit[lmt]);
            }
            if(show_exclusive && pdata->pool != 3) {
                auto& pti = (pdata->pool == 1) ? pool[0] : pool[1];
                glbase::FillVertex(&pvert[12], {left + (i % 2) * width + width / 2 - iwidth * 0.75f, top - (i / 2) * height + offy - height + iheight * 0.75f - 0.01f},
                                   {iwidth * 1.5f, -iheight * 0.75f}, pti);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, result_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::v2ct) * result_show_size * 16, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::UpdateInfo() {
        if(show_info_begin) {
            double now = SceneMgr::Get().GetGameTime();
            if(now - show_info_time >= 0.5) {
                show_info = true;
                show_info_begin = false;
                click_pos.first = 0;
                auto pos = hover_pos.first;
                if(pos > 0 && pos < 4) {
                    auto dcd = GetCard(pos, hover_pos.second);
                    if(dcd != nullptr)
                        ShowCardInfo(dcd->data->code);
                } else if(pos == 4) {
                    auto index = hover_pos.second;
                    if((size_t)(result_page * 10 + index) < search_result.size())
                        ShowCardInfo(search_result[result_page * 10 + index]->code);
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
    
    void BuildScene::RefreshHL(std::shared_ptr<DeckCardData> dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        std::array<glbase::v2ct, 4> verts;
        unsigned int cl = (((unsigned int)(ptr->hl.Get() * 255) & 0xff) << 24) | 0xffffff;
        auto& sz = ptr->size.Get();
        glbase::FillVertex(&verts[0], ptr->pos.Get(), {sz.x, -sz.y}, hmask, cl);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * (ptr->buffer_index * 16 + 4), sizeof(glbase::v2ct) * 4, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::RefreshLimit(std::shared_ptr<DeckCardData> dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        std::array<glbase::v2ct, 4> verts;
        if((ptr->show_limit) && dcd->limit < 3) {
            auto lti = limit[dcd->limit];
            glbase::FillVertex(&verts[0], ptr->pos.Get() + v2f{-0.01f, 0.01f}, {icon_size.x, -icon_size.y}, lti);
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * (ptr->buffer_index * 16 + 8), sizeof(glbase::v2ct) * 4, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::RefreshEx(std::shared_ptr<DeckCardData> dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        std::array<glbase::v2ct, 4> verts;
        if((ptr->show_exclusive) && dcd->data->pool != 3) {
            auto& pos = ptr->pos.Get();
            auto& sz = ptr->size.Get();
            float px = pos.x + sz.x / 2.0f - icon_size.x * 0.75f;
            auto& pti = (dcd->data->pool == 1) ? pool[0] : pool[1];
            glbase::FillVertex(&verts[0], {px, pos.y - sz.y + icon_size.y * 0.75f - 0.01f}, {icon_size.x * 1.5f, -icon_size.y * 0.75f}, pti);
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::v2ct) * (ptr->buffer_index * 16 + 12), sizeof(glbase::v2ct) * 4, &verts[0]);
        GLCheckError(__FILE__, __LINE__);
    }
    
    void BuildScene::ChangeExclusive(bool check) {
        show_exclusive = check;
        for(auto& dcd : current_deck.main_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
        for(auto& dcd : current_deck.side_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
        update_result = true;
    }
    
    void BuildScene::ChangeRegulation(int index) {
        LimitRegulationMgr::Get().SetLimitRegulation(index);
        if(view_regulation)
            ViewRegulation(view_regulation - 1);
        else
            LimitRegulationMgr::Get().GetDeckCardLimitCount(current_deck);
        RefreshAllCard();
        update_result = true;
    }
    
    void BuildScene::ViewRegulation(int limit) {
        ClearDeck();
        LimitRegulationMgr::Get().LoadCurrentListToDeck(current_deck, limit);
        for(auto& dcd : current_deck.main_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            exdata->show_exclusive = show_exclusive;
            dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            exdata->show_exclusive = show_exclusive;
            dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        for(auto& dcd : current_deck.side_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            exdata->show_exclusive = show_exclusive;
            dcd->extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        std::wstring title = L"\ue07a";
        if(limit == 0)
            title.append(stringCfg["eui_list_forbidden"]);
        else if(limit == 1)
            title.append(stringCfg["eui_list_limit"]);
        else
            title.append(stringCfg["eui_list_semilimit"]);
        deck_label.lock()->SetText(title, 0xff000000);
        view_regulation = limit + 1;
        current_file.clear();
        deck_edited = false;
        update_misc = true;
        RefreshAllCard();
    }
    
    void BuildScene::UnloadSearchResult() {
        for(int i = 0; i < 10; ++i) {
            if((size_t)(i + result_page * 10) >= search_result.size())
                break;
            ImageMgr::Get().UnloadCardTexture(search_result[i + result_page * 10]->code);
        }
    }
    
    void BuildScene::RefreshSearchResult() {
        for(int i = 0; i < 10; ++i) {
            if((size_t)(i + result_page * 10) >= search_result.size())
                break;
            result_tex[i] = ImageMgr::Get().GetCardTexture(search_result[i + result_page * 10]->code);
        }
        update_result = true;
        auto ptr = label_page.lock();
        if(ptr != nullptr) {
            int pageall = (search_result.size() == 0) ? 0 : (search_result.size() - 1) / 10 + 1;
            std::wstring s = To<std::wstring>(To<std::string>("%d/%d", result_page + 1, pageall));
            ptr->SetText(s, 0xff000000);
        }
    }
    
    void BuildScene::ResultPrevPage() {
        if(result_page == 0)
            return;
        UnloadSearchResult();
        result_page--;
        RefreshSearchResult();
    }
    
    void BuildScene::ResultNextPage() {
        if((size_t)(result_page * 10 + 10) >= search_result.size())
            return;
        UnloadSearchResult();
        result_page++;
        RefreshSearchResult();
    }
    
    void BuildScene::QuickSearch(const std::wstring& keystr) {
        FilterCondition fc;
        if(keystr.length() > 0) {
            if(keystr[0] == L'@') {
                fc.code = FilterDialog::ParseInt(&keystr[1], keystr.length() - 1);
            } else if(keystr[0] == L'#') {
                std::string setstr = "setname_";
                setstr.append(To<std::string>(keystr.substr(1)));
                if(stringCfg.Exists(setstr))
                    fc.setcode = stringCfg[setstr];
                else
                    fc.setcode = 0xffff;
            } else
                fc.keyword = keystr;
            UnloadSearchResult();
            search_result = DataMgr::Get().FilterCard(fc);
            std::sort(search_result.begin(), search_result.end(), CardData::card_sort);
            result_page = 0;
            RefreshSearchResult();
            auto ptr = label_result.lock();
            if(ptr != nullptr) {
                std::wstring s = stringCfg["eui_filter_count"];
                std::wstring ct = To<std::wstring>(To<std::string>("%ld", search_result.size()));
                size_t pos = s.find(L"{count}");
                if(pos != std::wstring::npos)
                    s.replace(pos, 7, ct);
                ptr->SetText(s, 0xff000000);
            }
        }
    }
    
    void BuildScene::Search(const FilterCondition& fc, int lmt) {
        UnloadSearchResult();
        if(lmt == 0)
            search_result = DataMgr::Get().FilterCard(fc);
        else
            search_result = LimitRegulationMgr::Get().FilterCard(lmt - 1, fc);
        std::sort(search_result.begin(), search_result.end(), CardData::card_sort);
        result_page = 0;
        RefreshSearchResult();
        auto ptr = label_result.lock();
        if(ptr != nullptr) {
            std::wstring s = stringCfg["eui_filter_count"];
            std::wstring ct = To<std::wstring>(To<std::string>("%ld", search_result.size()));
            size_t pos = s.find(L"{count}");
            if(pos != std::wstring::npos)
                s.replace(pos, 7, ct);
            ptr->SetText(s, 0xff000000);
        }
    }
    
    std::shared_ptr<DeckCardData> BuildScene::GetCard(int pos, int index) {
        if(index < 0)
            return nullptr;
        if(pos == 1) {
            if(index >= (int)current_deck.main_deck.size())
                return nullptr;
            return current_deck.main_deck[index];
        } else if(pos == 2) {
            if(index >= (int)current_deck.extra_deck.size())
                return nullptr;
            return current_deck.extra_deck[index];
        } else if(pos == 3) {
            if(index >= (int)current_deck.side_deck.size())
                return nullptr;
            return current_deck.side_deck[index];
        }
        return nullptr;
    }
    
    std::pair<int, int> BuildScene::GetHoverPos(int posx, int posy) {
        if(posx >= (int)(scene_size.x * 0.795f) && posx <= scene_size.x - 10 && posy >= 110 && posy <= scene_size.y - 10) {
            int sel = (int)((posy - 110.0f) / ((scene_size.y - 120.0f) / 5.0f)) * 2;
            if(sel > 8)
                sel = 8;
            sel += (posx >= ((int)(scene_size.x * 0.795f) + scene_size.x - 10) / 2) ? 1 : 0;
            return std::make_pair(4, sel);
        }
        float x = (float)posx / scene_size.x * 2.0f - 1.0f;
        float y = 1.0f - (float)posy / scene_size.y * 2.0f;
        if(x >= minx && x <= maxx) {
            if(y <= offsety[0] && y >= offsety[0] - main_y_spacing * 4) {
                unsigned int row = (unsigned int)((offsety[0] - y) / main_y_spacing);
                if(row > 3)
                    row = 3;
                int index = 0;
                if(x > maxx - card_size.x)
                    index = main_row_count - 1;
                else
                    index = (int)((x - minx) / dx[0]);
                int cindex = index;
                index += row * main_row_count;
                if(index >= (int)current_deck.main_deck.size())
                    return std::make_pair(0, 0);
                else {
                    if(y < offsety[0] - main_y_spacing * row - card_size.y || x > minx + cindex * dx[0] + card_size.x)
                        return std::make_pair(0, 0);
                    else
                        return std::make_pair(1, index);
                }
            } else if(y <= offsety[1] && y >= offsety[1] - card_size.y) {
                int rc = std::max((int)current_deck.extra_deck.size(), max_row_count);
                int index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int)((x - minx) / dx[1]);
                if(index >= (int)current_deck.extra_deck.size())
                    return std::make_pair(0, 0);
                else {
                    if(x > minx + index * dx[1] + card_size.x)
                        return std::make_pair(0, 0);
                    else
                        return std::make_pair(2, index);
                }
            } else if(y <= offsety[2] && y >= offsety[2] - card_size.y) {
                int rc = std::max((int)current_deck.side_deck.size(), max_row_count);
                int index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int)((x - minx) / dx[2]);
                if(index >= (int)current_deck.side_deck.size())
                    return std::make_pair(0, 0);
                else {
                    if(x > minx + index * dx[2] + card_size.x)
                        return std::make_pair(0, 0);
                    else
                        return std::make_pair(3, index);
                }
            }
        }
        return std::make_pair(0, 0);
    }
    
}