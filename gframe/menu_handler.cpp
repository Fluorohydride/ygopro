#include "config.h"
#include "menu_handler.h"
#include "myfilesystem.h"
#include "netserver.h"
#include "duelclient.h"
#include "deck_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "image_manager.h"
#include "sound_manager.h"
#include "game.h"

namespace ygo {

void UpdateDeck() {
	BufferIO::CopyWideString(mainGame->cbCategorySelect->getText(), mainGame->gameConf.lastcategory);
	BufferIO::CopyWideString(mainGame->cbDeckSelect->getText(), mainGame->gameConf.lastdeck);
	unsigned char deckbuf[1024]{};
	auto pdeck = deckbuf;
	BufferIO::Write<int32_t>(pdeck, static_cast<int32_t>(deckManager.current_deck.main.size() + deckManager.current_deck.extra.size()));
	BufferIO::Write<int32_t>(pdeck, static_cast<int32_t>(deckManager.current_deck.side.size()));
	for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
		BufferIO::Write<uint32_t>(pdeck, deckManager.current_deck.main[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
		BufferIO::Write<uint32_t>(pdeck, deckManager.current_deck.extra[i]->first);
	for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
		BufferIO::Write<uint32_t>(pdeck, deckManager.current_deck.side[i]->first);
	DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
}
bool MenuHandler::OnEvent(const irr::SEvent& event) {
	if(mainGame->dField.OnCommonEvent(event))
		return false;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		irr::gui::IGUIElement* caller = event.GUIEvent.Caller;
		irr::s32 id = caller->getID();
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO) {
			mainGame->wQuery->getParent()->bringToFront(mainGame->wQuery);
			break;
		}
		if(mainGame->wReplaySave->isVisible() && id != BUTTON_REPLAY_SAVE && id != BUTTON_REPLAY_CANCEL) {
			mainGame->wReplaySave->getParent()->bringToFront(mainGame->wReplaySave);
			break;
		}
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			if(id < 110)
				soundManager.PlaySoundEffect(SOUND_MENU);
			else
				soundManager.PlaySoundEffect(SOUND_BUTTON);
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
				bot_mode = false;
				mainGame->TrimText(mainGame->ebJoinHost);
				mainGame->TrimText(mainGame->ebJoinPort);
				char ip[20];
				wchar_t pstr[100];
				wchar_t portstr[10];
				BufferIO::CopyWideString(mainGame->ebJoinHost->getText(), pstr);
				BufferIO::CopyWideString(mainGame->ebJoinPort->getText(), portstr);
				BufferIO::EncodeUTF8(pstr, ip);
				unsigned int remote_addr = htonl(inet_addr(ip));
				if(remote_addr == -1) {
					char hostname[100];
					char port[6];
					BufferIO::EncodeUTF8(pstr, hostname);
					BufferIO::EncodeUTF8(portstr, port);
					struct evutil_addrinfo hints;
					struct evutil_addrinfo *answer = nullptr;
					std::memset(&hints, 0, sizeof hints);
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;
					hints.ai_flags = EVUTIL_AI_ADDRCONFIG;
					int status = evutil_getaddrinfo(hostname, port, &hints, &answer);
					if(status != 0) {
						mainGame->gMutex.lock();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412));
						mainGame->gMutex.unlock();
						break;
					} else {
						sockaddr_in * sin = ((struct sockaddr_in *)answer->ai_addr);
						evutil_inet_ntop(AF_INET, &(sin->sin_addr), ip, 20);
						remote_addr = htonl(inet_addr(ip));
						evutil_freeaddrinfo(answer);
					}
				}
				unsigned int remote_port = std::wcstol(portstr, nullptr, 10);
				BufferIO::CopyWideString(pstr, mainGame->gameConf.lasthost);
				BufferIO::CopyWideString(portstr, mainGame->gameConf.lastport);
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
			case BUTTON_HOST_CONFIRM: {
				bot_mode = false;
				BufferIO::CopyWideString(mainGame->ebServerName->getText(), mainGame->gameConf.gamename);
				if(!NetServer::StartServer(mainGame->gameConf.serverport)) {
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
					break;
				}
				if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
					NetServer::StopServer();
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
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
				mainGame->cbCategorySelect->setEnabled(true);
				mainGame->cbDeckSelect->setEnabled(true);
				DuelClient::SendPacketToServer(CTOS_HS_TODUELIST);
				break;
			}
			case BUTTON_HP_OBSERVER: {
				DuelClient::SendPacketToServer(CTOS_HS_TOOBSERVER);
				break;
			}
			case BUTTON_HP_KICK: {
				int index = 0;
				while(index < 4) {
					if(mainGame->btnHostPrepKick[index] == caller)
						break;
					++index;
				}
				CTOS_Kick csk;
				csk.pos = index;
				DuelClient::SendPacketToServer(CTOS_HS_KICK, csk);
				break;
			}
			case BUTTON_HP_READY: {
				if(mainGame->cbCategorySelect->getSelected() == -1 || mainGame->cbDeckSelect->getSelected() == -1 ||
					!deckManager.LoadCurrentDeck(mainGame->cbCategorySelect->getSelected(), mainGame->cbCategorySelect->getText(), mainGame->cbDeckSelect->getText())) {
					mainGame->gMutex.lock();
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1406));
					mainGame->gMutex.unlock();
					break;
				}
				UpdateDeck();
				DuelClient::SendPacketToServer(CTOS_HS_READY);
				mainGame->cbCategorySelect->setEnabled(false);
				mainGame->cbDeckSelect->setEnabled(false);
				break;
			}
			case BUTTON_HP_NOTREADY: {
				DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
				mainGame->cbCategorySelect->setEnabled(true);
				mainGame->cbDeckSelect->setEnabled(true);
				break;
			}
			case BUTTON_HP_START: {
				DuelClient::SendPacketToServer(CTOS_HS_START);
				break;
			}
			case BUTTON_HP_CANCEL: {
				DuelClient::StopClient();
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->btnStartBot->setEnabled(true);
				mainGame->btnBotCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wHostPrepare);
				if(bot_mode)
					mainGame->ShowElement(mainGame->wSinglePlay);
				else
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
				mainGame->RefreshBot();
				break;
			}
			case BUTTON_LOAD_REPLAY: {
				int start_turn = 1;
				if(open_file) {
					open_file = false;
					if (!ReplayMode::cur_replay.OpenReplay(open_file_name)) {
						if (exit_on_return)
							mainGame->device->closeDevice();
						break;
					}
				} else {
					auto selected = mainGame->lstReplayList->getSelected();
					if(selected == -1)
						break;
					wchar_t replay_path[256]{};
					myswprintf(replay_path, L"./replay/%ls", mainGame->lstReplayList->getListItem(selected));
					if (!ReplayMode::cur_replay.OpenReplay(replay_path))
						break;
					start_turn = std::wcstol(mainGame->ebRepStartTurn->getText(), nullptr, 10);
				}
				mainGame->ClearCardInfo();
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
				wchar_t textBuffer[256];
				myswprintf(textBuffer, L"%ls\n%ls", mainGame->lstReplayList->getListItem(sel), dataManager.GetSysString(1363));
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->guiFont, textBuffer);
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
				mainGame->wReplaySave->setText(dataManager.GetSysString(1364));
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
			case BUTTON_EXPORT_DECK: {
				auto selected = mainGame->lstReplayList->getSelected();
				if(selected == -1)
					break;
				Replay replay;
				wchar_t replay_filename[256]{};
				wchar_t namebuf[4][20]{};
				wchar_t filename[256]{};
				wchar_t replay_path[256]{};
				BufferIO::CopyWideString(mainGame->lstReplayList->getListItem(selected), replay_filename);
				myswprintf(replay_path, L"./replay/%ls", replay_filename);
				if (!replay.OpenReplay(replay_path))
					break;
				if (replay.pheader.base.flag & REPLAY_SINGLE_MODE)
					break;
				for (size_t i = 0; i < replay.decks.size(); ++i) {
					BufferIO::CopyWideString(replay.players[Replay::GetDeckPlayer(i)].c_str(), namebuf[i]);
					FileSystem::SafeFileName(namebuf[i]);
				}
				for (size_t i = 0; i < replay.decks.size(); ++i) {
					myswprintf(filename, L"./deck/%ls-%d %ls.ydk", replay_filename, i + 1, namebuf[i]);
					DeckManager::SaveDeckArray(replay.decks[i], filename);
				}
				mainGame->stACMessage->setText(dataManager.GetSysString(1335));
				mainGame->PopupElement(mainGame->wACMessage, 20);
				break;
			}
			case BUTTON_BOT_START: {
				int sel = mainGame->lstBotList->getSelected();
				if(sel == -1)
					break;
				bot_mode = true;
#ifdef _WIN32
				if(!NetServer::StartServer(mainGame->gameConf.serverport)) {
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
					break;
				}
				if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
					NetServer::StopServer();
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
					break;
				}
				STARTUPINFOW si;
				PROCESS_INFORMATION pi;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));
				wchar_t cmd[MAX_PATH];
				wchar_t arg1[512];
				if(mainGame->botInfo[sel].select_deckfile) {
					wchar_t botdeck[256];
					DeckManager::GetDeckFile(botdeck, mainGame->cbBotDeckCategory->getSelected(), mainGame->cbBotDeckCategory->getText(), mainGame->cbBotDeck->getText());
					myswprintf(arg1, L"%ls DeckFile='%ls'", mainGame->botInfo[sel].command, botdeck);
				}
				else
					myswprintf(arg1, L"%ls", mainGame->botInfo[sel].command);
				int flag = 0;
				flag += (mainGame->chkBotHand->isChecked() ? 0x1 : 0);
				myswprintf(cmd, L"Bot.exe \"%ls\" %d %d", arg1, flag, mainGame->gameConf.serverport);
				if(!CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
				{
					NetServer::StopServer();
					break;
				}
#else
				if(fork() == 0) {
					usleep(100000);
					wchar_t warg1[512];
					if(mainGame->botInfo[sel].select_deckfile) {
						wchar_t botdeck[256];
						DeckManager::GetDeckFile(botdeck, mainGame->cbBotDeckCategory->getSelected(), mainGame->cbBotDeckCategory->getText(), mainGame->cbBotDeck->getText());
						myswprintf(warg1, L"%ls DeckFile='%ls'", mainGame->botInfo[sel].command, botdeck);
					}
					else
						myswprintf(warg1, L"%ls", mainGame->botInfo[sel].command);
					char arg1[512];
					BufferIO::EncodeUTF8(warg1, arg1);
					int flag = 0;
					flag += (mainGame->chkBotHand->isChecked() ? 0x1 : 0);
					char arg2[8];
					std::snprintf(arg2, sizeof arg2, "%d", flag);
					char arg3[8];
					std::snprintf(arg3, sizeof arg3, "%d", mainGame->gameConf.serverport);
					execl("./bot", "bot", arg1, arg2, arg3, nullptr);
					std::exit(0);
				} else {
					if(!NetServer::StartServer(mainGame->gameConf.serverport)) {
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
						break;
					}
					if(!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
						NetServer::StopServer();
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
						break;
					}
				}
#endif
				mainGame->btnStartBot->setEnabled(false);
				mainGame->btnBotCancel->setEnabled(false);
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
				mainGame->RefreshCategoryDeck(mainGame->cbDBCategory, mainGame->cbDBDecks);
				if(open_file && deckManager.LoadCurrentDeck(open_file_name)) {
#ifdef _WIN32
					wchar_t *dash = std::wcsrchr(open_file_name, L'\\');
#else
					wchar_t *dash = std::wcsrchr(open_file_name, L'/');
#endif
					wchar_t *dot = std::wcsrchr(open_file_name, L'.');
					if(dash && dot && !mywcsncasecmp(dot, L".ydk", 4)) { // full path
						wchar_t deck_name[256];
						BufferIO::CopyWideString(dash + 1, deck_name, dot - dash - 1);
						mainGame->ebDeckname->setText(deck_name);
						mainGame->cbDBCategory->setSelected(-1);
						mainGame->cbDBDecks->setSelected(-1);
						mainGame->btnManageDeck->setEnabled(false);
						mainGame->cbDBCategory->setEnabled(false);
						mainGame->cbDBDecks->setEnabled(false);
					} else if(dash) { // has category
						wchar_t deck_name[256];
						BufferIO::CopyWideString(dash + 1, deck_name);
						for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
							if(!std::wcscmp(mainGame->cbDBDecks->getItem(i), deck_name)) {
								BufferIO::CopyWideString(deck_name, mainGame->gameConf.lastdeck);
								mainGame->cbDBDecks->setSelected(i);
								break;
							}
						}
					} else { // only deck name
						for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
							if(!std::wcscmp(mainGame->cbDBDecks->getItem(i), open_file_name)) {
								BufferIO::CopyWideString(open_file_name, mainGame->gameConf.lastdeck);
								mainGame->cbDBDecks->setSelected(i);
								break;
							}
						}
					}
					open_file = false;
				} 
				else if(mainGame->cbDBCategory->getSelected() != -1 && mainGame->cbDBDecks->getSelected() != -1) {
					deckManager.LoadCurrentDeck(mainGame->cbDBCategory->getSelected(), mainGame->cbDBCategory->getText(), mainGame->cbDBDecks->getText());
					mainGame->ebDeckname->setText(L"");
				}
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->deckBuilder.Initialize();
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
					wchar_t newname[256];
					BufferIO::CopyWideString(mainGame->ebRSName->getText(), newname);
					if(mywcsncasecmp(newname + std::wcslen(newname) - 4, L".yrp", 4)) {
						myswprintf(newname, L"%ls.yrp", mainGame->ebRSName->getText());
					}
					if(Replay::RenameReplay(mainGame->lstReplayList->getListItem(prev_sel), newname)) {
						mainGame->lstReplayList->setItem(prev_sel, newname, -1);
					} else {
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1365));
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
				wchar_t buf[20];
				myswprintf(buf, L"%d.%d.%d.%d", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
				mainGame->ebJoinHost->setText(buf);
				myswprintf(buf, L"%d", port);
				mainGame->ebJoinPort->setText(buf);
				break;
			}
			case LISTBOX_REPLAY_LIST: {
				int sel = mainGame->lstReplayList->getSelected();
				if(sel < 0)
					break;
				auto filename = mainGame->lstReplayList->getListItem(sel);
				if (!filename)
					break;
				wchar_t replay_path[256]{};
				myswprintf(replay_path, L"./replay/%ls", filename);
				if (!temp_replay.OpenReplay(replay_path)) {
					mainGame->stReplayInfo->setText(L"Error");
					break;
				}
				wchar_t infobuf[256]{};
				std::wstring repinfo;
				time_t curtime;
				const auto& rh = temp_replay.pheader.base;
				if(temp_replay.pheader.base.flag & REPLAY_UNIFORM)
					curtime = rh.start_time;
				else{
					curtime = rh.seed;
					wchar_t version_info[256]{};
					myswprintf(version_info, L"version 0x%X\n", rh.version);
					repinfo.append(version_info);
				}
				std::wcsftime(infobuf, sizeof infobuf / sizeof infobuf[0], L"%Y/%m/%d %H:%M:%S\n", std::localtime(&curtime));
				repinfo.append(infobuf);
				if (rh.flag & REPLAY_SINGLE_MODE) {
					wchar_t path[256]{};
					BufferIO::DecodeUTF8(temp_replay.script_name.c_str(), path);
					repinfo.append(path);
					repinfo.append(L"\n");
				}
				const auto& player_names = temp_replay.players;
				if(rh.flag & REPLAY_TAG)
					myswprintf(infobuf, L"%ls\n%ls\n===VS===\n%ls\n%ls\n", player_names[0].c_str(), player_names[1].c_str(), player_names[2].c_str(), player_names[3].c_str());
				else
					myswprintf(infobuf, L"%ls\n===VS===\n%ls\n", player_names[0].c_str(), player_names[1].c_str());
				repinfo.append(infobuf);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->SetStaticText(mainGame->stReplayInfo, 180, mainGame->guiFont, repinfo.c_str());
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				int sel = mainGame->lstSinglePlayList->getSelected();
				if(sel == -1)
					break;
				const wchar_t* name = mainGame->lstSinglePlayList->getListItem(sel);
				wchar_t fname[256];
				myswprintf(fname, L"./single/%ls", name);
				FILE* fp = mywfopen(fname, "rb");
				if(!fp) {
					mainGame->stSinglePlayInfo->setText(L"");
					break;
				}
				char linebuf[1024];
				wchar_t wlinebuf[1024];
				std::wstring message = L"";
				bool in_message = false;
				while(std::fgets(linebuf, 1024, fp)) {
					if(!std::strncmp(linebuf, "--[[message", 11)) {
						size_t len = std::strlen(linebuf);
						char* msgend = std::strrchr(linebuf, ']');
						if(len <= 13) {
							in_message = true;
							continue;
						} else if(len > 15 && msgend) {
							*(msgend - 1) = '\0';
							BufferIO::DecodeUTF8(linebuf + 12, wlinebuf);
							message.append(wlinebuf);
							break;
						}
					}
					if(!std::strncmp(linebuf, "]]", 2)) {
						in_message = false;
						break;
					}
					if(in_message) {
						BufferIO::DecodeUTF8(linebuf, wlinebuf);
						message.append(wlinebuf);
					}
				}
				std::fclose(fp);
				mainGame->SetStaticText(mainGame->stSinglePlayInfo, 200, mainGame->guiFont, message.c_str());
				break;
			}
			case LISTBOX_BOT_LIST: {
				int sel = mainGame->lstBotList->getSelected();
				if(sel == -1)
					break;
				mainGame->SetStaticText(mainGame->stBotInfo, 200, mainGame->guiFont, mainGame->botInfo[sel].desc);
				mainGame->cbBotDeckCategory->setVisible(mainGame->botInfo[sel].select_deckfile);
				mainGame->cbBotDeck->setVisible(mainGame->botInfo[sel].select_deckfile);
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
					if(mainGame->cbCategorySelect->getSelected() == -1 || mainGame->cbDeckSelect->getSelected() == -1 ||
						!deckManager.LoadCurrentDeck(mainGame->cbCategorySelect->getSelected(), mainGame->cbCategorySelect->getText(), mainGame->cbDeckSelect->getText())) {
						mainGame->gMutex.lock();
						static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
						soundManager.PlaySoundEffect(SOUND_INFO);
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1406));
						mainGame->gMutex.unlock();
						break;
					}
					UpdateDeck();
					DuelClient::SendPacketToServer(CTOS_HS_READY);
					mainGame->cbCategorySelect->setEnabled(false);
					mainGame->cbDeckSelect->setEnabled(false);
				} else {
					DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
					mainGame->cbCategorySelect->setEnabled(true);
					mainGame->cbDeckSelect->setEnabled(true);
				}
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_BOT_RULE: {
				mainGame->RefreshBot();
				break;
			}
			case COMBOBOX_HP_CATEGORY: {
				int catesel = mainGame->cbCategorySelect->getSelected();
				if(catesel == 3) {
					catesel = 2;
					mainGame->cbCategorySelect->setSelected(2);
				}
				if(catesel >= 0) {
					mainGame->RefreshDeck(mainGame->cbCategorySelect, mainGame->cbDeckSelect);
					mainGame->cbDeckSelect->setSelected(0);
				}
				break;
			}
			case COMBOBOX_BOT_DECKCATEGORY: {
				int catesel = mainGame->cbBotDeckCategory->getSelected();
				if(catesel == 3) {
					catesel = 2;
					mainGame->cbBotDeckCategory->setSelected(2);
				}
				if(catesel >= 0) {
					mainGame->RefreshDeck(mainGame->cbBotDeckCategory, mainGame->cbBotDeck);
					mainGame->cbBotDeck->setSelected(0);
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
	default: break;
	}
	return false;
}

}
