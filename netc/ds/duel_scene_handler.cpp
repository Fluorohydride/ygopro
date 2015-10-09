#include "utils/common.h"

#include "../config.h"
#include "../sgui.h"
#include "../gui_extra.h"
#include "../image_mgr.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    int32_t host_player = 0;
    std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
    std::vector<std::shared_ptr<FieldCard>> m_zone[2];
    std::vector<std::shared_ptr<FieldCard>> s_zone[2];
    std::vector<std::shared_ptr<FieldCard>> deck[2];
    std::vector<std::shared_ptr<FieldCard>> hand[2];
    std::vector<std::shared_ptr<FieldCard>> extra[2];
    std::vector<std::shared_ptr<FieldCard>> grave[2];
    std::vector<std::shared_ptr<FieldCard>> banished[2];
    
    DuelSceneHandler::DuelSceneHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
        info_panel = std::make_shared<InfoPanel>();
    }
    
    DuelSceneHandler::~DuelSceneHandler() {
        ClearField();
    }
    
    void DuelSceneHandler::BeginHandler() {
        InitField();
        AddCard(83764718, 0, 0x2, 0, 0);
        AddCard(64496451, 0, 0x2, 0, 0);
        AddCard(57728570, 1, 0x2, 1, 0);
        AddCard(97268402, 1, 0x2, 1, 0);
        for(auto& iter : hand[0])
            iter->UpdatePosition(0);
        for(auto& iter : hand[1])
            iter->UpdatePosition(0);
    }
    
    bool DuelSceneHandler::UpdateHandler() {
        if(show_info_time) {
            auto pscene = duel_scene.lock();
            auto now = SceneMgr::Get().GetSysClock();
            if(now - show_info_time >= 500) {
                show_info_time = 0;
                auto mp = SceneMgr::Get().GetMousePosition();
                auto cpos = pscene->GetHoverCardPos(pscene->GetHoverPos(mp.x, mp.y));
                auto ptr = pscene->GetHoverCard(cpos.x, cpos.y, cpos.z);
                if(ptr && ptr->code)
                    info_panel->ShowInfo(ptr->code);
            }
        }
        if(!message_lock) {
            auto cmd = messages.PullCommand();
            if(cmd) {
                BufferUtil bu(cmd->msg, cmd->len);
                SolveMessage(cmd->msg_type, bu);
            }
        }
        return true;
    }

    void DuelSceneHandler::MouseMove(int32_t x, int32_t y) {
        auto pscene = duel_scene.lock();
        auto scene_size = SceneMgr::Get().GetSceneSize();
        if(btnDown[0]) {
            float ratex = (float)(x - btnPos[0].x) / scene_size.x * 2.0f;
            float ratey = (float)(y - btnPos[0].y) / scene_size.y * 2.0f;
            btnPos[0] = {x, y};
            pscene->UpdateViewOffset({ratex, ratey});
            show_info_time = 0;
        }
        if(btnDown[1]) {
            float rate = (float)(y - btnPos[1].y) / scene_size.y * 3.1415926f * 0.5f;
            btnPos[1] = {x, y};
            pscene->UpdateViewAngle(rate);
        }
        auto pblock = pre_block.lock();
        auto pcard = pre_card.lock();
        auto hp = pscene->GetHoverPos(x, y);
        std::shared_ptr<FieldBlock> hover_block = pscene->GetFieldBlock(hp.x, hp.y);
        auto cpos = pscene->GetHoverCardPos(pscene->GetHoverPos(x, y));
        auto hover_card = pscene->GetHoverCard(cpos.x, cpos.y, cpos.z);
        if(pblock != hover_block) {
            if(pblock) {
                pblock->SetHL(0);
                SceneMgr::Get().RemoveAction(pblock.get());
            }
            if(hover_block) {
                auto act = std::make_shared<LerpAnimator<int64_t, FieldBlock>>(0, hover_block, [](FieldBlock* fb, double t)->bool {
                    uint32_t alpha = (uint32_t)((t * 0.6f + 0.2f) * 255);
                    fb->SetHL((alpha << 24) | 0xffffff);
                    return true;
                }, std::make_shared<TGenPeriodicRet<int64_t>>(1000));
                SceneMgr::Get().PushAction(act, hover_block.get());
            }
            pre_block = hover_block;
        }
        if(pcard != hover_card) {
            if(pcard && pcard->card_loc == 0x2) {
                auto curoff = pcard->yoffset;
                auto act = std::make_shared<LerpAnimator<int64_t, FieldCard>>(0, pcard, [curoff](FieldCard* fb, double t)->bool {
                    fb->SetYOffset(curoff * (1.0 - t));
                    return true;
                }, std::make_shared<TGenMove<int64_t>>(curoff / 0.2f * 500, 0.01));
                SceneMgr::Get().RemoveAction(pcard.get(), 2);
                SceneMgr::Get().PushAction(act, pcard.get(), 2);
            }
            if(hover_card && hover_card->card_loc == 0x2) {
                auto act = std::make_shared<LerpAnimator<int64_t, FieldCard>>(0, hover_card, [](FieldCard* fb, double t)->bool {
                    fb->SetYOffset(0.2f * t);
                    return true;
                }, std::make_shared<TGenMove<int64_t>>(500, 0.01));
                SceneMgr::Get().PushAction(act, hover_card.get(), 2);
            }
            if(hover_card && hover_card->code && info_panel->IsOpen())
                show_info_time = SceneMgr::Get().GetSysClock() - 200;
            pre_card = hover_card;
        }
    }

    void DuelSceneHandler::MouseButtonDown(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(button < 2) {
            btnDown[button] = true;
            btnPos[button] = {x, y};
            if(button == 0)
                show_info_time = SceneMgr::Get().GetSysClock();
        }
    }

    void DuelSceneHandler::MouseButtonUp(int32_t button, int32_t mods, int32_t x, int32_t y) {
        if(button < 2) {
            btnDown[button] = false;
            if(button == 0)
                show_info_time = 0;
        }
    }

    void DuelSceneHandler::MouseWheel(float deltax, float deltay) {
        auto pscene = duel_scene.lock();
        pscene->UpdateViewRadius(deltay / 30.0f);
        pscene->UpdateParams();
    }
    
    void DuelSceneHandler::KeyDown(int32_t key, int32_t mods) {

    }
    
    void DuelSceneHandler::KeyUp(int32_t key, int32_t mods) {
        
    }
    
    void DuelSceneHandler::InitField() {
        for(int32_t i = 0 ; i < 17; ++i) {
            field_blocks[0][i] = duel_scene.lock()->CreateFieldBlock();
            field_blocks[1][i] = duel_scene.lock()->CreateFieldBlock();
        }
        field_blocks[0][0 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone1"]));
        field_blocks[0][1 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone2"]));
        field_blocks[0][2 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone3"]));
        field_blocks[0][3 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone4"]));
        field_blocks[0][4 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mzone5"]));
        field_blocks[0][5 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone1"]));
        field_blocks[0][6 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone2"]));
        field_blocks[0][7 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone3"]));
        field_blocks[0][8 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone4"]));
        field_blocks[0][9 ]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["szone5"]));
        field_blocks[0][10]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["fdzone"]));
        field_blocks[0][11]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["pzonel"]));
        field_blocks[0][12]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["pzoner"]));
        field_blocks[0][13]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["mdeck"]));
        field_blocks[0][14]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["exdeck"]));
        field_blocks[0][15]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["grave"]));
        field_blocks[0][16]->SetPosition(sgui::SGJsonUtil::ConvertRectf(layoutCfg["banish"]));
        field_blocks[0][0 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][1 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][2 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][3 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][4 ]->SetTexture(ImageMgr::Get().GetTexture("mzone"));
        field_blocks[0][5 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][6 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][7 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][8 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][9 ]->SetTexture(ImageMgr::Get().GetTexture("szone"));
        field_blocks[0][10]->SetTexture(ImageMgr::Get().GetTexture("fdzone"));
        field_blocks[0][11]->SetTexture(ImageMgr::Get().GetTexture("pzonel"));
        field_blocks[0][12]->SetTexture(ImageMgr::Get().GetTexture("pzoner"));
        field_blocks[0][13]->SetTexture(ImageMgr::Get().GetTexture("mdeck"));
        field_blocks[0][14]->SetTexture(ImageMgr::Get().GetTexture("exdeck"));
        field_blocks[0][15]->SetTexture(ImageMgr::Get().GetTexture("grave"));
        field_blocks[0][16]->SetTexture(ImageMgr::Get().GetTexture("banish"));
        for(int32_t i = 0; i < 17; ++i)
            field_blocks[1][i]->Mirror(field_blocks[0][i].get());
        m_zone[0].resize(5);
        m_zone[1].resize(5);
        s_zone[0].resize(8);
        s_zone[1].resize(8);
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::AddCard(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs) {
        if(side > 1)
            return nullptr;
        if(zone & 0x4) {
            if(seq > 4)
                return nullptr;
            auto pre = m_zone[side][seq];
            if(pre != nullptr && zone == 0x4)
                return nullptr;
            if(pre == nullptr && (zone & 0x80))
                return nullptr;
        }
        if(zone & 0x8) {
            if(seq > 7)
                return nullptr;
            auto pre = s_zone[side][seq];
            if(pre != nullptr)
                return nullptr;
        }
        auto ptr = duel_scene.lock()->CreateCard();
        ptr->card_side = side;
        ptr->card_loc = zone;
        ptr->card_seq = seq;
        ptr->card_pos = subs;
        switch(zone & 0x7f) {
            case 0x1:
                ptr->card_seq = (int32_t)deck[side].size();
                deck[side].push_back(ptr);
                break;
            case 0x2:
                ptr->card_seq = (int32_t)hand[side].size();
                hand[side].push_back(ptr);
                break;
            case 0x4:
                if(zone & 0x80) {
                    auto pre = m_zone[side][seq];
                    pre->attached_cards.push_back(ptr);
                } else
                    m_zone[side][seq] = ptr;
                break;
            case 0x8:
                s_zone[side][seq] = ptr;
                break;
            case 0x10:
                ptr->card_seq = (int32_t)grave[side].size();
                grave[side].push_back(ptr);
                break;
            case 0x20:
                ptr->card_seq = (int32_t)banished[side].size();
                banished[side].push_back(ptr);
                break;
            case 0x40:
                ptr->card_seq = (int32_t)extra[side].size();
                extra[side].push_back(ptr);
                break;
        }
        ptr->SetCode(code);
        ptr->SetSleeveTex(ImageMgr::Get().GetTexture((side == 0) ? "sleeve1" : "sleeve2"));
        return ptr;
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::GetCard(int32_t side, int32_t zone, int32_t seq, int32_t subs) {
        std::vector<std::shared_ptr<FieldCard>>* plist = nullptr;
        switch(zone & 0x7f) {
            case 0x1: plist = &deck[side]; break;
            case 0x2: plist = &hand[side]; break;
            case 0x4:
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    if(pcard) {
                        plist = &pcard->attached_cards;
                        seq = subs;
                        break;
                    } else
                        return nullptr;
                } else
                    return m_zone[side][seq];
            case 0x8:
                return s_zone[side][seq];
            case 0x10: plist = &grave[side]; break;
            case 0x20: plist = &banished[side]; break;
            case 0x40: plist = &extra[side]; break;
        }
        if(plist) {
            if(seq >= 0)
                return (seq >= plist->size()) ? nullptr : (*plist)[seq];
            else
                return (-seq > plist->size()) ? nullptr : (*plist)[plist->size() + seq];
        }
        return nullptr;
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::RemoveCard(int32_t side, int32_t zone, int32_t seq, int32_t subs) {
        std::shared_ptr<FieldCard> ret;
        switch(zone & 0x7f) {
            case 0x1: {
                ret = deck[side][seq];
                deck[side].erase(deck[side].begin() + seq);
                for(size_t i = seq; i < deck[side].size(); ++i) {
                    deck[side][i]->card_seq = (int32_t)i;
                    deck[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x2: {
                ret = hand[side][seq];
                hand[side].erase(hand[side].begin() + seq);
                uint32_t index = 0;
                for(auto& iter : hand[side]) {
                    iter->card_seq = index++;
                    iter->UpdatePosition(0);
                }
                break;
            }
            case 0x4: {
                if(zone & 0x80) {
                    auto pcard = m_zone[side][seq];
                    ret = pcard->attached_cards[subs];
                    ret->Detach();
                    for(auto& iter : pcard->attached_cards)
                        iter->UpdatePosition(0);
                    pcard->UpdatePosition(0);
                } else {
                    ret = m_zone[side][seq];
                    m_zone[side][seq] = nullptr;
                }
                break;
            }
            case 0x8: {
                ret = s_zone[side][seq];
                s_zone[side][seq] = nullptr;
                break;
            }
            case 0x10: {
                ret = grave[side][seq];
                grave[side].erase(grave[side].begin() + seq);
                for(size_t i = seq; i < grave[side].size(); ++i) {
                    grave[side][i]->card_seq = (int32_t)i;
                    grave[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x20: {
                ret = banished[side][seq];
                banished[side].erase(banished[side].begin() + seq);
                for(size_t i = seq; i < banished[side].size(); ++i) {
                    banished[side][i]->card_seq = (int32_t)i;
                    banished[side][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x40: {
                ret = extra[side][seq];
                extra[side].erase(extra[side].begin() + seq);
                for(size_t i = seq; i < extra[side].size(); ++i) {
                    extra[side][i]->card_seq = (int32_t)i;
                    extra[side][i]->UpdatePosition(0);
                }
                break;
            }
        }
        return ret;
    }
    
    void DuelSceneHandler::MoveCard(std::shared_ptr<FieldCard> pcard, int32_t toside, int32_t tozone, int32_t toseq, int32_t tosubs) {
        if(pcard->card_side == toside && pcard->card_loc == tozone && pcard->card_seq == toseq && pcard->card_pos == tosubs)
            return;
        RemoveCard(pcard->card_side, pcard->card_loc, pcard->card_seq, pcard->card_pos);
        pcard->card_side = toside;
        pcard->card_loc = tozone;
        pcard->card_seq = toseq;
        pcard->card_pos = tosubs;
        switch(tozone & 0x7f) {
            case 0x1:
                pcard->card_seq = (int32_t)deck[toside].size();
                deck[toside].push_back(pcard);
                break;
            case 0x2:
                pcard->card_seq = (int32_t)hand[toside].size();
                hand[toside].push_back(pcard);
                break;
            case 0x4:
                if(tozone & 0x80) {
                    auto target = m_zone[toside][toseq];
                    if(target) {
                        pcard->Attach(target);
                        if(target->card_loc == 0x4) {
                            for(auto& iter : target->attached_cards)
                                iter->UpdatePosition(0);
                            target->UpdatePosition(0);
                        }
                    }
                } else {
                    m_zone[toside][toseq] = pcard;
                }
                break;
            case 0x8:
                s_zone[toside][toseq] = pcard;
                break;
            case 0x10:
                pcard->card_seq = (int32_t)grave[toside].size();
                grave[toside].push_back(pcard);
                break;
            case 0x20:
                pcard->card_seq = (int32_t)banished[toside].size();
                banished[toside].push_back(pcard);
                break;
            case 0x40:
                pcard->card_seq = (int32_t)extra[toside].size();
                extra[toside].push_back(pcard);
                break;
        }
    }
    
    void DuelSceneHandler::ChangePos(std::shared_ptr<FieldCard> pcard, int32_t pos) {
        
    }
    
    void DuelSceneHandler::ClearField() {
        for(int32_t i = 0; i < 2; ++i) {
            deck[i].clear();
            hand[i].clear();
            m_zone[i].clear();
            s_zone[i].clear();
            grave[i].clear();
            banished[i].clear();
            extra[i].clear();
            m_zone[i].resize(5);
            s_zone[i].resize(8);
        }
        duel_scene.lock()->ClearAllCards();
    }
    
    void DuelSceneHandler::InitHp(int32_t local_pl, int32_t hp) {
        
    }
    
    void DuelSceneHandler::AddChain(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs, int32_t tside, int32_t tzone, int32_t tseq) {
        
    }
    
}