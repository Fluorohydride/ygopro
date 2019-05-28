#include "event_handler.h"
#include "client_field.h"
#include "math.h"
#include "network.h"
#include "game.h"
#include "duelclient.h"
#include "data_manager.h"
#include "image_manager.h"
#include "sound_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "materials.h"
#include "../ocgcore/common.h"
#include <algorithm>

namespace ygo {

bool ClientField::OnEvent(const irr::SEvent& event) {
	if(OnCommonEvent(event))
		return false;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		if(mainGame->fadingList.size())
			break;
		s32 id = event.GUIEvent.Caller->getID();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_HAND1:
			case BUTTON_HAND2:
			case BUTTON_HAND3: {
				mainGame->wHand->setVisible(false);
				if(mainGame->dInfo.curMsg == MSG_ROCK_PAPER_SCISSORS) {
					DuelClient::SetResponseI(id - BUTTON_HAND1 + 1);
					DuelClient::SendResponse();
				} else {
					mainGame->stHintMsg->setText(L"");
					mainGame->stHintMsg->setVisible(true);
					CTOS_HandResult cshr;
					cshr.res = id - BUTTON_HAND1 + 1;
					DuelClient::SendPacketToServer(CTOS_HAND_RESULT, cshr);
				}
				break;
			}
			case BUTTON_FIRST:
			case BUTTON_SECOND: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->HideElement(mainGame->wFTSelect);
				CTOS_TPResult cstr;
				cstr.res = BUTTON_SECOND - id;
				DuelClient::SendPacketToServer(CTOS_TP_RESULT, cstr);
				break;
			}
			case BUTTON_REPLAY_START: {
				if(!mainGame->dInfo.isReplay)
					break;
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				ReplayMode::Pause(false, true);
				break;
			}
			case BUTTON_REPLAY_EXIT: {
				if(!mainGame->dInfo.isReplay)
					break;
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				ReplayMode::StopReplay();
				break;
			}
			case BUTTON_REPLAY_SWAP: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				if(mainGame->dInfo.isReplay)
					ReplayMode::SwapField();
				else if (mainGame->dInfo.player_type == 7)
					mainGame->dField.ReplaySwap();
				break;
			}
			case BUTTON_REPLAY_UNDO: {
				if(!mainGame->dInfo.isReplay)
					break;
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				ReplayMode::Undo();
				break;
			}
			case BUTTON_REPLAY_SAVE: {
				if(mainGame->ebRSName->getText()[0] == 0)
					break;
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->actionParam = 1;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->actionParam = 0;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_LEAVE_GAME: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				if(mainGame->dInfo.isSingleMode) {
					mainGame->singleSignal.SetNoWait(true);
					SingleMode::StopPlay(false);
					break;
				}
				if(mainGame->dInfo.player_type == 7) {
					DuelClient::StopClient();
					mainGame->dInfo.isStarted = false;
					mainGame->dInfo.isFinished = false;
					mainGame->device->setEventReceiver(&mainGame->menuHandler);
					mainGame->stTip->setVisible(false);
					mainGame->wCardImg->setVisible(false);
					mainGame->wInfos->setVisible(false);
					mainGame->wPhase->setVisible(false);
					mainGame->btnLeaveGame->setVisible(false);
					mainGame->btnSpectatorSwap->setVisible(false);
					mainGame->wChat->setVisible(false);
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->btnStartBot->setEnabled(true);
					mainGame->btnBotCancel->setEnabled(true);
					if(bot_mode)
						mainGame->ShowElement(mainGame->wSinglePlay);
					else
						mainGame->ShowElement(mainGame->wLanWindow);
					if(exit_on_return)
						mainGame->device->closeDevice();
				} else {
					mainGame->PopupElement(mainGame->wSurrender);
				}
				break;
			}
			case BUTTON_SURRENDER_YES: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				DuelClient::SendPacketToServer(CTOS_SURRENDER);
				mainGame->HideElement(mainGame->wSurrender);
				break;
			}
			case BUTTON_SURRENDER_NO: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->HideElement(mainGame->wSurrender);
				break;
			}
			case BUTTON_CHAIN_IGNORE: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->ignore_chain = mainGame->btnChainIgnore->isPressed();
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CHAIN_ALWAYS: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->always_chain = mainGame->btnChainAlways->isPressed();
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CHAIN_WHENAVAIL: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->chain_when_avail = mainGame->btnChainWhenAvail->isPressed();
				mainGame->always_chain = false;
				mainGame->ignore_chain = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CANCEL_OR_FINISH: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				CancelOrFinish();
				break;
			}
			case BUTTON_MSG_OK: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->HideElement(mainGame->wMessage);
				mainGame->actionSignal.Set();
				break;
			}
			case BUTTON_YES: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
				case MSG_SELECT_CARD:
				case MSG_SELECT_TRIBUTE:
				case MSG_SELECT_SUM: {
					mainGame->HideElement(mainGame->wQuery);
					if(select_panalmode)
						mainGame->dField.ShowSelectCard(true);
					break;
				}
				case MSG_SELECT_CHAIN: {
					mainGame->HideElement(mainGame->wQuery);
					if (!chain_forced) {
						ShowCancelOrFinishButton(1);
					}
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
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
					ShowCancelOrFinishButton(0);
					break;
				}
				case MSG_SELECT_CARD:
				case MSG_SELECT_TRIBUTE:
				case MSG_SELECT_SUM: {
					SetResponseSelectedCards();
					ShowCancelOrFinishButton(0);
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
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				selected_option--;
				mainGame->btnOptionn->setVisible(true);
				if(selected_option == 0)
					mainGame->btnOptionp->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->guiFont, dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_NEXT: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				selected_option++;
				mainGame->btnOptionp->setVisible(true);
				if(selected_option == select_options.size() - 1)
					mainGame->btnOptionn->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->guiFont, dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_0:
			case BUTTON_OPTION_1: 
			case BUTTON_OPTION_2: 
			case BUTTON_OPTION_3: 
			case BUTTON_OPTION_4: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				int step = mainGame->scrOption->isVisible() ? mainGame->scrOption->getPos() : 0;
				selected_option = id - BUTTON_OPTION_0 + step;
				SetResponseSelectedOption();
				break;
			}
			case BUTTON_OPTION_OK: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				SetResponseSelectedOption();
				break;
			}
			case BUTTON_ANNUMBER_OK: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				DuelClient::SetResponseI(mainGame->cbANNumber->getSelected());
				mainGame->HideElement(mainGame->wANNumber, true);
				break;
			}
			case BUTTON_ANCARD_OK: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
			case BUTTON_CMD_ACTIVATE:
			case BUTTON_CMD_RESET: {
				mainGame->wCmdMenu->setVisible(false);
				ShowCancelOrFinishButton(0);
				if(!list_command) {
					int index = -1;
					select_options.clear();
					for (size_t i = 0; i < activatable_cards.size(); ++i) {
						if (activatable_cards[i] == clicked_card) {
							if(activatable_descs[i].second == EDESC_OPERATION)
								continue;
							if(activatable_descs[i].second == EDESC_RESET) {
								if(id == BUTTON_CMD_ACTIVATE) continue;
							} else {
								if(id == BUTTON_CMD_RESET) continue;
							}
							select_options.push_back(activatable_descs[i].first);
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
						command_card = clicked_card;
						ShowSelectOption();
					}
				} else {
					selectable_cards.clear();
					conti_selecting = false;
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
						break;
					}
					case LOCATION_EXTRA: {
						for(size_t i = 0; i < extra[command_controler].size(); ++i)
							if(extra[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(extra[command_controler][i]);
						break;
					}
					case POSITION_HINT: {
						selectable_cards = conti_cards;
						std::sort(selectable_cards.begin(), selectable_cards.end());
						auto eit = std::unique(selectable_cards.begin(), selectable_cards.end());
						selectable_cards.erase(eit, selectable_cards.end());
						conti_selecting = true;
						break;
					}
					}
					if(!conti_selecting) {
						mainGame->wCardSelect->setText(dataManager.GetSysString(566));
						list_command = COMMAND_ACTIVATE;
					} else {
						mainGame->wCardSelect->setText(dataManager.GetSysString(568));
						list_command = COMMAND_OPERATION;
					}
					std::sort(selectable_cards.begin(), selectable_cards.end(), ClientCard::client_card_sort);
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
					select_ready = false;
					ShowCancelOrFinishButton(1);
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
				wchar_t formatBuffer[2048];
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
				std::sort(selectable_cards.begin(), selectable_cards.end(), ClientCard::client_card_sort);
				ShowSelectCard(true);
				break;
			}
			case BUTTON_PHASE: {
				mainGame->btnPhaseStatus->setPressed(true);
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
						ShowCancelOrFinishButton(0);
						break;
					}
					if(list_command == COMMAND_ACTIVATE || list_command == COMMAND_OPERATION) {
						int index = -1;
						command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
						select_options.clear();
						for (size_t i = 0; i < activatable_cards.size(); ++i) {
							if (activatable_cards[i] == command_card) {
								if(activatable_descs[i].second == EDESC_OPERATION) {
									if(list_command == COMMAND_ACTIVATE) continue;
								} else {
									if(list_command == COMMAND_OPERATION) continue;
								}
								select_options.push_back(activatable_descs[i].first);
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
							mainGame->wCardSelect->setVisible(false);
							ShowSelectOption();
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
						ShowCancelOrFinishButton(0);
						mainGame->HideElement(mainGame->wCardSelect, true);
					} else if (sel >= select_min) {
						select_ready = true;
						mainGame->btnSelectOK->setVisible(true);
						ShowCancelOrFinishButton(2);
					} else {
						select_ready = false;
						mainGame->btnSelectOK->setVisible(false);
						if (select_cancelable && sel == 0)
							ShowCancelOrFinishButton(1);
						else
							ShowCancelOrFinishButton(0);
					}
					break;
				}
				case MSG_SELECT_UNSELECT_CARD: {
					command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
					if (command_card->is_selected) {
						command_card->is_selected = false;
						if(command_card->controler)
							mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffd0d0d0);
						else mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffffffff);
					} else {
						command_card->is_selected = true;
						mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(0xffffff00);
					}
					selected_cards.push_back(command_card);
					if (selected_cards.size() > 0) {
						SetResponseSelectedCards();
						ShowCancelOrFinishButton(0);
						mainGame->HideElement(mainGame->wCardSelect, true);}
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
					wchar_t formatBuffer[2048];
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
						ShowCancelOrFinishButton(0);
						mainGame->HideElement(mainGame->wCardSelect, true);
					}
					break;
				} else if(mainGame->dInfo.curMsg == MSG_CONFIRM_CARDS) {
					mainGame->HideElement(mainGame->wCardSelect);
					mainGame->actionSignal.Set();
					break;
				} else if(mainGame->dInfo.curMsg == MSG_SELECT_UNSELECT_CARD){
					DuelClient::SetResponseI(-1);
					ShowCancelOrFinishButton(0);
					mainGame->HideElement(mainGame->wCardSelect, true);
				} else {
					mainGame->HideElement(mainGame->wCardSelect);
					if(mainGame->dInfo.curMsg == MSG_SELECT_CHAIN && !chain_forced)
						ShowCancelOrFinishButton(1);
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
				for(int i = 0; i < 25; ++i, filter <<= 1) {
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
			case LISTBOX_ANCARD: {
				int sel = mainGame->lstANCard->getSelected();
				if(sel != -1) {
					mainGame->ShowCardInfo(ancard[sel]);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
			case SCROLL_OPTION_SELECT: {
				int step = mainGame->scrOption->isVisible() ? mainGame->scrOption->getPos() : 0;
				for(int i = 0; i < 5; i++) {
					const wchar_t* option = dataManager.GetDesc(select_options[i + step]);
					mainGame->btnOption[i]->setText(option);
				}
				break;
			}
			case SCROLL_CARD_SELECT: {
				int pos = mainGame->scrCardList->getPos() / 10;
				for(int i = 0; i < 5; ++i) {
					// draw selectable_cards[i + pos] in btnCardSelect[i]
					mainGame->stCardPos[i]->enableOverrideColor(false);
					// image
					if(selectable_cards[i + pos]->code)
						mainGame->btnCardSelect[i]->setImage(imageManager.GetTexture(selectable_cards[i + pos]->code));
					else if(conti_selecting)
						mainGame->btnCardSelect[i]->setImage(imageManager.GetTexture(selectable_cards[i + pos]->chain_code));
					else
						mainGame->btnCardSelect[i]->setImage(imageManager.tCover[selectable_cards[i + pos]->controler + 2]);
					mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					// text
					wchar_t formatBuffer[2048];
					if(sort_list.size()) {
						if(sort_list[pos + i] > 0)
							myswprintf(formatBuffer, L"%d", sort_list[pos + i]);
						else
							myswprintf(formatBuffer, L"");
					} else {
						if(conti_selecting)
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
					// color
					if(conti_selecting)
						mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					else if(selectable_cards[i + pos]->location == LOCATION_OVERLAY) {
						if(selectable_cards[i + pos]->owner != selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setOverrideColor(0xff0000ff);
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
						else if(selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					} else if(selectable_cards[i + pos]->location == LOCATION_DECK || selectable_cards[i + pos]->location == LOCATION_EXTRA || selectable_cards[i + pos]->location == LOCATION_REMOVED) {
						if(selectable_cards[i + pos]->position & POS_FACEDOWN)
							mainGame->stCardPos[i]->setOverrideColor(0xff0000ff);
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
						else if(selectable_cards[i + pos]->controler)
							mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					} else {
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
						else if(selectable_cards[i + pos]->controler)
							mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
					}
				}
				break;
			}
			case SCROLL_CARD_DISPLAY: {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				for(int i = 0; i < 5; ++i) {
					// draw display_cards[i + pos] in btnCardDisplay[i]
					mainGame->stDisplayPos[i]->enableOverrideColor(false);
					if(display_cards[i + pos]->code)
						mainGame->btnCardDisplay[i]->setImage(imageManager.GetTexture(display_cards[i + pos]->code));
					else
						mainGame->btnCardDisplay[i]->setImage(imageManager.tCover[display_cards[i + pos]->controler + 2]);
					mainGame->btnCardDisplay[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					wchar_t formatBuffer[2048];
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
						// BackgroundColor: controller of the xyz monster
						if(display_cards[i + pos]->overlayTarget->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffffffff);
					} else if(display_cards[i + pos]->location == LOCATION_EXTRA || display_cards[i + pos]->location == LOCATION_REMOVED) {
						if(display_cards[i + pos]->position & POS_FACEDOWN)
							mainGame->stDisplayPos[i]->setOverrideColor(0xff0000ff);
						if(display_cards[i + pos]->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffffffff);
					} else {
						if(display_cards[i + pos]->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffd0d0d0);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(0xffffffff);
					}
				}
				break;
			}
			break;
			}
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch(id) {
			case EDITBOX_ANCARD: {
				UpdateDeclarableCode();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_ANCARD: {
				UpdateDeclarableCode();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_HOVERED: {
			if(id >= BUTTON_CARD_0 && id <= BUTTON_CARD_4) {
				int pos = mainGame->scrCardList->getPos() / 10;
				ClientCard* mcard = selectable_cards[id - BUTTON_CARD_0 + pos];
				SetShowMark(mcard, true);
				ShowCardInfoInList(mcard, mainGame->btnCardSelect[id - BUTTON_CARD_0], mainGame->wCardSelect);
				if(mcard->code) {
					mainGame->ShowCardInfo(mcard->code);
				} else {
					mainGame->ClearCardInfo(mcard->controler);
				}
			}
			if(id >= BUTTON_DISPLAY_0 && id <= BUTTON_DISPLAY_4) {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				ClientCard* mcard = display_cards[id - BUTTON_DISPLAY_0 + pos];
				SetShowMark(mcard, true);
				ShowCardInfoInList(mcard, mainGame->btnCardDisplay[id - BUTTON_DISPLAY_0], mainGame->wCardDisplay);
				if(mcard->code) {
					mainGame->ShowCardInfo(mcard->code);
				} else {
					mainGame->ClearCardInfo(mcard->controler);
				}
			}
			if(id == TEXT_CARD_LIST_TIP) {
				mainGame->stCardListTip->setVisible(true);
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_LEFT: {
			if(id >= BUTTON_CARD_0 && id <= BUTTON_CARD_4) {
				int pos = mainGame->scrCardList->getPos() / 10;
				ClientCard* mcard = selectable_cards[id - BUTTON_CARD_0 + pos];
				SetShowMark(mcard, false);
				mainGame->stCardListTip->setVisible(false);
			}
			if(id >= BUTTON_DISPLAY_0 && id <= BUTTON_DISPLAY_4) {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				ClientCard* mcard = display_cards[id - BUTTON_DISPLAY_0 + pos];
				SetShowMark(mcard, false);
				mainGame->stCardListTip->setVisible(false);
			}
			if(id == TEXT_CARD_LIST_TIP) {
				mainGame->stCardListTip->setVisible(false);
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
			hovered_location = 0;
			position2di pos = mainGame->ResizeReverse(event.MouseInput.X, event.MouseInput.Y);
			position2di mousepos(event.MouseInput.X, event.MouseInput.Y);
			s32 x = pos.X;
			s32 y = pos.Y;
			if(x < 300)
				break;
			if(mainGame->gameConf.control_mode == 1) {
				mainGame->always_chain = event.MouseInput.isLeftPressed();
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
			if(mainGame->wCmdMenu->isVisible() && !mainGame->wCmdMenu->getRelativePosition().isPointInside(mousepos))
				mainGame->wCmdMenu->setVisible(false);
			if(panel && panel->isVisible())
				break;
			GetHoverField(x, y);
			if(hovered_location & 0xe)
				clicked_card = GetCard(hovered_controler, hovered_location, hovered_sequence);
			else clicked_card = 0;
			wchar_t formatBuffer[2048];
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
				case LOCATION_REMOVED: {
					if (remove[hovered_controler].size() == 0)
						break;
					for (int32 i = (int32)remove[hovered_controler].size() - 1; i >= 0; --i)
						selectable_cards.push_back(remove[hovered_controler][i]);
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(1005), remove[hovered_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_EXTRA: {
					if (extra[hovered_controler].size() == 0)
						break;
					for (int32 i = (int32)extra[hovered_controler].size() - 1; i >= 0; --i)
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
					if(remove[hovered_controler].size() == 0)
						break;
					for(size_t i = 0; i < remove[hovered_controler].size(); ++i)
						command_flag |= remove[hovered_controler][i]->cmdFlag;
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
				case POSITION_HINT: {
					int command_flag = 0;
					if(conti_cards.size() == 0)
						break;
					command_flag |= COMMAND_OPERATION;
					list_command = 1;
					ShowMenu(command_flag, x, y);
					break;
				}
				}
				break;
			}
			case MSG_SELECT_PLACE:
			case MSG_SELECT_DISFIELD: {
				if (!(hovered_location & LOCATION_ONFIELD))
					break;
				unsigned int flag = 1 << (hovered_sequence + (hovered_controler << 4) + ((hovered_location == LOCATION_MZONE) ? 0 : 8));
				if (flag & selectable_field) {
					if (flag & selected_field) {
						selected_field &= ~flag;
						select_min++;
					} else {
						selected_field |= flag;
						select_min--;
						if (select_min == 0) {
							unsigned char respbuf[80];
							int filter = 1;
							int p = 0;
							for (int i = 0; i < 7; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(0);
									respbuf[p + 1] = LOCATION_MZONE;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x100;
							for (int i = 0; i < 8; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(0);
									respbuf[p + 1] = LOCATION_SZONE;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x10000;
							for (int i = 0; i < 7; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(1);
									respbuf[p + 1] = LOCATION_MZONE;
									respbuf[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x1000000;
							for (int i = 0; i < 8; ++i, filter <<= 1) {
								if (selected_field & filter) {
									respbuf[p] = mainGame->LocalPlayer(1);
									respbuf[p + 1] = LOCATION_SZONE;
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
					ShowCancelOrFinishButton(0);
					DuelClient::SendResponse();
				} else if (max >= select_min) {
					if(selected_cards.size() == selectable_cards.size()) {
						SetResponseSelectedCards();
						ShowCancelOrFinishButton(0);
						DuelClient::SendResponse();
					} else {
						select_ready = true;
						ShowCancelOrFinishButton(2);
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
					if (select_cancelable && min == 0)
						ShowCancelOrFinishButton(1);
					else
						ShowCancelOrFinishButton(0);
				}
				break;
			}
			case MSG_SELECT_UNSELECT_CARD: {
				if (!(hovered_location & 0xe) || !clicked_card || !clicked_card->is_selectable)
					break;
				if (clicked_card->is_selected) {
					clicked_card->is_selected = false;
				} else {
					clicked_card->is_selected = true;
				}
				selected_cards.push_back(clicked_card);
				if (selected_cards.size() > 0) {
					ShowCancelOrFinishButton(0);
					SetResponseSelectedCards();
					DuelClient::SendResponse();
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
					unsigned short int respbuf[32];
					for(size_t i = 0; i < selectable_cards.size(); ++i)
						respbuf[i] = (selectable_cards[i]->opParam >> 16) - (selectable_cards[i]->opParam & 0xffff);
					mainGame->stHintMsg->setVisible(false);
					ClearSelect();
					DuelClient::SetResponseB(respbuf, selectable_cards.size() * 2);
					DuelClient::SendResponse();
				} else {
					wchar_t formatBuffer[2048];
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
			if(event.MouseInput.isLeftPressed())
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300 * mainGame->xScale) {
				mainGame->ignore_chain = event.MouseInput.isRightPressed();
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
			if(mainGame->wSurrender->isVisible())
				mainGame->HideElement(mainGame->wSurrender);
			mainGame->wCmdMenu->setVisible(false);
			if(mainGame->fadingList.size())
				break;
			CancelOrFinish();
			break;
		}
		case irr::EMIE_MOUSE_MOVED: {
			if(!mainGame->dInfo.isStarted)
				break;
			bool should_show_tip = false;
			position2di pos = mainGame->ResizeReverse(event.MouseInput.X, event.MouseInput.Y);
			position2di mousepos = position2di(event.MouseInput.X, event.MouseInput.Y);
			s32 x = pos.X;
			s32 y = pos.Y;
			wchar_t formatBuffer[2048];
			if(x < 300) {
				irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
				irr::gui::IGUIElement* elem = root->getElementFromPoint(pos);
				if(elem == mainGame->btnCancelOrFinish) {
					should_show_tip = true;
					myswprintf(formatBuffer, dataManager.GetSysString(1700), mainGame->btnCancelOrFinish->getText());
					mainGame->stTip->setText(formatBuffer);
					irr::core::dimension2d<unsigned int> dtip = mainGame->guiFont->getDimension(formatBuffer) + irr::core::dimension2d<unsigned int>(10, 10);
					mainGame->stTip->setRelativePosition(mainGame->Resize(x - 10 - dtip.Width, y - 10 - dtip.Height, x - 10, y - 10));
				}
				mainGame->stTip->setVisible(should_show_tip);
				break;
			}
			hovered_location = 0;
			ClientCard* mcard = 0;
			int mplayer = -1;
			if(!panel || !panel->isVisible() || !panel->getRelativePosition().isPointInside(mousepos)) {
				GetHoverField(x, y);
				if(hovered_location & 0xe)
					mcard = GetCard(hovered_controler, hovered_location, hovered_sequence);
				else if(hovered_location == LOCATION_GRAVE) {
					if(grave[hovered_controler].size())
						mcard = grave[hovered_controler].back();
				} else if(hovered_location == LOCATION_REMOVED) {
					if(remove[hovered_controler].size()) {
						mcard = remove[hovered_controler].back();
						if(mcard->position & POS_FACEDOWN)
							mcard = 0;
					}
				} else if(hovered_location == LOCATION_EXTRA) {
					if(extra[hovered_controler].size()) {
						mcard = extra[hovered_controler].back();
						if(mcard->position & POS_FACEDOWN)
							mcard = 0;
					}
				} else if(hovered_location == LOCATION_DECK) {
					if(deck[hovered_controler].size())
						mcard = deck[hovered_controler].back();
				} else {
					if(mainGame->Resize(327, 8, 630, 51).isPointInside(mousepos))
						mplayer = 0;
					else if(mainGame->Resize(689, 8, 991, 51).isPointInside(mousepos))
						mplayer = 1;
				}
			}
			if(hovered_location == LOCATION_HAND && (mainGame->dInfo.is_shuffling || mainGame->dInfo.curMsg == MSG_SHUFFLE_HAND))
				mcard = 0;
			if(mcard == 0 && mplayer < 0)
				should_show_tip = false;
			else if(mcard == hovered_card && mplayer == hovered_player) {
				if(mainGame->stTip->isVisible()) {
					should_show_tip = true;
					irr::core::recti tpos = mainGame->stTip->getRelativePosition();
					mainGame->stTip->setRelativePosition(irr::core::position2di(mousepos.X - tpos.getWidth() - 10, mcard ? mousepos.Y - tpos.getHeight() - 10 : y + 10));
				}
			}
			if(mcard != hovered_card) {
				if(hovered_card) {
					if(hovered_card->location == LOCATION_HAND && !mainGame->dInfo.is_shuffling && mainGame->dInfo.curMsg != MSG_SHUFFLE_HAND) {
						hovered_card->is_hovered = false;
						MoveCard(hovered_card, 5);
						if(hovered_controler == 0)
							mainGame->hideChat = false;
					}
					SetShowMark(hovered_card, false);
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
					SetShowMark(mcard, true);
					if(mcard->code) {
						mainGame->ShowCardInfo(mcard->code);
						if(mcard->location & 0xe) {
							std::wstring str;
							myswprintf(formatBuffer, L"%ls", dataManager.GetName(mcard->code));
							str.append(formatBuffer);
							if(mcard->type & TYPE_MONSTER) {
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)
								        && wcscmp(dataManager.GetName(mcard->code), dataManager.GetName(mcard->alias))) {
									myswprintf(formatBuffer, L"\n(%ls)", dataManager.GetName(mcard->alias));
									str.append(formatBuffer);
								}
								myswprintf(formatBuffer, L"\n%ls/%ls", mcard->atkstring, mcard->defstring);
								str.append(formatBuffer);
								if(!(mcard->type & TYPE_LINK)) {
									const wchar_t* form = L"\u2605";
									if (mcard->rank) form = L"\u2606";
									myswprintf(formatBuffer, L"\n%ls%d", form, (mcard->level ? mcard->level : mcard->rank));
									str.append(formatBuffer);
								} else {
									myswprintf(formatBuffer, L"\nLINK-%d", mcard->link);
									str.append(formatBuffer);
								}
								myswprintf(formatBuffer, L" %ls/%ls", dataManager.FormatRace(mcard->race), dataManager.FormatAttribute(mcard->attribute));
								str.append(formatBuffer);
								if(mcard->location == LOCATION_HAND && (mcard->type & TYPE_PENDULUM)) {
									myswprintf(formatBuffer, L"\n%d/%d", mcard->lscale, mcard->rscale);
									str.append(formatBuffer);
								}
							} else {
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)) {
									myswprintf(formatBuffer, L"\n(%ls)", dataManager.GetName(mcard->alias));
									str.append(formatBuffer);
								}
								if(mcard->location == LOCATION_SZONE && mcard->lscale) {
									myswprintf(formatBuffer, L"\n%d/%d", mcard->lscale, mcard->rscale);
									str.append(formatBuffer);
								}
							}
							for(auto ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
								myswprintf(formatBuffer, L"\n[%ls]: %d", dataManager.GetCounterName(ctit->first), ctit->second);
								str.append(formatBuffer);
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
							should_show_tip = true;
							irr::core::dimension2d<unsigned int> dtip = mainGame->guiFont->getDimension(str.c_str()) + irr::core::dimension2d<unsigned int>(10, 10);
							mainGame->stTip->setRelativePosition(recti(mousepos.X - 10 - dtip.Width, mousepos.Y - 10 - dtip.Height, mousepos.X - 10, mousepos.Y - 10));
							mainGame->stTip->setText(str.c_str());
						}
					} else {
						should_show_tip = false;
						mainGame->ClearCardInfo(mcard->controler);
					}
				}
				hovered_card = mcard;
			}
			if(mplayer != hovered_player) {
				if(mplayer >= 0) {
					const wchar_t* player_name;
					if(mplayer == 0) {
						if(!mainGame->dInfo.isTag || !mainGame->dInfo.tag_player[0])
							player_name = mainGame->dInfo.hostname;
						else
							player_name = mainGame->dInfo.hostname_tag;
					} else {
						if(!mainGame->dInfo.isTag || !mainGame->dInfo.tag_player[1])
							player_name = mainGame->dInfo.clientname;
						else
							player_name = mainGame->dInfo.clientname_tag;
					}
					std::wstring str(player_name);
					const auto& player_desc_hints = mainGame->dField.player_desc_hints[mplayer];
					for(auto iter = player_desc_hints.begin(); iter != player_desc_hints.end(); ++iter) {
						myswprintf(formatBuffer, L"\n*%ls", dataManager.GetDesc(iter->first));
						str.append(formatBuffer);
					}
					should_show_tip = true;
					irr::core::dimension2d<unsigned int> dtip = mainGame->guiFont->getDimension(str.c_str()) + irr::core::dimension2d<unsigned int>(10, 10);
					mainGame->stTip->setRelativePosition(recti(mousepos.X - 10 - dtip.Width, mousepos.Y + 10, mousepos.X - 10, mousepos.Y + 10 + dtip.Height));
					mainGame->stTip->setText(str.c_str());
				}
				hovered_player = mplayer;
			}
			mainGame->stTip->setVisible(should_show_tip);
			break;
		}
		case irr::EMIE_MOUSE_WHEEL: {
			break;
		}
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			if(!mainGame->dInfo.isStarted)
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300 * mainGame->xScale) {
				mainGame->always_chain = event.MouseInput.isLeftPressed();
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
			break;
		}
		case irr::EMIE_RMOUSE_PRESSED_DOWN: {
			if(!mainGame->dInfo.isStarted)
				break;
			if(mainGame->gameConf.control_mode == 1 && event.MouseInput.X > 300 * mainGame->xScale) {
				mainGame->ignore_chain = event.MouseInput.isRightPressed();
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
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
			if(mainGame->gameConf.control_mode == 0 && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				mainGame->always_chain = event.KeyInput.PressedDown;
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
			break;
		}
		case irr::KEY_KEY_S: {
			if(mainGame->gameConf.control_mode == 0 && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				mainGame->ignore_chain = event.KeyInput.PressedDown;
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
			}
			break;
		}
		case irr::KEY_KEY_D: {
			if(mainGame->gameConf.control_mode == 0 && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				mainGame->chain_when_avail = event.KeyInput.PressedDown;
				mainGame->always_chain = false;
				mainGame->ignore_chain = false;
				UpdateChainButtons();
			}
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
						for(auto it = grave[0].rbegin(); it != grave[0].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F2:
						loc_id = 1005;
						for(auto it = remove[0].rbegin(); it != remove[0].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F3:
						loc_id = 1006;
						for(auto it = extra[0].rbegin(); it != extra[0].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F4:
						loc_id = 1007;
						for(auto it = mzone[0].begin(); it != mzone[0].end(); ++it) {
							if(*it) {
								for(auto oit = (*it)->overlayed.begin(); oit != (*it)->overlayed.end(); ++oit)
									display_cards.push_back(*oit);
							}
						}
						break;
					case irr::KEY_F5:
						loc_id = 1004;
						for(auto it = grave[1].rbegin(); it != grave[1].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F6:
						loc_id = 1005;
						for(auto it = remove[1].rbegin(); it != remove[1].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F7:
						loc_id = 1006;
						for(auto it = extra[1].rbegin(); it != extra[1].rend(); ++it)
							display_cards.push_back(*it);
						break;
					case irr::KEY_F8:
						loc_id = 1007;
						for(auto it = mzone[1].begin(); it != mzone[1].end(); ++it) {
							if(*it) {
								for(auto oit = (*it)->overlayed.begin(); oit != (*it)->overlayed.end(); ++oit)
									display_cards.push_back(*oit);
							}
						}
						break;
				}
				if(display_cards.size()) {
					wchar_t formatBuffer[2048];
					myswprintf(formatBuffer, L"%ls(%d)", dataManager.GetSysString(loc_id), display_cards.size());
					mainGame->wCardDisplay->setText(formatBuffer);
					ShowLocationCard();
				}
			}
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
bool ClientField::OnCommonEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_ELEMENT_HOVERED: {
			if(event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX) {
				mainGame->SetCursor(event.GUIEvent.Caller->isEnabled() ? ECI_IBEAM : ECI_NORMAL);
				return true;
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_LEFT: {
			if(event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX) {
				mainGame->SetCursor(ECI_NORMAL);
				return true;
			}
			break;
		}
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_LOG: {
				soundManager.PlaySoundEffect(SOUND_BUTTON);
				mainGame->lstLog->clear();
				mainGame->logParam.clear();
				return true;
				break;
			}
			case BUTTON_WINDOW_RESIZE_S: {
				mainGame->SetWindowsScale(0.8f);
				return true;
				break;
			}
			case BUTTON_WINDOW_RESIZE_M: {
				mainGame->SetWindowsScale(1.0f);
				return true;
				break;
			}
			case BUTTON_WINDOW_RESIZE_L: {
				mainGame->SetWindowsScale(1.25f);
				return true;
				break;
			}
			case BUTTON_WINDOW_RESIZE_XL: {
				mainGame->SetWindowsScale(1.5f);
				return true;
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch(id) {
			case CHECKBOX_AUTO_SEARCH: {
				mainGame->gameConf.auto_search_limit = mainGame->chkAutoSearch->isChecked() ? 0 : -1;
				if(mainGame->is_building && !mainGame->is_siding)
					mainGame->deckBuilder.InstantSearch();
				return true;
				break;
			}
			case CHECKBOX_MULTI_KEYWORDS: {
				mainGame->gameConf.search_multiple_keywords = mainGame->chkMultiKeywords->isChecked() ? 1 : 0;
				if(mainGame->is_building && !mainGame->is_siding)
					mainGame->deckBuilder.InstantSearch();
				return true;
				break;
			}
			case CHECKBOX_ENABLE_MUSIC: {
				if(!mainGame->chkEnableMusic->isChecked())
					soundManager.StopBGM();
				return true;
				break;
			}
			case CHECKBOX_DISABLE_CHAT: {
				bool show = mainGame->is_building ? false : !mainGame->chkIgnore1->isChecked();
				mainGame->wChat->setVisible(show);
				if(!show)
					mainGame->ClearChatMsg();
				return true;
				break;
			}
			case CHECKBOX_QUICK_ANIMATION: {
				mainGame->gameConf.quick_animation = mainGame->chkQuickAnimation->isChecked() ? 1 : 0;
				return true;
				break;
			}
			case CHECKBOX_PREFER_EXPANSION: {
				mainGame->gameConf.prefer_expansion_script = mainGame->chkPreferExpansionScript->isChecked() ? 1 : 0;
				return true;
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
				return true;
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
				return true;
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
			case SCROLL_CARDTEXT: {
				if(!mainGame->scrCardText->isVisible()) {
					return true;
					break;
				}
				u32 pos = mainGame->scrCardText->getPos();
				mainGame->SetStaticText(mainGame->stText, mainGame->stText->getRelativePosition().getWidth() - 25, mainGame->guiFont, mainGame->showingtext, pos);
				return true;
				break;
			}
			case SCROLL_VOLUME: {
				mainGame->gameConf.sound_volume = (double)mainGame->scrSoundVolume->getPos() / 100;
				mainGame->gameConf.music_volume = (double)mainGame->scrMusicVolume->getPos() / 100;
				soundManager.SetSoundVolume(mainGame->gameConf.sound_volume);
				soundManager.SetMusicVolume(mainGame->gameConf.music_volume);
				return true;
				break;
			}
			case SCROLL_TAB_HELPER: {
				rect<s32> pos = mainGame->tabHelper->getRelativePosition();
				mainGame->tabHelper->setRelativePosition(recti(0, mainGame->scrTabHelper->getPos() * -1, pos.LowerRightCorner.X, pos.LowerRightCorner.Y));
				return true;
				break;
			}
			case SCROLL_TAB_SYSTEM: {
				rect<s32> pos = mainGame->tabSystem->getRelativePosition();
				mainGame->tabSystem->setRelativePosition(recti(0, mainGame->scrTabSystem->getPos() * -1, pos.LowerRightCorner.X, pos.LowerRightCorner.Y));
				return true;
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_CHAT: {
				if(mainGame->dInfo.isReplay)
					break;
				const wchar_t* input = mainGame->ebChatInput->getText();
				if(input[0]) {
					unsigned short msgbuf[256];
					int len = BufferIO::CopyWStr(input, msgbuf, 256);
					DuelClient::SendBufferToServer(CTOS_CHAT, msgbuf, (len + 1) * sizeof(short));
					mainGame->ebChatInput->setText(L"");
					return true;
				}
				break;
			}
			}
			break;
		}
		default: break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_R: {
			if(mainGame->gameConf.control_mode == 0
				&& !event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				mainGame->textFont->setTransparency(true);
				mainGame->guiFont->setTransparency(true);
			}
			return true;
			break;
		}
		case irr::KEY_F9: {
			if(mainGame->gameConf.control_mode == 1
				&& !event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX)) {
				mainGame->textFont->setTransparency(true);
				mainGame->guiFont->setTransparency(true);
			}
			return true;
			break;
		}
		case irr::KEY_ESCAPE: {
			if(!mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->device->minimizeWindow();
			return true;
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
	int rule = (mainGame->dInfo.duel_rule >= 4) ? 1 : 0;
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
		if(boardx >= matManager.vFieldExtra[0][0].Pos.X && boardx <= matManager.vFieldExtra[0][1].Pos.X) {
			if(boardy >= matManager.vFieldExtra[0][0].Pos.Y && boardy <= matManager.vFieldExtra[0][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_EXTRA;
			} else if(boardy >= matManager.vFieldSzone[0][5][rule][0].Pos.Y && boardy <= matManager.vFieldSzone[0][5][rule][2].Pos.Y) {//field
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= matManager.vFieldSzone[0][6][rule][0].Pos.Y && boardy <= matManager.vFieldSzone[0][6][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(rule == 1 && boardy >= matManager.vFieldRemove[1][rule][2].Pos.Y && boardy <= matManager.vFieldRemove[1][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			} else if(rule == 0 && boardy >= matManager.vFieldSzone[1][7][rule][2].Pos.Y && boardy <= matManager.vFieldSzone[1][7][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(boardy >= matManager.vFieldGrave[1][rule][2].Pos.Y && boardy <= matManager.vFieldGrave[1][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= matManager.vFieldDeck[1][2].Pos.Y && boardy <= matManager.vFieldDeck[1][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_DECK;
			}
		} else if (boardx >= matManager.vFieldContiAct[0].X && boardx <= matManager.vFieldContiAct[1].X
				&& boardy >= matManager.vFieldContiAct[0].Y && boardy <= matManager.vFieldContiAct[2].Y) {
			hovered_controler = 0;
			hovered_location = POSITION_HINT;
		} else if(rule == 0 && boardx >= matManager.vFieldRemove[1][rule][1].Pos.X && boardx <= matManager.vFieldRemove[1][rule][0].Pos.X) {
			if(boardy >= matManager.vFieldRemove[1][rule][2].Pos.Y && boardy <= matManager.vFieldRemove[1][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			}
		} else if(rule == 1 && boardx >= matManager.vFieldSzone[1][7][rule][1].Pos.X && boardx <= matManager.vFieldSzone[1][7][rule][2].Pos.X) {
			if(boardy >= matManager.vFieldSzone[1][7][rule][2].Pos.Y && boardy <= matManager.vFieldSzone[1][7][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			}
		} else if(boardx >= matManager.vFieldDeck[0][0].Pos.X && boardx <= matManager.vFieldDeck[0][1].Pos.X) {
			if(boardy >= matManager.vFieldDeck[0][0].Pos.Y && boardy <= matManager.vFieldDeck[0][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_DECK;
			} else if(boardy >= matManager.vFieldGrave[0][rule][0].Pos.Y && boardy <= matManager.vFieldGrave[0][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= matManager.vFieldSzone[1][6][rule][2].Pos.Y && boardy <= matManager.vFieldSzone[1][6][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(rule == 0 && boardy >= matManager.vFieldSzone[0][7][rule][0].Pos.Y && boardy <= matManager.vFieldSzone[0][7][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(rule == 1 && boardy >= matManager.vFieldRemove[0][rule][0].Pos.Y && boardy <= matManager.vFieldRemove[0][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			} else if(boardy >= matManager.vFieldSzone[1][5][rule][2].Pos.Y && boardy <= matManager.vFieldSzone[1][5][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= matManager.vFieldExtra[1][2].Pos.Y && boardy <= matManager.vFieldExtra[1][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_EXTRA;
			}
		} else if(rule == 0 && boardx >= matManager.vFieldSzone[0][7][rule][0].Pos.X && boardx <= matManager.vFieldSzone[0][7][rule][1].Pos.X) {
			if(boardy >= matManager.vFieldSzone[0][7][rule][0].Pos.Y && boardy <= matManager.vFieldSzone[0][7][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			}
		} else if(rule == 0 && boardx >= matManager.vFieldRemove[0][rule][0].Pos.X && boardx <= matManager.vFieldRemove[0][rule][1].Pos.X) {
			if(boardy >= matManager.vFieldRemove[0][rule][0].Pos.Y && boardy <= matManager.vFieldRemove[0][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			}
		} else if(boardx >= matManager.vFieldMzone[0][0][0].Pos.X && boardx <= matManager.vFieldMzone[0][4][1].Pos.X) {
			int sequence = (boardx - matManager.vFieldMzone[0][0][0].Pos.X) / (matManager.vFieldMzone[0][0][1].Pos.X - matManager.vFieldMzone[0][0][0].Pos.X);
			if(sequence > 4)
				sequence = 4;
			if(boardy > matManager.vFieldSzone[0][0][rule][0].Pos.Y && boardy <= matManager.vFieldSzone[0][0][rule][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = sequence;
			} else if(boardy >= matManager.vFieldMzone[0][0][0].Pos.Y && boardy <= matManager.vFieldMzone[0][0][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = sequence;
			} else if(boardy >= matManager.vFieldMzone[0][5][0].Pos.Y && boardy <= matManager.vFieldMzone[0][5][2].Pos.Y) {
				if(sequence == 1) {
					if(!mzone[1][6]) {
						hovered_controler = 0;
						hovered_location = LOCATION_MZONE;
						hovered_sequence = 5;
					} else {
						hovered_controler = 1;
						hovered_location = LOCATION_MZONE;
						hovered_sequence = 6;
					}
				} else if(sequence == 3) {
					if(!mzone[1][5]) {
						hovered_controler = 0;
						hovered_location = LOCATION_MZONE;
						hovered_sequence = 6;
					} else {
						hovered_controler = 1;
						hovered_location = LOCATION_MZONE;
						hovered_sequence = 5;
					}
				}
			} else if(boardy >= matManager.vFieldMzone[1][0][2].Pos.Y && boardy <= matManager.vFieldMzone[1][0][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = 4 - sequence;
			} else if(boardy >= matManager.vFieldSzone[1][0][rule][2].Pos.Y && boardy < matManager.vFieldSzone[1][0][rule][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 4 - sequence;
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
	int offset = mainGame->gameConf.resize_popup_menu ? ((mainGame->yScale >= 0.666) ? 21 * mainGame->yScale : 14) : 21;
	if(flag & COMMAND_ACTIVATE) {
		mainGame->btnActivate->setVisible(true);
		mainGame->btnActivate->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnActivate->setVisible(false);
	if(flag & COMMAND_SUMMON) {
		mainGame->btnSummon->setVisible(true);
		mainGame->btnSummon->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnSummon->setVisible(false);
	if(flag & COMMAND_SPSUMMON) {
		mainGame->btnSPSummon->setVisible(true);
		mainGame->btnSPSummon->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnSPSummon->setVisible(false);
	if(flag & COMMAND_MSET) {
		mainGame->btnMSet->setVisible(true);
		mainGame->btnMSet->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnMSet->setVisible(false);
	if(flag & COMMAND_SSET) {
		if(!(clicked_card->type & TYPE_MONSTER))
			mainGame->btnSSet->setText(dataManager.GetSysString(1153));
		else
			mainGame->btnSSet->setText(dataManager.GetSysString(1159));
		mainGame->btnSSet->setVisible(true);
		mainGame->btnSSet->setRelativePosition(position2di(1, height));
		height += offset;
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
		height += offset;
	} else mainGame->btnRepos->setVisible(false);
	if(flag & COMMAND_ATTACK) {
		mainGame->btnAttack->setVisible(true);
		mainGame->btnAttack->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnAttack->setVisible(false);
	if(flag & COMMAND_LIST) {
		mainGame->btnShowList->setVisible(true);
		mainGame->btnShowList->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnShowList->setVisible(false);
	if(flag & COMMAND_OPERATION) {
		mainGame->btnOperation->setVisible(true);
		mainGame->btnOperation->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnOperation->setVisible(false);
	if(flag & COMMAND_RESET) {
		mainGame->btnReset->setVisible(true);
		mainGame->btnReset->setRelativePosition(position2di(1, height));
		height += offset;
	} else mainGame->btnReset->setVisible(false);
	panel = mainGame->wCmdMenu;
	mainGame->wCmdMenu->setVisible(true);
	if(mainGame->gameConf.resize_popup_menu)
		mainGame->wCmdMenu->setRelativePosition(mainGame->Resize(x - 20, y - 20, x + 80, y - 20, 0, -height, 0, 0));
	else
		mainGame->wCmdMenu->setRelativePosition(mainGame->Resize(x, y, x, y, -20, -(20 + height), 80, -20));
}
void ClientField::UpdateChainButtons() {
	if(mainGame->btnChainAlways->isVisible()) {
		mainGame->btnChainIgnore->setPressed(mainGame->ignore_chain);
		mainGame->btnChainAlways->setPressed(mainGame->always_chain);
		mainGame->btnChainWhenAvail->setPressed(mainGame->chain_when_avail);
	}
}
void ClientField::ShowCancelOrFinishButton(int buttonOp) {
	if (!mainGame->gameConf.hide_hint_button && !mainGame->dInfo.isReplay) {
		switch (buttonOp) {
		case 1:
			mainGame->btnCancelOrFinish->setText(dataManager.GetSysString(1295));
			mainGame->btnCancelOrFinish->setVisible(true);
			break;
		case 2:
			mainGame->btnCancelOrFinish->setText(dataManager.GetSysString(1296));
			mainGame->btnCancelOrFinish->setVisible(true);
			break;
		case 0:
		default:
			mainGame->btnCancelOrFinish->setVisible(false);
			break;
		}
	} else {
		mainGame->btnCancelOrFinish->setVisible(false);
	}
}
void ClientField::SetShowMark(ClientCard* pcard, bool enable) {
	if(pcard->equipTarget)
		pcard->equipTarget->is_showequip = enable;
	for(auto cit = pcard->equipped.begin(); cit != pcard->equipped.end(); ++cit)
		(*cit)->is_showequip = enable;
	for(auto cit = pcard->cardTarget.begin(); cit != pcard->cardTarget.end(); ++cit)
		(*cit)->is_showtarget = enable;
	for(auto cit = pcard->ownerTarget.begin(); cit != pcard->ownerTarget.end(); ++cit)
		(*cit)->is_showtarget = enable;
	for(auto chit = chains.begin(); chit != chains.end(); ++chit) {
		if(pcard == chit->chain_card) {
			for(auto tgit = chit->target.begin(); tgit != chit->target.end(); ++tgit)
				(*tgit)->is_showchaintarget = enable;
		}
		if(chit->target.find(pcard) != chit->target.end())
			chit->chain_card->is_showchaintarget = enable;
	}
}
void ClientField::ShowCardInfoInList(ClientCard* pcard, irr::gui::IGUIElement* element, irr::gui::IGUIElement* parent) {
	std::wstring str(L"");
	if(pcard->code) {
		str.append(dataManager.GetName(pcard->code));
	}
	if((pcard->status & STATUS_PROC_COMPLETE)
		&& (pcard->type & (TYPE_RITUAL | TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK | TYPE_SPSUMMON)))
		str.append(L"\n").append(dataManager.GetSysString(224));
	for(size_t i = 0; i < chains.size(); ++i) {
		wchar_t formatBuffer[2048];
		auto chit = chains[i];
		if(pcard == chit.chain_card) {
			myswprintf(formatBuffer, dataManager.GetSysString(216), i + 1);
			str.append(L"\n").append(formatBuffer);
		}
		if(chit.target.find(pcard) != chit.target.end()) {
			myswprintf(formatBuffer, dataManager.GetSysString(217), i + 1, dataManager.GetName(chit.chain_card->code));
			str.append(L"\n").append(formatBuffer);
		}
	}
	if(str.length() > 0) {
		parent->addChild(mainGame->stCardListTip);
		irr::core::rect<s32> ePos = element->getRelativePosition();
		s32 x = (ePos.UpperLeftCorner.X + ePos.LowerRightCorner.X) / 2;
		s32 y = ePos.LowerRightCorner.Y;
		mainGame->SetStaticText(mainGame->stCardListTip, 320, mainGame->guiFont, str.c_str());
		irr::core::dimension2d<unsigned int> dTip = mainGame->guiFont->getDimension(mainGame->stCardListTip->getText()) + irr::core::dimension2d<unsigned int>(10, 10);
		s32 w = dTip.Width / 2;
		if(x - w < 10)
			x = w + 10;
		if(x + w > 670)
			x = 670 - w;
		mainGame->stCardListTip->setRelativePosition(recti(x - w, y - 10, x + w, y - 10 + dTip.Height));
		mainGame->stCardListTip->setVisible(true);
	}
}
void ClientField::SetResponseSelectedCards() const {
	unsigned char respbuf[64];
	respbuf[0] = selected_cards.size();
	for (size_t i = 0; i < selected_cards.size(); ++i)
		respbuf[i + 1] = selected_cards[i]->select_seq;
	DuelClient::SetResponseB(respbuf, selected_cards.size() + 1);
}
void ClientField::SetResponseSelectedOption() const {
	if(mainGame->dInfo.curMsg == MSG_SELECT_OPTION) {
		DuelClient::SetResponseI(selected_option);
	} else {
		int index = 0;
		while(activatable_cards[index] != command_card || activatable_descs[index].first != select_options[selected_option]) index++;
		if(mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
			DuelClient::SetResponseI((index << 16) + 5);
		} else if(mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
			DuelClient::SetResponseI(index << 16);
		} else {
			DuelClient::SetResponseI(index);
		}
	}
	mainGame->HideElement(mainGame->wOptions, true);
}
void ClientField::CancelOrFinish() {
	switch(mainGame->dInfo.curMsg) {
	case MSG_WAITING: {
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		break;
	}
	case MSG_SELECT_BATTLECMD: {
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		if(mainGame->wOptions->isVisible()) {
			mainGame->HideElement(mainGame->wOptions);
			ShowCancelOrFinishButton(0);
		}
		break;
	}
	case MSG_SELECT_IDLECMD: {
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		if(mainGame->wOptions->isVisible()) {
			mainGame->HideElement(mainGame->wOptions);
			ShowCancelOrFinishButton(0);
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
				ShowCancelOrFinishButton(0);
				if(mainGame->wCardSelect->isVisible())
					mainGame->HideElement(mainGame->wCardSelect, true);
				else
					DuelClient::SendResponse();
			}
		}
		if(mainGame->wQuery->isVisible()) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wQuery, true);
			break;
		}
		if(select_ready) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			if(mainGame->wCardSelect->isVisible())
				mainGame->HideElement(mainGame->wCardSelect, true);
			else
				DuelClient::SendResponse();
		}
		break;
	}
	case MSG_SELECT_UNSELECT_CARD: {
        if (select_cancelable) {
            DuelClient::SetResponseI(-1);
            ShowCancelOrFinishButton(0);
            if (mainGame->wCardSelect->isVisible())
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
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wQuery, true);
			break;
		}
		break;
	}
	case MSG_SELECT_SUM: {
		if(mainGame->wQuery->isVisible()) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
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
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wQuery, true);
		} else {
			mainGame->PopupElement(mainGame->wQuery);
			ShowCancelOrFinishButton(0);
		}
		if(mainGame->wOptions->isVisible()) {
			DuelClient::SetResponseI(-1);
			ShowCancelOrFinishButton(0);
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
}
}
