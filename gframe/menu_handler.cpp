#include "config.h"
#include "menu_handler.h"
#include "netserver.h"
#include "duelclient.h"
#include "deck_manager.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "image_manager.h"
#include "game.h"
#include "server_lobby.h"

namespace ygo {

void UpdateDeck() {
	mainGame->gameConf.lastdeck = mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected());
	char deckbuf[1024];
	char* pdeck = deckbuf;
	BufferIO::Write<int32_t>(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
	BufferIO::Write<int32_t>(pdeck, deckManager.current_deck.side.size());
	for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
		BufferIO::Write<int32_t>(pdeck, deckManager.current_deck.main[i]->code);
	for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
		BufferIO::Write<int32_t>(pdeck, deckManager.current_deck.extra[i]->code);
	for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
		BufferIO::Write<int32_t>(pdeck, deckManager.current_deck.side[i]->code);
	DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
}
void LoadReplay() {
	auto& replay = ReplayMode::cur_replay;
	if(open_file) {
		bool res = replay.OpenReplay(TEXT("./replay/") + open_file_name);
		open_file = false;
		if(!res || (replay.pheader.id == REPLAY_YRP1 && !mainGame->coreloaded)) {
			if(exit_on_return)
				mainGame->device->closeDevice();
			return;
		}
	} else {
		if(mainGame->lstReplayList->getSelected() == -1)
			return;
		if(!replay.OpenReplay(Utils::ParseFilename(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected(), true))) || (replay.pheader.id == REPLAY_YRP1 && !mainGame->coreloaded))
			return;
	}
	if(mainGame->chkYrp->isChecked() && !replay.yrp)
		return;
	if(replay.pheader.id == REPLAY_YRP1 && (!mainGame->coreloaded || !(replay.pheader.flag & REPLAY_NEWREPLAY)))
		return;
	replay.Rewind();
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
	ReplayMode::StartReplay(start_turn, mainGame->chkYrp->isChecked());
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
			case BUTTON_ONLINE_MULTIPLAYER: {
				mainGame->isHostingOnline = true;
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wRoomListPlaceholder);
				break;
			}
			case BUTTON_LAN_REFRESH2: {
				ServerLobby::RefreshRooms();
				break;
			}
			case BUTTON_JOIN_HOST2: {
				if(wcslen(mainGame->ebNickNameOnline->getText()) <= 0) {
					mainGame->env->addMessageBox(L"Nickname empty", L"Please enter a nickname", true, EMBF_OK, 0, 0);
					break;
				}
				if(mainGame->roomListTable->getSelected() >= 0) {
					mainGame->HideElement(mainGame->wRoomListPlaceholder);
					ServerLobby::JoinServer(false);
				}
				break;
			}
			case BUTTON_JOIN_CANCEL2: {
				mainGame->HideElement(mainGame->wRoomListPlaceholder);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_ROOMPASSWORD_OK: {
				ServerLobby::JoinServer(false);
				mainGame->wRoomPassword->setVisible(false);
				break;
			}
			case BUTTON_ROOMPASSWORD_CANCEL: {
				mainGame->wRoomPassword->setVisible(false);
				mainGame->ShowElement(mainGame->wRoomListPlaceholder);
				break;
			}
			case BUTTON_LAN_MODE: {
				mainGame->isHostingOnline = false;
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->ShowElement(mainGame->wLanWindow);
				break;
			}
			case BUTTON_JOIN_HOST: {
				try {
					auto parsed = DuelClient::ResolveServer(mainGame->ebJoinHost->getText(), mainGame->ebJoinPort->getText());
					mainGame->gameConf.lasthost = mainGame->ebJoinHost->getText();
					mainGame->gameConf.lastport = mainGame->ebJoinPort->getText();
					mainGame->dInfo.secret.pass = BufferIO::EncodeUTF8s(mainGame->ebJoinPass->getText());
					if(DuelClient::StartClient(parsed.first, parsed.second, 0, false)) {
						mainGame->btnCreateHost->setEnabled(false);
						mainGame->btnJoinHost->setEnabled(false);
						mainGame->btnJoinCancel->setEnabled(false);
					}
					break;
				}
				catch(...) {
					mainGame->gMutex.lock();
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412).c_str());
					mainGame->gMutex.unlock();
					break;
				}
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
				mainGame->isHostingOnline = false;
				mainGame->btnHostConfirm->setEnabled(true);
				mainGame->btnHostCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wLanWindow);
				mainGame->stHostPort->setVisible(true);
				mainGame->ebHostPort->setVisible(true);
				mainGame->stHostNotes->setVisible(false);
				mainGame->ebHostNotes->setVisible(false);
				mainGame->ShowElement(mainGame->wCreateHost);
				break;
			}
			case BUTTON_CREATE_HOST2: {
				mainGame->isHostingOnline = true;
				mainGame->btnHostConfirm->setEnabled(true);
				mainGame->btnHostCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wRoomListPlaceholder);
				mainGame->stHostPort->setVisible(false);
				mainGame->ebHostPort->setVisible(false);
				mainGame->stHostNotes->setVisible(true);
				mainGame->ebHostNotes->setVisible(true);
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
					mainGame->duel_param = DUEL_MODE_MR1;
					mainGame->forbiddentypes = DUEL_MODE_MR1_FORB;
					break;
				}
				case 1: {
					mainGame->duel_param = DUEL_MODE_MR2;
					mainGame->forbiddentypes = DUEL_MODE_MR2_FORB;
					break;
				}
				case 2: {
					mainGame->duel_param = DUEL_MODE_MR3;
					mainGame->forbiddentypes = DUEL_MODE_MR3_FORB;
					break;
				}
				case 3: {
					mainGame->duel_param = DUEL_MODE_MR4;
					mainGame->forbiddentypes = DUEL_MODE_MR4_FORB;
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
				if(mainGame->isHostingOnline) {
					ServerLobby::JoinServer(true);
				} else {
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
				}
				break;
			}
			case BUTTON_HOST_CANCEL: {
				mainGame->dInfo.isInLobby = false;
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				if(mainGame->wRules->isVisible())
					mainGame->HideElement(mainGame->wRules);
				mainGame->HideElement(mainGame->wCreateHost);
				if(mainGame->isHostingOnline) {
					mainGame->ShowElement(mainGame->wRoomListPlaceholder);
				} else {
					mainGame->ShowElement(mainGame->wLanWindow);
				}
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
					check = (mainGame->cbDeckSelect->getSelected() == -1 || !deckManager.LoadDeck(Utils::ParseFilename(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()))));
				else
					check = (mainGame->cbDeckSelect->getSelected() == -1 || mainGame->cbDeckSelect2->getSelected() == -1 || !deckManager.LoadDeckDouble(Utils::ParseFilename(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected())), Utils::ParseFilename(mainGame->cbDeckSelect2->getItem(mainGame->cbDeckSelect2->getSelected()))));
				if(check)
					break;
				UpdateDeck();
				DuelClient::SendPacketToServer(CTOS_HS_READY);
				mainGame->cbDeckSelect->setEnabled(false);
				mainGame->cbDeckSelect2->setEnabled(false);
				if(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2)
					mainGame->btnHostPrepDuelist->setEnabled(false);
				break;
			}
			case BUTTON_HP_NOTREADY: {
				DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
				mainGame->cbDeckSelect->setEnabled(true);
				mainGame->cbDeckSelect2->setEnabled(true);
				if(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2)
					mainGame->btnHostPrepDuelist->setEnabled(true);
				break;
			}
			case BUTTON_HP_START: {
				DuelClient::SendPacketToServer(CTOS_HS_START);
				break;
			}
			case BUTTON_HP_CANCEL: {
				DuelClient::StopClient();
				mainGame->dInfo.isInLobby = false;
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->HideElement(mainGame->wHostPrepare);
                mainGame->HideElement(mainGame->gBot.window);
				if(mainGame->wHostPrepare2->isVisible())
					mainGame->HideElement(mainGame->wHostPrepare2);
				if(mainGame->isHostingOnline)
					mainGame->ShowElement(mainGame->wRoomListPlaceholder);
				else
					mainGame->ShowElement(mainGame->wLanWindow);
				mainGame->wChat->setVisible(false);
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_REPLAY_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->stReplayInfo->setText(L"");
				mainGame->btnLoadReplay->setEnabled(false);
				mainGame->btnDeleteReplay->setEnabled(false);
				mainGame->btnRenameReplay->setEnabled(false);
				mainGame->btnExportDeck->setEnabled(false);
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->chkYrp->setChecked(false);
				mainGame->chkYrp->setEnabled(false);
				mainGame->ShowElement(mainGame->wReplay);
				mainGame->RefreshReplay();
				break;
			}
			case BUTTON_SINGLE_MODE: {
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->stSinglePlayInfo->setText(L"");
				mainGame->btnLoadSinglePlay->setEnabled(false);
				mainGame->ShowElement(mainGame->wSinglePlay);
				mainGame->RefreshSingleplay();
				break;
			}
			case BUTTON_LOAD_REPLAY: {
				LoadReplay();
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
			case BUTTON_BOT_START: {
				int port = std::stoi(mainGame->gameConf.serverport);
				int index = mainGame->gBot.deckBox->getSelected();
				if(!NetServer::StartServer(port))
					break;
				if(!DuelClient::StartClient(0x7f000001, port)) {
					NetServer::StopServer();
					break;
				}
				if(!mainGame->bots[index].Launch(port)) {
					NetServer::StopServer();
				}
				break;
			}
			case BUTTON_BOT_ADD: {
				int port = std::stoi(mainGame->gameConf.serverport);
				int index = mainGame->gBot.deckBox->getSelected();
				mainGame->bots[index].Launch(port);
				break;
			}
			case BUTTON_EXPORT_DECK: {
				if(!ReplayMode::cur_replay.IsExportable())
					break;
				auto players = ReplayMode::cur_replay.GetPlayerNames();
				if(players.empty())
					break;
				auto decks = ReplayMode::cur_replay.GetPlayerDecks();
				if(players.size() > decks.size())
					break;
				auto replay_name = Utils::GetFileName(ReplayMode::cur_replay.GetReplayName());
				for(int i = 0; i < decks.size(); i++) {
					deckManager.SaveDeck(fmt::format(TEXT("{} player{:02} {}"), replay_name.c_str(), i, Utils::ParseFilename(players[i]).c_str()), decks[i].main_deck, decks[i].extra_deck, std::vector<int>());
				}
				mainGame->stACMessage->setText(dataManager.GetSysString(1335).c_str());
				mainGame->PopupElement(mainGame->wACMessage, 20);
				break;
			}
			case BUTTON_LOAD_SINGLEPLAY: {
				if(!open_file && mainGame->lstSinglePlayList->getSelected() == -1)
					break;
				SingleMode::singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			case BUTTON_CANCEL_SINGLEPLAY: {
				if(mainGame->dInfo.isSingleMode)
					break;
				mainGame->HideElement(mainGame->wSinglePlay);
				mainGame->ShowElement(mainGame->wMainMenu);
				break;
			}
			case BUTTON_DECK_EDIT: {
				mainGame->RefreshDeck(mainGame->cbDBDecks);
				if(open_file && deckManager.LoadDeck(open_file_name)) {
					auto name = Utils::GetFileName(open_file_name);
					mainGame->ebDeckname->setText(Utils::ToUnicodeIfNeeded(name).c_str());
					mainGame->cbDBDecks->setSelected(-1);
					open_file = false;
				} else if(mainGame->cbDBDecks->getSelected() != -1) {
					deckManager.LoadDeck(Utils::ParseFilename(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected())));
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
					if(Replay::DeleteReplay(Utils::ParseFilename(mainGame->lstReplayList->getListItem(prev_sel, true)))) {
						mainGame->stReplayInfo->setText(L"");
						mainGame->lstReplayList->refreshList();
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
					auto oldname = Utils::ParseFilename(mainGame->lstReplayList->getListItem(prev_sel, true));
					auto oldpath = oldname.substr(0, oldname.find_last_of(TEXT("/"))) + TEXT("/");
					if(Replay::RenameReplay(oldname, oldpath + Utils::ParseFilename(mainGame->ebRSName->getText()))) {
						mainGame->lstReplayList->refreshList();
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
				mainGame->stReplayInfo->setText(L"");
				mainGame->btnLoadReplay->setEnabled(false);
				mainGame->btnDeleteReplay->setEnabled(false);
				mainGame->btnRenameReplay->setEnabled(false);
				mainGame->btnExportDeck->setEnabled(false);
				if(sel == -1)
					break;
				auto& replay = ReplayMode::cur_replay;
				if(!replay.OpenReplay(Utils::ParseFilename(mainGame->lstReplayList->getListItem(sel, true))))
					break;
				bool has_yrp = (replay.pheader.id == REPLAY_YRPX) && (replay.yrp != nullptr);
				if(!(replay.pheader.id == REPLAY_YRP1 && (!mainGame->coreloaded || !(replay.pheader.flag & REPLAY_NEWREPLAY))))
					mainGame->btnLoadReplay->setEnabled(true);
				mainGame->btnDeleteReplay->setEnabled(true);
				mainGame->btnRenameReplay->setEnabled(true);
				mainGame->btnExportDeck->setEnabled(replay.IsExportable());
				std::wstring repinfo;
				time_t curtime = replay.pheader.seed;
				tm* st = localtime(&curtime);
				repinfo.append(fmt::format(L"{}/{}/{} {:02}:{:02}:{:02}\n", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec).c_str());
				auto names = replay.GetPlayerNames();
				for(int i = 0; i < replay.GetPlayersCount(0); i++) {
					repinfo.append(names[i] + L"\n");
				}
				repinfo.append(L"===VS===\n");
				for(int i = 0; i < replay.GetPlayersCount(1); i++) {
					repinfo.append(names[i + replay.GetPlayersCount(0)] + L"\n");
				}
				mainGame->ebRepStartTurn->setText(L"1");
				mainGame->stReplayInfo->setText((wchar_t*)repinfo.c_str());
				mainGame->chkYrp->setChecked(false);
				mainGame->chkYrp->setEnabled(has_yrp && mainGame->coreloaded);
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				mainGame->btnLoadSinglePlay->setEnabled(false);
				int sel = mainGame->lstSinglePlayList->getSelected();
				mainGame->stSinglePlayInfo->setText(L"");
				if(sel == -1)
					break;
				mainGame->btnLoadSinglePlay->setEnabled(true);
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
				int sel = mainGame->lstHostList->getSelected();
				if(sel == -1)
					break;
				try {
					auto parsed = DuelClient::ResolveServer(mainGame->ebJoinHost->getText(), mainGame->ebJoinPort->getText());
					mainGame->gameConf.lasthost = mainGame->ebJoinHost->getText();
					mainGame->gameConf.lastport = mainGame->ebJoinPort->getText();
					mainGame->dInfo.secret.pass = BufferIO::EncodeUTF8s(mainGame->ebJoinPass->getText());
					if(DuelClient::StartClient(parsed.first, parsed.second, 0, false)) {
						mainGame->btnCreateHost->setEnabled(false);
						mainGame->btnJoinHost->setEnabled(false);
						mainGame->btnJoinCancel->setEnabled(false);
					}
					break;
				}
				catch(...) {
					mainGame->gMutex.lock();
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1412).c_str());
					mainGame->gMutex.unlock();
					break;
				}
			}
			case LISTBOX_REPLAY_LIST: {
				LoadReplay();
				break;
			}
			case LISTBOX_SINGLEPLAY_LIST: {
				if(!open_file && (mainGame->lstSinglePlayList->getSelected() == -1))
					break;
				SingleMode::singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch(id) {
			case CHECK_SHOW_LOCKED_ROOMS: {
				ServerLobby::FillOnlineRooms();
				break;
			}
			case CHECK_SHOW_ACTIVE_ROOMS: {
				ServerLobby::RefreshRooms();
				break;
			}
			case CHECKBOX_HP_READY: {
				if(!caller->isEnabled())
					break;
				mainGame->env->setFocus(mainGame->wHostPrepare);
				if(static_cast<irr::gui::IGUICheckBox*>(caller)->isChecked()) {
					bool check = false;
					if (!mainGame->cbDeckSelect2->isVisible())
						check = (mainGame->cbDeckSelect->getSelected() == -1 || !deckManager.LoadDeck(Utils::ParseFilename(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected()))));
					else
						check = (mainGame->cbDeckSelect->getSelected() == -1 || mainGame->cbDeckSelect2->getSelected() == -1 || !deckManager.LoadDeckDouble(Utils::ParseFilename(mainGame->cbDeckSelect->getItem(mainGame->cbDeckSelect->getSelected())), Utils::ParseFilename(mainGame->cbDeckSelect2->getItem(mainGame->cbDeckSelect2->getSelected()))));
					if(check) {
						static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
						break;
					}
					UpdateDeck();
					DuelClient::SendPacketToServer(CTOS_HS_READY);
					mainGame->cbDeckSelect->setEnabled(false);
					mainGame->cbDeckSelect2->setEnabled(false);
					if(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2)
						mainGame->btnHostPrepDuelist->setEnabled(false);
				} else {
					DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
					mainGame->cbDeckSelect->setEnabled(true);
					mainGame->cbDeckSelect2->setEnabled(true);
					if(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2)
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
			case EDITBOX_TEAM_COUNT: {
				auto elem = static_cast<irr::gui::IGUIEditBox*>(event.GUIEvent.Caller);
				wchar_t* min = L"1";
				if(elem == mainGame->ebOnlineTeam1 || elem == mainGame->ebOnlineTeam2)
					min = L"0";
				auto text = elem->getText();
				auto len = wcslen(text);
				if(len < 1)
					break;
				if(text[len - 1] < min[0] || text[len - 1] > L'3') {
					elem->setText(min);
					break;
				}
				wchar_t string[] = { text[len - 1], 0 };
				elem->setText(string);
				break;
			}
			case EDITBOX_NICKNAME: {
				auto elem = static_cast<irr::gui::IGUIEditBox*>(event.GUIEvent.Caller);
				auto target = (elem == mainGame->ebNickNameOnline) ? mainGame->ebNickName : mainGame->ebNickNameOnline;
				target->setText(elem->getText());
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch (id) {
			case COMBOBOX_DUEL_RULE: {
				auto combobox = static_cast<irr::gui::IGUIComboBox*>(event.GUIEvent.Caller);
				switch (combobox->getSelected()) {
				case 0:{
					combobox->removeItem(4);
					mainGame->duel_param = DUEL_MODE_MR1;
					mainGame->forbiddentypes = DUEL_MODE_MR1_FORB;
					break;
				}
				case 1: {
					combobox->removeItem(4);
					mainGame->duel_param = DUEL_MODE_MR2;
					mainGame->forbiddentypes = DUEL_MODE_MR2_FORB;
					break;
				}
				case 2: {
					combobox->removeItem(4);
					mainGame->duel_param = DUEL_MODE_MR3;
					mainGame->forbiddentypes = DUEL_MODE_MR3_FORB;
					break;
				}
				case 3: {
					combobox->removeItem(4);
					mainGame->duel_param = DUEL_MODE_MR4;
					mainGame->forbiddentypes = DUEL_MODE_MR4_FORB;
					break;
				}
				}
			}
			case COMBOBOX_MATCH_MODE: {
				auto combobox = static_cast<irr::gui::IGUIComboBox*>(event.GUIEvent.Caller);
				if(combobox->getSelected() == 4) {
					mainGame->ebTeam1->setVisible(true);
					mainGame->ebTeam2->setVisible(true);
				} else {
					mainGame->ebTeam1->setVisible(false);
					mainGame->ebTeam2->setVisible(false);
				}
			}
			case COMBOBOX_BOT_DECK: {
				mainGame->gBot.UpdateDeckDescription();
			}
			}
		}
		case EGET_TABLE_SELECTED_AGAIN: {
			switch(id) {
			case TABLE_ROOMLIST: {
				if(wcslen(mainGame->ebNickNameOnline->getText()) <= 0) {
					mainGame->env->addMessageBox(L"Nickname empty", L"Please enter a nickname", true, EMBF_OK, 0, 0);
					break;
				}
				if(mainGame->roomListTable->getSelected() >= 0) {
					mainGame->HideElement(mainGame->wRoomListPlaceholder);
					ServerLobby::JoinServer(false);
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
		case irr::KEY_F5: {
			if(!event.KeyInput.PressedDown && mainGame->wRoomListPlaceholder->isVisible())
				ServerLobby::RefreshRooms();
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
