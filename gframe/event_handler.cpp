#include "event_handler.h"
#include "client_field.h"
#include "math.h"
#include "network.h"
#include "game.h"
#include "duelclient.h"
#include "data_manager.h"
#include "image_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "materials.h"
#include "../ocgcore/field.h"

namespace ygo {

bool ClientField::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_LOG: {
				mainGame->lstLog->clear();
				mainGame->logParam.clear();
				break;
			}
			case BUTTON_HAND1:
			case BUTTON_HAND2:
			case BUTTON_HAND3: {
				mainGame->wHand->setVisible(false);
				mainGame->stHintMsg->setText(L"");
				mainGame->stHintMsg->setVisible(true);
				CTOS_HandResult cshr;
				cshr.res = id - BUTTON_HAND1 + 1;
				DuelClient::SendPacketToServer(CTOS_HAND_RESULT, cshr);
				break;
			}
			case BUTTON_FIRST:
			case BUTTON_SECOND: {
				mainGame->HideElement(mainGame->wFTSelect);
				CTOS_TPResult cstr;
				cstr.res = BUTTON_SECOND - id;
				DuelClient::SendPacketToServer(CTOS_TP_RESULT, cstr);
				break;
			}
			case BUTTON_REPLAY_START: {
				if(!mainGame->dInfo.isReplay)
					break;
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->btnReplayUndo->setVisible(false);
				ReplayMode::Pause(false, false);
				break;
			}
			case BUTTON_REPLAY_PAUSE: {
				if(!mainGame->dInfo.isReplay)
					break;
				mainGame->btnReplayStart->setVisible(true);
				mainGame->btnReplayPause->setVisible(false);
				mainGame->btnReplayStep->setVisible(true);
				mainGame->btnReplayUndo->setVisible(true);
				ReplayMode::Pause(true, false);
				break;
			}
			case BUTTON_REPLAY_STEP: {
				if(!mainGame->dInfo.isReplay)
					break;
				ReplayMode::Pause(false, true);
				break;
			}
			case BUTTON_REPLAY_EXIT: {
				if(!mainGame->dInfo.isReplay)
					break;
				ReplayMode::StopReplay();
				break;
			}
			case BUTTON_REPLAY_SWAP: {
				if(!mainGame->dInfo.isReplay)
					break;
				ReplayMode::SwapField();
				break;
			}
			case BUTTON_REPLAY_UNDO: {
				if(!mainGame->dInfo.isReplay)
					break;
				ReplayMode::Undo();
				break;
			}
			case BUTTON_REPLAY_SAVE: {
				if(mainGame->ebRSName->getText()[0] == 0)
					break;
				mainGame->actionParam = 1;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				mainGame->actionParam = 0;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_LEAVE_GAME: {
				if(mainGame->dInfo.isSingleMode) {
					mainGame->singleSignal.SetNoWait(true);
					SingleMode::StopPlay(false);
					break;
				}
				if(mainGame->dInfo.player_type == 7) {
					DuelClient::StopClient();
					mainGame->dInfo.isStarted = false;
					mainGame->device->setEventReceiver(&mainGame->menuHandler);
					mainGame->wCardImg->setVisible(false);
					mainGame->wInfos->setVisible(false);
					mainGame->wPhase->setVisible(false);
					mainGame->btnLeaveGame->setVisible(false);
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->ShowElement(mainGame->wLanWindow);
				} else {
					DuelClient::SendPacketToServer(CTOS_SURRENDER);
					if(panel)
						mainGame->HideElement(panel);
				}
				break;
			}
			case BUTTON_MSG_OK: {
				mainGame->HideElement(mainGame->wMessage);
				mainGame->actionSignal.Set();
				break;
			}
			case BUTTON_YES: {
				switch(mainGame->dInfo.curMsg) {
				case MSG_SELECT_YESNO:
				case MSG_SELECT_EFFECTYN: {
					if(highlighting_card)
						highlighting_card->is_highlighting = false;
					highlighting_card = 0;
					DuelClient::SetResponseI(1);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				case MSG_SELECT_CHAIN:
				case MSG_SELECT_CARD:
				case MSG_SELECT_TRIBUTE:
				case MSG_SELECT_SUM: {
					mainGame->HideElement(mainGame->wQuery);
					break;
				}
				default: {
					mainGame->HideElement(mainGame->wQuery);
					break;
				}
				}
				break;
			}
			case BUTTON_NO: {
				switch(mainGame->dInfo.curMsg) {
				case MSG_SELECT_YESNO:
				case MSG_SELECT_EFFECTYN: {
					if(highlighting_card)
						highlighting_card->is_highlighting = false;
					highlighting_card = 0;
					DuelClient::SetResponseI(0);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				case MSG_SELECT_CHAIN: {
					DuelClient::SetResponseI(-1);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				case MSG_SELECT_CARD:
				case MSG_SELECT_TRIBUTE:
				case MSG_SELECT_SUM: {
					SetResponseSelectedCards();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				default: {
					mainGame->HideElement(mainGame->wQuery);
					break;
				}
				}
				break;
			}
			case BUTTON_POS_AU: {
				DuelClient::SetResponseI(POS_FACEUP_ATTACK);
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_AD: {
				DuelClient::SetResponseI(POS_FACEDOWN_ATTACK);
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_DU: {
				DuelClient::SetResponseI(POS_FACEUP_DEFENSE);
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_DD: {
				DuelClient::SetResponseI(POS_FACEDOWN_DEFENSE);
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_OPTION_PREV: {
				selected_option--;
				mainGame->btnOptionn->setVisible(true);
				if(selected_option == 0)
					mainGame->btnOptionp->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_NEXT: {
				selected_option++;
				mainGame->btnOptionp->setVisible(true);
				if(selected_option == select_options.size() - 1)
					mainGame->btnOptionn->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_OK: {
				if (mainGame->dInfo.curMsg == MSG_SELECT_OPTION) {
					DuelClient::SetResponseI(selected_option);
				} else if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					DuelClient::SetResponseI((index << 16) + 5);
				} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					DuelClient::SetResponseI(index << 16);
				} else {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					DuelClient::SetResponseI(index);
				}
				mainGame->HideElement(mainGame->wOptions, true);
				break;
			}
			case BUTTON_ANNUMBER_OK: {
				DuelClient::SetResponseI(mainGame->cbANNumber->getSelected());
				mainGame->HideElement(mainGame->wANNumber, true);
				break;
			}
			case BUTTON_ANCARD_OK: {
				int sel = mainGame->lstANCard->getSelected();
				if(sel == -1)
					break;
				DuelClient::SetResponseI(ancard[sel]);
				mainGame->HideElement(mainGame->wANCard, true);
				break;
			}
			case BUTTON_CMD_SHUFFLE: {
				mainGame->btnShuffle->setVisible(false);
				DuelClient::SetResponseI(8);
				DuelClient::SendResponse();
				break;
			}
			case BUTTON_CMD_ACTIVATE: {
				mainGame->wCmdMenu->setVisible(false);
				if(!list_command) {
					int index = -1;
					select_options.clear();
					for (size_t i = 0; i < activatable_cards.size(); ++i) {
						if (activatable_cards[i] == clicked_card) {
							select_options.push_back(activatable_descs[i]);
							if (index == -1) index = i;
						}
					}
					if (select_options.size() == 1) {
						if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
							DuelClient::SetResponseI((index << 16) + 5);
						} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
							DuelClient::SetResponseI(index << 16);
						} else {
							DuelClient::SetResponseI(index);
						}
						DuelClient::SendResponse();
					} else {
						mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)dataManager.GetDesc(select_options[0]));
						selected_option = 0;
						command_card = clicked_card;
						mainGame->btnOptionp->setVisible(false);
						mainGame->btnOptionn->setVisible(true);
						mainGame->ShowElement(mainGame->wOptions);
					}
				} else {
					selectable_cards.clear();
					switch(command_location) {
					case LOCATION_DECK: {
						for(size_t i = 0; i < deck[command_controler].size(); ++i)
							if(deck[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(deck[command_controler][i]);
						break;
					}
					case LOCATION_GRAVE: {
						for(size_t i = 0; i < grave[command_controler].size(); ++i)
							if(grave[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(grave[command_controler][i]);
						break;
					}
					case LOCATION_REMOVED: {
						for(size_t i = 0; i < remove[command_controler].size(); ++i)
							if(remove[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(remove[command_controler][i]);
						selectable_cards.reserve(selectable_cards.size() + conti_cards.size());
						selectable_cards.insert(selectable_cards.end(), conti_cards.begin(), conti_cards.end());
						break;
					}
					case LOCATION_EXTRA: {
						for(size_t i = 0; i < extra[command_controler].size(); ++i)
							if(extra[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(extra[command_controler][i]);
						break;
					}
					}
					mainGame->wCardSelect->setText(dataManager.GetSysString(566));
					list_command = COMMAND_ACTIVATE;
					ShowSelectCard(true, true);
				}
				break;
			}
			case BUTTON_CMD_SUMMON: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(size_t i = 0; i < summonable_cards.size(); ++i) {
					if(summonable_cards[i] == clicked_card) {
						ClearCommandFlag();
						DuelClient::SetResponseI(i << 16);
						DuelClient::SendResponse();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_SPSUMMON: {
				mainGame->wCmdMenu->setVisible(false);
				if(!list_command) {
					if(!clicked_card)
						break;
					for(size_t i = 0; i < spsummonable_cards.size(); ++i) {
						if(spsummonable_cards[i] == clicked_card) {
							ClearCommandFlag();
							DuelClient::SetResponseI((i << 16) + 1);
							DuelClient::SendResponse();
							break;
						}
					}
				} else {
					selectable_cards.clear();
					switch(command_location) {
					case LOCATION_DECK: {
						for(size_t i = 0; i < deck[command_controler].size(); ++i)
							if(deck[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(deck[command_controler][i]);
						break;
					}
					case LOCATION_GRAVE: {
						for(size_t i = 0; i < grave[command_controler].size(); ++i)
							if(grave[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(grave[command_controler][i]);
						break;
					}
					case LOCATION_EXTRA: {
						for(size_t i = 0; i < extra[command_controler].size(); ++i)
							if(extra[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(extra[command_controler][i]);
						break;
					}
					}
					list_command = COMMAND_SPSUMMON;
					mainGame->wCardSelect->setText(dataManager.GetSysString(509));
					ShowSelectCard();
				}
				break;
			}
			case BUTTON_CMD_MSET: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(size_t i = 0; i < msetable_cards.size(); ++i) {
					if(msetable_cards[i] == clicked_card) {
						DuelClient::SetResponseI((i << 16) + 3);
						DuelClient::SendResponse();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_SSET: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(size_t i = 0; i < ssetable_cards.size(); ++i) {
					if(ssetable_cards[i] == clicked_card) {
						DuelClient::SetResponseI((i << 16) + 4);
						DuelClient::SendResponse();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_REPOS: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(size_t i = 0; i < reposable_cards.size(); ++i) {
					if(reposable_cards[i] == clicked_card) {
						DuelClient::SetResponseI((i << 16) + 2);
						DuelClient::SendResponse();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_ATTACK: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(size_t i = 0; i < attackable_cards.size(); ++i) {
					if(attackable_cards[i] == clicked_card) {
						DuelClient::SetResponseI((i << 16) + 1);
						DuelClient::SendResponse();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_SHOWLIST: {
				mainGame->wCmdMenu->setVisible(false);
				selectable_cards.clear();
				switch(command_location) {
				case LOCATION_DECK: {
					for(int32 i = (int32)deck[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(deck[command_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1000), deck[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_MZONE: {
					ClientCard* pcard = mzone[command_controler][command_sequence];
					for(int32 i = 0; i < (int32)pcard->overlayed.size(); ++i)
						selectable_cards.push_back(pcard->overlayed[i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1007), pcard->overlayed.size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_GRAVE: {
					for(int32 i = (int32)grave[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[command_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1004), grave[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_REMOVED: {
					for(int32 i = (int32)remove[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(remove[command_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1005), remove[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_EXTRA: {
					for(int32 i = (int32)extra[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(extra[command_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1006), extra[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				}
				list_command = COMMAND_LIST;
				ShowSelectCard(true);
				break;
			}
			case BUTTON_BP: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
					DuelClient::SetResponseI(6);
					DuelClient::SendResponse();
				}
				break;
			}
			case BUTTON_M2: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					DuelClient::SetResponseI(2);
					DuelClient::SendResponse();
				}
				break;
			}
			case BUTTON_EP: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					DuelClient::SetResponseI(3);
					DuelClient::SendResponse();
				} else if(mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
					DuelClient::SetResponseI(7);
					DuelClient::SendResponse();
				}
				break;
			}
			case BUTTON_CARD_0:
			case BUTTON_CARD_1:
			case BUTTON_CARD_2:
			case BUTTON_CARD_3:
			case BUTTON_CARD_4: {
				if(mainGame->dInfo.isReplay)
					break;
				switch(mainGame->dInfo.curMsg) {
				case MSG_SELECT_IDLECMD:
				case MSG_SELECT_BATTLECMD:
				case MSG_SELECT_CHAIN: {
					if(list_command == COMMAND_LIST)
						break;
					if(list_command == COMMAND_SPSUMMON) {
						command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
						int index = 0;
						while(spsummonable_cards[index] != command_card) index++;
						DuelClient::SetResponseI((index << 16) + 1);
						mainGame->HideElement(mainGame->wCardSelect, true);
						break;
					}
					if(list_command == COMMAND_ACTIVATE) {
						int index = -1;
						command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
						select_options.clear();
						for (size_t i = 0; i < activatable_cards.size(); ++i) {
							if (activatable_cards[i] == command_card) {
								select_options.push_back(activatable_descs[i]);
								if (index == -1) index = i;
							}
						}
						if (select_options.size() == 1) {
							if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
								DuelClient::SetResponseI((index << 16) + 5);
							} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
								DuelClient::SetResponseI(index << 16);
							} else {
								DuelClient::SetResponseI(index);
							}
							mainGame->HideElement(mainGame->wCardSelect, true);
						} else {
							mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)dataManager.GetDesc(select_options[0]));
							selected_option = 0;
							mainGame->btnOptionp->setVisible(false);
							mainGame->btnOptionn->setVisible(true);
							mainGame->wCardSelect->setVisible(false);
							mainGame->ShowElement(mainGame->wOptions);
						}
						break;
					}
					break;
				}
				case MSG_SELECT_CARD: {
					command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
					if (command_card->is_selected) {
						command_card->is_selected = false;
						int i = 0;
						while(selected_cards[i] != command_card) i++;
						selected_cards.erase(selected_cards.begin() + i);
						if(command_card->controler)
							mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffffffff);
					} else {
						command_card->is_selected = true;
						mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffffff00);
						selected_cards.push_back(command_card);
					}
					int sel = selected_cards.size();
					if (sel >= select_max) {
						SetResponseSelectedCards();
						mainGame->HideElement(mainGame->wCardSelect, true);
					} else if (sel >= select_min) {
						select_ready = true;
						mainGame->btnSelectOK->setVisible(true);
					} else {
						select_ready = false;
						mainGame->btnSelectOK->setVisible(false);
					}
					break;
				}
				case MSG_SELECT_SUM: {
					command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
					selected_cards.push_back(command_card);
					ShowSelectSum(true);
					break;
				}
				case MSG_SORT_CHAIN:
				case MSG_SORT_CARD: {
					int offset = mainGame->scrCardList->getPos() / 10;
					int sel_seq = id - BUTTON_CARD_0 + offset;
					if(sort_list[sel_seq]) {
						select_min--;
						int sel = sort_list[sel_seq];
						sort_list[sel_seq] = 0;
						for(int i = 0; i < select_max; ++i)
							if(sort_list[i] > sel)
								sort_list[i]--;
						for(int i = 0; i < 5; ++i) {
							if(offset + i >= select_max)
								break;
							if(sort_list[offset + i]) {
								myswprintf(formatBuffer, L"%d", sort_list[offset + i]);
								mainGame->stCardPos[i]->setText(formatBuffer);
							} else mainGame->stCardPos[i]->setText(L"");
						}
					} else {
						select_min++;
						sort_list[sel_seq] = select_min;
						myswprintf(formatBuffer, L"%d", select_min);
						mainGame->stCardPos[id - BUTTON_CARD_0]->setText(formatBuffer);
						if(select_min == select_max) {
							unsigned char respbuf[64];
							for(int i = 0; i < select_max; ++i)
								respbuf[i] = sort_list[i] - 1;
							DuelClient::SetResponseB(respbuf, select_max);
							mainGame->HideElement(mainGame->wCardSelect, true);
							sort_list.clear();
						}
					}
					break;
				}
				}
				break;
			}
			case BUTTON_CARD_SEL_OK: {
				if(mainGame->dInfo.isReplay) {
					mainGame->HideElement(mainGame->wCardSelect);
					break;
				}
				if(mainGame->dInfo.curMsg == MSG_SELECT_CARD || mainGame->dInfo.curMsg == MSG_SELECT_SUM) {
					if(select_ready) {
						SetResponseSelectedCards();
						mainGame->HideElement(mainGame->wCardSelect, true);
					}
					break;
				} else if(mainGame->dInfo.curMsg == MSG_CONFIRM_CARDS) {
					mainGame->HideElement(mainGame->wCardSelect);
					mainGame->actionSignal.Set();
					break;
				} else {
					mainGame->HideElement(mainGame->wCardSelect);
					break;
				}
				break;
			}
			case BUTTON_CARD_DISP_OK: {
				mainGame->HideElement(mainGame->wCardDisplay);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch(id) {
			case CHECK_ATTRIBUTE: {
				int att = 0, filter = 0x1, count = 0;
				for(int i = 0; i < 7; ++i, filter <<= 1) {
					if(mainGame->chkAttribute[i]->isChecked()) {
						att |= filter;
						count++;
					}
				}
				if(count == announce_count) {
					DuelClient::SetResponseI(att);
					mainGame->HideElement(mainGame->wANAttribute, true);
				}
				break;
			}
			case CHECK_RACE: {
				int rac = 0, filter = 0x1, count = 0;
				for(int i = 0; i < 24; ++i, filter <<= 1) {
					if(mainGame->chkRace[i]->isChecked()) {
						rac |= filter;
						count++;
					}
				}
				if(count == announce_count) {
					DuelClient::SetResponseI(rac);
					mainGame->HideElement(mainGame->wANRace, true);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_CHANGED: {
			switch(id) {
			case LISTBOX_LOG: {
				int sel = mainGame->lstLog->getSelected();
				if(sel != -1 && (int)mainGame->logParam.size() >= sel && mainGame->logParam[sel]) {
					mainGame->ShowCardInfo(mainGame->logParam[sel]);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_SELECTED_AGAIN: {
			switch(id) {
			case LISTBOX_LOG: {
				int sel = mainGame->lstLog->getSelected();
				if(sel != -1 && (int)mainGame->logParam.size() >= sel && mainGame->logParam[sel]) {
					mainGame->wInfos->setActiveTab(0);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
			case SCROLL_CARD_SELECT: {
				int pos = mainGame->scrCardList->getPos() / 10;
				for(int i = 0; i < 5; ++i) {
					mainGame->stCardPos[i]->enableOverrideColor(false);
					if(selectable_cards[i + pos]->code)
						mainGame->btnCardSelect[i]->setImage(imageManager.GetTexture(selectable_cards[i + pos]->code));
					else
						mainGame->btnCardSelect[i]->setImage(imageManager.tCover[0]);
					mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					if(sort_list.size()) {
						if(sort_list[pos + i] > 0)
							myswprintf(formatBuffer, L"%d", sort_list[pos + i]);
						else
							myswprintf(formatBuffer, L"");
					} else {
						if(selectable_cards[i + pos]->is_conti && !selectable_cards[i + pos]->code)
							myswprintf(formatBuffer, L"%ls", DataManager::unknown_string);
						else if(selectable_cards[i + pos]->location == LOCATION_OVERLAY)
							myswprintf(formatBuffer, L"%ls[%d](%d)",
								dataManager.FormatLocation(selectable_cards[i + pos]->overlayTarget->location, selectable_cards[i + pos]->overlayTarget->sequence),
								selectable_cards[i + pos]->overlayTarget->sequence + 1, selectable_cards[i + pos]->sequence + 1);
						else
							myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(selectable_cards[i + pos]->location, selectable_cards[i + pos]->sequence),
								selectable_cards[i + pos]->sequence + 1);
					}
					mainGame->stCardPos[i]->setText(formatBuffer);
					if(selectable_cards[i + pos]->location == LOCATION_OVERLAY) {
						if(selectable_cards[i + pos]->owner != selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setOverrideColor(0xff0000ff);
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
						else if(selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					} else {
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
						else if(selectable_cards[i + pos]->controler)
							mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					}
				}
				break;
			}
			case SCROLL_CARD_DISPLAY: {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				for(int i = 0; i < 5; ++i) {
					mainGame->stDisplayPos[i]->enableOverrideColor(false);
					if(display_cards[i + pos]->code)
						mainGame->btnCardDisplay[i]->setImage(imageManager.GetTexture(display_cards[i + pos]->code));
					else
						mainGame->btnCardDisplay[i]->setImage(imageManager.tCover[0]);
					mainGame->btnCardDisplay[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					if(display_cards[i + pos]->location == LOCATION_OVERLAY) {
							myswprintf(formatBuffer, L"%ls[%d](%d)",
								dataManager.FormatLocation(display_cards[i + pos]->overlayTarget->location, display_cards[i + pos]->overlayTarget->sequence),
								display_cards[i + pos]->overlayTarget->sequence + 1, display_cards[i + pos]->sequence + 1);
					} else
						myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(display_cards[i + pos]->location, display_cards[i + pos]->sequence),
							display_cards[i + pos]->sequence + 1);
					mainGame->stDisplayPos[i]->setText(formatBuffer);
					if(display_cards[i + pos]->location == LOCATION_OVERLAY) {
						if(display_cards[i + pos]->owner != display_cards[i + pos]->overlayTarget->controler)
							mainGame->stDisplayPos[i]->setOverrideColor(0xff0000ff);
						if(display_cards[i + pos]->overlayTarget->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stDisplayPos[i]->setBackgroundColor(0xffffffff);
					} else {
						if(display_cards[i + pos]->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stDisplayPos[i]->setBackgroundColor(0xffffffff);
					}
				}
				break;
			}
			case SCROLL_CARDTEXT: {
				u32 pos = mainGame->scrCardText->getPos();
				mainGame->SetStaticText(mainGame->stText, mainGame->stText->getRelativePosition().getWidth()-25, mainGame->textFont, mainGame->showingtext, pos);
				break;
			}
			break;
			}
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch(id) {
			case EDITBOX_ANCARD: {
				UpdateDeclarableCode(false);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_ANCARD: {
				UpdateDeclarableCode(true);
				break;
			}
			case EDITBOX_CHAT: {
				if(mainGame->dInfo.isReplay)
					break;
				const wchar_t* input = mainGame->ebChatInput->getText();
				if(input[0]) {
					unsigned short msgbuf[256];
					if(mainGame->dInfo.isStarted) {
						if(mainGame->dInfo.player_type < 7) {
							if(mainGame->dInfo.isTag && (mainGame->dInfo.player_type % 2))
								mainGame->AddChatMsg((wchar_t*)input, 2);
							else
								mainGame->AddChatMsg((wchar_t*)input, 0);
						} else
							mainGame->AddChatMsg((wchar_t*)input, 10);
					} else
						mainGame->AddChatMsg((wchar_t*)input, 7);
					int len = BufferIO::CopyWStr(input, msgbuf, 256);
					DuelClient::SendBufferToServer(CTOS_CHAT, msgbuf, (len + 1) * sizeof(short));
					mainGame->ebChatInput->setText(L"");
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_HOVERED: {
			if(id >= BUTTON_CARD_0 && id <= BUTTON_CARD_4) {
				int pos = mainGame->scrCardList->getPos() / 10;
				ClientCard* mcard = selectable_cards[id - BUTTON_CARD_0 + pos];
				if(mcard->code) {
					mainGame->ShowCardInfo(mcard->code);
				} else {
					mainGame->imgCard->setImage(imageManager.tCover[0]);
					mainGame->stName->setText(L"");
					mainGame->stInfo->setText(L"");
					mainGame->stDataInfo->setText(L"");
					mainGame->stSetName->setText(L"");
					mainGame->stText->setText(L"");
					mainGame->scrCardText->setVisible(false);
				}
			}
			if(id >= BUTTON_DISPLAY_0 && id <= BUTTON_DISPLAY_4) {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				ClientCard* mcard = display_cards[id - BUTTON_DISPLAY_0 + pos];
				if(mcard->code) {
					mainGame->ShowCardInfo(mcard->code);
				} else {
					mainGame->imgCard->setImage(imageManager.tCover[0]);
					mainGame->stName->setText(L"");
					mainGame->stInfo->setText(L"");
					mainGame->stDataInfo->setText(L"");
					mainGame->stSetName->setText(L"");
					mainGame->stText->setText(L"");
					mainGame->scrCardText->setVisible(false);
				}
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case irr::EET_MOUSE_INPUT_EVENT: {
		switch(event.MouseInput.Event) {
		case irr::EMIE_LMOUSE_LEFT_UP: {
			if(!mainGame->dInfo.isStarted)
				break;
			s32 x = event.MouseInput.X;
			s32 y = event.MouseInput.Y;
			hovered_location = 0;
			irr::core::position2di pos(x, y);
			if(x < 300)
				break;
			if(mainGame->gameConf.control_mode == 1)
				mainGame->always_chain = event.MouseInput.isLeftPressed();
			if(mainGame->wCmdMenu->isVisible() && !mainGame->wCmdMenu->getRelativePosition().isPointInside(pos))
				mainGame->wCmdMenu->setVisible(false);
			if(panel && panel->isVisible())
				break;
			GetHoverField(x, y);
			if(hovered_location & 0xe)
				clicked_card = GetCard(hovered_controler, hovered_location, hovered_sequence);
			else clicked_card = 0;
			if(mainGame->dInfo.isReplay) {
				if(mainGame->wCardSelect->isVisible())
					break;
				selectable_cards.clear();
				switch(hovered_location) {
				case LOCATION_DECK: {
					if(deck[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)deck[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(deck[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1000), deck[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_MZONE: {
					if(!clicked_card || clicked_card->overlayed.size() == 0)
						break;
					for(int32 i = 0; i < (int32)clicked_card->overlayed.size(); ++i)
						selectable_cards.push_back(clicked_card->overlayed[i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1007), clicked_card->overlayed.size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_GRAVE: {
					if(grave[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)grave[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1004), grave[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_REMOVED: {
					if(remove[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)remove[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(remove[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1005), remove[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_EXTRA: {
					if(extra[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)extra[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(extra[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1006), extra[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				}
				if(selectable_cards.size())
					ShowSelectCard(true);
				break;
			}
			if(mainGame->dInfo.player_type == 7) {
				if(mainGame->wCardSelect->isVisible())
					break;
				selectable_cards.clear();
				switch(hovered_location) {
				case LOCATION_MZONE: {
					if(!clicked_card || clicked_card->overlayed.size() == 0)
						break;
					for(int32 i = 0; i < (int32)clicked_card->overlayed.size(); ++i)
						selectable_cards.push_back(clicked_card->overlayed[i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1007), clicked_card->overlayed.size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_GRAVE: {
					if(grave[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)grave[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1004), grave[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				}
				if(selectable_cards.size())
					ShowSelectCard(true);
				break;
			}
			command_controler = hovered_controler;
			command_location = hovered_location;
			command_sequence = hovered_sequence;
			switch(mainGame->dInfo.curMsg) {
			case MSG_WAITING: {
				switch(hovered_location) {
				case LOCATION_MZONE:
				case LOCATION_SZONE: {
					if(!clicked_card || clicked_card->overlayed.size() == 0)
						break;
					ShowMenu(COMMAND_LIST, x, y);
					break;
				}
				case LOCATION_GRAVE: {
					if(grave[hovered_controler].size() == 0)
						break;
					ShowMenu(COMMAND_LIST, x, y);
					break;
				}
				case LOCATION_REMOVED: {
					if(remove[hovered_controler].size() == 0)
						break;
					ShowMenu(COMMAND_LIST, x, y);
					break;
				}
				case LOCATION_EXTRA: {
					if(extra[hovered_controler].size() == 0)
						break;
					ShowMenu(COMMAND_LIST, x, y);
					break;
				}
				}
				break;
			}
			case MSG_SELECT_BATTLECMD:
			case MSG_SELECT_IDLECMD:
			case MSG_SELECT_CHAIN: {
				switch(hovered_location) {
				case LOCATION_DECK: {
					int command_flag = 0;
					if(deck[hovered_controler].size() == 0)
						break;
					for(size_t i = 0; i < deck[hovered_controler].size(); ++i)
						command_flag |= deck[hovered_controler][i]->cmdFlag;
					if(mainGame->dInfo.isSingleMode)
						command_flag |= COMMAND_LIST;
					list_command = 1;
					ShowMenu(command_flag, x, y);
					break;
				}
				case LOCATION_HAND:
				case LOCATION_MZONE:
				case LOCATION_SZONE: {
					if(!clicked_card)
						break;
					int command_flag = clicked_card->cmdFlag;
					if(clicked_card->overlayed.size())
						command_flag |= COMMAND_LIST;
					list_command = 0;
					ShowMenu(command_flag, x, y);
					break;
				}
				case LOCATION_GRAVE: {
					int command_flag = 0;
					if(grave[hovered_controler].size() == 0)
						break;
					for(size_t i = 0; i < grave[hovered_controler].size(); ++i)
						command_flag |= grave[hovered_controler][i]->cmdFlag;
					command_flag |= COMMAND_LIST;
					list_command = 1;
					ShowMenu(command_flag, x, y);
					break;
				}
				case LOCATION_REMOVED: {
					int command_flag = 0;
					if(remove[hovered_controler].size() == 0 && conti_cards.size() == 0)
						break;
					for(size_t i = 0; i < remove[hovered_controler].size(); ++i)
						command_flag |= remove[hovered_controler][i]->cmdFlag;
					if(conti_cards.size())
						command_flag |= COMMAND_ACTIVATE;
					if(remove[hovered_controler].size())
						command_flag |= COMMAND_LIST;
					list_command = 1;
					ShowMenu(command_flag, x, y);
					break;
				}
				case LOCATION_EXTRA: {
					int command_flag = 0;
					if(extra[hovered_controler].size() == 0)
						break;
					for(size_t i = 0; i < extra[hovered_controler].size(); ++i)
						command_flag |= extra[hovered_controler][i]->cmdFlag;
					command_flag |= COMMAND_LIST;
					list_command = 1;
					ShowMenu(command_flag, x, y);
					break;
				}
				}
				break;
			}
			case MSG_SELECT_PLACE:
			case MSG_SELECT_DISFIELD: {
				if (!(hovered_location & LOCATION_ONFIELD) || hovered_sequence == 5)
					break;
				int flag = 1 << (hovered_sequence + (hovered_controler << 4) + ((hovered_location == LOCATION_MZONE) ? 0 : 8));
				if ((flag & selectable_field) > 0) {
					if ((flag & selected_field) > 0) {
						selected_field &= ~flag;
						select_min++;
					} else {
						selected_field |= flag;
						select_min--;
						if (select_min == 0) {
							unsigned char respbuf[80];
							int filter = 1;
							int p = 0;
							for (int i = 0; i < 5; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(0);
									respbuf[p + 1] = 0x4;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x100;
							for (int i = 0; i < 8; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(0);
									respbuf[p + 1] = 0x8;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x10000;
							for (int i = 0; i < 5; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(1);
									respbuf[p + 1] = 0x4;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x1000000;
							for (int i = 0; i < 8; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(1);
									respbuf[p + 1] = 0x8;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							selectable_field = 0;
							selected_field = 0;
							DuelClient::SetResponseB(respbuf, p);
							DuelClient::SendResponse();
						}
					}
				}
				break;
			}
			case MSG_SELECT_CARD:
			case MSG_SELECT_TRIBUTE: {
				if (!(hovered_location & 0xe) || !clicked_card || !clicked_card->is_selectable)
					break;
				if (clicked_card->is_selected) {
					clicked_card->is_selected = false;
					int i = 0;
					while(selected_cards[i] != clicked_card) i++;
					selected_cards.erase(selected_cards.begin() + i);
				} else {
					clicked_card->is_selected = true;
					selected_cards.push_back(clicked_card);
				}
				int min = selected_cards.size(), max = 0;
				if (mainGame->dInfo.curMsg == MSG_SELECT_CARD) {
					max = selected_cards.size();
				} else {
					for(size_t i = 0; i < selected_cards.size(); ++i)
						max += selected_cards[i]->opParam;
				}
				if (min >= select_max) {
					SetResponseSelectedCards();
					DuelClient::SendResponse();
				} else if (max >= select_min) {
					if(selected_cards.size() == selectable_cards.size()) {
						SetResponseSelectedCards();
						DuelClient::SendResponse();
					} else {
						select_ready = true;
						if(mainGame->dInfo.curMsg == MSG_SELECT_TRIBUTE) {
							wchar_t wbuf[256], *pwbuf = wbuf;
							BufferIO::CopyWStrRef(dataManager.GetSysString(209), pwbuf, 256);
							*pwbuf++ = L'\n';
							BufferIO::CopyWStrRef(dataManager.GetSysString(210), pwbuf, 256);
							mainGame->stQMessage->setText(wbuf);
							mainGame->PopupElement(mainGame->wQuery);
						}
					}
				} else {
					select_ready = false;
				}
				break;
			}
			case MSG_SELECT_COUNTER: {
				if (!clicked_card || !clicked_card->is_selectable)
					break;
				clicked_card->opParam--;
				if ((clicked_card->opParam & 0xffff) == 0)
					clicked_card->is_selectable = false;
				select_counter_count--;
				if (select_counter_count == 0) {
					unsigned char respbuf[64];
					for(size_t i = 0; i < selectable_cards.size(); ++i)
						respbuf[i] = (selectable_cards[i]->opParam >> 16) - (selectable_cards[i]->opParam & 0xffff);
					mainGame->stHintMsg->setVisible(false);
					ClearSelect();
					DuelClient::SetResponseB(respbuf, selectable_cards.size());
					DuelClient::SendResponse();
				} else {
					myswprintf(formatBuffer, dataManager.GetSysString(204), select_counter_count, dataManager.GetCounterName(select_counter_type));
					mainGame->stHintMsg->setText(formatBuffer);
				}
				break;
			}
			case MSG_SELECT_SUM: {
				if (!clicked_card || !clicked_card->is_selectable)
					break;
				if (clicked_card->is_selected) {
					auto it = std::find(selected_cards.begin(), selected_cards.end(), clicked_card);
					selected_cards.erase(it);
				} else
					selected_cards.push_back(clicked_card);
				ShowSelectSum(false);
				break;
			}
			}
			break;
		}
		case irr::EMIE_RMOUSE_LEFT_UP: {
			if(mainGame->dInfo.isReplay)
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300)
				mainGame->ignore_chain = event.MouseInput.isRightPressed();
			mainGame->wCmdMenu->setVisible(false);
			if(mainGame->fadingList.size())
				break;
			switch(mainGame->dInfo.curMsg) {
			case MSG_WAITING: {
				if(mainGame->wCardSelect->isVisible()) {
					mainGame->HideElement(mainGame->wCardSelect);
				}
				break;
			}
			case MSG_SELECT_BATTLECMD: {
				if(mainGame->wCardSelect->isVisible()) {
					mainGame->HideElement(mainGame->wCardSelect);
				}
				if(mainGame->wOptions->isVisible()) {
					mainGame->HideElement(mainGame->wOptions);
				}
				break;
			}
			case MSG_SELECT_IDLECMD: {
				if(mainGame->wCardSelect->isVisible()) {
					mainGame->HideElement(mainGame->wCardSelect);
				}
				if(mainGame->wOptions->isVisible()) {
					mainGame->HideElement(mainGame->wOptions);
				}
				break;
			}
			case MSG_SELECT_YESNO:
			case MSG_SELECT_EFFECTYN: {
				if(highlighting_card)
					highlighting_card->is_highlighting = false;
				highlighting_card = 0;
				DuelClient::SetResponseI(0);
				mainGame->HideElement(mainGame->wQuery, true);
				break;
			}
			case MSG_SELECT_CARD: {
				if(selected_cards.size() == 0) {
					if(select_cancelable) {
						DuelClient::SetResponseI(-1);
						if(mainGame->wCardSelect->isVisible())
							mainGame->HideElement(mainGame->wCardSelect, true);
						else
							DuelClient::SendResponse();
					}
					break;
				}
				if(mainGame->wQuery->isVisible()) {
					SetResponseSelectedCards();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				if(select_ready) {
					SetResponseSelectedCards();
					if(mainGame->wCardSelect->isVisible())
						mainGame->HideElement(mainGame->wCardSelect, true);
					else
						DuelClient::SendResponse();
				}
				break;
			}
			case MSG_SELECT_TRIBUTE: {
				if(selected_cards.size() == 0) {
					if(select_cancelable) {
						DuelClient::SetResponseI(-1);
						if(mainGame->wCardSelect->isVisible())
							mainGame->HideElement(mainGame->wCardSelect, true);
						else
							DuelClient::SendResponse();
					}
					break;
				}
				if(mainGame->wQuery->isVisible()) {
					SetResponseSelectedCards();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				break;
			}
			case MSG_SELECT_SUM: {
				if(mainGame->wQuery->isVisible()) {
					SetResponseSelectedCards();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				break;
			}
			case MSG_SELECT_CHAIN: {
				if(chain_forced)
					break;
				if(mainGame->wCardSelect->isVisible()) {
					mainGame->HideElement(mainGame->wCardSelect);
					break;
				}
				if(mainGame->wQuery->isVisible()) {
					DuelClient::SetResponseI(-1);
					mainGame->HideElement(mainGame->wQuery, true);
				} else {
					mainGame->PopupElement(mainGame->wQuery);
				}
				if(mainGame->wOptions->isVisible()) {
					DuelClient::SetResponseI(-1);
					mainGame->HideElement(mainGame->wOptions);
				}
				break;
			}
			case MSG_SORT_CHAIN:
			case MSG_SORT_CARD: {
				if(mainGame->wCardSelect->isVisible()) {
					DuelClient::SetResponseI(-1);
					mainGame->HideElement(mainGame->wCardSelect, true);
				}
				break;
			}
			}
			break;
		}
		case irr::EMIE_MOUSE_MOVED: {
			if(!mainGame->dInfo.isStarted)
				break;
			s32 x = event.MouseInput.X;
			s32 y = event.MouseInput.Y;
			hovered_location = 0;
			irr::core::position2di pos(x, y);
			if(x < 300)
				break;
			ClientCard* mcard = 0;
			if(!panel || !panel->isVisible() || !panel->getRelativePosition().isPointInside(pos)) {
				GetHoverField(x, y);
				if(hovered_location & 0xe)
					mcard = GetCard(hovered_controler, hovered_location, hovered_sequence);
				else if(hovered_location == LOCATION_GRAVE && grave[hovered_controler].size())
					mcard = *(grave[hovered_controler].rbegin());
				else if(hovered_location == LOCATION_REMOVED && remove[hovered_controler].size()) {
					mcard = *(remove[hovered_controler].rbegin());
					if(mcard->position & POS_FACEDOWN)
						mcard = 0;
				} else if(hovered_location == LOCATION_DECK && deck[hovered_controler].size())
					mcard = *(deck[hovered_controler].rbegin());
				else mcard = 0;
			}
			if(hovered_location == LOCATION_HAND && (mainGame->dInfo.is_shuffling || mainGame->dInfo.curMsg == MSG_SHUFFLE_HAND))
				mcard = 0;
			if(mcard != hovered_card) {
				if(hovered_card) {
					if(hovered_card->location == LOCATION_HAND && !mainGame->dInfo.is_shuffling && mainGame->dInfo.curMsg != MSG_SHUFFLE_HAND) {
						hovered_card->is_hovered = false;
						MoveCard(hovered_card, 5);
						if(hovered_controler == 0)
							mainGame->hideChat = false;
					}
					if(hovered_card->equipTarget)
						hovered_card->equipTarget->is_showequip = false;
					if(hovered_card->equipped.size())
						for(auto cit = hovered_card->equipped.begin(); cit != hovered_card->equipped.end(); ++cit)
							(*cit)->is_showequip = false;
					if(hovered_card->cardTarget.size())
						for(auto cit = hovered_card->cardTarget.begin(); cit != hovered_card->cardTarget.end(); ++cit)
							(*cit)->is_showtarget = false;
					if(hovered_card->ownerTarget.size())
						for(auto cit = hovered_card->ownerTarget.begin(); cit != hovered_card->ownerTarget.end(); ++cit)
							(*cit)->is_showtarget = false;
				}
				if(mcard) {
					if(mcard != clicked_card)
						mainGame->wCmdMenu->setVisible(false);
					if(hovered_location == LOCATION_HAND) {
						mcard->is_hovered = true;
						MoveCard(mcard, 5);
						if(hovered_controler == 0)
							mainGame->hideChat = true;
					}
					if(mcard->equipTarget)
						mcard->equipTarget->is_showequip = true;
					if(mcard->equipped.size())
						for(auto cit = mcard->equipped.begin(); cit != mcard->equipped.end(); ++cit)
							(*cit)->is_showequip = true;
					if(mcard->cardTarget.size())
						for(auto cit = mcard->cardTarget.begin(); cit != mcard->cardTarget.end(); ++cit)
							(*cit)->is_showtarget = true;
					if(mcard->ownerTarget.size())
						for(auto cit = mcard->ownerTarget.begin(); cit != mcard->ownerTarget.end(); ++cit)
							(*cit)->is_showtarget = true;
					if(mcard->code) {
						mainGame->ShowCardInfo(mcard->code);
						if(mcard->location & 0xe) {
							std::wstring str;
							if(mcard->type & TYPE_MONSTER) {
								myswprintf(formatBuffer, L"%ls", dataManager.GetName(mcard->code));
								str.append(formatBuffer);
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)
								        && wcscmp(dataManager.GetName(mcard->code), dataManager.GetName(mcard->alias))) {
									myswprintf(formatBuffer, L"\n(%ls)", dataManager.GetName(mcard->alias));
									str.append(formatBuffer);
								}
								myswprintf(formatBuffer, L"\n%ls/%ls", mcard->atkstring, mcard->defstring);
								str.append(formatBuffer);
								int form = 0x2605;
								if (mcard->rank) ++form;
								myswprintf(formatBuffer, L"\n%c%d %ls/%ls", form, (mcard->level ? mcard->level : mcard->rank), dataManager.FormatRace(mcard->race), dataManager.FormatAttribute(mcard->attribute));
								str.append(formatBuffer);
								if(mcard->location == LOCATION_HAND && (mcard->type & TYPE_PENDULUM)) {
									myswprintf(formatBuffer, L"\n%d/%d", mcard->lscale, mcard->rscale);
									str.append(formatBuffer);
								}
								if(mcard->counters.size()) {
									for(std::map<int, int>::iterator ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
										myswprintf(formatBuffer, L"\n[%ls]: %d", dataManager.GetCounterName(ctit->first), ctit->second);
										str.append(formatBuffer);
									}
								}
								if(mcard->cHint && mcard->chValue && (mcard->location & LOCATION_ONFIELD)) {
									if(mcard->cHint == CHINT_TURN)
										myswprintf(formatBuffer, L"\n%ls%d", dataManager.GetSysString(211), mcard->chValue);
									else if(mcard->cHint == CHINT_CARD)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(212), dataManager.GetName(mcard->chValue));
									else if(mcard->cHint == CHINT_RACE)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(213), dataManager.FormatRace(mcard->chValue));
									else if(mcard->cHint == CHINT_ATTRIBUTE)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(214), dataManager.FormatAttribute(mcard->chValue));
									else if(mcard->cHint == CHINT_NUMBER)
										myswprintf(formatBuffer, L"\n%ls%d", dataManager.GetSysString(215), mcard->chValue);
									str.append(formatBuffer);
								}
								for(auto iter = mcard->desc_hints.begin(); iter != mcard->desc_hints.end(); ++iter) {
									myswprintf(formatBuffer, L"\n*%ls", dataManager.GetDesc(iter->first));
									str.append(formatBuffer);
								}
							} else {
								myswprintf(formatBuffer, L"%ls", dataManager.GetName(mcard->code));
								str.append(formatBuffer);
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)) {
									myswprintf(formatBuffer, L"\n(%ls)", dataManager.GetName(mcard->alias));
									str.append(formatBuffer);
								}
								if(mcard->location == LOCATION_SZONE && (mcard->sequence == 6 || mcard->sequence == 7)) {
									myswprintf(formatBuffer, L"\n%d/%d", mcard->lscale, mcard->rscale);
									str.append(formatBuffer);
								}
								if(mcard->counters.size()) {
									for(std::map<int, int>::iterator ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
										myswprintf(formatBuffer, L"\n[%ls]: %d", dataManager.GetCounterName(ctit->first), ctit->second);
										str.append(formatBuffer);
									}
								}
								if(mcard->cHint && mcard->chValue && (mcard->location & LOCATION_ONFIELD)) {
									if(mcard->cHint == CHINT_TURN)
										myswprintf(formatBuffer, L"\n%ls%d", dataManager.GetSysString(211), mcard->chValue);
									else if(mcard->cHint == CHINT_CARD)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(212), dataManager.GetName(mcard->chValue));
									else if(mcard->cHint == CHINT_RACE)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(213), dataManager.FormatRace(mcard->chValue));
									else if(mcard->cHint == CHINT_ATTRIBUTE)
										myswprintf(formatBuffer, L"\n%ls%ls", dataManager.GetSysString(214), dataManager.FormatAttribute(mcard->chValue));
									else if(mcard->cHint == CHINT_NUMBER)
										myswprintf(formatBuffer, L"\n%ls%d", dataManager.GetSysString(215), mcard->chValue);
									str.append(formatBuffer);
								}
								for(auto iter = mcard->desc_hints.begin(); iter != mcard->desc_hints.end(); ++iter) {
									myswprintf(formatBuffer, L"\n*%ls", dataManager.GetDesc(iter->first));
									str.append(formatBuffer);
								}
							}
							mainGame->stTip->setVisible(true);
							irr::core::dimension2d<unsigned int> dtip = mainGame->textFont->getDimension(str.c_str());
							mainGame->stTip->setRelativePosition(recti(x - 10 - dtip.Width, y - 10 - dtip.Height, x, y));
							mainGame->stTip->setText(str.c_str());
						}
					} else {
						mainGame->stTip->setVisible(false);
						mainGame->imgCard->setImage(imageManager.tCover[0]);
						mainGame->stName->setText(L"");
						mainGame->stInfo->setText(L"");
						mainGame->stDataInfo->setText(L"");
						mainGame->stSetName->setText(L"");
						mainGame->stText->setText(L"");
						mainGame->scrCardText->setVisible(false);
					}
				} else {
					mainGame->stTip->setVisible(false);
				}
				hovered_card = mcard;
			} else {
				if(mainGame->stTip->isVisible()) {
					irr::core::recti tpos = mainGame->stTip->getRelativePosition();
					mainGame->stTip->setRelativePosition(irr::core::position2di(x - tpos.getWidth() - 10, y - tpos.getHeight() - 10));
				}
			}
			break;
		}
		case irr::EMIE_MOUSE_WHEEL: {
			break;
		}
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			if(!mainGame->dInfo.isStarted)
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300)
				mainGame->always_chain = event.MouseInput.isLeftPressed();
			break;
		}
		case irr::EMIE_RMOUSE_PRESSED_DOWN: {
			if(!mainGame->dInfo.isStarted)
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300)
				mainGame->ignore_chain = event.MouseInput.isRightPressed();
			break; 
		}
		default:
			break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_A: {
			if(mainGame->gameConf.control_mode == 0)
				mainGame->always_chain = event.KeyInput.PressedDown;
			break;
		}
		case irr::KEY_KEY_S: {
			if(mainGame->gameConf.control_mode == 0)
				mainGame->ignore_chain = event.KeyInput.PressedDown;
			break;
		}
		case irr::KEY_KEY_R: {
			if(mainGame->gameConf.control_mode == 0
				&& !event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_F1:
		case irr::KEY_F2:
		case irr::KEY_F3:
		case irr::KEY_F4:
		case irr::KEY_F5:
		case irr::KEY_F6:
		case irr::KEY_F7:
		case irr::KEY_F8: {
			if(!event.KeyInput.PressedDown && !mainGame->dInfo.isReplay && mainGame->dInfo.player_type != 7 && mainGame->dInfo.isStarted
					&& !mainGame->wCardDisplay->isVisible() && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				int loc_id = 0;
				display_cards.clear();
				switch(event.KeyInput.Key) {
					case irr::KEY_F1:
						loc_id = 1004;
						for(int32 i = (int32)grave[0].size() - 1; i >= 0 ; --i)
							display_cards.push_back(grave[0][i]);
						break;
					case irr::KEY_F2:
						loc_id = 1005;
						for(int32 i = (int32)remove[0].size() - 1; i >= 0 ; --i)
							display_cards.push_back(remove[0][i]);
						break;
					case irr::KEY_F3:
						loc_id = 1006;
						for(int32 i = (int32)extra[0].size() - 1; i >= 0 ; --i)
							display_cards.push_back(extra[0][i]);
						break;
					case irr::KEY_F4:
						loc_id = 1007;
						for(int32 i = 0; i <= 4 ; ++i) {
							if(mzone[0][i] && mzone[0][i]->overlayed.size()) {
								for(int32 j = 0; j <= (int32)mzone[0][i]->overlayed.size() - 1 ; ++j)
									display_cards.push_back(mzone[0][i]->overlayed[j]);
							}
						}
						break;
					case irr::KEY_F5:
						loc_id = 1004;
						for(int32 i = (int32)grave[1].size() - 1; i >= 0 ; --i)
							display_cards.push_back(grave[1][i]);
						break;
					case irr::KEY_F6:
						loc_id = 1005;
						for(int32 i = (int32)remove[1].size() - 1; i >= 0 ; --i)
							display_cards.push_back(remove[1][i]);
						break;
					case irr::KEY_F7:
						loc_id = 1006;
						for(int32 i = (int32)extra[1].size() - 1; i >= 0 ; --i)
							display_cards.push_back(extra[1][i]);
						break;
					case irr::KEY_F8:
						loc_id = 1007;
						for(int32 i = 0; i <= 4 ; ++i) {
							if(mzone[1][i] && mzone[1][i]->overlayed.size()) {
								for(int32 j = 0; j <= (int32)mzone[1][i]->overlayed.size() - 1 ; ++j)
									display_cards.push_back(mzone[1][i]->overlayed[j]);
							}
						}
						break;
				}
				if(display_cards.size()) {
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(loc_id), display_cards.size());
					mainGame->wCardDisplay->setText(formatBuffer);
					ShowLocationCard();
				}
			}
			break;
		}
		case irr::KEY_F9: {
			if(mainGame->gameConf.control_mode == 1
				&& !event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			if(!mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->device->minimizeWindow();
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
	return false;
}
void ClientField::GetHoverField(int x, int y) {
	irr::core::recti sfRect(430, 504, 875, 600);
	irr::core::recti ofRect(531, 135, 800, 191);
	irr::core::position2di pos(x, y);
	if(sfRect.isPointInside(pos)) {
		int hc = hand[0].size();
		int cardSize = 66;
		int cardSpace = 10;
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7) {
			int left = sfRect.UpperLeftCorner.X + (cardSize + cardSpace) * (6 - hc) / 2;
			if(x < left)
				hovered_location = 0;
			else {
				int seq = (x - left) / (cardSize + cardSpace);
				if(seq >= hc) seq = hc - 1;
				if(x - left - (cardSize + cardSpace) * seq < cardSize) {
					hovered_controler = 0;
					hovered_location = LOCATION_HAND;
					hovered_sequence = seq;
				} else hovered_location = 0;
			}
		} else {
			hovered_controler = 0;
			hovered_location = LOCATION_HAND;
			if(x >= sfRect.UpperLeftCorner.X + (cardSize + cardSpace) * 5)
				hovered_sequence = hc - 1;
			else
				hovered_sequence = (x - sfRect.UpperLeftCorner.X) * (hc - 1) / ((cardSize + cardSpace) * 5);
		}
	} else if(ofRect.isPointInside(pos)) {
		int hc = hand[1].size();
		int cardSize = 39;
		int cardSpace = 7;
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7) {
			int left = ofRect.UpperLeftCorner.X + (cardSize + cardSpace) * (6 - hc) / 2;
			if(x < left)
				hovered_location = 0;
			else {
				int seq = (x - left) / (cardSize + cardSpace);
				if(seq >= hc) seq = hc - 1;
				if(x - left - (cardSize + cardSpace) * seq < cardSize) {
					hovered_controler = 1;
					hovered_location = LOCATION_HAND;
					hovered_sequence = hc - 1 - seq;
				} else hovered_location = 0;
			}
		} else {
			hovered_controler = 1;
			hovered_location = LOCATION_HAND;
			if(x >= ofRect.UpperLeftCorner.X + (cardSize + cardSpace) * 5)
				hovered_sequence = 0;
			else
				hovered_sequence = hc - 1 - (x - ofRect.UpperLeftCorner.X) * (hc - 1) / ((cardSize + cardSpace) * 5);
		}
	} else {
		double screenx = x / 1024.0 * 1.35 - 0.90;
		double screeny = y / 640.0 * 0.84 - 0.42;
		double angle = 0.798056 - atan(screeny);	//0.798056 = arctan(8.0/7.8)
		double vlen = sqrt(1.0 + screeny * screeny);
		double boardx = 4.2 + 7.8 * screenx / vlen / cos(angle);
		double boardy = 8.0 - 7.8 * tan(angle);
		hovered_location = 0;
		if(boardx >= matManager.vFields[8].Pos.X && boardx <= matManager.vFields[9].Pos.X) {
			if(boardy >= matManager.vFields[8].Pos.Y && boardy <= matManager.vFields[10].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_EXTRA;
			} else if(boardy >= matManager.vFields[56].Pos.Y && boardy <= matManager.vFields[58].Pos.Y) {//field
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= matManager.vFields[60].Pos.Y && boardy <= matManager.vFields[62].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(boardy >= matManager.vFields[134].Pos.Y && boardy <= matManager.vFields[132].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(boardy >= matManager.vFields[74].Pos.Y && boardy <= matManager.vFields[72].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= matManager.vFields[70].Pos.Y && boardy <= matManager.vFields[68].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_DECK;
			}
		} else if(boardx >= matManager.vFields[81].Pos.X && boardx <= matManager.vFields[80].Pos.X) {
			if(boardy >= matManager.vFields[82].Pos.Y && boardy <= matManager.vFields[80].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			}
		} else if(boardx >= matManager.vFields[0].Pos.X && boardx <= matManager.vFields[1].Pos.X) {
			if(boardy >= matManager.vFields[0].Pos.Y && boardy <= matManager.vFields[2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_DECK;
			} else if(boardy >= matManager.vFields[4].Pos.Y && boardy <= matManager.vFields[6].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= matManager.vFields[130].Pos.Y && boardy <= matManager.vFields[128].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(boardy >= matManager.vFields[64].Pos.Y && boardy <= matManager.vFields[66].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(boardy >= matManager.vFields[126].Pos.Y && boardy <= matManager.vFields[124].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= matManager.vFields[78].Pos.Y && boardy <= matManager.vFields[76].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_EXTRA;
			}
		} else if(boardx >= matManager.vFields[12].Pos.X && boardx <= matManager.vFields[13].Pos.X) {
			if(boardy >= matManager.vFields[12].Pos.Y && boardy <= matManager.vFields[14].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			}
		} else if(boardx >= matManager.vFields[36].Pos.X && boardx <= matManager.vFields[55].Pos.X) {
			if(boardy > matManager.vFields[36].Pos.Y && boardy <= matManager.vFields[38].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = (boardx - 1.2) / 1.1;
				if(hovered_sequence > 4)
					hovered_sequence = 4;
			} else if(boardy >= matManager.vFields[16].Pos.Y && boardy <= matManager.vFields[35].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = (boardx - 1.2) / 1.1;
				if(hovered_sequence > 4)
					hovered_sequence = 4;
			} else if(boardy >= matManager.vFields[103].Pos.Y && boardy <= matManager.vFields[84].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = 4 - (int)((boardx - 1.2) / 1.1);
				if(hovered_sequence < 0)
					hovered_sequence = 0;
			} else if(boardy >= matManager.vFields[123].Pos.Y && boardy < matManager.vFields[104].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 4 - (int)((boardx - 1.2) / 1.1);
				if(hovered_sequence < 0)
					hovered_sequence = 0;
			}
		}
}
}
void ClientField::ShowMenu(int flag, int x, int y) {
	if(!flag) {
		mainGame->wCmdMenu->setVisible(false);
		return;
	}
	int height = 1;
	if(flag & COMMAND_ACTIVATE) {
		mainGame->btnActivate->setVisible(true);
		mainGame->btnActivate->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnActivate->setVisible(false);
	if(flag & COMMAND_SUMMON) {
		mainGame->btnSummon->setVisible(true);
		mainGame->btnSummon->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnSummon->setVisible(false);
	if(flag & COMMAND_SPSUMMON) {
		mainGame->btnSPSummon->setVisible(true);
		mainGame->btnSPSummon->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnSPSummon->setVisible(false);
	if(flag & COMMAND_MSET) {
		mainGame->btnMSet->setVisible(true);
		mainGame->btnMSet->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnMSet->setVisible(false);
	if(flag & COMMAND_SSET) {
		if(!(clicked_card->type & TYPE_MONSTER))
			mainGame->btnSSet->setText(dataManager.GetSysString(1153));
		else
			mainGame->btnSSet->setText(dataManager.GetSysString(1159));
		mainGame->btnSSet->setVisible(true);
		mainGame->btnSSet->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnSSet->setVisible(false);
	if(flag & COMMAND_REPOS) {
		if(clicked_card->position & POS_FACEDOWN)
			mainGame->btnRepos->setText(dataManager.GetSysString(1154));
		else if(clicked_card->position & POS_ATTACK)
			mainGame->btnRepos->setText(dataManager.GetSysString(1155));
		else
			mainGame->btnRepos->setText(dataManager.GetSysString(1156));
		mainGame->btnRepos->setVisible(true);
		mainGame->btnRepos->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnRepos->setVisible(false);
	if(flag & COMMAND_ATTACK) {
		mainGame->btnAttack->setVisible(true);
		mainGame->btnAttack->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnAttack->setVisible(false);
	if(flag & COMMAND_LIST) {
		mainGame->btnShowList->setVisible(true);
		mainGame->btnShowList->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnShowList->setVisible(false);
	panel = mainGame->wCmdMenu;
	mainGame->wCmdMenu->setVisible(true);
	mainGame->wCmdMenu->setRelativePosition(irr::core::recti(x - 20 , y - 20 - height, x + 80, y - 20));
}

void ClientField::SetResponseSelectedCards() const {
	unsigned char respbuf[64];
	respbuf[0] = selected_cards.size();
	for (size_t i = 0; i < selected_cards.size(); ++i)
		respbuf[i + 1] = selected_cards[i]->select_seq;
	DuelClient::SetResponseB(respbuf, selected_cards.size() + 1);
}
}
