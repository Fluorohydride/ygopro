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

bool MenuHandler::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		irr::gui::IGUIElement* caller = event.GUIEvent.Caller;
		s32 id = caller->getID();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_MODE_EXIT: {
				mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_MODE: {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_JOIN_HOST: {
			#if WINVER >= 0x0600
				struct addrinfo hints, *servinfo;
				memset(&hints, 0, sizeof(struct addrinfo));
				hints.ai_family = AF_INET;			/* Allow IPv4 or IPv6 */
				hints.ai_socktype = SOCK_STREAM;	/* Datagram socket */
				hints.ai_flags = AI_PASSIVE;		/* For wildcard IP address */
				hints.ai_protocol = 0;				/* Any protocol */
				hints.ai_canonname = NULL;
				hints.ai_addr = NULL;
				hints.ai_next = NULL;
				int status;
				char hostname[100];
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinIP->getText();
				BufferIO::CopyWStr(pstr, hostname, 100);
				if ((status = getaddrinfo(hostname, NULL, &hints, &servinfo)) == -1) {
					fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
					//error handling
					BufferIO::CopyWStr(pstr, ip, 16);
				} else
					inet_ntop(AF_INET, &(((struct sockaddr_in *)servinfo->ai_addr)->sin_addr), ip, 20);
				freeaddrinfo(servinfo);
			#else
				char hostname[100];
				char ip[20];
				const wchar_t* pstr = mainGame->ebJoinIP->getText();
				BufferIO::CopyWStr(pstr, hostname, 100);
				BufferIO::CopyWStr(pstr, ip, 16);
			#endif
				unsigned int remote_addr = htonl(inet_addr(ip));
				unsigned int remote_port = _wtoi(mainGame->ebJoinPort->getText());
				BufferIO::CopyWStr(pstr, mainGame->gameConf.lastip, 20);
				BufferIO::CopyWStr(mainGame->ebJoinPort->getText(), mainGame->gameConf.lastport, 20);
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
			case BUTTON_HOST_CONFIRM: {
				BufferIO::CopyWStr(mainGame->ebServerName->getText(), mainGame->gameConf.gamename, 20);
				if(!NetServer::StartServer(mainGame->gameConf.serverport))
					break;
				if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
					NetServer::StopServer();
					break;
				}
				mainGame->btnHostConfirm->setEnabled(false);
				mainGame->btnHostCancel->setEnabled(false);
				break;
			}
			case BUTTON_HOST_CANCEL: {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wCreateHost);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_HP_DUELIST: {
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
			case BUTTON_HP_START: {
				if(!mainGame->chkHostPrepReady[0]->isChecked()
				        || !mainGame->chkHostPrepReady[1]->isChecked())
					break;
				DuelClient::SendPacketToServer(CTOS_HS_START);
				break;
			}
			case BUTTON_HP_CANCEL: {
				DuelClient::StopClient();
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wHostPrepare);
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
				if(mainGame->lstReplayList->getSelected() == -1)
					break;
				if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected())))
					break;
				mainGame->imgCard->setImage(imageManager.tCover[0]);
				mainGame->wCardImg->setVisible(true);
				mainGame->wInfos->setVisible(true);
				mainGame->wReplay->setVisible(true);
				mainGame->stName->setText(L"");
				mainGame->stInfo->setText(L"");
				mainGame->stDataInfo->setText(L"");
				mainGame->stSetName->setText(L"");
				mainGame->stText->setText(L"");
				mainGame->scrCardText->setVisible(false);
				mainGame->wReplayControl->setVisible(true);
				mainGame->btnReplayStart->setVisible(false);
				mainGame->btnReplayPause->setVisible(true);
				mainGame->btnReplayStep->setVisible(false);
				mainGame->btnReplayUndo->setVisible(false);
				mainGame->wPhase->setVisible(true);
				mainGame->dField.panel = 0;
				mainGame->dField.hovered_card = 0;
				mainGame->dField.clicked_card = 0;
				mainGame->dField.Clear();
				mainGame->HideElement(mainGame->wReplay);
				mainGame->device->setEventReceiver(&mainGame->dField);
				unsigned int start_turn = _wtoi(mainGame->ebRepStartTurn->getText());
				if(start_turn == 1)
					start_turn = 0;
				ReplayMode::StartReplay(start_turn);
				break;
			}
			case BUTTON_CANCEL_REPLAY: {
				mainGame->HideElement(mainGame->wReplay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_LOAD_SINGLEPLAY: {
				if(mainGame->lstSinglePlayList->getSelected() == -1)
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
				if(mainGame->cbDBDecks->getSelected() != -1)
					deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->is_building = true;
				mainGame->is_siding = false;
				mainGame->wInfos->setVisible(true);
				mainGame->wCardImg->setVisible(true);
				mainGame->wDeckEdit->setVisible(true);
				mainGame->wFilter->setVisible(true);
				mainGame->wSort->setVisible(true);
				mainGame->btnLeaveGame->setVisible(true);
				mainGame->btnLeaveGame->setText(dataManager.GetSysString(1306));
				mainGame->btnSideOK->setVisible(false);
				mainGame->deckBuilder.filterList = deckManager._lfList[0].content;
				mainGame->cbDBLFList->setSelected(0);
				mainGame->cbCardType->setSelected(0);
				mainGame->cbCardType2->setSelected(0);
				mainGame->cbAttribute->setSelected(0);
				mainGame->cbRace->setSelected(0);
				mainGame->ebAttack->setText(L"");
				mainGame->ebDefense->setText(L"");
				mainGame->ebStar->setText(L"");
				mainGame->ebScale->setText(L"");
				mainGame->cbCardType2->setEnabled(false);
				mainGame->cbAttribute->setEnabled(false);
				mainGame->cbRace->setEnabled(false);
				mainGame->ebAttack->setEnabled(false);
				mainGame->ebDefense->setEnabled(false);
				mainGame->ebStar->setEnabled(false);
				mainGame->ebScale->setEnabled(false);
				mainGame->deckBuilder.filter_effect = 0;
				mainGame->deckBuilder.result_string[0] = L'0';
				mainGame->deckBuilder.result_string[1] = 0;
				mainGame->deckBuilder.results.clear();
				mainGame->deckBuilder.is_draging = false;
				mainGame->device->setEventReceiver(&mainGame->deckBuilder);
				for(int i = 0; i < 32; ++i)
					mainGame->chkCategory[i]->setChecked(false);
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
				wchar_t buf[20];
				myswprintf(buf, L"%d.%d.%d.%d", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
				mainGame->ebJoinIP->setText(buf);
				myswprintf(buf, L"%d", port);
				mainGame->ebJoinPort->setText(buf);
				break;
			}
			case LISTBOX_REPLAY_LIST: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel == -1)
					break;
				if(!ReplayMode::cur_replay.OpenReplay(mainGame->lstReplayList->getListItem(sel)))
					break;
				wchar_t infobuf[256];
				std::wstring repinfo;
				time_t curtime = ReplayMode::cur_replay.pheader.seed;
				tm* st = localtime(&curtime);
				myswprintf(infobuf, L"%d/%d/%d %02d:%02d:%02d\n", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
				repinfo.append(infobuf);
				wchar_t namebuf[4][20];
				BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[0], namebuf[0], 20);
				BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[40], namebuf[1], 20);
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_TAG) {
					BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[80], namebuf[2], 20);
					BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[120], namebuf[3], 20);
				}
				if(ReplayMode::cur_replay.pheader.flag & REPLAY_TAG)
					myswprintf(infobuf, L"%ls\n%ls\n===VS===\n%ls\n%ls\n", namebuf[0], namebuf[1], namebuf[2], namebuf[3]);
				else
					myswprintf(infobuf, L"%ls\n===VS===\n%ls\n", namebuf[0], namebuf[1]);
				repinfo.append(infobuf);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->SetStaticText(mainGame->stReplayInfo, 180, mainGame->guiFont, (wchar_t*)repinfo.c_str());
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
					if(mainGame->cbDeckSelect->getSelected() == -1 ||
					        !deckManager.LoadDeck(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()))) {
						static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
						break;
					}
					BufferIO::CopyWStr(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()),
					                   mainGame->gameConf.lastdeck, 64);
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
					DuelClient::SendPacketToServer(CTOS_HS_READY);
					mainGame->cbDeckSelect->setEnabled(false);
				} else {
					DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
					mainGame->cbDeckSelect->setEnabled(true);
				}
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
		default: break;
		}
		break;
	}
	default: break;
	}
	return false;
}

}
