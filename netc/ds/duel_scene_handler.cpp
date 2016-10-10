#include "utils/common.h"

#include "../config.h"
#include "../sgui.h"
#include "../gui_extra.h"
#include "../image_mgr.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    LocalDuelData g_duel;
    LocalPlayerData g_player[2];
    
    std::wstring CardPosInfo::ToString() {
        std::string locstr = stringCfg["eui_location_format"].to_string();
        auto pos = locstr.find("{controler}");
        if(pos != std::string::npos) {
            if(controler == 0)
                locstr.replace(pos, 11, stringCfg["eui_controler1"].to_string());
            else
                locstr.replace(pos, 11, stringCfg["eui_controler2"].to_string());
        }
        pos = locstr.find("{location}");
        std::string seqstr;
        std::string xyzstr;
        if(pos != std::string::npos) {
            switch(location & 0x7f) {
                case 0x1:
                    locstr.replace(pos, 10, stringCfg["eui_location_deck"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    break;
                case 0x2:
                    locstr.replace(pos, 10, stringCfg["eui_location_hand"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    break;
                case 0x4:
                    locstr.replace(pos, 10, stringCfg["eui_location_szone"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    if(location & 0x80)
                        xyzstr = stringCfg["eui_location_xyz"].to_string();
                    break;
                case 0x8:
                    if(sequence < 5) {
                        locstr.replace(pos, 10, stringCfg["eui_location_szone"].to_string());
                        seqstr = To<std::string>("[%d]", sequence);
                    } else if(sequence == 5)
                        locstr.replace(pos, 10, stringCfg["eui_location_fzone"].to_string());
                    else {
                        locstr.replace(pos, 10, stringCfg["eui_location_pzone"].to_string());
                        seqstr = To<std::string>("[%d]", sequence - 6);
                    }
                    break;
                case 0x10:
                    locstr.replace(pos, 10, stringCfg["eui_location_deck"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    break;
                case 0x20:
                    locstr.replace(pos, 10, stringCfg["eui_location_deck"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    break;
                case 0x40:
                    locstr.replace(pos, 10, stringCfg["eui_location_deck"].to_string());
                    seqstr = To<std::string>("[%d]", sequence);
                    break;
            }
            
        }
        pos = locstr.find("{sequence}");
        if(pos != std::string::npos)
           locstr.replace(pos, 10, seqstr);
        pos = locstr.find("{xyz}");
        if(pos != std::string::npos)
            locstr.replace(pos, 10, xyzstr);
        return To<std::wstring>(locstr);
    }
    
    DuelSceneHandler::DuelSceneHandler(DuelScene* pscene) {
        duel_scene = pscene;
        log_panel = std::make_shared<LogPanel>();
        InitField();
    }
    
    DuelSceneHandler::~DuelSceneHandler() {
    }
    
    void DuelSceneHandler::BeginHandler() {
        AddCard(84013237, CardPosInfo(0, 0x4, 1, 0x1))->UpdatePosition(0);
        for(int32_t i = 0; i < 3; ++i)
            AddCard(83764718, CardPosInfo(0, 0x2, 1, 0x1))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 0, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 1, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 2, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 3, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 4, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x1, 5, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x84, 1, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x84, 1, 0))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x84, 1, 0))->UpdatePosition(0);
        AddCard(83764718, CardPosInfo(0, 0x4, 3, 0x8))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(0, 0x4, 4, 0x8))->UpdatePosition(0);
        AddCard(64496451, CardPosInfo(0, 0x2, 2, 0x1))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x10, 2, 0xa))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(0, 0x10, 2, 0xa))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(0, 0x10, 2, 0xa))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(0, 0x10, 2, 0xa))->UpdatePosition(0);
        AddCard(84013237, CardPosInfo(0, 0x10, 2, 0xa))->UpdatePosition(0);
        AddCard(83764718, CardPosInfo(1, 0x4, 3, 0x1))->UpdatePosition(0);
        AddCard(97268402, CardPosInfo(1, 0x4, 4, 0x1))->UpdatePosition(0);
        for(auto& pc : g_player[0].hand)
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
                    InfoPanel::Get().ShowInfo(ptr->code);
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
            if(hover_card && hover_card->code && InfoPanel::Get().IsOpen())
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
            g_player[0].field_blocks[i] = duel_scene->CreateFieldBlock();
            g_player[1].field_blocks[i] = duel_scene->CreateFieldBlock();
            g_player[0].field_blocks[i]->SetTexture(ImageMgr::Get().GetRawMiscTexture());
            g_player[1].field_blocks[i]->SetTexture(ImageMgr::Get().GetRawMiscTexture());
        }
        g_player[0].field_blocks[0 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mzone1"]));
        g_player[0].field_blocks[1 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mzone2"]));
        g_player[0].field_blocks[2 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mzone3"]));
        g_player[0].field_blocks[3 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mzone4"]));
        g_player[0].field_blocks[4 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mzone5"]));
        g_player[0].field_blocks[5 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["szone1"]));
        g_player[0].field_blocks[6 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["szone2"]));
        g_player[0].field_blocks[7 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["szone3"]));
        g_player[0].field_blocks[8 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["szone4"]));
        g_player[0].field_blocks[9 ]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["szone5"]));
        g_player[0].field_blocks[10]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["fdzone"]));
        g_player[0].field_blocks[11]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["pzonel"]));
        g_player[0].field_blocks[12]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["pzoner"]));
        g_player[0].field_blocks[13]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["mdeck"]));
        g_player[0].field_blocks[14]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["exdeck"]));
        g_player[0].field_blocks[15]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["grave"]));
        g_player[0].field_blocks[16]->SetPosition(sgui::SGJsonUtil::ConvertRect<float>(layoutCfg["banish"]));
        g_player[0].field_blocks[0 ]->SetTexcoord(ImageMgr::Get().GetTexture("mzone"));
        g_player[0].field_blocks[1 ]->SetTexcoord(ImageMgr::Get().GetTexture("mzone"));
        g_player[0].field_blocks[2 ]->SetTexcoord(ImageMgr::Get().GetTexture("mzone"));
        g_player[0].field_blocks[3 ]->SetTexcoord(ImageMgr::Get().GetTexture("mzone"));
        g_player[0].field_blocks[4 ]->SetTexcoord(ImageMgr::Get().GetTexture("mzone"));
        g_player[0].field_blocks[5 ]->SetTexcoord(ImageMgr::Get().GetTexture("szone"));
        g_player[0].field_blocks[6 ]->SetTexcoord(ImageMgr::Get().GetTexture("szone"));
        g_player[0].field_blocks[7 ]->SetTexcoord(ImageMgr::Get().GetTexture("szone"));
        g_player[0].field_blocks[8 ]->SetTexcoord(ImageMgr::Get().GetTexture("szone"));
        g_player[0].field_blocks[9 ]->SetTexcoord(ImageMgr::Get().GetTexture("szone"));
        g_player[0].field_blocks[10]->SetTexcoord(ImageMgr::Get().GetTexture("fdzone"));
        g_player[0].field_blocks[11]->SetTexcoord(ImageMgr::Get().GetTexture("pzonel"));
        g_player[0].field_blocks[12]->SetTexcoord(ImageMgr::Get().GetTexture("pzoner"));
        g_player[0].field_blocks[13]->SetTexcoord(ImageMgr::Get().GetTexture("mdeck"));
        g_player[0].field_blocks[14]->SetTexcoord(ImageMgr::Get().GetTexture("exdeck"));
        g_player[0].field_blocks[15]->SetTexcoord(ImageMgr::Get().GetTexture("grave"));
        g_player[0].field_blocks[16]->SetTexcoord(ImageMgr::Get().GetTexture("banish"));
        for(int32_t i = 0; i < 17; ++i)
            g_player[1].field_blocks[i]->Mirror(g_player[0].field_blocks[i].get());
        g_player[0].m_zone.resize(5);
        g_player[1].m_zone.resize(5);
        g_player[0].s_zone.resize(8);
        g_player[1].s_zone.resize(8);
        auto add_avatar_lp = [this](jaweson::JsonValue& avatar_node, jaweson::JsonValue& lp_node, int32_t pl) {
            std::array<v2i, 4> rel;
            std::array<v2f, 4> prop;
            for(int32_t i = 0; i < 4; ++i) {
                auto& point_node = avatar_node[i];
                rel[i] = {point_node[0].to_value<int32_t>(), point_node[2].to_value<int32_t>()};
                prop[i] = {point_node[1].to_value<float>(), point_node[3].to_value<float>()};
            }
            auto f1 = duel_scene->AddFloatingSprite();
            auto ava_tex = ImageMgr::Get().GetAvatarTexture(pl);
            f1->SetTexture(ava_tex);
            f1->BuildSprite(rel.data(), ava_tex->GetTextureInfo(), prop.data());
            auto f2 = duel_scene->AddFloatingSprite();
            auto misc_tex = ImageMgr::Get().GetRawMiscTexture();
            f2->SetTexture(misc_tex);
            f2->BuildSprite(rel.data(), ImageMgr::Get().GetTexture("avatar frame"), prop.data());
            for(int32_t i = 0; i < 4; ++i) {
                auto& point_node = lp_node[i];
                g_player[pl].lp_verts_rel[i] = {point_node[0].to_value<int32_t>(), point_node[2].to_value<int32_t>()};
                g_player[pl].lp_verts_prop[i] = {point_node[1].to_value<float>(), point_node[3].to_value<float>()};
            }
            g_player[pl].lpbar = duel_scene->AddFloatingSprite();
            g_player[pl].lpbar->SetTexture(misc_tex);
            auto f4 = duel_scene->AddFloatingSprite();
            f4->SetTexture(misc_tex);
            f4->BuildSprite(g_player[pl].lp_verts_rel.data(), ImageMgr::Get().GetTexture("lp frame"), g_player[pl].lp_verts_prop.data());
        };
        add_avatar_lp(layoutCfg["avatar1"], layoutCfg["lpbar1"], 0);
        add_avatar_lp(layoutCfg["avatar2"], layoutCfg["lpbar2"], 1);
        for(int32_t p = 0; p < 2; ++p) {
            for(int32_t i = 0 ; i < 22; ++i) {
                g_player[p].fixed_numbers[i] = duel_scene->AddFloatingNumber();
                g_player[p].fixed_numbers[i]->SetColor(0xff00ffff);
                g_player[p].fixed_numbers[i]->SetSColor(0xff000000);
                if(i < 6)
                    g_player[p].fixed_numbers[i]->SetCharSize({18, 24});
                else
                    g_player[p].fixed_numbers[i]->SetCharSize({12, 15});
            }
            g_player[p].fixed_numbers[4]->SetColor(0xffff00ff);
            g_player[p].fixed_numbers[5]->SetColor(0xff0000ff);
            jaweson::JsonValue& val_node = layoutCfg[p == 0 ? "lpval1" : "lpval2"];
            v2i pos = {val_node[0].to_value<int32_t>(), val_node[2].to_value<int32_t>()};
            v2f prop = {val_node[1].to_value<float>(), val_node[3].to_value<float>()};
            v2i csize = {val_node[4].to_value<int32_t>(), val_node[5].to_value<int32_t>()};
            g_player[p].fixed_numbers[21]->SetCenter(pos, prop);
            g_player[p].fixed_numbers[21]->SetCharSize(csize);
            g_player[p].fixed_numbers[21]->SetRotation(val_node[6].to_value<float>());
            SetLP(p, 4000);
        }
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::AddCard(uint32_t code, CardPosInfo pos_info) {
        if(pos_info.controler > 1 || pos_info.location == 0)
            return nullptr;
        std::shared_ptr<FieldCard> ptr;
        if(pos_info.location & 0x80) {
            if(((pos_info.location & 0x7f) != 0x4) || (pos_info.sequence > 4))
                return nullptr;
            auto attach_card = g_player[pos_info.controler].m_zone[pos_info.sequence];
            if(attach_card) {
                ptr = duel_scene->CreateCard();
                ptr->pos_info = pos_info;
                ptr->Attach(attach_card);
                attach_card->UpdatePosition(0);
            }
        } else {
            if(pos_info.location == 0x4) {
                if(pos_info.sequence > 4)
                    return nullptr;
                if(g_player[pos_info.controler].m_zone[pos_info.sequence] != nullptr)
                    return nullptr;
            }
            if(pos_info.location == 0x8) {
                if(pos_info.sequence > 7)
                    return nullptr;
                if(g_player[pos_info.controler].s_zone[pos_info.sequence] != nullptr)
                    return nullptr;
            }
            ptr = duel_scene->CreateCard();
            ptr->pos_info = pos_info;
            switch(pos_info.location & 0x7f) {
                case 0x1: {
                    ptr->pos_info.sequence = (int32_t)g_player[pos_info.controler].deck.size();
                    g_player[pos_info.controler].deck.push_back(ptr);
                    g_player[pos_info.controler].fixed_numbers[0]->SetValue((int32_t)g_player[pos_info.controler].deck.size());
                    break;
                }
                case 0x2: {
                    ptr->pos_info.sequence = (int32_t)g_player[pos_info.controler].hand.size();
                    g_player[pos_info.controler].hand.push_back(ptr);
                    break;
                }
                case 0x4: {
                    g_player[pos_info.controler].m_zone[pos_info.sequence] = ptr;
                    break;
                }
                case 0x8: {
                    g_player[pos_info.controler].s_zone[pos_info.sequence] = ptr;
                    break;
                }
                case 0x10: {
                    ptr->pos_info.sequence = (int32_t)g_player[pos_info.controler].grave.size();
                    g_player[pos_info.controler].grave.push_back(ptr);
                    g_player[pos_info.controler].fixed_numbers[1]->SetValue((int32_t)g_player[pos_info.controler].grave.size());
                    break;
                }
                case 0x20: {
                    ptr->pos_info.sequence = (int32_t)g_player[pos_info.controler].banished.size();
                    g_player[pos_info.controler].banished.push_back(ptr);
                    g_player[pos_info.controler].fixed_numbers[2]->SetValue((int32_t)g_player[pos_info.controler].banished.size());
                    break;
                }
                case 0x40: {
                    ptr->pos_info.sequence = (int32_t)g_player[pos_info.controler].extra.size();
                    g_player[pos_info.controler].extra.push_back(ptr);
                    if(pos_info.position & 0x5)
                        g_player[pos_info.controler].extra_faceup_count++;
                    std::string str = To<std::string>("%d(%d)", g_player[pos_info.controler].extra.size(), g_player[pos_info.controler].extra_faceup_count);
                    g_player[pos_info.controler].fixed_numbers[3]->SetValueStr(str.c_str());
                    break;
                }
            }
        }
        if(ptr) {
            ptr->SetCode(code);
            ptr->SetSleeveTex(ImageMgr::Get().GetTexture((pos_info.controler == 0) ? "sleeve1" : "sleeve2"));
            duel_scene->RedrawAllCards();
        }
        return ptr;
    }
    
    std::shared_ptr<FieldCard> DuelSceneHandler::GetCard(CardPosInfo pos_info) {
        std::vector<std::shared_ptr<FieldCard>>* plist = nullptr;
        switch(pos_info.location & 0x7f) {
            case 0x1: plist = &g_player[pos_info.controler].deck; break;
            case 0x2: plist = &g_player[pos_info.controler].hand; break;
            case 0x4:
                if(pos_info.location & 0x80) {
                    auto pcard = g_player[pos_info.controler].m_zone[pos_info.sequence];
                    if(pcard) {
                        plist = &pcard->attached_cards;
                        pos_info.sequence = pos_info.subsequence;
                        break;
                    } else
                        return nullptr;
                } else
                    return g_player[pos_info.controler].m_zone[pos_info.sequence];
            case 0x8:
                return g_player[pos_info.controler].s_zone[pos_info.sequence];
            case 0x10: plist = &g_player[pos_info.controler].grave; break;
            case 0x20: plist = &g_player[pos_info.controler].banished; break;
            case 0x40: plist = &g_player[pos_info.controler].extra; break;
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
                ret = g_player[pos_info.controler].deck[pos_info.sequence];
                g_player[pos_info.controler].deck.erase(g_player[pos_info.controler].deck.begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < g_player[pos_info.controler].deck.size(); ++i) {
                    g_player[pos_info.controler].deck[i]->pos_info.sequence = (int8_t)i;
                    g_player[pos_info.controler].deck[i]->UpdatePosition(0);
                }
                if(!g_player[pos_info.controler].deck.empty())
                    g_player[pos_info.controler].fixed_numbers[0]->SetValue((int32_t)g_player[pos_info.controler].deck.size());
                break;
            }
            case 0x2: {
                ret = g_player[pos_info.controler].hand[pos_info.sequence];
                g_player[pos_info.controler].hand.erase(g_player[pos_info.controler].hand.begin() + pos_info.sequence);
                int8_t index = 0;
                for(auto& iter : g_player[pos_info.controler].hand)
                    iter->pos_info.sequence = index++;
                break;
            }
            case 0x4: {
                if(pos_info.location & 0x80) {
                    auto pcard = g_player[pos_info.controler].m_zone[pos_info.sequence];
                    ret = pcard->attached_cards[pos_info.subsequence];
                    ret->Detach();
                    for(auto& iter : pcard->attached_cards)
                        iter->UpdatePosition(0);
                    pcard->UpdatePosition(0);
                } else {
                    ret = g_player[pos_info.controler].m_zone[pos_info.sequence];
                    g_player[pos_info.controler].m_zone[pos_info.sequence] = nullptr;
                }
                break;
            }
            case 0x8: {
                ret = g_player[pos_info.controler].s_zone[pos_info.sequence];
                g_player[pos_info.controler].s_zone[pos_info.sequence] = nullptr;
                break;
            }
            case 0x10: {
                ret = g_player[pos_info.controler].grave[pos_info.sequence];
                g_player[pos_info.controler].grave.erase(g_player[pos_info.controler].grave.begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < g_player[pos_info.controler].grave.size(); ++i) {
                    g_player[pos_info.controler].grave[i]->pos_info.sequence = (int8_t)i;
                    g_player[pos_info.controler].grave[i]->UpdatePosition(0);
                }
                if(!g_player[pos_info.controler].grave.empty())
                    g_player[pos_info.controler].fixed_numbers[1]->SetValue((int32_t)g_player[pos_info.controler].grave.size());
                break;
            }
            case 0x20: {
                ret = g_player[pos_info.controler].banished[pos_info.sequence];
                g_player[pos_info.controler].banished.erase(g_player[pos_info.controler].banished.begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < g_player[pos_info.controler].banished.size(); ++i) {
                    g_player[pos_info.controler].banished[i]->pos_info.sequence = (int8_t)i;
                    g_player[pos_info.controler].banished[i]->UpdatePosition(0);
                }
                if(!g_player[pos_info.controler].banished.empty())
                    g_player[pos_info.controler].fixed_numbers[2]->SetValue((int32_t)g_player[pos_info.controler].banished.size());
                break;
            }
            case 0x40: {
                ret = g_player[pos_info.controler].extra[pos_info.sequence];
                g_player[pos_info.controler].extra.erase(g_player[pos_info.controler].extra.begin() + pos_info.sequence);
                for(size_t i = pos_info.sequence; i < g_player[pos_info.controler].extra.size(); ++i) {
                    g_player[pos_info.controler].extra[i]->pos_info.sequence = (int8_t)i;
                    g_player[pos_info.controler].extra[i]->UpdatePosition(0);
                }
                if(ret->pos_info.position & 0x5)
                    g_player[pos_info.controler].extra_faceup_count--;
                if(!g_player[pos_info.controler].extra.empty()) {
                    std::string str = To<std::string>("%d(%d)", g_player[pos_info.controler].extra.size(), g_player[pos_info.controler].extra_faceup_count);
                    g_player[pos_info.controler].fixed_numbers[3]->SetValueStr(str.c_str());
                }
                break;
            }
        }
        if(ret)
            ret->pos_info.info = 0;
        return ret;
    }
    
    void DuelSceneHandler::MoveCard(std::shared_ptr<FieldCard> pcard, CardPosInfo pos_info) {
        if(pcard->pos_info == pos_info)
            return;
        if(pos_info.location & 0x80) {
            auto target = GetCard(CardPosInfo(pos_info.controler, pos_info.location & 0x7f, pos_info.sequence, 0));
            if(target) {
                RemoveCard(pcard->pos_info);
                pcard->pos_info = pos_info;
                pcard->Attach(target);
                if((pos_info.location & 0x7f) == 0x4)
                    target->UpdatePosition(0);
                duel_scene->RedrawAllCards();
            }
        } else {
            switch(pos_info.location & 0x7f) {
                case 0x1:
                    RemoveCard(pcard->pos_info);
                    pcard->pos_info = pos_info;
                    duel_scene->RedrawAllCards();
                    pcard->pos_info.sequence = (int32_t)g_player[pos_info.controler].deck.size();
                    g_player[pos_info.controler].deck.push_back(pcard);
                    g_player[pos_info.controler].fixed_numbers[0]->SetValue((int32_t)g_player[pos_info.controler].deck.size());
                    break;
                case 0x2:
                    RemoveCard(pcard->pos_info);
                    pcard->pos_info = pos_info;
                    duel_scene->RedrawAllCards();
                    pcard->pos_info.sequence = (int32_t)g_player[pos_info.controler].hand.size();
                    g_player[pos_info.controler].hand.push_back(pcard);
                    break;
                case 0x4:
                    if(g_player[pos_info.controler].m_zone[pos_info.sequence] == nullptr) {
                        RemoveCard(pcard->pos_info);
                        pcard->pos_info = pos_info;
                        g_player[pos_info.controler].m_zone[pos_info.sequence] = pcard;
                        duel_scene->RedrawAllCards();
                    }
                    break;
                case 0x8:
                    if(g_player[pos_info.controler].s_zone[pos_info.sequence] == nullptr) {
                        RemoveCard(pcard->pos_info);
                        pcard->pos_info = pos_info;
                        g_player[pos_info.controler].s_zone[pos_info.sequence] = pcard;
                        duel_scene->RedrawAllCards();
                    }
                    break;
                case 0x10:
                    RemoveCard(pcard->pos_info);
                    pcard->pos_info = pos_info;
                    duel_scene->RedrawAllCards();
                    pcard->pos_info.sequence = (int32_t)g_player[pos_info.controler].grave.size();
                    g_player[pos_info.controler].grave.push_back(pcard);
                    g_player[pos_info.controler].fixed_numbers[1]->SetValue((int32_t)g_player[pos_info.controler].grave.size());
                    break;
                case 0x20:
                    RemoveCard(pcard->pos_info);
                    pcard->pos_info = pos_info;
                    duel_scene->RedrawAllCards();
                    pcard->pos_info.sequence = (int32_t)g_player[pos_info.controler].banished.size();
                    g_player[pos_info.controler].banished.push_back(pcard);
                    g_player[pos_info.controler].fixed_numbers[2]->SetValue((int32_t)g_player[pos_info.controler].banished.size());
                    break;
                case 0x40: {
                    RemoveCard(pcard->pos_info);
                    pcard->pos_info = pos_info;
                    duel_scene->RedrawAllCards();
                    pcard->pos_info.sequence = (int32_t)g_player[pos_info.controler].extra.size();
                    g_player[pos_info.controler].extra.push_back(pcard);
                    if(pos_info.position & 0x5)
                        g_player[pos_info.controler].extra_faceup_count++;
                    std::string str = To<std::string>("%d(%d)", g_player[pos_info.controler].extra.size(), g_player[pos_info.controler].extra_faceup_count);
                    g_player[pos_info.controler].fixed_numbers[3]->SetValueStr(str.c_str());
                    break;
                }
            }
        }
    }
    
    void DuelSceneHandler::ClearField() {
        for(int32_t i = 0; i < 2; ++i) {
            g_player[i].deck.clear();
            g_player[i].hand.clear();
            g_player[i].m_zone.clear();
            g_player[i].s_zone.clear();
            g_player[i].grave.clear();
            g_player[i].banished.clear();
            g_player[i].extra.clear();
            g_player[i].m_zone.resize(5);
            g_player[i].s_zone.resize(8);
        }
        duel_scene->ClearAllCards();
    }
    
    void DuelSceneHandler::SetLP(int32_t local_pl, int32_t lp) {
        if(local_pl > 1)
            return;
        float rate = lp / 8000.0f;
        if(rate < 0.0f)
            rate = 0.0f;
        if(rate > 1.0f)
            rate = 1.0f;
        std::array<v2i, 4> rel;
        std::array<v2f, 4> prop;
        rel[0] = g_player[local_pl].lp_verts_rel[0];
        rel[1] = base::interpolate(g_player[local_pl].lp_verts_rel[0], g_player[local_pl].lp_verts_rel[1], rate);
        rel[2] = g_player[local_pl].lp_verts_rel[2];
        rel[3] = base::interpolate(g_player[local_pl].lp_verts_rel[2], g_player[local_pl].lp_verts_rel[3], rate);
        prop[0] = g_player[local_pl].lp_verts_prop[0];
        prop[1] = base::interpolate(g_player[local_pl].lp_verts_prop[0], g_player[local_pl].lp_verts_prop[1], rate);
        prop[2] = g_player[local_pl].lp_verts_prop[2];
        prop[3] = base::interpolate(g_player[local_pl].lp_verts_prop[2], g_player[local_pl].lp_verts_prop[3], rate);
        texf4 tex = ImageMgr::Get().GetTexture("lp bar");
        tex.vert[1] = base::interpolate(tex.vert[0], tex.vert[1], rate);
        tex.vert[3] = base::interpolate(tex.vert[2], tex.vert[3], rate);
        g_player[local_pl].lpbar->BuildSprite(rel.data(), tex, prop.data());
        g_player[local_pl].fixed_numbers[(int32_t)FloatingNumberType::LP]->SetValue(lp);
        g_player[local_pl].lp = lp;
    }
    
    void DuelSceneHandler::AddChain(int32_t ct) {
        auto& param_node = layoutCfg["chain param"];
        auto param = sgui::SGJsonUtil::ConvertArray<float, 4>(param_node);
        auto& ch = g_duel.chains[ct];
        auto chain_sp = duel_scene->AddFieldSprite().get();
        chain_sp->SetTexture(ImageMgr::Get().GetRawMiscTexture());
        chain_sp->SetTexcoord(ImageMgr::Get().GetTexture("chain"));
        auto ccolor = sgui::SGJsonUtil::ConvertRGBA(param_node[4]);
        auto ncolor = sgui::SGJsonUtil::ConvertRGBA(param_node[5]);
        chain_sp->SetColor(ccolor);
        v3f translation;
        auto check_rep = [](int32_t ct, uint8_t loc, uint8_t seq)->int32_t {
            int32_t repc = 0;
            for(int32_t i = 0; i < ct; ++i)
                if((g_duel.chains[i].triggering_pos.location == loc)
                   && (seq != 0xff || g_duel.chains[i].triggering_pos.sequence == seq))
                    repc++;
            return repc;
        };
        switch(ch.triggering_pos.location) {
            case 0x1:
                translation = g_player[ch.triggering_pos.controler].field_blocks[13]->translation;
                translation.y += check_rep(ct, 0x1, 0xf) * 0.2f * translation.y;
                break;
            case 0x2:
                break;
            case 0x4:
                translation = g_player[ch.triggering_pos.controler].field_blocks[ch.triggering_pos.sequence]->translation;
                translation.y += check_rep(ct, 0x4, ch.triggering_pos.sequence) * 0.2f * translation.y;
                break;
            case 0x8:
                translation = g_player[ch.triggering_pos.controler].field_blocks[ch.triggering_pos.sequence + 5]->translation;
                translation.y += check_rep(ct, 0x8, ch.triggering_pos.sequence) * 0.2f * translation.y;
                break;
            case 0x10:
                translation = g_player[ch.triggering_pos.controler].field_blocks[15]->translation;
                translation.y += check_rep(ct, 0x10, 0xf) * 0.2f * translation.y;
                break;
            case 0x20:
                translation = g_player[ch.triggering_pos.controler].field_blocks[16]->translation;
                translation.y += check_rep(ct, 0x20, 0xf) * 0.2f * translation.y;
                break;
            case 0x40:
                translation = g_player[ch.triggering_pos.controler].field_blocks[14]->translation;
                translation.y += check_rep(ct, 0x40, 0xf) * 0.2f * translation.y;
                break;
        }
        chain_sp->SetTranslation(translation);
        chain_sp->SetSize({param[0], param[1]});
        g_duel.chains[ct].chain_sprites.push_back(chain_sp);
        auto rotact = std::make_shared<LerpAnimator<int64_t>>(0, [chain_sp](double t)->bool {
            chain_sp->SetRotation(glm::angleAxis(-3.1415926f * 2 * (float)t, glm::vec3(0.0f, 0.0f, 1.0f)));
            return true;
        }, std::make_shared<TGenPeriodic<int64_t>>(4000));
        SceneMgr::Get().PushAction(rotact, chain_sp);
        if(ct < 9) {
            auto num_sp = duel_scene->AddFieldSprite().get();
            num_sp->SetTexture(ImageMgr::Get().GetRawMiscTexture());
            num_sp->SetTexcoord(ImageMgr::Get().GetCharTex(L'0' + ct + 1));
            num_sp->SetTranslation(translation);
            num_sp->SetSize({param[2], param[3]});
            num_sp->SetColor(ncolor);
            g_duel.chains[ct].chain_sprites.push_back(num_sp);
        } else {
            auto num_sp1 = duel_scene->AddFieldSprite().get();
            num_sp1->SetTexture(ImageMgr::Get().GetRawMiscTexture());
            num_sp1->SetTexcoord(ImageMgr::Get().GetCharTex(L'0' + (ct + 1) / 10));
            num_sp1->SetTranslation(translation - v3f{param[2] * 0.25f, 0.0f, 0.0f});
            num_sp1->SetSize({param[2] * 0.5f, param[3]});
            num_sp1->SetColor(ncolor);
            auto num_sp2 = duel_scene->AddFieldSprite().get();
            num_sp2->SetTexture(ImageMgr::Get().GetRawMiscTexture());
            num_sp2->SetTexcoord(ImageMgr::Get().GetCharTex(L'0' + (ct + 1) % 10));
            num_sp2->SetTranslation(translation + v3f{param[2] * 0.25f, 0.0f, 0.0f});
            num_sp2->SetSize({param[2] * 0.5f, param[3]});
            num_sp2->SetColor(ncolor);
            g_duel.chains[ct].chain_sprites.push_back(num_sp1);
            g_duel.chains[ct].chain_sprites.push_back(num_sp2);
        }
    }
    
    void DuelSceneHandler::SetDisabledField(uint32_t fd_info) {
        if(g_duel.disabled_field == fd_info)
            return;
        g_duel.disabled_field = fd_info;
        uint32_t fd_playerinf[2] = {fd_info & 0xffff, fd_info >> 16};
        auto m_tex = ImageMgr::Get().GetTexture("mzone");
        auto m_tex_d = ImageMgr::Get().GetTexture("mzone_d");
        auto s_tex = ImageMgr::Get().GetTexture("szone");
        auto s_tex_d = ImageMgr::Get().GetTexture("szone_d");
        for(int32_t i = 0; i < 2; ++i) {
            int32_t pid = LocalPlayer(i);
            uint32_t d_mzone = fd_playerinf[i] & 0x1f;
            uint32_t d_szone = (fd_playerinf[i] >> 8) & 0x1f;
            for(int32_t s = 0; s < 5; ++s) {
                if(d_mzone & (1 << s))
                    g_player[pid].field_blocks[s]->SetTexcoord(m_tex_d);
                else
                    g_player[pid].field_blocks[s]->SetTexcoord(m_tex);
            }
            for(int32_t s = 0; s < 5; ++s) {
                if(d_szone & (1 << s))
                    g_player[pid].field_blocks[s + 5]->SetTexcoord(s_tex_d);
                else
                    g_player[pid].field_blocks[s + 5]->SetTexcoord(s_tex);
            }
        }
    }
    
    std::shared_ptr<FieldSprite> DuelSceneHandler::AddAttackSprite(v3f trans, glm::quat rot) {
        auto ptr = duel_scene->AddFieldSprite();
        ptr->SetSize(sgui::SGJsonUtil::ConvertVec2<float>(layoutCfg["attack icon size"]));
        ptr->SetTexture(ImageMgr::Get().GetRawMiscTexture());
        ptr->SetTexcoord(ImageMgr::Get().GetTexture("attack"));
        ptr->SetTranslation(trans);
        ptr->SetRotation(rot);
        g_duel.attack_sprite.push_back(ptr.get());
        return ptr;
    }
    
    void DuelSceneHandler::ClearAttackSprite() {
        for(auto& fs : g_duel.attack_sprite) {
            duel_scene->RemoveFieldSprite(fs);
            SceneMgr::Get().RemoveAction(fs);
        }
    }
    
}
