#include <algorithm>
#include "game_config.h"
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
#include "progressivebuffer.h"
#include "utils_gui.h"
#include "sound_manager.h"
#include "CGUIImageButton/CGUIImageButton.h"
#include "CGUITTFont/CGUITTFont.h"
#include "custom_skin_enum.h"
#ifdef __ANDROID__
#include <android_native_app_glue.h>
#include "Android/porting_android.h"
#endif
#include <IrrlichtDevice.h>
#include <IGUIEnvironment.h>
#include <IGUIWindow.h>
#include <IGUIStaticText.h>
#include <IGUIEditBox.h>
#include <IGUIComboBox.h>
#include <IGUIListBox.h>
#include <IGUICheckBox.h>
#include <IGUIContextMenu.h>
#include <IGUITabControl.h>
#include <IGUIScrollBar.h>

namespace ygo {

std::string showing_repo = "";

bool ClientField::OnEvent(const irr::SEvent& event) {
	bool stopPropagation = false;
	if(OnCommonEvent(event, stopPropagation))
		return stopPropagation;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		int id = event.GUIEvent.Caller->getID();
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
				if(mainGame->dInfo.isReplay)
					ReplayMode::SwapField();
				else if (mainGame->dInfo.player_type == 7)
					DuelClient::SwapField();
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
				mainGame->saveReplay = 1;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				mainGame->saveReplay = 0;
				mainGame->HideElement(mainGame->wReplaySave);
				mainGame->replaySignal.Set();
				break;
			}
			case BUTTON_LEAVE_GAME: {
				if(mainGame->dInfo.isSingleMode) {
					SingleMode::singleSignal.SetNoWait(true);
					SingleMode::StopPlay(false);
					break;
				}
				if(mainGame->dInfo.player_type == 7) {
					DuelClient::StopClient();
					mainGame->dInfo.isInDuel = false;
					mainGame->dInfo.isStarted = false;
					gSoundManager->StopSounds();
					mainGame->device->setEventReceiver(&mainGame->menuHandler);
					mainGame->mTopMenu->setVisible(true);
					mainGame->stTip->setVisible(false);
					mainGame->stHintMsg->setVisible(false);
					mainGame->wCardImg->setVisible(false);
					mainGame->wInfos->setVisible(false);
					mainGame->wPhase->setVisible(false);
					mainGame->btnLeaveGame->setVisible(false);
					mainGame->btnSpectatorSwap->setVisible(false);
					mainGame->wChat->setVisible(false);
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					if(mainGame->isHostingOnline) {
						mainGame->ShowElement(mainGame->wRoomListPlaceholder);
					} else {
						mainGame->ShowElement(mainGame->wLanWindow);
					}
					mainGame->SetMessageWindow();
					if(exit_on_return)
						mainGame->device->closeDevice();
				} else {
					DuelClient::SendPacketToServer(CTOS_SURRENDER);
				}
				break;
			}
			case BUTTON_CHAIN_IGNORE: {
				mainGame->ignore_chain = mainGame->btnChainIgnore->isPressed();
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CHAIN_ALWAYS: {
				mainGame->always_chain = mainGame->btnChainAlways->isPressed();
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CHAIN_WHENAVAIL: {
				mainGame->chain_when_avail = mainGame->btnChainWhenAvail->isPressed();
				mainGame->always_chain = false;
				mainGame->ignore_chain = false;
				UpdateChainButtons();
				break;
			}
			case BUTTON_CANCEL_OR_FINISH: {
				CancelOrFinish();
				break;
			}
			case BUTTON_RESTART_SINGLE: {
				if(mainGame->dInfo.isSingleMode)
					SingleMode::Restart();
				break;
			}
			case BUTTON_MSG_OK: {
				mainGame->HideElement(mainGame->wMessage);
				mainGame->actionSignal.Set();
				break;
			}
			case BUTTON_YES: {
				if(mainGame->dInfo.checkRematch) {
					mainGame->HideElement(mainGame->wQuery);
					CTOS_RematchResponse crr;
					crr.rematch = true;
					DuelClient::SendPacketToServer(CTOS_REMATCH_RESPONSE, crr);
					break;
				}
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
				case MSG_SELECT_TRIBUTE: {
					mainGame->HideElement(mainGame->wQuery);
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
				if(mainGame->dInfo.checkRematch) {
					mainGame->HideElement(mainGame->wQuery);
					CTOS_RematchResponse crr;
					crr.rematch = false;
					DuelClient::SendPacketToServer(CTOS_REMATCH_RESPONSE, crr);
					break;
				}
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
				case MSG_SELECT_TRIBUTE: {
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
				selected_option--;
				mainGame->btnOptionn->setVisible(true);
				if(selected_option == 0)
					mainGame->btnOptionp->setVisible(false);
				mainGame->stOptions->setText(gDataManager->GetDesc(select_options[selected_option], mainGame->dInfo.compat_mode).c_str());
				break;
			}
			case BUTTON_OPTION_NEXT: {
				selected_option++;
				mainGame->btnOptionp->setVisible(true);
				if(selected_option == select_options.size() - 1)
					mainGame->btnOptionn->setVisible(false);
				mainGame->stOptions->setText(gDataManager->GetDesc(select_options[selected_option], mainGame->dInfo.compat_mode).c_str());
				break;
			}
			case BUTTON_OPTION_0:
			case BUTTON_OPTION_1:
			case BUTTON_OPTION_2:
			case BUTTON_OPTION_3:
			case BUTTON_OPTION_4: {
				int step = mainGame->scrOption->isVisible() ? mainGame->scrOption->getPos() : 0;
				selected_option = id - BUTTON_OPTION_0 + step;
				SetResponseSelectedOption();
				break;
			}
			case BUTTON_OPTION_OK: {
				SetResponseSelectedOption();
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
			case BUTTON_CMD_ACTIVATE:
			case BUTTON_CMD_RESET: {
				mainGame->wCmdMenu->setVisible(false);
				ShowCancelOrFinishButton(0);
				if(!list_command) {
					int index = -1;
					select_options.clear();
					for (size_t i = 0; i < activatable_cards.size(); ++i) {
						if (activatable_cards[i] == clicked_card) {
							if(activatable_descs[i].second == EFFECT_CLIENT_MODE_RESOLVE)
								continue;
							if(activatable_descs[i].second == EFFECT_CLIENT_MODE_RESET) {
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
						mainGame->wCardSelect->setText(gDataManager->GetSysString(566).c_str());
						list_command = COMMAND_ACTIVATE;
					} else {
						mainGame->wCardSelect->setText(gDataManager->GetSysString(568).c_str());
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
					case LOCATION_REMOVED: {
						for (size_t i = 0; i < remove[command_controler].size(); ++i)
							if (remove[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(remove[command_controler][i]);
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
					mainGame->wCardSelect->setText(gDataManager->GetSysString(509).c_str());
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
				switch(command_location) {
				case LOCATION_DECK: {
					for(int32 i = (int32)deck[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(deck[command_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1000), deck[command_controler].size()).c_str());
					break;
				}
				case LOCATION_MZONE: {
					ClientCard* pcard = mzone[command_controler][command_sequence];
					for(int32 i = 0; i < (int32)pcard->overlayed.size(); ++i)
						selectable_cards.push_back(pcard->overlayed[i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1007), pcard->overlayed.size()).c_str());
					break;
				}
				case LOCATION_SZONE: {
					ClientCard* pcard = szone[command_controler][command_sequence];
					for (int32 i = 0; i < (int32)pcard->overlayed.size(); ++i)
						selectable_cards.push_back(pcard->overlayed[i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1007), pcard->overlayed.size()).c_str());
					break;
				}
				case LOCATION_GRAVE: {
					for(int32 i = (int32)grave[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[command_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1004), grave[command_controler].size()).c_str());
					break;
				}
				case LOCATION_REMOVED: {
					for(int32 i = (int32)remove[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(remove[command_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1005), remove[command_controler].size()).c_str());
					break;
				}
				case LOCATION_EXTRA: {
					for(int32 i = (int32)extra[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(extra[command_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1006), extra[command_controler].size()).c_str());
					break;
				}
				}
				list_command = COMMAND_LIST;
				std::sort(selectable_cards.begin(), selectable_cards.end(), ClientCard::client_card_sort);
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
				mainGame->stCardListTip->setVisible(false);
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
								if(activatable_descs[i].second == EFFECT_CLIENT_MODE_RESOLVE) {
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
							mainGame->stOptions->setText(gDataManager->GetDesc(select_options[0], mainGame->dInfo.compat_mode).c_str());
							selected_option = 0;
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
						auto it = std::find(selected_cards.begin(), selected_cards.end(), command_card);
						selected_cards.erase(it);
						if(command_card->controler)
							mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else {
						command_card->is_selected = true;
						mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
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
							mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else {
						command_card->is_selected = true;
						mainGame->stCardPos[id - BUTTON_CARD_0]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
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
					if (std::find(must_select_cards.begin(), must_select_cards.end(), command_card) != must_select_cards.end())
						break;
					if (command_card->is_selected) {
						command_card->is_selected = false;
						auto it = std::find(selected_cards.begin(), selected_cards.end(), command_card);
						selected_cards.erase(it);
					} else
						selected_cards.push_back(command_card);
					ShowSelectSum();
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
								mainGame->stCardPos[i]->setText(fmt::to_wstring(sort_list[offset + i]).c_str());
							} else mainGame->stCardPos[i]->setText(L"");
						}
					} else {
						select_min++;
						sort_list[sel_seq] = select_min;
						mainGame->stCardPos[id - BUTTON_CARD_0]->setText(fmt::to_wstring(select_min).c_str());
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
				// Space and Return trigger the last focused button so the hide animation can be triggered again
				// even if it's already hidden along with its child OK button
				auto HideCardSelectIfVisible = [](bool setAction = false) {
					if (mainGame->wCardSelect->isVisible())
						mainGame->HideElement(mainGame->wCardSelect, setAction);
				};
 				mainGame->stCardListTip->setVisible(false);
				if(mainGame->dInfo.isReplay) {
					HideCardSelectIfVisible();
					break;
				}
				if(mainGame->dInfo.curMsg == MSG_SELECT_CARD || mainGame->dInfo.curMsg == MSG_SELECT_SUM) {
					if(select_ready) {
						SetResponseSelectedCards();
						ShowCancelOrFinishButton(0);
						HideCardSelectIfVisible(true);
					}
					break;
				} else if(mainGame->dInfo.curMsg == MSG_CONFIRM_CARDS) {
					HideCardSelectIfVisible();
					mainGame->actionSignal.Set();
					break;
				} else if(mainGame->dInfo.curMsg == MSG_SELECT_UNSELECT_CARD){
					DuelClient::SetResponseI(-1);
					ShowCancelOrFinishButton(0);
					HideCardSelectIfVisible(true);
				} else {
					HideCardSelectIfVisible();
					if (mainGame->dInfo.curMsg == MSG_SELECT_CHAIN && !chain_forced)
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
			case CHECKBOX_CHAIN_BUTTONS: {
				if(mainGame->dInfo.isStarted && !mainGame->dInfo.isReplay && mainGame->dInfo.player_type < 7) {
					const bool checked = !mainGame->tabSettings.chkHideChainButtons->isChecked();
					mainGame->btnChainIgnore->setVisible(checked);
					mainGame->btnChainAlways->setVisible(checked);
					mainGame->btnChainWhenAvail->setVisible(checked);
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
					mainGame->btnOption[i]->setText(gDataManager->GetDesc(select_options[i + step], mainGame->dInfo.compat_mode).c_str());
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
						mainGame->imageLoading[mainGame->btnCardSelect[i]] = selectable_cards[i + pos]->code;
					else if(conti_selecting)
						mainGame->imageLoading[mainGame->btnCardSelect[i]] = selectable_cards[i + pos]->chain_code;
					else
						mainGame->btnCardSelect[i]->setImage(mainGame->imageManager.tCover[0]);
					mainGame->btnCardSelect[i]->setRelativePosition(mainGame->Scale(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					// text
					std::wstring text = L"";
					if(sort_list.size()) {
						if(sort_list[pos + i] > 0)
							text = fmt::to_wstring(sort_list[pos + i]);
					} else {
						if(conti_selecting)
							text = DataManager::unknown_string;
						else if(selectable_cards[i + pos]->location == LOCATION_OVERLAY) {
							text = fmt::format(L"{}[{}]({})", gDataManager->FormatLocation(selectable_cards[i + pos]->overlayTarget->location, selectable_cards[i + pos]->overlayTarget->sequence),
								selectable_cards[i + pos]->overlayTarget->sequence + 1, selectable_cards[i + pos]->sequence + 1);
						} else if(selectable_cards[i]->location != 0) {
							text = fmt::format(L"{}[{}]", gDataManager->FormatLocation(selectable_cards[i + pos]->location, selectable_cards[i + pos]->sequence),
								selectable_cards[i + pos]->sequence + 1);
						}
					}
					mainGame->stCardPos[i]->setText(text.c_str());
					// color
					if(conti_selecting)
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					else if(selectable_cards[i + pos]->location == LOCATION_OVERLAY) {
						if(selectable_cards[i + pos]->owner != selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_OVERLAY_TEXT_VAL);
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
						else if(selectable_cards[i + pos]->overlayTarget->controler)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else if(selectable_cards[i + pos]->location == LOCATION_DECK || selectable_cards[i + pos]->location == LOCATION_EXTRA || selectable_cards[i + pos]->location == LOCATION_REMOVED) {
						if(selectable_cards[i + pos]->position & POS_FACEDOWN)
							mainGame->stCardPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_SET_TEXT_VAL);
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
						else if(selectable_cards[i + pos]->controler)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else {
						if(selectable_cards[i + pos]->is_selected)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
						else if(selectable_cards[i + pos]->controler)
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
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
						mainGame->imageLoading[mainGame->btnCardDisplay[i]] = display_cards[i + pos]->code;
					else
						mainGame->btnCardDisplay[i]->setImage(mainGame->imageManager.tCover[0]);
					mainGame->btnCardDisplay[i]->setRelativePosition(mainGame->Scale(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					std::wstring text;
					if(display_cards[i + pos]->location == LOCATION_OVERLAY) {
						text = fmt::format(L"{}[{}]({})", gDataManager->FormatLocation(display_cards[i + pos]->overlayTarget->location, display_cards[i + pos]->overlayTarget->sequence),
							display_cards[i + pos]->overlayTarget->sequence + 1, display_cards[i + pos]->sequence + 1);
					} else {
						text = fmt::format(L"{}[{}]", gDataManager->FormatLocation(display_cards[i + pos]->location, display_cards[i + pos]->sequence),
							display_cards[i + pos]->sequence + 1);
					}
					mainGame->stDisplayPos[i]->setText(text.c_str());
					if(display_cards[i + pos]->location == LOCATION_OVERLAY) {
						if(display_cards[i + pos]->owner != display_cards[i + pos]->overlayTarget->controler)
							mainGame->stDisplayPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_OVERLAY_TEXT_VAL);
						// BackgroundColor: controller of the xyz monster
						if(display_cards[i + pos]->overlayTarget->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else if(display_cards[i + pos]->location == LOCATION_EXTRA || display_cards[i + pos]->location == LOCATION_REMOVED) {
						if(display_cards[i + pos]->position & POS_FACEDOWN)
							mainGame->stDisplayPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_SET_TEXT_VAL);
						if(display_cards[i + pos]->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
					} else {
						if(display_cards[i + pos]->controler)
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
						else
							mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
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
				UpdateDeclarableList();
				break;
			}
			case EDITBOX_REPLAY_NAME: {
				mainGame->ValidateName(event.GUIEvent.Caller);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_ANCARD: {
				UpdateDeclarableList(true);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_HOVERED: {
			if(id >= BUTTON_CARD_0 && id <= BUTTON_CARD_4) {
				int pos = mainGame->scrCardList->getPos() / 10;
				ClientCard* mcard = selectable_cards[id - BUTTON_CARD_0 + pos];
				if(mcard) {
					SetShowMark(mcard, true);
					ShowCardInfoInList(mcard, mainGame->btnCardSelect[id - BUTTON_CARD_0], mainGame->wCardSelect);
					if(mcard->code) {
						mainGame->ShowCardInfo(mcard->code);
					} else {
						if(mcard->cover)
							mainGame->ShowCardInfo(mcard->cover, false, ImageManager::imgType::COVER);
						else
							mainGame->ClearCardInfo(mcard->controler);
					}
				}
			}
			if(id >= BUTTON_DISPLAY_0 && id <= BUTTON_DISPLAY_4) {
				int pos = mainGame->scrDisplayList->getPos() / 10;
				ClientCard* mcard = display_cards[id - BUTTON_DISPLAY_0 + pos];
				if(mcard) {
					SetShowMark(mcard, true);
					ShowCardInfoInList(mcard, mainGame->btnCardDisplay[id - BUTTON_DISPLAY_0], mainGame->wCardDisplay);
					if(mcard->code) {
						mainGame->ShowCardInfo(mcard->code);
					} else {
						if(mcard->cover)
							mainGame->ShowCardInfo(mcard->cover, false, ImageManager::imgType::COVER);
						else
							mainGame->ClearCardInfo(mcard->controler);
					}
				}
			}
			if(id == TEXT_CARD_LIST_TIP) {
				mainGame->stCardListTip->setVisible(true);
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_LEFT: {
			if(mainGame->stCardListTip->isVisible()) {
				if(id >= BUTTON_CARD_0 && id <= BUTTON_CARD_4) {
					int pos = mainGame->scrCardList->getPos() / 10;
					ClientCard* mcard = selectable_cards[id - BUTTON_CARD_0 + pos];
					if(mcard)
						SetShowMark(mcard, false);
					mainGame->stCardListTip->setVisible(false);
				}
				if(id >= BUTTON_DISPLAY_0 && id <= BUTTON_DISPLAY_4) {
					int pos = mainGame->scrDisplayList->getPos() / 10;
					ClientCard* mcard = display_cards[id - BUTTON_DISPLAY_0 + pos];
					if(mcard)
						SetShowMark(mcard, false);
					mainGame->stCardListTip->setVisible(false);
				}
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
			if(!mainGame->dInfo.isInDuel)
				break;
			hovered_location = 0;
			irr::core::vector2di pos = mainGame->Resize(event.MouseInput.X, event.MouseInput.Y, true);
			irr::core::vector2di mousepos(event.MouseInput.X, event.MouseInput.Y);
			irr::s32 x = pos.X;
			irr::s32 y = pos.Y;
			if(x < 300)
				break;
			if(mainGame->btnChainAlways->isPressed())
				mainGame->always_chain = true;
			else
				mainGame->always_chain = false;
			mainGame->ignore_chain = false;
			mainGame->chain_when_avail = false;
			//UpdateChainButtons();
			if(mainGame->wCmdMenu->isVisible() && !mainGame->wCmdMenu->getRelativePosition().isPointInside(mousepos))
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
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1000), deck[hovered_controler].size()).c_str());
					break;
				}
				case LOCATION_MZONE: {
					if(!clicked_card || clicked_card->overlayed.size() == 0)
						break;
					for(int32 i = 0; i < (int32)clicked_card->overlayed.size(); ++i)
						selectable_cards.push_back(clicked_card->overlayed[i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1007), clicked_card->overlayed.size()).c_str());
					break;
				}
				case LOCATION_GRAVE: {
					if(grave[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)grave[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1004), grave[hovered_controler].size()).c_str());
					break;
				}
				case LOCATION_REMOVED: {
					if(remove[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)remove[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(remove[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1005), remove[hovered_controler].size()).c_str());
					break;
				}
				case LOCATION_EXTRA: {
					if(extra[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)extra[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(extra[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1006), extra[hovered_controler].size()).c_str());
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
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1007), clicked_card->overlayed.size()).c_str());
					break;
				}
				case LOCATION_GRAVE: {
					if(grave[hovered_controler].size() == 0)
						break;
					for(int32 i = (int32)grave[hovered_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1004), grave[hovered_controler].size()).c_str());
					break;
				}
				case LOCATION_REMOVED: {
					if (remove[hovered_controler].size() == 0)
						break;
					for (int32 i = (int32)remove[hovered_controler].size() - 1; i >= 0; --i)
						selectable_cards.push_back(remove[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1005), remove[hovered_controler].size()).c_str());
					break;
				}
				case LOCATION_EXTRA: {
					if (extra[hovered_controler].size() == 0)
						break;
					for (int32 i = (int32)extra[hovered_controler].size() - 1; i >= 0; --i)
						selectable_cards.push_back(extra[hovered_controler][i]);
					mainGame->wCardSelect->setText(fmt::format(L"{}({})", gDataManager->GetSysString(1006), extra[hovered_controler].size()).c_str());
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
					auto it = std::find(selected_cards.begin(), selected_cards.end(), clicked_card);
					selected_cards.erase(it);
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
					mainGame->stHintMsg->setText(fmt::sprintf(gDataManager->GetSysString(204), select_counter_count, gDataManager->GetCounterName(select_counter_type)).c_str());
				}
				break;
			}
			case MSG_SELECT_SUM: {
				if (!clicked_card || !clicked_card->is_selectable || (std::find(must_select_cards.begin(), must_select_cards.end(), clicked_card) != must_select_cards.end()))
					break;
				if (clicked_card->is_selected) {
					clicked_card->is_selected = false;
					auto it = std::find(selected_cards.begin(), selected_cards.end(), clicked_card);
					selected_cards.erase(it);
				} else
					selected_cards.push_back(clicked_card);
				ShowSelectSum();
				break;
			}
			}
			break;
		}
		case irr::EMIE_RMOUSE_LEFT_UP: {
			if(mainGame->dInfo.isReplay)
				break;
			auto x = event.MouseInput.X;
			auto y = event.MouseInput.Y;
			irr::core::vector2di pos(x, y);
			irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
			if(event.MouseInput.X > 300) {
				if (mainGame->btnChainIgnore->isPressed())
					mainGame->ignore_chain = true;
				else
					mainGame->ignore_chain = false;
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				//UpdateChainButtons();
			}
			mainGame->wCmdMenu->setVisible(false);
			if(mainGame->fadingList.size())
				break;
			CancelOrFinish();
			break;
		}
		case irr::EMIE_MOUSE_MOVED: {
			if(!mainGame->dInfo.isInDuel)
				break;
			bool should_show_tip = false;
			irr::core::vector2di pos = mainGame->Resize(event.MouseInput.X, event.MouseInput.Y, true);
			irr::core::vector2di mousepos(event.MouseInput.X, event.MouseInput.Y);
			irr::s32 x = pos.X;
			irr::s32 y = pos.Y;
			if(x < 300) {
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
					}
				} else if(hovered_location == LOCATION_EXTRA) {
					if(extra[hovered_controler].size()) {
						mcard = extra[hovered_controler].back();
					}
				} else if(hovered_location == LOCATION_DECK) {
					if(deck[hovered_controler].size())
						mcard = deck[hovered_controler].back();
				} else if(hovered_location == LOCATION_SKILL) {
					mcard = skills[hovered_controler];
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
					mainGame->stTip->setRelativePosition(irr::core::vector2di(mousepos.X - tpos.getWidth() - mainGame->Scale(10), mcard ? mousepos.Y - tpos.getHeight() - mainGame->Scale(10) : y + mainGame->Scale(10)));
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
						if(mcard->location & (0xe|0x400)) {
							std::wstring str(gDataManager->GetName(mcard->code));
							if(mcard->type & TYPE_MONSTER) {
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)
										&& wcscmp(gDataManager->GetName(mcard->code).c_str(), gDataManager->GetName(mcard->alias).c_str())) {
									str.append(fmt::format(L"\n({})",gDataManager->GetName(mcard->alias)));
								}
								if (mcard->type & TYPE_LINK) {
									str.append(fmt::format(L"\n{}/Link {}\n{}/{}", mcard->atkstring, mcard->link, gDataManager->FormatRace(mcard->race),
										gDataManager->FormatAttribute(mcard->attribute)));
								} else {
									str.append(fmt::format(L"\n{}/{}", mcard->atkstring, mcard->defstring));
									if(mcard->rank && mcard->level)
										str.append(fmt::format(L"\n\u2606{}/\u2605{} {}/{}", mcard->level, mcard->rank, gDataManager->FormatRace(mcard->race), gDataManager->FormatAttribute(mcard->attribute)));
									else {
										str.append(fmt::format(L"\n{}{} {}/{}", (mcard->level ? L"\u2605" : L"\u2606"), (mcard->level ? mcard->level : mcard->rank), gDataManager->FormatRace(mcard->race), gDataManager->FormatAttribute(mcard->attribute)));
									}
								}
								if(mcard->location == LOCATION_HAND && (mcard->type & TYPE_PENDULUM)) {
									str.append(fmt::format(L"\n{}/{}", mcard->lscale, mcard->rscale));
								}
							} else {
								if(mcard->alias && (mcard->alias < mcard->code - 10 || mcard->alias > mcard->code + 10)) {
									str.append(fmt::format(L"\n({})", gDataManager->GetName(mcard->alias)));
								}
								if(mcard->location == LOCATION_SZONE && mcard->lscale) {
									str.append(fmt::format(L"\n{}/{}", mcard->lscale, mcard->rscale));
								}
							}
							for(auto ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
								str.append(fmt::format(L"\n[{}]: {}", gDataManager->GetCounterName(ctit->first), ctit->second));
							}
							if(mcard->cHint && mcard->chValue && (mcard->location & LOCATION_ONFIELD)) {
								if(mcard->cHint == CHINT_TURN)
									str.append(fmt::format(L"\n{}{}", gDataManager->GetSysString(211), mcard->chValue));
								else if(mcard->cHint == CHINT_CARD)
									str.append(fmt::format(L"\n{}{}", gDataManager->GetSysString(212), gDataManager->GetName(mcard->chValue)));
								else if(mcard->cHint == CHINT_RACE)
									str.append(fmt::format(L"\n{}{}", gDataManager->GetSysString(213), gDataManager->FormatRace(mcard->chValue)));
								else if(mcard->cHint == CHINT_ATTRIBUTE)
									str.append(fmt::format(L"\n{}{}", gDataManager->GetSysString(214), gDataManager->FormatAttribute(mcard->chValue)));
								else if(mcard->cHint == CHINT_NUMBER)
									str.append(fmt::format(L"\n{}{}", gDataManager->GetSysString(215), mcard->chValue));
							}
							for(auto iter = mcard->desc_hints.begin(); iter != mcard->desc_hints.end(); ++iter) {
								str.append(fmt::format(L"\n*{}", gDataManager->GetDesc(iter->first, mainGame->dInfo.compat_mode)));
							}
							should_show_tip = true;
							irr::core::dimension2d<unsigned int> dtip = mainGame->textFont->getDimension(str.c_str()) + mainGame->Scale(irr::core::dimension2d<unsigned int>(10, 10));
							mainGame->stTip->setRelativePosition(irr::core::recti(mousepos.X - mainGame->Scale(10) - dtip.Width, mousepos.Y - mainGame->Scale(10) - dtip.Height, mousepos.X - mainGame->Scale(10), mousepos.Y - mainGame->Scale(10)));
							mainGame->stTip->setText(str.c_str());
						}
					} else {
						should_show_tip = false;
						if(mcard->cover)
							mainGame->ShowCardInfo(mcard->cover, false, ImageManager::imgType::COVER);
						else
							mainGame->ClearCardInfo(mcard->controler);
					}
				}
				hovered_card = mcard;
			}
			if(mplayer != hovered_player) {
				if(mplayer >= 0) {
					std::wstring player_name;
					auto& self = mainGame->dInfo.isTeam1 ? mainGame->dInfo.selfnames : mainGame->dInfo.opponames;
					auto& oppo = mainGame->dInfo.isTeam1 ? mainGame->dInfo.opponames : mainGame->dInfo.selfnames;
					if (mplayer == 0)
						player_name = self[mainGame->dInfo.current_player[mplayer]];
					else
						player_name = oppo[mainGame->dInfo.current_player[mplayer]];
					const auto& player_desc_hints = mainGame->dField.player_desc_hints[mplayer];
					for(auto iter = player_desc_hints.begin(); iter != player_desc_hints.end(); ++iter) {
						player_name.append(fmt::format(L"\n*{}", gDataManager->GetDesc(iter->first, mainGame->dInfo.compat_mode)));
					}
					should_show_tip = true;
					irr::core::dimension2d<unsigned int> dtip = mainGame->textFont->getDimension(player_name.c_str()) + mainGame->Scale(irr::core::dimension2d<unsigned int>(10, 10));
					mainGame->stTip->setRelativePosition(irr::core::recti(mousepos.X - mainGame->Scale(10) - dtip.Width, mousepos.Y + mainGame->Scale(10), mousepos.X - mainGame->Scale(10), mousepos.Y + mainGame->Scale(10) + dtip.Height));
					mainGame->stTip->setText(player_name.c_str());
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
			if(!mainGame->dInfo.isInDuel)
				break;
			if(event.MouseInput.X > 300) {
				mainGame->always_chain = event.MouseInput.isLeftPressed();
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				//UpdateChainButtons();
			}
			break;
		}
		case irr::EMIE_RMOUSE_PRESSED_DOWN: {
			if(!mainGame->dInfo.isInDuel)
				break;
			if(event.MouseInput.X > 300) {
				mainGame->ignore_chain = event.MouseInput.isRightPressed();
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				//UpdateChainButtons();
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
			if(!mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
				mainGame->always_chain = event.KeyInput.PressedDown;
				mainGame->ignore_chain = false;
				mainGame->chain_when_avail = false;
				//UpdateChainButtons();
			}
			break;
		}
		case irr::KEY_KEY_S: {
			if(!mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
				mainGame->ignore_chain = event.KeyInput.PressedDown;
				mainGame->always_chain = false;
				mainGame->chain_when_avail = false;
				//UpdateChainButtons();
			}
			break;
		}
		case irr::KEY_KEY_D: {
			if(!mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
				mainGame->chain_when_avail = event.KeyInput.PressedDown;
				mainGame->always_chain = false;
				mainGame->ignore_chain = false;
				//UpdateChainButtons();
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
			if(!event.KeyInput.PressedDown && !mainGame->dInfo.isReplay && mainGame->dInfo.player_type != 7 && mainGame->dInfo.isInDuel
					&& !mainGame->wCardDisplay->isVisible() && !mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
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
					default: break;
				}
				if(display_cards.size()) {
					mainGame->wCardDisplay->setText(fmt::format(L"{}({})", gDataManager->GetSysString(loc_id), display_cards.size()).c_str());
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
bool ClientField::OnCommonEvent(const irr::SEvent& event, bool& stopPropagation) {
#ifdef __ANDROID__
	if(porting::transformEvent(event, stopPropagation)) {
		return true;
	}
#endif
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		irr::s32 id = event.GUIEvent.Caller->getID();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_ELEMENT_HOVERED: {
			// Set cursor to an I-Beam if hovering over an edit box
			if (event.GUIEvent.Caller->getType() == irr::gui::EGUIET_EDIT_BOX && event.GUIEvent.Caller->isEnabled()) {
				GUIUtils::ChangeCursor(mainGame->device, irr::gui::ECI_IBEAM);
				return true;
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_LEFT: {
			// Set cursor to normal if left an edit box
			if (event.GUIEvent.Caller->getType() == irr::gui::EGUIET_EDIT_BOX) {
				GUIUtils::ChangeCursor(mainGame->device, irr::gui::ECI_NORMAL);
				return true;
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_CLOSED: {
			if(event.GUIEvent.Caller == mainGame->gSettings.window) {
				stopPropagation = true;
				mainGame->HideElement(mainGame->gSettings.window);
				return true;
			}
			break;
		}
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_LOG: {
				mainGame->lstLog->clear();
				mainGame->logParam.clear();
				return true;
			}
			case BUTTON_CLEAR_CHAT: {
				mainGame->lstChat->clear();
				for(int i = 0; i < 8; i++) {
					mainGame->chatMsg[i].clear();
					mainGame->chatType[i] = 0;
					mainGame->chatTiming[i] = 0;
				}
				return true;
			}
			case BUTTON_EXPAND_INFOBOX: {
				mainGame->infosExpanded = mainGame->infosExpanded ? 0 : 1;
				mainGame->btnExpandLog->setText(mainGame->infosExpanded ? gDataManager->GetSysString(2044).c_str() : gDataManager->GetSysString(2043).c_str());
				mainGame->btnExpandChat->setText(mainGame->infosExpanded ? gDataManager->GetSysString(2044).c_str() : gDataManager->GetSysString(2043).c_str());
				mainGame->wInfos->setRelativePosition(mainGame->Resize(1, 275, mainGame->infosExpanded ? 1023 : 301, 639));
				mainGame->lstLog->setRelativePosition(mainGame->Resize(10, 10, mainGame->infosExpanded ? 1012 : 290, 290));
				mainGame->lstChat->setRelativePosition(mainGame->Resize(10, 10, mainGame->infosExpanded ? 1012 : 290, 290));
				return true;
			}
			case BUTTON_REPO_CHANGELOG:	{
				irr::gui::IGUIButton* button = (irr::gui::IGUIButton*)event.GUIEvent.Caller;
				for(auto& repo : mainGame->repoInfoGui) {
					if(repo.second.history_button1 == button || repo.second.history_button2 == button) {
						showing_repo = repo.first;
						mainGame->stCommitLog->setText(mainGame->chkCommitLogExpand->isChecked() ? repo.second.commit_history_full.c_str() : repo.second.commit_history_partial.c_str());
						mainGame->SetCentered(mainGame->wCommitsLog);
						mainGame->PopupElement(mainGame->wCommitsLog);
						break;
					}
				}
				return true;
			}
			case BUTTON_REPO_CHANGELOG_EXIT: {
				mainGame->HideElement(mainGame->wCommitsLog);
				return true;
			}
			case BUTTON_RELOAD_SKIN: {
				mainGame->should_reload_skin = true;
				break;
			}
			case BUTTON_SHOW_SETTINGS: {
				if (!mainGame->gSettings.window->isVisible())
					mainGame->PopupElement(mainGame->gSettings.window);
				mainGame->env->setFocus(mainGame->gSettings.window);
				break;
			}
			case BUTTON_APPLY_RESTART: {
				try {
					gGameConfig->dpi_scale = static_cast<uint32_t>(std::stol(mainGame->gSettings.ebDpiScale->getText())) / 100.0;
					mainGame->restart = true;
					//mainGame->device->closeDevice();
				} catch(...){}
				break;
			}
			case BUTTON_FPS_CAP: {
				try {
					gGameConfig->maxFPS = static_cast<int32_t>(std::stol(mainGame->gSettings.ebFPSCap->getText()));
				} catch (...) {
					mainGame->gSettings.ebFPSCap->setText(fmt::to_wstring(gGameConfig->maxFPS).c_str());
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
				return true;
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
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
			case SCROLL_MUSIC_VOLUME: {
				gGameConfig->musicVolume = static_cast<irr::gui::IGUIScrollBar*>(event.GUIEvent.Caller)->getPos();
				mainGame->tabSettings.scrMusicVolume->setPos(gGameConfig->musicVolume);
				mainGame->gSettings.scrMusicVolume->setPos(gGameConfig->musicVolume);
				gSoundManager->SetMusicVolume(gGameConfig->musicVolume / 100.0);
				return true;
			}
			case SCROLL_SOUND_VOLUME: {
				gGameConfig->soundVolume = static_cast<irr::gui::IGUIScrollBar*>(event.GUIEvent.Caller)->getPos();
				mainGame->tabSettings.scrSoundVolume->setPos(gGameConfig->soundVolume);
				mainGame->gSettings.scrSoundVolume->setPos(gGameConfig->soundVolume);
				gSoundManager->SetSoundVolume(gGameConfig->soundVolume / 100.0);
				return true;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch (id) {
			case CHECKBOX_ENABLE_MUSIC: {
				gGameConfig->enablemusic = static_cast<irr::gui::IGUICheckBox*>(event.GUIEvent.Caller)->isChecked();
				mainGame->tabSettings.chkEnableMusic->setChecked(gGameConfig->enablemusic);
				mainGame->gSettings.chkEnableMusic->setChecked(gGameConfig->enablemusic);
				gSoundManager->EnableMusic(gGameConfig->enablemusic);
				return true;
			}
			case CHECKBOX_ENABLE_SOUND: {
				gGameConfig->enablesound = static_cast<irr::gui::IGUICheckBox*>(event.GUIEvent.Caller)->isChecked();
				mainGame->tabSettings.chkEnableSound->setChecked(gGameConfig->enablesound);
				mainGame->gSettings.chkEnableSound->setChecked(gGameConfig->enablesound);
				gSoundManager->EnableSounds(gGameConfig->enablesound);
				return true;
			}
			case CHECKBOX_QUICK_ANIMATION: {
				gGameConfig->quick_animation = mainGame->tabSettings.chkQuickAnimation->isChecked();
				return true;
			}
			case CHECKBOX_ALTERNATIVE_PHASE_LAYOUT: {
				gGameConfig->alternative_phase_layout = mainGame->tabSettings.chkAlternativePhaseLayout->isChecked();
				return true;
			}
			case CHECKBOX_HIDE_ARCHETYPES: {
				gGameConfig->chkHideSetname = mainGame->gSettings.chkHideSetname->isChecked();
				mainGame->stSetName->setVisible(!gGameConfig->chkHideSetname);
				mainGame->RefreshCardInfoTextPositions();
				return true;
			}
			case CHECKBOX_HIDE_PASSCODE_SCOPE: {
				gGameConfig->hidePasscodeScope = mainGame->gSettings.chkHidePasscodeScope->isChecked();
				mainGame->stPasscodeScope->setVisible(!gGameConfig->hidePasscodeScope);
				mainGame->RefreshCardInfoTextPositions();
				return true;
			}
			case CHECKBOX_SHOW_SCOPE_LABEL: {
				gGameConfig->showScopeLabel = mainGame->gSettings.chkShowScopeLabel->isChecked();
				return true;
			}
			case CHECKBOX_VSYNC: {
				gGameConfig->vsync = mainGame->gSettings.chkVSync->isChecked();
#ifndef __ANDROID__
				mainGame->driver->setVsync(gGameConfig->vsync);
#endif
				return true;
			}
			case CHECKBOX_SHOW_FPS: {
				gGameConfig->showFPS = mainGame->gSettings.chkShowFPS->isChecked();
				mainGame->fpsCounter->setVisible(gGameConfig->showFPS);
				return true;
			}
			case CHECKBOX_DRAW_FIELD_SPELLS: {
				gGameConfig->draw_field_spell = mainGame->gSettings.chkDrawFieldSpells->isChecked();
				return true;
			}
			case CHECKBOX_FILTER_BOT: {
				gGameConfig->filterBot = mainGame->gSettings.chkFilterBot->isChecked();
				mainGame->gBot.Refresh(gGameConfig->filterBot * (mainGame->cbDuelRule->getSelected() + 1), gGameConfig->lastBot);
				return true;
			}
			case CHECKBOX_FULLSCREEN: {
				GUIUtils::ToggleFullscreen(mainGame->device, gGameConfig->fullscreen);
				return true;
			}
			case CHECKBOX_SCALE_BACKGROUND: {
				gGameConfig->scale_background = mainGame->gSettings.chkScaleBackground->isChecked();
				return true;
			}
#ifndef ANDROID
			case CHECKBOX_ACCURATE_BACKGROUND_RESIZE: {
				gGameConfig->accurate_bg_resize = mainGame->gSettings.chkAccurateBackgroundResize->isChecked();
				return true;
			}
#endif
			case BUTTON_REPO_CHANGELOG_EXPAND: {
				auto& repo = mainGame->repoInfoGui[showing_repo];
				mainGame->stCommitLog->setText(mainGame->chkCommitLogExpand->isChecked() ? repo.commit_history_full.c_str() : repo.commit_history_partial.c_str());
				return true;
			}
			case CHECKBOX_SAVE_HAND_TEST_REPLAY: {
				gGameConfig->saveHandTest = mainGame->gSettings.chkSaveHandTest->isChecked();
				return true;
			}
			case CHECKBOX_LOOP_MUSIC: {
				gGameConfig->loopMusic = static_cast<irr::gui::IGUICheckBox*>(event.GUIEvent.Caller)->isChecked();
				break;
			}
#ifdef DISCORD_APP_ID
			case CHECKBOX_DISCORD_INTEGRATION: {
				gGameConfig->discordIntegration = static_cast<irr::gui::IGUICheckBox*>(event.GUIEvent.Caller)->isChecked();
				mainGame->discord.UpdatePresence(gGameConfig->discordIntegration ? DiscordWrapper::INITIALIZE : DiscordWrapper::TERMINATE);
				break;
			}
#endif
			case CHECKBOX_HIDE_HANDS_REPLAY: {
				gGameConfig->hideHandsInReplays = static_cast<irr::gui::IGUICheckBox*>(event.GUIEvent.Caller)->isChecked();
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
				}
				return true;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch(id) {
			case EDITBOX_NUMERIC: {
				std::wstring tmp(event.GUIEvent.Caller->getText());
				if(Utils::KeepOnlyDigits(tmp))
					event.GUIEvent.Caller->setText(tmp.c_str());
				break;
			}
			case EDITBOX_FPS_CAP: {
				std::wstring tmp(event.GUIEvent.Caller->getText());
				if(Utils::KeepOnlyDigits(tmp, true) || tmp.size() > 1) {
					if(tmp.size()>1)
						if(tmp[0] == L'-' && (tmp[1] != L'1' || tmp.size() != 2)) {
							event.GUIEvent.Caller->setText(L"-");
							break;
						}
					event.GUIEvent.Caller->setText(tmp.c_str());
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_TAB_CHANGED: {
			if(event.GUIEvent.Caller == mainGame->wInfos) {
				auto curTab = mainGame->wInfos->getTab(mainGame->wInfos->getActiveTab());
				if((curTab != mainGame->tabLog && curTab != mainGame->tabChat) && mainGame->infosExpanded) {
					if(mainGame->infosExpanded == 1)
						mainGame->wInfos->setRelativePosition(mainGame->Resize(1, 275, 301, 639));
					mainGame->infosExpanded = 2;
				} else if(mainGame->infosExpanded) {
					if(mainGame->infosExpanded == 2)
						mainGame->wInfos->setRelativePosition(mainGame->Resize(1, 275, 1023, 639));
					mainGame->infosExpanded = 1;
				}
				return true;
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_CURRENT_SKIN: {
				auto newskin = Utils::ToPathString(mainGame->gSettings.cbCurrentSkin->getItem(mainGame->gSettings.cbCurrentSkin->getSelected()));
				mainGame->should_reload_skin = newskin != gGameConfig->skin;
				return true;
			}
			case COMBOBOX_CURRENT_LOCALE: {
				mainGame->ApplyLocale(mainGame->gSettings.cbCurrentLocale->getSelected());
				return true;
			}
			case COMBOBOX_CORE_LOG_OUTPUT: {
				int selected = mainGame->gSettings.cbCoreLogOutput->getSelected();
				if (selected < 0) return true;
				gGameConfig->coreLogOutput = mainGame->gSettings.cbCoreLogOutput->getItemData(selected);
				return true;
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
		case irr::KEY_KEY_C: {
			if(event.KeyInput.Control && mainGame->HasFocus(irr::gui::EGUIET_LIST_BOX)) {
				auto focus = static_cast<irr::gui::IGUIListBox*>(mainGame->env->getFocus());
				int sel = focus->getSelected();
				if(sel != -1) {
					mainGame->device->getOSOperator()->copyToClipboard(focus->getListItem(sel));
					return true;
				}
			}
			return false;
		}
		case irr::KEY_KEY_R: {
			if(event.KeyInput.Control) {
				mainGame->should_reload_skin = true;
				return true;
			}
			if(!event.KeyInput.PressedDown && !mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX))
				mainGame->textFont->setTransparency(true);
			return true;
		}
		case irr::KEY_KEY_O: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown) {
				if (mainGame->gSettings.window->isVisible())
					mainGame->HideElement(mainGame->gSettings.window);
				else
					mainGame->PopupElement(mainGame->gSettings.window);
			}
			return true;
		}
		case irr::KEY_ESCAPE: {
			if(!mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX))
				mainGame->device->minimizeWindow();
			return true;
		}
		case irr::KEY_F9: {
			if (!event.KeyInput.PressedDown) {
				gSoundManager->StopMusic();
				gSoundManager->StopSounds();
				gSoundManager->RefreshBGMList();
				gSoundManager->RefreshChantsList();
			}
			return true;
		}
		case irr::KEY_F11: {
			if(!event.KeyInput.PressedDown) {
				GUIUtils::ToggleFullscreen(mainGame->device, gGameConfig->fullscreen);
				mainGame->gSettings.chkFullscreen->setChecked(gGameConfig->fullscreen);
			}
			return true;
		}
		case irr::KEY_F12: {
			if (!event.KeyInput.PressedDown)
				GUIUtils::TakeScreenshot(mainGame->device);
			return true;
		}
		case irr::KEY_KEY_1: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown)
				mainGame->wInfos->setActiveTab(0);
			break;
		}
		case irr::KEY_KEY_2: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown)
				mainGame->wInfos->setActiveTab(1);
			break;
		}
		case irr::KEY_KEY_3: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown)
				mainGame->wInfos->setActiveTab(2);
			break;
		}
		case irr::KEY_KEY_4: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown)
				mainGame->wInfos->setActiveTab(3);
			break;
		}
		case irr::KEY_KEY_5: {
			if (event.KeyInput.Control && !event.KeyInput.PressedDown)
				mainGame->wInfos->setActiveTab(4);
			break;
		}
		default: break;
		}
		break;
	}
	case irr::EET_MOUSE_INPUT_EVENT: {
		auto SimulateMouse = [device=mainGame->device, &event](irr::EMOUSE_INPUT_EVENT type) {
			irr::SEvent simulated = event;
			simulated.MouseInput.Event = type;
			device->postEventFromUser(simulated);
			return true;
		};
		switch (event.MouseInput.Event) {
			case irr::EMIE_MOUSE_WHEEL: {
				irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
				irr::gui::IGUIElement* elem = root->getElementFromPoint({ event.MouseInput.X, event.MouseInput.Y });
				auto checkstatic = [](irr::gui::IGUIElement* elem) -> bool {
					return elem && elem->getType() == irr::gui::EGUIET_STATIC_TEXT;
				};
				auto checkscroll = [&checkstatic](irr::gui::IGUIElement* elem) -> bool {
					return elem && (elem->getType() == irr::gui::EGUIET_SCROLL_BAR) && checkstatic(elem->getParent());
				};
				auto checkbutton = [&checkscroll](irr::gui::IGUIElement* elem) -> bool {
					return elem && (elem->getType() == irr::gui::EGUIET_BUTTON) && checkscroll(elem->getParent());
				};
				if(checkstatic(elem) || checkscroll(elem) || checkbutton(elem)) {
					if(elem->OnEvent(event)) {
						stopPropagation = true;
						return true;
					}
				}
				break;
			}
			default: break;
		}
		if(!gGameConfig->ctrlClickIsRMB || !event.MouseInput.Control)
			break;
		switch(event.MouseInput.Event) {
#define REMAP(TYPE) case irr::EMIE_LMOUSE_##TYPE: return SimulateMouse(irr::EMIE_RMOUSE_##TYPE)
			REMAP(PRESSED_DOWN);
			REMAP(LEFT_UP);
			REMAP(DOUBLE_CLICK);
			REMAP(TRIPLE_CLICK);
#undef REMAP
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
	if(mainGame->dInfo.duel_params & DUEL_3_COLUMNS_FIELD) {
		sfRect = irr::core::recti(509, 504, 796, 600);
		ofRect = irr::core::recti(531+ 46, 135, 800- 46, 191);
	}
	irr::core::vector2di pos(x, y);
	int field = (mainGame->dInfo.duel_field == 3 || mainGame->dInfo.duel_field == 5) ? 0 : 1;
	int speed = (mainGame->dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	if(sfRect.isPointInside(pos)) {
		int hc = hand[0].size();
		int cardSize = 66;
		int cardSpace = 10;
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7 - speed * 2) {
			int left = sfRect.UpperLeftCorner.X + (cardSize + cardSpace) * (6 - speed * 2 - hc) / 2;
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
			if(x >= sfRect.UpperLeftCorner.X + (cardSize + cardSpace) * (5 - speed * 2))
				hovered_sequence = hc - 1;
			else
				hovered_sequence = (x - sfRect.UpperLeftCorner.X) * (hc - 1) / ((cardSize + cardSpace) * (5 - speed * 2));
		}
	} else if(ofRect.isPointInside(pos)) {
		int hc = hand[1].size();
		int cardSize = 39;
		int cardSpace = 7;
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7 - speed * 2) {
			int left = ofRect.UpperLeftCorner.X + (cardSize + cardSpace) * (6 - speed * 2 - hc) / 2;
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
			if(x >= ofRect.UpperLeftCorner.X + (cardSize + cardSpace) * (5 - speed * 2))
				hovered_sequence = 0;
			else
				hovered_sequence = hc - 1 - (x - ofRect.UpperLeftCorner.X) * (hc - 1) / ((cardSize + cardSpace) * (5 - speed * 2));
		}
	} else {
		double screenx = x / 1024.0 * (CAMERA_RIGHT - CAMERA_LEFT) + CAMERA_LEFT;
		double screeny = y / 640.0 * (CAMERA_TOP - CAMERA_BOTTOM) + CAMERA_BOTTOM;
		double angle = FIELD_ANGLE - atan(screeny);
		double vlen = sqrt(1.0 + screeny * screeny);
		double boardx = FIELD_X + FIELD_Z * screenx / vlen / cos(angle);
		double boardy = FIELD_Y - FIELD_Z * tan(angle);
		hovered_location = 0;
		if(boardx >= matManager.vFieldExtra[0][speed][0].Pos.X && boardx <= matManager.vFieldExtra[0][speed][1].Pos.X) {
			if(boardy >= matManager.vFieldExtra[0][speed][0].Pos.Y && boardy <= matManager.vFieldExtra[0][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_EXTRA;
			} else if(boardy >= matManager.vFieldSzone[0][5][field][speed][0].Pos.Y && boardy <= matManager.vFieldSzone[0][5][field][speed][2].Pos.Y) {//field
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(field == 0 && boardy >= matManager.vFieldSzone[0][6][field][speed][0].Pos.Y && boardy <= matManager.vFieldSzone[0][6][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(field == 1 && boardy >= matManager.vFieldRemove[1][field][speed][2].Pos.Y && boardy <= matManager.vFieldRemove[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			} else if(field == 0 && boardy >= matManager.vFieldSzone[1][7][field][speed][2].Pos.Y && boardy <= matManager.vFieldSzone[1][7][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(boardy >= matManager.vFieldGrave[1][field][speed][2].Pos.Y && boardy <= matManager.vFieldGrave[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= matManager.vFieldDeck[1][speed][2].Pos.Y && boardy <= matManager.vFieldDeck[1][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_DECK;
			} else if(field == 1 && boardy >= matManager.vSkillZone[0][field][speed][0].Pos.Y && boardy <= matManager.vSkillZone[0][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SKILL;
			}
		} else if(field == 0 && boardx >= matManager.vFieldRemove[1][field][speed][1].Pos.X && boardx <= matManager.vFieldRemove[1][field][speed][0].Pos.X) {
			if(boardy >= matManager.vFieldRemove[1][field][speed][2].Pos.Y && boardy <= matManager.vFieldRemove[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			} else if(boardy >= matManager.vFieldContiAct[speed][0].Y && boardy <= matManager.vFieldContiAct[speed][2].Y) {
				hovered_controler = 0;
				hovered_location = POSITION_HINT;
			} else if(boardy >= matManager.vSkillZone[0][field][speed][0].Pos.Y && boardy <= matManager.vSkillZone[0][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SKILL;
			}
		} else if(speed == 1 && boardx >= matManager.vSkillZone[0][field][speed][1].Pos.X && boardx <= matManager.vSkillZone[0][field][speed][2].Pos.X &&
				  boardy >= matManager.vSkillZone[0][field][speed][0].Pos.Y && boardy <= matManager.vSkillZone[0][field][speed][2].Pos.Y) {
			hovered_controler = 0;
			hovered_location = LOCATION_SKILL;
		} else if(field == 1 && boardx >= matManager.vFieldSzone[1][7][field][speed][1].Pos.X && boardx <= matManager.vFieldSzone[1][7][field][speed][2].Pos.X) {
			if(boardy >= matManager.vFieldSzone[1][7][field][speed][2].Pos.Y && boardy <= matManager.vFieldSzone[1][7][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(boardy >= matManager.vFieldContiAct[speed][0].Y && boardy <= matManager.vFieldContiAct[speed][2].Y) {
				hovered_controler = 0;
				hovered_location = POSITION_HINT;
			}
		} else if(boardx >= matManager.vFieldDeck[0][speed][0].Pos.X && boardx <= matManager.vFieldDeck[0][speed][1].Pos.X) {
			if(boardy >= matManager.vFieldDeck[0][speed][0].Pos.Y && boardy <= matManager.vFieldDeck[0][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_DECK;
			} else if(boardy >= matManager.vFieldGrave[0][field][speed][0].Pos.Y && boardy <= matManager.vFieldGrave[0][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_GRAVE;
			} else if(field == 0 && boardy >= matManager.vFieldSzone[1][6][field][speed][2].Pos.Y && boardy <= matManager.vFieldSzone[1][6][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 6;
			} else if(field == 0 && boardy >= matManager.vFieldSzone[0][7][field][speed][0].Pos.Y && boardy <= matManager.vFieldSzone[0][7][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			} else if(field == 1 && boardy >= matManager.vFieldRemove[0][field][speed][0].Pos.Y && boardy <= matManager.vFieldRemove[0][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			} else if(boardy >= matManager.vFieldSzone[1][5][field][speed][2].Pos.Y && boardy <= matManager.vFieldSzone[1][5][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= matManager.vFieldExtra[1][speed][2].Pos.Y && boardy <= matManager.vFieldExtra[1][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_EXTRA;
			} else if(field == 1 && boardy >= matManager.vSkillZone[1][field][speed][2].Pos.Y && boardy <= matManager.vSkillZone[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SKILL;
			}
		} else if(speed == 0 && field == 1 && boardx >= matManager.vFieldSzone[0][7][field][speed][1].Pos.X && boardx <= matManager.vFieldSzone[0][7][field][speed][0].Pos.X) {
			if(boardy >= matManager.vFieldSzone[0][7][field][speed][0].Pos.Y && boardy <= matManager.vFieldSzone[0][7][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 7;
			}
		} else if(field == 0 && boardx >= matManager.vFieldRemove[0][field][speed][0].Pos.X && boardx <= matManager.vFieldRemove[0][field][speed][1].Pos.X) {
			if(boardy >= matManager.vFieldRemove[0][field][speed][0].Pos.Y && boardy <= matManager.vFieldRemove[0][field][speed][2].Pos.Y) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			} else if(field == 0 && boardy >= matManager.vSkillZone[1][field][speed][2].Pos.Y && boardy <= matManager.vSkillZone[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SKILL;
			}
		} else if(field == 1 && speed == 1 && boardx >= matManager.vSkillZone[1][field][speed][1].Pos.X && boardx <= matManager.vSkillZone[1][field][speed][0].Pos.X){
			if(boardy >= matManager.vSkillZone[1][field][speed][2].Pos.Y && boardy <= matManager.vSkillZone[1][field][speed][0].Pos.Y) {
				hovered_controler = 1;
				hovered_location = LOCATION_SKILL;
			}
		} else if(boardx >= matManager.vFieldMzone[0][0][0].Pos.X && boardx <= matManager.vFieldMzone[0][4][1].Pos.X) {
			int sequence = (boardx - matManager.vFieldMzone[0][0][0].Pos.X) / (matManager.vFieldMzone[0][0][1].Pos.X - matManager.vFieldMzone[0][0][0].Pos.X);
			if(sequence > 4)
				sequence = 4;
			if((mainGame->dInfo.duel_params & DUEL_3_COLUMNS_FIELD) && (sequence == 0 || sequence== 4))
				hovered_location = 0;
			else if(boardy > matManager.vFieldSzone[0][0][field][speed][0].Pos.Y && boardy <= matManager.vFieldSzone[0][0][field][speed][2].Pos.Y) {
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
			} else if(boardy >= matManager.vFieldSzone[1][0][field][speed][2].Pos.Y && boardy < matManager.vFieldSzone[1][0][field][speed][0].Pos.Y) {
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
	int height = mainGame->Scale(1);
	auto increase = mainGame->Scale(21);
	if(flag & COMMAND_ACTIVATE) {
		mainGame->btnActivate->setVisible(true);
		mainGame->btnActivate->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnActivate->setVisible(false);
	if(flag & COMMAND_SUMMON) {
		mainGame->btnSummon->setVisible(true);
		mainGame->btnSummon->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnSummon->setVisible(false);
	if(flag & COMMAND_SPSUMMON) {
		mainGame->btnSPSummon->setVisible(true);
		mainGame->btnSPSummon->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnSPSummon->setVisible(false);
	if(flag & COMMAND_MSET) {
		mainGame->btnMSet->setVisible(true);
		mainGame->btnMSet->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnMSet->setVisible(false);
	if(flag & COMMAND_SSET) {
		if(!(clicked_card->type & TYPE_MONSTER))
			mainGame->btnSSet->setText(gDataManager->GetSysString(1153).c_str());
		else
			mainGame->btnSSet->setText(gDataManager->GetSysString(1159).c_str());
		mainGame->btnSSet->setVisible(true);
		mainGame->btnSSet->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnSSet->setVisible(false);
	if(flag & COMMAND_REPOS) {
		if(clicked_card->position & POS_FACEDOWN)
			mainGame->btnRepos->setText(gDataManager->GetSysString(1154).c_str());
		else if(clicked_card->position & POS_ATTACK)
			mainGame->btnRepos->setText(gDataManager->GetSysString(1155).c_str());
		else
			mainGame->btnRepos->setText(gDataManager->GetSysString(1156).c_str());
		mainGame->btnRepos->setVisible(true);
		mainGame->btnRepos->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnRepos->setVisible(false);
	if(flag & COMMAND_ATTACK) {
		mainGame->btnAttack->setVisible(true);
		mainGame->btnAttack->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnAttack->setVisible(false);
	if(flag & COMMAND_LIST) {
		mainGame->btnShowList->setVisible(true);
		mainGame->btnShowList->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnShowList->setVisible(false);
	if(flag & COMMAND_OPERATION) {
		mainGame->btnOperation->setVisible(true);
		mainGame->btnOperation->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnOperation->setVisible(false);
	if(flag & COMMAND_RESET) {
		mainGame->btnReset->setVisible(true);
		mainGame->btnReset->setRelativePosition(irr::core::vector2di(1, height));
		height += increase;
	} else mainGame->btnReset->setVisible(false);
	panel = mainGame->wCmdMenu;
	mainGame->wCmdMenu->setVisible(true);
	irr::core::vector2di mouse = mainGame->Resize(x, y);
	x = mouse.X;
	y = mouse.Y;
	mainGame->wCmdMenu->setRelativePosition(irr::core::recti(x - mainGame->Scale(20), y - mainGame->Scale(20) - height, x + mainGame->Scale(80), y - mainGame->Scale(20)));
}
void ClientField::UpdateChainButtons() {
	if(mainGame->btnChainAlways->isVisible()) {
		mainGame->btnChainIgnore->setPressed(mainGame->ignore_chain);
		mainGame->btnChainAlways->setPressed(mainGame->always_chain);
		mainGame->btnChainWhenAvail->setPressed(mainGame->chain_when_avail);
	}
}
void ClientField::ShowCancelOrFinishButton(int buttonOp) {
	if (!mainGame->dInfo.isReplay) {
		switch (buttonOp) {
		case 1:
			mainGame->btnCancelOrFinish->setText(gDataManager->GetSysString(1295).c_str());
			mainGame->btnCancelOrFinish->setVisible(true);
			break;
		case 2:
			mainGame->btnCancelOrFinish->setText(gDataManager->GetSysString(1296).c_str());
			mainGame->btnCancelOrFinish->setVisible(true);
			break;
		case 0:
		default:
			mainGame->btnCancelOrFinish->setVisible(false);
			break;
		}
	}
	else {
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
		str.append(gDataManager->GetName(pcard->code));
	}
	if((pcard->status & STATUS_PROC_COMPLETE)
		&& (pcard->type & (TYPE_RITUAL | TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK | TYPE_SPSUMMON)))
		str.append(L"\n").append(gDataManager->GetSysString(224));
	for(size_t i = 0; i < chains.size(); ++i) {
		auto chit = chains[i];
		if(pcard == chit.chain_card) {
			str.append(L"\n").append(fmt::sprintf(gDataManager->GetSysString(216), i + 1));
		}
		if(chit.target.find(pcard) != chit.target.end()) {
			str.append(L"\n").append(fmt::sprintf(gDataManager->GetSysString(217), i + 1, gDataManager->GetName(chit.chain_card->code)));
		}
	}
	if(str.length() > 0) {
		parent->addChild(mainGame->stCardListTip);
		irr::core::recti ePos = element->getRelativePosition();
		irr::s32 x = (ePos.UpperLeftCorner.X + ePos.LowerRightCorner.X) / 2;
		irr::s32 y = ePos.LowerRightCorner.Y;
		mainGame->stCardListTip->setText(str.c_str());
		irr::core::dimension2d<unsigned int> dTip = mainGame->guiFont->getDimension(mainGame->stCardListTip->getText()) + mainGame->Scale(irr::core::dimension2d<unsigned int>(10, 10));
		irr::s32 w = dTip.Width / 2;
		if(x - w < mainGame->Scale(10))
			x = w + mainGame->Scale(10);
		if(x + w > mainGame->Scale(670))
			x = mainGame->Scale(670) - w;
		mainGame->stCardListTip->setRelativePosition(irr::core::recti(x - w, y - mainGame->Scale(10), x + w, y - mainGame->Scale(10) + dTip.Height));
		mainGame->stCardListTip->setVisible(true);
	}
}
int GetSuitableReturn(uint32 maxseq, size_t size) {
	int bitvaluesize = maxseq;
	int uint8size = (maxseq < 255) ? size * 8 : -1;
	int uint16size = (maxseq < 65535) ? size * 16 : -1;
	int uint32size = (maxseq < 4294967295) ? size * 32 : -1;
	int res = std::min((uint32)bitvaluesize, std::min((uint32)uint8size, std::min((uint32)uint16size, (uint32)uint32size)));
	if(res == bitvaluesize)
		return 1;
	if(res == uint8size)
		return 2;
	if(res == uint16size)
		return 3;
	if(res == uint32size)
		return 4;
	return 1;
}
void ClientField::SetResponseSelectedCards() const {
	if (!mainGame->dInfo.compat_mode) {
		if(mainGame->dInfo.curMsg == MSG_SELECT_UNSELECT_CARD) {
			unsigned int respbuf[] = { 1, selected_cards[0]->select_seq };
			DuelClient::SetResponseB((char*)respbuf, sizeof(respbuf));
		} else {
			uint32 maxseq = 0;
			size_t size = selected_cards.size();
			for(auto& c : selected_cards) {
				maxseq = std::max(maxseq, c->select_seq);
			}
			ProgressiveBuffer ret;
			switch(GetSuitableReturn(maxseq, size)) {
				case 1: {
					ret.at<int32>(0) = 3;
					for(auto c : selected_cards)
						ret.bitSet(c->select_seq + (sizeof(int) * 8));
					break;
				}
				case 2:	{
					ret.at<int32>(0) = 2;
					ret.at<int32>(1) = size;
					for(size_t i = 0; i < size; ++i)
						ret.at<int8>(i + 8) = selected_cards[i]->select_seq;
					break;
				}
				case 3:	{
					ret.at<int32>(0) = 1;
					ret.at<int32>(1) = size;
					for(size_t i = 0; i < size; ++i)
						ret.at<int16>(i + 4) = selected_cards[i]->select_seq;
					break;
				}
				case 4:	{
					ret.at<int32>(0) = 0;
					ret.at<int32>(1) = size;
					for(size_t i = 0; i < size; ++i)
						ret.at<int32>(i + 2) = selected_cards[i]->select_seq;
					break;
				}
			}
			DuelClient::SetResponseB(ret.data.data(), ret.data.size());
		}
	} else {
		uint8_t respbuf[64];
		respbuf[0] = static_cast<uint8_t>(selected_cards.size() + must_select_cards.size());
		auto offset = must_select_cards.size() + 1;
		for(size_t i = 0; i < selected_cards.size(); ++i)
			respbuf[i + offset] = static_cast<uint8_t>(selected_cards[i]->select_seq);
		DuelClient::SetResponseB(respbuf, selected_cards.size() + must_select_cards.size() + 1);
	}
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
	if(mainGame->dInfo.checkRematch) {
		mainGame->HideElement(mainGame->wQuery);
		CTOS_RematchResponse crr;
		crr.rematch = false;
		DuelClient::SendPacketToServer(CTOS_REMATCH_RESPONSE, crr);
		return;
	}
	switch (mainGame->dInfo.curMsg) {
	case MSG_WAITING: {
		if (mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		break;
	}
	case MSG_SELECT_BATTLECMD: {
		if (mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		if (mainGame->wOptions->isVisible()) {
			mainGame->HideElement(mainGame->wOptions);
			ShowCancelOrFinishButton(0);
		}
		break;
	}
	case MSG_SELECT_IDLECMD: {
		if (mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			ShowCancelOrFinishButton(0);
		}
		if (mainGame->wOptions->isVisible()) {
			mainGame->HideElement(mainGame->wOptions);
			ShowCancelOrFinishButton(0);
		}
		break;
	}
	case MSG_SELECT_YESNO:
	case MSG_SELECT_EFFECTYN: {
		if (highlighting_card)
			highlighting_card->is_highlighting = false;
		highlighting_card = 0;
		DuelClient::SetResponseI(0);
		mainGame->HideElement(mainGame->wQuery, true);
		break;
	}
	case MSG_SELECT_CARD:
	case MSG_SELECT_TRIBUTE: {
		if (selected_cards.size() == 0) {
			if(select_cancelable) {
				DuelClient::SetResponseI(-1);
				ShowCancelOrFinishButton(0);
				if(mainGame->wCardSelect->isVisible())
					mainGame->HideElement(mainGame->wCardSelect, true);
				else
					DuelClient::SendResponse();
			}
			break;
		}
		if (mainGame->wQuery->isVisible()) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wQuery, true);
			break;
		}
		if (select_ready) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			if (mainGame->wCardSelect->isVisible())
				mainGame->HideElement(mainGame->wCardSelect, true);
			else
				DuelClient::SendResponse();
		}
		break;
	}
	case MSG_SELECT_UNSELECT_CARD: {
		if(select_cancelable) {
			DuelClient::SetResponseI(-1);
			ShowCancelOrFinishButton(0);
			if(mainGame->wCardSelect->isVisible())
				mainGame->HideElement(mainGame->wCardSelect, true);
			else
				DuelClient::SendResponse();
		}
		break;
	}
	case MSG_SELECT_SUM: {
		if(select_ready) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			if (mainGame->wCardSelect->isVisible())
				mainGame->HideElement(mainGame->wCardSelect, true);
			DuelClient::SendResponse();
			break;
		}
		break;
	}
	case MSG_SELECT_CHAIN: {
		if (chain_forced)
			break;
		if (mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			break;
		}
		if (mainGame->wQuery->isVisible()) {
			DuelClient::SetResponseI(-1);
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wQuery, true);
		}
		else {
			mainGame->PopupElement(mainGame->wQuery);
			ShowCancelOrFinishButton(0);
		}
		if (mainGame->wOptions->isVisible()) {
			DuelClient::SetResponseI(-1);
			ShowCancelOrFinishButton(0);
			mainGame->HideElement(mainGame->wOptions);
		}
		break;
	}
	case MSG_SORT_CHAIN:
	case MSG_SORT_CARD: {
		if (mainGame->wCardSelect->isVisible()) {
			DuelClient::SetResponseI(-1);
			mainGame->HideElement(mainGame->wCardSelect, true);
		}
		break;
	}
	}
}
}
