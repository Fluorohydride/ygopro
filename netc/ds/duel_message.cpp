#include "utils/common.h"

#include "ocgcore/common.h"

#include "../sgui.h"
#include "../gui_extra.h"

#include "duel_scene.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    int32_t DuelSceneHandler::SolveMessage(uint8_t msg_type, BufferUtil& reader) {
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
            case MSG_SELECT_OPTION: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_options.clear();
//                for (int i = 0; i < count; ++i)
//                    mainGame->dField.select_options.push_back(BufferIO::ReadInt32(pbuf));
//                mainGame->gMutex.Lock();
//                mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont,
//                                        (wchar_t*)dataManager.GetDesc(mainGame->dField.select_options[0]));
//                mainGame->btnOptionp->setVisible(false);
//                if(count > 1)
//                    mainGame->btnOptionn->setVisible(true);
//                else mainGame->btnOptionn->setVisible(false);
//                mainGame->dField.selected_option = 0;
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
//                else myswprintf(textBuffer, dataManager.GetSysString(555));
//                select_hint = 0;
//                mainGame->wOptions->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wOptions);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_SELECT_CARD: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_cancelable = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectable_cards.clear();
//                mainGame->dField.selected_cards.clear();
//                int c, l, s, ss;
//                unsigned int code;
//                bool panelmode = false;
//                mainGame->dField.select_ready = false;
//                ClientCard* pcard;
//                for (int i = 0; i < count; ++i) {
//                    code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    ss = BufferIO::ReadInt8(pbuf);
//                    if ((l & LOCATION_OVERLAY) > 0)
//                        pcard = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
//                    else
//                        pcard = mainGame->dField.GetCard(c, l, s);
//                    if (code != 0 && pcard->code != code)
//                        pcard->SetCode(code);
//                    pcard->select_seq = i;
//                    mainGame->dField.selectable_cards.push_back(pcard);
//                    pcard->is_selectable = true;
//                    pcard->is_selected = false;
//                    if (l & 0xf1)
//                        panelmode = true;
//                }
//                std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetDesc(select_hint),
//                               mainGame->dField.select_min, mainGame->dField.select_max);
//                else myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetSysString(560), mainGame->dField.select_min, mainGame->dField.select_max);
//                select_hint = 0;
//                if (panelmode) {
//                    mainGame->gMutex.Lock();
//                    mainGame->wCardSelect->setText(textBuffer);
//                    mainGame->dField.ShowSelectCard();
//                    mainGame->gMutex.Unlock();
//                } else {
//                    mainGame->stHintMsg->setText(textBuffer);
//                    mainGame->stHintMsg->setVisible(true);
//                }
                break;
            }
            case MSG_SELECT_CHAIN: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                int specount = BufferIO::ReadInt8(pbuf);
//                int forced = BufferIO::ReadInt8(pbuf);
//                /*int hint0 = */BufferIO::ReadInt32(pbuf);
//                /*int hint1 = */BufferIO::ReadInt32(pbuf);
//                int code, c, l, s, ss, desc;
//                ClientCard* pcard;
//                bool panelmode = false;
//                mainGame->dField.chain_forced = (forced != 0);
//                mainGame->dField.activatable_cards.clear();
//                mainGame->dField.activatable_descs.clear();
//                mainGame->dField.conti_cards.clear();
//                for (int i = 0; i < count; ++i) {
//                    code = BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    ss = BufferIO::ReadInt8(pbuf);
//                    desc = BufferIO::ReadInt32(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s, ss);
//                    mainGame->dField.activatable_cards.push_back(pcard);
//                    mainGame->dField.activatable_descs.push_back(desc);
//                    pcard->is_selected = false;
//                    if(code >= 1000000000) {
//                        pcard->is_conti = true;
//                        pcard->chain_code = code % 1000000000;
//                        mainGame->dField.conti_cards.push_back(pcard);
//                        mainGame->dField.remove_act = true;
//                    }
//                    else {
//                        pcard->chain_code = code;
//                        pcard->is_selectable = true;
//                        pcard->cmdFlag |= COMMAND_ACTIVATE;
//                        if(l == LOCATION_GRAVE)
//                            mainGame->dField.grave_act = true;
//                        else if(l == LOCATION_REMOVED)
//                            mainGame->dField.remove_act = true;
//                        else if(l == LOCATION_EXTRA)
//                            mainGame->dField.extra_act = true;
//                        else if(l == LOCATION_OVERLAY)
//                            panelmode = true;
//                    }
//                }
//                if(!forced && (mainGame->ignore_chain || ((count == 0 || specount == 0) && !mainGame->always_chain))) {
//                    SetResponseI(-1);
//                    mainGame->dField.ClearChainSelect();
//                    if(mainGame->chkWaitChain->isChecked()) {
//                        mainGame->WaitFrameSignal(rnd.real() * 20 + 20);
//                    }
//                    DuelClient::SendResponse();
//                    return true;
//                }
//                if(mainGame->chkAutoChain->isChecked() && forced) {
//                    SetResponseI(0);
//                    mainGame->dField.ClearChainSelect();
//                    DuelClient::SendResponse();
//                    return true;
//                }
//                mainGame->gMutex.Lock();
//                mainGame->stHintMsg->setText(dataManager.GetSysString(550));
//                mainGame->stHintMsg->setVisible(true);
//                if(panelmode) {
//                    mainGame->dField.list_command = COMMAND_ACTIVATE;
//                    mainGame->dField.selectable_cards = mainGame->dField.activatable_cards;
//                    std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
//                    auto eit = std::unique(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
//                    mainGame->dField.selectable_cards.erase(eit, mainGame->dField.selectable_cards.end());
//                    mainGame->dField.ShowChainCard();
//                } else {
//                    if(!forced) {
//                        if(count == 0)
//                            myswprintf(textBuffer, L"%ls\n%ls", dataManager.GetSysString(201), dataManager.GetSysString(202));
//                        else
//                            myswprintf(textBuffer, L"%ls\n%ls", event_string, dataManager.GetSysString(203));
//                        mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)textBuffer);
//                        mainGame->PopupElement(mainGame->wQuery);
//                    }
//                }
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_SELECT_PLACE:
            case MSG_SELECT_DISFIELD: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectable_field = ~BufferIO::ReadInt32(pbuf);
//                mainGame->dField.selected_field = 0;
//                unsigned char respbuf[64];
//                int pzone = 0;
//                if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE && mainGame->chkAutoPos->isChecked()) {
//                    int filter;
//                    if (mainGame->dField.selectable_field & 0x1f) {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 0 : 1;
//                        respbuf[1] = 0x4;
//                        filter = mainGame->dField.selectable_field & 0x1f;
//                    } else if (mainGame->dField.selectable_field & 0x1f00) {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 0 : 1;
//                        respbuf[1] = 0x8;
//                        filter = (mainGame->dField.selectable_field >> 8) & 0x1f;
//                    } else if (mainGame->dField.selectable_field & 0xc000) {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 0 : 1;
//                        respbuf[1] = 0x8;
//                        filter = (mainGame->dField.selectable_field >> 14) & 0x3;
//                        pzone = 1;
//                    } else if (mainGame->dField.selectable_field & 0x1f0000) {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 1 : 0;
//                        respbuf[1] = 0x4;
//                        filter = (mainGame->dField.selectable_field >> 16) & 0x1f;
//                    } else if (mainGame->dField.selectable_field & 0x1f000000) {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 1 : 0;
//                        respbuf[1] = 0x8;
//                        filter = (mainGame->dField.selectable_field >> 24) & 0x1f;
//                    } else {
//                        respbuf[0] = mainGame->dInfo.isFirst ? 1 : 0;
//                        respbuf[1] = 0x8;
//                        filter = (mainGame->dField.selectable_field >> 30) & 0x3;
//                        pzone = 1;
//                    }
//                    if(!pzone) {
//                        if(mainGame->chkRandomPos->isChecked()) {
//                            respbuf[2] = rnd.real() * 5;
//                            while(!(filter & (1 << respbuf[2])))
//                                respbuf[2] = rnd.real() * 5;
//                        } else {
//                            if (filter & 0x4) respbuf[2] = 2;
//                            else if (filter & 0x2) respbuf[2] = 1;
//                            else if (filter & 0x8) respbuf[2] = 3;
//                            else if (filter & 0x1) respbuf[2] = 0;
//                            else if (filter & 0x10) respbuf[2] = 4;
//                        }
//                    } else {
//                        if (filter & 0x1) respbuf[2] = 6;
//                        else if (filter & 0x2) respbuf[2] = 7;
//                    }
//                    mainGame->dField.selectable_field = 0;
//                    SetResponseB(respbuf, 3);
//                    DuelClient::SendResponse();
//                    return true;
//                }
                break;
            }
            case MSG_SELECT_POSITION: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                int positions = BufferIO::ReadInt8(pbuf);
//                if (positions == 0x1 || positions == 0x2 || positions == 0x4 || positions == 0x8) {
//                    SetResponseI(positions);
//                    return true;
//                }
//                int count = 0, filter = 0x1, startpos;
//                while(filter != 0x10) {
//                    if(positions & filter) count++;
//                    filter <<= 1;
//                }
//                if(count == 4) startpos = 10;
//                else if(count == 3) startpos = 82;
//                else startpos = 155;
//                if(positions & 0x1) {
//                    mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSAU, code));
//                    mainGame->btnPSAU->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
//                    mainGame->btnPSAU->setVisible(true);
//                    startpos += 145;
//                } else mainGame->btnPSAU->setVisible(false);
//                if(positions & 0x2) {
//                    mainGame->btnPSAD->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
//                    mainGame->btnPSAD->setVisible(true);
//                    startpos += 145;
//                } else mainGame->btnPSAD->setVisible(false);
//                if(positions & 0x4) {
//                    mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSDU, code));
//                    mainGame->btnPSDU->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
//                    mainGame->btnPSDU->setVisible(true);
//                    startpos += 145;
//                } else mainGame->btnPSDU->setVisible(false);
//                if(positions & 0x8) {
//                    mainGame->btnPSDD->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
//                    mainGame->btnPSDD->setVisible(true);
//                    startpos += 145;
//                } else mainGame->btnPSDD->setVisible(false);
//                mainGame->gMutex.Lock();
//                mainGame->PopupElement(mainGame->wPosSelect);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_SELECT_TRIBUTE: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_cancelable = BufferIO::ReadInt8(pbuf) ? true : false;
//                mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectable_cards.clear();
//                mainGame->dField.selected_cards.clear();
//                int c, l, s, t;
//                unsigned int code;
//                ClientCard* pcard;
//                mainGame->dField.select_ready = false;
//                for (int i = 0; i < count; ++i) {
//                    code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    t = BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    if (code && pcard->code != code)
//                        pcard->SetCode(code);
//                    mainGame->dField.selectable_cards.push_back(pcard);
//                    pcard->opParam = t;
//                    pcard->select_seq = i;
//                    pcard->is_selectable = true;
//                }
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetDesc(select_hint),
//                               mainGame->dField.select_min, mainGame->dField.select_max);
//                else myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetSysString(531), mainGame->dField.select_min, mainGame->dField.select_max);
//                select_hint = 0;
//                mainGame->gMutex.Lock();
//                mainGame->stHintMsg->setText(textBuffer);
//                mainGame->stHintMsg->setVisible(true);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_SELECT_COUNTER: {
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_counter_type = BufferIO::ReadInt16(pbuf);
//                mainGame->dField.select_counter_count = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectable_cards.clear();
//                int c, l, s, t/*, code*/;
//                ClientCard* pcard;
//                for (int i = 0; i < count; ++i) {
//                    /*code = */BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    t = BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    mainGame->dField.selectable_cards.push_back(pcard);
//                    pcard->opParam = (t << 16) | t;
//                    pcard->is_selectable = true;
//                }
//                myswprintf(textBuffer, dataManager.GetSysString(204), mainGame->dField.select_counter_count, dataManager.GetCounterName(mainGame->dField.select_counter_type));
//                mainGame->gMutex.Lock();
//                mainGame->stHintMsg->setText(textBuffer);
//                mainGame->stHintMsg->setVisible(true);
//                mainGame->gMutex.Unlock();
//                break;
            }
            case MSG_SELECT_SUM: {
//                mainGame->dField.select_mode = BufferIO::ReadInt8(pbuf);
//                /*int selecting_player = */BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_sumval = BufferIO::ReadInt32(pbuf);
//                mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectsum_all.clear();
//                mainGame->dField.selected_cards.clear();
//                mainGame->dField.selectsum_cards.clear();
//                int c, l, s;
//                unsigned int code;
//                bool panelmode = false;
//                ClientCard* pcard;
//                for (int i = 0; i < count; ++i) {
//                    code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    if (code != 0 && pcard->code != code)
//                        pcard->SetCode(code);
//                    pcard->opParam = BufferIO::ReadInt32(pbuf);
//                    pcard->select_seq = i;
//                    mainGame->dField.selectsum_all.push_back(pcard);
//                    if ((l & 0xe) == 0)
//                        panelmode = true;
//                }
//                std::sort(mainGame->dField.selectsum_all.begin(), mainGame->dField.selectsum_all.end(), ClientCard::client_card_sort);
//                mainGame->dField.CheckSelectSum();
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls(%d)", dataManager.GetDesc(select_hint), mainGame->dField.select_sumval);
//                else myswprintf(textBuffer, L"%ls(%d)", dataManager.GetSysString(560), mainGame->dField.select_sumval);
//                select_hint = 0;
//                if (panelmode) {
//                    mainGame->wCardSelect->setText(textBuffer);
//                    mainGame->dField.ShowSelectCard();
//                } else {
//                    mainGame->stHintMsg->setText(textBuffer);
//                    mainGame->stHintMsg->setVisible(true);
//                }
                break;
            }
            case MSG_SORT_CHAIN:
            case MSG_SORT_CARD: {
//                /*int player = */BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->dField.selectable_cards.clear();
//                mainGame->dField.selected_cards.clear();
//                mainGame->dField.sort_list.clear();
//                int c, l, s;
//                unsigned int code;
//                ClientCard* pcard;
//                for (int i = 0; i < count; ++i) {
//                    code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    if (code != 0 && pcard->code != code)
//                        pcard->SetCode(code);
//                    mainGame->dField.selectable_cards.push_back(pcard);
//                    mainGame->dField.sort_list.push_back(0);
//                }
//                if (mainGame->chkAutoChain->isChecked() && mainGame->dInfo.curMsg == MSG_SORT_CHAIN) {
//                    mainGame->dField.sort_list.clear();
//                    SetResponseI(-1);
//                    DuelClient::SendResponse();
//                    return true;
//                }
//                if(mainGame->dInfo.curMsg == MSG_SORT_CHAIN)
//                    mainGame->wCardSelect->setText(dataManager.GetSysString(206));
//                else
//                    mainGame->wCardSelect->setText(dataManager.GetSysString(205));
//                mainGame->dField.select_min = 0;
//                mainGame->dField.select_max = count;
//                mainGame->dField.ShowSelectCard();
                break;
            }
            case MSG_CONFIRM_DECKTOP: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int count = BufferIO::ReadInt8(pbuf);
//                int code;
//                ClientCard* pcard;
//                mainGame->dField.selectable_cards.clear();
//                for (int i = 0; i < count; ++i) {
//                    code = BufferIO::ReadInt32(pbuf);
//                    pbuf += 3;
//                    pcard = *(mainGame->dField.deck[player].rbegin() + i);
//                    if (code != 0)
//                        pcard->SetCode(code);
//                }
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                myswprintf(textBuffer, dataManager.GetSysString(207), count);
//                mainGame->lstLog->addItem(textBuffer);
//                mainGame->logParam.push_back(0);
//                for (int i = 0; i < count; ++i) {
//                    pcard = *(mainGame->dField.deck[player].rbegin() + i);
//                    mainGame->gMutex.Lock();
//                    myswprintf(textBuffer, L"*[%ls]", dataManager.GetName(pcard->code));
//                    mainGame->lstLog->addItem(textBuffer);
//                    mainGame->logParam.push_back(pcard->code);
//                    mainGame->gMutex.Unlock();
//                    float shift = -0.15f;
//                    if (player == 1) shift = 0.15f;
//                    pcard->dPos = irr::core::vector3df(shift, 0, 0);
//                    if(!mainGame->dField.deck_reversed)
//                        pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
//                    else pcard->dRot = irr::core::vector3df(0, 0, 0);
//                    pcard->is_moving = true;
//                    pcard->aniFrame = 5;
//                    mainGame->WaitFrameSignal(45);
//                    mainGame->dField.MoveCard(pcard, 5);
//                    mainGame->WaitFrameSignal(5);
//                }
                break;
            }
            case MSG_CONFIRM_CARDS: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int count = BufferIO::ReadInt8(pbuf);
//                int code, c, l, s;
//                std::vector<ClientCard*> field_confirm;
//                std::vector<ClientCard*> panel_confirm;
//                ClientCard* pcard;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pbuf += count * 7;
//                    return true;
//                }
//                myswprintf(textBuffer, dataManager.GetSysString(208), count);
//                mainGame->lstLog->addItem(textBuffer);
//                mainGame->logParam.push_back(0);
//                for (int i = 0; i < count; ++i) {
//                    code = BufferIO::ReadInt32(pbuf);
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    if (code != 0)
//                        pcard->SetCode(code);
//                    mainGame->gMutex.Lock();
//                    myswprintf(textBuffer, L"*[%ls]", dataManager.GetName(code));
//                    mainGame->lstLog->addItem(textBuffer);
//                    mainGame->logParam.push_back(code);
//                    mainGame->gMutex.Unlock();
//                    if (l & 0x41) {
//                        if(count == 1) {
//                            float shift = -0.15f;
//                            if (c == 0 && l == 0x40) shift = 0.15f;
//                            pcard->dPos = irr::core::vector3df(shift, 0, 0);
//                            if((l == LOCATION_DECK) && mainGame->dField.deck_reversed)
//                                pcard->dRot = irr::core::vector3df(0, 0, 0);
//                            else pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
//                            pcard->is_moving = true;
//                            pcard->aniFrame = 5;
//                            mainGame->WaitFrameSignal(45);
//                            mainGame->dField.MoveCard(pcard, 5);
//                            mainGame->WaitFrameSignal(5);
//                        } else {
//                            if(!mainGame->dInfo.isReplay)
//                                panel_confirm.push_back(pcard);
//                        }
//                    } else {
//                        if(!mainGame->dInfo.isReplay || (l & LOCATION_ONFIELD))
//                            field_confirm.push_back(pcard);
//                    }
//                }
//                if (field_confirm.size() > 0) {
//                    mainGame->WaitFrameSignal(5);
//                    for(size_t i = 0; i < field_confirm.size(); ++i) {
//                        pcard = field_confirm[i];
//                        c = pcard->controler;
//                        l = pcard->location;
//                        if (l == LOCATION_HAND) {
//                            mainGame->dField.MoveCard(pcard, 5);
//                        } else if (l == LOCATION_MZONE) {
//                            if (pcard->position & POS_FACEUP)
//                                continue;
//                            pcard->dPos = irr::core::vector3df(0, 0, 0);
//                            if (pcard->position == POS_FACEDOWN_ATTACK)
//                                pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
//                            else
//                                pcard->dRot = irr::core::vector3df(3.14159f / 5.0f, 0, 0);
//                            pcard->is_moving = true;
//                            pcard->aniFrame = 5;
//                        } else if (l == LOCATION_SZONE) {
//                            if (pcard->position & POS_FACEUP)
//                                continue;
//                            pcard->dPos = irr::core::vector3df(0, 0, 0);
//                            pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
//                            pcard->is_moving = true;
//                            pcard->aniFrame = 5;
//                        }
//                    }
//                    mainGame->WaitFrameSignal(90);
//                    for(size_t i = 0; i < field_confirm.size(); ++i) {
//                        mainGame->dField.MoveCard(field_confirm[i], 5);
//                    }
//                    mainGame->WaitFrameSignal(5);
//                }
//                if (panel_confirm.size()) {
//                    std::sort(panel_confirm.begin(), panel_confirm.end(), ClientCard::client_card_sort);
//                    mainGame->gMutex.Lock();
//                    mainGame->dField.selectable_cards = panel_confirm;
//                    myswprintf(textBuffer, dataManager.GetSysString(208), panel_confirm.size());
//                    mainGame->wCardSelect->setText(textBuffer);
//                    mainGame->dField.ShowSelectCard(true);
//                    mainGame->gMutex.Unlock();
//                    mainGame->actionSignal.Reset();
//                    mainGame->actionSignal.Wait();
//                }
                break;
            }
            case MSG_SHUFFLE_DECK: {
                int32_t playerid = LocalPlayer(reader.Read<uint8_t>());
                if(deck[playerid].size() < 2)
                    break;
                auto cb_action1 = std::make_shared<ActionCallback<int64_t>>([playerid]() {
                    for(auto& pcard : deck[playerid])
                        pcard->UpdatePosition(200, pcard->GetPositionInfo(1));
                });
                auto wait1 = std::make_shared<ActionWait<int64_t>>(200);
                auto cb_action2 = std::make_shared<ActionCallback<int64_t>>([playerid]() {
                    for(auto& pcard : deck[playerid])
                        pcard->UpdatePosition(200, pcard->GetPositionInfo());
                });
                auto wait2 = std::make_shared<ActionWait<int64_t>>(200);
                auto shuffle_action = std::make_shared<ActionSequence<int64_t>>(cb_action1, wait1, cb_action2, wait2);
                auto rep_action = std::make_shared<ActionRepeat<int64_t>>(shuffle_action, 2);
                if(deck_reversed) {
                    deck_reversed = false;
                    for(auto& pcard : deck[playerid])
                        pcard->UpdatePosition(200, pcard->GetPositionInfo());
                    auto wait_rev1 = std::make_shared<ActionWait<int64_t>>(200);
                    auto flip_action1 = std::make_shared<ActionCallback<int64_t>>([playerid]() {
                        deck_reversed = true;
                        for(auto& pcard : deck[playerid])
                            pcard->UpdatePosition(200, pcard->GetPositionInfo());
                    });
                    auto wait_rev2 = std::make_shared<ActionWait<int64_t>>(200);
                    PushMessageActions(wait_rev1, rep_action, flip_action1, wait_rev2);
                } else {
                    PushMessageActions(rep_action);
                }
                break;
            }
            case MSG_SHUFFLE_HAND: {
                int32_t playerid = LocalPlayer(reader.Read<uint8_t>());
                /*int32_t count = */reader.Read<uint8_t>();
                std::vector<uint32_t> code_after_shuffle;
                bool need_flip = false;
                for(auto& pcard : hand[playerid]) {
                    if(pcard->code == 0)
                        need_flip = true;
                    uint32_t ac = reader.Read<uint32_t>();
                    if(ac == 0)
                        need_flip = true;
                    code_after_shuffle.push_back(ac);
                }
                if(need_flip) {
                    for(auto& pcard : hand[playerid]) {
                        auto npos = pcard->GetPositionInfo(1);
                        pcard->UpdatePosition(200, npos);
                    }
                    auto wait1 = std::make_shared<ActionWait<int64_t>>(200);
                    auto cb_action1 = std::make_shared<ActionCallback<int64_t>>([playerid]() {
                        auto npos = hand[playerid][hand[playerid].size() / 2]->GetPositionInfo();
                        for(auto& pcard : hand[playerid])
                            pcard->UpdatePosition(200, npos);
                    });
                    auto wait2 = std::make_shared<ActionWait<int64_t>>(200);
                    auto cb_action2 = std::make_shared<ActionCallback<int64_t>>([playerid, code_after_shuffle]() {
                        for(size_t i = 0; i < hand[playerid].size(); ++i) {
                            hand[playerid][i]->SetCode(code_after_shuffle[i]);
                            auto npos = hand[playerid][i]->GetPositionInfo(1);
                            hand[playerid][i]->UpdatePosition(200, npos);
                        }
                    });
                    auto wait3 = std::make_shared<ActionWait<int64_t>>(200);
                    auto cb_action3 = std::make_shared<ActionCallback<int64_t>>([playerid]() {
                        for(auto& pcard : hand[playerid]) {
                            auto npos = pcard->GetPositionInfo();
                            pcard->UpdatePosition(200, npos);
                        }
                    });
                    auto wait4 = std::make_shared<ActionWait<int64_t>>(200);
                    PushMessageActions(wait1, cb_action1, wait2, cb_action2, wait3, cb_action3, wait4);
                } else {
                    auto npos = hand[playerid][hand[playerid].size() / 2]->GetPositionInfo();
                    for(auto& pcard : hand[playerid])
                        pcard->UpdatePosition(200, npos);
                    auto wait1 = std::make_shared<ActionWait<int64_t>>(200);
                    auto cb_action = std::make_shared<ActionCallback<int64_t>>([playerid, code_after_shuffle]() {
                        for(size_t i = 0; i < hand[playerid].size(); ++i) {
                            hand[playerid][i]->SetCode(code_after_shuffle[i]);
                            hand[playerid][i]->UpdatePosition(200);
                        }
                    });
                    auto wait2 = std::make_shared<ActionWait<int64_t>>(200);
                    PushMessageActions(wait1, cb_action, wait2);
                }
                break;
            }
            case MSG_REFRESH_DECK: {
                reader.Skip(1);
                break;
            }
            case MSG_SWAP_GRAVE_DECK: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
//                    for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit)
//                        (*cit)->location = LOCATION_GRAVE;
//                    for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
//                        if ((*cit)->type & 0x802040) {
//                            (*cit)->location = LOCATION_EXTRA;
//                            mainGame->dField.extra[player].push_back(*cit);
//                            cit = mainGame->dField.deck[player].erase(cit);
//                        } else {
//                            (*cit)->location = LOCATION_DECK;
//                            ++cit;
//                        }
//                    }
//                } else {
//                    mainGame->gMutex.Lock();
//                    mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
//                    for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit) {
//                        (*cit)->location = LOCATION_GRAVE;
//                        mainGame->dField.MoveCard(*cit, 10);
//                    }
//                    for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
//                        ClientCard* pcard = *cit;
//                        if (pcard->type & 0x802040) {
//                            pcard->location = LOCATION_EXTRA;
//                            mainGame->dField.extra[player].push_back(pcard);
//                            cit = mainGame->dField.deck[player].erase(cit);
//                        } else {
//                            pcard->location = LOCATION_DECK;
//                            ++cit;
//                        }
//                        mainGame->dField.MoveCard(pcard, 10);
//                    }
//                    mainGame->gMutex.Unlock();
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_SHUFFLE_SET_CARD: {
//                std::vector<ClientCard*>::iterator cit;
//                int count = BufferIO::ReadInt8(pbuf);
//                ClientCard* mc[5];
//                ClientCard* swp;
//                int c, l, s, ps;
//                for (int i = 0; i < count; ++i) {
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    BufferIO::ReadInt8(pbuf);
//                    mc[i] = mainGame->dField.mzone[c][s];
//                    mc[i]->SetCode(0);
//                    if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                        mc[i]->dPos = irr::core::vector3df((3.95f - mc[i]->curPos.X) / 10, 0, 0.05f);
//                        mc[i]->dRot = irr::core::vector3df(0, 0, 0);
//                        mc[i]->is_moving = true;
//                        mc[i]->aniFrame = 10;
//                    }
//                }
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
//                    mainGame->WaitFrameSignal(20);
//                for (int i = 0; i < count; ++i) {
//                    c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    l = BufferIO::ReadInt8(pbuf);
//                    s = BufferIO::ReadInt8(pbuf);
//                    BufferIO::ReadInt8(pbuf);
//                    ps = mc[i]->sequence;
//                    if (l > 0) {
//                        swp = mainGame->dField.mzone[c][s];
//                        mainGame->dField.mzone[c][ps] = swp;
//                        mainGame->dField.mzone[c][s] = mc[i];
//                        mc[i]->sequence = s;
//                        swp->sequence = ps;
//                    }
//                }
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for (int i = 0; i < count; ++i) {
//                        mainGame->dField.MoveCard(mc[i], 10);
//                        for (cit = mc[i]->overlayed.begin(); cit != mc[i]->overlayed.end(); ++cit)
//                            mainGame->dField.MoveCard(*cit, 10);
//                    }
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_REVERSE_DECK: {
//                mainGame->dField.deck_reversed = !mainGame->dField.deck_reversed;
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for(size_t i = 0; i < mainGame->dField.deck[0].size(); ++i)
//                        mainGame->dField.MoveCard(mainGame->dField.deck[0][i], 10);
//                    for(size_t i = 0; i < mainGame->dField.deck[1].size(); ++i)
//                        mainGame->dField.MoveCard(mainGame->dField.deck[1][i], 10);
//                }
                break;
            }
            case MSG_DECK_TOP: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int seq = BufferIO::ReadInt8(pbuf);
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                ClientCard* pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1 - seq);
//                pcard->SetCode(code & 0x7fffffff);
//                bool rev = (code & 0x80000000) != 0;
//                if(pcard->is_reversed != rev) {
//                    pcard->is_reversed = rev;
//                    mainGame->dField.MoveCard(pcard, 5);
//                }
                break;
            }
            case MSG_NEW_TURN: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                mainGame->dInfo.turn++;
//                if(!mainGame->dInfo.isTag && mainGame->dInfo.turn == 5 && !mainGame->dInfo.isReplay && mainGame->dInfo.player_type < 7) {
//                    mainGame->btnLeaveGame->setText(dataManager.GetSysString(1351));
//                    mainGame->btnLeaveGame->setVisible(true);
//                }
//                if(mainGame->dInfo.isTag && mainGame->dInfo.turn != 1) {
//                    if(player == 0)
//                        mainGame->dInfo.tag_player[0] = !mainGame->dInfo.tag_player[0];
//                    else
//                        mainGame->dInfo.tag_player[1] = !mainGame->dInfo.tag_player[1];
//                }
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    myswprintf(mainGame->dInfo.strTurn, L"Turn:%d", mainGame->dInfo.turn);
//                    mainGame->showcardcode = 10;
//                    mainGame->showcarddif = 30;
//                    mainGame->showcardp = 0;
//                    mainGame->showcard = 101;
//                    mainGame->WaitFrameSignal(40);
//                    mainGame->showcard = 0;
//                }
                break;
            }
            case MSG_NEW_PHASE: {
//                int phase = (unsigned char)BufferIO::ReadInt8(pbuf);
//                mainGame->btnDP->setVisible(false);
//                mainGame->btnSP->setVisible(false);
//                mainGame->btnM1->setVisible(false);
//                mainGame->btnBP->setVisible(false);
//                mainGame->btnM2->setVisible(false);
//                mainGame->btnEP->setVisible(false);
//                mainGame->btnShuffle->setVisible(false);
//                mainGame->showcarddif = 30;
//                mainGame->showcardp = 0;
//                switch (phase) {
//                    case PHASE_DRAW:
//                        mainGame->btnDP->setVisible(true);
//                        mainGame->showcardcode = 4;
//                        break;
//                    case PHASE_STANDBY:
//                        mainGame->btnSP->setVisible(true);
//                        mainGame->showcardcode = 5;
//                        break;
//                    case PHASE_MAIN1:
//                        mainGame->btnM1->setVisible(true);
//                        mainGame->showcardcode = 6;
//                        break;
//                    case PHASE_BATTLE:
//                        mainGame->btnBP->setVisible(true);
//                        mainGame->btnBP->setPressed(true);
//                        mainGame->btnBP->setEnabled(false);
//                        mainGame->showcardcode = 7;
//                        break;
//                    case PHASE_MAIN2:
//                        mainGame->btnM2->setVisible(true);
//                        mainGame->btnM2->setPressed(true);
//                        mainGame->btnM2->setEnabled(false);
//                        mainGame->showcardcode = 8;
//                        break;
//                    case PHASE_END:
//                        mainGame->btnEP->setVisible(true);
//                        mainGame->btnEP->setPressed(true);
//                        mainGame->btnEP->setEnabled(false);
//                        mainGame->showcardcode = 9;
//                        break;
//                }
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    mainGame->showcard = 101;
//                    mainGame->WaitFrameSignal(40);
//                    mainGame->showcard = 0;
//                }
                break;
            }
            case MSG_MOVE: {
                CardPosInfo pi_from, pi_to;
                uint32_t code = reader.Read<uint32_t>();
                pi_from.info = reader.Read<int32_t>();
                pi_to.info = reader.Read<int32_t>();
                uint32_t reason = reader.Read<uint32_t>();
                auto ptr = GetCard(pi_from);
                if(ptr != nullptr) {
                    if(code)
                        ptr->SetCode(code);
                    ptr->reason = reason;
                    if(pi_from.location == 0) {
                        
                    } else if(pi_to.location == 0) {
                        
                    } else if((pi_from.location & 0x80) && (pi_to.location & 0x80)) {
                        MoveCard(ptr, pi_to);
                        ptr->UpdatePosition(500);
                        PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                    } else if((pi_from.location & 0x80) && !(pi_to.location & 0x80)) {
                        MoveCard(ptr, pi_to);
                        ptr->UpdatePosition(500);
                        PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                    } else if(!(pi_from.location & 0x80) && (pi_to.location & 0x80)) {
                        auto attach_card = GetCard(pi_to);
                        if(attach_card) {
                            if(pi_to.location != LOCATION_EXTRA) {
                                MoveCard(ptr, pi_to);
                                ptr->UpdatePosition(500);
                                PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                            }
                        }
                    } else {
                        if(!ptr->attached_cards.empty()) {
                            if(pi_from.location == LOCATION_EXTRA) {
                                MoveCard(ptr, pi_to);
                                for(auto olcard : ptr->attached_cards)
                                    olcard->UpdatePosition(500);
                                auto wait_action = std::make_shared<ActionWait<int64_t>>(500);
                                auto update_action = std::make_shared<ActionCallback<int64_t>>([ptr](){ ptr->UpdatePosition(500); });
                                PushMessageActions(wait_action, update_action);
                            } else {
                                MoveCard(ptr, pi_to);
                                ptr->UpdatePosition(500);
                                PushMessageActions(std::make_shared<ActionWait<int64_t>>(300));
                            }
                        }
                    }
                }
                break;
            }
            case MSG_POS_CHANGE: {
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int cl = BufferIO::ReadInt8(pbuf);
//                int cs = BufferIO::ReadInt8(pbuf);
//                int pp = BufferIO::ReadInt8(pbuf);
//                int cp = BufferIO::ReadInt8(pbuf);
//                ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
//                if((pp & POS_FACEUP) && (cp & POS_FACEDOWN)) {
//                    pcard->counters.clear();
//                    pcard->ClearTarget();
//                }
//                if (code != 0 && pcard->code != code)
//                    pcard->SetCode(code);
//                pcard->position = cp;
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    myswprintf(event_string, dataManager.GetSysString(1600));
//                    mainGame->dField.MoveCard(pcard, 10);
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_SET: {
//                /*int code = */BufferIO::ReadInt32(pbuf);
//                /*int cc = mainGame->LocalPlayer*/(BufferIO::ReadInt8(pbuf));
//                /*int cl = */BufferIO::ReadInt8(pbuf);
//                /*int cs = */BufferIO::ReadInt8(pbuf);
//                /*int cp = */BufferIO::ReadInt8(pbuf);
//                myswprintf(event_string, dataManager.GetSysString(1601));
                break;
            }
            case MSG_SWAP: {
//                /*int code1 = */BufferIO::ReadInt32(pbuf);
//                int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l1 = BufferIO::ReadInt8(pbuf);
//                int s1 = BufferIO::ReadInt8(pbuf);
//                /*int p1 = */BufferIO::ReadInt8(pbuf);
//                /*int code2 = */BufferIO::ReadInt32(pbuf);
//                int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l2 = BufferIO::ReadInt8(pbuf);
//                int s2 = BufferIO::ReadInt8(pbuf);
//                /*int p2 = */BufferIO::ReadInt8(pbuf);
//                myswprintf(event_string, dataManager.GetSysString(1602));
//                ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
//                ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    mainGame->gMutex.Lock();
//                    mainGame->dField.RemoveCard(c1, l1, s1);
//                    mainGame->dField.RemoveCard(c2, l2, s2);
//                    mainGame->dField.AddCard(pc1, c2, l2, s2);
//                    mainGame->dField.AddCard(pc2, c1, l1, s1);
//                    mainGame->dField.MoveCard(pc1, 10);
//                    mainGame->dField.MoveCard(pc2, 10);
//                    for (size_t i = 0; i < pc1->overlayed.size(); ++i)
//                        mainGame->dField.MoveCard(pc1->overlayed[i], 10);
//                    for (size_t i = 0; i < pc2->overlayed.size(); ++i)
//                        mainGame->dField.MoveCard(pc2->overlayed[i], 10);
//                    mainGame->gMutex.Unlock();
//                    mainGame->WaitFrameSignal(11);
//                } else {
//                    mainGame->dField.RemoveCard(c1, l1, s1);
//                    mainGame->dField.RemoveCard(c2, l2, s2);
//                    mainGame->dField.AddCard(pc1, c2, l2, s2);
//                    mainGame->dField.AddCard(pc2, c1, l1, s1);
//                }
                break;
            }
            case MSG_FIELD_DISABLED: {
//                int disabled = BufferIO::ReadInt32(pbuf);
//                if (!mainGame->dInfo.isFirst)
//                    disabled = (disabled >> 16) | (disabled << 16);
//                mainGame->dField.disabled_field = disabled;
                break;
            }
            case MSG_SUMMONING: {
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                /*int cc = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                /*int cl = */BufferIO::ReadInt8(pbuf);
//                /*int cs = */BufferIO::ReadInt8(pbuf);
//                /*int cp = */BufferIO::ReadInt8(pbuf);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    myswprintf(event_string, dataManager.GetSysString(1603), dataManager.GetName(code));
//                    mainGame->showcardcode = code;
//                    mainGame->showcarddif = 0;
//                    mainGame->showcardp = 0;
//                    mainGame->showcard = 7;
//                    mainGame->WaitFrameSignal(30);
//                    mainGame->showcard = 0;
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_SUMMONED: {
//                myswprintf(event_string, dataManager.GetSysString(1604));
                break;
            }
            case MSG_SPSUMMONING: {
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                /*int cc = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                /*int cl = */BufferIO::ReadInt8(pbuf);
//                /*int cs = */BufferIO::ReadInt8(pbuf);
//                /*int cp = */BufferIO::ReadInt8(pbuf);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    myswprintf(event_string, dataManager.GetSysString(1605), dataManager.GetName(code));
//                    mainGame->showcardcode = code;
//                    mainGame->showcarddif = 1;
//                    mainGame->showcard = 5;
//                    mainGame->WaitFrameSignal(30);
//                    mainGame->showcard = 0;
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_SPSUMMONED: {
//                myswprintf(event_string, dataManager.GetSysString(1606));
                break;
            }
            case MSG_FLIPSUMMONING: {
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int cl = BufferIO::ReadInt8(pbuf);
//                int cs = BufferIO::ReadInt8(pbuf);
//                int cp = BufferIO::ReadInt8(pbuf);
//                ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
//                pcard->SetCode(code);
//                pcard->position = cp;
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    myswprintf(event_string, dataManager.GetSysString(1607), dataManager.GetName(code));
//                    mainGame->dField.MoveCard(pcard, 10);
//                    mainGame->WaitFrameSignal(11);
//                    mainGame->showcardcode = code;
//                    mainGame->showcarddif = 0;
//                    mainGame->showcardp = 0;
//                    mainGame->showcard = 7;
//                    mainGame->WaitFrameSignal(30);
//                    mainGame->showcard = 0;
//                    mainGame->WaitFrameSignal(11);
//                }
                break;
            }
            case MSG_FLIPSUMMONED: {
//                myswprintf(event_string, dataManager.GetSysString(1608));
                break;
            }
            case MSG_CHAINING: {
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                int pcc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int pcl = BufferIO::ReadInt8(pbuf);
//                int pcs = BufferIO::ReadInt8(pbuf);
//                int subs = BufferIO::ReadInt8(pbuf);
//                int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int cl = BufferIO::ReadInt8(pbuf);
//                int cs = BufferIO::ReadInt8(pbuf);
//                int desc = BufferIO::ReadInt32(pbuf);
//                /*int ct = */BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                ClientCard* pcard = mainGame->dField.GetCard(pcc, pcl, pcs, subs);
//                if(pcard->code != code) {
//                    pcard->code = code;
//                    mainGame->dField.MoveCard(pcard, 10);
//                }
//                mainGame->showcardcode = code;
//                mainGame->showcarddif = 0;
//                mainGame->showcard = 1;
//                pcard->is_highlighting = true;
//                mainGame->WaitFrameSignal(30);
//                pcard->is_highlighting = false;
//                mainGame->dField.current_chain.chain_card = pcard;
//                mainGame->dField.current_chain.code = code;
//                mainGame->dField.current_chain.desc = desc;
//                mainGame->dField.current_chain.controler = cc;
//                mainGame->dField.current_chain.location = cl;
//                mainGame->dField.current_chain.sequence = cs;
//                mainGame->dField.GetChainLocation(cc, cl, cs, &mainGame->dField.current_chain.chain_pos);
//                mainGame->dField.current_chain.solved = false;
//                int chc = 0;
//                for(size_t i = 0; i < mainGame->dField.chains.size(); ++i) {
//                    if (cl == 0x10 || cl == 0x20) {
//                        if (mainGame->dField.chains[i].controler == cc && mainGame->dField.chains[i].location == cl)
//                            chc++;
//                    } else {
//                        if (mainGame->dField.chains[i].controler == cc && mainGame->dField.chains[i].location == cl && mainGame->dField.chains[i].sequence == cs)
//                            chc++;
//                    }
//                }
//                if(cl == LOCATION_HAND)
//                    mainGame->dField.current_chain.chain_pos.X += 0.35f;
//                else
//                    mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
                break;
            }
            case MSG_CHAINED: {
//                int ct = BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                myswprintf(event_string, dataManager.GetSysString(1609), dataManager.GetName(mainGame->dField.current_chain.code));
//                mainGame->gMutex.Lock();
//                mainGame->dField.chains.push_back(mainGame->dField.current_chain);
//                mainGame->gMutex.Unlock();
//                if (ct > 1)
//                    mainGame->WaitFrameSignal(20);
//                mainGame->dField.last_chain = true;
                break;
            }
            case MSG_CHAIN_SOLVING: {
//                int ct = BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                if (mainGame->dField.chains.size() > 1) {
//                    if (mainGame->dField.last_chain)
//                        mainGame->WaitFrameSignal(11);
//                    for(int i = 0; i < 5; ++i) {
//                        mainGame->dField.chains[ct - 1].solved = false;
//                        mainGame->WaitFrameSignal(3);
//                        mainGame->dField.chains[ct - 1].solved = true;
//                        mainGame->WaitFrameSignal(3);
//                    }
//                }
//                mainGame->dField.last_chain = false;
                break;
            }
            case MSG_CHAIN_SOLVED: {
//                /*int ct = */BufferIO::ReadInt8(pbuf);
                break;
            }
            case MSG_CHAIN_END: {
//                mainGame->dField.chains.clear();
                break;
            }
            case MSG_CHAIN_NEGATED:
            case MSG_CHAIN_DISABLED: {
//                int ct = BufferIO::ReadInt8(pbuf);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    mainGame->showcardcode = mainGame->dField.chains[ct - 1].code;
//                    mainGame->showcarddif = 0;
//                    mainGame->showcard = 3;
//                    mainGame->WaitFrameSignal(30);
//                    mainGame->showcard = 0;
//                }
                break;
            }
            case MSG_CARD_SELECTED: {
                break;
            }
            case MSG_RANDOM_SELECTED: {
//                /*int player = */BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pbuf += count * 4;
//                    return true;
//                }
//                ClientCard* pcards[10];
//                for (int i = 0; i < count; ++i) {
//                    int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    int l = BufferIO::ReadInt8(pbuf);
//                    int s = BufferIO::ReadInt8(pbuf);
//                    int ss = BufferIO::ReadInt8(pbuf);
//                    if ((l & 0x80) > 0)
//                        pcards[i] = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
//                    else
//                        pcards[i] = mainGame->dField.GetCard(c, l, s);
//                    pcards[i]->is_highlighting = true;
//                }
//                mainGame->WaitFrameSignal(30);
//                for(int i = 0; i < count; ++i)
//                    pcards[i]->is_highlighting = false;
                break;
            }
            case MSG_BECOME_TARGET: {
//                int count = BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pbuf += count * 4;
//                    return true;
//                }
//                ClientCard* pcard;
//                for (int i = 0; i < count; ++i) {
//                    int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                    int l = BufferIO::ReadInt8(pbuf);
//                    int s = BufferIO::ReadInt8(pbuf);
//                    /*int ss = */BufferIO::ReadInt8(pbuf);
//                    pcard = mainGame->dField.GetCard(c, l, s);
//                    pcard->is_highlighting = true;
//                    if(pcard->location & LOCATION_ONFIELD) {
//                        for (int j = 0; j < 3; ++j) {
//                            mainGame->dField.FadeCard(pcard, 5, 5);
//                            mainGame->WaitFrameSignal(5);
//                            mainGame->dField.FadeCard(pcard, 255, 5);
//                            mainGame->WaitFrameSignal(5);
//                        }
//                    } else
//                        mainGame->WaitFrameSignal(30);
//                    myswprintf(textBuffer, dataManager.GetSysString(1610), dataManager.GetName(pcard->code), dataManager.FormatLocation(l, s), s + 1);
//                    mainGame->lstLog->addItem(textBuffer);
//                    mainGame->logParam.push_back(pcard->code);
//                    pcard->is_highlighting = false;
//                }
                break;
            }
            case MSG_DRAW: {
                uint32_t playerid = LocalPlayer(reader.Read<uint8_t>());
                uint32_t count = reader.Read<uint8_t>();
                std::vector<std::shared_ptr<Action<int64_t>>> acts;
                for(uint32_t i = 0; i < count; ++i) {
                    uint32_t data = reader.Read<uint32_t>();
                    acts.push_back(std::make_shared<ActionCallback<int64_t>>([playerid, count, data, this]() {
                        if(deck[playerid].empty())
                            return;
                        auto ptr = deck[playerid].back();
                        MoveCard(ptr, CardPosInfo(playerid, LOCATION_HAND, 0, 0));
                        for(auto& iter : hand[playerid])
                            iter->UpdatePosition(500);
                    }));
                    acts.push_back(std::make_shared<ActionWait<int64>>(300));
                }
                PushMessageActions(acts);
                break;
            }
            case MSG_DAMAGE: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int val = BufferIO::ReadInt32(pbuf);
//                int final = mainGame->dInfo.lp[player] - val;
//                if (final < 0)
//                    final = 0;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dInfo.lp[player] = final;
//                    myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                    return true;
//                }
//                mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
//                if (player == 0)
//                    myswprintf(event_string, dataManager.GetSysString(1613), val);
//                else
//                    myswprintf(event_string, dataManager.GetSysString(1614), val);
//                mainGame->lpccolor = 0xffff0000;
//                mainGame->lpplayer = player;
//                myswprintf(textBuffer, L"-%d", val);
//                mainGame->lpcstring = textBuffer;
//                mainGame->WaitFrameSignal(30);
//                mainGame->lpframe = 10;
//                mainGame->WaitFrameSignal(11);
//                mainGame->lpcstring = 0;
//                mainGame->dInfo.lp[player] = final;
//                mainGame->gMutex.Lock();
//                myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_RECOVER: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int val = BufferIO::ReadInt32(pbuf);
//                int final = mainGame->dInfo.lp[player] + val;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dInfo.lp[player] = final;
//                    myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                    return true;
//                }
//                mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
//                if (player == 0)
//                    myswprintf(event_string, dataManager.GetSysString(1615), val);
//                else
//                    myswprintf(event_string, dataManager.GetSysString(1616), val);
//                mainGame->lpccolor = 0xff00ff00;
//                mainGame->lpplayer = player;
//                myswprintf(textBuffer, L"+%d", val);
//                mainGame->lpcstring = textBuffer;
//                mainGame->WaitFrameSignal(30);
//                mainGame->lpframe = 10;
//                mainGame->WaitFrameSignal(11);
//                mainGame->lpcstring = 0;
//                mainGame->dInfo.lp[player] = final;
//                mainGame->gMutex.Lock();
//                myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_EQUIP: {
//                int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l1 = BufferIO::ReadInt8(pbuf);
//                int s1 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l2 = BufferIO::ReadInt8(pbuf);
//                int s2 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
//                ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pc1->equipTarget = pc2;
//                    pc2->equipped.insert(pc1);
//                } else {
//                    mainGame->gMutex.Lock();
//                    pc1->equipTarget = pc2;
//                    pc2->equipped.insert(pc1);
//                    if (mainGame->dField.hovered_card == pc1)
//                        pc2->is_showequip = true;
//                    else if (mainGame->dField.hovered_card == pc2)
//                        pc1->is_showequip = true;
//                    mainGame->gMutex.Unlock();
//                }
                break;
            }
            case MSG_LPUPDATE: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int val = BufferIO::ReadInt32(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dInfo.lp[player] = val;
//                    myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                    return true;
//                }
//                mainGame->lpd = (mainGame->dInfo.lp[player] - val) / 10;
//                mainGame->lpplayer = player;
//                mainGame->lpframe = 10;
//                mainGame->WaitFrameSignal(11);
//                mainGame->dInfo.lp[player] = val;
//                mainGame->gMutex.Lock();
//                myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_UNEQUIP: {
//                int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l1 = BufferIO::ReadInt8(pbuf);
//                int s1 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                ClientCard* pc = mainGame->dField.GetCard(c1, l1, s1);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pc->equipTarget->equipped.erase(pc);
//                    pc->equipTarget = 0;
//                } else {
//                    mainGame->gMutex.Lock();
//                    if (mainGame->dField.hovered_card == pc)
//                        pc->equipTarget->is_showequip = false;
//                    else if (mainGame->dField.hovered_card == pc->equipTarget)
//                        pc->is_showequip = false;
//                    pc->equipTarget->equipped.erase(pc);
//                    pc->equipTarget = 0;
//                    mainGame->gMutex.Unlock();
//                }
                break;
            }
            case MSG_CARD_TARGET: {
//                int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l1 = BufferIO::ReadInt8(pbuf);
//                int s1 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l2 = BufferIO::ReadInt8(pbuf);
//                int s2 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
//                ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pc1->cardTarget.insert(pc2);
//                    pc2->ownerTarget.insert(pc1);
//                } else {
//                    mainGame->gMutex.Lock();
//                    pc1->cardTarget.insert(pc2);
//                    pc2->ownerTarget.insert(pc1);
//                    if (mainGame->dField.hovered_card == pc1)
//                        pc2->is_showtarget = true;
//                    else if (mainGame->dField.hovered_card == pc2)
//                        pc1->is_showtarget = true;
//                    mainGame->gMutex.Unlock();
//                }
                break;
            }
            case MSG_CANCEL_TARGET: {
//                int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l1 = BufferIO::ReadInt8(pbuf);
//                int s1 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l2 = BufferIO::ReadInt8(pbuf);
//                int s2 = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
//                ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    pc1->cardTarget.erase(pc2);
//                    pc2->ownerTarget.erase(pc1);
//                } else {
//                    mainGame->gMutex.Lock();
//                    pc1->cardTarget.erase(pc2);
//                    pc2->ownerTarget.erase(pc1);
//                    if (mainGame->dField.hovered_card == pc1)
//                        pc2->is_showtarget = false;
//                    else if (mainGame->dField.hovered_card == pc2)
//                        pc1->is_showtarget = false;
//                    mainGame->gMutex.Unlock();
//                }
                break;
            }
            case MSG_PAY_LPCOST: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int cost = BufferIO::ReadInt32(pbuf);
//                int final = mainGame->dInfo.lp[player] - cost;
//                if (final < 0)
//                    final = 0;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
//                    mainGame->dInfo.lp[player] = final;
//                    myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                    return true;
//                }
//                mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
//                mainGame->lpccolor = 0xff0000ff;
//                mainGame->lpplayer = player;
//                myswprintf(textBuffer, L"-%d", cost);
//                mainGame->lpcstring = textBuffer;
//                mainGame->WaitFrameSignal(30);
//                mainGame->lpframe = 10;
//                mainGame->WaitFrameSignal(11);
//                mainGame->lpcstring = 0;
//                mainGame->dInfo.lp[player] = final;
//                mainGame->gMutex.Lock();
//                myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_ADD_COUNTER: {
//                int type = BufferIO::ReadInt16(pbuf);
//                int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l = BufferIO::ReadInt8(pbuf);
//                int s = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                ClientCard* pc = mainGame->dField.GetCard(c, l, s);
//                if (pc->counters.count(type))
//                    pc->counters[type] += count;
//                else pc->counters[type] = count;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                myswprintf(textBuffer, dataManager.GetSysString(1617), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type));
//                pc->is_highlighting = true;
//                mainGame->gMutex.Lock();
//                mainGame->stACMessage->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wACMessage, 20);
//                mainGame->gMutex.Unlock();
//                mainGame->WaitFrameSignal(40);
//                pc->is_highlighting = false;
                break;
            }
            case MSG_REMOVE_COUNTER: {
//                int type = BufferIO::ReadInt16(pbuf);
//                int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l = BufferIO::ReadInt8(pbuf);
//                int s = BufferIO::ReadInt8(pbuf);
//                int count = BufferIO::ReadInt8(pbuf);
//                ClientCard* pc = mainGame->dField.GetCard(c, l, s);
//                pc->counters[type] -= count;
//                if (pc->counters[type] <= 0)
//                    pc->counters.erase(type);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                myswprintf(textBuffer, dataManager.GetSysString(1618), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type));
//                pc->is_highlighting = true;
//                mainGame->gMutex.Lock();
//                mainGame->stACMessage->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wACMessage, 20);
//                mainGame->gMutex.Unlock();
//                mainGame->WaitFrameSignal(40);
//                pc->is_highlighting = false;
                break;
            }
            case MSG_ATTACK: {
//                int ca = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int la = BufferIO::ReadInt8(pbuf);
//                int sa = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                mainGame->dField.attacker = mainGame->dField.GetCard(ca, la, sa);
//                int cd = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int ld = BufferIO::ReadInt8(pbuf);
//                int sd = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                float sy;
//                if (ld != 0) {
//                    mainGame->dField.attack_target = mainGame->dField.GetCard(cd, ld, sd);
//                    myswprintf(event_string, dataManager.GetSysString(1619), dataManager.GetName(mainGame->dField.attacker->code),
//                               dataManager.GetName(mainGame->dField.attack_target->code));
//                    float xa = mainGame->dField.attacker->curPos.X;
//                    float ya = mainGame->dField.attacker->curPos.Y;
//                    float xd = mainGame->dField.attack_target->curPos.X;
//                    float yd = mainGame->dField.attack_target->curPos.Y;
//                    sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
//                    mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
//                    if (ca == 0)
//                        mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
//                    else
//                        mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
//                } else {
//                    myswprintf(event_string, dataManager.GetSysString(1620), dataManager.GetName(mainGame->dField.attacker->code));
//                    float xa = mainGame->dField.attacker->curPos.X;
//                    float ya = mainGame->dField.attacker->curPos.Y;
//                    float xd = 3.95f;
//                    float yd = 3.5f;
//                    if (ca == 0)
//                        yd = -3.5f;
//                    sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
//                    mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
//                    if (ca == 0)
//                        mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
//                    else
//                        mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
//                }
//                matManager.GenArrow(sy);
//                mainGame->attack_sv = 0;
//                mainGame->is_attacking = true;
//                mainGame->WaitFrameSignal(40);
//                mainGame->is_attacking = false;
                break;
            }
            case MSG_BATTLE: {
//                int ca = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int la = BufferIO::ReadInt8(pbuf);
//                int sa = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int aatk = BufferIO::ReadInt32(pbuf);
//                int adef = BufferIO::ReadInt32(pbuf);
//                /*int da = */BufferIO::ReadInt8(pbuf);
//                int cd = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int ld = BufferIO::ReadInt8(pbuf);
//                int sd = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int datk = BufferIO::ReadInt32(pbuf);
//                int ddef = BufferIO::ReadInt32(pbuf);
//                /*int dd = */BufferIO::ReadInt8(pbuf);
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                mainGame->gMutex.Lock();
//                ClientCard* pcard = mainGame->dField.GetCard(ca, la, sa);
//                if(aatk != pcard->attack) {
//                    pcard->attack = aatk;
//                    myswprintf(pcard->atkstring, L"%d", aatk);
//                }
//                if(adef != pcard->defence) {
//                    pcard->defence = adef;
//                    myswprintf(pcard->defstring, L"%d", adef);
//                }
//                if(ld) {
//                    pcard = mainGame->dField.GetCard(cd, ld, sd);
//                    if(datk != pcard->attack) {
//                        pcard->attack = datk;
//                        myswprintf(pcard->atkstring, L"%d", datk);
//                    }
//                    if(ddef != pcard->defence) {
//                        pcard->defence = ddef;
//                        myswprintf(pcard->defstring, L"%d", ddef);
//                    }
//                }
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_ATTACK_NEGATED: {
//                myswprintf(event_string, dataManager.GetSysString(1621), dataManager.GetName(mainGame->dField.attacker->code));
                break;
            }
            case MSG_DAMAGE_STEP_START: {
                break;
            }
            case MSG_DAMAGE_STEP_END: {
                break;
            }
            case MSG_MISSED_EFFECT: {
//                BufferIO::ReadInt32(pbuf);
//                unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
//                myswprintf(textBuffer, dataManager.GetSysString(1622), dataManager.GetName(code));
//                mainGame->lstLog->addItem(textBuffer);
//                mainGame->logParam.push_back(code);
                break;
            }
            case MSG_BE_CHAIN_TARGET:
                break;
            case MSG_CREATE_RELATION:
                break;
            case MSG_RELEASE_RELATION:
                break;
            case MSG_TOSS_COIN: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int count = BufferIO::ReadInt8(pbuf);
//                wchar_t* pwbuf = textBuffer;
//                BufferIO::CopyWStrRef(dataManager.GetSysString(1623), pwbuf, 256);
//                for (int i = 0; i < count; ++i) {
//                    int res = BufferIO::ReadInt8(pbuf);
//                    *pwbuf++ = L'[';
//                    BufferIO::CopyWStrRef(dataManager.GetSysString(res ? 60 : 61), pwbuf, 256);
//                    *pwbuf++ = L']';
//                }
//                *pwbuf = 0;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                mainGame->gMutex.Lock();
//                mainGame->lstLog->addItem(textBuffer);
//                mainGame->logParam.push_back(0);
//                mainGame->stACMessage->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wACMessage, 20);
//                mainGame->gMutex.Unlock();
//                mainGame->WaitFrameSignal(40);
                break;
            }
            case MSG_TOSS_DICE: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int count = BufferIO::ReadInt8(pbuf);
//                wchar_t* pwbuf = textBuffer;
//                BufferIO::CopyWStrRef(dataManager.GetSysString(1624), pwbuf, 256);
//                for (int i = 0; i < count; ++i) {
//                    int res = BufferIO::ReadInt8(pbuf);
//                    *pwbuf++ = L'[';
//                    *pwbuf++ = L'0' + res;
//                    *pwbuf++ = L']';
//                }
//                *pwbuf = 0;
//                if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                    return true;
//                mainGame->gMutex.Lock();
//                mainGame->lstLog->addItem(textBuffer);
//                mainGame->logParam.push_back(0);
//                mainGame->stACMessage->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wACMessage, 20);
//                mainGame->gMutex.Unlock();
//                mainGame->WaitFrameSignal(40);
                break;
            }
            case MSG_DECLEAR_RACE: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                mainGame->dField.announce_count = BufferIO::ReadInt8(pbuf);
//                int available = BufferIO::ReadInt32(pbuf);
//                for(int i = 0, filter = 0x1; i < 24; ++i, filter <<= 1) {
//                    mainGame->chkRace[i]->setChecked(false);
//                    if(filter & available)
//                        mainGame->chkRace[i]->setVisible(true);
//                    else mainGame->chkRace[i]->setVisible(false);
//                }
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
//                else myswprintf(textBuffer, dataManager.GetSysString(563));
//                select_hint = 0;
//                mainGame->gMutex.Lock();
//                mainGame->wANRace->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wANRace);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_DECLEAR_ATTRIB: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                mainGame->dField.announce_count = BufferIO::ReadInt8(pbuf);
//                int available = BufferIO::ReadInt32(pbuf);
//                for(int i = 0, filter = 0x1; i < 7; ++i, filter <<= 1) {
//                    mainGame->chkAttribute[i]->setChecked(false);
//                    if(filter & available)
//                        mainGame->chkAttribute[i]->setVisible(true);
//                    else mainGame->chkAttribute[i]->setVisible(false);
//                }
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
//                else myswprintf(textBuffer, dataManager.GetSysString(562));
//                select_hint = 0;
//                mainGame->gMutex.Lock();
//                mainGame->wANAttribute->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wANAttribute);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_DECLEAR_CARD: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
//                else myswprintf(textBuffer, dataManager.GetSysString(564));
//                select_hint = 0;
//                mainGame->gMutex.Lock();
//                mainGame->ebANCard->setText(L"");
//                mainGame->wANCard->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wANCard);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_DECLEAR_NUMBER: {
//                /*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int count = BufferIO::ReadInt8(pbuf);
//                mainGame->gMutex.Lock();
//                mainGame->cbANNumber->clear();
//                for (int i = 0; i < count; ++i) {
//                    int value = BufferIO::ReadInt32(pbuf);
//                    myswprintf(textBuffer, L" % d", value);
//                    mainGame->cbANNumber->addItem(textBuffer, value);
//                }
//                mainGame->cbANNumber->setSelected(0);
//                if(select_hint)
//                    myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
//                else myswprintf(textBuffer, dataManager.GetSysString(565));
//                select_hint = 0;
//                mainGame->wANNumber->setText(textBuffer);
//                mainGame->PopupElement(mainGame->wANNumber);
//                mainGame->gMutex.Unlock();
                break;
            }
            case MSG_CARD_HINT: {
//                int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                int l = BufferIO::ReadInt8(pbuf);
//                int s = BufferIO::ReadInt8(pbuf);
//                BufferIO::ReadInt8(pbuf);
//                int chtype = BufferIO::ReadInt8(pbuf);
//                int value = BufferIO::ReadInt32(pbuf);
//                ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
//                if(!pcard)
//                    return true;
//                if(chtype == CHINT_DESC_ADD) {
//                    pcard->desc_hints[value]++;
//                } else if(chtype == CHINT_DESC_REMOVE) {
//                    pcard->desc_hints[value]--;
//                    if(pcard->desc_hints[value] == 0)
//                        pcard->desc_hints.erase(value);
//                } else {
//                    pcard->cHint = chtype;
//                    pcard->chValue = value;
//                    if(chtype == CHINT_TURN) {
//                        if(value == 0)
//                            return true;
//                        if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
//                            return true;
//                        if(pcard->location & LOCATION_ONFIELD)
//                            pcard->is_highlighting = true;
//                        mainGame->showcardcode = pcard->code;
//                        mainGame->showcarddif = 0;
//                        mainGame->showcardp = value - 1;
//                        mainGame->showcard = 6;
//                        mainGame->WaitFrameSignal(30);
//                        pcard->is_highlighting = false;
//                        mainGame->showcard = 0;
//                    }
//                }
                break;
            }
            case MSG_TAG_SWAP: {
//                int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
//                size_t mcount = (size_t)BufferIO::ReadInt8(pbuf);
//                size_t ecount = (size_t)BufferIO::ReadInt8(pbuf);
//                size_t pcount = (size_t)BufferIO::ReadInt8(pbuf);
//                size_t hcount = (size_t)BufferIO::ReadInt8(pbuf);
//                int topcode = BufferIO::ReadInt32(pbuf);
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
//                        if(player == 0) (*cit)->dPos.Y = 0.4f;
//                        else (*cit)->dPos.Y = -0.6f;
//                        (*cit)->dRot = irr::core::vector3df(0, 0, 0);
//                        (*cit)->is_moving = true;
//                        (*cit)->aniFrame = 5;
//                    }
//                    for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
//                        if(player == 0) (*cit)->dPos.Y = 0.4f;
//                        else (*cit)->dPos.Y = -0.6f;
//                        (*cit)->dRot = irr::core::vector3df(0, 0, 0);
//                        (*cit)->is_moving = true;
//                        (*cit)->aniFrame = 5;
//                    }
//                    for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
//                        if(player == 0) (*cit)->dPos.Y = 0.4f;
//                        else (*cit)->dPos.Y = -0.6f;
//                        (*cit)->dRot = irr::core::vector3df(0, 0, 0);
//                        (*cit)->is_moving = true;
//                        (*cit)->aniFrame = 5;
//                    }
//                    mainGame->WaitFrameSignal(5);
//                }
//                //
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
//                    mainGame->gMutex.Lock();
//                if(mainGame->dField.deck[player].size() > mcount) {
//                    while(mainGame->dField.deck[player].size() > mcount) {
//                        ClientCard* ccard = *mainGame->dField.deck[player].rbegin();
//                        mainGame->dField.deck[player].pop_back();
//                        delete ccard;
//                    }
//                } else {
//                    while(mainGame->dField.deck[player].size() < mcount) {
//                        ClientCard* ccard = new ClientCard();
//                        ccard->controler = player;
//                        ccard->location = LOCATION_DECK;
//                        ccard->sequence = mainGame->dField.deck[player].size();
//                        mainGame->dField.deck[player].push_back(ccard);
//                    }
//                }
//                if(mainGame->dField.hand[player].size() > hcount) {
//                    while(mainGame->dField.hand[player].size() > hcount) {
//                        ClientCard* ccard = *mainGame->dField.hand[player].rbegin();
//                        mainGame->dField.hand[player].pop_back();
//                        delete ccard;
//                    }
//                } else {
//                    while(mainGame->dField.hand[player].size() < hcount) {
//                        ClientCard* ccard = new ClientCard();
//                        ccard->controler = player;
//                        ccard->location = LOCATION_HAND;
//                        ccard->sequence = mainGame->dField.hand[player].size();
//                        mainGame->dField.hand[player].push_back(ccard);
//                    }
//                }
//                if(mainGame->dField.extra[player].size() > ecount) {
//                    while(mainGame->dField.extra[player].size() > ecount) {
//                        ClientCard* ccard = *mainGame->dField.extra[player].rbegin();
//                        mainGame->dField.extra[player].pop_back();
//                        delete ccard;
//                    }
//                } else {
//                    while(mainGame->dField.extra[player].size() < ecount) {
//                        ClientCard* ccard = new ClientCard();
//                        ccard->controler = player;
//                        ccard->location = LOCATION_EXTRA;
//                        ccard->sequence = mainGame->dField.extra[player].size();
//                        mainGame->dField.extra[player].push_back(ccard);
//                    }
//                }
//                mainGame->dField.extra_p_count[player] = pcount;
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
//                    mainGame->gMutex.Unlock();
//                //
//                if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
//                    for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
//                        ClientCard* pcard = *cit;
//                        mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
//                        if(player == 0) pcard->curPos.Y += 2.0f;
//                        else pcard->curPos.Y -= 3.0f;
//                        mainGame->dField.MoveCard(*cit, 5);
//                    }
//                    if(mainGame->dField.deck[player].size())
//                        (*mainGame->dField.deck[player].rbegin())->code = topcode;
//                    for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
//                        ClientCard* pcard = *cit;
//                        pcard->code = BufferIO::ReadInt32(pbuf);
//                        mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
//                        if(player == 0) pcard->curPos.Y += 2.0f;
//                        else pcard->curPos.Y -= 3.0f;
//                        mainGame->dField.MoveCard(*cit, 5);
//                    }
//                    for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
//                        ClientCard* pcard = *cit;
//                        pcard->code = BufferIO::ReadInt32(pbuf) & 0x7fffffff;
//                        mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
//                        if(player == 0) pcard->curPos.Y += 2.0f;
//                        else pcard->curPos.Y -= 3.0f;
//                        mainGame->dField.MoveCard(*cit, 5);
//                    }
//                    mainGame->WaitFrameSignal(5);
//                }
                break;
            }
            case MSG_RELOAD_FIELD: {
                for(int32_t p = 0; p < 2; ++p) {
                    InitHp(p, reader.Read<int32_t>());
                    for(int32_t i = 0; i < 5; ++i) {
                        uint32_t exist_card = reader.Read<uint8_t>();
                        if(exist_card) {
                            uint32_t pos = reader.Read<uint8_t>();
                            uint32_t sz = reader.Read<uint8_t>();
                            AddCard(0, CardPosInfo(p, LOCATION_MZONE, i, pos));
                            for(int32_t ov = 0; ov < sz; ++ov)
                                AddCard(0, CardPosInfo(p, LOCATION_MZONE | LOCATION_OVERLAY, i, ov));
                        }
                    }
                    for(int32_t i = 0; i < 8; ++i) {
                        uint32_t exist_card = reader.Read<uint8_t>();
                        if(exist_card) {
                            uint32_t pos = reader.Read<uint8_t>();
                            AddCard(0, CardPosInfo(p, LOCATION_SZONE, i, pos));
                        }
                    }
                    int32_t main_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < main_sz; ++i)
                        AddCard(0, CardPosInfo(p, LOCATION_DECK, i, POS_FACEDOWN));
                    int32_t hand_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < hand_sz; ++i)
                        AddCard(0, CardPosInfo(p, LOCATION_HAND, i, POS_FACEDOWN));
                    int32_t grave_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < grave_sz; ++i)
                        AddCard(0, CardPosInfo(p, LOCATION_GRAVE, i, POS_FACEUP));
                    int32_t remove_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < remove_sz; ++i)
                        AddCard(0, CardPosInfo(p, LOCATION_REMOVED, i, POS_FACEUP));
                    int32_t extra_sz = reader.Read<uint8_t>();
                    for(int32_t i = 0; i < extra_sz; ++i)
                        AddCard(0, CardPosInfo(p, LOCATION_EXTRA, i, POS_FACEDOWN));
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
