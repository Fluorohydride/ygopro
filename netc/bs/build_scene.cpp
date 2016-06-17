#include "utils/common.h"
#include "utils/filesystem.h"

#include "../render_base.h"
#include "../sgui.h"
#include "../image_mgr.h"
#include "../card_data.h"
#include "../deck_data.h"
#include "../config.h"

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
    v2f res_card_size = {0.0f, 0.0f};
    v2f res_offset = {0.0f, 0.0f};
    v2i res_count = {1, 1};
    rectf deck_area = {0.0f};
    rectf res_area = {0.0f};
    float icon_offset = 0.0f;
    float x_left = 0.0f;
    float x_right = 0.0f;
    float main_y_spacing = 0.0f;
    float offsety[3] = {0.0f, 0.0f, 0.0f};
    float dx[3] = {0.0f, 0.0f, 0.0f};
    int64_t update_ver = 0;
    bool remove_lock = false;
    DeckData current_deck;
    std::vector<std::shared_ptr<DeckCardData>> result_card;
    
    int32_t BuilderCard::GetTextureId() {
        return ImageMgr::Get().GetRawCardTexture()->GetTextureId();
    }
    
    void BuilderCard::RefreshVertices() {
        vertices.resize(8);
        indices.resize(12);
        vt2::Fill(vertices.data(), pos, {size.x, -size.y}, card_tex, 0xffffffff, hl);
        if(limit < 3) {
            auto& lti = limit_tex[limit];
            vt2::Fill(&vertices[4], pos + v2f{-icon_offset, icon_offset}, {icon_size.x, -icon_size.y}, lti);
            vt2::GenQuadIndex(indices.data(), 2, vert_index);
        } else {
            vt2::GenQuadIndex(indices.data(), 1, vert_index);
            for(int32_t i = 6; i < 12; ++i)
                indices[i] = 0;
        }
    }
    
    v2f BuilderCard::GetCurrentPos(CardLocation loc, int32_t seq) {
        switch(loc) {
            case CardLocation::Main:
                return {x_left + dx[0] * (seq % main_row_count), offsety[0] - main_y_spacing * (seq / main_row_count)};
            case CardLocation::Extra:
                return {x_left + dx[1] * seq, offsety[1]};
            case CardLocation::Side:
                return {x_left + dx[2] * seq, offsety[2]};
            case CardLocation::Result: {
                int32_t y = seq / res_count.x;
                int32_t x = seq % res_count.x;
                return {res_area.left + x * res_offset.x, res_area.top - y * res_offset.y};
            }
            default:
                return {0.0f, 0.0f};
        }
    }
    
    void BuilderCard::SetNewIndex(int16_t idx) {
        index_index = idx;
        SetUpdate();
    }
    
    void BuilderCard::PushIndices(std::vector<int16_t>& idx) {
        idx.insert(idx.end(), indices.begin(), indices.end());
    }
    
    void BuilderCard::LoadCardTexture(uint32_t cid) {
        std::weak_ptr<BuilderCard> wptr = shared_from_this();
        card_tex = ImageMgr::Get().GetCardTexture(cid, [wptr](texf4 tex) {
            auto ptr = wptr.lock();
            if(ptr) {
                ptr->card_tex = tex;
                ptr->SetUpdate();
            }
        });
        SetUpdate();
    }
    
    void MiscRenderer::PushVerticesAll() {
        static const int32_t quad_count = 34;
        base::RenderObject2DLayout::PushVerticesAll();
        BeginPrimitive(GL_TRIANGLES, ImageMgr::Get().GetRawMiscTexture()->GetTextureId());
        std::array<vt2, quad_count * 4> verts;
        std::array<int16_t, quad_count * 6> indices;
        auto msk = ImageMgr::Get().GetTexture("mmask");
        auto nbk = ImageMgr::Get().GetTexture("numback");
        float lx = deck_area.left;
        float rx = deck_area.left + deck_area.width;
        float y0 = deck_area.top;
        float y1 = (offsety[0] - main_y_spacing * 3 - card_size.y + offsety[1]) / 2;
        float y2 = (offsety[1] - card_size.y + offsety[2]) / 2;
        float y3 = deck_area.top - deck_area.height;
        float nw = 60.0f / screen_size.x;
        float nh = 60.0f / screen_size.y;
        float nx = lx + 10.0f / screen_size.x * 2.0f;
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
        
        vt2::Fill(&verts[132], {res_area.left, res_area.top}, {res_area.width, -res_area.height}, msk, 0x80000000);
        vt2::GenQuadIndex(indices.data(), quad_count);
        PushVertices(verts.data(), indices.data(), quad_count * 4, quad_count * 6);
    }
    
    void CardRenderer::PushVerticesAll() {
        base::RenderObject2DLayout::PushVerticesAll();
        for(auto& iter : current_deck.main_deck)
            iter->builder_card->PushVertices();
        for(auto& iter : current_deck.extra_deck)
            iter->builder_card->PushVertices();
        for(auto& iter : current_deck.side_deck)
            iter->builder_card->PushVertices();
        for(auto& iter : result_card)
            if(iter)
                iter->builder_card->PushVertices();
    }
    
    BuildScene::BuildScene(base::Shader* _2dshader) {
        limit_tex[0] = ImageMgr::Get().GetTexture("limit0");
        limit_tex[1] = ImageMgr::Get().GetTexture("limit1");
        limit_tex[2] = ImageMgr::Get().GetTexture("limit2");
        hmask = ImageMgr::Get().GetTexture("cmask");
        bg_renderer = std::make_shared<base::SimpleTextureRenderer>();
        misc_renderer = std::make_shared<MiscRenderer>();
        card_renderer = std::make_shared<CardRenderer>();
        bg_renderer->SetShader(_2dshader);
        misc_renderer->SetShader(_2dshader);
        card_renderer->SetShader(_2dshader);
        PushObject(bg_renderer.get());
        PushObject(misc_renderer.get());
        PushObject(card_renderer.get());
        PushObject(sgui::SGGUIRoot::GetSingleton());
        bg_renderer->ClearVertices();
        bg_renderer->AddVertices(ImageMgr::Get().GetRawBGTexture(), rectf{-1.0f, 1.0f, 2.0f, -2.0f}, ImageMgr::Get().GetTexture("bg"));
        scene_handler = std::make_shared<BuildSceneHandler>(this);
    }
    
    BuildScene::~BuildScene() {
    }
    
    void BuildScene::Activate() {
        scene_handler->BeginHandler();
        RefreshAllCard();
    }
    
    void BuildScene::Terminate() {
        scene_handler->EndHandler();
        sgui::SGGUIRoot::GetSingleton().ClearChilds();
    }
    
    bool BuildScene::Update() {
        scene_handler->UpdateHandler();
        return IsActive();
    }
    
    bool BuildScene::Draw() {
        ImageMgr::Get().LoadCardTextureFromList(3);
        auto need_render = PrepareRender();
        if(need_render)
            Render();
        return need_render;
    }
    
    void BuildScene::SetSceneSize(v2i sz) {
        float h1 = 0.038333f;
        float h2 = 0.01f;
        auto& da_node = layoutCfg["deck area"];
        deck_area.left = da_node[0].to_integer() * 2.0f / sz.x + da_node[4].to_value<float>() * 2.0f - 1.0f;
        deck_area.top = 1.0f - da_node[1].to_integer() * 2.0f / sz.y - da_node[5].to_value<float>() * 2.0f;
        deck_area.width = da_node[2].to_integer() * 2.0f / sz.x + da_node[6].to_value<float>() * 2.0f;
        deck_area.height = da_node[3].to_integer() * 2.0f / sz.y + da_node[7].to_value<float>() * 2.0f;
        auto& sa_node = layoutCfg["search area"];
        res_area.left = sa_node[0].to_integer() * 2.0f / sz.x + sa_node[4].to_value<float>() * 2.0f - 1.0f;
        res_area.top = 1.0f - sa_node[1].to_integer() * 2.0f / sz.y - sa_node[5].to_value<float>() * 2.0f;
        res_area.width = sa_node[2].to_integer() * 2.0f / sz.x + sa_node[6].to_value<float>() * 2.0f;
        res_area.height = sa_node[3].to_integer() * 2.0f / sz.y + sa_node[7].to_value<float>() * 2.0f;
        float yrate = deck_area.height / 2;
        x_left = deck_area.left + 50.0f / sz.x * 2.0f;
        x_right = deck_area.left + deck_area.width - h1 * yrate * sz.y / sz.x;
        card_size = {0.2f * yrate * sz.y / sz.x, 0.29f * yrate};
        icon_size = {0.08f * yrate * sz.y / sz.x, 0.08f * yrate};
        icon_offset = 0.01f * yrate * sz.y / sz.x;
        main_y_spacing = 0.3f * yrate;
        offsety[0] = deck_area.top - h1 * yrate;
        offsety[1] = offsety[0] - card_size.y * 4 - h2 * yrate * 3 - h1 * yrate * 2;
        offsety[2] = offsety[1] - card_size.y - h1 * yrate * 2;
        res_count = sgui::SGJsonUtil::ConvertVec2<int32_t>(layoutCfg["search count"], 0);
        res_count.x = res_count.x ? res_count.x : 1;
        res_count.y = res_count.y ? res_count.y : 1;
        float resw = res_area.width / res_count.x * 1.45f * sz.x / sz.y;
        float resh = res_area.height / res_count.y;
        res_card_size.y = std::max(resw, resh);
        res_card_size.x = res_card_size.y * sz.y / sz.x / 1.45f;
        res_offset.x = (res_count.x > 1) ? ((res_area.width - res_card_size.x) / (res_count.x - 1)) : 0;
        res_offset.y = (res_count.y > 1) ? ((res_area.height - res_card_size.y) / (res_count.y - 1)) : 0;
        RefreshParams();
        SetViewport({0, 0, sz.x, sz.y});
        bg_renderer->SetScreenSize(sz);
        misc_renderer->SetScreenSize(sz);
        card_renderer->SetScreenSize(sz);
        UpdateAllCard();
        RefreshResult();
    }
    
    recti BuildScene::GetScreenshotClip() {
        return {
            (int32_t)((deck_area.left + 1.0f) / 2.0f * viewport.width),
            (int32_t)((-deck_area.top + 1.0f) / 2.0f * viewport.height),
            (int32_t)((deck_area.width) / 2.0f * viewport.width),
            (int32_t)((deck_area.height) / 2.0f * viewport.height)
        };
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
        card_renderer->RequestRedraw();
    }
    
    void BuildScene::SortDeck() {
        if(remove_lock)
            return;
        current_deck.Sort();
        UpdateAllCard();
    }
    
    void BuildScene::ShuffleDeck() {
        if(remove_lock)
            return;
        current_deck.Shuffle();
        UpdateAllCard();
    }
    
    bool BuildScene::LoadDeckFromFile(const std::wstring& file) {
        if(remove_lock)
            return false;
        DeckData tempdeck;
        if(tempdeck.LoadFromFile(FileSystem::WSTRToLocalCharset(file))) {
            LoadDeck(tempdeck);
            return true;
        }
        return false;
    }
    
    bool BuildScene::LoadDeckFromString(const std::string& deck_string) {
        if(remove_lock)
            return false;
        DeckData tempdeck;
        if(deck_string.find("ydk://") == 0 && tempdeck.LoadFromString(deck_string.substr(6))) {
            LoadDeck(tempdeck);
            return true;
        }
        return false;
    }
    
    bool BuildScene::SaveDeckToFile(const std::wstring& file) {
        auto deckfile = file;
        if(deckfile.find(L".ydk") != deckfile.length() - 4)
            deckfile.append(L".ydk");
        current_deck.SaveToFile(FileSystem::WSTRToLocalCharset(file));
        return true;
    }
    
    std::string BuildScene::SaveDeckToString() {
        return current_deck.SaveToString();
    }
    
    void BuildScene::LoadDeck(DeckData& new_deck) {
        ClearDeck();
        current_deck = new_deck;
        for(auto& dcd : current_deck.main_deck) {
            auto bc = card_renderer->NewSharedObject<BuilderCard>();
            bc->LoadCardTexture(dcd->data->code);
            bc->SetLimit(dcd->limit);
            dcd->builder_card = bc;
        }
        for(auto& dcd : current_deck.extra_deck) {
            auto bc = card_renderer->NewSharedObject<BuilderCard>();
            bc->LoadCardTexture(dcd->data->code);
            bc->SetLimit(dcd->limit);
            dcd->builder_card = bc;
        }
        for(auto& dcd : current_deck.side_deck) {
            auto bc = card_renderer->NewSharedObject<BuilderCard>();
            bc->LoadCardTexture(dcd->data->code);
            bc->SetLimit(dcd->limit);
            dcd->builder_card = bc;
        }
        RefreshAllCard();
    }
    
    void BuildScene::UpdateAllCard() {
        update_ver = SceneMgr::Get().GetSysClock();
        int64_t ver = update_ver;
        auto cb = [this, ver](std::vector<std::shared_ptr<DeckCardData>>& lst, CardLocation loc) {
            for(size_t i = 0; i < lst.size(); ++i) {
                auto ptr = lst[i]->builder_card;
                auto bpos = ptr->pos;
                auto cpos = ptr->GetCurrentPos(loc, (int32_t)i);
                ptr->SetSize(card_size);
                if(std::abs((bpos.x - cpos.x) / bpos.x) > 0.001f || std::abs((bpos.y - cpos.y) / bpos.y) > 0.001f) {
                    auto action = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(1000, ptr, [bpos, cpos, ver](BuilderCard* bc, double t) ->bool {
                        if(ver != update_ver)
                            return false;
                        bc->SetPos(bpos + (cpos - bpos) * t);
                        return true;
                    }, std::make_shared<TGenMove<int64_t>>(1000, 0.01));
                    SceneMgr::Get().PushAction(action);
                }
                ptr->SetLimit(lst[i]->limit);
            }
        };
        cb(current_deck.main_deck, CardLocation::Main);
        cb(current_deck.extra_deck, CardLocation::Extra);
        cb(current_deck.side_deck, CardLocation::Side);
        card_renderer->RequestRedraw();
    }
    
    void BuildScene::RefreshParams() {
        max_row_count = (x_right - x_left - card_size.x) / (card_size.x * 1.1f);
        main_row_count = max_row_count;
        if(main_row_count < 10)
            main_row_count = 10;
        if((int32_t)current_deck.main_deck.size() > main_row_count * 4)
            main_row_count = (int32_t)((current_deck.main_deck.size() - 1) / 4 + 1);
        dx[0] = (x_right - x_left - card_size.x) / (main_row_count - 1);
        int32_t rc1 = std::max((int32_t)current_deck.extra_deck.size(), max_row_count);
        dx[1] = (rc1 == 1) ? 0.0f : (x_right - x_left - card_size.x) / (rc1 - 1);
        if(dx[1] > card_size.x * 1.1f)
            dx[1] = card_size.x * 1.1f;
        int32_t rc2 = std::max((int32_t)current_deck.side_deck.size(), max_row_count);
        dx[2] = (rc2 == 1) ? 0.0f : (x_right - x_left - card_size.x) / (rc2 - 1);
        if(dx[2] > card_size.x * 1.1f)
            dx[2] = card_size.x * 1.1f;
        misc_renderer->RequestRedraw();
    }
    
    void BuildScene::RefreshAllCard() {
        RefreshParams();
        for(size_t i = 0; i < current_deck.main_deck.size(); ++i) {
            auto ptr = current_deck.main_deck[i]->builder_card;
            ptr->SetPos(ptr->GetCurrentPos(CardLocation::Main, (int32_t)i));
            ptr->SetSize(card_size);
            ptr->SetLimit(current_deck.main_deck[i]->limit);
        }
        for(size_t i = 0; i < current_deck.extra_deck.size(); ++i) {
            auto ptr = current_deck.extra_deck[i]->builder_card;
            ptr->SetPos(ptr->GetCurrentPos(CardLocation::Extra, (int32_t)i));
            ptr->SetSize(card_size);
            ptr->SetLimit(current_deck.extra_deck[i]->limit);
        }
        for(size_t i = 0; i < current_deck.side_deck.size(); ++i) {
            auto ptr = current_deck.side_deck[i]->builder_card;
            ptr->SetPos(ptr->GetCurrentPos(CardLocation::Side, (int32_t)i));
            ptr->SetSize(card_size);
            ptr->SetLimit(current_deck.side_deck[i]->limit);
        }
        card_renderer->RequestRedraw();
    }
    
    void BuildScene::RefreshResult() {
        for(size_t i = 0; i < result_card.size(); ++i) {
            if(result_card[i]) {
                auto ptr = result_card[i]->builder_card;
                ptr->SetPos(ptr->GetCurrentPos(CardLocation::Result, (int32_t)i));
                ptr->SetSize(res_card_size);
                ptr->SetLimit(result_card[i]->limit);
            }
        }
        card_renderer->RequestRedraw();
    }
    
    void BuildScene::HighlightCode(int32_t code) {
        auto fun = [code](std::vector<std::shared_ptr<DeckCardData>>& vec) {
            for(auto& iter : vec)
                if(iter->data->GetRawCode() == code)
                    iter->builder_card->SetHL(0xc0ffffff);
        };
        fun(current_deck.main_deck);
        fun(current_deck.extra_deck);
        fun(current_deck.side_deck);
    }
    
    void BuildScene::HighlightCancel() {
        auto fun = [](std::vector<std::shared_ptr<DeckCardData>>& vec) {
            for(auto& iter : vec)
                if(iter->builder_card->hl != 0)
                    iter->builder_card->SetHL(0);
        };
        fun(current_deck.main_deck);
        fun(current_deck.extra_deck);
        fun(current_deck.side_deck);
    }
    
    void BuildScene::ChangeRegulation(int32_t index, int32_t view_regulation) {
        if(remove_lock)
            return;
        LimitRegulationMgr::Get().SetLimitRegulation(index);
        if(view_regulation)
            ViewRegulation(view_regulation - 1);
        else
            LimitRegulationMgr::Get().GetDeckCardLimitCount(current_deck);
        for(auto& ptr : result_card) {
            if(ptr)
                ptr->limit = LimitRegulationMgr::Get().GetCardLimitCount(ptr->data->code);
        }
        RefreshAllCard();
        RefreshResult();
    }
    
    void BuildScene::ViewRegulation(int32_t limit) {
        if(remove_lock)
            return;
        DeckData new_deck;
        LimitRegulationMgr::Get().LoadCurrentListToDeck(new_deck, limit);
        LoadDeck(new_deck);
    }
    
    void BuildScene::MoveCard(CardLocation pos, int32_t index) {
        auto dcd = GetCard(pos, index);
        if(dcd == nullptr)
            return;
        switch(pos) {
            case CardLocation::Main: {
                auto ptr = current_deck.main_deck[index]->builder_card;
                ptr->SetHL(0);
                current_deck.side_deck.push_back(current_deck.main_deck[index]);
                current_deck.main_deck.erase(current_deck.main_deck.begin() + index);
                break;
            }
            case CardLocation::Extra: {
                auto ptr = current_deck.extra_deck[index]->builder_card;
                ptr->SetHL(0);
                current_deck.side_deck.push_back(current_deck.extra_deck[index]);
                current_deck.extra_deck.erase(current_deck.extra_deck.begin() + index);
                break;
            }
            case CardLocation::Side: {
                if(dcd->data->type & 0x802040) {
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
                break;
            }
            default:
                break;
        }
        current_deck.CalCount();
        RefreshParams();
        UpdateAllCard();
    }
    
    void BuildScene::RemoveCard(CardLocation pos, int32_t index) {
        if(remove_lock)
            return;
        remove_lock = true;
        auto dcd = GetCard(pos, index);
        if(dcd == nullptr)
            return;
        uint32_t code = dcd->data->code;
        auto ptr = dcd->builder_card;
        v2f cpos = ptr->pos;
        v2f csz = ptr->size;
        v2f dst = ptr->pos + v2f{card_size.x / 2, -card_size.y / 2};
        v2f dsz = {0.0f, 0.0f};
        ptr->SetLimit(3);
        auto rm_act = std::make_shared<LerpAnimator<int64_t, BuilderCard>>(200, ptr, [cpos, csz, dst, dsz](BuilderCard* bc, double t) ->bool {
            bc->SetPos(cpos + (dst - cpos) * t);
            bc->SetSize(csz + (dsz - csz) * t);
            return true;
        }, std::make_shared<TGenLinear<int64_t>>(200));
        auto cb_act = std::make_shared<ActionCallback<int64_t>>([pos, index, code, this]() {
            auto ptr = GetCard(pos, index)->builder_card;
            if(current_deck.RemoveCard(pos, index)) {
                ImageMgr::Get().UnloadCardTexture(code);
                current_deck.CalCount();
                RefreshParams();
                UpdateAllCard();
                card_renderer->DeleteObject(ptr.get());
                remove_lock = false;
                if(scene_handler)
                    scene_handler->MouseMove(SceneMgr::Get().GetMousePosition().x, SceneMgr::Get().GetMousePosition().y);
            }
        });
        SceneMgr::Get().PushAction(std::make_shared<ActionSequence<int64_t>>(rm_act, cb_act));
    }
    
    void BuildScene::UpdateResult(const std::vector<CardData*> new_results) {
        std::vector<std::shared_ptr<DeckCardData>> result;
        result.resize(new_results.size());
        for(size_t i = 0; i < new_results.size(); ++i) {
            if(new_results[i] != nullptr) {
                result[i] = std::make_shared<DeckCardData>(new_results[i], LimitRegulationMgr::Get().GetCardLimitCount(new_results[i]->code));
                auto bc = card_renderer->NewSharedObject<BuilderCard>();
                bc->LoadCardTexture(new_results[i]->code);
                result[i]->builder_card = bc;
            }
        }
        for(auto& rc : result_card) {
            if(rc!= nullptr)
                ImageMgr::Get().UnloadCardTexture(rc->data->code);
        }
        result_card.clear();
        result_card = std::move(result);
        RefreshResult();
    }
    
    void BuildScene::InsertSearchResult(int32_t index, bool is_side) {
        if(index >= result_card.size() || result_card[index] == nullptr)
            return;
        auto dcd = result_card[index];
        auto ptr = InsertCardFromPos(dcd->data->code, dcd->builder_card->pos, is_side);
        if(ptr)
            ptr->builder_card->SetHL(0xc0ffffff);
    }
    
    std::shared_ptr<DeckCardData> BuildScene::InsertCardFromPos(int32_t code, v2f pos, bool is_side) {
        std::shared_ptr<DeckCardData> ptr;
        if(!is_side)
            ptr = current_deck.InsertCard(CardLocation::Main, -1, code, true);
        else
            ptr = current_deck.InsertCard(CardLocation::Side, -1, code, true);
        if(ptr != nullptr) {
            auto bc = card_renderer->NewSharedObject<BuilderCard>();
            bc->LoadCardTexture(code);
            bc->SetPos(pos);
            ptr->builder_card = bc;
            RefreshParams();
            UpdateAllCard();
        }
        return ptr;
    }
    
    std::shared_ptr<DeckCardData> BuildScene::GetCard(CardLocation pos, int32_t index) {
        if(index < 0)
            return nullptr;
        switch(pos) {
            case CardLocation::Main:
                if(index >= (int32_t)current_deck.main_deck.size())
                    return nullptr;
                return current_deck.main_deck[index];
            case CardLocation::Extra:
                if(index >= (int32_t)current_deck.extra_deck.size())
                    return nullptr;
                return current_deck.extra_deck[index];
            case CardLocation::Side:
                if(index >= (int32_t)current_deck.side_deck.size())
                    return nullptr;
                return current_deck.side_deck[index];
            case CardLocation::Result:
                if(index >= (int32_t)result_card.size())
                    return nullptr;
                return result_card[index];
            default:
                return nullptr;
        }
        return nullptr;
    }
    
    std::pair<CardLocation, int32_t> BuildScene::GetHoverPos(int32_t posx, int32_t posy) {
        float x = (float)posx / viewport.width * 2.0f - 1.0f;
        float y = 1.0f - (float)posy / viewport.height * 2.0f;
        if(x >= res_area.left && x <= res_area.left + res_area.width && y <= res_area.top && y >= res_area.top - res_area.height) {
            int32_t rx = (x - res_area.left) / res_offset.x;
            int32_t ry = (res_area.top - y) / res_offset.y;
            if(rx >= res_count.x)
                rx = res_count.x - 1;
            if(ry >= res_count.y)
                ry = res_count.y - 1;
            return std::make_pair(CardLocation::Result, rx + ry * res_count.x);
        }
        if(x >= x_left && x <= x_right) {
            if(y <= offsety[0] && y >= offsety[0] - main_y_spacing * 4) {
                uint32_t row = (uint32_t)((offsety[0] - y) / main_y_spacing);
                if(row > 3)
                    row = 3;
                int32_t index = 0;
                if(x > x_right - card_size.x)
                    index = main_row_count - 1;
                else
                    index = (int32_t)((x - x_left) / dx[0]);
                int32_t cindex = index;
                index += row * main_row_count;
                if(index >= (int32_t)current_deck.main_deck.size()) {
                    if(!current_deck.main_deck.empty()) {
                        auto bc = current_deck.main_deck.back()->builder_card.get();
                        if(x >= bc->pos.x && x <= bc->pos.x + card_size.x && y <= bc->pos.y && y >= bc->pos.y - card_size.y)
                            return std::make_pair(CardLocation::Main, (int32_t)current_deck.main_deck.size() - 1);
                    }
                    return std::make_pair(CardLocation::Null, 0);
                } else {
                    if(y < offsety[0] - main_y_spacing * row - card_size.y || x > x_left + cindex * dx[0] + card_size.x)
                        return std::make_pair(CardLocation::Null, 0);
                    else
                        return std::make_pair(CardLocation::Main, index);
                }
            } else if(y <= offsety[1] && y >= offsety[1] - card_size.y) {
                int32_t rc = std::max((int32_t)current_deck.extra_deck.size(), max_row_count);
                int32_t index = 0;
                if(x > x_right - card_size.x)
                    index = rc - 1;
                else
                    index = (int32_t)((x - x_left) / dx[1]);
                if(index >= (int32_t)current_deck.extra_deck.size()) {
                    if(!current_deck.extra_deck.empty()) {
                        auto bc = current_deck.extra_deck.back()->builder_card.get();
                        if(x >= bc->pos.x && x <= bc->pos.x + card_size.x)
                            return std::make_pair(CardLocation::Extra, (int32_t)current_deck.extra_deck.size() - 1);
                    }
                    return std::make_pair(CardLocation::Null, 0);
                } else {
                    if(x > x_left + index * dx[1] + card_size.x)
                        return std::make_pair(CardLocation::Null, 0);
                    else
                        return std::make_pair(CardLocation::Extra, index);
                }
            } else if(y <= offsety[2] && y >= offsety[2] - card_size.y) {
                int32_t rc = std::max((int32_t)current_deck.side_deck.size(), max_row_count);
                int32_t index = 0;
                if(x > x_right - card_size.x)
                    index = rc - 1;
                else
                    index = (int32_t)((x - x_left) / dx[2]);
                if(index >= (int32_t)current_deck.side_deck.size()) {
                    if(!current_deck.side_deck.empty()) {
                        auto bc = current_deck.side_deck.back()->builder_card.get();
                        if(x >= bc->pos.x && x <= bc->pos.x + card_size.x)
                            return std::make_pair(CardLocation::Side, (int32_t)current_deck.side_deck.size() - 1);
                    }
                    return std::make_pair(CardLocation::Null, 0);
                } else {
                    if(x > x_left + index * dx[2] + card_size.x)
                        return std::make_pair(CardLocation::Null, 0);
                    else
                        return std::make_pair(CardLocation::Side, index);
                }
            }
        }
        return std::make_pair(CardLocation::Null, 0);
    }
    
}