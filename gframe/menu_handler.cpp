#include "config.h"
#include "menu_handler.h"
#include "netserver.h"
#include "duelclient.h"
#include "deck_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "image_manager.h"
#include "game.h"

namespace ygo {

void UpdateDeck() {
	mainGame->gameConf.lastdeck = mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected());
	char deckbuf[1024];
	char* pdeck = deckbuf;
	BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
	BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
	for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
		BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->first);
	DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
}
bool MenuHandler::OnEvent(const irr::SEvent& event) {
	if(mainGame->dField.OnCommonEvent(event))
		return false;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		irr::gui::IGUIElement* caller = event.GUIEvent.Caller;
		s32 id = caller->getID();
		if(mainGame->wRules->isVisible() && (id != BUTTON_RULE_OK && id != CHECKBOX_EXTRA_RULE))
			break;
		if(mainGame->wMessage->isVisible() && id != BUTTON_MSG_OK)
			break;
		if(mainGame->wCustomRules->isVisible() && id != BUTTON_CUSTOM_RULE_OK && (id < CHECKBOX_OBSOLETE || id > CHECKBOX_EMZONE))
			break;
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO) {
			mainGame->wQuery->getParent()->bringToFront(mainGame->wQuery);
			break;
		}
		if(mainGame->wReplaySave->isVisible() && id != BUTTON_REPLAY_SAVE && id != BUTTON_REPLAY_CANCEL) {
			mainGame->wReplaySave->getParent()->bringToFront(mainGame->wReplaySave);
			break;
		}
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_ELEMENT_HOVERED: {
			// Set cursor to an I-Beam if hovering over an edit box
			if (event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX && event.GUIEvent.Caller->isEnabled())
			{
				Utils::changeCursor(ECI_IBEAM);
			}
			break;
		}
		case irr::gui::EGET_ELEMENT_LEFT: {
			// Set cursor to normal if left an edit box
			if (event.GUIEvent.Caller->getType() == EGUIET_EDIT_BOX && event.GUIEvent.Caller->isEnabled())
			{
				Utils::changeCursor(ECI_NORMAL);
			}
			break;
		}
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_MODE_EXIT: {
				mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_MODE: {
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_JOIN_HOST: {
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412).c_str());
						mainGame->gMutex.unlock();
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = std::stoi(mainGame->ebJoinPort->getText());
				mainGame->gameConf.lasthost = pstr;
				mainGame->gameConf.lastport = mainGame->ebJoinPort->getText();
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHost->setEnabled(false);
					mainGame->btnJoinHost->setEnabled(false);
					mainGame->btnJoinCancel->setEnabled(false);
				}
				break;
			}
			case BUTTON_JOIN_CANCEL: {
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->ShowElement(mainGame->wMainMenu);
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_REFRESH: {
				DuelClient::BeginRefreshHost();
				break;
			}
			case BUTTON_CREATE_HOST: {
				mainGame->btnHostConfirm->setEnabled(true);
				mainGame->btnHostCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->ShowElement(mainGame->wCreateHost);
				break;
			}
			case BUTTON_RULE_CARDS: {
				mainGame->PopupElement(mainGame->wRules);
				break;
			}
			case BUTTON_RULE_OK: {
				mainGame->HideElement(mainGame->wRules);
				break;
			}
			case BUTTON_CUSTOM_RULE: {
				switch (mainGame->cbDuelRule->getSelected()) {
				case 0: {
					mainGame->duel_param = MASTER_RULE_1;
					mainGame->forbiddentypes = MASTER_RULE_1_FORB;
					break;
				}
				case 1: {
					mainGame->duel_param = MASTER_RULE_2;
					mainGame->forbiddentypes = MASTER_RULE_2_FORB;
					break;
				}
				case 2: {
					mainGame->duel_param = MASTER_RULE_3;
					mainGame->forbiddentypes = MASTER_RULE_3_FORB;
					break;
				}
				case 3: {
					mainGame->duel_param = MASTER_RULE_4;
					mainGame->forbiddentypes = MASTER_RULE_4_FORB;
					break;
				}
				}
				uint32 filter = 0x100;
				for (int i = 0; i < 6; ++i, filter <<= 1) {
						mainGame->chkCustomRules[i]->setChecked(mainGame->duel_param & filter);
					if(i == 3)
						mainGame->chkCustomRules[4]->setEnabled(mainGame->duel_param & filter);
				}
				uint32 limits[] = { TYPE_FUSION, TYPE_SYNCHRO, TYPE_XYZ, TYPE_PENDULUM, TYPE_LINK };
				for (int i = 0; i < 5; ++i, filter <<= 1)
						mainGame->chkTypeLimit[i]->setChecked(mainGame->forbiddentypes & limits[i]);
				mainGame->PopupElement(mainGame->wCustomRules);
				break;
			}
			case BUTTON_CUSTOM_RULE_OK: {
				mainGame->UpdateDuelParam();
				mainGame->HideElement(mainGame->wCustomRules);
				break;
			}
			case BUTTON_HOST_CONFIRM: {
				unsigned int host_port = std::stoi(mainGame->ebHostPort->getText());
				mainGame->gameConf.gamename = mainGame->ebServerName->getText();
				mainGame->gameConf.serverport = mainGame->ebHostPort->getText();
				if(!NetServer::StartServer(host_port))
					break;
				if(!DuelClient::StartClient(0x7f000001, host_port)) {
					NetServer::StopServer();
					break;
				}
				mainGame->btnHostConfirm->setEnabled(false);
				mainGame->btnHostCancel->setEnabled(false);
				break;
			}
			case BUTTON_HOST_CANCEL: {
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				if(mainGame->wRules->isVisible())
					mainGame->HideElement(mainGame->wRules);
				mainGame->HideElement(mainGame->wCreateHost);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_HP_DUELIST: {
				mainGame->cbDeckSelect->setEnabled(true);
				DuelClient::SendPacketToServer(CTOS_HS_TODUELIST);
				break;
			}
			case BUTTON_HP_OBSERVER: {
				DuelClient::SendPacketToServer(CTOS_HS_TOOBSERVER);
				break;
			}
			case BUTTON_HP_KICK: {
				int id = 0;
				while(id < 4) {
					if(mainGame->btnHostPrepKick[id] == caller)
						break;
					id++;
				}
				CTOS_Kick csk;
				csk.pos = id;
				DuelClient::SendPacketToServer(CTOS_HS_KICK, csk);
				break;
			}
			case BUTTON_HP_READY: {
				bool check = false;
				if(!mainGame->cbDeckSelect2->isVisible())
					check = (mainGame->cbDeckSelect->getSelected() == -1 || !deckManager.LoadDeck(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected())));
				else
					check = (mainGame->cbDeckSelect->getSelected() == -1 || mainGame->cbDeckSelect2->getSelected() == -1 || !deckManager.LoadDeckDouble(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()), mainGame->cbDeckSelect2->getItem(mainGame->cbDeckSelect2->getSelected())));
				if(check)
					break;
				UpdateDeck();
				DuelClient::SendPacketToServer(CTOS_HS_READY);
				mainGame->cbDeckSelect->setEnabled(false);
				mainGame->cbDeckSelect2->setEnabled(false);
				if(mainGame->dInfo.isTag || mainGame->dInfo.isRelay)
					mainGame->btnHostPrepDuelist->setEnabled(false);
				break;
			}
			case BUTTON_HP_NOTREADY: {
				DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
				mainGame->cbDeckSelect->setEnabled(true);
				mainGame->cbDeckSelect2->setEnabled(true);
				if(mainGame->dInfo.isTag || mainGame->dInfo.isRelay)
					mainGame->btnHostPrepDuelist->setEnabled(true);
				break;
			}
			case BUTTON_HP_START: {
				DuelClient::SendPacketToServer(CTOS_HS_START);
				break;
			}
			case BUTTON_HP_CANCEL: {
				DuelClient::StopClient();
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wHostPrepare);
				if(mainGame->wHostPrepare2->isVisible())
					mainGame->HideElement(mainGame->wHostPrepare2);
				mainGame->ShowElement(mainGame->wLanWindow);
				mainGame->wChat->setVisible(false);
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_REPLAY_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wReplay);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->stReplayInfo->setText(L"");
				mainGame->RefreshReplay();
				break;
			}
			case BUTTON_SINGLE_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wSinglePlay);
				mainGame->RefreshSingleplay();
				break;
			}
			case BUTTON_LOAD_REPLAY: {
				if(open_file) {
					ReplayMode::cur_replay.OpenReplay(open_file_name);
					open_file = false;
				} else {
					if(mainGame->lstReplayList->getSelected() == -1)
						break;
					if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected(), true)))
						break;
				}
				if(mainGame->chkYrp->isChecked() && !ReplayMode::cur_replay.LoadYrp())
					break;
				mainGame->ClearCardInfo();
				mainGame->mTopMenu->setVisible(false);
				mainGame->wCardImg->setVisible(true);
				mainGame->wInfos->setVisible(true);
				mainGame->wReplay->setVisible(true);
				mainGame->wReplayControl->setVisible(true);
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->btnReplayUndo->setVisible(false);
				mainGame->wPhase->setVisible(true);
				mainGame->dField.Clear();
				mainGame->HideElement(mainGame->wReplay);
				mainGame->device->setEventReceiver(&mainGame->dField);
				unsigned int start_turn = _wtoi(mainGame->ebRepStartTurn->getText());
				if(start_turn == 1)
					start_turn = 0;
				ReplayMode::StartReplay(start_turn);
				break;
			}
			case BUTTON_DELETE_REPLAY: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				mainGame->stQMessage->setText(fmt::format(L"{}\n{}", mainGame->lstReplayList->getListItem(sel), dataManager.GetSysString(1363)).c_str());
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.unlock();
				prev_operation = id;
				prev_sel = sel;
				break;
			}
			case BUTTON_RENAME_REPLAY: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				mainGame->wReplaySave->setText(dataManager.GetSysString(1364).c_str());
				mainGame->ebRSName->setText(mainGame->lstReplayList->getListItem(sel));
				mainGame->PopupElement(mainGame->wReplaySave);
				mainGame->gMutex.unlock();
				prev_operation = id;
				prev_sel = sel;
				break;
			}
			case BUTTON_CANCEL_REPLAY: {
				mainGame->HideElement(mainGame->wReplay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_LOAD_SINGLEPLAY: {
				if(!open_file && mainGame->lstSinglePlayList->getSelected() == -1)
					break;
				mainGame->singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			case BUTTON_CANCEL_SINGLEPLAY: {
				mainGame->HideElement(mainGame->wSinglePlay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_DECK_EDIT: {
				mainGame->RefreshDeck(mainGame->cbDBDecks);
				if(open_file && deckManager.LoadDeck(open_file_name)) {
#ifdef WIN32
					auto dash = open_file_name.find_last_of(L"\\");
#else
					auto dash = open_file_name.find_last_of(L"/");
#endif
					auto dot = open_file_name.find_last_of(L".");
					if(dash != std::wstring::npos && dot != std::wstring::npos) {
						mainGame->ebDeckname->setText(open_file_name.substr(dash + 1, dot - dash - 1).c_str());
						mainGame->cbDBDecks->setSelected(-1);
					} else {
						for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
							if(mainGame->cbDBDecks->getItem(i) == open_file_name) {
								mainGame->cbDBDecks->setSelected(i);
								break;
							}
						}
					}
					open_file = false;
				} else if(mainGame->cbDBDecks->getSelected() != -1) {
					deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
					mainGame->ebDeckname->setText(L"");
				}
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->deckBuilder.Initialize();
				break;
			}
			case BUTTON_MSG_OK: {
				mainGame->HideElement(mainGame->wMessage);
				break;
			}
			case BUTTON_YES: {
				mainGame->HideElement(mainGame->wQuery);
				if(prev_operation == BUTTON_DELETE_REPLAY) {
					if(Replay::DeleteReplay(mainGame->lstReplayList->getListItem(prev_sel))) {
						mainGame->stReplayInfo->setText(L"");
						mainGame->lstReplayList->removeItem(prev_sel);
					}
				}
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_NO: {
				mainGame->HideElement(mainGame->wQuery);
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_REPLAY_SAVE: {
				mainGame->HideElement(mainGame->wReplaySave);
				if(prev_operation == BUTTON_RENAME_REPLAY) {
					std::wstring newname(mainGame->ebRSName->getText());
					newname = newname.substr(0, newname.find_last_of(L"."));
					std::wstring oldname(mainGame->lstReplayList->getListItem(prev_sel));
					auto oldextension = oldname.substr(oldname.find_last_of(L"."));
					newname += oldextension;
					if(Replay::RenameReplay(oldname, newname)) {
						mainGame->lstReplayList->setItem(prev_sel, newname.c_str(), -1);
					} else {
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1365).c_str());
					}
				}
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			case BUTTON_REPLAY_CANCEL: {
				mainGame->HideElement(mainGame->wReplaySave);
				prev_operation = 0;
				prev_sel = -1;
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_CHANGED: {
			switch(id) {
			case LISTBOX_LAN_HOST: {
				int sel = mainGame->lstHostList->getSelected();
				if(sel == -1)
					break;
				int addr = DuelClient::hosts[sel].ipaddr;
				int port = DuelClient::hosts[sel].port;
				mainGame->ebJoinHost->setText(fmt::format(L"{}.{}.{}.{}", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff).c_str());
				mainGame->ebJoinPort->setText(fmt::to_wstring(port).c_str());
				break;
			}
			case LISTBOX_REPLAY_LIST: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1 || sel == 0)
					break;
				if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(sel, true)))
					break;
				std::wstring repinfo;
				time_t curtime = ReplayMode::cur_replay.pheader.seed;
				tm* st = localtime(&curtime);
				repinfo.append(fmt::format(L"{}/{}/{} {:02}:{:02}:{:02}\n", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec).c_str());
				wchar_t namebuf[6][20];
				ReplayMode::cur_replay.ReadName(namebuf[0]);
				ReplayMode::cur_replay.ReadName(namebuf[1]);
				if(ReplayMode::cur_replay.pheader.flag & (REPLAY_TAG + REPLAY_RELAY)) {
					ReplayMode::cur_replay.ReadName(namebuf[2]);
					ReplayMode::cur_replay.ReadName(namebuf[3]);
				}
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_RELAY) {
					ReplayMode::cur_replay.ReadName(namebuf[4]);
					ReplayMode::cur_replay.ReadName(namebuf[5]);
				}
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_TAG)
					repinfo.append(fmt::format(L"{}\n{}\n===VS===\n{}\n{}\n", namebuf[0], namebuf[1], namebuf[2], namebuf[3]));
				else if (ReplayMode::cur_replay.pheader.flag & REPLAY_RELAY)
					repinfo.append(fmt::format(L"{}\n{}\n{}\n===VS===\n{}\n{}\n{}\n", namebuf[0], namebuf[1], namebuf[2], namebuf[3], namebuf[4], namebuf[5]));
				else
					repinfo.append(fmt::format(L"{}\n===VS===\n{}\n", namebuf[0], namebuf[1]));
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->stReplayInfo->setText((wchar_t*)repinfo.c_str());
				if(ReplayMode::cur_replay.pheader.id == 0x31707279) {
					mainGame->chkYrp->setChecked(false);
					mainGame->chkYrp->setEnabled(false);
				} else
					mainGame->chkYrp->setEnabled(mainGame->coreloaded);
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				int sel = mainGame->lstSinglePlayList->getSelected();
				if(sel == -1 || sel == 0)
					break;
				const wchar_t* name = mainGame->lstSinglePlayList->getListItem(mainGame->lstSinglePlayList->getSelected(), true);
				mainGame->stSinglePlayInfo->setText(mainGame->ReadPuzzleMessage(name).c_str());
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_LISTBOX_SELECTED_AGAIN: {
			switch(id) {
			case LISTBOX_LAN_HOST: {
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinHost->getText();
				BufferIO::CopyWStr(pstr, ip, 16);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::CopyWStr(pstr, hostname, 100);
					BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), port, 6);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = NULL;
					memset(&hints, 0, sizeof(hints));
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412).c_str());
						mainGame->gMutex.unlock();
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
					}
				}
				unsigned int remote_port = std::stoi(mainGame->ebJoinPort->getText());
				mainGame->gameConf.lasthost = pstr;
				mainGame->gameConf.lastport = mainGame->ebJoinPort->getText();
				if(DuelClient::StartClient(remote_addr, remote_port, false)) {
					mainGame->btnCreateHost->setEnabled(false);
					mainGame->btnJoinHost->setEnabled(false);
					mainGame->btnJoinCancel->setEnabled(false);
				}
				break;
			}
			case LISTBOX_REPLAY_LIST: {
				if(open_file) {
					ReplayMode::cur_replay.OpenReplay(open_file_name);
					open_file = false;
				} else {
					if(mainGame->lstReplayList->getSelected() == -1 || mainGame->lstReplayList->getSelected() == 0)
						break;
					if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected(),true)))
						break;
				}
				if(mainGame->chkYrp->isChecked() && !ReplayMode::cur_replay.LoadYrp())
					break;
				mainGame->ClearCardInfo();
				mainGame->mTopMenu->setVisible(false);
				mainGame->wCardImg->setVisible(true);
				mainGame->wInfos->setVisible(true);
				mainGame->wReplay->setVisible(true);
				mainGame->wReplayControl->setVisible(true);
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->btnReplayUndo->setVisible(false);
				mainGame->wPhase->setVisible(true);
				mainGame->dField.Clear();
				mainGame->HideElement(mainGame->wReplay);
				mainGame->device->setEventReceiver(&mainGame->dField);
				unsigned int start_turn = _wtoi(mainGame->ebRepStartTurn->getText());
				if(start_turn == 1)
					start_turn = 0;
				ReplayMode::StartReplay(start_turn);
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				if(!open_file && (mainGame->lstSinglePlayList->getSelected() == -1 || mainGame->lstSinglePlayList->getSelected() == 0))
					break;
				mainGame->singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch(id) {
			case CHECKBOX_HP_READY: {
				if(!caller->isEnabled())
					break;
				mainGame->env->setFocus(mainGame->wHostPrepare);
				if(static_cast<irr::gui::IGUICheckBox*>(caller)->isChecked()) {
					bool check = false;
					if (!mainGame->cbDeckSelect2->isVisible())
						check = (mainGame->cbDeckSelect->getSelected() == -1 || !deckManager.LoadDeck(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected())));
					else
						check = (mainGame->cbDeckSelect->getSelected() == -1 || mainGame->cbDeckSelect2->getSelected() == -1 || !deckManager.LoadDeckDouble(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()), mainGame->cbDeckSelect2->getItem(mainGame->cbDeckSelect2->getSelected())));
					if(check) {
						static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
						break;
					}
					UpdateDeck();
					DuelClient::SendPacketToServer(CTOS_HS_READY);
					mainGame->cbDeckSelect->setEnabled(false);
					mainGame->cbDeckSelect2->setEnabled(false);
					if(mainGame->dInfo.isTag || mainGame->dInfo.isRelay)
						mainGame->btnHostPrepDuelist->setEnabled(false);
				} else {
					DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
					mainGame->cbDeckSelect->setEnabled(true);
					mainGame->cbDeckSelect2->setEnabled(true);
					if(mainGame->dInfo.isTag || mainGame->dInfo.isRelay)
						mainGame->btnHostPrepDuelist->setEnabled(true);
				}
				break;
			}
			case CHECKBOX_EXTRA_RULE: {
				mainGame->UpdateExtraRules();
				break;
			}
			case CHECKBOX_PZONE: {
				if(mainGame->chkCustomRules[3]->isChecked())
					mainGame->chkCustomRules[4]->setEnabled(true);
				else {
					mainGame->chkCustomRules[4]->setChecked(false);
					mainGame->chkCustomRules[4]->setEnabled(false);
				}
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
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch(id) {
			case EDITBOX_PORT_BOX: {
				stringw text = caller->getText();
				wchar_t filtered[20];
				int j = 0;
				bool changed = false;
				for(int i = 0; text[i]; i++) {
					if(text[i] >= L'0' && text[i]<= L'9') {
						filtered[j] = text[i];
						j++;
						changed = true;
					}
				}
				filtered[j] = 0;
				if(BufferIO::GetVal(filtered) > 65535) {
					wcscpy(filtered, L"65535");
					changed = true;
				}
				if(changed)
					caller->setText(filtered);
				break;
				}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch (id) {
			case COMBOBOX_DUEL_RULE: {
				auto& combobox = mainGame->cbDuelRule;
				switch (combobox->getSelected()) {
				case 0:{
					combobox->removeItem(4);
					mainGame->duel_param = MASTER_RULE_1;
					mainGame->forbiddentypes = MASTER_RULE_1_FORB;
					break;
				}
				case 1: {
					combobox->removeItem(4);
					mainGame->duel_param = MASTER_RULE_2;
					mainGame->forbiddentypes = MASTER_RULE_2_FORB;
					break;
				}
				case 2: {
					combobox->removeItem(4);
					mainGame->duel_param = MASTER_RULE_3;
					mainGame->forbiddentypes = MASTER_RULE_3_FORB;
					break;
				}
				case 3: {
					combobox->removeItem(4);
					mainGame->duel_param = MASTER_RULE_4;
					mainGame->forbiddentypes = MASTER_RULE_4_FORB;
					break;
				}
				}
			}
			}
		}
		default: break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_R: {
			if(!event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			if(!mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->device->minimizeWindow();
			break;
		}
		case irr::KEY_F12: {
			if (!event.KeyInput.PressedDown)
				Utils::takeScreenshot(mainGame->device);
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

}