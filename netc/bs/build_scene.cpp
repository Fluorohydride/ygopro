#include "../../common/common.h"

#include "../glbase.h"
#include "../sungui.h"
#include "../image_mgr.h"
#include "../card_data.h"
#include "../deck_data.h"
#include "build_scene_handler.h"
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
        for(int i = 0; i < 10; ++i)
            result_data[i] = nullptr;
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
        auto scene_handler = GetSceneHandler<BuildSceneHandler>();
        scene_handler->InitTimer(SceneMgr::Get().GetGameTime());
        scene_handler->BeginHandler();
        RefreshAllCard();
    }
    
    bool BuildScene::Update() {
        auto scene_handler = GetSceneHandler<BuildSceneHandler>();
        if(scene_handler) {
            double tm = SceneMgr::Get().GetGameTime();
            scene_handler->UpdateTime(tm);
            scene_handler->UpdateEvent();
        }
        if(input_handler)
            input_handler->UpdateInput();
        UpdateBackGround();
        UpdateCard();
        UpdateMisc();
        UpdateResult();
        return IsActive();
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
    
    void BuildScene::ShowSelectedInfo(unsigned int pos, unsigned int index) {
        if(pos > 0 && pos < 4) {
            auto dcd = GetCard(pos, index);
            if(dcd != nullptr)
                GetSceneHandler<BuildSceneHandler>()->ShowCardInfo(dcd->data->code);
        } else if(pos == 4) {
            if(result_data[index] != 0)
                GetSceneHandler<BuildSceneHandler>()->ShowCardInfo(result_data[index]->code);
        }
    }
    
    void BuildScene::ShowCardInfo(unsigned int code) {
        GetSceneHandler<BuildSceneHandler>()->ShowCardInfo(code);
    }
    
    void BuildScene::HideCardInfo() {
        GetSceneHandler<BuildSceneHandler>()->HideCardInfo();
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
        GetSceneHandler<BuildSceneHandler>()->SetDeckEdited();
        update_misc = true;
    }
    
    bool BuildScene::LoadDeckFromFile(const std::wstring& file) {
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(file)) {
            ClearDeck();
            current_deck = tempdeck;
            update_misc = true;
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
            RefreshAllCard();
            return true;
        }
        return false;
    }
    
    bool BuildScene::LoadDeckFromString(const std::string& deck_string) {
        DeckData tempdeck;
        if(deck_string.find("ydk://") == 0 && tempdeck.LoadFromString(deck_string.substr(6))) {
            ClearDeck();
            current_deck = tempdeck;
            SetDeckDirty();
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
            RefreshAllCard();
            return true;
        }
        return false;
    }
    
    bool BuildScene::SaveDeckToFile(const std::wstring& file) {
        auto deckfile = file;
        if(deckfile.find(L".ydk") != deckfile.length() - 4)
            deckfile.append(L".ydk");
        current_deck.SaveToFile(deckfile);
        return true;
    }
    
    std::string BuildScene::SaveDeckToString() {
        return std::move(current_deck.SaveToString());
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
                ptr->updating = false;
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
            if(!ptr->updating) {
                ptr->updating = true;
                updating_cards.push_back(current_deck.main_deck[i]);
            }
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[1] * i, offsety[1]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.extra_deck[i]->extra);
            ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenMove>>(ptr->pos.Get(), cpos, SceneMgr::Get().GetGameTime(), 1.0, 10));
            if(!ptr->updating) {
                ptr->updating = true;
                updating_cards.push_back(current_deck.extra_deck[i]);
            }
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[2] * i, offsety[2]};
            auto ptr = std::static_pointer_cast<BuilderCard>(current_deck.side_deck[i]->extra);
            ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenMove>>(ptr->pos.Get(), cpos, SceneMgr::Get().GetGameTime(), 1.0, 10));
            if(!ptr->updating) {
                ptr->updating = true;
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
            if(result_data[i] == nullptr)
                continue;
            result_show_size++;
            auto pvert = &verts[i * 16];
            unsigned int cl = (i == current_sel_result) ? 0xc0ffffff : 0xc0000000;
            glbase::FillVertex(&pvert[0], {left + (i % 2) * width, top - (i / 2) * height}, {width, -height}, hmask, cl);
            CardData* pdata = result_data[i];
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
    
    void BuildScene::ChangeRegulation(int index, int view_regulation) {
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
        update_misc = true;
        RefreshAllCard();
    }
    
    void BuildScene::RefreshSearchResult(const std::array<CardData*, 10> new_results) {
        for(int i = 0; i < 10; ++i) {
            if(new_results[i] != nullptr)
                result_tex[i] = ImageMgr::Get().GetCardTexture(new_results[i]->code);
        }
        for(int i = 0; i < 10; ++i) {
            if(result_data[i] != nullptr)
                ImageMgr::Get().UnloadCardTexture(result_data[i]->code);
            result_data[i] = new_results[i];
        }
        update_result = true;
    }
    
    void BuildScene::SetCurrentSelection(int sel, bool show_info) {
        if(sel != current_sel_result) {
            current_sel_result = sel;
            update_result = true;
            if(current_sel_result >= 0 && show_info && result_data[current_sel_result] != nullptr)
                GetSceneHandler<BuildSceneHandler>()->ShowCardInfo(result_data[current_sel_result]->code);
        }
    }
    
    void BuildScene::AddUpdatingCard(std::shared_ptr<DeckCardData> dcd) {
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        if(!ptr->updating) {
            ptr->updating = true;
            updating_cards.push_back(dcd);
        }
    }
    
    void BuildScene::MoveCard(int pos, int index) {
        auto dcd = GetCard(pos, index);
        if(dcd == nullptr)
            return;
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
        if(input_handler)
            input_handler->MouseMove({SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y});
    }
    
    void BuildScene::RemoveCard(int pos, int index) {
        if(update_status == 1)
            return;
        update_status = 1;
        auto dcd = GetCard(pos, index);
        if(dcd == nullptr)
            return;
        unsigned int code = dcd->data->code;
        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
        v2f dst = ptr->pos.Get() + v2f{card_size.x / 2, -card_size.y / 2};
        v2f dsz = {0.0f, 0.0f};
        ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->pos.Get(), dst, SceneMgr::Get().GetGameTime(), 0.2));
        ptr->size.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->size.Get(), dsz, SceneMgr::Get().GetGameTime(), 0.2));
        ptr->show_limit = false;
        ptr->show_exclusive = false;
        AddUpdatingCard(dcd);
        GetSceneHandler<BuildSceneHandler>()->RegisterEvent([pos, index, code, this]() {
            if(current_deck.RemoveCard(pos, index)) {
                ImageMgr::Get().UnloadCardTexture(code);
                RefreshParams();
                RefreshAllIndex();
                UpdateAllCard();
                SetDeckDirty();
                if(input_handler)
                    input_handler->MouseMove({SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y});
                update_status = 0;
            }
        }, 0.2, 0, false);
    }
    
    void BuildScene::InsertSearchResult(int index, bool is_side) {
        if(result_data[index] == nullptr)
            return;
        auto data = result_data[index];
        std::shared_ptr<DeckCardData> ptr;
        if(!is_side)
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