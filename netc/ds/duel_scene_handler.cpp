#include "utils/common.h"

#include "../config.h"
#include "../sgui.h"
#include "../gui_extra.h"
#include "../image_mgr.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    bool deck_reversed = false;
    int32_t host_player = 0;
    std::array<std::shared_ptr<FieldBlock>, 17> field_blocks[2];
    std::vector<std::shared_ptr<FieldCard>> m_zone[2];
    std::vector<std::shared_ptr<FieldCard>> s_zone[2];
    std::vector<std::shared_ptr<FieldCard>> deck[2];
    std::vector<std::shared_ptr<FieldCard>> hand[2];
    std::vector<std::shared_ptr<FieldCard>> extra[2];
    std::vector<std::shared_ptr<FieldCard>> grave[2];
    std::vector<std::shared_ptr<FieldCard>> banished[2];
    
    DuelSceneHandler::DuelSceneHandler(DuelScene* pscene) {
        duel_scene = pscene;
        info_panel = std::make_shared<InfoPanel>();
        InitField();
    }
    
    DuelSceneHandler::~DuelSceneHandler() {
    }
    
    void DuelSceneHandler::BeginHandler() {
        AddCard(83764718, CardPosInfo(0, 0x4, 1, 0x1))->UpdatePosition(0);
        AddCard(64496451, CardPosInfo(0, 0x84, 1, 0x0))->UpdatePosition(0);
        AddCard(57728570, CardPosInfo(0, 0x40, 2, 0xa))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(0, 0x2, 1, 0xa))->UpdatePosition(0);
        for(auto& pc : hand[0])
            pc->UpdatePosition(0);
    }
    
    void DuelSceneHandler::EndHandler() {
        ClearField();
    }
    
    bool DuelSceneHandler::UpdateHandler() {
        if(show_info_time) {
            auto now = SceneMgr::Get().GetSysClock();
            if(now - show_info_time >= 500) {
                show_info_time = 0;
                auto mp = SceneMgr::Get().GetMousePosition();
                auto cpos = duel_scene->GetHoverCardPos(duel_scene->GetHoverPos(mp.x, mp.y));
                auto ptr = duel_scene->GetHoverCard(cpos);
                if(ptr && ptr->code)
                    info_panel->ShowInfo(ptr->code);
            }
        }
        if(!message_lock) {
            auto cmd = messages.PullCommand();
            if(cmd) {
                BufferReader bu(cmd->msg_buffer.data(), cmd->msg_buffer.size());
                SolveMessage(cmd->msg_type, bu);
                messages.PopCommand();
            }
        }
        return true;
    }

    void DuelSceneHandler::MouseMove(int32_t x, int32_t y) {
        auto scene_size = SceneMgr::Get().GetSceneSize();
        if(btnDown[0]) {
            float ratex = (float)(x - btnPos[0].x) / scene_size.x * 2.0f;
            float ratey = (float)(y - btnPos[0].y) / scene_size.y * 2.0f;
            btnPos[0] = {x, y};
            duel_scene->UpdateViewOffset({ratex, ratey});
            show_info_time = 0;
        }
        if(btnDown[1]) {
            float rate = (float)(y - btnPos[1].y) / scene_size.y * 3.1415926f * 0.5f;
            btnPos[1] = {x, y};
            duel_scene->UpdateViewAngle(rate);
        }
        auto pblock = pre_block.lock();
        auto pcard = pre_card.lock();
        auto hp = duel_scene->GetHoverPos(x, y);
        std::shared_ptr<FieldBlock> hover_block = duel_scene->GetFieldBlock(hp.x, hp.y);
        auto cpos = duel_scene->GetHoverCardPos(duel_scene->GetHoverPos(x, y));
        auto hover_card = duel_scene->GetHoverCard(cpos);
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
            if(pcard && pcard->pos_info.location == 0x2) {
                auto curoff = pcard->yoffset;
                auto act = std::make_shared<LerpAnimator<int64_t, FieldCard>>(0, pcard, [curoff](FieldCard* fb, double t)->bool {
                    fb->SetYOffset(curoff * (1.0 - t));
                    return true;
                }, std::make_shared<TGenMove<int64_t>>(curoff / 0.2f * 500, 0.01));
                SceneMgr::Get().RemoveAction(pcard.get(), 2);
                SceneMgr::Get().PushAction(act, pcard.get(), 2);
            }
            if(hover_card && hover_card->pos_info.location == 0x2) {
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
        duel_scene->UpdateViewRadius(deltay / 30.0f);
        duel_scene->UpdateParams();
    }
    
    void DuelSceneHandler::KeyDown(int32_t key, int32_t mods) {

    }
    
    void DuelSceneHandler::KeyUp(int32_t key, int32_t mods) {
        if(key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            Test(key - GLFW_KEY_0);
    }
    
    void DuelSceneHandler::InitField() {
        for(int32_t i = 0 ; i < 17; ++i) {
            field_blocks[0][i] = duel_scene->CreateFieldBlock();
            field_blocks[1][i] = duel_scene->CreateFieldBlock();
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
    
    std::shared_ptr<FieldCard> DuelSceneHandler::AddCard(uint32_t code, CardPosInfo pos_info) {
        if(pos_info.controler > 1)
            return nullptr;
        if((pos_info.location & 0x7f) == 0x4) {
            if(pos_info.sequence > 4)
                return nullptr;
            auto pre = m_zone[pos_info.controler][pos_info.sequence];
            if(pre != nullptr && pos_info.location == 0x4)
                return nullptr;
            if(pre == nullptr && (pos_info.location & 0x80))
                return nullptr;
        }
        if((pos_info.location & 0x7f) == 0x8) {
            if(pos_info.sequence > 7)
                return nullptr;
            auto pre = s_zone[pos_info.controler][pos_info.sequence];
            if(pre != nullptr)
                return nullptr;
        }
        auto ptr = duel_scene->CreateCard();
        ptr->pos_info = pos_info;
        switch(pos_info.location & 0x7f) {
            case 0x1:
                ptr->pos_info.sequence = (int32_t)deck[pos_info.controler].size();
                deck[pos_info.controler].push_back(ptr);
                break;
            case 0x2:
                ptr->pos_info.sequence = (int32_t)hand[pos_info.controler].size();
                hand[pos_info.controler].push_back(ptr);
                break;
            case 0x4:
                if(pos_info.location & 0x80) {
                    auto pre = m_zone[pos_info.controler][pos_info.sequence];
                    pre->attached_cards.push_back(ptr);
                    ptr->attaching_card = pre.get();
                } else
                    m_zone[pos_info.controler][pos_info.sequence] = ptr;
                break;
            case 0x8:
                s_zone[pos_info.controler][pos_info.sequence] = ptr;
                break;
            case 0x10:
                ptr->pos_info.sequence = (int32_t)grave[pos_info.controler].size();
                grave[pos_info.controler].push_back(ptr);
                break;
            case 0x20:
                ptr->pos_info.sequence = (int32_t)banished[pos_info.controler].size();
                banished[pos_info.controler].push_back(ptr);
                break;
            case 0x40:
                ptr->pos_info.sequence = (int32_t)extra[pos_info.controler].size();
                extra[pos_info.controler].push_back(ptr);
                break;
        }
        ptr->SetCode(code);
        ptr->SetSleeveTex(ImageMgr::Get().GetTexture((pos_info.controler == 0) ? "sleeve1" : "sleeve2"));
        duel_scene->RedrawAllCards();
        return ptr;
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::GetCard(CardPosInfo pos_info) {
        std::vector<std::shared_ptr<FieldCard>>* plist = nullptr;
        switch(pos_info.location & 0x7f) {
            case 0x1: plist = &deck[pos_info.controler]; break;
            case 0x2: plist = &hand[pos_info.controler]; break;
            case 0x4:
                if(pos_info.location & 0x80) {
                    auto pcard = m_zone[pos_info.controler][pos_info.sequence];
                    if(pcard) {
                        plist = &pcard->attached_cards;
                        pos_info.sequence = pos_info.subsequence;
                        break;
                    } else
                        return nullptr;
                } else
                    return m_zone[pos_info.controler][pos_info.sequence];
            case 0x8:
                return s_zone[pos_info.controler][pos_info.sequence];
            case 0x10: plist = &grave[pos_info.controler]; break;
            case 0x20: plist = &banished[pos_info.controler]; break;
            case 0x40: plist = &extra[pos_info.controler]; break;
        }
        if(plist) {
            if(pos_info.sequence >= 0)
                return (pos_info.sequence >= plist->size()) ? nullptr : (*plist)[pos_info.sequence];
            else
                return (-pos_info.sequence > plist->size()) ? nullptr : (*plist)[plist->size() + pos_info.sequence];
        }
        return nullptr;
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::RemoveCard(CardPosInfo pos_info) {
        std::shared_ptr<FieldCard> ret;
        switch(pos_info.location & 0x7f) {
            case 0x1: {
                ret = deck[pos_info.controler][pos_info.sequence];
                deck[pos_info.controler].erase(deck[pos_info.controler].begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < deck[pos_info.controler].size(); ++i) {
                    deck[pos_info.controler][i]->pos_info.sequence = (int8_t)i;
                    deck[pos_info.controler][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x2: {
                ret = hand[pos_info.controler][pos_info.sequence];
                hand[pos_info.controler].erase(hand[pos_info.controler].begin() + pos_info.sequence);
                int8_t index = 0;
                for(auto& iter : hand[pos_info.controler])
                    iter->pos_info.sequence = index++;
                break;
            }
            case 0x4: {
                if(pos_info.location & 0x80) {
                    auto pcard = m_zone[pos_info.controler][pos_info.sequence];
                    ret = pcard->attached_cards[pos_info.subsequence];
                    ret->Detach();
                    for(auto& iter : pcard->attached_cards)
                        iter->UpdatePosition(0);
                    pcard->UpdatePosition(0);
                } else {
                    ret = m_zone[pos_info.controler][pos_info.sequence];
                    m_zone[pos_info.controler][pos_info.sequence] = nullptr;
                }
                break;
            }
            case 0x8: {
                ret = s_zone[pos_info.controler][pos_info.sequence];
                s_zone[pos_info.controler][pos_info.sequence] = nullptr;
                break;
            }
            case 0x10: {
                ret = grave[pos_info.controler][pos_info.sequence];
                grave[pos_info.controler].erase(grave[pos_info.controler].begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < grave[pos_info.controler].size(); ++i) {
                    grave[pos_info.controler][i]->pos_info.sequence = (int8_t)i;
                    grave[pos_info.controler][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x20: {
                ret = banished[pos_info.controler][pos_info.sequence];
                banished[pos_info.controler].erase(banished[pos_info.controler].begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < banished[pos_info.controler].size(); ++i) {
                    banished[pos_info.controler][i]->pos_info.sequence = (int8_t)i;
                    banished[pos_info.controler][i]->UpdatePosition(0);
                }
                break;
            }
            case 0x40: {
                ret = extra[pos_info.controler][pos_info.sequence];
                extra[pos_info.controler].erase(extra[pos_info.controler].begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < extra[pos_info.controler].size(); ++i) {
                    extra[pos_info.controler][i]->pos_info.sequence = (int8_t)i;
                    extra[pos_info.controler][i]->UpdatePosition(0);
                }
                break;
            }
        }
        return ret;
    }
    
    void DuelSceneHandler::MoveCard(std::shared_ptr<FieldCard> pcard, CardPosInfo pos_info) {
        if(pcard->pos_info == pos_info)
            return;
        RemoveCard(pcard->pos_info);
        pcard->pos_info = pos_info;
        switch(pos_info.location & 0x7f) {
            case 0x1:
                pcard->pos_info.sequence = (int32_t)deck[pos_info.controler].size();
                deck[pos_info.controler].push_back(pcard);
                break;
            case 0x2:
                pcard->pos_info.sequence = (int32_t)hand[pos_info.controler].size();
                hand[pos_info.controler].push_back(pcard);
                break;
            case 0x4:
                if(pos_info.location & 0x80) {
                    auto target = m_zone[pos_info.controler][pos_info.sequence];
                    if(target) {
                        pcard->Attach(target);
                        if(target->pos_info.location == 0x4) {
                            for(auto& iter : target->attached_cards)
                                iter->UpdatePosition(0);
                            target->UpdatePosition(0);
                        }
                    }
                } else {
                    m_zone[pos_info.controler][pos_info.sequence] = pcard;
                }
                break;
            case 0x8:
                s_zone[pos_info.controler][pos_info.sequence] = pcard;
                break;
            case 0x10:
                pcard->pos_info.sequence = (int32_t)grave[pos_info.controler].size();
                grave[pos_info.controler].push_back(pcard);
                break;
            case 0x20:
                pcard->pos_info.sequence = (int32_t)banished[pos_info.controler].size();
                banished[pos_info.controler].push_back(pcard);
                break;
            case 0x40:
                pcard->pos_info.sequence = (int32_t)extra[pos_info.controler].size();
                extra[pos_info.controler].push_back(pcard);
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
        duel_scene->ClearAllCards();
    }
    
    void DuelSceneHandler::InitHp(int32_t local_pl, int32_t hp) {
        
    }
    
    void DuelSceneHandler::AddChain(uint32_t code, int32_t side, int32_t zone, int32_t seq, int32_t subs, int32_t tside, int32_t tzone, int32_t tseq) {
        
    }
    
}