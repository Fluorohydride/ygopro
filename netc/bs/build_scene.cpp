#include "utils/common.h"
#include "utils/filesystem.h"

#include "../render_base.h"
#include "../sgui.h"
#include "../image_mgr.h"
#include "../card_data.h"
#include "../deck_data.h"

#include "build_scene_handler.h"
#include "build_scene.h"

namespace ygopro
{
    std::array<texf4, 3> limit_tex;
    texf4 hmask;
    int32_t max_row_count = 0;
    int32_t main_row_count = 0;
    v2f card_size = {0.0f, 0.0f};
    v2f icon_size = {0.0f, 0.0f};
    float minx = 0.0f;
    float maxx = 0.0f;
    float main_y_spacing = 0.0f;
    float offsety[3] = {0.0f, 0.0f, 0.0f};
    float dx[3] = {0.0f, 0.0f, 0.0f};
    std::array<CardData*, 10> result_data;
    std::array<texf4, 10> result_tex;
    int32_t current_sel_result = -1;
    int32_t result_show_size = 0;
    v2i scene_size = {1, 1};
    DeckData current_deck;
    
    int32_t BuilderCard::GetTextureId() {
        return ImageMgr::Get().GetRawCardTexture()->GetTextureId();
    }
    
    void BuilderCard::RefreshVertices() {
        vertices.resize(8);
        indices.resize(12);
        vt2::Fill(&vertices[0], pos, {size.x, -size.y}, card_tex, 0xffffffff, hl);
        if(limit < 3) {
            auto& lti = limit_tex[limit];
            vt2::Fill(&vertices[4], pos + v2f{-0.01f, 0.01f}, {icon_size.x, -icon_size.y}, lti);
        }
    }
    
    void MiscRenderer::PushVerticesAll() {
        base::RenderObject2DLayout::PushVerticesAll();
        BeginPrimitive(GL_TRIANGLES, 0);
        std::array<vt2, 33 * 4> verts;
        auto msk = ImageMgr::Get().GetTexture("mmask");
        auto nbk = ImageMgr::Get().GetTexture("numback");
        float yrate = 1.0f - 40.0f / screen_size.y;
        float lx = 10.0f / screen_size.x * 2.0f - 1.0f;
        float rx = 0.5625f;
        float y0 = (0.95f + 1.0f) * yrate - 1.0f;
        float y1 = (offsety[0] - main_y_spacing * 3 - card_size.y + offsety[1]) / 2;
        float y2 = (offsety[1] - card_size.y + offsety[2]) / 2;
        float y3 = offsety[2] - card_size.y - 0.03f * yrate;
        float nw = 60.0f / screen_size.x;
        float nh = 60.0f / screen_size.y;
        float nx = 15.0f / screen_size.x * 2.0f - 1.0f;
        float ndy = 64.0f / screen_size.y;
        float th = 120.0f / screen_size.y;
        float my = offsety[0] - main_y_spacing * 3 - card_size.y + th;
        float ey = offsety[1] - card_size.y + th;
        float sy = offsety[2] - card_size.y + th;
        auto numblock = [&nw, &nh, &nbk](vt2* v, v2f pos, uint32_t cl1, uint32_t cl2, int32_t val) {
            vt2::Fill(&v[0], {pos.x, pos.y}, {nw, -nh}, nbk, cl1);
            if(val >= 10) {
                vt2::Fill(&v[4], {pos.x + nw * 0.1f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + (val % 100) / 10), cl2);
                vt2::Fill(&v[8], {pos.x + nw * 0.5f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + val % 10), cl2);
            } else
                vt2::Fill(&v[4], {pos.x + nw * 0.3f, pos.y - nh * 0.2f}, {nw * 0.4f, -nh * 0.6f}, ImageMgr::Get().GetCharTex(L'0' + val), cl2);
        };
        vt2::Fill(&verts[0], {lx, y0}, {rx - lx, y1 - y0}, msk, 0xc0ffffff);
        vt2::Fill(&verts[4], {lx, y1}, {rx - lx, y2 - y1}, msk, 0xc0c0c0c0);
        vt2::Fill(&verts[8], {lx, y2}, {rx - lx, y3 - y2}, msk, 0xc0808080);
        vt2::Fill(&verts[12], {nx, my}, {nw, -th}, ImageMgr::Get().GetTexture("main_t"), 0xff80ffff);
        vt2::Fill(&verts[16], {nx, ey}, {nw, -th}, ImageMgr::Get().GetTexture("extra_t"), 0xff80ffff);
        vt2::Fill(&verts[20], {nx, sy}, {nw, -th}, ImageMgr::Get().GetTexture("side_t"), 0xff80ffff);
        numblock(&verts[24], {nx, offsety[0] - ndy * 0}, 0xf03399ff, 0xff000000, current_deck.mcount);
        numblock(&verts[36], {nx, offsety[0] - ndy * 1}, 0xf0a0b858, 0xff000000, current_deck.scount);
        numblock(&verts[48], {nx, offsety[0] - ndy * 2}, 0xf09060bb, 0xff000000, current_deck.tcount);
        numblock(&verts[60], {nx, offsety[0] - ndy * 3}, 0xf0b050a0, 0xff000000, current_deck.fuscount);
        numblock(&verts[72], {nx, offsety[0] - ndy * 4}, 0xf0ffffff, 0xff000000, current_deck.syncount);
        numblock(&verts[84], {nx, offsety[0] - ndy * 5}, 0xf0000000, 0xffffffff, current_deck.xyzcount);
        numblock(&verts[96], {nx, my + card_size.y - th}, 0x80ffffff, 0xff000000, (int32_t)current_deck.main_deck.size());
        numblock(&verts[108], {nx, ey + card_size.y - th}, 0x80ffffff, 0xff000000, (int32_t)current_deck.extra_deck.size());
        numblock(&verts[120], {nx, sy + card_size.y - th}, 0x80ffffff, 0xff000000, (int32_t)current_deck.side_deck.size());
    }
    
    void CardRenderer::PushVerticesAll() {
    }
    
    BuildScene::BuildScene() {
        limit_tex[0] = ImageMgr::Get().GetTexture("limit0");
        limit_tex[1] = ImageMgr::Get().GetTexture("limit1");
        limit_tex[2] = ImageMgr::Get().GetTexture("limit2");
        hmask = ImageMgr::Get().GetTexture("cmask");
        result_data.fill(nullptr);
        bg_renderer = std::make_shared<base::SimpleTextureRenderer>();
        misc_renderer = std::make_shared<MiscRenderer>();
        card_renderer = std::make_shared<CardRenderer>();
        bg_renderer->SetShader(&base::Shader::GetDefaultShader());
        misc_renderer->SetShader(&base::Shader::GetDefaultShader());
        card_renderer->SetShader(&base::Shader::GetDefaultShader());
        PushObject(ImageMgr::Get());
        PushObject(bg_renderer.get());
        //PushObject(misc_renderer.get());
        //PushObject(card_renderer.get());
        PushObject(sgui::SGGUIRoot::GetSingleton());
    }
    
    BuildScene::~BuildScene() {
    }
    
    void BuildScene::Activate() {
        if(scene_handler)
            scene_handler->BeginHandler();
        RefreshAllCard();
    }
    
    bool BuildScene::Update() {
        if(input_handler)
            input_handler->UpdateInput();
        if(scene_handler)
            scene_handler->UpdateEvent();
        return IsActive();
    }
    
    bool BuildScene::Draw() {
        auto need_render = PrepareRender();
        if(need_render)
            Render();
        return need_render;
    }
    
    void BuildScene::SetSceneSize(v2i sz) {
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
        if((int32_t)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (int32_t)((current_deck.main_deck.size() - 1) / 4 + 1);
        dx[0] = (maxx - minx - card_size.x) / (main_row_count - 1);
        int32_t rc1 = std::max((int32_t)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc1 - 1);
        int32_t rc2 = std::max((int32_t)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc2 - 1);
        scene_size = sz;
        bg_renderer->SetScreenSize(sz);
        bg_renderer->SetViewport({0, 0, sz.x, sz.y});
        misc_renderer->SetScreenSize(sz);
        misc_renderer->SetViewport({0, 0, sz.x, sz.y});
        card_renderer->SetScreenSize(sz);
        card_renderer->SetViewport({0, 0, sz.x, sz.y});
        bg_renderer->ClearVertices();
        bg_renderer->AddVertices(ImageMgr::Get().GetRawBGTexture(), recti{0, 0, sz.x, sz.y}, ImageMgr::Get().GetTexture("bg"));
        UpdateAllCard();
    }
    
    recti BuildScene::GetScreenshotClip() {
        int32_t maxx = (int32_t)(scene_size.x * 0.795f) - 1;
        return {0, 40, maxx, scene_size.y - 40};
    }
    
    void BuildScene::ShowSelectedInfo(uint32_t pos, uint32_t index) {
        if(pos > 0 && pos < 4) {
            auto dcd = GetCard(pos, index);
            if(dcd != nullptr)
                std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->ShowCardInfo(dcd->data->code);
        } else if(pos == 4) {
            if(result_data[index] != 0)
                std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->ShowCardInfo(result_data[index]->code);
        }
    }
    
    void BuildScene::ShowCardInfo(uint32_t code) {
        std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->ShowCardInfo(code);
    }
    
    void BuildScene::HideCardInfo() {
        std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->HideCardInfo();
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
        UpdateAllCard();
    }
    
    void BuildScene::ShuffleDeck() {
        current_deck.Shuffle();
        UpdateAllCard();
    }
    
    void BuildScene::SetDeckDirty() {
        std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->SetDeckEdited();
    }
    
    bool BuildScene::LoadDeckFromFile(const std::wstring& file) {
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(FileSystem::WSTRToLocalFilename(file))) {
            ClearDeck();
            current_deck = tempdeck;
            for(auto& dcd : current_deck.main_deck) {
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
            }
            for(auto& dcd : current_deck.side_deck) {
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
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
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
            }
            for(auto& dcd : current_deck.extra_deck) {
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
            }
            for(auto& dcd : current_deck.side_deck) {
                auto bc = std::make_shared<BuilderCard>();
                bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
                dcd->builder_card = bc;
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
        current_deck.SaveToFile(FileSystem::WSTRToLocalFilename(file));
        return true;
    }
    
    std::string BuildScene::SaveDeckToString() {
        return std::move(current_deck.SaveToString());
    }
    
    void BuildScene::UpdateAllCard() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto ptr = current_deck.main_deck[i]->builder_card;
            auto bpos = ptr->pos;
            auto cpos = (v2f){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            auto action = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(1000, ptr, [bpos, cpos](BuilderCard* bc, double t) ->bool {
                bc->SetPos(bpos + (cpos - bpos) * t);
                return true;
            }, std::make_shared<TGenMove<int64_t>>(10, 10.0));
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto ptr = current_deck.extra_deck[i]->builder_card;
            auto bpos = ptr->pos;
            auto cpos = (v2f){minx + dx[1] * i, offsety[1]};
            auto action = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(1000, ptr, [bpos, cpos](BuilderCard* bc, double t) ->bool {
                bc->SetPos(bpos + (cpos - bpos) * t);
                return true;
            }, std::make_shared<TGenMove<int64_t>>(10, 10.0));
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto ptr = current_deck.side_deck[i]->builder_card;
            auto bpos = ptr->pos;
            auto cpos = (v2f){minx + dx[2] * i, offsety[2]};
            auto action = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(1000, ptr, [bpos, cpos](BuilderCard* bc, double t) ->bool {
                bc->SetPos(bpos + (cpos - bpos) * t);
                return true;
            }, std::make_shared<TGenMove<int64_t>>(10, 10.0));
        }
    }
    
    void BuildScene::RefreshParams() {
        main_row_count = max_row_count;
        if((int32_t)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (int32_t)((current_deck.main_deck.size() - 1) / 4 + 1);
        dx[0] = (maxx - minx - card_size.x) / (main_row_count - 1);
        int32_t rc1 = std::max((int32_t)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc1 - 1);
        int32_t rc2 = std::max((int32_t)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (maxx - minx - card_size.x) / (rc2 - 1);
    }
    
    void BuildScene::RefreshAllCard() {
        size_t deck_sz = current_deck.main_deck.size() + current_deck.extra_deck.size() + current_deck.side_deck.size();
        if(deck_sz == 0)
            return;
        RefreshParams();
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[0] * (i % main_row_count), offsety[0] - main_y_spacing * (i / main_row_count)};
            auto ptr = current_deck.main_deck[i]->builder_card;
            ptr->SetPos(cpos);
            ptr->SetSize(card_size);
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[1] * i, offsety[1]};
            auto ptr = current_deck.extra_deck[i]->builder_card;
            ptr->SetPos(cpos);
            ptr->SetSize(card_size);
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto cpos = (v2f){minx + dx[2] * i, offsety[2]};
            auto ptr = current_deck.side_deck[i]->builder_card;
            ptr->SetPos(cpos);
            ptr->SetSize(card_size);
        }
    }
    
    void BuildScene::UpdateResult() {
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
        std::array<vt2, 160> verts;
        for(int32_t i = 0; i < 10 ; ++i) {
            if(result_data[i] == nullptr)
                continue;
            result_show_size++;
            auto pvert = &verts[i * 16];
            uint32_t cl = (i == current_sel_result) ? 0xc0ffffff : 0xc0000000;
            vt2::Fill(&pvert[0], {left + (i % 2) * width, top - (i / 2) * height}, {width, -height}, hmask, cl);
            CardData* pdata = result_data[i];
            vt2::Fill(&pvert[4], {left + (i % 2) * width + width / 2 - cwidth / 2, top - (i / 2) * height - offy}, {cwidth, -cheight}, result_tex[i]);
            uint32_t lmt = LimitRegulationMgr::Get().GetCardLimitCount(pdata->alias ? pdata->alias : pdata->code);
            if(lmt < 3) {
                vt2::Fill(&pvert[8], {left + (i % 2) * width + width / 2 - cwidth / 2 - 0.01f, top - (i / 2) * height - offy + 0.01f},
                                   {iwidth, -iheight}, limit_tex[lmt]);
            }
        }
    }
    
    void BuildScene::ChangeRegulation(int32_t index, int32_t view_regulation) {
        LimitRegulationMgr::Get().SetLimitRegulation(index);
        if(view_regulation)
            ViewRegulation(view_regulation - 1);
        else
            LimitRegulationMgr::Get().GetDeckCardLimitCount(current_deck);
        RefreshAllCard();
    }
    
    void BuildScene::ViewRegulation(int32_t limit) {
        ClearDeck();
        LimitRegulationMgr::Get().LoadCurrentListToDeck(current_deck, limit);
        for(auto& dcd : current_deck.main_deck) {
            auto bc = std::make_shared<BuilderCard>();
            bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            dcd->builder_card = bc;
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto bc = std::make_shared<BuilderCard>();
            bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            dcd->builder_card = bc;
        }
        for(auto& dcd : current_deck.side_deck) {
            auto bc = std::make_shared<BuilderCard>();
            bc->card_tex = ImageMgr::Get().GetCardTexture(dcd->data->code);
            dcd->builder_card = bc;
        }
        RefreshAllCard();
    }
    
    void BuildScene::RefreshSearchResult(const std::array<CardData*, 10> new_results) {
        for(int32_t i = 0; i < 10; ++i) {
            if(new_results[i] != nullptr)
                result_tex[i] = ImageMgr::Get().GetCardTexture(new_results[i]->code);
        }
        for(int32_t i = 0; i < 10; ++i) {
            if(result_data[i] != nullptr)
                ImageMgr::Get().UnloadCardTexture(result_data[i]->code);
            result_data[i] = new_results[i];
        }
    }
    
    void BuildScene::SetCurrentSelection(int32_t sel, bool show_info) {
        if(sel != current_sel_result) {
            current_sel_result = sel;
            if(current_sel_result >= 0 && show_info && result_data[current_sel_result] != nullptr)
                std::static_pointer_cast<BuildSceneHandler>(GetSceneHandler())->ShowCardInfo(result_data[current_sel_result]->code);
        }
    }
    
    void BuildScene::MoveCard(int32_t pos, int32_t index) {
        auto dcd = GetCard(pos, index);
        if(dcd == nullptr)
            return;
        if(pos == 1) {
            auto ptr = current_deck.main_deck[index]->builder_card;
            ptr->SetHL(0);
            current_deck.side_deck.push_back(current_deck.main_deck[index]);
            current_deck.main_deck.erase(current_deck.main_deck.begin() + index);
        } else if(pos == 2) {
            auto ptr = current_deck.extra_deck[index]->builder_card;
            ptr->SetHL(0);
            current_deck.side_deck.push_back(current_deck.extra_deck[index]);
            current_deck.extra_deck.erase(current_deck.extra_deck.begin() + index);
        } else if(dcd->data->type & 0x802040) {
            auto ptr = current_deck.side_deck[index]->builder_card;
            ptr->SetHL(0);
            current_deck.extra_deck.push_back(current_deck.side_deck[index]);
            current_deck.side_deck.erase(current_deck.side_deck.begin() + index);
        } else {
            auto ptr = current_deck.side_deck[index]->builder_card;
            ptr->SetHL(0);
            current_deck.main_deck.push_back(current_deck.side_deck[index]);
            current_deck.side_deck.erase(current_deck.side_deck.begin() + index);
        }
        current_deck.CalCount();
        RefreshParams();
        UpdateAllCard();
        SetDeckDirty();
        if(input_handler)
            input_handler->MouseMove(SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y);
    }
    
    void BuildScene::RemoveCard(int32_t pos, int32_t index) {
//        if(update_status == 1)
//            return;
//        update_status = 1;
//        auto dcd = GetCard(pos, index);
//        if(dcd == nullptr)
//            return;
//        uint32_t code = dcd->data->code;
//        auto ptr = std::static_pointer_cast<BuilderCard>(dcd->extra);
//        v2f dst = ptr->pos.Get() + v2f{card_size.x / 2, -card_size.y / 2};
//        v2f dsz = {0.0f, 0.0f};
//        ptr->pos.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->pos.Get(), dst, SceneMgr::Get().GetGameTime(), 0.2));
//        ptr->size.SetAnimator(std::make_shared<LerpAnimator<v2f, TGenLinear>>(ptr->size.Get(), dsz, SceneMgr::Get().GetGameTime(), 0.2));
//        ptr->show_limit = false;
//        ptr->show_exclusive = false;
//        AddUpdatingCard(dcd);
//        GetSceneHandler<BuildSceneHandler>()->RegisterEvent([pos, index, code, this]() {
//            if(current_deck.RemoveCard(pos, index)) {
//                ImageMgr::Get().UnloadCardTexture(code);
//                RefreshParams();
//                RefreshAllIndex();
//                UpdateAllCard();
//                SetDeckDirty();
//                if(input_handler)
//                    input_handler->MouseMove({SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y});
//                update_status = 0;
//            }
//        }, 0.2, 0, false);
    }
    
    void BuildScene::InsertSearchResult(int32_t index, bool is_side) {
        if(result_data[index] == nullptr)
            return;
        auto data = result_data[index];
        std::shared_ptr<DeckCardData> ptr;
        if(!is_side)
            ptr = current_deck.InsertCard(1, -1, data->code, true);
        else
            ptr = current_deck.InsertCard(3, -1, data->code, true);
        if(ptr != nullptr) {
            auto bc = std::make_shared<BuilderCard>();
            bc->card_tex = ImageMgr::Get().GetCardTexture(data->code);
            auto mpos = SceneMgr::Get().GetMousePosition();
            bc->pos = (v2f){(float)mpos.x / scene_size.x * 2.0f - 1.0f, 1.0f - (float)mpos.y / scene_size.y * 2.0f};
            bc->size = card_size;
            bc->hl = 0;
            ptr->builder_card = bc;
            RefreshParams();
            UpdateAllCard();
            SetDeckDirty();
        }
    }
    
    std::shared_ptr<DeckCardData> BuildScene::GetCard(int32_t pos, int32_t index) {
        if(index < 0)
            return nullptr;
        if(pos == 1) {
            if(index >= (int32_t)current_deck.main_deck.size())
                return nullptr;
            return current_deck.main_deck[index];
        } else if(pos == 2) {
            if(index >= (int32_t)current_deck.extra_deck.size())
                return nullptr;
            return current_deck.extra_deck[index];
        } else if(pos == 3) {
            if(index >= (int32_t)current_deck.side_deck.size())
                return nullptr;
            return current_deck.side_deck[index];
        }
        return nullptr;
    }
    
    std::pair<int32_t, int32_t> BuildScene::GetHoverPos(int32_t posx, int32_t posy) {
        if(posx >= (int32_t)(scene_size.x * 0.795f) && posx <= scene_size.x - 10 && posy >= 110 && posy <= scene_size.y - 10) {
            int32_t sel = (int32_t)((posy - 110.0f) / ((scene_size.y - 120.0f) / 5.0f)) * 2;
            if(sel > 8)
                sel = 8;
            sel += (posx >= ((int32_t)(scene_size.x * 0.795f) + scene_size.x - 10) / 2) ? 1 : 0;
            return std::make_pair(4, sel);
        }
        float x = (float)posx / scene_size.x * 2.0f - 1.0f;
        float y = 1.0f - (float)posy / scene_size.y * 2.0f;
        if(x >= minx && x <= maxx) {
            if(y <= offsety[0] && y >= offsety[0] - main_y_spacing * 4) {
                uint32_t row = (uint32_t)((offsety[0] - y) / main_y_spacing);
                if(row > 3)
                    row = 3;
                int32_t index = 0;
                if(x > maxx - card_size.x)
                    index = main_row_count - 1;
                else
                    index = (int32_t)((x - minx) / dx[0]);
                int32_t cindex = index;
                index += row * main_row_count;
                if(index >= (int32_t)current_deck.main_deck.size())
                    return std::make_pair(0, 0);
                else {
                    if(y < offsety[0] - main_y_spacing * row - card_size.y || x > minx + cindex * dx[0] + card_size.x)
                        return std::make_pair(0, 0);
                    else
                        return std::make_pair(1, index);
                }
            } else if(y <= offsety[1] && y >= offsety[1] - card_size.y) {
                int32_t rc = std::max((int32_t)current_deck.extra_deck.size(), max_row_count);
                int32_t index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int32_t)((x - minx) / dx[1]);
                if(index >= (int32_t)current_deck.extra_deck.size())
                    return std::make_pair(0, 0);
                else {
                    if(x > minx + index * dx[1] + card_size.x)
                        return std::make_pair(0, 0);
                    else
                        return std::make_pair(2, index);
                }
            } else if(y <= offsety[2] && y >= offsety[2] - card_size.y) {
                int32_t rc = std::max((int32_t)current_deck.side_deck.size(), max_row_count);
                int32_t index = 0;
                if(x > maxx - card_size.x)
                    index = rc - 1;
                else
                    index = (int32_t)((x - minx) / dx[2]);
                if(index >= (int32_t)current_deck.side_deck.size())
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