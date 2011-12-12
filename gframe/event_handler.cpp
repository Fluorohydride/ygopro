#include "client_field.h"
#include "math.h"
#include "network.h"
#include "game.h"
#include "../ocgcore/field.h"

namespace ygo {

bool ClientField::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_LOG: {
				mainGame->lstLog->clear();
				mainGame->logParam.clear();
				break;
			}
			case BUTTON_MODE_EXIT: {
				mainGame->netManager.CancelHost();
				mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_START_SERVER: {
				if(mainGame->cbDeckSel->getSelected() == -1)
					break;
				if(!mainGame->deckManager.LoadDeck(mainGame->cbDeckSel->getItem(mainGame->cbDeckSel->getSelected()))) {
					mainGame->stModeStatus->setText(L"无效卡组");
					break;
				}
				if(!mainGame->chkNoCheckDeck->isChecked()
				        && !mainGame->deckManager.CheckLFList(mainGame->deckManager.deckhost, mainGame->cbLFlist->getSelected())) {
					mainGame->stModeStatus->setText(L"无效卡组或者卡组不符合禁卡表规范");
					break;
				}
				if(_wtoi(mainGame->ebStartLP->getText()) == 0)
					mainGame->ebStartLP->setText(L"8000");
				if(_wtoi(mainGame->ebStartHand->getText()) == 0)
					mainGame->ebStartLP->setText(L"5");
				if(_wtoi(mainGame->ebDrawCount->getText()) == 0)
					mainGame->ebStartLP->setText(L"1");
				if(mainGame->netManager.CreateHost()) {
					mainGame->btnLanStartServer->setEnabled(false);
					mainGame->btnLanCancelServer->setEnabled(true);
					mainGame->btnLanConnect->setEnabled(false);
					mainGame->btnRefreshList->setEnabled(false);
					mainGame->btnLoadReplay->setEnabled(false);
					mainGame->btnDeckEdit->setEnabled(false);
					mainGame->stModeStatus->setText(L"等待连接...");
				}
				break;
			}
			case BUTTON_LAN_CANCEL_SERVER: {
				mainGame->netManager.CancelHost();
				mainGame->stModeStatus->setText(L"");
				break;
			}
			case BUTTON_LAN_REFRESH: {
				if(mainGame->netManager.RefreshHost()) {
					mainGame->btnLanStartServer->setEnabled(false);
					mainGame->btnLanConnect->setEnabled(false);
					mainGame->btnRefreshList->setEnabled(false);
					mainGame->btnLoadReplay->setEnabled(false);
					mainGame->btnDeckEdit->setEnabled(false);
				}
				break;
			}
			case BUTTON_LAN_CONNECT: {
				if(mainGame->cbDeckSel->getSelected() == -1)
					break;
				if(!mainGame->deckManager.LoadDeck(mainGame->cbDeckSel->getItem(mainGame->cbDeckSel->getSelected()))) {
					mainGame->stModeStatus->setText(L"无效卡组");
					break;
				}
				if(mainGame->netManager.JoinHost()) {
					mainGame->btnLanStartServer->setEnabled(false);
					mainGame->btnLanConnect->setEnabled(false);
					mainGame->btnRefreshList->setEnabled(false);
					mainGame->btnLoadReplay->setEnabled(false);
					mainGame->btnDeckEdit->setEnabled(false);
					mainGame->stModeStatus->setText(L"连接中...");
				}
				break;
			}
			case BUTTON_DECK_EDIT: {
				if(mainGame->cbDeckSel->getSelected() == -1)
					break;
				if(!mainGame->deckManager.LoadDeck(mainGame->cbDeckSel->getItem(mainGame->cbDeckSel->getSelected()))) {
					mainGame->stModeStatus->setText(L"无法载入卡组");
					break;
				}
				mainGame->HideElement(mainGame->wModeSelection);
				mainGame->is_building = true;
				mainGame->wInfos->setVisible(true);
				mainGame->wCardImg->setVisible(true);
				mainGame->wDeckEdit->setVisible(true);
				mainGame->wFilter->setVisible(true);
				mainGame->deckBuilder.filterList = mainGame->deckManager._lfList[mainGame->cbLFlist->getSelected()].content;;
				mainGame->cbDBLFList->setSelected(mainGame->cbLFlist->getSelected());
				mainGame->device->setEventReceiver(&mainGame->deckBuilder);
				mainGame->cbCardType->setSelected(0);
				mainGame->cbCardType2->setSelected(0);
				mainGame->cbAttribute->setSelected(0);
				mainGame->cbRace->setSelected(0);
				mainGame->ebAttack->setText(L"");
				mainGame->ebDefence->setText(L"");
				mainGame->ebStar->setText(L"");
				mainGame->cbCardType2->setEnabled(false);
				mainGame->cbAttribute->setEnabled(false);
				mainGame->cbRace->setEnabled(false);
				mainGame->ebAttack->setEnabled(false);
				mainGame->ebDefence->setEnabled(false);
				mainGame->ebStar->setEnabled(false);
				mainGame->deckBuilder.filter_effect = 0;
				mainGame->deckBuilder.result_string[0] = L'0';
				mainGame->deckBuilder.result_string[1] = 0;
				mainGame->deckBuilder.results.clear();
				mainGame->deckBuilder.is_draging = false;
				mainGame->cbDBDecks->setSelected(mainGame->cbDeckSel->getSelected());
				for(int i = 0; i < 32; ++i)
					mainGame->chkCategory[i]->setChecked(false);
				break;
			}
			case BUTTON_LOAD_REPLAY: {
				if(mainGame->lstReplayList->getSelected() == -1)
					break;
				if(!mainGame->lastReplay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected()))) {
					mainGame->stModeStatus->setText(L"录像损坏或丢失，无法播放");
					break;
				}
				mainGame->stModeStatus->setText(L"");
				Thread::NewThread(Game::ReplayThread, &mainGame->dInfo);
				break;
			}
			case BUTTON_REPLAY_START: {
				if(!mainGame->dField.is_replaying)
					break;
				is_pausing = false;
				is_paused = false;
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->localAction.Set();
				break;
			}
			case BUTTON_REPLAY_PAUSE: {
				if(!mainGame->dField.is_replaying)
					break;
				is_pausing = true;
				mainGame->btnReplayStart->setVisible(true);
				mainGame->btnReplayPause->setVisible(false);
				mainGame->btnReplayStep->setVisible(true);
				break;
			}
			case BUTTON_REPLAY_STEP: {
				if(!mainGame->dField.is_replaying)
					break;
				is_paused = false;
				mainGame->localAction.Set();
				break;
			}
			case BUTTON_REPLAY_EXIT: {
				if(!mainGame->dField.is_replaying)
					break;
				mainGame->dField.is_replaying = false;
				mainGame->localAction.Set();
				break;
			}
			case BUTTON_REPLAY_SWAP: {
				if(!mainGame->dField.is_replaying)
					break;
				if(is_paused)
					ReplaySwap();
				else
					is_swaping = true;
				break;
			}
			case BUTTON_REPLAY_SAVE: {
				if(mainGame->ebRSName->getText()[0] == 0)
					break;
				mainGame->lastReplay.SaveReplay(mainGame->ebRSName->getText());
				mainGame->HideElement(mainGame->wReplaySave, true);
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				mainGame->HideElement(mainGame->wReplaySave, true);
				break;
			}
			case BUTTON_MSG_OK: {
				mainGame->HideElement(mainGame->wMessage, true);
				break;
			}
			case BUTTON_YES: {
				switch(mainGame->dInfo.curMsg) {
				case MSG_SELECT_YESNO:
				case MSG_SELECT_EFFECTYN: {
					mainGame->dInfo.responseI = 1;
					mainGame->SetResponseI();
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
				}
				mainGame->HideElement(mainGame->wQuery);
				break;
			}
			case BUTTON_NO: {
				switch(mainGame->dInfo.curMsg) {
				case MSG_SELECT_YESNO:
				case MSG_SELECT_EFFECTYN: {
					mainGame->dInfo.responseI = 0;
					mainGame->SetResponseI();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				case MSG_SELECT_CHAIN: {
					mainGame->dInfo.responseI = -1;
					mainGame->SetResponseI();
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				case MSG_SELECT_CARD:
				case MSG_SELECT_TRIBUTE:
				case MSG_SELECT_SUM: {
					mainGame->dInfo.responseB[0] = selected_cards.size();
					for (int i = 0; i < selected_cards.size(); ++i)
						mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
					mainGame->SetResponseB(selected_cards.size() + 1);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				}
				mainGame->HideElement(mainGame->wQuery);
				break;
			}
			case BUTTON_POS_AU: {
				mainGame->dInfo.responseI = POS_FACEUP_ATTACK;
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_AD: {
				mainGame->dInfo.responseI = POS_FACEDOWN_ATTACK;
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_DU: {
				mainGame->dInfo.responseI = POS_FACEUP_DEFENCE;
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_POS_DD: {
				mainGame->dInfo.responseI = POS_FACEDOWN_DEFENCE;
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wPosSelect, true);
				break;
			}
			case BUTTON_OPTION_PREV: {
				selected_option--;
				mainGame->btnOptionn->setVisible(true);
				if(selected_option == 0)
					mainGame->btnOptionp->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_NEXT: {
				selected_option++;
				mainGame->btnOptionp->setVisible(true);
				if(selected_option == select_options.size() - 1)
					mainGame->btnOptionn->setVisible(false);
				mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetDesc(select_options[selected_option]));
				break;
			}
			case BUTTON_OPTION_OK: {
				if (mainGame->dInfo.curMsg == MSG_SELECT_OPTION) {
					mainGame->dInfo.responseI = selected_option;
					mainGame->SetResponseI();
				} else if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					mainGame->dInfo.responseI = (index << 16) + 5;
					mainGame->SetResponseI();
				} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					mainGame->dInfo.responseI = (index << 16);
					mainGame->SetResponseI();
				} else {
					int index = 0;
					while(activatable_cards[index] != command_card || activatable_descs[index] != select_options[selected_option]) index++;
					mainGame->dInfo.responseI = index;
					mainGame->SetResponseI();
				}
				mainGame->HideElement(mainGame->wOptions, true);
				break;
			}
			case BUTTON_ANNUMBER_OK: {
				mainGame->dInfo.responseI = mainGame->cbANNumber->getSelected();
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wANNumber, true);
				break;
			}
			case BUTTON_ANCARD_OK: {
				int sel = mainGame->lstANCard->getSelected();
				if(sel == -1)
					break;
				mainGame->dInfo.responseI = ancard[sel];
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wANCard, true);
				break;
			}
			case BUTTON_CMD_ACTIVATE: {
				mainGame->wCmdMenu->setVisible(false);
				if(!list_command) {
					int index = -1;
					select_options.clear();
					for (int i = 0; i < activatable_cards.size(); ++i) {
						if (activatable_cards[i] == clicked_card) {
							select_options.push_back(activatable_descs[i]);
							if (index == -1) index = i;
						}
					}
					if (select_options.size() == 1) {
						if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
							mainGame->dInfo.responseI = (index << 16) + 5;
						} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
							mainGame->dInfo.responseI = (index << 16);
						} else {
							mainGame->dInfo.responseI = index;
						}
						mainGame->SetResponseI();
						mainGame->localAction.Set();
					} else {
						mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetDesc(select_options[0]));
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
						for(int i = 0; i < deck[command_controler].size(); ++i)
							if(deck[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(deck[command_controler][i]);
						break;
					}
					case LOCATION_GRAVE: {
						for(int i = 0; i < grave[command_controler].size(); ++i)
							if(grave[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(grave[command_controler][i]);
						break;
					}
					case LOCATION_REMOVED: {
						for(int i = 0; i < remove[command_controler].size(); ++i)
							if(remove[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(remove[command_controler][i]);
						break;
					}
					case LOCATION_EXTRA: {
						for(int i = 0; i < extra[command_controler].size(); ++i)
							if(extra[command_controler][i]->cmdFlag & COMMAND_ACTIVATE)
								selectable_cards.push_back(extra[command_controler][i]);
						break;
					}
					}
					mainGame->wCardSelect->setText(L"请选择要发动效果的卡：");
					list_command = COMMAND_ACTIVATE;
					ShowSelectCard();
				}
				break;
			}
			case BUTTON_CMD_SUMMON: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(int i = 0; i < summonable_cards.size(); ++i) {
					if(summonable_cards[i] == clicked_card) {
						ClearCommandFlag();
						mainGame->dInfo.responseI = i << 16;
						mainGame->SetResponseI();
						mainGame->localAction.Set();
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
					for(int i = 0; i < spsummonable_cards.size(); ++i) {
						if(spsummonable_cards[i] == clicked_card) {
							ClearCommandFlag();
							mainGame->dInfo.responseI = (i << 16) + 1;
							mainGame->SetResponseI();
							mainGame->localAction.Set();
							break;
						}
					}
				} else {
					selectable_cards.clear();
					switch(command_location) {
					case LOCATION_DECK: {
						for(int i = 0; i < deck[command_controler].size(); ++i)
							if(deck[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(deck[command_controler][i]);
						break;
					}
					case LOCATION_GRAVE: {
						for(int i = 0; i < grave[command_controler].size(); ++i)
							if(grave[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(grave[command_controler][i]);
						break;
					}
					case LOCATION_EXTRA: {
						for(int i = 0; i < extra[command_controler].size(); ++i)
							if(extra[command_controler][i]->cmdFlag & COMMAND_SPSUMMON)
								selectable_cards.push_back(extra[command_controler][i]);
						break;
					}
					}
					list_command = COMMAND_SPSUMMON;
					mainGame->wCardSelect->setText(L"请选择要特殊召唤的怪兽：");
					ShowSelectCard();
				}
				break;
			}
			case BUTTON_CMD_MSET: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(int i = 0; i < msetable_cards.size(); ++i) {
					if(msetable_cards[i] == clicked_card) {
						mainGame->dInfo.responseI = (i << 16) + 3;
						mainGame->SetResponseI();
						mainGame->localAction.Set();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_SSET: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(int i = 0; i < ssetable_cards.size(); ++i) {
					if(ssetable_cards[i] == clicked_card) {
						mainGame->dInfo.responseI = (i << 16) + 4;
						mainGame->SetResponseI();
						mainGame->localAction.Set();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_REPOS: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(int i = 0; i < reposable_cards.size(); ++i) {
					if(reposable_cards[i] == clicked_card) {
						mainGame->dInfo.responseI = (i << 16) + 2;
						mainGame->SetResponseI();
						mainGame->localAction.Set();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_ATTACK: {
				mainGame->wCmdMenu->setVisible(false);
				if(!clicked_card)
					break;
				for(int i = 0; i < attackable_cards.size(); ++i) {
					if(attackable_cards[i] == clicked_card) {
						mainGame->dInfo.responseI = (i << 16) + 1;
						mainGame->SetResponseI();
						mainGame->localAction.Set();
						break;
					}
				}
				break;
			}
			case BUTTON_CMD_SHOWLIST: {
				mainGame->wCmdMenu->setVisible(false);
				selectable_cards.clear();
				switch(command_location) {
				case LOCATION_MZONE: {
					ClientCard* pcard = mzone[command_controler][command_sequence];
					for(int i = 0; i < pcard->overlayed.size(); ++i)
						selectable_cards.push_back(pcard->overlayed[i]);
					myswprintf(formatBuffer, L"查看叠放卡：(%d)", pcard->overlayed.size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_GRAVE: {
					for(int i = grave[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(grave[command_controler][i]);
					myswprintf(formatBuffer, L"查看墓地：(%d)", grave[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_REMOVED: {
					for(int i = remove[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(remove[command_controler][i]);
					myswprintf(formatBuffer, L"查看除外：(%d)", remove[command_controler].size());
					mainGame->wCardSelect->setText(formatBuffer);
					break;
				}
				case LOCATION_EXTRA: {
					for(int i = extra[command_controler].size() - 1; i >= 0 ; --i)
						selectable_cards.push_back(extra[command_controler][i]);
					myswprintf(formatBuffer, L"查看额外：(%d)", extra[command_controler].size());
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
					mainGame->dInfo.responseI = 6;
					mainGame->SetResponseI();
					mainGame->localAction.Set();
				}
				break;
			}
			case BUTTON_M2: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					mainGame->dInfo.responseI = 2;
					mainGame->SetResponseI();
					mainGame->localAction.Set();
				}
				break;
			}
			case BUTTON_EP: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
					mainGame->dInfo.responseI = 3;
					mainGame->SetResponseI();
					mainGame->localAction.Set();
				} else if(mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
					mainGame->dInfo.responseI = 7;
					mainGame->SetResponseI();
					mainGame->localAction.Set();
				}
				break;
			}
			case BUTTON_CARD_0:
			case BUTTON_CARD_1:
			case BUTTON_CARD_2:
			case BUTTON_CARD_3:
			case BUTTON_CARD_4: {
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
						mainGame->dInfo.responseI = (index << 16) + 1;
						mainGame->SetResponseI();
						mainGame->HideElement(mainGame->wCardSelect, true);
						break;
					}
					if(list_command == COMMAND_ACTIVATE) {
						int index = -1;
						command_card = selectable_cards[id - BUTTON_CARD_0 + mainGame->scrCardList->getPos() / 10];
						select_options.clear();
						for (int i = 0; i < activatable_cards.size(); ++i) {
							if (activatable_cards[i] == command_card) {
								select_options.push_back(activatable_descs[i]);
								if (index == -1) index = i;
							}
						}
						if (select_options.size() == 1) {
							if (mainGame->dInfo.curMsg == MSG_SELECT_IDLECMD) {
								mainGame->dInfo.responseI = (index << 16) + 5;
							} else if (mainGame->dInfo.curMsg == MSG_SELECT_BATTLECMD) {
								mainGame->dInfo.responseI = (index << 16);
							} else {
								mainGame->dInfo.responseI = index;
							}
							mainGame->SetResponseI();
							mainGame->HideElement(mainGame->wCardSelect, true);
						} else {
							mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetDesc(select_options[0]));
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
						mainGame->dInfo.responseB[0] = selected_cards.size();
						for (int i = 0; i < selected_cards.size(); ++i)
							mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
						mainGame->SetResponseB(selected_cards.size() + 1);
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
					if (CheckSelectSum()) {
						mainGame->dInfo.responseB[0] = selected_cards.size();
						for (int i = 0; i < selected_cards.size(); ++i)
							mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
						mainGame->SetResponseB(selected_cards.size() + 1);
						mainGame->HideElement(mainGame->wCardSelect, true);
					} else {
						mainGame->wCardSelect->setVisible(false);
						mainGame->dField.ShowSelectCard();
					}
					break;
				}
				case MSG_SORT_CHAIN:
				case MSG_SORT_CARD: {
					int offset = mainGame->scrCardList->getPos() / 10;
					command_card = selectable_cards[id - BUTTON_CARD_0 + offset];
					if(sort_list[command_card->select_seq]) {
						select_min--;
						int sel = sort_list[command_card->select_seq];
						sort_list[command_card->select_seq] = 0;
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
						sort_list[command_card->select_seq] = select_min;
						myswprintf(formatBuffer, L"%d", select_min);
						mainGame->stCardPos[id - BUTTON_CARD_0]->setText(formatBuffer);
						if(select_min == select_max) {
							for(int i = 0; i < select_max; ++i)
								mainGame->dInfo.responseB[i] = sort_list[i] - 1;
							mainGame->SetResponseB(select_max);
							mainGame->HideElement(mainGame->wCardSelect, true);
						}
					}
					break;
				}
				}
				break;
			}
			case BUTTON_CARD_SEL_OK: {
				if(mainGame->dInfo.curMsg == MSG_SELECT_CARD) {
					if(select_ready) {
						mainGame->dInfo.responseB[0] = selected_cards.size();
						for (int i = 0; i < selected_cards.size(); ++i)
							mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
						mainGame->SetResponseB(selected_cards.size() + 1);
						mainGame->HideElement(mainGame->wCardSelect, true);
					}
					break;
				} else {
					mainGame->HideElement(mainGame->wCardSelect);
					break;
				}
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
					mainGame->dInfo.responseI = att;
					mainGame->SetResponseI();
					mainGame->HideElement(mainGame->wANAttribute, true);
				}
				break;
			}
			case CHECK_RACE: {
				int rac = 0, filter = 0x1, count = 0;
				for(int i = 0; i < 22; ++i, filter <<= 1) {
					if(mainGame->chkRace[i]->isChecked()) {
						rac |= filter;
						count++;
					}
				}
				if(count == announce_count) {
					mainGame->dInfo.responseI = rac;
					mainGame->SetResponseI();
					mainGame->HideElement(mainGame->wANRace, true);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_CHANGED: {
			switch(id) {
			case LISTBOX_SERVER_LIST: {
				if(mainGame->lstServerList->getSelected() == -1)
					break;
				HostInfo& hi = mainGame->netManager.hosts[mainGame->lstServerList->getSelected()];
				myswprintf(formatBuffer, L"%d.%d.%d.%d", hi.address & 0xff, (hi.address >> 8) & 0xff, (hi.address >> 16) & 0xff, (hi.address >> 24) & 0xff);
				mainGame->ebJoinIP->setText(formatBuffer);
				myswprintf(formatBuffer, L"%d", hi.port);
				mainGame->ebJoinPort->setText(formatBuffer);
				break;
			}
			case LISTBOX_LOG: {
				int sel = mainGame->lstLog->getSelected();
				if(sel != -1 && mainGame->logParam.size() >= sel && mainGame->logParam[sel]) {
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
				if(sel != -1 && mainGame->logParam.size() >= sel && mainGame->logParam[sel]) {
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
					if(selectable_cards[i + pos]->code)
						mainGame->btnCardSelect[i]->setImage(mainGame->imageManager.GetTexture(selectable_cards[i + pos]->code));
					else
						mainGame->btnCardSelect[i]->setImage(mainGame->imageManager.tCover);
					mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
					myswprintf(formatBuffer, L"%ls[%d]", DataManager::FormatLocation(selectable_cards[i + pos]->location),
					           selectable_cards[i + pos]->sequence + 1);
					mainGame->stCardPos[i]->setText(formatBuffer);
					if(selectable_cards[i + pos]->is_selected)
						mainGame->stCardPos[i]->setBackgroundColor(0xffffff00);
					else if(selectable_cards[i + pos]->controler)
						mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
					else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
				}
				break;
			}
			break;
			}
		}
		case irr::gui::EGET_TAB_CHANGED: {
			switch(id) {
			case TAB_MODES: {
				if(mainGame->wModes->getActiveTab() == 1) {
					if(mainGame->is_refreshing || mainGame->netManager.is_creating_host)
						break;
					if(mainGame->netManager.RefreshHost()) {
						mainGame->btnLanStartServer->setEnabled(false);
						mainGame->btnLanConnect->setEnabled(false);
						mainGame->btnRefreshList->setEnabled(false);
						mainGame->btnLoadReplay->setEnabled(false);
						mainGame->btnDeckEdit->setEnabled(false);
					}
				} else if(mainGame->wModes->getActiveTab() == 2) {
					mainGame->RefreshReplay();
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch(id) {
			case EDITBOX_ANCARD: {
				const wchar_t* pname = mainGame->ebANCard->getText();
				int trycode = DataManager::GetVal(pname);
				CardString cstr;
				if(mainGame->dataManager.GetString(trycode, &cstr)) {
					mainGame->lstANCard->clear();
					ancard.clear();
					mainGame->lstANCard->addItem(cstr.name);
					ancard.push_back(trycode);
					break;
				}
				if(pname[0] == 0 || pname[1] == 0)
					break;
				mainGame->lstANCard->clear();
				ancard.clear();
				for(auto cit = mainGame->dataManager._strings.begin(); cit != mainGame->dataManager._strings.end(); ++cit) {
					if(wcsstr(cit->second.name, pname) != 0) {
						mainGame->lstANCard->addItem(cit->second.name);
						ancard.push_back(cit->first);
					}
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_ANCARD: {
				const wchar_t* pname = mainGame->ebANCard->getText();
				int trycode = DataManager::GetVal(pname);
				CardString cstr;
				if(mainGame->dataManager.GetString(trycode, &cstr)) {
					mainGame->lstANCard->clear();
					ancard.clear();
					mainGame->lstANCard->addItem(cstr.name);
					ancard.push_back(trycode);
					break;
				}
				if(pname[0] == 0)
					break;
				mainGame->lstANCard->clear();
				ancard.clear();
				for(auto cit = mainGame->dataManager._strings.begin(); cit != mainGame->dataManager._strings.end(); ++cit) {
					if(wcsstr(cit->second.name, pname) != 0) {
						mainGame->lstANCard->addItem(cit->second.name);
						ancard.push_back(cit->first);
					}
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
					CardData cd;
					mainGame->dataManager.GetData(mcard->code, &cd);
					mainGame->imgCard->setImage(mainGame->imageManager.GetTexture(mcard->code));
					myswprintf(formatBuffer, L"%ls[%d]", mainGame->dataManager.GetName(mcard->code), mcard->code);
					mainGame->stName->setText(formatBuffer);
					if(cd.type & TYPE_MONSTER) {
						myswprintf(formatBuffer, L"[%ls] %ls/%ls", DataManager::FormatType(cd.type), DataManager::FormatRace(cd.race), DataManager::FormatAttribute(cd.attribute));
						mainGame->stInfo->setText(formatBuffer);
						formatBuffer[0] = L'[';
						for(int i = 1; i <= cd.level; ++i)
							formatBuffer[i] = L'★';
						formatBuffer[cd.level + 1] = L']';
						formatBuffer[cd.level + 2] = L' ';
						if(cd.attack < 0 && cd.defence < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"?/?");
						else if(cd.attack < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"?/%d", cd.defence);
						else if(cd.defence < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"%d/?", cd.attack);
						else
							myswprintf(&formatBuffer[cd.level + 3], L"%d/%d", cd.attack, cd.defence);
						mainGame->stDataInfo->setText(formatBuffer);
						mainGame->stText->setRelativePosition(irr::core::position2di(15, 83));
					} else {
						myswprintf(formatBuffer, L"[%ls]", DataManager::FormatType(cd.type));
						mainGame->stInfo->setText(formatBuffer);
						mainGame->stDataInfo->setText(L"");
						mainGame->stText->setRelativePosition(irr::core::position2di(15, 60));
					}
					mainGame->SetStaticText(mainGame->stText, 270, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetText(mcard->code));
				} else {
					mainGame->stTip->setVisible(false);
					mainGame->imgCard->setImage(mainGame->imageManager.tCover);
					mainGame->stName->setText(L"");
					mainGame->stInfo->setText(L"");
					mainGame->stDataInfo->setText(L"");
					mainGame->stText->setText(L"");
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
			if(is_replaying)
				break;
			if(!mainGame->dInfo.isStarted)
				break;
			s32 x = event.MouseInput.X;
			s32 y = event.MouseInput.Y;
			hovered_location = 0;
			irr::core::position2di pos(x, y);
			if(x < 300)
				break;
			if(mainGame->wCmdMenu->isVisible() && !mainGame->wCmdMenu->getRelativePosition().isPointInside(pos))
				mainGame->wCmdMenu->setVisible(false);
			if(panel && panel->isVisible())
				break;
			GetHoverField(x, y);
			if(hovered_location & 0xe)
				clicked_card = GetCard(hovered_controler, hovered_location, hovered_sequence);
			else clicked_card = 0;
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
					int command_flag = 0;
					if(remove[hovered_controler].size() == 0)
						break;
					ShowMenu(COMMAND_LIST, x, y);
					break;
				}
				case LOCATION_EXTRA: {
					int command_flag = 0;
					if(extra[hovered_controler].size() == 0 || hovered_controler != 0)
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
					for(int i = 0; i < deck[hovered_controler].size(); ++i)
						command_flag |= deck[hovered_controler][i]->cmdFlag;
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
					for(int i = 0; i < grave[hovered_controler].size(); ++i)
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
					for(int i = 0; i < remove[hovered_controler].size(); ++i)
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
					for(int i = 0; i < extra[hovered_controler].size(); ++i)
						command_flag |= extra[hovered_controler][i]->cmdFlag;
					if(hovered_controler == 0)
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
				if (!(hovered_location & LOCATION_ONFIELD))
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
							int filter = 1;
							int p = 0;
							for (int i = 0; i < 5; ++i, filter <<= 1) {
								if (selected_field & filter) {
									mainGame->dInfo.responseB[p] = mainGame->LocalPlayer(0);
									mainGame->dInfo.responseB[p + 1] = 0x4;
									mainGame->dInfo.responseB[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x100;
							for (int i = 0; i < 6; ++i, filter <<= 1) {
								if (selected_field & filter) {
									mainGame->dInfo.responseB[p] = mainGame->LocalPlayer(0);
									mainGame->dInfo.responseB[p + 1] = 0x8;
									mainGame->dInfo.responseB[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x10000;
							for (int i = 0; i < 5; ++i, filter <<= 1) {
								if (selected_field & filter) {
									mainGame->dInfo.responseB[p] = mainGame->LocalPlayer(1);
									mainGame->dInfo.responseB[p + 1] = 0x4;
									mainGame->dInfo.responseB[p + 2] = i;
									p += 3;
								}
							}
							filter = 0x1000000;
							for (int i = 0; i < 6; ++i, filter <<= 1) {
								if (selected_field & filter) {
									mainGame->dInfo.responseB[p] = mainGame->LocalPlayer(1);
									mainGame->dInfo.responseB[p + 1] = 0x8;
									mainGame->dInfo.responseB[p + 2] = i;
									p += 3;
								}
							}
							selectable_field = 0;
							selected_field = 0;
							mainGame->SetResponseB(p);
							mainGame->localAction.Set();
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
					for(int i = 0; i < selected_cards.size(); ++i)
						max += selected_cards[i]->opParam;
				}
				if (min >= select_max) {
					mainGame->dInfo.responseB[0] = selected_cards.size();
					for (int i = 0; i < selected_cards.size(); ++i)
						mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
					mainGame->SetResponseB(selected_cards.size() + 1);
					mainGame->localAction.Set();
				} else if (max >= select_min) {
					if(selected_cards.size() == selectable_cards.size()) {
						mainGame->dInfo.responseB[0] = selected_cards.size();
						for (int i = 0; i < selected_cards.size(); ++i)
							mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
						mainGame->SetResponseB(selected_cards.size() + 1);
						mainGame->localAction.Set();
					} else {
						select_ready = true;
						mainGame->stQMessage->setText(L"当前所选的卡已选择条件\n是否要继续选择？");
						mainGame->PopupElement(mainGame->wQuery);
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
				mainGame->dInfo.responseB[clicked_card->select_seq]++;
				if (clicked_card->opParam == 0)
					clicked_card->is_selectable = false;
				select_counter_count--;
				if (select_counter_count == 0) {
					mainGame->stHintMsg->setVisible(false);
					mainGame->SetResponseB(selectable_cards.size());
					mainGame->localAction.Set();
				} else {
					myswprintf(formatBuffer, L"请移除%d个[%ls]:", select_counter_count, mainGame->dataManager.GetCounterName(select_counter_type));
					mainGame->stHintMsg->setText(formatBuffer);
				}
				break;
			}
			case MSG_SELECT_SUM: {
				if (!clicked_card)
					break;
				if (clicked_card->is_selected) {
					int i = 0;
					while(selected_cards[i] != clicked_card) i++;
					selected_cards.erase(selected_cards.begin() + i);
				} else if (clicked_card->is_selectable)
					selected_cards.push_back(clicked_card);
				else break;
				if (CheckSelectSum()) {
					if(selectable_cards.size() == 0) {
						mainGame->dInfo.responseB[0] = selected_cards.size();
						for (int i = 0; i < selected_cards.size(); ++i)
							mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
						mainGame->SetResponseB(selected_cards.size() + 1);
						mainGame->localAction.Set();
					} else {
						select_ready = true;
						mainGame->stQMessage->setText(L"当前所选的卡已选择条件\n是否要继续选择？");
						mainGame->PopupElement(mainGame->wQuery);
					}
				} else
					select_ready = false;
				break;
			}
			}
			break;
		}
		case irr::EMIE_RMOUSE_LEFT_UP: {
			if(is_replaying)
				break;
			mainGame->wCmdMenu->setVisible(false);
			if(mainGame->fadingFrame)
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
				mainGame->dInfo.responseI = 0;
				mainGame->SetResponseI();
				mainGame->HideElement(mainGame->wQuery, true);
				break;
			}
			case MSG_SELECT_CARD: {
				if(selected_cards.size() == 0) {
					if(select_cancelable) {
						mainGame->dInfo.responseI = -1;
						mainGame->SetResponseI();
						if(mainGame->wCardSelect->isVisible())
							mainGame->HideElement(mainGame->wCardSelect, true);
						else
							mainGame->localAction.Set();
					}
					break;
				}
				if(mainGame->wQuery->isVisible()) {
					mainGame->dInfo.responseB[0] = selected_cards.size();
					for (int i = 0; i < selected_cards.size(); ++i)
						mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
					mainGame->SetResponseB(selected_cards.size() + 1);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				if(select_ready) {
					mainGame->dInfo.responseB[0] = selected_cards.size();
					for (int i = 0; i < selected_cards.size(); ++i)
						mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
					mainGame->SetResponseB(selected_cards.size() + 1);
					if(mainGame->wCardSelect->isVisible())
						mainGame->HideElement(mainGame->wCardSelect, true);
					else
						mainGame->localAction.Set();
				}
				break;
			}
			case MSG_SELECT_TRIBUTE:
			case MSG_SELECT_SUM: {
				if(mainGame->wQuery->isVisible()) {
					mainGame->dInfo.responseB[0] = selected_cards.size();
					for (int i = 0; i < selected_cards.size(); ++i)
						mainGame->dInfo.responseB[i + 1] = selected_cards[i]->select_seq;
					mainGame->SetResponseB(selected_cards.size() + 1);
					mainGame->HideElement(mainGame->wQuery, true);
					break;
				}
				break;
			}
			case MSG_SELECT_CHAIN: {
				if(mainGame->wCardSelect->isVisible()) {
					mainGame->HideElement(mainGame->wCardSelect);
					break;
				}
				if(mainGame->wQuery->isVisible()) {
					mainGame->dInfo.responseI = -1;
					mainGame->SetResponseI();
					mainGame->HideElement(mainGame->wQuery, true);
				} else {
					mainGame->PopupElement(mainGame->wQuery);
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
				} else mcard = 0;
			}
			if(hovered_location == LOCATION_HAND && (mainGame->dInfo.is_shuffling || mainGame->dInfo.curMsg == MSG_SHUFFLE_HAND))
				mcard = 0;
			if(mcard != hovered_card) {
				if(hovered_card) {
					if(hovered_card->location == LOCATION_HAND && !mainGame->dInfo.is_shuffling && mainGame->dInfo.curMsg != MSG_SHUFFLE_HAND) {
						hovered_card->is_hovered = false;
						MoveCard(hovered_card, 5);
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
								myswprintf(formatBuffer, L"%ls[%d]", mainGame->dataManager.GetName(mcard->code), mcard->code);
								str.append(formatBuffer);
								if(mcard->alias && mcard->alias != mcard->code) {
									myswprintf(formatBuffer, L"\n(%ls[%d])", mainGame->dataManager.GetName(mcard->alias), mcard->alias);
									str.append(formatBuffer);
								}
								myswprintf(formatBuffer, L"\n%ls/%ls", mcard->atkstring, mcard->defstring);
								str.append(formatBuffer);
								myswprintf(formatBuffer, L"\n★%d %ls/%ls", (mcard->level ? mcard->level : mcard->rank), DataManager::FormatRace(mcard->race), DataManager::FormatAttribute(mcard->attribute));
								str.append(formatBuffer);
								if(mcard->counters.size()) {
									for(std::map<int, int>::iterator ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
										myswprintf(formatBuffer, L"\n[%ls]：%d", mainGame->dataManager.GetCounterName(ctit->first), ctit->second);
										str.append(formatBuffer);
									}
								}
								if(mcard->turnCounter && (mcard->location & LOCATION_ONFIELD)) {
									myswprintf(formatBuffer, L"\n回合计数：%d", mcard->turnCounter);
									str.append(formatBuffer);
								}
							} else {
								myswprintf(formatBuffer, L"%ls[%d]", mainGame->dataManager.GetName(mcard->code), mcard->code);
								str.append(formatBuffer);
								if(mcard->alias && mcard->alias != mcard->code) {
									myswprintf(formatBuffer, L"\n%ls[%d]", mainGame->dataManager.GetName(mcard->alias), mcard->alias);
									str.append(formatBuffer);
								}
								if(mcard->counters.size()) {
									for(std::map<int, int>::iterator ctit = mcard->counters.begin(); ctit != mcard->counters.end(); ++ctit) {
										myswprintf(formatBuffer, L"\n[%ls]：%d", mainGame->dataManager.GetCounterName(ctit->first), ctit->second);
										str.append(formatBuffer);
									}
								}
								if(mcard->turnCounter && (mcard->location & LOCATION_ONFIELD)) {
									myswprintf(formatBuffer, L"\n回合计数：%d", mcard->turnCounter);
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
						mainGame->imgCard->setImage(mainGame->imageManager.tCover);
						mainGame->stName->setText(L"");
						mainGame->stInfo->setText(L"");
						mainGame->stDataInfo->setText(L"");
						mainGame->stText->setText(L"");
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
		default:
			break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_A: {
			mainGame->always_chain = event.KeyInput.PressedDown;
			break;
		}
		case irr::KEY_KEY_S: {
			mainGame->ignore_chain = event.KeyInput.PressedDown;
			break;
		}
		case irr::KEY_KEY_R: {
			if(!event.KeyInput.PressedDown)
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			device->minimizeWindow();
			break;
		}
		}
		break;
	}
	}
	return false;
}
void ClientField::GetHoverField(int x, int y) {
	irr::core::recti sfRect(433, 528, 883, 618);
	irr::core::recti ofRect(513, 117, 807, 175);
	irr::core::position2di pos(x, y);
	if(sfRect.isPointInside(pos)) {
		int hc = hand[0].size();
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7) {
			int left = 433 + 77 * (6 - hc) / 2;
			if(x < left)
				hovered_location = 0;
			else {
				int seq = (x - left) / 77;
				if(seq >= hc) seq = hc - 1;
				if(x - left - 77 * seq < 68) {
					hovered_controler = 0;
					hovered_location = LOCATION_HAND;
					hovered_sequence = seq;
				} else hovered_location = 0;
			}
		} else {
			hovered_controler = 0;
			hovered_location = LOCATION_HAND;
			if(x >= 817)
				hovered_sequence = hc - 1;
			else
				hovered_sequence = (x - 433) * (hc - 1) / 384;
		}
	} else if(ofRect.isPointInside(pos)) {
		int hc = hand[1].size();
		if(hc == 0)
			hovered_location = 0;
		else if(hc < 7) {
			int left = 513 + 50 * (6 - hc) / 2;
			if(x < left)
				hovered_location = 0;
			else {
				int seq = (x - left) / 50;
				if(seq >= hc) seq = hc - 1;
				if(x - left - 50 * seq < 45) {
					hovered_controler = 1;
					hovered_location = LOCATION_HAND;
					hovered_sequence = hc - 1 - seq;
				} else hovered_location = 0;
			}
		} else {
			hovered_controler = 1;
			hovered_location = LOCATION_HAND;
			if(x >= 763)
				hovered_sequence = 0;
			else
				hovered_sequence = hc - 1 - (x - 513) * (hc - 1) / 250;
		}
	} else {
		double screenx = x / 1024.0 * 1.25 - 0.81;
		double screeny = y / 640.0 * 0.84 - 0.42;
		double angle = 0.66104316885 - atan(screeny);	//0.66104316885 = arctan(7.0/9.0)
		double vlen = sqrt(1.0 + screeny * screeny);
		double boardx = 3.95 + 9.0 * screenx / vlen / cos(angle);
		double boardy = 7.0 - 9.0 * tan(angle);
		hovered_location = 0;
		if(boardx >= 0.2 && boardx <= 1.0) {
			if(boardy >= 2.4 && boardy <= 3.6) {
				hovered_controler = 0;
				hovered_location = LOCATION_EXTRA;
			} else if(boardy >= 1.1 && boardy <= 2.3) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= -1.0 && boardy <= 0.2) {
				hovered_controler = 1;
				hovered_location = LOCATION_REMOVED;
			} else if(boardy >= -2.3 && boardy <= -1.1) {
				hovered_controler = 1;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= -3.6 && boardy <= -2.4) {
				hovered_controler = 1;
				hovered_location = LOCATION_DECK;
			}
		} else if(boardx >= 6.9 && boardx <= 7.7) {
			if(boardy >= 2.4 && boardy <= 3.6) {
				hovered_controler = 0;
				hovered_location = LOCATION_DECK;
			} else if(boardy >= 1.1 && boardy <= 2.3) {
				hovered_controler = 0;
				hovered_location = LOCATION_GRAVE;
			} else if(boardy >= -0.2 && boardy <= 1.0) {
				hovered_controler = 0;
				hovered_location = LOCATION_REMOVED;
			} else if(boardy >= -2.3 && boardy <= -1.1) {
				hovered_controler = 1;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = 5;
			} else if(boardy >= -3.6 && boardy <= -2.4) {
				hovered_controler = 1;
				hovered_location = LOCATION_EXTRA;
			}
		} else if(boardx >= 1.2 && boardx <= 6.7) {
			if(boardy > 1.7 && boardy <= 2.9) {
				hovered_controler = 0;
				hovered_location = LOCATION_SZONE;
				hovered_sequence = (boardx - 1.2) / 1.1;
				if(hovered_sequence > 4)
					hovered_sequence = 4;
			} else if(boardy >= 0.5 && boardy <= 1.7) {
				hovered_controler = 0;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = (boardx - 1.2) / 1.1;
				if(hovered_sequence > 4)
					hovered_sequence = 4;
			} else if(boardy >= -1.7 && boardy <= -0.5) {
				hovered_controler = 1;
				hovered_location = LOCATION_MZONE;
				hovered_sequence = 4 - (int)((boardx - 1.2) / 1.1);
				if(hovered_sequence < 0)
					hovered_sequence = 0;
			} else if(boardy >= -2.9 && boardy < -1.7) {
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
		mainGame->btnSSet->setVisible(true);
		mainGame->btnSSet->setRelativePosition(position2di(1, height));
		height += 21;
	} else mainGame->btnSSet->setVisible(false);
	if(flag & COMMAND_REPOS) {
		if(clicked_card->position & POS_FACEDOWN)
			mainGame->btnRepos->setText(L"反转召唤");
		else if(clicked_card->position & POS_ATTACK)
			mainGame->btnRepos->setText(L"守备表示");
		else
			mainGame->btnRepos->setText(L"攻击表示");
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
}
