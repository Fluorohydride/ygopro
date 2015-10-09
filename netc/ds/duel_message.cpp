#include "utils/common.h"

#include "ocgcore/common.h"

#include "../sgui.h"
#include "../gui_extra.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    int32_t DuelSceneHandler::SolveMessage(uint8_t msg_type, BufferUtil& reader) {
        auto pscene = duel_scene.lock();
        switch(msg_type) {
            case MSG_RETRY:
                break;
            case MSG_HINT: {
//                uint8_t hint_type = reader.Read<uint8_t>();
//                reader.Skip(1);
//                uint32_t hint_data = reader.Read<uint32_t>();
//                switch (hint_type) {
//                    case HINT_EVENT:
//                        break;
//                    case HINT_MESSAGE:
//                        break;
//                    case HINT_SELECTMSG:
//                        break;
//                    case HINT_OPSELECTED:
//                        break;
//                    case HINT_EFFECT:
//                        break;
//                    case HINT_RACE:
//                        break;
//                    case HINT_ATTRIB:
//                        break;
//                    case HINT_CODE:
//                        break;
//                    case HINT_NUMBER:
//                        break;
//                    case HINT_CARD:
//                        break;
//                    default:
//                        break;
//                }
                break;
            }
            case MSG_START:
                break;
            case MSG_WIN: {
//                uint8_t win_player = reader.Read<uint8_t>();
//                uint8_t win_reason = reader.Read<uint8_t>();
                break;
            }
            case MSG_UPDATE_CARD:
                break;
            case MSG_SELECT_BATTLECMD: {
//                int32_t playerid = reader.Read<uint8_t>();
//                int32_t act_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < act_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                    uint32_t desc = reader.Read<uint32_t>();
//                }
//                int32_t atk_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < act_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                    uint32_t param = reader.Read<uint8_t>();
//                }
//                bool can_m2 = reader.Read<uint8_t>() == 1;
//                bool can_ep = reader.Read<uint8_t>() == 1;
                break;
            }
            case MSG_SELECT_MAINCMD: {
//                int32_t playerid = reader.Read<uint8_t>();
//                int32_t summon_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < summon_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                }
//                int32_t spsummon_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < spsummon_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                }
//                int32_t repos_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < repos_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                }
//                int32_t mset_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < mset_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                }
//                int32_t sset_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < sset_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                }
//                int32_t act_sz = reader.Read<uint8_t>();
//                for(int32_t i = 0; i < act_sz; ++i) {
//                    uint32_t code = reader.Read<uint32_t>();
//                    uint32_t con = reader.Read<uint8_t>();
//                    uint32_t loc = reader.Read<uint8_t>();
//                    uint32_t seq = reader.Read<uint8_t>();
//                    uint32_t desc = reader.Read<uint32_t>();
//                }
//                bool can_bp = reader.Read<uint8_t>() == 1;
//                bool can_ep = reader.Read<uint8_t>() == 1;
                break;
            }
            case MSG_SELECT_EFFECTYN: {
//                int32_t playerid = reader.Read<uint8_t>();
//                uint32_t code = reader.Read<uint32_t>();
//                uint32_t info_loc = reader.Read<uint32_t>();
                break;
            }
            case MSG_SELECT_YESNO: {
//                int32_t playerid = reader.Read<uint8_t>();
//                uint32_t desc = reader.Read<uint32_t>();
                break;
            }
            case MSG_SELECT_OPTION:
                break;
            case MSG_SELECT_CARD:
                break;
            case MSG_SELECT_CHAIN:
                break;
            case MSG_SELECT_PLACE:
                break;
            case MSG_SELECT_POSITION:
                break;
            case MSG_SELECT_TRIBUTE:
                break;
            case MSG_SORT_CHAIN:
                break;
            case MSG_SELECT_COUNTER:
                break;
            case MSG_SELECT_SUM:
                break;
            case MSG_SELECT_DISFIELD:
                break;
            case MSG_SORT_CARD:
                break;
            case MSG_CONFIRM_DECKTOP:
                break;
            case MSG_CONFIRM_CARDS:
                break;
            case MSG_SHUFFLE_DECK: {
//                int32_t player = LocalPlayer(reader.Read<uint8_t>());
//                if(mainGame->dField.deck[player].size() < 2)
//                    return true;
//                bool rev = mainGame->dField.deck_reversed;
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dField.deck_reversed = false;
//                    if(rev) {
//                        for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
//                            mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
//                        mainGame->WaitFrameSignal(10);
//                    }
//                }
//                for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i) {
//                    mainGame->dField.deck[player][i]->code = 0;
//                    mainGame->dField.deck[player][i]->is_reversed = false;
//                }
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for (int i = 0; i < 5; ++i) {
//                        for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
//                            (*cit)->dPos = irr::core::vector3df(rand() * 0.4f / RAND_MAX - 0.2f, 0, 0);
//                            (*cit)->dRot = irr::core::vector3df(0, 0, 0);
//                            (*cit)->is_moving = true;
//                            (*cit)->aniFrame = 3;
//                        }
//                        mainGame->WaitFrameSignal(3);
//                        for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit)
//                            mainGame->dField.MoveCard(*cit, 3);
//                        mainGame->WaitFrameSignal(3);
//                    }
//                    mainGame->dField.deck_reversed = rev;
//                    if(rev) {
//                        for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
//                            mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
//                    }
//                }
                break;
            }
            case MSG_SHUFFLE_HAND: {
//                int32_t player = LocalPlayer(reader.Read<uint8_t>());
//                /*int count = */BufferIO::ReadInt8(pbuf);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    mainGame->WaitFrameSignal(5);
//                    if(player == 1 && !mainGame->dInfo.isReplay && !mainGame->dInfo.isSingleMode) {
//                        bool flip = false;
//                        for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
//                            if((*cit)->code) {
//                                (*cit)->dPos = irr::core::vector3df(0, 0, 0);
//                                (*cit)->dRot = irr::core::vector3df(1.322f / 5, 3.1415926f / 5, 0);
//                                (*cit)->is_moving = true;
//                                (*cit)->is_hovered = false;
//                                (*cit)->aniFrame = 5;
//                                flip = true;
//                            }
//                        if(flip)
//                            mainGame->WaitFrameSignal(5);
//                    }
//                    for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
//                        (*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / 5, 0, 0);
//                        (*cit)->dRot = irr::core::vector3df(0, 0, 0);
//                        (*cit)->is_moving = true;
//                        (*cit)->is_hovered = false;
//                        (*cit)->aniFrame = 5;
//                    }
//                    mainGame->WaitFrameSignal(11);
//                }
//                for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
//                    (*cit)->SetCode(BufferIO::ReadInt32(pbuf));
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
//                        (*cit)->is_hovered = false;
//                        mainGame->dField.MoveCard(*cit, 5);
//                    }
//                    mainGame->WaitFrameSignal(5);
//                }
                break;
            }
            case MSG_REFRESH_DECK:
                break;
            case MSG_SWAP_GRAVE_DECK:
                break;
            case MSG_SHUFFLE_SET_CARD:
                break;
            case MSG_REVERSE_DECK:
                break;
            case MSG_DECK_TOP:
                break;
            case MSG_NEW_TURN:
                break;
            case MSG_NEW_PHASE:
                break;
            case MSG_MOVE: {
                uint32_t code = reader.Read<uint32_t>();
                uint32_t pcon = reader.Read<uint8_t>();
                uint32_t ploc = reader.Read<uint8_t>();
                uint32_t pseq = reader.Read<uint8_t>();
                uint32_t psubs = reader.Read<uint8_t>();
                uint32_t con = reader.Read<uint8_t>();
                uint32_t loc = reader.Read<uint8_t>();
                uint32_t seq = reader.Read<uint8_t>();
                uint32_t subs = reader.Read<uint8_t>();
                uint32_t reason = reader.Read<uint32_t>();
                auto ptr = GetCard(pcon, ploc, pseq, psubs);
                if(ptr != nullptr) {
                    if(code)
                        ptr->SetCode(code);
                    if((ploc & 0x80) && (loc & 0x80)) {
                        MoveCard(ptr, con, loc, seq, subs);
                        ptr->UpdatePosition(500);
                        PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                    } else if((ploc & 0x80) && !(loc & 0x80)) {
                        MoveCard(ptr, con, loc, seq, subs);
                        ptr->UpdatePosition(500);
                        PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                    } else if(!(ploc & 0x80) && (loc & 0x80)) {
                        auto attach_card = GetCard(con, loc, seq, subs);
                        if(attach_card) {
                            if(loc != LOCATION_EXTRA) {
                                MoveCard(ptr, con, loc, seq, subs);
                                ptr->UpdatePosition(500);
                                PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                            }
                        }
                    } else {
                        if(!ptr->attached_cards.empty()) {
                            if(ploc == LOCATION_EXTRA) {
                                MoveCard(ptr, con, loc, seq, subs);
                                for(auto olcard : ptr->attached_cards)
                                    olcard->UpdatePosition(500);
                                auto wait_action = std::make_shared<ActionWait<int64_t>>(500);
                                auto update_action = std::make_shared<ActionCallback<int64_t>>([ptr](){ ptr->UpdatePosition(500); });
                                PushMessageActions(wait_action, update_action);
                            } else {
                                MoveCard(ptr, con, loc, seq, subs);
                                ptr->UpdatePosition(500);
                                PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                            }
                        }
                    }
                }
                break;
            }
            case MSG_POS_CHANGE:
                break;
            case MSG_SET:
                break;
            case MSG_SWAP:
                break;
            case MSG_FIELD_DISABLED:
                break;
            case MSG_SUMMONING:
                break;
            case MSG_SUMMONED:
                break;
            case MSG_SPSUMMONING:
                break;
            case MSG_SPSUMMONED:
                break;
            case MSG_FLIPSUMMONING:
                break;
            case MSG_FLIPSUMMONED:
                break;
            case MSG_CHAINING:
                break;
            case MSG_CHAINED:
                break;
            case MSG_CHAIN_SOLVING:
                break;
            case MSG_CHAIN_SOLVED:
                break;
            case MSG_CHAIN_END:
                break;
            case MSG_CHAIN_NEGATED:
                break;
            case MSG_CHAIN_DISABLED:
                break;
            case MSG_CARD_SELECTED:
                break;
            case MSG_RANDOM_SELECTED:
                break;
            case MSG_BECOME_TARGET:
                break;
            case MSG_DRAW: {
                uint32_t playerid = LocalPlayer(reader.Read<uint8_t>());
                uint32_t count = reader.Read<uint8_t>();
                std::vector<std::shared_ptr<Action<int64_t>>> acts;
                for(uint32_t i = 0; i < count; ++i) {
                    uint32_t data = reader.Read<uint32_t>();
                    acts.push_back(std::make_shared<ActionCallback<int64_t>>([playerid, count, data, this]() {
                        auto pscene = duel_scene.lock();
                        if(deck[playerid].empty())
                            return;
                        auto ptr = deck[playerid].back();
                        MoveCard(ptr, playerid, LOCATION_HAND, 0, 0);
                        for(auto& iter : hand[playerid])
                            iter->UpdatePosition(500);
                    }));
                    acts.push_back(std::make_shared<ActionWait<int64>>(300));
                }
                PushMessageActions(acts);
                break;
            }
            case MSG_DAMAGE:
                break;
            case MSG_RECOVER:
                break;
            case MSG_EQUIP:
                break;
            case MSG_LPUPDATE:
                break;
            case MSG_UNEQUIP:
                break;
            case MSG_CARD_TARGET:
                break;
            case MSG_CANCEL_TARGET:
                break;
            case MSG_PAY_LPCOST:
                break;
            case MSG_ADD_COUNTER:
                break;
            case MSG_REMOVE_COUNTER:
                break;
            case MSG_ATTACK:
                break;
            case MSG_BATTLE:
                break;
            case MSG_ATTACK_NEGATED:
                break;
            case MSG_DAMAGE_STEP_START:
                break;
            case MSG_DAMAGE_STEP_END:
                break;
            case MSG_MISSED_EFFECT:
                break;
            case MSG_BE_CHAIN_TARGET:
                break;
            case MSG_CREATE_RELATION:
                break;
            case MSG_RELEASE_RELATION:
                break;
            case MSG_TOSS_COIN:
                break;
            case MSG_TOSS_DICE:
                break;
            case MSG_DECLEAR_RACE:
                break;
            case MSG_DECLEAR_ATTRIB:
                break;
            case MSG_DECLEAR_CARD:
                break;
            case MSG_DECLEAR_NUMBER:
                break;
            case MSG_CARD_HINT:
                break;
            case MSG_TAG_SWAP:
                break;
            case MSG_RELOAD_FIELD: {
                for(int32_t p = 0; p < 2; ++p) {
                    InitHp(p, reader.Read<int32_t>());
                    for(int32_t i = 0; i < 5; ++i) {
                        uint32_t exist_card = reader.Read<uint8_t>();
                        if(exist_card) {
                            uint32_t pos = reader.Read<uint8_t>();
                            uint32_t sz = reader.Read<uint8_t>();
                            AddCard(0, p, LOCATION_MZONE, i, pos);
                            for(int32_t ov = 0; ov < sz; ++ov)
                                AddCard(0, p, LOCATION_MZONE | LOCATION_OVERLAY, i, ov);
                        }
                    }
                    for(int32_t i = 0; i < 8; ++i) {
                        uint32_t exist_card = reader.Read<uint8_t>();
                        if(exist_card) {
                            uint32_t pos = reader.Read<uint8_t>();
                            AddCard(0, p, LOCATION_SZONE, i, pos);
                        }
                    }
                    int32_t main_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < main_sz; ++i)
                        AddCard(0, p, LOCATION_DECK, i, POS_FACEDOWN);
                    int32_t hand_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < hand_sz; ++i)
                        AddCard(0, p, LOCATION_HAND, i, POS_FACEDOWN);
                    int32_t grave_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < grave_sz; ++i)
                        AddCard(0, p, LOCATION_GRAVE, i, POS_FACEUP);
                    int32_t remove_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < remove_sz; ++i)
                        AddCard(0, p, LOCATION_REMOVED, i, POS_FACEUP);
                    int32_t extra_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < extra_sz; ++i)
                        AddCard(0, p, LOCATION_EXTRA, i, POS_FACEDOWN);
                }
                int32_t chain_sz = reader.Read<uint8_t>();
                for(int32_t i = 0; i < chain_sz; ++i) {
                    reader.Skip(15);
    //                    cinfos[i].code = reader.Read<uint32_t>();
    //                    cinfos[i].card_con = reader.Read<uint8_t>();
    //                    cinfos[i].card_loc = reader.Read<uint8_t>();
    //                    cinfos[i].card_seq = reader.Read<uint8_t>();
    //                    cinfos[i].card_pos = reader.Read<uint8_t>();
    //                    cinfos[i].trig_con = reader.Read<uint8_t>();
    //                    cinfos[i].trig_loc = reader.Read<uint8_t>();
    //                    cinfos[i].trig_seq = reader.Read<uint8_t>();
    //                    cinfos[i].desc = reader.Read<uint32_t>();
                }
                break;
            }
            case MSG_AI_NAME:
                break;
            case MSG_SHOW_HINT:
                break;
            case MSG_MATCH_KILL:
                break;
            case MSG_CUSTOM_MSG:
                break;
            default:
                break;
        }
        return 0;
    }
    
}
