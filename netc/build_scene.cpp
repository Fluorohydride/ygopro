#include <array>
#include <algorithm>
#include <iostream>

#include <wx/clipbrd.h>

#include "glbase.h"
#include "sungui.h"
#include "image_mgr.h"

#include "card_data.h"
#include "deck_data.h"
#include "build_scene.h"

void GLCheckError(int line) {
    auto err = glGetError();
    switch (err) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            std::cout << "Invalid enum at " << line << std::endl;
        case GL_INVALID_VALUE:
            std::cout << "Invalid value at " << line << std::endl;
        case GL_INVALID_OPERATION:
            std::cout << "Invalid operation at " << line << std::endl;
        case GL_STACK_OVERFLOW:
            std::cout << "Stack overflow at " << line << std::endl;
        case GL_STACK_UNDERFLOW:
            std::cout << "Stack underflow at " << line << std::endl;
        case GL_OUT_OF_MEMORY:
            std::cout << "Out of memory at " << line << std::endl;
        case GL_TABLE_TOO_LARGE:
            std::cout << "Table to large at " << line << std::endl;
        default:
            break;
    }
}
namespace ygopro
{
    
    BuildScene::BuildScene() {
        glGenBuffers(1, &index_buffer);
        glGenBuffers(1, &deck_buffer);
        glGenBuffers(1, &back_buffer);
        glGenBuffers(1, &misc_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * 4, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__LINE__);
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * 256 * 16, nullptr, GL_DYNAMIC_DRAW);
        GLCheckError(__LINE__);
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
        GLCheckError(__LINE__);
        limit[0] = imageMgr.GetTexture("limit0");
        limit[1] = imageMgr.GetTexture("limit1");
        limit[2] = imageMgr.GetTexture("limit2");
        pool[0] = imageMgr.GetTexture("pool_ocg");
        pool[1] = imageMgr.GetTexture("pool_tcg");
        pool[2] = imageMgr.GetTexture("pool_ex");
        hmask = imageMgr.GetTexture("hmask");
    }
    
    BuildScene::~BuildScene() {
        glDeleteBuffers(1, &index_buffer);
        glDeleteBuffers(1, &deck_buffer);
        glDeleteBuffers(1, &back_buffer);
        glDeleteBuffers(1, &misc_buffer);
    }
    
    void BuildScene::Activate() {
        view_regulation = 0;
        file_dialog = std::make_shared<FileDialog>();
        filter_dialog = std::make_shared<FilterDialog>();
        auto pnl = sgui::SGPanel::Create(nullptr, {10, 10}, {240, 300});
        pnl->eventKeyDown.Bind([this](sgui::SGWidget& sender, sf::Event::KeyEvent evt)->bool {
            KeyDown(evt);
            return true;
        });
        auto icon_label = sgui::SGIconLabel::Create(pnl, {10, 10}, std::wstring(L"\ue08c").append(stringCfg[L"eui_msg_new_deck"]));
        deck_label = icon_label;
        auto load_deck = sgui::SGButton::Create(pnl, {10, 40}, {70, 25});
        load_deck->SetText(stringCfg[L"eui_deck_load"], 0xff000000);
        load_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            file_dialog->Show(stringCfg[L"eui_msg_deck_load"], commonCfg[L"deck_path"], L".ydk");
            file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                if(deck_edited || deck != current_file)
                    LoadDeckFromFile(deck);
            });
            return true;
        });
        auto save_deck = sgui::SGButton::Create(pnl, {85, 40}, {70, 25});
        save_deck->SetText(stringCfg[L"eui_deck_save"], 0xff000000);
        save_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            if(current_file.length() == 0) {
                file_dialog->Show(stringCfg[L"eui_msg_deck_save"], commonCfg[L"deck_path"], L".ydk");
                file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                    SaveDeckToFile(deck);
                });
            } else
                SaveDeckToFile(current_file);
            return true;
        });
        auto save_as = sgui::SGButton::Create(pnl, {160, 40}, {70, 25});
        save_as->SetText(stringCfg[L"eui_deck_saveas"], 0xff000000);
        save_as->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            file_dialog->Show(stringCfg[L"eui_msg_deck_save"], commonCfg[L"deck_path"], L".ydk");
            file_dialog->SetOKCallback([this](const std::wstring& deck)->void {
                SaveDeckToFile(deck);
            });
            return true;
        });
        auto clear_deck = sgui::SGButton::Create(pnl, {10, 70}, {70, 25});
        clear_deck->SetText(stringCfg[L"eui_button_clear"], 0xff000000);
        clear_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            ClearDeck();
            SetDeckDirty();
            return true;
        });
        auto sort_deck = sgui::SGButton::Create(pnl, {85, 70}, {70, 25});
        sort_deck->SetText(stringCfg[L"eui_button_sort"], 0xff000000);
        sort_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            SortDeck();
            return true;
        });
        auto shuffle_deck = sgui::SGButton::Create(pnl, {160, 70}, {70, 25});
        shuffle_deck->SetText(stringCfg[L"eui_button_shuffle"], 0xff000000);
        shuffle_deck->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            ShuffleDeck();
            return true;
        });
        auto load_str = sgui::SGButton::Create(pnl, {10, 100}, {100, 25});
        load_str->SetText(stringCfg[L"eui_deck_loadstr"], 0xff000000);
        load_str->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            LoadDeckFromClipboard();
            return true;
        });
        auto save_str = sgui::SGButton::Create(pnl, {130, 100}, {100, 25});
        save_str->SetText(stringCfg[L"eui_deck_savestr"], 0xff000000);
        save_str->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            SaveDeckToClipboard();
            return true;
        });
        auto limit_reg = sgui::SGComboBox::Create(pnl, {10, 130}, {150, 30});
        auto& lrs = limitRegulationMgr.GetLimitRegulations();
        for(unsigned int i = 0; i < lrs.size(); ++i)
            limit_reg->AddItem(lrs[i].name, 0xff000000);
        limit_reg->SetSelection(0);
        limit_reg->eventSelChange.Bind([this](sgui::SGWidget& sender, int index)->bool {
            ChangeRegulation(index);
            return true;
        });
        auto show_ex = sgui::SGCheckbox::Create(pnl, {10, 160}, {100, 30});
        show_ex->SetText(stringCfg[L"eui_show_exclusive"], 0xff000000);
        show_ex->SetChecked(true);
        show_ex->eventCheckChange.Bind([this](sgui::SGWidget& sender, bool check)->bool {
            ChangeExclusive(check);
            return true;
        });
        auto label = sgui::SGLabel::Create(pnl, {10, 190}, stringCfg[L"eui_filter_qsearch"]);
        auto keyword = sgui::SGTextEdit::Create(pnl, {10, 210}, {200, 30});
        keyword->eventTextEnter.Bind([this](sgui::SGWidget& sender)->bool {
            return true;
        });
        auto filter = sgui::SGButton::Create(pnl, {30, 240}, {160, 25});
        filter->SetText(stringCfg[L"eui_button_filter"], 0xff000000);
        filter->eventButtonClick.Bind([this](sgui::SGWidget& sender)->bool {
            filter_dialog->Show();
            filter_dialog->SetOKCallback([this](const FilterCondition fc)->void {
                
            });
            return true;
        });
        RefreshAllCard();
        
    }
    
    void BuildScene::Update() {
        UpdateBackGround();
        UpdateCard();
    }
    
    void BuildScene::Draw() {
        glViewport(0, 0, scene_size.x, scene_size.y);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        // background
        imageMgr.BindTexture(0);
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glVertexPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::tex_offset);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__LINE__);
        // deck
        imageMgr.BindTexture(3);
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        glVertexPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), 0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::color_offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(glbase::VertexVCT), (const GLvoid*)glbase::VertexVCT::tex_offset);
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        glDrawElements(GL_TRIANGLES, deck_sz * 24, GL_UNSIGNED_SHORT, 0);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::SetSceneSize(glbase::vector2<int> sz) {
        scene_size = sz;
        card_size = {0.2f * sz.y / sz.x, 0.29f};
        icon_size = {0.08f * sz.y / sz.x, 0.08f};
        minx = 300.0f / sz.x * 2.0f - 1.0f;
        maxx = 0.95f;
        main_y_spacing = 0.3f;
        offsety[0] = 0.92f;
        offsety[1] = -0.31f;
        offsety[2] = -0.64f;
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
    }
    
    void BuildScene::MouseMove(sf::Event::MouseMoveEvent evt) {
        auto hov = GetHoverCard((float)evt.x / scene_size.x * 2.0f - 1.0f, 1.0f - (float)evt.y / scene_size.y * 2.0f);
        static std::tuple<int, int, int> preh = std::make_tuple(0, 0, 0);
        auto pre = GetCard(std::get<0>(preh), std::get<1>(preh));
        auto dcd = GetCard(std::get<0>(hov), std::get<1>(hov));
        if(dcd != pre) {
            if(pre)
                ChangeHL(*pre, 0.1f, 0.0f);
            if(dcd)
                ChangeHL(*dcd, 0.1f, 0.7f);
            preh = hov;
        }
    }
    
    void BuildScene::MouseButtonDown(sf::Event::MouseButtonEvent evt) {
        
    }
    
    void BuildScene::MouseButtonUp(sf::Event::MouseButtonEvent evt) {
        
    }
    
    void BuildScene::KeyDown(sf::Event::KeyEvent evt) {
        switch(evt.code) {
            case sf::Keyboard::Num1:
                if(evt.alt)
                    ViewRegulation(0);
                break;
            case sf::Keyboard::Num2:
                if(evt.alt)
                    ViewRegulation(1);
                break;
            case sf::Keyboard::Num3:
                if(evt.alt)
                    ViewRegulation(2);
                break;
            default:
                break;
        }
    }
    
    void BuildScene::KeyUp(sf::Event::KeyEvent evt) {
        
    }
    
    void BuildScene::SetCardInfo(unsigned int code) {
        
    }
    
    void BuildScene::AddCard(unsigned int code, unsigned int pos) {
        
    }
    
    void BuildScene::ClearDeck() {
        if(current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size() == 0)
            return;
        for(auto& dcd : current_deck.main_deck)
            imageMgr.UnloadCardTexture(dcd.data->code);
        for(auto& dcd : current_deck.extra_deck)
            imageMgr.UnloadCardTexture(dcd.data->code);
        for(auto& dcd : current_deck.side_deck)
            imageMgr.UnloadCardTexture(dcd.data->code);
        current_deck.Clear();
        SetDeckDirty();
    }
    
    void BuildScene::SortDeck() {
        current_deck.Sort();
        RefreshAllCard();
    }
    
    void BuildScene::ShuffleDeck() {
        current_deck.Shuffle();
        RefreshAllCard();
    }
    
    void BuildScene::SetDeckDirty() {
        if(!deck_edited) {
            if(current_file.length() == 0)
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(stringCfg[L"eui_msg_new_deck"]).append(L"*"), 0xff000000);
            else
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(current_file).append(L"*"), 0xff000000);
            deck_edited = true;
        }
        view_regulation = 0;
    }
    
    void BuildScene::LoadDeckFromFile(const std::wstring& file) {
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(file)) {
            ClearDeck();
            current_deck = tempdeck;
            current_file = file;
            deck_edited = false;
            view_regulation = 0;
            for(auto& dcd : current_deck.main_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.side_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            if(!deck_label.expired())
                deck_label.lock()->SetText(std::wstring(L"\ue08c").append(current_file), 0xff000000);
        }
        RefreshAllCard();
    }
    
    void BuildScene::LoadDeckFromClipboard() {
        DeckData tempdeck;
        wxTextDataObject tdo;
        wxTheClipboard->Open();
        wxTheClipboard->GetData(tdo);
        wxTheClipboard->Close();
        auto deck_string = tdo.GetText().ToStdString();
        if(deck_string.find("ydk://") == 0 && tempdeck.LoadFromString(deck_string.substr(6))) {
            ClearDeck();
            current_deck = tempdeck;
            SetDeckDirty();
            view_regulation = 0;
            for(auto& dcd : current_deck.main_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
            }
            for(auto& dcd : current_deck.side_deck) {
                auto exdata = std::make_shared<BuilderCard>();
                exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
                exdata->show_exclusive = show_exclusive;
                dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
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
        wxString deck_string;
        deck_string.append("ydk://").append(str);
        wxTheClipboard->Open();
        wxTheClipboard->SetData(new wxTextDataObject(deck_string));
        wxTheClipboard->Close();
        MessageBox::ShowOK(L"", stringCfg[L"eui_msg_deck_tostr_ok"]);
    }
    
    void BuildScene::UpdateBackGround() {
        if(!update_bg)
            return;
        update_bg = false;
        auto ti = imageMgr.GetTexture("bg");
        std::array<glbase::VertexVCT, 4> verts;
        verts[0].vertex = {-1.0f, 1.0f, 0.0f};
        verts[0].texcoord = ti.vert[0];
        verts[1].vertex = {1.0f, 1.0f, 0.0f};
        verts[1].texcoord = ti.vert[1];
        verts[2].vertex = {-1.0f, -1.0f, 0.0f};
        verts[2].texcoord = ti.vert[2];
        verts[3].vertex = {1.0f, -1.0f, 0.0f};
        verts[3].texcoord = ti.vert[3];
        glBindBuffer(GL_ARRAY_BUFFER, back_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glbase::VertexVCT) * verts.size(), &verts[0]);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::UpdateCard() {
        float tm = sceneMgr.GetGameTime();
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        for(auto iter = updating_cards.begin(); iter != updating_cards.end();) {
            auto cur = iter++;
            auto ptr = std::static_pointer_cast<BuilderCard>((*cur)->extra);
            bool up = ptr->update_pos;
            if(ptr->update_pos) {
                if(tm >= ptr->moving_time_e) {
                    ptr->pos = ptr->dest_pos;
                    ptr->size = ptr->dest_size;
                    ptr->update_pos = false;
                } else {
                    float rate = (tm - ptr->moving_time_b) / (ptr->moving_time_e - ptr->moving_time_b);
                    ptr->pos = ptr->start_pos + (ptr->dest_pos - ptr->start_pos) * rate;
                    ptr->size = ptr->start_size + (ptr->dest_size - ptr->start_size) * rate;
                }
            }
            if(ptr->update_color) {
                if(tm >= ptr->fading_time_e)
                {
                    ptr->hl = ptr->dest_hl;
                    ptr->update_color = false;
                } else {
                    float rate = (tm - ptr->fading_time_b) / (ptr->fading_time_e - ptr->fading_time_b);
                    ptr->hl = ptr->start_hl + (ptr->dest_hl - ptr->start_hl) * rate;
                }
            }
            if(up)
                RefreshCardPos(**cur);
            else
                RefreshHL(**cur);
            if(!ptr->update_pos && !ptr->update_color)
                updating_cards.erase(cur);
        }
    }
    
    void BuildScene::UpdateAllCard() {
        update_card = false;
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto cpos = (glbase::vector2<float>){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            MoveTo(current_deck.main_deck[i], 0.1f, cpos, card_size);
        }
        if(current_deck.extra_deck.size()) {
            for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
                auto cpos = (glbase::vector2<float>){minx + dx[1] * i, offsety[1]};
                MoveTo(current_deck.extra_deck[i], 0.1f, cpos, card_size);
            }
        }
        if(current_deck.side_deck.size()) {
            for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
                auto cpos = (glbase::vector2<float>){minx + dx[2] * i, offsety[2]};
                MoveTo(current_deck.side_deck[i], 0.1f, cpos, card_size);
            }
        }
    }
    
    void BuildScene::RefreshAllCard() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        main_row_count = max_row_count;
        if((int)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (current_deck.main_deck.size() - 1) / 4 + 1;
        dx[0] = (maxx - minx - card_size.x) / (main_row_count - 1);
        int rc1 = std::max((int)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc1 - 1);
        int rc2 = std::max((int)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc2 - 1);
        glBindBuffer(GL_ARRAY_BUFFER, deck_buffer);
        unsigned int index = 0;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto cpos = (glbase::vector2<float>){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.main_deck[i].extra);
            ptr->buffer_index = index++;
            ptr->pos = cpos;
            ptr->size = card_size;
            RefreshCardPos(current_deck.main_deck[i]);
        }
        if(current_deck.extra_deck.size()) {
            for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
                auto cpos = (glbase::vector2<float>){minx + dx[1] * i, offsety[1]};
                auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[i].extra);
                ptr->buffer_index = index++;
                ptr->pos = cpos;
                ptr->size = card_size;
                RefreshCardPos(current_deck.extra_deck[i]);
            }
        }
        if(current_deck.side_deck.size()) {
            for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
                auto cpos = (glbase::vector2<float>){minx + dx[2] * i, offsety[2]};
                auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[i].extra);
                ptr->buffer_index = index++;
                ptr->pos = cpos;
                ptr->size = card_size;
                RefreshCardPos(current_deck.side_deck[i]);
            }
        }
    }
    
    void BuildScene::RefreshCardPos(DeckCardData& dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        std::array<glbase::VertexVCT, 16> verts;
        verts[0].vertex = ptr->pos;
        verts[0].texcoord = ptr->card_tex.ti.vert[0];
        verts[1].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y, 0.0f};
        verts[1].texcoord = ptr->card_tex.ti.vert[1];
        verts[2].vertex = {ptr->pos.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[2].texcoord = ptr->card_tex.ti.vert[2];
        verts[3].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[3].texcoord = ptr->card_tex.ti.vert[3];
        unsigned int cl = (((unsigned int)(ptr->hl * 255) & 0xff) << 24) | 0xffffff;
        verts[4].vertex = ptr->pos;
        verts[4].texcoord = hmask.vert[0];
        verts[4].color = cl;
        verts[5].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y, 0.0f};
        verts[5].texcoord = hmask.vert[1];
        verts[5].color = cl;
        verts[6].vertex = {ptr->pos.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[6].texcoord = hmask.vert[2];
        verts[6].color = cl;
        verts[7].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[7].texcoord = hmask.vert[3];
        verts[7].color = cl;
        if(dcd.limit < 3) {
            verts[8].vertex = {ptr->pos.x - 0.01f, ptr->pos.y + 0.01f, 0.0f};
            verts[8].texcoord = limit[dcd.limit].vert[0];
            verts[9].vertex = {ptr->pos.x - 0.01f + icon_size.x, ptr->pos.y + 0.01f, 0.0f};
            verts[9].texcoord = limit[dcd.limit].vert[1];
            verts[10].vertex = {ptr->pos.x - 0.01f, ptr->pos.y + 0.01f - icon_size.y, 0.0f};
            verts[10].texcoord = limit[dcd.limit].vert[2];
            verts[11].vertex = {ptr->pos.x - 0.01f + icon_size.x, ptr->pos.y + 0.01f - icon_size.y, 0.0f};
            verts[11].texcoord = limit[dcd.limit].vert[3];
        }
        if((ptr->show_exclusive) && dcd.data->pool != 3) {
            float px = ptr->pos.x + ptr->size.x / 2.0f - icon_size.x * 0.75f;
            auto& pti = (dcd.data->pool == 1) ? pool[0] : pool[1];
            verts[12].vertex = {px, ptr->pos.y - ptr->size.y + icon_size.y * 0.75f - 0.01f, 0.0f};
            verts[12].texcoord = pti.vert[0];
            verts[13].vertex = {px + icon_size.x * 1.5f, ptr->pos.y - ptr->size.y + icon_size.y * 0.75f - 0.01f, 0.0f};
            verts[13].texcoord = pti.vert[1];
            verts[14].vertex = {px, ptr->pos.y - ptr->size.y - 0.01f, 0.0f};
            verts[14].texcoord = pti.vert[2];
            verts[15].vertex = {px + icon_size.x * 1.5f, ptr->pos.y - ptr->size.y - 0.01f, 0.0f};
            verts[15].texcoord = pti.vert[3];
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * ptr->buffer_index * 16, sizeof(glbase::VertexVCT) * 16, &verts[0]);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::RefreshHL(DeckCardData& dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        std::array<glbase::VertexVCT, 4> verts;
        unsigned int cl = (((unsigned int)(ptr->hl * 255) & 0xff) << 24) | 0xffffff;
        verts[0].vertex = ptr->pos;
        verts[0].texcoord = hmask.vert[0];
        verts[0].color = cl;
        verts[1].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y, 0.0f};
        verts[1].texcoord = hmask.vert[1];
        verts[1].color = cl;
        verts[2].vertex = {ptr->pos.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[2].texcoord = hmask.vert[2];
        verts[2].color = cl;
        verts[3].vertex = {ptr->pos.x + ptr->size.x, ptr->pos.y - ptr->size.y, 0.0f};
        verts[3].texcoord = hmask.vert[3];
        verts[3].color = cl;
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * (ptr->buffer_index * 16 + 4), sizeof(glbase::VertexVCT) * 4, &verts[0]);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::RefreshLimit(DeckCardData& dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        std::array<glbase::VertexVCT, 4> verts;
        if(dcd.limit < 3) {
            verts[0].vertex = {ptr->pos.x - 0.01f, ptr->pos.y + 0.01f, 0.0f};
            verts[0].texcoord = limit[dcd.limit].vert[0];
            verts[1].vertex = {ptr->pos.x - 0.01f + icon_size.x, ptr->pos.y + 0.01f, 0.0f};
            verts[1].texcoord = limit[dcd.limit].vert[1];
            verts[2].vertex = {ptr->pos.x - 0.01f, ptr->pos.y + 0.01f - icon_size.y, 0.0f};
            verts[2].texcoord = limit[dcd.limit].vert[2];
            verts[3].vertex = {ptr->pos.x - 0.01f + icon_size.x, ptr->pos.y + 0.01f - icon_size.y, 0.0f};
            verts[3].texcoord = limit[dcd.limit].vert[3];
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * (ptr->buffer_index * 16 + 8), sizeof(glbase::VertexVCT) * 4, &verts[0]);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::RefreshEx(DeckCardData& dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        std::array<glbase::VertexVCT, 4> verts;
        if((ptr->show_exclusive) && dcd.data->pool != 3) {
            float px = ptr->pos.x + ptr->size.x / 2.0f - icon_size.x * 0.75f;
            auto& pti = (dcd.data->pool == 1) ? pool[0] : pool[1];
            verts[0].vertex = {px, ptr->pos.y - ptr->size.y + icon_size.y * 0.75f - 0.01f, 0.0f};
            verts[0].texcoord = pti.vert[0];
            verts[1].vertex = {px + icon_size.x * 1.5f, ptr->pos.y - ptr->size.y + icon_size.y * 0.75f - 0.01f, 0.0f};
            verts[1].texcoord = pti.vert[1];
            verts[2].vertex = {px, ptr->pos.y - ptr->size.y - 0.01f, 0.0f};
            verts[2].texcoord = pti.vert[2];
            verts[3].vertex = {px + icon_size.x * 1.5f, ptr->pos.y - ptr->size.y - 0.01f, 0.0f};
            verts[3].texcoord = pti.vert[3];
        }
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(glbase::VertexVCT) * (ptr->buffer_index * 16 + 12), sizeof(glbase::VertexVCT) * 4, &verts[0]);
        GLCheckError(__LINE__);
    }
    
    void BuildScene::MoveTo(DeckCardData& dcd, float tm, glbase::vector2<float> dst, glbase::vector2<float> dsz) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        ptr->start_pos = ptr->pos;
        ptr->start_size = ptr->size;
        ptr->dest_pos = dst;
        ptr->dest_size = dsz;
        ptr->moving_time_b = sceneMgr.GetGameTime();
        ptr->moving_time_e = ptr->moving_time_b + tm;
        ptr->update_pos = true;
        updating_cards.insert(&dcd);
    }
    
    void BuildScene::ChangeHL(DeckCardData& dcd, float tm, float desthl) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
        ptr->start_hl = ptr->hl;
        ptr->dest_hl = desthl;
        ptr->fading_time_b = sceneMgr.GetGameTime();
        ptr->fading_time_e = ptr->fading_time_b + tm;
        ptr->update_color = true;
        updating_cards.insert(&dcd);
    }
    
    DeckCardData* BuildScene::GetCard(int pos, int index) {
        if(index < 0)
            return nullptr;
        if(pos == 1) {
            if(index >= (int)current_deck.main_deck.size())
                return nullptr;
            return &current_deck.main_deck[index];
        } else if(pos == 2) {
            if(index >= (int)current_deck.extra_deck.size())
                return nullptr;
            return &current_deck.extra_deck[index];
        } else if(pos == 3) {
            if(index >= (int)current_deck.side_deck.size())
                return nullptr;
            return &current_deck.side_deck[index];
        }
        return nullptr;
    }
    
    void BuildScene::ChangeExclusive(bool check) {
        show_exclusive = check;
        for(auto& dcd : current_deck.main_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
        for(auto& dcd : current_deck.side_deck) {
            auto ptr = std::static_pointer_cast<BuilderCard>(dcd.extra);
            ptr->show_exclusive = show_exclusive;
            RefreshEx(dcd);
        }
    }
    
    void BuildScene::ChangeRegulation(int index) {
        limitRegulationMgr.SetLimitRegulation(index);
        if(view_regulation)
            ViewRegulation(view_regulation - 1);
        else
            limitRegulationMgr.GetDeckCardLimitCount(current_deck);
        RefreshAllCard();
    }
    
    void BuildScene::ViewRegulation(int limit) {
        ClearDeck();
        limitRegulationMgr.LoadCurrentListToDeck(current_deck, limit);
        for(auto& dcd : current_deck.main_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
            exdata->show_exclusive = show_exclusive;
            dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
            exdata->show_exclusive = show_exclusive;
            dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        for(auto& dcd : current_deck.side_deck) {
            auto exdata = std::make_shared<BuilderCard>();
            exdata->card_tex = imageMgr.GetCardTexture(dcd.data->code);
            exdata->show_exclusive = show_exclusive;
            dcd.extra = std::static_pointer_cast<DeckCardExtraData>(exdata);
        }
        SetDeckDirty();
        view_regulation = limit + 1;
        std::wstring title = L"\ue07a";
        if(limit == 0)
            title.append(stringCfg[L"eui_list_forbidden"]);
        else if(limit == 1)
            title.append(stringCfg[L"eui_list_limit"]);
        else
            title.append(stringCfg[L"eui_list_semilimit"]);
        deck_label.lock()->SetText(title, 0xff000000);
        current_file.clear();
        RefreshAllCard();
    }
    
    std::tuple<int, int, int> BuildScene::GetHoverCard(float x, float y) {
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
                if(index > (int)current_deck.main_deck.size())
                    return std::make_tuple(1, -1, current_deck.side_deck.size());
                else {
                    if(y < offsety[0] - main_y_spacing * row - card_size.y || x > minx + cindex * dx[0] + card_size.x)
                        return std::make_tuple(1, -1, index);
                    else
                        return std::make_tuple(1, index, 0);
                }
            } else if(y <= offsety[1] && y >= offsety[1] - card_size.y) {
                int rc = std::max((int)current_deck.extra_deck.size(), max_row_count);
                int index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int)((x - minx) / dx[1]);
                if(index > (int)current_deck.extra_deck.size())
                    return std::make_tuple(2, -1, current_deck.extra_deck.size());
                else
                    return std::make_tuple(2, index, 0);
            } else if(y <= offsety[2] && y >= offsety[2] - card_size.y) {
                int rc = std::max((int)current_deck.side_deck.size(), max_row_count);
                int index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int)((x - minx) / dx[2]);
                if(index > (int)current_deck.side_deck.size())
                    return std::make_tuple(3, -1, current_deck.side_deck.size());
                else
                    return std::make_tuple(3, index, 0);
            }
        }
        return std::make_tuple(0, 0, 0);
    }
    
}