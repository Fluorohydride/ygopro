#include "duelclient.h"
#include "client_card.h"
#include "materials.h"
#include "image_manager.h"
#include "single_mode.h"
#include "game.h"
#include "replay.h"
#include "replay_mode.h"
#include <algorithm>

namespace ygo {

unsigned DuelClient::connect_state = 0;
std::vector<unsigned char> DuelClient::response_buf;
unsigned int DuelClient::watching = 0;
unsigned char DuelClient::selftype = 0;
bool DuelClient::is_host = false;
event_base* DuelClient::client_base = nullptr;
bufferevent* DuelClient::client_bev = nullptr;
std::vector<uint8_t> DuelClient::duel_client_read;
std::vector<uint8_t> DuelClient::duel_client_write;
bool DuelClient::is_closing = false;
u64 DuelClient::select_hint = 0;
std::wstring DuelClient::event_string;
std::mt19937 DuelClient::rnd;

ReplayStream DuelClient::replay_stream;
Replay DuelClient::last_replay;
bool DuelClient::old_replay = true;
bool DuelClient::is_swapping = false;

bool DuelClient::is_refreshing = false;
int DuelClient::match_kill = 0;
std::vector<HostPacket> DuelClient::hosts;
std::set<unsigned int> DuelClient::remotes;
event* DuelClient::resp_event = 0;

unsigned int DuelClient::temp_ip = 0;
unsigned short DuelClient::temp_port = 0;
unsigned short DuelClient::temp_ver = 0;
bool DuelClient::try_needed = false;

bool DuelClient::StartClient(unsigned int ip, unsigned short port, bool create_game) {
	if(connect_state)
		return false;
	sockaddr_in sin;
	client_base = event_base_new();
	if(!client_base)
		return false;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(ip);
	sin.sin_port = htons(port);
	client_bev = bufferevent_socket_new(client_base, -1, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(client_bev, ClientRead, NULL, ClientEvent, (void*)create_game);
	temp_ip = ip;
	temp_port = port;
	if (bufferevent_socket_connect(client_bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
		bufferevent_free(client_bev);
		event_base_free(client_base);
		client_bev = 0;
		client_base = 0;
		return false;
	}
	connect_state = 0x1;
	rnd.seed(time(0));
	if(!create_game) {
		timeval timeout = {5, 0};
		event* resp_event = event_new(client_base, 0, EV_TIMEOUT, ConnectTimeout, 0);
		event_add(resp_event, &timeout);
	}
	std::thread(ClientThread).detach();
	return true;
}
void DuelClient::ConnectTimeout(evutil_socket_t fd, short events, void* arg) {
	if(connect_state == 0x7)
		return;
	if(!is_closing) {
		temp_ver = 0;
		mainGame->gMutex.lock();
		mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		if(!mainGame->wLanWindow->isVisible())
			mainGame->ShowElement(mainGame->wLanWindow);
		mainGame->PopupMessage(dataManager.GetSysString(1400));
		mainGame->gMutex.unlock();
	}
	event_base_loopbreak(client_base);
}
void DuelClient::StopClient(bool is_exiting) {
	if(connect_state != 0x7)
		return;
	is_closing = is_exiting;
	event_base_loopbreak(client_base);
	if(!is_closing) {
		
	}
}
void DuelClient::ClientRead(bufferevent* bev, void* ctx) {
	evbuffer* input = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(input);
	unsigned short packet_len = 0;
	while(true) {
		if(len < 2)
			return;
		evbuffer_copyout(input, &packet_len, 2);
		if(len < (size_t)packet_len + 2)
			return;
		duel_client_read.resize(packet_len + 2);
		evbuffer_remove(input, duel_client_read.data(), packet_len + 2);
		if(packet_len)
			HandleSTOCPacketLan((char*)&duel_client_read[2], packet_len);
		len = evbuffer_get_length(input);
	}
}
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ctx) {
	if (events & BEV_EVENT_CONNECTED) {
		bool create_game = (size_t)ctx != 0;
		CTOS_PlayerInfo cspi;
		BufferIO::CopyWStr(mainGame->ebNickName->getText(), cspi.name, 20);
		SendPacketToServer(CTOS_PLAYER_INFO, cspi);
		if(create_game) {
			CTOS_CreateGame cscg;
			BufferIO::CopyWStr(mainGame->ebServerName->getText(), cscg.name, 20);
			BufferIO::CopyWStr(mainGame->ebServerPass->getText(), cscg.pass, 20);
			cscg.info.rule = mainGame->cbRule->getSelected();
			cscg.info.mode = 0;
			cscg.info.start_hand = _wtoi(mainGame->ebStartHand->getText());
			cscg.info.start_lp = _wtoi(mainGame->ebStartLP->getText());
			cscg.info.draw_count = _wtoi(mainGame->ebDrawCount->getText());
			cscg.info.time_limit = _wtoi(mainGame->ebTimeLimit->getText());
			cscg.info.lflist = mainGame->cbLFlist->getItemData(mainGame->cbLFlist->getSelected());
			cscg.info.duel_rule = mainGame->GetMasterRule(mainGame->duel_param, mainGame->forbiddentypes);
			cscg.info.duel_flag = mainGame->duel_param;
			cscg.info.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
			cscg.info.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
			cscg.info.handshake = SERVER_HANDSHAKE;
			cscg.info.team1 = std::stoi(mainGame->ebTeam1->getText());
			cscg.info.team2 = std::stoi(mainGame->ebTeam2->getText());
			try {
				cscg.info.best_of = std::stoi(mainGame->ebBestOf->getText());
			} catch(...) {
				cscg.info.best_of = 1;
			}
			if(mainGame->btnRelayMode->isPressed()) {
				cscg.info.duel_flag |= DUEL_RELAY;
			}
			cscg.info.forbiddentypes = mainGame->forbiddentypes;
			cscg.info.extra_rules = mainGame->extra_rules;
			SendPacketToServer(CTOS_CREATE_GAME, cscg);
		} else {
			CTOS_JoinGame csjg;
			if (temp_ver)
				csjg.version = temp_ver;
			else
				csjg.version = PRO_VERSION;
			csjg.gameid = 0;
			BufferIO::CopyWStr(mainGame->ebJoinPass->getText(), csjg.pass, 20);
			SendPacketToServer(CTOS_JOIN_GAME, csjg);
		}
		bufferevent_enable(bev, EV_READ);
		connect_state |= 0x2;
	} else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_disable(bev, EV_READ);
		if(!is_closing) {
			mainGame->dInfo.isInLobby = false;
			if(connect_state == 0x1) {
				temp_ver = 0;
				mainGame->gMutex.lock();
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				if(!mainGame->wLanWindow->isVisible())
					mainGame->ShowElement(mainGame->wLanWindow);
				mainGame->PopupMessage(dataManager.GetSysString(1400));
				mainGame->gMutex.unlock();
			} else if(connect_state == 0x7) {
				if(!mainGame->dInfo.isInDuel && !mainGame->is_building) {
					mainGame->gMutex.lock();
					mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wHostPrepare);
					mainGame->HideElement(mainGame->wHostPrepare2);
					mainGame->ShowElement(mainGame->wLanWindow);
					mainGame->wChat->setVisible(false);
					if(events & BEV_EVENT_EOF)
						mainGame->PopupMessage(dataManager.GetSysString(1401));
					else mainGame->PopupMessage(dataManager.GetSysString(1402));
					mainGame->gMutex.unlock();
				} else {
					ReplayPrompt(true);
					mainGame->gMutex.lock();
					mainGame->PopupMessage(dataManager.GetSysString(1502));
					mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->stTip->setVisible(false);
					mainGame->gMutex.unlock();
					mainGame->closeDoneSignal.Reset();
					mainGame->closeSignal.lock();
					mainGame->closeDoneSignal.Wait();
					mainGame->closeSignal.unlock();
					mainGame->gMutex.lock();
					mainGame->dInfo.isInDuel = false;
					mainGame->dInfo.isStarted = false;
					mainGame->dField.Clear();
					mainGame->is_building = false;
					mainGame->device->setEventReceiver(&mainGame->menuHandler);
					mainGame->ShowElement(mainGame->wLanWindow);
					mainGame->SetMesageWindow();
					mainGame->gMutex.unlock();
				}
			}
		}
		event_base_loopexit(client_base, 0);
	}
}
int DuelClient::ClientThread() {
	event_base_dispatch(client_base);
	bufferevent_free(client_bev);
	event_base_free(client_base);
	client_bev = 0;
	client_base = 0;
	connect_state = 0;
	return 0;
}
void DuelClient::HandleSTOCPacketLan(char* data, unsigned int len) {
	char* pdata = data;
	unsigned char pktType = BufferIO::Read<uint8_t>(pdata);
	switch(pktType) {
	case STOC_GAME_MSG: {
		if(mainGame->analyzeMutex.try_lock()){
			ClientAnalyze(pdata, len - 1);
			mainGame->analyzeMutex.unlock();
		}
		break;
	}
	case STOC_ERROR_MSG: {
		STOC_ErrorMsg* pkt = (STOC_ErrorMsg*)pdata;
		switch(pkt->msg) {
		case ERRMSG_JOINERROR: {
			temp_ver = 0;
			mainGame->gMutex.lock();
			mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
			mainGame->btnJoinHost->setEnabled(true);
			mainGame->btnJoinCancel->setEnabled(true);
			if(pkt->code == 0)
				mainGame->PopupMessage(dataManager.GetSysString(1403));
			else if(pkt->code == 1)
				mainGame->PopupMessage(dataManager.GetSysString(1404));
			else if(pkt->code == 2)
				mainGame->PopupMessage(dataManager.GetSysString(1405));
			mainGame->gMutex.unlock();
			event_base_loopbreak(client_base);
			break;
		}
		case ERRMSG_DECKERROR: {
			mainGame->gMutex.lock();
			int mainmin = 40, mainmax = 60, extramax = 15, sidemax = 15;
			if (mainGame->cbDeckSelect2->isVisible()) {
				if (mainGame->dInfo.extraval & 0x1) {
					mainmin = 40;
					mainmax = 60;
					extramax = 10;
					sidemax = 0;
				} else {
					mainmin = 100;
					mainmax = 100;
					extramax = 30;
					sidemax = 30;
				}
			} else if (mainGame->dInfo.extraval & 0x1) {
				mainmin = 20;
				mainmax = 30;
				extramax = 10;
				sidemax = 0;
			}
			unsigned int code = pkt->code & 0xFFFFFFF;
			int flag = pkt->code >> 28;
			std::wstring text;
			switch(flag)
			{
			case DECKERROR_LFLIST: {
				text = fmt::sprintf(dataManager.GetSysString(1407).c_str(), dataManager.GetName(code).c_str());
				break;
			}
			case DECKERROR_OCGONLY: {
				text = fmt::sprintf(dataManager.GetSysString(1413).c_str(), dataManager.GetName(code).c_str());
				break;
			}
			case DECKERROR_TCGONLY: {
				text = fmt::sprintf(dataManager.GetSysString(1414).c_str(), dataManager.GetName(code).c_str());
				break;
			}
			case DECKERROR_UNKNOWNCARD: {
				text = fmt::sprintf(dataManager.GetSysString(1415).c_str(), dataManager.GetName(code).c_str(), code);
				break;
			}
			case DECKERROR_CARDCOUNT: {
				text = fmt::sprintf(dataManager.GetSysString(1416).c_str(), dataManager.GetName(code).c_str());
				break;
			}
			case DECKERROR_MAINCOUNT: {
				text = fmt::sprintf(dataManager.GetSysString(1417).c_str(), mainmin, mainmax, code);
				break;
			}
			case DECKERROR_EXTRACOUNT: {
				if(code>0)
					text = fmt::sprintf(dataManager.GetSysString(1418).c_str(), extramax, code);
				else
					text = dataManager.GetSysString(1420);
				break;
			}
			case DECKERROR_SIDECOUNT: {
				text = fmt::sprintf(dataManager.GetSysString(1419).c_str(), sidemax, code);
				break;
			}
			case DECKERROR_FORBTYPE: {
				text = dataManager.GetSysString(1421);
				break;
			}
			default: {
				text = dataManager.GetSysString(1406);
				break;
			}
			}
			mainGame->PopupMessage(text);
			mainGame->cbDeckSelect->setEnabled(true);
			mainGame->cbDeckSelect2->setEnabled(true);
			if(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2)
				mainGame->btnHostPrepDuelist->setEnabled(true);
			mainGame->gMutex.unlock();
			break;
		}
		case ERRMSG_SIDEERROR: {
			mainGame->gMutex.lock();
			mainGame->PopupMessage(dataManager.GetSysString(1408));
			mainGame->gMutex.unlock();
			break;
		}
		case ERRMSG_VERERROR: {
			if (temp_ver) {
				temp_ver = 0;
				mainGame->gMutex.lock();
				mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->PopupMessage(fmt::sprintf(dataManager.GetSysString(1411).c_str(), pkt->code >> 12, (pkt->code >> 4) & 0xff, pkt->code & 0xf));
				mainGame->gMutex.unlock();
				event_base_loopbreak(client_base);
			} else {
				event_base_loopbreak(client_base);
				temp_ver = pkt->code;
				try_needed = true;
			}
			break;
		}
		}
		break;
	}
	case STOC_SELECT_HAND: {
		mainGame->wHand->setVisible(true);
		break;
	}
	case STOC_SELECT_TP: {
		mainGame->gMutex.lock();
		mainGame->PopupElement(mainGame->wFTSelect);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HAND_RESULT: {
		STOC_HandResult* pkt = (STOC_HandResult*)pdata;
		mainGame->stHintMsg->setVisible(false);
		mainGame->showcardcode = (pkt->res1 - 1) + ((pkt->res2 - 1) << 16);
		mainGame->showcarddif = 50;
		mainGame->showcardp = 0;
		mainGame->showcard = 100;
		mainGame->WaitFrameSignal(60);
		break;
	}
	case STOC_TP_RESULT: {
		break;
	}
	case STOC_CHANGE_SIDE: {
		ReplayPrompt(old_replay);
		mainGame->gMutex.lock();
		mainGame->dInfo.isInLobby = false;
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->dField.Clear();
		mainGame->is_building = true;
		mainGame->is_siding = true;
		mainGame->wChat->setVisible(false);
		mainGame->wPhase->setVisible(false);
		mainGame->wDeckEdit->setVisible(false);
		mainGame->wFilter->setVisible(false);
		mainGame->wSort->setVisible(false);
		mainGame->stTip->setVisible(false);
		mainGame->btnSideOK->setVisible(true);
		mainGame->btnSideShuffle->setVisible(true);
		mainGame->btnSideSort->setVisible(true);
		mainGame->btnSideReload->setVisible(true);
		if(mainGame->dInfo.player_type < 7)
			mainGame->btnLeaveGame->setVisible(false);
		mainGame->btnSpectatorSwap->setVisible(false);
		mainGame->btnChainIgnore->setVisible(false);
		mainGame->btnChainAlways->setVisible(false);
		mainGame->btnChainWhenAvail->setVisible(false);
		mainGame->btnCancelOrFinish->setVisible(false);
		mainGame->deckBuilder.result_string = L"0";
		mainGame->deckBuilder.results.clear();
		mainGame->deckBuilder.hovered_code = 0;
		mainGame->deckBuilder.is_draging = false;
		deckManager.pre_deck = deckManager.current_deck;
		mainGame->device->setEventReceiver(&mainGame->deckBuilder);
		mainGame->dInfo.isFirst = mainGame->dInfo.player_type < mainGame->dInfo.team1;
		mainGame->SetMesageWindow();
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_WAITING_SIDE: {
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->stHintMsg->setText(dataManager.GetSysString(1409).c_str());
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_CREATE_GAME: {
		break;
	}
	case STOC_JOIN_GAME: {
		temp_ver = 0;
		STOC_JoinGame* pkt = (STOC_JoinGame*)pdata;
		mainGame->dInfo.isInLobby = true;
		mainGame->dInfo.compat_mode = pkt->info.handshake != SERVER_HANDSHAKE;
		if(mainGame->dInfo.compat_mode) {
			if(pkt->info.mode = MODE_SINGLE) {
				pkt->info.team1 = 1;
				pkt->info.team2 = 1;
				pkt->info.best_of = 0;
			}
			if(pkt->info.mode = MODE_MATCH) {
				pkt->info.team1 = 1;
				pkt->info.team2 = 1;
				pkt->info.best_of = 3;
			}
			if(pkt->info.mode = MODE_TAG) {
				pkt->info.team1 = 2;
				pkt->info.team2 = 2;
				pkt->info.best_of = 0;
			}
		}
		mainGame->dInfo.isRelay = pkt->info.duel_flag & DUEL_RELAY;
		pkt->info.duel_flag &= ~DUEL_RELAY;
		mainGame->dInfo.team1 = pkt->info.team1;
		mainGame->dInfo.team2 = pkt->info.team2;
		mainGame->dInfo.best_of = pkt->info.best_of;
		std::wstring str, str2;
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1226), deckManager.GetLFListName(pkt->info.lflist)));
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1225), dataManager.GetSysString(1240 + pkt->info.rule)));
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1227), dataManager.GetSysString(1244 + pkt->info.mode)));
		if(pkt->info.time_limit) {
			str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1237), pkt->info.time_limit));
		}
		str.append(L"==========\n");
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1231), pkt->info.start_lp));
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1232), pkt->info.start_hand));
		str.append(fmt::format(L"{}{}\n", dataManager.GetSysString(1233), pkt->info.draw_count));
		int rule;
		if (mainGame->dInfo.compat_mode) {
			rule = pkt->info.duel_rule;
			if(rule == 0) {
				mainGame->dInfo.duel_field = 3;
				rule = 3;
			} else
				mainGame->dInfo.duel_field = rule;
		} else {
			mainGame->dInfo.duel_field = mainGame->GetMasterRule(pkt->info.duel_flag, pkt->info.forbiddentypes, &rule);
		}
		if (rule == 5) {
			uint32 filter = 0x100;
			for (int i = 0; i < 6; ++i, filter <<= 1)
				if (pkt->info.duel_flag & filter) {
					str2.append(fmt::format(L"*{}\n", dataManager.GetSysString(1631 + i)));
				}
			str.append(fmt::format(L"*{}\n", dataManager.GetSysString(1630)));
		} else if (rule != DEFAULT_DUEL_RULE) {
			str.append(fmt::format(L"*{}\n", dataManager.GetSysString(1260 + rule - 1)));
		}
		if(!mainGame->dInfo.compat_mode) {
			for(int flag = SEALED_DUEL, i = 0; flag < DECK_MASTER + 1; flag = flag << 1, i++)
				if(pkt->info.extra_rules & flag) {
					str2.append(fmt::format(L"*{}\n", dataManager.GetSysString(1132 + i)));
				}
		}
		if(pkt->info.no_check_deck) {
			str.append(fmt::format(L"*{}\n", dataManager.GetSysString(1229)));
		}
		if(pkt->info.no_shuffle_deck) {
			str.append(fmt::format(L"*{}\n", dataManager.GetSysString(1230)));
		}
		mainGame->gMutex.lock();
		int x = (pkt->info.team1 + pkt->info.team2 >= 5) ? 60 : 0;
		mainGame->btnHostPrepOB->setRelativePosition(mainGame->Scale<s32>(10, 180 + x, 110, 205 + x));
		mainGame->stHostPrepOB->setRelativePosition(mainGame->Scale<s32>(10, 210 + x, 270, 230 + x));
		mainGame->stHostPrepRule->setRelativePosition(mainGame->Scale<s32>(280, 30, 460, 230 + x));
		mainGame->stDeckSelect->setRelativePosition(mainGame->Scale<s32>(10, 235 + x, 110, 255 + x));
		mainGame->cbDeckSelect->setRelativePosition(mainGame->Scale<s32>(120, 230 + x, 270, 255 + x));
		mainGame->cbDeckSelect2->setRelativePosition(mainGame->Scale<s32>(280, 230 + x, 430, 255 + x));
		mainGame->btnHostPrepReady->setRelativePosition(mainGame->Scale<s32>(170, 180 + x, 270, 205 + x));
		mainGame->btnHostPrepNotReady->setRelativePosition(mainGame->Scale<s32>(170, 180 + x, 270, 205 + x));
		mainGame->btnHostPrepStart->setRelativePosition(mainGame->Scale<s32>(230, 280 + x, 340, 305 + x));
		mainGame->btnHostPrepCancel->setRelativePosition(mainGame->Scale<s32>(350, 280 + x, 460, 305 + x));
		mainGame->wHostPrepare->setRelativePosition(mainGame->ResizeWin(270, 120, 750, 440 + x));
		mainGame->wHostPrepare2->setRelativePosition(mainGame->ResizeWin(750, 120, 950, 440 + x));
		for(int i = 0; i < 6; i++) {
			mainGame->chkHostPrepReady[i]->setVisible(false);
			mainGame->chkHostPrepReady[i]->setChecked(false);
			mainGame->btnHostPrepKick[i]->setVisible(false);
			mainGame->stHostPrepDuelist[i]->setVisible(false);
			mainGame->stHostPrepDuelist[i]->setText(L"");
		}
		for(int i = 0; i < pkt->info.team1; i++) {
			mainGame->chkHostPrepReady[i]->setVisible(true);
			mainGame->stHostPrepDuelist[i]->setVisible(true);
			mainGame->stHostPrepDuelist[i]->setRelativePosition(mainGame->Scale<s32>(40, 65 + i * 25, 240, 85 + i * 25));
			mainGame->btnHostPrepKick[i]->setRelativePosition(mainGame->Scale<s32>(10, 65 + i * 25, 30, 85 + i * 25));
			mainGame->chkHostPrepReady[i]->setRelativePosition(mainGame->Scale<s32>(250, 65 + i * 25, 270, 85 + i * 25));
		}
		for(int i = pkt->info.team1; i < pkt->info.team1 + pkt->info.team2; i++) {
			mainGame->chkHostPrepReady[i]->setVisible(true);
			mainGame->stHostPrepDuelist[i]->setVisible(true);
			mainGame->stHostPrepDuelist[i]->setRelativePosition(mainGame->Scale<s32>(40, 75 + i * 25, 240, 95 + i * 25));
			mainGame->btnHostPrepKick[i]->setRelativePosition(mainGame->Scale<s32>(10, 75 + i * 25, 30, 95 + i * 25));
			mainGame->chkHostPrepReady[i]->setRelativePosition(mainGame->Scale<s32>(250, 75 + i * 25, 270, 95 + i * 25));
		}
		mainGame->dInfo.hostname.resize(pkt->info.team1);
		mainGame->dInfo.clientname.resize(pkt->info.team2);
		mainGame->btnHostPrepReady->setVisible(true);
		mainGame->btnHostPrepNotReady->setVisible(false);
		mainGame->dInfo.time_limit = pkt->info.time_limit;
		mainGame->dInfo.time_left[0] = 0;
		mainGame->dInfo.time_left[1] = 0;
		mainGame->deckBuilder.filterList = 0;
		for(auto lit = deckManager._lfList.begin(); lit != deckManager._lfList.end(); ++lit)
			if(lit->hash == pkt->info.lflist)
				mainGame->deckBuilder.filterList = &(*lit);
		if(mainGame->deckBuilder.filterList == 0)
			mainGame->deckBuilder.filterList = &deckManager._lfList[0];
		watching = 0;
		mainGame->stHostPrepOB->setText(fmt::format(L"{} {}", dataManager.GetSysString(1253), watching).c_str());
		mainGame->stHostPrepRule->setText((wchar_t*)str.c_str());
		mainGame->stHostPrepRule2->setText((wchar_t*)str2.c_str());
		mainGame->RefreshDeck(mainGame->cbDeckSelect);
		mainGame->RefreshDeck(mainGame->cbDeckSelect2);
		mainGame->cbDeckSelect->setEnabled(true);
		if (!mainGame->dInfo.compat_mode && pkt->info.extra_rules & DOUBLE_DECK) {
			mainGame->cbDeckSelect2->setVisible(true);
			mainGame->cbDeckSelect2->setEnabled(true);
		} else {
			mainGame->cbDeckSelect2->setVisible(false);
			mainGame->cbDeckSelect2->setEnabled(false);
		}
		if(mainGame->wCreateHost->isVisible())
			mainGame->HideElement(mainGame->wCreateHost);
		else if (mainGame->wLanWindow->isVisible())
			mainGame->HideElement(mainGame->wLanWindow);
		mainGame->ShowElement(mainGame->wHostPrepare);
		if(str2.size())
			mainGame->ShowElement(mainGame->wHostPrepare2);
		mainGame->wChat->setVisible(true);
		mainGame->gMutex.unlock();
		mainGame->dInfo.extraval = (!mainGame->dInfo.compat_mode && pkt->info.extra_rules & SPEED_DUEL) ? 1 : 0;
		mainGame->dInfo.isFirst = mainGame->dInfo.player_type < mainGame->dInfo.team1;
		connect_state |= 0x4;
		break;
	}
	case STOC_TYPE_CHANGE: {
		STOC_TypeChange* pkt = (STOC_TypeChange*)pdata;
		selftype = pkt->type & 0xf;
		is_host = ((pkt->type >> 4) & 0xf) != 0;
		for(int i = 0; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
			mainGame->btnHostPrepKick[i]->setVisible(is_host);
		}
		for(int i = 0; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
			mainGame->chkHostPrepReady[i]->setEnabled(false);
		}
		if(selftype >= mainGame->dInfo.team1 + mainGame->dInfo.team2) {
			mainGame->btnHostPrepDuelist->setEnabled(true);
			mainGame->btnHostPrepOB->setEnabled(false);
			mainGame->btnHostPrepReady->setVisible(false);
			mainGame->btnHostPrepNotReady->setVisible(false);
		} else {
			mainGame->chkHostPrepReady[selftype]->setEnabled(true);
			mainGame->chkHostPrepReady[selftype]->setChecked(false);
			mainGame->btnHostPrepDuelist->setEnabled(mainGame->dInfo.team1 + mainGame->dInfo.team2 > 2);
			mainGame->btnHostPrepOB->setEnabled(true);
			mainGame->btnHostPrepReady->setVisible(true);
			mainGame->btnHostPrepNotReady->setVisible(false);
		}
		mainGame->btnHostPrepStart->setVisible(is_host);
		mainGame->btnHostPrepStart->setEnabled(is_host && CheckReady());
		mainGame->dInfo.player_type = selftype;
		mainGame->dInfo.isFirst = mainGame->dInfo.player_type < mainGame->dInfo.team1;
		break;
	}
	case STOC_DUEL_START: {
		mainGame->HideElement(mainGame->wHostPrepare);
		mainGame->HideElement(mainGame->wHostPrepare2);
		mainGame->WaitFrameSignal(11);
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->dInfo.isInLobby = false;
		mainGame->dInfo.isInDuel = true;
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isCatchingUp = false;
		mainGame->dInfo.lp[0] = 0;
		mainGame->dInfo.lp[1] = 0;
		mainGame->dInfo.turn = 0;
		mainGame->dInfo.time_left[0] = 0;
		mainGame->dInfo.time_left[1] = 0;
		mainGame->dInfo.time_player = 2;
		mainGame->dInfo.current_player[0] = 0;
		mainGame->dInfo.current_player[1] = 0;
		mainGame->dInfo.isReplaySwapped = false;
		mainGame->is_building = false;
		mainGame->mTopMenu->setVisible(false);
		mainGame->wCardImg->setVisible(true);
		mainGame->wInfos->setVisible(true);
		mainGame->wPhase->setVisible(true);
		mainGame->btnSideOK->setVisible(false);
		mainGame->btnDP->setVisible(false);
		mainGame->btnSP->setVisible(false);
		mainGame->btnM1->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		mainGame->btnSideShuffle->setVisible(false);
		mainGame->btnSideSort->setVisible(false);
		mainGame->btnSideReload->setVisible(false);
		mainGame->wChat->setVisible(true);
		mainGame->device->setEventReceiver(&mainGame->dField);
		mainGame->SetPhaseButtons();
		mainGame->SetMesageWindow();
		mainGame->dInfo.hostname.clear();
		mainGame->dInfo.clientname.clear();
		SetPlayersCount();
		int i;
		for(i = 0; i < mainGame->dInfo.team1; i++) {
			mainGame->dInfo.hostname.push_back(mainGame->stHostPrepDuelist[i]->getText());
		}
		for(; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
			mainGame->dInfo.clientname.push_back(mainGame->stHostPrepDuelist[i]->getText());
		}
		if(selftype >= mainGame->dInfo.team1 + mainGame->dInfo.team2) {
			mainGame->dInfo.player_type = 7;
			mainGame->btnLeaveGame->setText(dataManager.GetSysString(1350).c_str());
			mainGame->btnLeaveGame->setVisible(true);
			mainGame->btnSpectatorSwap->setVisible(true);
		}
		if(selftype >= mainGame->dInfo.team1) {
			std::swap(mainGame->dInfo.clientname, mainGame->dInfo.hostname);
		}
		mainGame->dInfo.current_player[0] = 0;
		mainGame->dInfo.current_player[1] = 0;
		mainGame->gMutex.unlock();
		match_kill = 0;
		replay_stream.clear();
		old_replay = true;
		break;
	}
	case STOC_DUEL_END: {
		ReplayPrompt(old_replay);
		mainGame->gMutex.lock();
		if(mainGame->dInfo.player_type < 7)
			mainGame->btnLeaveGame->setVisible(false);
		mainGame->btnSpectatorSwap->setVisible(false);
		mainGame->btnChainIgnore->setVisible(false);
		mainGame->btnChainAlways->setVisible(false);
		mainGame->btnChainWhenAvail->setVisible(false);
		mainGame->stMessage->setText(dataManager.GetSysString(1500).c_str());
		mainGame->btnCancelOrFinish->setVisible(false);
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.unlock();
		mainGame->actionSignal.Reset();
		mainGame->actionSignal.Wait();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.lock();
		mainGame->closeDoneSignal.Wait();
		mainGame->closeSignal.unlock();
		mainGame->gMutex.lock();
		mainGame->dInfo.isInLobby = false;
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->dField.Clear();
		mainGame->is_building = false;
		mainGame->wDeckEdit->setVisible(false);
		mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		mainGame->stTip->setVisible(false);
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->ShowElement(mainGame->wLanWindow);
		mainGame->SetMesageWindow();
		mainGame->gMutex.unlock();
		event_base_loopbreak(client_base);
		if(exit_on_return)
			mainGame->device->closeDevice();
		break;
	}
	case STOC_REPLAY: {
		if(!old_replay) break;
		char* prep = pdata;
		replay_stream.push_back(ReplayPacket(OLD_REPLAY_MODE, prep, len - 1));
		break;
	}
	case STOC_TIME_LIMIT: {
		STOC_TimeLimit* pkt = (STOC_TimeLimit*)pdata;
		int lplayer = mainGame->LocalPlayer(pkt->player);
		if(lplayer == 0)
			DuelClient::SendPacketToServer(CTOS_TIME_CONFIRM);
		mainGame->dInfo.time_player = lplayer;
		mainGame->dInfo.time_left[lplayer] = pkt->left_time;
		break;
	}
	case STOC_CHAT: {
		STOC_Chat* pkt = (STOC_Chat*)pdata;
		int player = pkt->player;
		int type = -1;
		if(player < mainGame->dInfo.team1 + mainGame->dInfo.team2) {
			if(mainGame->chkIgnore1->isChecked()) {
				if(player >= mainGame->dInfo.team1 && mainGame->dInfo.isFirst)
					break;
				if(player < mainGame->dInfo.team1 && !mainGame->dInfo.isFirst)
					break;
			}
			if(player >= mainGame->dInfo.team1) {
				player -= mainGame->dInfo.team1;
				type = !mainGame->dInfo.isInDuel ? 1 : mainGame->LocalPlayer(1);
			} else {
				type = !mainGame->dInfo.isInDuel ? 0 : mainGame->LocalPlayer(0);
			}
		} else {
			type = 2;
			if(player == 8) { //system custom message.
				if(mainGame->chkIgnore1->isChecked())
					break;
			} else if(player < 11 || player > 19) {
				if(mainGame->chkIgnore2->isChecked())
					break;
				player = 10;
			}
		}
		wchar_t msg[256];
		BufferIO::CopyWStr(pkt->msg, msg, 256);
		mainGame->gMutex.lock();
		mainGame->AddChatMsg(msg, player, type);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HS_PLAYER_ENTER: {
		mainGame->soundManager->PlaySoundEffect(SoundManager::SFX::PLAYER_ENTER);
		STOC_HS_PlayerEnter* pkt = (STOC_HS_PlayerEnter*)pdata;
		if(pkt->pos > 5)
			break;
		wchar_t name[20];
		BufferIO::CopyWStr(pkt->name, name, 20);
		if(pkt->pos < mainGame->dInfo.team1)
			mainGame->dInfo.hostname[pkt->pos] = name;
		else
			mainGame->dInfo.clientname[pkt->pos - mainGame->dInfo.team1] = name;
		mainGame->gMutex.lock();
		mainGame->stHostPrepDuelist[pkt->pos]->setText(name);
		mainGame->btnHostPrepStart->setVisible(is_host);
		mainGame->btnHostPrepStart->setEnabled(is_host && CheckReady());
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HS_PLAYER_CHANGE: {
		STOC_HS_PlayerChange* pkt = (STOC_HS_PlayerChange*)pdata;
		unsigned char pos = (pkt->status >> 4) & 0xf;
		unsigned char state = pkt->status & 0xf;
		if(pos > 5)
			break;
		mainGame->gMutex.lock();
		if(state < 8) {
			mainGame->soundManager->PlaySoundEffect(SoundManager::SFX::PLAYER_ENTER);
			std::wstring prename = mainGame->stHostPrepDuelist[pos]->getText();
			mainGame->stHostPrepDuelist[state]->setText(prename.c_str());
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			if(pos < mainGame->dInfo.team1)
				mainGame->dInfo.hostname[pos] = L"";
			else
				mainGame->dInfo.clientname[pos - mainGame->dInfo.team1] = L"";
			if(state < mainGame->dInfo.team1)
				mainGame->dInfo.hostname[state] = prename;
			else
				mainGame->dInfo.clientname[state - mainGame->dInfo.team1] = prename;
		} else if(state == PLAYERCHANGE_READY) {
			mainGame->chkHostPrepReady[pos]->setChecked(true);
			if(pos == selftype) {
				mainGame->btnHostPrepReady->setVisible(false);
				mainGame->btnHostPrepNotReady->setVisible(true);
			}
		} else if(state == PLAYERCHANGE_NOTREADY) {
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			if(pos == selftype) {
				mainGame->btnHostPrepReady->setVisible(true);
				mainGame->btnHostPrepNotReady->setVisible(false);
			}
		} else if(state == PLAYERCHANGE_LEAVE) {
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_OBSERVE) {
			watching++;
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			mainGame->stHostPrepOB->setText(fmt::format(L"{} {}", dataManager.GetSysString(1253), watching).c_str());
		}
		mainGame->btnHostPrepStart->setVisible(is_host);
		mainGame->btnHostPrepStart->setEnabled(is_host && CheckReady());
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HS_WATCH_CHANGE: {
		STOC_HS_WatchChange* pkt = (STOC_HS_WatchChange*)pdata;
		watching = pkt->watch_count;
		mainGame->gMutex.lock();
		mainGame->stHostPrepOB->setText(fmt::format(L"{} {}", dataManager.GetSysString(1253), watching).c_str());
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_NEW_REPLAY: {
		old_replay = false;
		char* prep = pdata;
		last_replay;
		memcpy(&last_replay.pheader, prep, sizeof(ReplayHeader));
		prep += sizeof(ReplayHeader);
		last_replay.comp_data.resize(len - sizeof(ReplayHeader) - 1);
		memcpy(last_replay.comp_data.data(), prep, len - sizeof(ReplayHeader) - 1);
		last_replay.comp_size = len - sizeof(ReplayHeader) - 1;
		break;
	}
	case STOC_CATCHUP: {
		mainGame->dInfo.isCatchingUp = !!BufferIO::Read<uint8_t>(pdata);
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->gMutex.lock();
			mainGame->dField.RefreshAllCards();
			mainGame->gMutex.unlock();
		}
		break;
	}
	}
}
bool DuelClient::CheckReady() {
	bool ready1 = false, ready2 = false;
	for(int i = 0; i < mainGame->dInfo.team1; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			ready1 = mainGame->chkHostPrepReady[i]->isChecked();
		} else if(!mainGame->dInfo.isRelay) {
			return false;
		}
	}
	for(int i = mainGame->dInfo.team1; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			ready2 = mainGame->chkHostPrepReady[i]->isChecked();
		} else if(!mainGame->dInfo.isRelay) {
			return false;
		}
	}
	return ready1 && ready2;
}
void DuelClient::SetPlayersCount() {
	int count1 = 0, count2 = 0;
	for(int i = 0; i < mainGame->dInfo.team1; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			count1++;
		}
	}
	for(int i = mainGame->dInfo.team1; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			count2++;
		}
	}
	mainGame->dInfo.team1 = count1;
	mainGame->dInfo.team2 = count2;
}
std::pair<int, int> DuelClient::GetPlayersCount() {
	int count1 = 0, count2 = 0;
	for(int i = 0; i < mainGame->dInfo.team1; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			count1++;
		}
	}
	for(int i = mainGame->dInfo.team1; i < mainGame->dInfo.team1 + mainGame->dInfo.team2; i++) {
		if(std::wstring(mainGame->stHostPrepDuelist[i]->getText()).size()) {
			count2++;
		}
	}
	return { count1, count2 };
}
#define COMPAT_READ(val1,val2,buf) (mainGame->dInfo.compat_mode) ? BufferIO::Read<val1>(buf) : BufferIO::Read<val2>(buf)
#define PLAY_SOUND(sound) if (!mainGame->dInfo.isCatchingUp) mainGame->soundManager->PlaySoundEffect(sound)
int DuelClient::ClientAnalyze(char * msg, unsigned int len) {
	char* pbuf = msg;
	if(!mainGame->dInfo.isReplay) {
		mainGame->dInfo.curMsg = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.curMsg != MSG_WAITING && !mainGame->dInfo.isSingleMode) {
			ReplayPacket p(mainGame->dInfo.curMsg, pbuf, len - 1);
			replay_stream.push_back(p);
		}
	}
	mainGame->wCmdMenu->setVisible(false);
	if(!mainGame->dInfo.isReplay && mainGame->dInfo.curMsg != MSG_WAITING && mainGame->dInfo.curMsg != MSG_CARD_SELECTED) {
		mainGame->waitFrame = -1;
		mainGame->stHintMsg->setVisible(false);
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->gMutex.lock();
			mainGame->HideElement(mainGame->wCardSelect);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
		if(mainGame->wOptions->isVisible()) {
			mainGame->gMutex.lock();
			mainGame->HideElement(mainGame->wOptions);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
	}
	if(mainGame->dInfo.time_player == 1)
		mainGame->dInfo.time_player = 2;
	if(is_swapping) {
		mainGame->gMutex.lock();
		mainGame->dField.ReplaySwap();
		mainGame->gMutex.unlock();
		is_swapping = false;
	}
	switch(mainGame->dInfo.curMsg) {
	case MSG_RETRY: {
		mainGame->gMutex.lock();
		mainGame->stMessage->setText(L"Error occurs.");
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.unlock();
		mainGame->actionSignal.Reset();
		mainGame->actionSignal.Wait();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.lock();
		mainGame->closeDoneSignal.Wait();
		mainGame->closeSignal.unlock();
		ReplayPrompt(true);
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->dInfo.isInLobby = false;
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->btnCreateHost->setEnabled(mainGame->coreloaded);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		mainGame->stTip->setVisible(false);
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->ShowElement(mainGame->wLanWindow);
		mainGame->SetMesageWindow();
		mainGame->gMutex.unlock();
		event_base_loopbreak(client_base);
		if(exit_on_return)
			mainGame->device->closeDevice();
		return false;
	}
	case MSG_HINT: {
		int type = BufferIO::Read<uint8_t>(pbuf);
		/*int player = */BufferIO::Read<uint8_t>(pbuf);
		u64 data = COMPAT_READ(int32_t, int64_t, pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		switch (type) {
		case HINT_EVENT: {
			event_string = dataManager.GetDesc(data);
			break;
		}
		case HINT_MESSAGE: {
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(dataManager.GetDesc(data).c_str());
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			break;
		}
		case HINT_SELECTMSG: {
			select_hint = data;
			break;
		}
		case HINT_OPSELECTED: {
			std::wstring text(fmt::sprintf(dataManager.GetSysString(1510), dataManager.GetDesc(data)));
			mainGame->AddLog(text);
			mainGame->gMutex.lock();
			mainGame->stACMessage->setText(text.c_str());
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_EFFECT: {
			mainGame->showcardcode = data;
			mainGame->showcarddif = 0;
			mainGame->showcard = 1;
			mainGame->WaitFrameSignal(30);
			break;
		}
		case HINT_RACE: {
			std::wstring text(fmt::sprintf(dataManager.GetSysString(1511), dataManager.FormatRace(data)));
			mainGame->AddLog(text);
			mainGame->gMutex.lock();
			mainGame->stACMessage->setText(text.c_str());
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_ATTRIB: {
			std::wstring text(fmt::sprintf(dataManager.GetSysString(1511), dataManager.FormatAttribute(data)));
			mainGame->AddLog(text);
			mainGame->gMutex.lock();
			mainGame->stACMessage->setText(text.c_str());
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_CODE: {
			std::wstring text(fmt::sprintf(dataManager.GetSysString(1511), dataManager.GetName(data)));
			mainGame->AddLog(text);
			mainGame->gMutex.lock();
			mainGame->stACMessage->setText(text.c_str());
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_NUMBER: {
			std::wstring text(fmt::sprintf(dataManager.GetSysString(1512), data));
			mainGame->AddLog(text);
			mainGame->gMutex.lock();
			mainGame->stACMessage->setText(text.c_str());
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_CARD: {
			mainGame->showcardcode = data;
			mainGame->showcarddif = 0;
			mainGame->showcard = 1;
			mainGame->WaitFrameSignal(30);
			break;
		}
		}
		break;
	}
	case MSG_WIN: {
		int player = BufferIO::Read<uint8_t>(pbuf);
		int type = BufferIO::Read<uint8_t>(pbuf);
		mainGame->showcarddif = 110;
		mainGame->showcardp = 0;
		mainGame->dInfo.vic_string = L"";
		mainGame->showcardcode = 3;
		std::wstring formatted_string = L"";
		if(player < 2) {
			mainGame->showcardcode = mainGame->LocalPlayer(player) + 1;
			if(match_kill)
				mainGame->dInfo.vic_string = fmt::sprintf(dataManager.GetVictoryString(0x20), dataManager.GetName(match_kill));
			else if(type < 0x10) {
				auto curplayer = mainGame->dInfo.current_player[mainGame->LocalPlayer(player)];
				mainGame->dInfo.vic_string = fmt::format(L"[{}] {}", (mainGame->LocalPlayer(player) == 0) ? mainGame->dInfo.clientname[curplayer] : mainGame->dInfo.hostname[curplayer],
					dataManager.GetVictoryString(type));
			} else
				mainGame->dInfo.vic_string = dataManager.GetVictoryString(type);
		}
		mainGame->showcard = 101;
		mainGame->WaitFrameSignal(120);
		mainGame->dInfo.vic_string = L"";
		mainGame->showcard = 0;
		break;
	}
	case MSG_WAITING: {
		mainGame->waitFrame = 0;
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(dataManager.GetSysString(1390).c_str());
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_START: {
		int playertype = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.compat_mode)
			/*duel_rule = */BufferIO::Read<uint8_t>(pbuf);
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->showcardcode = 11;
			mainGame->showcarddif = 30;
			mainGame->showcardp = 0;
			mainGame->showcard = 101;
			mainGame->WaitFrameSignal(40);
			mainGame->showcard = 0;
			mainGame->gMutex.lock();
		}
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isFirst = (playertype & 0xf) ? false : true;
		if(playertype & 0xf0)
			mainGame->dInfo.player_type = 7;
		if(!mainGame->dInfo.isRelay) {
			if(mainGame->dInfo.isFirst)
				mainGame->dInfo.current_player[mainGame->LocalPlayer(1)] = mainGame->dInfo.team2 - 1;
			else
				mainGame->dInfo.current_player[mainGame->LocalPlayer(0)] = mainGame->dInfo.team1 - 1;
		}
		mainGame->dInfo.lp[mainGame->LocalPlayer(0)] = BufferIO::Read<int32_t>(pbuf);
		mainGame->dInfo.lp[mainGame->LocalPlayer(1)] = BufferIO::Read<int32_t>(pbuf);
		if(mainGame->dInfo.lp[mainGame->LocalPlayer(0)] > 0)
			mainGame->dInfo.startlp = mainGame->dInfo.lp[mainGame->LocalPlayer(0)];
		else
			mainGame->dInfo.startlp = 8000;
		mainGame->dInfo.strLP[0] = fmt::to_wstring(mainGame->dInfo.lp[0]);
		mainGame->dInfo.strLP[1] = fmt::to_wstring(mainGame->dInfo.lp[1]);
		int deckc = BufferIO::Read<uint16_t>(pbuf);
		int extrac = BufferIO::Read<uint16_t>(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(0), deckc, extrac);
		deckc = BufferIO::Read<uint16_t>(pbuf);
		extrac = BufferIO::Read<uint16_t>(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(1), deckc, extrac);
		mainGame->dInfo.turn = 0;
		mainGame->dInfo.is_shuffling = false;
		if(mainGame->dInfo.isReplaySwapped) {
			std::swap(mainGame->dInfo.hostname, mainGame->dInfo.clientname);
			mainGame->dInfo.isReplaySwapped = false;
			mainGame->dField.ReplaySwap();
		}
		if (!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UPDATE_DATA: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int location = BufferIO::Read<uint8_t>(pbuf);
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		mainGame->dField.UpdateFieldCard(player, location, pbuf, len - 2);
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UPDATE_CARD: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int loc = BufferIO::Read<uint8_t>(pbuf);
		int seq = BufferIO::Read<uint8_t>(pbuf);
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		mainGame->dField.UpdateCard(player, loc, seq, pbuf, len - 3);
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.unlock();
		break;
	}
	case MSG_SELECT_BATTLECMD: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		int code, count, con, loc, seq/*, diratt*/;
		u64 desc;
		ClientCard* pcard;
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		//cards with effects that can be activated, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc =  BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			desc = COMPAT_READ(int32_t, int64_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			int flag = 0;
			if(code & 0x80000000) {
				flag = EDESC_OPERATION;
				code &= 0x7fffffff;
			}
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
			} else {
				pcard->cmdFlag |= COMMAND_ACTIVATE;
				if (pcard->location == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if (pcard->location == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
			}
		}
		mainGame->dField.attackable_cards.clear();
		//cards that can attack, will remain under 255
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			/*code = */BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = BufferIO::Read<uint8_t>(pbuf);
			/*diratt = */BufferIO::Read<uint8_t>(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.attackable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_ATTACK;
		}
		mainGame->gMutex.lock();
		if(BufferIO::Read<uint8_t>(pbuf)) {
			mainGame->btnM2->setVisible(true);
			mainGame->btnM2->setEnabled(true);
			mainGame->btnM2->setPressed(false);
		}
		if(BufferIO::Read<uint8_t>(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_IDLECMD: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		int code, count, con, loc, seq;
		u64 desc;
		ClientCard* pcard;
		mainGame->dField.summonable_cards.clear();
		//cards that can be normal summoned, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.summonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SUMMON;
		}
		mainGame->dField.spsummonable_cards.clear();
		//cards that can be special summoned, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.spsummonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SPSUMMON;
			if (pcard->location == LOCATION_DECK) {
				pcard->SetCode(code);
				mainGame->dField.deck_act = true;
			} else if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			else if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
			else if (pcard->location == LOCATION_EXTRA)
				mainGame->dField.extra_act = true;
			else {
				int seq = mainGame->dInfo.duel_field == 4 ? (mainGame->dInfo.extraval & 0x1) ? 1 : 0 : 6;
				if (pcard->location == LOCATION_SZONE && pcard->sequence == seq && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget)
					mainGame->dField.pzone_act[pcard->controler] = true;
			}
		}
		mainGame->dField.reposable_cards.clear();
		//cards whose position can be changed, will remain under 255
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = BufferIO::Read<uint8_t>(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.reposable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_REPOS;
		}
		mainGame->dField.msetable_cards.clear();
		//cards that can be set in the mzone, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.msetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_MSET;
		}
		mainGame->dField.ssetable_cards.clear();
		//cards that can be set in the szone, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.ssetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SSET;
		}
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		//cards with effects that can be activated, can be an arbitrary size
		count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			con = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			loc = BufferIO::Read<uint8_t>(pbuf);
			seq = COMPAT_READ(int8_t, int32_t, pbuf);
			desc = COMPAT_READ(int32_t, int64_t, pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			int flag = 0;
			if(code & 0x80000000) {
				flag = EDESC_OPERATION;
				code &= 0x7fffffff;
			}
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
			} else {
				pcard->cmdFlag |= COMMAND_ACTIVATE;
				if (pcard->location == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if (pcard->location == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
			}
		}
		if(BufferIO::Read<uint8_t>(pbuf)) {
			mainGame->btnBP->setVisible(true);
			mainGame->btnBP->setEnabled(true);
			mainGame->btnBP->setPressed(false);
		}
		if(BufferIO::Read<uint8_t>(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		if (BufferIO::Read<uint8_t>(pbuf)) {
			mainGame->btnShuffle->setVisible(true);
		} else {
			mainGame->btnShuffle->setVisible(false);
		}
		return false;
	}
	case MSG_SELECT_EFFECTYN: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
		info.controler = mainGame->LocalPlayer(info.controler);
		ClientCard* pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
		if (pcard->code != code)
			pcard->SetCode(code);
		if(info.location != LOCATION_DECK) {
			pcard->is_highlighting = true;
			mainGame->dField.highlighting_card = pcard;
		}
		u64 desc = COMPAT_READ(int32_t, int64_t, pbuf);
		std::wstring text;
		
		if(desc == 0) {
			text = fmt::sprintf(L"%ls\n%ls", event_string,
				fmt::sprintf(dataManager.GetSysString(200), dataManager.FormatLocation(info.location, info.sequence), dataManager.GetName(code)));
		} else if(desc == 221) {
			text = fmt::format(L"{}\n{}\n{}", event_string,
				fmt::sprintf(dataManager.GetSysString(221), dataManager.FormatLocation(info.location, info.sequence), dataManager.GetName(code)),
				dataManager.GetSysString(223));
		} else if(desc < 2048) {
			text = fmt::sprintf(dataManager.GetSysString(desc), dataManager.GetName(code));
		} else {
			text = fmt::sprintf(dataManager.GetDesc(desc), dataManager.GetName(code));
		}
		mainGame->gMutex.lock();
		mainGame->stQMessage->setText(text.c_str());
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_YESNO: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		u64 desc = COMPAT_READ(int32_t, int64_t, pbuf);
		mainGame->dField.highlighting_card = 0;
		mainGame->gMutex.lock();
		mainGame->stQMessage->setText((wchar_t*)dataManager.GetDesc(desc).c_str());
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_OPTION: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		int count = BufferIO::Read<uint8_t>(pbuf);
		mainGame->dField.select_options.clear();
		for(int i = 0; i < count; ++i)
			mainGame->dField.select_options.push_back(COMPAT_READ(int32_t, int64_t, pbuf));
		mainGame->dField.ShowSelectOption(select_hint);
		select_hint = 0;
		return false;
	}
	case MSG_SELECT_CARD: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		mainGame->dField.select_cancelable = BufferIO::Read<uint8_t>(pbuf) != 0;
		mainGame->dField.select_min = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.select_max = COMPAT_READ(int8_t, int32_t, pbuf);
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		unsigned int code;
		bool panelmode = false;
		bool select_ready = mainGame->dField.select_min == 0;
		mainGame->dField.select_ready = select_ready;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			info.controler = mainGame->LocalPlayer(info.controler);
			if ((info.location & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(info.controler, info.location & (~LOCATION_OVERLAY) & 0xff, info.sequence)->overlayed[info.position];
			else if (info.location == 0) {
				pcard = new ClientCard();
				mainGame->dField.limbo_temp.push_back(pcard);
				panelmode = true;
			} else
				pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = false;
			if (info.location & 0xf1)
				panelmode = true;
		}
		std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
		std::wstring text = fmt::sprintf(L"%ls(%d-%d)", dataManager.GetDesc(select_hint ? select_hint : 560).c_str(),
			mainGame->dField.select_min, mainGame->dField.select_max);
		select_hint = 0;
		if (panelmode) {
			mainGame->gMutex.lock();
			mainGame->wCardSelect->setText(text.c_str());
			mainGame->dField.ShowSelectCard(select_ready);
			mainGame->gMutex.unlock();
		} else {
			mainGame->stHintMsg->setText(text.c_str());
			mainGame->stHintMsg->setVisible(true);
		}
		if (mainGame->dField.select_cancelable) {
			mainGame->dField.ShowCancelOrFinishButton(1);
		} else if (select_ready) {
			mainGame->dField.ShowCancelOrFinishButton(2);
		} else {
			mainGame->dField.ShowCancelOrFinishButton(0);
		}
		return false;
	}
	case MSG_SELECT_UNSELECT_CARD: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		bool finishable = BufferIO::Read<uint8_t>(pbuf) != 0;;
		bool cancelable = BufferIO::Read<uint8_t>(pbuf) != 0;
		mainGame->dField.select_cancelable = finishable || cancelable;
		mainGame->dField.select_min = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.select_max = COMPAT_READ(int8_t, int32_t, pbuf);
		int count1 = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		unsigned int code;
		bool panelmode = false;
		mainGame->dField.select_ready = false;
		ClientCard* pcard;
		for (int i = 0; i < count1; ++i) {
			code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			info.controler = mainGame->LocalPlayer(info.controler);
			if ((info.location & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(info.controler, info.location & (~LOCATION_OVERLAY) & 0xff, info.sequence)->overlayed[info.position];
			else if (info.location == 0) {
				pcard = new ClientCard();
				mainGame->dField.limbo_temp.push_back(pcard);
				panelmode = true;
			} else
				pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = false;
			if (info.location & 0xf1)
				panelmode = true;
		}
		int count2 = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = count1; i < count1 + count2; ++i) {
			code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			info.controler = mainGame->LocalPlayer(info.controler);
			if ((info.location & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(info.controler, info.location & (~LOCATION_OVERLAY) & 0xff, info.sequence)->overlayed[info.position];
			else if (info.location == 0) {
				pcard = new ClientCard();
				mainGame->dField.limbo_temp.push_back(pcard);
				panelmode = true;
			} else
				pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = true;
			if (info.location & 0xf1)
				panelmode = true;
		}
		std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
		std::wstring text = fmt::sprintf(L"%ls(%d-%d)", dataManager.GetDesc(select_hint ? select_hint : 560).c_str(),
			mainGame->dField.select_min, mainGame->dField.select_max);
		select_hint = 0;
		if (panelmode) {
			mainGame->gMutex.lock();
			mainGame->wCardSelect->setText(text.c_str());
			mainGame->dField.ShowSelectCard(mainGame->dField.select_cancelable);
			mainGame->gMutex.unlock();
		} else {
			mainGame->stHintMsg->setText(text.c_str());
			mainGame->stHintMsg->setVisible(true);
		}
		if (mainGame->dField.select_cancelable) {
			if (count2 == 0)
				mainGame->dField.ShowCancelOrFinishButton(1);
			else
				mainGame->dField.ShowCancelOrFinishButton(2);
		}
		else
			mainGame->dField.ShowCancelOrFinishButton(0);
		return false;
	}
	case MSG_SELECT_CHAIN: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		int count = COMPAT_READ(int8_t,int32_t,pbuf);
		int specount = BufferIO::Read<uint8_t>(pbuf);
		int forced = BufferIO::Read<uint8_t>(pbuf);
		/*int hint0 = */BufferIO::Read<int32_t>(pbuf);
		/*int hint1 = */BufferIO::Read<int32_t>(pbuf);
		int code;
		u64 desc;
		ClientCard* pcard;
		bool panelmode = false;
		bool conti_exist = false;
		bool select_trigger = (specount == 0x7f);
		mainGame->dField.chain_forced = (forced != 0);
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		for (int i = 0; i < count; ++i) {
			int flag = BufferIO::Read<uint8_t>(pbuf);
			code = BufferIO::Read<int32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			info.controler = mainGame->LocalPlayer(info.controler);
			desc = COMPAT_READ(int32_t, int64_t, pbuf);
			pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence, info.position);
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			pcard->is_selected = false;
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
				conti_exist = true;
			} else {
				pcard->is_selectable = true;
				if(flag == EDESC_RESET)
					pcard->cmdFlag |= COMMAND_RESET;
				else
					pcard->cmdFlag |= COMMAND_ACTIVATE;
				if(pcard->location == LOCATION_DECK) {
					pcard->SetCode(code);
					mainGame->dField.deck_act = true;
				} else if(info.location == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if(info.location == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
				else if(info.location == LOCATION_EXTRA)
					mainGame->dField.extra_act = true;
				else if(info.location == LOCATION_OVERLAY)
					panelmode = true;
			}
		}
		if(!select_trigger && !forced && (mainGame->ignore_chain || ((count == 0 || specount == 0) && !mainGame->always_chain)) && (count == 0 || !mainGame->chain_when_avail)) {
			SetResponseI(-1);
			mainGame->dField.ClearChainSelect();
			if(mainGame->chkWaitChain->isChecked() && !mainGame->ignore_chain) {
				mainGame->WaitFrameSignal(rnd() * 20 + 20);
			}
			DuelClient::SendResponse();
			return true;
		}
		if(mainGame->chkAutoChain->isChecked() && forced && !(mainGame->always_chain || mainGame->chain_when_avail)) {
			SetResponseI(0);
			mainGame->dField.ClearChainSelect();
			DuelClient::SendResponse();
			return true;
		}
		mainGame->gMutex.lock();
		if(!conti_exist)
			mainGame->stHintMsg->setText(dataManager.GetSysString(550).c_str());
		else
			mainGame->stHintMsg->setText(dataManager.GetSysString(556).c_str());
		mainGame->stHintMsg->setVisible(true);
		if(panelmode) {
			mainGame->dField.list_command = COMMAND_ACTIVATE;
			mainGame->dField.selectable_cards = mainGame->dField.activatable_cards;
			std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
			auto eit = std::unique(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
			mainGame->dField.selectable_cards.erase(eit, mainGame->dField.selectable_cards.end());
			mainGame->dField.ShowChainCard();
		} else {
			if(!forced) {
				if(count == 0)
					mainGame->stQMessage->setText(fmt::sprintf(L"%ls\n%ls", dataManager.GetSysString(201), dataManager.GetSysString(202)).c_str());
				else if(select_trigger)
					mainGame->stQMessage->setText(fmt::sprintf(L"%ls\n%ls\n%ls", event_string, dataManager.GetSysString(222), dataManager.GetSysString(223)).c_str());
				else
					mainGame->stQMessage->setText(fmt::sprintf(L"%ls\n%ls", event_string, dataManager.GetSysString(203)).c_str());				
				mainGame->PopupElement(mainGame->wQuery);
			}
		}
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_PLACE:
	case MSG_SELECT_DISFIELD: {
		int selecting_player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		mainGame->dField.select_min = BufferIO::Read<uint8_t>(pbuf);
		unsigned int flag = BufferIO::Read<int32_t>(pbuf);
		if(selecting_player == 1) {
			flag = flag << 16 | flag >> 16;
		}
		mainGame->dField.selectable_field = ~flag;
		mainGame->dField.selected_field = 0;
		unsigned char respbuf[64];
		int pzone = 0;
		std::wstring text;
		if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE) {
			if (select_hint) {
				text = fmt::sprintf(dataManager.GetSysString(569), dataManager.GetName(select_hint));
			} else
				text = dataManager.GetDesc(560);
		} else
			text = dataManager.GetDesc(select_hint ? select_hint : 570);
		select_hint = 0;
		mainGame->stHintMsg->setText(text.c_str());
		mainGame->stHintMsg->setVisible(true);
		if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE && (
			(mainGame->chkMAutoPos->isChecked() && mainGame->dField.selectable_field & 0x7f007f) ||
			(mainGame->chkSTAutoPos->isChecked() && !(mainGame->dField.selectable_field & 0x7f007f)))) {
			if(mainGame->chkRandomPos->isChecked()) {
				std::vector<char> positions;
				for(char i = 0; i < 32; i++) {
					if(mainGame->dField.selectable_field & (1 << i))
						positions.push_back(i);
				}
				char res = positions[(std::uniform_int_distribution<>(0, positions.size() - 1))(rnd)];
				respbuf[0] = mainGame->LocalPlayer((res < 16) ? 0 : 1);
				respbuf[1] = ((1 << res) & 0x7f007f) ? LOCATION_MZONE : LOCATION_SZONE;
				respbuf[2] = (res%16) - (2 * (respbuf[1] - LOCATION_MZONE));
			} else {
				unsigned int filter;
				if(mainGame->dField.selectable_field & 0x7f) {
					respbuf[0] = mainGame->LocalPlayer(0);
					respbuf[1] = LOCATION_MZONE;
					filter = mainGame->dField.selectable_field & 0x7f;
				} else if(mainGame->dField.selectable_field & 0x1f00) {
					respbuf[0] = mainGame->LocalPlayer(0);
					respbuf[1] = LOCATION_SZONE;
					filter = (mainGame->dField.selectable_field >> 8) & 0x1f;
				} else if(mainGame->dField.selectable_field & 0xc000) {
					respbuf[0] = mainGame->LocalPlayer(0);
					respbuf[1] = LOCATION_SZONE;
					filter = (mainGame->dField.selectable_field >> 14) & 0x3;
					pzone = 1;
				} else if(mainGame->dField.selectable_field & 0x7f0000) {
					respbuf[0] = mainGame->LocalPlayer(1);
					respbuf[1] = LOCATION_MZONE;
					filter = (mainGame->dField.selectable_field >> 16) & 0x7f;
				} else if(mainGame->dField.selectable_field & 0x1f000000) {
					respbuf[0] = mainGame->LocalPlayer(1);
					respbuf[1] = LOCATION_SZONE;
					filter = (mainGame->dField.selectable_field >> 24) & 0x1f;
				} else {
					respbuf[0] = mainGame->LocalPlayer(1);
					respbuf[1] = LOCATION_SZONE;
					filter = (mainGame->dField.selectable_field >> 30) & 0x3;
					pzone = 1;
				}
				if(!pzone) {
					if(filter & 0x40) respbuf[2] = 6;
					else if(filter & 0x20) respbuf[2] = 5;
					else if(filter & 0x4) respbuf[2] = 2;
					else if(filter & 0x2) respbuf[2] = 1;
					else if(filter & 0x8) respbuf[2] = 3;
					else if(filter & 0x1) respbuf[2] = 0;
					else if(filter & 0x10) respbuf[2] = 4;
				} else {
					if(filter & 0x1) respbuf[2] = 6;
					else if(filter & 0x2) respbuf[2] = 7;
				}
			}
			mainGame->dField.selectable_field = 0;
			SetResponseB(respbuf, 3);
			DuelClient::SendResponse();
			return true;
		}
		return false;
	}
	case MSG_SELECT_POSITION: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		int positions = BufferIO::Read<uint8_t>(pbuf);
		if (positions == POS_FACEUP_ATTACK || positions == POS_FACEDOWN_ATTACK || positions == POS_FACEUP_DEFENSE || positions == POS_FACEDOWN_DEFENSE) {
			SetResponseI(positions);
			return true;
		}
		int count = 0, filter = 0x1, startpos;
		while(filter != 0x10) {
			if(positions & filter) count++;
			filter <<= 1;
		}
		if(count == 4) startpos = 10;
		else if(count == 3) startpos = 82;
		else startpos = 155;
		if(positions & POS_FACEUP_ATTACK) {
			mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSAU, code));
			mainGame->btnPSAU->setRelativePosition(mainGame->Scale<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSAU->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSAU->setVisible(false);
		if(positions & POS_FACEDOWN_ATTACK) {
			mainGame->btnPSAD->setRelativePosition(mainGame->Scale<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSAD->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSAD->setVisible(false);
		if(positions & POS_FACEUP_DEFENSE) {
			mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSDU, code));
			mainGame->btnPSDU->setRelativePosition(mainGame->Scale<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSDU->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSDU->setVisible(false);
		if(positions & POS_FACEDOWN_DEFENSE) {
			mainGame->btnPSDD->setRelativePosition(mainGame->Scale<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSDD->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSDD->setVisible(false);
		mainGame->gMutex.lock();
		mainGame->PopupElement(mainGame->wPosSelect);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_TRIBUTE: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		mainGame->dField.select_cancelable = BufferIO::Read<uint8_t>(pbuf) != 0;
		mainGame->dField.select_min = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.select_max = COMPAT_READ(int8_t, int32_t, pbuf);
		int count = BufferIO::Read<int32_t>(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		int c, l, s, t;
		unsigned int code;
		ClientCard* pcard;
		mainGame->dField.select_ready = false;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			l = BufferIO::Read<uint8_t>(pbuf);
			s = COMPAT_READ(int8_t, int32_t, pbuf);
			t = BufferIO::Read<uint8_t>(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->opParam = t;
			pcard->select_seq = i;
			pcard->is_selectable = true;
		}
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(fmt::sprintf(L"%ls(%d-%d)", dataManager.GetDesc(select_hint ? select_hint : 531), mainGame->dField.select_min, mainGame->dField.select_max).c_str());
		mainGame->stHintMsg->setVisible(true);
		if (mainGame->dField.select_cancelable) {
			mainGame->dField.ShowCancelOrFinishButton(1);
		}
		mainGame->gMutex.unlock();
		select_hint = 0;
		return false;
	}
	case MSG_SELECT_COUNTER: {
		/*int selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		mainGame->dField.select_counter_type = BufferIO::Read<uint16_t>(pbuf);
		mainGame->dField.select_counter_count = BufferIO::Read<uint16_t>(pbuf);
		int count = BufferIO::Read<int32_t>(pbuf);
		mainGame->dField.selectable_cards.clear();
		int c, l, s, t/*, code*/;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			/*code = */BufferIO::Read<int32_t>(pbuf);
			c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			l = BufferIO::Read<uint8_t>(pbuf);
			s = BufferIO::Read<uint8_t>(pbuf);
			t = BufferIO::Read<uint16_t>(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->opParam = (t << 16) | t;
			pcard->is_selectable = true;
		}
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(fmt::sprintf(dataManager.GetSysString(204), mainGame->dField.select_counter_count, dataManager.GetCounterName(mainGame->dField.select_counter_type)).c_str());
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_SUM: {
		/*int selecting_player*/
		if(mainGame->dInfo.compat_mode) {
			mainGame->dField.select_mode = BufferIO::Read<uint8_t>(pbuf);
			/*selecting_player = */BufferIO::Read<uint8_t>(pbuf);
		} else {
			/*selecting_player = */BufferIO::Read<uint8_t>(pbuf);
			mainGame->dField.select_mode = BufferIO::Read<uint8_t>(pbuf);
		}
		mainGame->dField.select_sumval = BufferIO::Read<int32_t>(pbuf);
		mainGame->dField.select_min = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.select_max = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.must_select_count = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.selectsum_all.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.must_select_cards.clear();
		mainGame->dField.selectsum_cards.clear();
		for (int i = 0; i < mainGame->dField.must_select_count; ++i) {
			unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			int c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			int l = BufferIO::Read<uint8_t>(pbuf);
			int s = COMPAT_READ(int8_t, int32_t, pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->opParam = BufferIO::Read<int32_t>(pbuf);
			pcard->select_seq = 0;
			mainGame->dField.must_select_cards.push_back(pcard);
		}
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		for (int i = 0; i < count; ++i) {
			unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			int c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			int l = BufferIO::Read<uint8_t>(pbuf);
			int s = COMPAT_READ(int8_t, int32_t, pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->opParam = BufferIO::Read<int32_t>(pbuf);
			pcard->select_seq = i;
			mainGame->dField.selectsum_all.push_back(pcard);
		}
		std::sort(mainGame->dField.selectsum_all.begin(), mainGame->dField.selectsum_all.end(), ClientCard::client_card_sort);
		std::wstring text = fmt::sprintf(L"%ls(%d)", dataManager.GetDesc(select_hint ? select_hint : 560), mainGame->dField.select_sumval);
		select_hint = 0;
		mainGame->wCardSelect->setText(text.c_str());
		mainGame->stHintMsg->setText(text.c_str());
		return mainGame->dField.ShowSelectSum();
	}
	case MSG_SORT_CARD:
	case MSG_SORT_CHAIN: {
		/*int player = */BufferIO::Read<uint8_t>(pbuf);
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.sort_list.clear();
		int c, l, s;
		unsigned int code;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			l = COMPAT_READ(int8_t, int32_t, pbuf);
			s = COMPAT_READ(int8_t, int32_t, pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			mainGame->dField.sort_list.push_back(0);
		}
		if (mainGame->chkAutoChain->isChecked() && mainGame->dInfo.curMsg == MSG_SORT_CHAIN) {
			mainGame->dField.sort_list.clear();
			SetResponseI(-1);
			DuelClient::SendResponse();
			return true;
		}
		if(mainGame->dInfo.curMsg == MSG_SORT_CHAIN)
			mainGame->wCardSelect->setText(dataManager.GetSysString(206).c_str());
		else
			mainGame->wCardSelect->setText(dataManager.GetSysString(205).c_str());
		mainGame->dField.select_min = 0;
		mainGame->dField.select_max = count;
		mainGame->dField.ShowSelectCard();
		return false;
	}
	case MSG_CONFIRM_DECKTOP: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		int code;
		ClientCard* pcard;
		mainGame->dField.selectable_cards.clear();
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			pbuf += (mainGame->dInfo.compat_mode) ? 3 : 6;
			pcard = *(mainGame->dField.deck[player].rbegin() + i);
			if (code != 0)
				pcard->SetCode(code);
		}
		if(mainGame->dInfo.isCatchingUp)
			return true;
		mainGame->AddLog(fmt::sprintf(dataManager.GetSysString(207).c_str(), count));
		for (int i = 0; i < count; ++i) {
			pcard = *(mainGame->dField.deck[player].rbegin() + i);
			mainGame->gMutex.lock();
			mainGame->AddLog(fmt::format(L"*[{}]", dataManager.GetName(pcard->code)), pcard->code);
			mainGame->gMutex.unlock();
			float milliseconds = 5.0f * 1000.0f / 60.0f;
			float shift = -0.75f / milliseconds;
			if (player == 1) shift *= -1.0f;
			pcard->dPos = irr::core::vector3df(shift, 0, 0);
			if(!mainGame->dField.deck_reversed && !pcard->is_reversed)
				pcard->dRot = irr::core::vector3df(0, PI / milliseconds, 0);
			else pcard->dRot = irr::core::vector3df(0, 0, 0);
			pcard->is_moving = true;
			pcard->aniFrame = milliseconds;
			mainGame->WaitFrameSignal(45);
			mainGame->dField.MoveCard(pcard, 5);
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_CONFIRM_EXTRATOP: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		int code;
		ClientCard* pcard;
		mainGame->dField.selectable_cards.clear();
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			pbuf += (mainGame->dInfo.compat_mode) ? 3 : 6;
			pcard = *(mainGame->dField.extra[player].rbegin() + i + mainGame->dField.extra_p_count[player]);
			if (code != 0)
				pcard->SetCode(code);
		}
		if(mainGame->dInfo.isCatchingUp)
			return true;
		mainGame->AddLog(fmt::sprintf(dataManager.GetSysString(207).c_str(), count));
		for (int i = 0; i < count; ++i) {
			pcard = *(mainGame->dField.extra[player].rbegin() + i + mainGame->dField.extra_p_count[player]);
			mainGame->gMutex.lock();
			mainGame->AddLog(fmt::format(L"*[{}]", dataManager.GetName(pcard->code)), pcard->code);
			mainGame->gMutex.unlock();
			float milliseconds = 5.0f * 1000.0f / 60.0f;
			if (player == 0)
				pcard->dPos = irr::core::vector3df(0, -1.0f / milliseconds, 0);
			else
				pcard->dPos = irr::core::vector3df(0.75f / milliseconds, 0, 0);
			pcard->dRot = irr::core::vector3df(0, PI / milliseconds, 0);
			pcard->is_moving = true;
			pcard->aniFrame = milliseconds;
			mainGame->WaitFrameSignal(45);
			mainGame->dField.MoveCard(pcard, 5);
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_CONFIRM_CARDS: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		int code, c, l, s;
		std::vector<ClientCard*> field_confirm;
		std::vector<ClientCard*> panel_confirm;
		ClientCard* pcard;
		if(mainGame->dInfo.isCatchingUp) {
			pbuf += count * (mainGame->dInfo.compat_mode) ? 7 : 10;
			return true;
		}
		mainGame->AddLog(fmt::sprintf(dataManager.GetSysString(208).c_str(), count));
		for (int i = 0; i < count; ++i) {
			code = BufferIO::Read<int32_t>(pbuf);
			c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
			l = BufferIO::Read<uint8_t>(pbuf);
			s = COMPAT_READ(int8_t, int32_t, pbuf);
			if (l == 0) {
				pcard = new ClientCard();
				mainGame->dField.limbo_temp.push_back(pcard);
			} else
				pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0)
				pcard->SetCode(code);
			mainGame->gMutex.lock();
			mainGame->AddLog(fmt::format(L"*[{}]", dataManager.GetName(code)), code);
			mainGame->gMutex.unlock();
			if (l & (LOCATION_EXTRA | LOCATION_DECK) || l == 0) {
				if(count == 1 && l != 0) {
					float milliseconds = 5.0f * 1000.0f / 60.0f;
					float shift = -0.75f / milliseconds;
					if (c == 0 && l == LOCATION_EXTRA) shift *= -1.0f;
					pcard->dPos = irr::core::vector3df(shift, 0, 0);
					if(((l == LOCATION_DECK) && mainGame->dField.deck_reversed) || pcard->is_reversed || (pcard->position & POS_FACEUP))
						pcard->dRot = irr::core::vector3df(0, 0, 0);
					else pcard->dRot = irr::core::vector3df(0, PI / milliseconds, 0);
					pcard->is_moving = true;
					pcard->aniFrame = milliseconds;
					mainGame->WaitFrameSignal(45);
					mainGame->dField.MoveCard(pcard, 5);
					mainGame->WaitFrameSignal(5);
				} else {
					if(!mainGame->dInfo.isReplay)
						panel_confirm.push_back(pcard);
				}
			} else {
				if(!mainGame->dInfo.isReplay || (l & LOCATION_ONFIELD))
					field_confirm.push_back(pcard);
			}
		}
		if (field_confirm.size() > 0) {
			mainGame->WaitFrameSignal(5);
			for(size_t i = 0; i < field_confirm.size(); ++i) {
				pcard = field_confirm[i];
				c = pcard->controler;
				l = pcard->location;
				if (l == LOCATION_HAND) {
					mainGame->dField.MoveCard(pcard, 5);
					pcard->is_highlighting = true;
				} else if (l == LOCATION_MZONE) {
					if (pcard->position & POS_FACEUP)
						continue;
					float milliseconds = 5.0f * 1000.0f / 60.0f;
					pcard->dPos = irr::core::vector3df(0, 0, 0);
					if (pcard->position == POS_FACEDOWN_ATTACK)
						pcard->dRot = irr::core::vector3df(0, PI / milliseconds, 0);
					else
						pcard->dRot = irr::core::vector3df(PI / milliseconds, 0, 0);
					pcard->is_moving = true;
					pcard->aniFrame = milliseconds;
				} else if (l == LOCATION_SZONE) {
					if (pcard->position & POS_FACEUP)
						continue;
					float milliseconds = 5.0f * 1000.0f / 60.0f;
					pcard->dPos = irr::core::vector3df(0, 0, 0);
					pcard->dRot = irr::core::vector3df(0, PI / milliseconds, 0);
					pcard->is_moving = true;
					pcard->aniFrame = milliseconds;
				}
			}
			if (mainGame->dInfo.isReplay)
				mainGame->WaitFrameSignal(30);
			else
				mainGame->WaitFrameSignal(90);
			for(size_t i = 0; i < field_confirm.size(); ++i) {
				pcard = field_confirm[i];
				mainGame->dField.MoveCard(pcard, 5);
				pcard->is_highlighting = false;
			}
			mainGame->WaitFrameSignal(5);
		}
		if (panel_confirm.size()) {
			std::sort(panel_confirm.begin(), panel_confirm.end(), ClientCard::client_card_sort);
			mainGame->gMutex.lock();
			mainGame->dField.selectable_cards = panel_confirm;
			mainGame->wCardSelect->setText(fmt::sprintf(dataManager.GetSysString(208), panel_confirm.size()).c_str());
			mainGame->dField.ShowSelectCard(true);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
		}
		return true;
	}
	case MSG_SHUFFLE_DECK: {
		PLAY_SOUND(SoundManager::SFX::SHUFFLE);
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		if(mainGame->dField.deck[player].size() < 2)
			return true;
		bool rev = mainGame->dField.deck_reversed;
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->dField.deck_reversed = false;
			if(rev) {
				for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
				mainGame->WaitFrameSignal(10);
			}
		}
		for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i) {
			mainGame->dField.deck[player][i]->code = 0;
			mainGame->dField.deck[player][i]->is_reversed = false;
		}
		if(!mainGame->dInfo.isCatchingUp) {
			for (int i = 0; i < 5; ++i) {
				for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
					float milliseconds = 3.0f * 1000.0f / 60.0f;
					(*cit)->dPos = irr::core::vector3df((rand() * 1.2f / RAND_MAX - 0.2f)/ milliseconds, 0, 0);
					(*cit)->dRot = irr::core::vector3df(0, 0, 0);
					(*cit)->is_moving = true;
					(*cit)->aniFrame = milliseconds;
				}
				mainGame->WaitFrameSignal(3);
				for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit)
					mainGame->dField.MoveCard(*cit, 3);
				mainGame->WaitFrameSignal(3);
			}
			mainGame->dField.deck_reversed = rev;
			if(rev) {
				for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
			}
		}
		return true;
	}
	case MSG_SHUFFLE_HAND: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		/*int count = */COMPAT_READ(int8_t, int32_t, pbuf);
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->WaitFrameSignal(5);
			if(player == 1 && !mainGame->dInfo.isReplay && !mainGame->dInfo.isSingleMode) {
				bool flip = false;
				for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
					if((*cit)->code) {
						float milliseconds = 5.0f * 1000.0f / 60.0f;
						(*cit)->dPos = irr::core::vector3df(0, 0, 0);
						(*cit)->dRot = irr::core::vector3df(1.322f / milliseconds, PI / milliseconds, 0);
						(*cit)->is_moving = true;
						(*cit)->is_hovered = false;
						(*cit)->aniFrame = milliseconds;
						flip = true;
					}
				if(flip)
					mainGame->WaitFrameSignal(5);
			}
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				float milliseconds = 5.0f * 1000.0f / 60.0f;
				(*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / milliseconds, 0, 0);
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->is_hovered = false;
				(*cit)->aniFrame = milliseconds;
			}
			mainGame->WaitFrameSignal(11);
		}
		for(auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
			(*cit)->SetCode(BufferIO::Read<int32_t>(pbuf));
			(*cit)->desc_hints.clear();
		}
		if(!mainGame->dInfo.isCatchingUp) {
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->is_hovered = false;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_SHUFFLE_EXTRA: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		if((mainGame->dField.extra[player].size() - mainGame->dField.extra_p_count[player]) < 2)
			return true;
		if(!mainGame->dInfo.isCatchingUp) {
			if(count > 1)
				PLAY_SOUND(SoundManager::SFX::SHUFFLE);
			for (int i = 0; i < 5; ++i) {
				for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
					if(!((*cit)->position & POS_FACEUP)) {
						float milliseconds = 3.0f * 1000.0f / 60.0f;
						(*cit)->dPos = irr::core::vector3df((rand() * 1.2f / RAND_MAX - 0.2f) / milliseconds, 0, 0);
						(*cit)->dRot = irr::core::vector3df(0, 0, 0);
						(*cit)->is_moving = true;
						(*cit)->aniFrame = milliseconds;
					}
				}
				mainGame->WaitFrameSignal(3);
				for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit)
					if(!((*cit)->position & POS_FACEUP))
						mainGame->dField.MoveCard(*cit, 3);
				mainGame->WaitFrameSignal(3);
			}
		}
		for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit)
			if(!((*cit)->position & POS_FACEUP))
				(*cit)->SetCode(BufferIO::Read<int32_t>(pbuf));
		return true;
	}
	case MSG_REFRESH_DECK: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		return true;
	}
	case MSG_SWAP_GRAVE_DECK: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		if(mainGame->dInfo.isCatchingUp) {
			mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
			for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit)
				(*cit)->location = LOCATION_GRAVE;
			int m = 0;
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
				if ((*cit)->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) {
					(*cit)->position = POS_FACEDOWN;
					mainGame->dField.AddCard(*cit, player, LOCATION_EXTRA, 0);
					cit = mainGame->dField.deck[player].erase(cit);
				} else {
					(*cit)->location = LOCATION_DECK;
					(*cit)->sequence = m++;
					++cit;
				}
			}
		} else {
			mainGame->gMutex.lock();
			mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
			for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit) {
				(*cit)->location = LOCATION_GRAVE;
				mainGame->dField.MoveCard(*cit, 10);
			}
			int m = 0;
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
				ClientCard* pcard = *cit;
				if (pcard->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) {
					pcard->position = POS_FACEDOWN;
					mainGame->dField.AddCard(pcard, player, LOCATION_EXTRA, 0);
					cit = mainGame->dField.deck[player].erase(cit);
				} else {
					pcard->location = LOCATION_DECK;
					pcard->sequence = m++;
					++cit;
				}
				mainGame->dField.MoveCard(pcard, 10);
			}
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_REVERSE_DECK: {
		mainGame->dField.deck_reversed = !mainGame->dField.deck_reversed;
		if(!mainGame->dInfo.isCatchingUp) {
			for(size_t i = 0; i < mainGame->dField.deck[0].size(); ++i)
				mainGame->dField.MoveCard(mainGame->dField.deck[0][i], 10);
			for(size_t i = 0; i < mainGame->dField.deck[1].size(); ++i)
				mainGame->dField.MoveCard(mainGame->dField.deck[1][i], 10);
		}
		return true;
	}
	case MSG_DECK_TOP: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int seq = COMPAT_READ(int8_t, int32_t, pbuf);
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1 - seq);
		pcard->SetCode(code & 0x7fffffff);
		bool rev = (code & 0x80000000) != 0;
		if(pcard->is_reversed != rev) {
			pcard->is_reversed = rev;
			mainGame->dField.MoveCard(pcard, 5);
		}
		return true;
	}
	case MSG_SHUFFLE_SET_CARD: {
		std::vector<ClientCard*>* lst = 0;
		int loc = BufferIO::Read<uint8_t>(pbuf);
		int count = BufferIO::Read<uint8_t>(pbuf);
		if(loc == LOCATION_MZONE)
			lst = mainGame->dField.mzone;
		else
			lst = mainGame->dField.szone;
		ClientCard* mc[7];
		ClientCard* swp;
		int ps;
		for (int i = 0; i < count; ++i) {
			CoreUtils::loc_info previous = CoreUtils::ReadLocInfo(pbuf);
			previous.controler = mainGame->LocalPlayer(previous.controler);
			mc[i] = lst[previous.controler][previous.sequence];
			mc[i]->SetCode(0);
			if(!mainGame->dInfo.isCatchingUp) {
				float milliseconds = 10.0f * 1000.0f / 60.0f;
				mc[i]->dPos = irr::core::vector3df((3.95f - mc[i]->curPos.X) / milliseconds, 0, 0.5f / milliseconds);
				mc[i]->dRot = irr::core::vector3df(0, 0, 0);
				mc[i]->is_moving = true;
				mc[i]->aniFrame = milliseconds;
			}
		}
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->WaitFrameSignal(20);
		for (int i = 0; i < count; ++i) {
			CoreUtils::loc_info previous = CoreUtils::ReadLocInfo(pbuf);
			previous.controler = mainGame->LocalPlayer(previous.controler);
			ps = mc[i]->sequence;
			if (previous.location > 0) {
				swp = lst[previous.controler][previous.sequence];
				lst[previous.controler][ps] = swp;
				lst[previous.controler][previous.sequence] = mc[i];
				mc[i]->sequence = previous.sequence;
				swp->sequence = ps;
			}
		}
		if(!mainGame->dInfo.isCatchingUp) {
			for (int i = 0; i < count; ++i) {
				mainGame->dField.MoveCard(mc[i], 10);
				for (auto pcard : mc[i]->overlayed)
					mainGame->dField.MoveCard(pcard, 10);
			}
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_NEW_TURN: {
		PLAY_SOUND(SoundManager::SFX::NEXT_TURN);
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		mainGame->dInfo.turn++;
		if(!mainGame->dInfo.isReplay && mainGame->dInfo.player_type < (mainGame->dInfo.team1 + mainGame->dInfo.team2)) {
			mainGame->btnLeaveGame->setText(dataManager.GetSysString(1351).c_str());
			mainGame->btnLeaveGame->setVisible(true);
		}
		if(!mainGame->dInfo.isReplay && mainGame->dInfo.player_type < 7) {
			if(!mainGame->chkHideHintButton->isChecked()) {
				mainGame->btnChainIgnore->setVisible(true);
				mainGame->btnChainAlways->setVisible(true);
				mainGame->btnChainWhenAvail->setVisible(true);
				//mainGame->dField.UpdateChainButtons();
			} else {
				mainGame->btnChainIgnore->setVisible(false);
				mainGame->btnChainAlways->setVisible(false);
				mainGame->btnChainWhenAvail->setVisible(false);
				mainGame->btnCancelOrFinish->setVisible(false);
			}
		}
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->showcardcode = 10;
			mainGame->showcarddif = 30;
			mainGame->showcardp = 0;
			mainGame->showcard = 101;
			mainGame->WaitFrameSignal(40);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_NEW_PHASE: {
		PLAY_SOUND(SoundManager::SFX::PHASE);
		unsigned short phase = BufferIO::Read<uint16_t>(pbuf);
		mainGame->btnDP->setVisible(false);
		mainGame->btnSP->setVisible(false);
		mainGame->btnM1->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		mainGame->showcarddif = 30;
		mainGame->showcardp = 0;
		switch (phase) {
		case PHASE_DRAW:
			mainGame->btnDP->setVisible(true);
			mainGame->showcardcode = 4;
			break;
		case PHASE_STANDBY:
			mainGame->btnSP->setVisible(true);
			mainGame->showcardcode = 5;
			break;
		case PHASE_MAIN1:
			mainGame->btnM1->setVisible(true);
			mainGame->showcardcode = 6;
			break;
		case PHASE_BATTLE_START:
			mainGame->btnBP->setVisible(true);
			mainGame->btnBP->setPressed(true);
			mainGame->btnBP->setEnabled(false);
			mainGame->showcardcode = 7;
			break;
		case PHASE_MAIN2:
			mainGame->btnM2->setVisible(true);
			mainGame->btnM2->setPressed(true);
			mainGame->btnM2->setEnabled(false);
			mainGame->showcardcode = 8;
			break;
		case PHASE_END:
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setPressed(true);
			mainGame->btnEP->setEnabled(false);
			mainGame->showcardcode = 9;
			break;
		}
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->showcard = 101;
			mainGame->WaitFrameSignal(40);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_MOVE: {
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info previous = CoreUtils::ReadLocInfo(pbuf);
		previous.controler = mainGame->LocalPlayer(previous.controler);
		CoreUtils::loc_info current = CoreUtils::ReadLocInfo(pbuf);
		current.controler = mainGame->LocalPlayer(current.controler);
		int reason = BufferIO::Read<int32_t>(pbuf);
		if (reason & REASON_DESTROY && previous.location != current.location)
			PLAY_SOUND(SoundManager::SFX::DESTROYED);
		if (previous.location == 0) {
			ClientCard* pcard = new ClientCard();
			pcard->position = current.position;
			pcard->SetCode(code);
			if(!mainGame->dInfo.isCatchingUp) {
				mainGame->gMutex.lock();
				mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
				mainGame->gMutex.unlock();
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot, true);
				pcard->curAlpha = 5;
				mainGame->dField.FadeCard(pcard, 255, 20);
				mainGame->WaitFrameSignal(20);
			} else
				mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
		} else if (current.location == 0) {
			ClientCard* pcard = nullptr;
			if(previous.location & LOCATION_OVERLAY) {
				auto olcard = mainGame->dField.GetCard(previous.controler, (previous.location & (~LOCATION_OVERLAY)) & 0xff, previous.sequence);
				pcard = *(olcard->overlayed.begin() + previous.position);
			} else
				pcard = mainGame->dField.GetCard(previous.controler, previous.location, previous.sequence);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->ClearTarget();
			for(auto eqit = pcard->equipped.begin(); eqit != pcard->equipped.end(); ++eqit)
				(*eqit)->equipTarget = 0;
			if(!mainGame->dInfo.isCatchingUp) {
				mainGame->dField.FadeCard(pcard, 5, 20);
				mainGame->WaitFrameSignal(20);
				mainGame->gMutex.lock();
				if(pcard->location & LOCATION_OVERLAY) {
					pcard->overlayTarget->overlayed.erase(pcard->overlayTarget->overlayed.begin() + pcard->sequence);
					pcard->overlayTarget = 0;
					mainGame->dField.overlay_cards.erase(pcard);
				} else
					mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
				mainGame->gMutex.unlock();
				if(pcard == mainGame->dField.hovered_card)
					mainGame->dField.hovered_card = 0;
			} else
				mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
			delete pcard;
		} else {
			if (!(previous.location & LOCATION_OVERLAY) && !(current.location & LOCATION_OVERLAY)) {
				ClientCard* pcard = mainGame->dField.GetCard(previous.controler, previous.location, previous.sequence);
				if (pcard->code != code && (code != 0 || current.location == LOCATION_EXTRA))
					pcard->SetCode(code);
				pcard->cHint = 0;
				pcard->chValue = 0;
				if((previous.location & LOCATION_ONFIELD) && (current.location != previous.location))
					pcard->counters.clear();
				if(current.location != previous.location) {
					pcard->ClearTarget();
					if(pcard->equipTarget) {
						pcard->equipTarget->is_showequip = false;
						pcard->equipTarget->equipped.erase(pcard);
						pcard->equipTarget = 0;
					}
				}
				pcard->is_hovered = false;
				pcard->is_showequip = false;
				pcard->is_showtarget = false;
				pcard->is_showchaintarget = false;
				if(mainGame->dInfo.isCatchingUp) {
					mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
					pcard->position = current.position;
					mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
				} else {
					mainGame->gMutex.lock();
					mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
					pcard->position = current.position;
					mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
					mainGame->gMutex.unlock();
					if (previous.location == current.location && previous.controler == current.controler && (current.location & (LOCATION_DECK | LOCATION_GRAVE | LOCATION_REMOVED | LOCATION_EXTRA))) {
						float milliseconds = 5.0f * 1000.0f / 60.0f;
						pcard->dPos = irr::core::vector3df(-1.5f / milliseconds, 0, 0);
						pcard->dRot = irr::core::vector3df(0, 0, 0);
						if (previous.controler == 1) pcard->dPos.X *= -0.1f;
						pcard->is_moving = true;
						pcard->aniFrame = milliseconds;
						mainGame->WaitFrameSignal(5);
						mainGame->dField.MoveCard(pcard, 5);
						mainGame->WaitFrameSignal(5);
					} else {
						if (current.location == LOCATION_MZONE && pcard->overlayed.size() > 0) {
							mainGame->gMutex.lock();
							for (size_t i = 0; i < pcard->overlayed.size(); ++i)
								mainGame->dField.MoveCard(pcard->overlayed[i], 10);
							mainGame->gMutex.unlock();
							mainGame->WaitFrameSignal(10);
						}
						if (current.location == LOCATION_HAND) {
							mainGame->gMutex.lock();
							for (size_t i = 0; i < mainGame->dField.hand[current.controler].size(); ++i)
								mainGame->dField.MoveCard(mainGame->dField.hand[current.controler][i], 10);
							mainGame->gMutex.unlock();
						} else {
							mainGame->gMutex.lock();
							mainGame->dField.MoveCard(pcard, 10);
							if (previous.location == LOCATION_HAND)
								for (size_t i = 0; i < mainGame->dField.hand[previous.controler].size(); ++i)
									mainGame->dField.MoveCard(mainGame->dField.hand[previous.controler][i], 10);
							mainGame->gMutex.unlock();
						}
						mainGame->WaitFrameSignal(5);
					}
				}
			} else if (!(previous.location & LOCATION_OVERLAY)) {
				ClientCard* pcard = mainGame->dField.GetCard(previous.controler, previous.location, previous.sequence);
				if (code != 0 && pcard->code != code)
					pcard->SetCode(code);
				pcard->counters.clear();
				pcard->ClearTarget();
				pcard->is_showtarget = false;
				pcard->is_showchaintarget = false;
				ClientCard* olcard = mainGame->dField.GetCard(current.controler, current.location & (~LOCATION_OVERLAY) & 0xff, current.sequence);
				if(mainGame->dInfo.isCatchingUp) {
					mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
					olcard->overlayed.push_back(pcard);
					mainGame->dField.overlay_cards.insert(pcard);
					pcard->overlayTarget = olcard;
					pcard->location = LOCATION_OVERLAY;
					pcard->sequence = olcard->overlayed.size() - 1;
				} else {
					mainGame->gMutex.lock();
					mainGame->dField.RemoveCard(previous.controler, previous.location, previous.sequence);
					olcard->overlayed.push_back(pcard);
					mainGame->dField.overlay_cards.insert(pcard);
					mainGame->gMutex.unlock();
					pcard->overlayTarget = olcard;
					pcard->location = LOCATION_OVERLAY;
					pcard->sequence = olcard->overlayed.size() - 1;
					if (olcard->location & LOCATION_ONFIELD) {
						mainGame->gMutex.lock();
						mainGame->dField.MoveCard(pcard, 10);
						if (previous.location == LOCATION_HAND)
							for (size_t i = 0; i < mainGame->dField.hand[previous.controler].size(); ++i)
								mainGame->dField.MoveCard(mainGame->dField.hand[previous.controler][i], 10);
						mainGame->gMutex.unlock();
						mainGame->WaitFrameSignal(5);
					}
				}
			} else if (!(current.location & LOCATION_OVERLAY)) {
				ClientCard* olcard = mainGame->dField.GetCard(previous.controler, previous.location & (~LOCATION_OVERLAY) & 0xff, previous.sequence);
				ClientCard* pcard = olcard->overlayed[previous.position];
				if(mainGame->dInfo.isCatchingUp) {
					olcard->overlayed.erase(olcard->overlayed.begin() + pcard->sequence);
					pcard->overlayTarget = 0;
					pcard->position = current.position;
					mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
					mainGame->dField.overlay_cards.erase(pcard);
					for (size_t i = 0; i < olcard->overlayed.size(); ++i)
						olcard->overlayed[i]->sequence = i;
				} else {
					mainGame->gMutex.lock();
					olcard->overlayed.erase(olcard->overlayed.begin() + pcard->sequence);
					pcard->overlayTarget = 0;
					pcard->position = current.position;
					mainGame->dField.AddCard(pcard, current.controler, current.location, current.sequence);
					mainGame->dField.overlay_cards.erase(pcard);
					for (size_t i = 0; i < olcard->overlayed.size(); ++i) {
						olcard->overlayed[i]->sequence = i;
						mainGame->dField.MoveCard(olcard->overlayed[i], 2);
					}
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
					mainGame->gMutex.lock();
					mainGame->dField.MoveCard(pcard, 10);
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
				}
			} else {
				ClientCard* olcard1 = mainGame->dField.GetCard(previous.controler, previous.location & (~LOCATION_OVERLAY) & 0xff, previous.sequence);
				ClientCard* pcard = olcard1->overlayed[previous.position];
				ClientCard* olcard2 = mainGame->dField.GetCard(current.controler, current.location & (~LOCATION_OVERLAY) & 0xff, current.sequence);
				if(mainGame->dInfo.isCatchingUp) {
					olcard1->overlayed.erase(olcard1->overlayed.begin() + pcard->sequence);
					olcard2->overlayed.push_back(pcard);
					pcard->sequence = olcard2->overlayed.size() - 1;
					pcard->location = LOCATION_OVERLAY;
					pcard->overlayTarget = olcard2;
					for (size_t i = 0; i < olcard1->overlayed.size(); ++i) {
						olcard1->overlayed[i]->sequence = i;
					}
				} else {
					mainGame->gMutex.lock();
					olcard1->overlayed.erase(olcard1->overlayed.begin() + pcard->sequence);
					olcard2->overlayed.push_back(pcard);
					pcard->sequence = olcard2->overlayed.size() - 1;
					pcard->location = LOCATION_OVERLAY;
					pcard->overlayTarget = olcard2;
					for (size_t i = 0; i < olcard1->overlayed.size(); ++i) {
						olcard1->overlayed[i]->sequence = i;
						mainGame->dField.MoveCard(olcard1->overlayed[i], 2);
					}
					mainGame->dField.MoveCard(pcard, 10);
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
				}
			}
		}
		return true;
	}
	case MSG_POS_CHANGE: {
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int cl = BufferIO::Read<uint8_t>(pbuf);
		int cs = BufferIO::Read<uint8_t>(pbuf);
		int pp = BufferIO::Read<uint8_t>(pbuf);
		int cp = BufferIO::Read<uint8_t>(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
		if((pp & POS_FACEUP) && (cp & POS_FACEDOWN)) {
			pcard->counters.clear();
			pcard->ClearTarget();
		}
		if (code != 0 && pcard->code != code)
			pcard->SetCode(code);
		pcard->position = cp;
		if(!mainGame->dInfo.isCatchingUp) {
			event_string = dataManager.GetSysString(1600);
			mainGame->dField.MoveCard(pcard, 10);
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SET: {
		PLAY_SOUND(SoundManager::SFX::SET);
		/*int code = */BufferIO::Read<int32_t>(pbuf);
		/*CoreUtils::loc_info info = */CoreUtils::ReadLocInfo(pbuf);
		event_string = dataManager.GetSysString(1601);
		return true;
	}
	case MSG_SWAP: {
		/*int code1 = */BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		info1.controler = mainGame->LocalPlayer(info1.controler);
		/*int code2 = */BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		info2.controler = mainGame->LocalPlayer(info2.controler);
		event_string = dataManager.GetSysString(1602);
		ClientCard* pc1 = mainGame->dField.GetCard(info1.controler, info1.location, info1.sequence);
		ClientCard* pc2 = mainGame->dField.GetCard(info2.controler, info2.location, info2.sequence);
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->gMutex.lock();
			mainGame->dField.RemoveCard(info1.controler, info1.location, info1.sequence);
			mainGame->dField.RemoveCard(info2.controler, info2.location, info2.sequence);
			mainGame->dField.AddCard(pc1, info2.controler, info2.location, info2.sequence);
			mainGame->dField.AddCard(pc2, info1.controler, info1.location, info1.sequence);
			mainGame->dField.MoveCard(pc1, 10);
			mainGame->dField.MoveCard(pc2, 10);
			for (size_t i = 0; i < pc1->overlayed.size(); ++i)
				mainGame->dField.MoveCard(pc1->overlayed[i], 10);
			for (size_t i = 0; i < pc2->overlayed.size(); ++i)
				mainGame->dField.MoveCard(pc2->overlayed[i], 10);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		} else {
			mainGame->dField.RemoveCard(info1.controler, info1.location, info1.sequence);
			mainGame->dField.RemoveCard(info2.controler, info2.location, info2.sequence);
			mainGame->dField.AddCard(pc1, info2.controler, info2.location, info2.sequence);
			mainGame->dField.AddCard(pc2, info1.controler, info1.location, info1.sequence);
		}
		return true;
	}
	case MSG_FIELD_DISABLED: {
		unsigned int disabled = BufferIO::Read<int32_t>(pbuf);
		if (!mainGame->dInfo.isFirst)
			disabled = (disabled >> 16) | (disabled << 16);
		mainGame->dField.disabled_field = disabled;
		return true;
	}
	case MSG_SUMMONING: {
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		/*CoreUtils::loc_info info = */CoreUtils::ReadLocInfo(pbuf);
		if(!mainGame->soundManager->PlayChant(code))
			PLAY_SOUND(SoundManager::SFX::SUMMON);
		if(!mainGame->dInfo.isCatchingUp) {
			event_string = fmt::sprintf(dataManager.GetSysString(1603), dataManager.GetName(code));
			mainGame->showcardcode = code;
			mainGame->showcarddif = 0;
			mainGame->showcardp = 0;
			mainGame->showcard = 7;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SUMMONED: {
		event_string = dataManager.GetSysString(1604);
		return true;
	}
	case MSG_SPSUMMONING: {
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		/*CoreUtils::loc_info info = */CoreUtils::ReadLocInfo(pbuf);
		if(!mainGame->soundManager->PlayChant(code))
			PLAY_SOUND(SoundManager::SFX::SPECIAL_SUMMON);
		if(!mainGame->dInfo.isCatchingUp) {
			event_string = fmt::sprintf(dataManager.GetSysString(1605), dataManager.GetName(code));
			mainGame->showcardcode = code;
			mainGame->showcarddif = 1;
			mainGame->showcard = 5;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SPSUMMONED: {
		event_string = dataManager.GetSysString(1606);
		return true;
	}
	case MSG_FLIPSUMMONING: {
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
		info.controler = mainGame->LocalPlayer(info.controler);
		if(!mainGame->soundManager->PlayChant(code))
			PLAY_SOUND(SoundManager::SFX::FLIP);
		ClientCard* pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
		pcard->SetCode(code);
		pcard->position = info.position;
		if(!mainGame->dInfo.isCatchingUp) {
			event_string = fmt::sprintf(dataManager.GetSysString(1607), dataManager.GetName(code));
			mainGame->dField.MoveCard(pcard, 10);
			mainGame->WaitFrameSignal(11);
			mainGame->showcardcode = code;
			mainGame->showcarddif = 0;
			mainGame->showcardp = 0;
			mainGame->showcard = 7;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_FLIPSUMMONED: {
		event_string = dataManager.GetSysString(1608);
		return true;
	}
	case MSG_CHAINING: {
		PLAY_SOUND(SoundManager::SFX::ACTIVATE);
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int cl = BufferIO::Read<uint8_t>(pbuf);
		int cs = COMPAT_READ(int8_t, int32_t, pbuf);
		u64 desc = COMPAT_READ(int32_t, int64_t, pbuf);
		/*int ct = */COMPAT_READ(int8_t, int32_t, pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		ClientCard* pcard = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence, info.position);
		if(pcard->code != code) {
			pcard->code = code;
			mainGame->dField.MoveCard(pcard, 10);
		}
		mainGame->showcardcode = code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 1;
		pcard->is_highlighting = true;
		if(pcard->location & 0x30) {
			float milliseconds = 5.0f * 1000.0f / 60.0f;
			float shift = -0.75f / milliseconds;
			if(info.controler == 1) milliseconds *= -1.0f;
			pcard->dPos = irr::core::vector3df(shift, 0, 0);
			pcard->dRot = irr::core::vector3df(0, 0, 0);
			pcard->is_moving = true;
			pcard->aniFrame = milliseconds;
			mainGame->WaitFrameSignal(30);
			mainGame->dField.MoveCard(pcard, 5);
		} else
			mainGame->WaitFrameSignal(30);
		pcard->is_highlighting = false;
		mainGame->dField.current_chain.chain_card = pcard;
		mainGame->dField.current_chain.code = code;
		mainGame->dField.current_chain.desc = desc;
		mainGame->dField.current_chain.controler = cc;
		mainGame->dField.current_chain.location = cl;
		mainGame->dField.current_chain.sequence = cs;
		mainGame->dField.GetChainLocation(cc, cl, cs, &mainGame->dField.current_chain.chain_pos);
		mainGame->dField.current_chain.solved = false;
		mainGame->dField.current_chain.target.clear();
		int chc = 0;
		for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
			if (cl == LOCATION_GRAVE || cl == LOCATION_REMOVED) {
				if (chit->controler == cc && chit->location == cl)
					chc++;
			} else {
				if (chit->controler == cc && chit->location == cl && chit->sequence == cs)
					chc++;
			}
		}
		if(cl == LOCATION_HAND)
			mainGame->dField.current_chain.chain_pos.X += 0.35f;
		else
			mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
		return true;
	}
	case MSG_CHAINED: {
		int ct = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		event_string = fmt::sprintf(dataManager.GetSysString(1609), dataManager.GetName(mainGame->dField.current_chain.code));
		mainGame->gMutex.lock();
		mainGame->dField.chains.push_back(mainGame->dField.current_chain);
		mainGame->gMutex.unlock();
		if (ct > 1)
			mainGame->WaitFrameSignal(20);
		mainGame->dField.last_chain = true;
		return true;
	}
	case MSG_CHAIN_SOLVING: {
		int ct = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		if (mainGame->dField.chains.size() > 1) {
			if (mainGame->dField.last_chain)
				mainGame->WaitFrameSignal(11);
			for(int i = 0; i < 5; ++i) {
				mainGame->dField.chains[ct - 1].solved = false;
				mainGame->WaitFrameSignal(3);
				mainGame->dField.chains[ct - 1].solved = true;
				mainGame->WaitFrameSignal(3);
			}
		}
		mainGame->dField.last_chain = false;
		return true;
	}
	case MSG_CHAIN_SOLVED: {
		/*int ct = */BufferIO::Read<uint8_t>(pbuf);
		return true;
	}
	case MSG_CHAIN_END: {
		for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
			for(auto tgit = chit->target.begin(); tgit != chit->target.end(); ++tgit)
				(*tgit)->is_showchaintarget = false;
			chit->chain_card->is_showchaintarget = false;
		}
		mainGame->dField.chains.clear();
		return true;
	}
	case MSG_CHAIN_NEGATED:
	case MSG_CHAIN_DISABLED: {
		int ct = BufferIO::Read<uint8_t>(pbuf);
		if(!mainGame->dInfo.isCatchingUp) {
			mainGame->showcardcode = mainGame->dField.chains[ct - 1].code;
			mainGame->showcarddif = 0;
			mainGame->showcard = 3;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_CARD_SELECTED: {
		return true;
	}
	case MSG_RANDOM_SELECTED: {
		/*int player = */BufferIO::Read<uint8_t>(pbuf);
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		if(mainGame->dInfo.isCatchingUp) {
			pbuf += count * 10;
			return true;
		}
		ClientCard* pcards[10];
		for (int i = 0; i < count; ++i) {
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			info.controler = mainGame->LocalPlayer(info.controler);
			if ((info.location & LOCATION_OVERLAY) > 0)
				pcards[i] = mainGame->dField.GetCard(info.controler, info.location & (~LOCATION_OVERLAY) & 0xff, info.sequence)->overlayed[info.position];
			else
				pcards[i] = mainGame->dField.GetCard(info.controler, info.location, info.sequence);
			pcards[i]->is_highlighting = true;
		}
		mainGame->WaitFrameSignal(30);
		for(int i = 0; i < count; ++i)
			pcards[i]->is_highlighting = false;
		return true;
	}
	case MSG_BECOME_TARGET: {
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		if(mainGame->dInfo.isCatchingUp) {
			pbuf += count * 10;
			return true;
		}
		for (int i = 0; i < count; ++i) {
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence);
			pcard->is_highlighting = true;
			mainGame->dField.current_chain.target.insert(pcard);
			if(pcard->location & LOCATION_ONFIELD) {
				for (int j = 0; j < 3; ++j) {
					mainGame->dField.FadeCard(pcard, 5, 5);
					mainGame->WaitFrameSignal(5);
					mainGame->dField.FadeCard(pcard, 255, 5);
					mainGame->WaitFrameSignal(5);
				}
			} else if(pcard->location & 0x30) {
				float milliseconds = 5.0f * 1000.0f / 60.0f;
				float shift = -0.75f / milliseconds;
				if(info.controler == 1) milliseconds *= -1.0f;
				pcard->dPos = irr::core::vector3df(shift, 0, 0);
				pcard->dRot = irr::core::vector3df(0, 0, 0);
				pcard->is_moving = true;
				pcard->aniFrame = milliseconds;
				mainGame->WaitFrameSignal(30);
				mainGame->dField.MoveCard(pcard, 5);
			} else
				mainGame->WaitFrameSignal(30);
			mainGame->AddLog(fmt::sprintf(dataManager.GetSysString(1610), dataManager.GetName(pcard->code), dataManager.FormatLocation(info.location, info.sequence), info.sequence + 1), pcard->code);
			pcard->is_highlighting = false;
		}
		return true;
	}
	case MSG_DRAW: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = COMPAT_READ(int8_t, int32_t, pbuf);
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
			pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1 - i);
			if(!mainGame->dField.deck_reversed || code)
				pcard->SetCode(code & 0x7fffffff);
		}
		if(mainGame->dInfo.isCatchingUp) {
			for (int i = 0; i < count; ++i) {
				pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1);
				mainGame->dField.deck[player].erase(mainGame->dField.deck[player].end() - 1);
				mainGame->dField.AddCard(pcard, player, LOCATION_HAND, 0);
			}
		} else {
			for (int i = 0; i < count; ++i) {
				PLAY_SOUND(SoundManager::SFX::DRAW);
				mainGame->gMutex.lock();
				pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1);
				mainGame->dField.deck[player].erase(mainGame->dField.deck[player].end() - 1);
				mainGame->dField.AddCard(pcard, player, LOCATION_HAND, 0);
				for(size_t i = 0; i < mainGame->dField.hand[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.hand[player][i], 10);
				mainGame->gMutex.unlock();
				mainGame->WaitFrameSignal(5);
			}
		}
		event_string = fmt::sprintf(dataManager.GetSysString(1611 + player), count);
		return true;
	}
	case MSG_DAMAGE: {
		PLAY_SOUND(SoundManager::SFX::DAMAGE);
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int val = BufferIO::Read<int32_t>(pbuf);
		int final = mainGame->dInfo.lp[player] - val;
		if (final < 0)
			final = 0;
		if(mainGame->dInfo.isCatchingUp) {
			mainGame->dInfo.lp[player] = final;
			mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
			return true;
		}
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		event_string = fmt::sprintf(dataManager.GetSysString(1613 + player), val);
		mainGame->lpccolor = 0xff0000;
		mainGame->lpcalpha = 0xff;
		mainGame->lpplayer = player;
		mainGame->lpcstring = fmt::format(L"-{}", val);
		mainGame->WaitFrameSignal(30);
		//float milliseconds = (float)frame * 1000.0f / 60.0f;
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = L"";
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_RECOVER: {
		PLAY_SOUND(SoundManager::SFX::RECOVER);
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int val = BufferIO::Read<int32_t>(pbuf);
		int final = mainGame->dInfo.lp[player] + val;
		if(mainGame->dInfo.isCatchingUp) {
			mainGame->dInfo.lp[player] = final;
			mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
			return true;
		}
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		event_string = fmt::sprintf(dataManager.GetSysString(1615 + player), val);
		mainGame->lpccolor = 0x00ff00;
		mainGame->lpcalpha = 0xff;
		mainGame->lpplayer = player;
		mainGame->lpcstring = fmt::format(L"+{}", val);
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = L"";
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_EQUIP: {
		PLAY_SOUND(SoundManager::SFX::EQUIP);
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(mainGame->LocalPlayer(info1.controler), info1.location, info1.sequence);
		ClientCard* pc2 = mainGame->dField.GetCard(mainGame->LocalPlayer(info2.controler), info2.location, info2.sequence);
		if(mainGame->dInfo.isCatchingUp) {
			if(pc1->equipTarget)
				pc1->equipTarget->equipped.erase(pc1);
			pc1->equipTarget = pc2;
			pc2->equipped.insert(pc1);
		} else {
			mainGame->gMutex.lock();
			if(pc1->equipTarget) {
				pc1->is_showequip = false;
				pc1->equipTarget->is_showequip = false;
				pc1->equipTarget->equipped.erase(pc1);
			}
			pc1->equipTarget = pc2;
			pc2->equipped.insert(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showequip = true;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showequip = true;
			mainGame->gMutex.unlock();
		}
		return true;
	}
	case MSG_LPUPDATE: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int val = BufferIO::Read<int32_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp) {
			mainGame->dInfo.lp[player] = val;
			mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
			return true;
		}
		mainGame->lpd = (mainGame->dInfo.lp[player] - val) / 10;
		mainGame->lpplayer = player;
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->dInfo.lp[player] = val;
		mainGame->gMutex.lock();
		mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UNEQUIP: {
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence);
		if(mainGame->dInfo.isCatchingUp) {
			pc->equipTarget->equipped.erase(pc);
			pc->equipTarget = 0;
		} else {
			mainGame->gMutex.lock();
			if (mainGame->dField.hovered_card == pc)
				pc->equipTarget->is_showequip = false;
			else if (mainGame->dField.hovered_card == pc->equipTarget)
				pc->is_showequip = false;
			pc->equipTarget->equipped.erase(pc);
			pc->equipTarget = 0;
			mainGame->gMutex.unlock();
		}
		return true;
	}
	case MSG_CARD_TARGET: {
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(mainGame->LocalPlayer(info1.controler), info1.location, info1.sequence);
		ClientCard* pc2 = mainGame->dField.GetCard(mainGame->LocalPlayer(info2.controler), info2.location, info2.sequence);
		if(mainGame->dInfo.isCatchingUp) {
			pc1->cardTarget.insert(pc2);
			pc2->ownerTarget.insert(pc1);
		} else {
			mainGame->gMutex.lock();
			pc1->cardTarget.insert(pc2);
			pc2->ownerTarget.insert(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showtarget = true;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showtarget = true;
			mainGame->gMutex.unlock();
		}
		break;
	}
	case MSG_CANCEL_TARGET: {
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(mainGame->LocalPlayer(info1.controler), info1.location, info1.sequence);
		ClientCard* pc2 = mainGame->dField.GetCard(mainGame->LocalPlayer(info2.controler), info2.location, info2.sequence);
		if(mainGame->dInfo.isCatchingUp) {
			pc1->cardTarget.erase(pc2);
			pc2->ownerTarget.erase(pc1);
		} else {
			mainGame->gMutex.lock();
			pc1->cardTarget.erase(pc2);
			pc2->ownerTarget.erase(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showtarget = false;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showtarget = false;
			mainGame->gMutex.unlock();
		}
		break;
	}
	case MSG_PAY_LPCOST: {
		PLAY_SOUND(SoundManager::SFX::DAMAGE);
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int cost = BufferIO::Read<int32_t>(pbuf);
		int final = mainGame->dInfo.lp[player] - cost;
		if (final < 0)
			final = 0;
		if(mainGame->dInfo.isCatchingUp) {
			mainGame->dInfo.lp[player] = final;
			mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
			return true;
		}
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		mainGame->lpccolor = 0x0000ff;
		mainGame->lpcalpha = 0xff;
		mainGame->lpplayer = player;
		mainGame->lpcstring = fmt::format(L"-{}", cost);
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = L"";
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		mainGame->dInfo.strLP[player] = fmt::to_wstring(mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_ADD_COUNTER: {
		PLAY_SOUND(SoundManager::SFX::COUNTER_ADD);
		int type = BufferIO::Read<uint16_t>(pbuf);
		int c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int l = BufferIO::Read<uint8_t>(pbuf);
		int s = BufferIO::Read<uint8_t>(pbuf);
		int count = BufferIO::Read<uint16_t>(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		if (pc->counters.count(type))
			pc->counters[type] += count;
		else pc->counters[type] = count;
		if(mainGame->dInfo.isCatchingUp)
			return true;
		pc->is_highlighting = true;
		mainGame->gMutex.lock();
		mainGame->stACMessage->setText(fmt::sprintf(dataManager.GetSysString(1617), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type)).c_str());
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_REMOVE_COUNTER: {
		PLAY_SOUND(SoundManager::SFX::COUNTER_REMOVE);
		int type = BufferIO::Read<uint16_t>(pbuf);
		int c = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int l = BufferIO::Read<uint8_t>(pbuf);
		int s = BufferIO::Read<uint8_t>(pbuf);
		int count = BufferIO::Read<uint16_t>(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		pc->counters[type] -= count;
		if (pc->counters[type] <= 0)
			pc->counters.erase(type);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		pc->is_highlighting = true;
		mainGame->gMutex.lock();
		mainGame->stACMessage->setText(fmt::sprintf(dataManager.GetSysString(1618), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type)).c_str());
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_ATTACK: {
		PLAY_SOUND(SoundManager::SFX::ATTACK);
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		info1.controler = mainGame->LocalPlayer(info1.controler);
		mainGame->dField.attacker = mainGame->dField.GetCard(info1.controler, info1.location, info1.sequence);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		info2.controler = mainGame->LocalPlayer(info2.controler);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		float sy;
		if (info2.location) {
			mainGame->dField.attack_target = mainGame->dField.GetCard(info2.controler, info2.location, info2.sequence);
			event_string = fmt::sprintf(dataManager.GetSysString(1619), dataManager.GetName(mainGame->dField.attacker->code),
				dataManager.GetName(mainGame->dField.attack_target->code));
			float xa = mainGame->dField.attacker->curPos.X;
			float ya = mainGame->dField.attacker->curPos.Y;
			float xd = mainGame->dField.attack_target->curPos.X;
			float yd = mainGame->dField.attack_target->curPos.Y;
			sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
			mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
			if (info1.controler == 0)
				mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
			else
				mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
		} else {
			event_string = fmt::sprintf(dataManager.GetSysString(1620), dataManager.GetName(mainGame->dField.attacker->code));
			float xa = mainGame->dField.attacker->curPos.X;
			float ya = mainGame->dField.attacker->curPos.Y;
			float xd = 3.95f;
			float yd = 3.5f;
			if (info1.controler == 0)
				yd = -3.5f;
			sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
			mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
			if (info1.controler == 0)
				mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
			else
				mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
		}
		matManager.GenArrow(sy);
		mainGame->attack_sv = 0.0f;
		mainGame->is_attacking = true;
		mainGame->WaitFrameSignal(40);
		mainGame->is_attacking = false;
		return true;
	}
	case MSG_BATTLE: {
		CoreUtils::loc_info info1 = CoreUtils::ReadLocInfo(pbuf);
		info1.controler = mainGame->LocalPlayer(info1.controler);
		int aatk = BufferIO::Read<int32_t>(pbuf);
		int adef = BufferIO::Read<int32_t>(pbuf);
		/*int da = */BufferIO::Read<uint8_t>(pbuf);
		CoreUtils::loc_info info2 = CoreUtils::ReadLocInfo(pbuf);
		info2.controler = mainGame->LocalPlayer(info2.controler);
		int datk = BufferIO::Read<int32_t>(pbuf);
		int ddef = BufferIO::Read<int32_t>(pbuf);
		/*int dd = */BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		mainGame->gMutex.lock();
		ClientCard* pcard = mainGame->dField.GetCard(info1.controler, info1.location, info1.sequence);
		if(aatk != pcard->attack) {
			pcard->attack = aatk;
			pcard->atkstring = fmt::to_wstring(aatk);
		}
		if(adef != pcard->defense) {
			pcard->defense = adef;
			pcard->defstring = fmt::to_wstring(adef);
		}
		if(info2.location) {
			pcard = mainGame->dField.GetCard(info2.controler, info2.location, info2.sequence);
			if(datk != pcard->attack) {
				pcard->attack = datk;
				pcard->atkstring = fmt::to_wstring(datk);
			}
			if(ddef != pcard->defense) {
				pcard->defense = ddef;
				pcard->defstring = fmt::to_wstring(ddef);
			}
		}
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_ATTACK_DISABLED: {
		event_string = fmt::sprintf(dataManager.GetSysString(1621), dataManager.GetName(mainGame->dField.attacker->code));
		return true;
	}
	case MSG_DAMAGE_STEP_START: {
		return true;
	}
	case MSG_DAMAGE_STEP_END: {
		return true;
	}
	case MSG_MISSED_EFFECT: {
		CoreUtils::ReadLocInfo(pbuf);
		unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
		mainGame->AddLog(fmt::sprintf(dataManager.GetSysString(1622), dataManager.GetName(code)), code);
		return true;
	}
	case MSG_TOSS_COIN: {
		PLAY_SOUND(SoundManager::SFX::COIN);
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp) {
			pbuf += count;
			return true;
		}
		std::wstring text(dataManager.GetSysString(1623));
		for (int i = 0; i < count; ++i) {
			int res = BufferIO::Read<uint8_t>(pbuf);
			text += L"[" + dataManager.GetSysString(res ? 60 : 61) +L"]";
		}
		mainGame->gMutex.lock();
		mainGame->AddLog(text);
		mainGame->stACMessage->setText(text.c_str());
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_TOSS_DICE: {
		PLAY_SOUND(SoundManager::SFX::DICE);
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp) {
			pbuf += count;
			return true;
		}
		std::wstring text(dataManager.GetSysString(1624));
		for (int i = 0; i < count; ++i) {
			int res = BufferIO::Read<uint8_t>(pbuf);
			text += L"[" + fmt::to_wstring(res) + L"]";
		}
		mainGame->gMutex.lock();
		mainGame->AddLog(text);
		mainGame->stACMessage->setText(text.c_str());
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_ROCK_PAPER_SCISSORS: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		if(mainGame->dInfo.isCatchingUp)
			return true;
		mainGame->gMutex.lock();
		mainGame->wHand->setVisible(true);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_HAND_RES: {
		int res = BufferIO::Read<uint8_t>(pbuf);
		if(mainGame->dInfo.isCatchingUp)
			return true;
		mainGame->stHintMsg->setVisible(false);
		int res1 = (res & 0x3) - 1;
		int res2 = ((res >> 2) & 0x3) - 1;
		if(mainGame->dInfo.isFirst)
			mainGame->showcardcode = res1 + (res2 << 16);
		else
			mainGame->showcardcode = res2 + (res1 << 16);
		mainGame->showcarddif = 50;
		mainGame->showcardp = 0;
		mainGame->showcard = 100;
		mainGame->WaitFrameSignal(60);
		return false;
	}
	case MSG_ANNOUNCE_RACE: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		mainGame->dField.announce_count = BufferIO::Read<uint8_t>(pbuf);
		int available = BufferIO::Read<int32_t>(pbuf);
		for(int i = 0, filter = 0x1; i < 25; ++i, filter <<= 1) {
			mainGame->chkRace[i]->setChecked(false);
			if(filter & available)
				mainGame->chkRace[i]->setVisible(true);
			else mainGame->chkRace[i]->setVisible(false);
		}
		mainGame->gMutex.lock();
		mainGame->wANRace->setText(dataManager.GetDesc(select_hint ? select_hint : 563).c_str());
		mainGame->PopupElement(mainGame->wANRace);
		mainGame->gMutex.unlock();
		select_hint = 0;
		return false;
	}
	case MSG_ANNOUNCE_ATTRIB: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		mainGame->dField.announce_count = BufferIO::Read<uint8_t>(pbuf);
		int available = BufferIO::Read<int32_t>(pbuf);
		for(int i = 0, filter = 0x1; i < 7; ++i, filter <<= 1) {
			mainGame->chkAttribute[i]->setChecked(false);
			if(filter & available)
				mainGame->chkAttribute[i]->setVisible(true);
			else mainGame->chkAttribute[i]->setVisible(false);
		}
		mainGame->gMutex.lock();
		mainGame->wANAttribute->setText(dataManager.GetDesc(select_hint ? select_hint : 562).c_str());
		mainGame->PopupElement(mainGame->wANAttribute);
		mainGame->gMutex.unlock();
		select_hint = 0;
		return false;
	}
	case MSG_ANNOUNCE_CARD: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = BufferIO::Read<uint8_t>(pbuf);
		mainGame->dField.declare_opcodes.clear();
		for (int i = 0; i < count; ++i)
			mainGame->dField.declare_opcodes.push_back(COMPAT_READ(int32_t, int64_t, pbuf));
		mainGame->gMutex.lock();
		mainGame->ebANCard->setText(L"");
		mainGame->wANCard->setText(dataManager.GetDesc(select_hint ? select_hint : 564).c_str());
		mainGame->dField.UpdateDeclarableList();
		mainGame->PopupElement(mainGame->wANCard);
		mainGame->gMutex.unlock();
		select_hint = 0;
		return false;
	}
	case MSG_ANNOUNCE_NUMBER: {
		/*int player = */mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int count = BufferIO::Read<uint8_t>(pbuf);
		mainGame->gMutex.lock();
		mainGame->cbANNumber->clear();
		for (int i = 0; i < count; ++i) {
			u32 value = (u32)(COMPAT_READ(int32_t, int64_t, pbuf));
			mainGame->cbANNumber->addItem(fmt::format(L" {}", value).c_str(), value);
		}
		mainGame->cbANNumber->setSelected(0);
		mainGame->wANNumber->setText(dataManager.GetDesc(select_hint ? select_hint : 565).c_str());
		mainGame->PopupElement(mainGame->wANNumber);
		mainGame->gMutex.unlock();
		select_hint = 0;
		return false;
	}
	case MSG_CARD_HINT: {
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
		int chtype = BufferIO::Read<uint8_t>(pbuf);
		u64 value = COMPAT_READ(int32_t, int64_t, pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence);
		if(!pcard)
			return true;
		if(chtype == CHINT_DESC_ADD) {
			pcard->desc_hints[value]++;
		} else if(chtype == CHINT_DESC_REMOVE) {
			pcard->desc_hints[value]--;
			if(pcard->desc_hints[value] == 0)
				pcard->desc_hints.erase(value);
		} else {
			pcard->cHint = chtype;
			pcard->chValue = value;
			if(chtype == CHINT_TURN) {
				if(value == 0)
					return true;
				if(mainGame->dInfo.isCatchingUp)
					return true;
				if(pcard->location & LOCATION_ONFIELD)
					pcard->is_highlighting = true;
				mainGame->showcardcode = pcard->code;
				mainGame->showcarddif = 0;
				mainGame->showcardp = value - 1;
				mainGame->showcard = 6;
				mainGame->WaitFrameSignal(30);
				pcard->is_highlighting = false;
				mainGame->showcard = 0;
			}
		}
		return true;
	}
	case MSG_PLAYER_HINT: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		int chtype = BufferIO::Read<uint8_t>(pbuf);
		u64 value = COMPAT_READ(int32_t, int64_t, pbuf);
		auto& player_desc_hints = mainGame->dField.player_desc_hints[player];
		if(chtype == PHINT_DESC_ADD) {
			player_desc_hints[value]++;
		} else if(chtype == PHINT_DESC_REMOVE) {
			player_desc_hints[value]--;
			if(player_desc_hints[value] == 0)
				player_desc_hints.erase(value);
		}
		return true;
	}
	case MSG_MATCH_KILL: {
		match_kill = BufferIO::Read<int32_t>(pbuf);
		return true;
	}
	case MSG_REMOVE_CARDS: {
		int count = BufferIO::Read<int32_t>(pbuf);
		if(count > 0) {
			std::vector<ClientCard*> cards;
			for(int i = 0; i < count; i++) {
				CoreUtils::loc_info loc = CoreUtils::ReadLocInfo(pbuf);
				ClientCard* pcard = nullptr;
				if(loc.location & LOCATION_OVERLAY) {
					auto olcard = mainGame->dField.GetCard(loc.controler, (loc.location & (~LOCATION_OVERLAY)) & 0xff, loc.sequence);
					pcard = *(olcard->overlayed.begin() + loc.position);
				}else
					pcard = mainGame->dField.GetCard(loc.controler, loc.location, loc.sequence);
				cards.push_back(pcard);
			}
			if(!mainGame->dInfo.isCatchingUp) {
				for(auto& pcard : cards)
					mainGame->dField.FadeCard(pcard, 5, 5);
				mainGame->WaitFrameSignal(5);
			}
			for(size_t i = 0; i < cards.size(); i++) {
				auto& pcard = cards[i];
				mainGame->gMutex.lock();
				if(pcard == mainGame->dField.hovered_card)
					mainGame->dField.hovered_card = 0;
				if(pcard->location & LOCATION_OVERLAY) {
					pcard->overlayTarget->overlayed.erase(pcard->overlayTarget->overlayed.begin() + pcard->sequence);
					mainGame->dField.overlay_cards.erase(pcard);
					for(size_t j = 0; j < pcard->overlayTarget->overlayed.size(); ++j)
						pcard->overlayTarget->overlayed[j]->sequence = j;
					pcard->overlayTarget = 0;
				} else
					mainGame->dField.RemoveCard(pcard->controler, pcard->location, pcard->sequence);
				mainGame->gMutex.unlock();
				delete pcard;
			}
		}
		return true;
	}
	case MSG_TAG_SWAP: {
		int player = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
		size_t mcount = (size_t)COMPAT_READ(int8_t, int32_t, pbuf);
		size_t ecount = (size_t)COMPAT_READ(int8_t, int32_t, pbuf);
		size_t pcount = (size_t)COMPAT_READ(int8_t, int32_t, pbuf);
		size_t hcount = (size_t)COMPAT_READ(int8_t, int32_t, pbuf);
		int topcode = BufferIO::Read<int32_t>(pbuf);
		if(!mainGame->dInfo.isCatchingUp) {
			float milliseconds = 5.0f * 1000.0f / 60.0f;
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 2.0f / milliseconds;
				else (*cit)->dPos.Y = -3.0f / milliseconds;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = milliseconds;
			}
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 2.0f / milliseconds;
				else (*cit)->dPos.Y = -3.0f / milliseconds;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = milliseconds;
			}
			for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 2.0f / milliseconds;
				else (*cit)->dPos.Y = -3.0f / milliseconds;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = milliseconds;
			}
			mainGame->WaitFrameSignal(5);
		}
		//
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		if(mainGame->dField.deck[player].size() > mcount) {
			while(mainGame->dField.deck[player].size() > mcount) {
				ClientCard* ccard = *mainGame->dField.deck[player].rbegin();
				mainGame->dField.deck[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.deck[player].size() < mcount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_DECK;
				ccard->sequence = mainGame->dField.deck[player].size();
				mainGame->dField.deck[player].push_back(ccard);
			}
		}
		if(mainGame->dField.hand[player].size() > hcount) {
			while(mainGame->dField.hand[player].size() > hcount) {
				ClientCard* ccard = *mainGame->dField.hand[player].rbegin();
				mainGame->dField.hand[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.hand[player].size() < hcount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_HAND;
				ccard->sequence = mainGame->dField.hand[player].size();
				mainGame->dField.hand[player].push_back(ccard);
			}
		}
		if(mainGame->dField.extra[player].size() > ecount) {
			while(mainGame->dField.extra[player].size() > ecount) {
				ClientCard* ccard = *mainGame->dField.extra[player].rbegin();
				mainGame->dField.extra[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.extra[player].size() < ecount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_EXTRA;
				ccard->sequence = mainGame->dField.extra[player].size();
				mainGame->dField.extra[player].push_back(ccard);
			}
		}
		mainGame->dField.extra_p_count[player] = pcount;
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.unlock();
		//
		if(!mainGame->dInfo.isCatchingUp) {
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			if(mainGame->dField.deck[player].size())
				(*mainGame->dField.deck[player].rbegin())->code = topcode;
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				pcard->code = BufferIO::Read<int32_t>(pbuf);
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				pcard->code = BufferIO::Read<int32_t>(pbuf) & 0x7fffffff;
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		mainGame->gMutex.lock();
		mainGame->dInfo.current_player[player] = (mainGame->dInfo.current_player[player] + 1) % ((player == 0) ? mainGame->dInfo.clientname.size() : mainGame->dInfo.hostname.size());
		mainGame->gMutex.unlock();
		break;
	}
	case MSG_RELOAD_FIELD: {
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		mainGame->dField.Clear();
		if(mainGame->dInfo.compat_mode) {
			int field = BufferIO::Read<uint8_t>(pbuf);
			mainGame->dInfo.duel_field = field & 0xf;
			mainGame->dInfo.extraval = field >> 4;
		} else {
			uint32_t opts = BufferIO::Read<uint32_t>(pbuf);
			mainGame->dInfo.duel_field = mainGame->GetMasterRule(opts, 1);
			mainGame->dInfo.extraval = opts & SPEED_DUEL;
		}
		mainGame->SetPhaseButtons();
		int val = 0;
		for(int i = 0; i < 2; ++i) {
			int p = mainGame->LocalPlayer(i);
			mainGame->dInfo.lp[p] = BufferIO::Read<int32_t>(pbuf);
			mainGame->dInfo.strLP[p] = fmt::to_wstring(mainGame->dInfo.lp[p]);
			for(int seq = 0; seq < 7; ++seq) {
				val = BufferIO::Read<uint8_t>(pbuf);
				if(val) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_MZONE, seq);
					ccard->position = BufferIO::Read<uint8_t>(pbuf);
					val = COMPAT_READ(int8_t, int32_t, pbuf);
					if(val) {
						for(int xyz = 0; xyz < val; ++xyz) {
							ClientCard* xcard = new ClientCard;
							ccard->overlayed.push_back(xcard);
							mainGame->dField.overlay_cards.insert(xcard);
							xcard->overlayTarget = ccard;
							xcard->location = LOCATION_OVERLAY;
							xcard->sequence = ccard->overlayed.size() - 1;
							xcard->owner = p;
							xcard->controler = p;
						}
					}
				}
			}
			for(int seq = 0; seq < 8; ++seq) {
				val = BufferIO::Read<uint8_t>(pbuf);
				if(val) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_SZONE, seq);
					ccard->position = BufferIO::Read<uint8_t>(pbuf);
				}
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			for(int seq = 0; seq < val; ++seq) {
				ClientCard* ccard = new ClientCard;
				mainGame->dField.AddCard(ccard, p, LOCATION_DECK, seq);
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			for(int seq = 0; seq < val; ++seq) {
				ClientCard* ccard = new ClientCard;
				mainGame->dField.AddCard(ccard, p, LOCATION_HAND, seq);
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			for(int seq = 0; seq < val; ++seq) {
				ClientCard* ccard = new ClientCard;
				mainGame->dField.AddCard(ccard, p, LOCATION_GRAVE, seq);
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			for(int seq = 0; seq < val; ++seq) {
				ClientCard* ccard = new ClientCard;
				mainGame->dField.AddCard(ccard, p, LOCATION_REMOVED, seq);
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			for(int seq = 0; seq < val; ++seq) {
				ClientCard* ccard = new ClientCard;
				mainGame->dField.AddCard(ccard, p, LOCATION_EXTRA, seq);
			}
			val = COMPAT_READ(int8_t, int32_t, pbuf);
			mainGame->dField.extra_p_count[p] = val;
		}
		mainGame->dInfo.startlp = std::max(mainGame->dInfo.lp[0], mainGame->dInfo.lp[1]);
		mainGame->dField.RefreshAllCards();
		val = COMPAT_READ(int8_t, int32_t, pbuf); //chains, always 0 in single mode
		if(!mainGame->dInfo.isSingleMode) {
			for(int i = 0; i < val; ++i) {
				unsigned int code = (unsigned int)BufferIO::Read<int32_t>(pbuf);
				CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf);
				info.controler = mainGame->LocalPlayer(info.controler);
				int cc = mainGame->LocalPlayer(BufferIO::Read<uint8_t>(pbuf));
				int cl = BufferIO::Read<uint8_t>(pbuf);
				int cs = BufferIO::Read<int32_t>(pbuf);
				u64 desc = COMPAT_READ(int32_t, int64_t, pbuf);
				ClientCard* pcard = mainGame->dField.GetCard(info.controler, info.location, info.sequence, info.position);
				mainGame->dField.current_chain.chain_card = pcard;
				mainGame->dField.current_chain.code = code;
				mainGame->dField.current_chain.desc = desc;
				mainGame->dField.current_chain.controler = cc;
				mainGame->dField.current_chain.location = cl;
				mainGame->dField.current_chain.sequence = cs;
				mainGame->dField.GetChainLocation(cc, cl, cs, &mainGame->dField.current_chain.chain_pos);
				mainGame->dField.current_chain.solved = false;
				int chc = 0;
				for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
					if (cl == LOCATION_GRAVE || cl == LOCATION_REMOVED) {
						if (chit->controler == cc && chit->location == cl)
							chc++;
					} else {
						if (chit->controler == cc && chit->location == cl && chit->sequence == cs)
							chc++;
					}
				}
				if(cl == LOCATION_HAND)
					mainGame->dField.current_chain.chain_pos.X += 0.35f;
				else
					mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
				mainGame->dField.chains.push_back(mainGame->dField.current_chain);
			}
			if(val) {
				event_string = fmt::sprintf(dataManager.GetSysString(1609), dataManager.GetName(mainGame->dField.current_chain.code));
				mainGame->dField.last_chain = true;
			}
		}
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.unlock();
		break;
	}
	}
	return true;
}
#undef PLAY_SOUND
#undef COMPAT_READ
void DuelClient::SwapField() {
	if(mainGame->dInfo.curMsg != MSG_WAITING)
		is_swapping = !is_swapping;
	else {
		mainGame->gMutex.lock();
		mainGame->dField.ReplaySwap();
		mainGame->gMutex.unlock();
	}
}
void DuelClient::SetResponseI(int respI) {
	response_buf.resize(sizeof(int));
	memcpy(response_buf.data(), &respI, sizeof(int));
}
void DuelClient::SetResponseB(void* respB, unsigned int len) {
	response_buf.resize(len);
	memcpy(response_buf.data(), respB, len);
}
void DuelClient::SendResponse() {
	switch(mainGame->dInfo.curMsg) {
	case MSG_SELECT_BATTLECMD: {
		mainGame->dField.ClearCommandFlag();
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		break;
	}
	case MSG_SELECT_IDLECMD: {
		mainGame->dField.ClearCommandFlag();
		mainGame->btnBP->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		break;
	}
	case MSG_SELECT_CARD:
	case MSG_SELECT_UNSELECT_CARD: {
		mainGame->dField.ClearSelect();
		for (auto cit = mainGame->dField.limbo_temp.begin(); cit != mainGame->dField.limbo_temp.end(); ++cit)
			delete *cit;
		mainGame->dField.limbo_temp.clear();
		break;
	}
	case MSG_SELECT_CHAIN: {
		mainGame->dField.ClearChainSelect();
		break;
	}
	case MSG_SELECT_TRIBUTE: {
		mainGame->dField.ClearSelect();
		break;
	}
	case MSG_SELECT_COUNTER: {
		mainGame->dField.ClearSelect();
		break;
	}
	case MSG_SELECT_SUM: {
		for(int i = 0; i < mainGame->dField.must_select_cards.size(); ++i) {
			mainGame->dField.must_select_cards[i]->is_selected = false;
		}
		for(size_t i = 0; i < mainGame->dField.selectsum_all.size(); ++i) {
			mainGame->dField.selectsum_all[i]->is_selectable = false;
			mainGame->dField.selectsum_all[i]->is_selected = false;
		}
		break;
	}
	case MSG_CONFIRM_CARDS: {
		for (auto cit = mainGame->dField.limbo_temp.begin(); cit != mainGame->dField.limbo_temp.end(); ++cit)
			delete *cit;
		mainGame->dField.limbo_temp.clear();
		break;
	}
	}
	if(mainGame->dInfo.isSingleMode) {
		SingleMode::SetResponse(response_buf.data(), response_buf.size());
		SingleMode::singleSignal.Set();
	} else if (!mainGame->dInfo.isReplay) {
		replay_stream.pop_back();
		mainGame->dInfo.time_player = 2;
		SendBufferToServer(CTOS_RESPONSE, response_buf.data(), response_buf.size());
	}
}
void DuelClient::BeginRefreshHost() {
	if(is_refreshing)
		return;
	is_refreshing = true;
	mainGame->btnLanRefresh->setEnabled(false);
	mainGame->lstHostList->clear();
	remotes.clear();
	hosts.clear();
	event_base* broadev = event_base_new();
	char hname[256];
	gethostname(hname, 256);
	hostent* host = gethostbyname(hname);
	if(!host)
		return;
	SOCKET reply = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in reply_addr;
	memset(&reply_addr, 0, sizeof(reply_addr));
	reply_addr.sin_family = AF_INET;
	reply_addr.sin_port = htons(7921);
	reply_addr.sin_addr.s_addr = 0;
	if(bind(reply, (sockaddr*)&reply_addr, sizeof(reply_addr)) == SOCKET_ERROR) {
		closesocket(reply);
		return;
	}
	timeval timeout = { 3, 0 };
	resp_event = event_new(broadev, reply, EV_TIMEOUT | EV_READ | EV_PERSIST, BroadcastReply, broadev);
	event_add(resp_event, &timeout);
	std::thread(RefreshThread, broadev).detach();
	//send request
	SOCKADDR_IN local;
	local.sin_family = AF_INET;
	local.sin_port = htons(7922);
	SOCKADDR_IN sockTo;
	sockTo.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	sockTo.sin_family = AF_INET;
	sockTo.sin_port = htons(7920);
	HostRequest hReq;
	hReq.identifier = NETWORK_CLIENT_ID;
	for(int i = 0; i < 8; ++i) {
		if(host->h_addr_list[i] == 0)
			break;
		unsigned int local_addr = *(unsigned int*)host->h_addr_list[i];
		local.sin_addr.s_addr = local_addr;
		SOCKET sSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sSend == INVALID_SOCKET)
			break;
		BOOL opt = TRUE;
		setsockopt(sSend, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
		if(bind(sSend, (sockaddr*)&local, sizeof(sockaddr)) == SOCKET_ERROR) {
			closesocket(sSend);
			break;
		}
		sendto(sSend, (const char*)&hReq, sizeof(HostRequest), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
		closesocket(sSend);
	}
}
int DuelClient::RefreshThread(event_base* broadev) {
	event_base_dispatch(broadev);
	evutil_socket_t fd;
	event_get_assignment(resp_event, 0, &fd, 0, 0, 0);
	evutil_closesocket(fd);
	event_free(resp_event);
	event_base_free(broadev);
	is_refreshing = false;
	return 0;
}
void DuelClient::BroadcastReply(evutil_socket_t fd, short events, void * arg) {
	if(events & EV_TIMEOUT) {
		evutil_closesocket(fd);
		event_base_loopbreak((event_base*)arg);
		if(!is_closing)
			mainGame->btnLanRefresh->setEnabled(true);
	} else if(events & EV_READ) {
		sockaddr_in bc_addr;
		socklen_t sz = sizeof(sockaddr_in);
		char buf[256];
		/*int ret = */recvfrom(fd, buf, 256, 0, (sockaddr*)&bc_addr, &sz);
		unsigned int ipaddr = bc_addr.sin_addr.s_addr;
		HostPacket* pHP = (HostPacket*)buf;
		if(!is_closing && pHP->identifier == NETWORK_SERVER_ID && remotes.find(ipaddr) == remotes.end() ) {
			mainGame->gMutex.lock();
			remotes.insert(ipaddr);
			pHP->ipaddr = ipaddr;
			hosts.push_back(*pHP);
			std::wstring hoststr;
			hoststr.append(L"[");
			hoststr.append(deckManager.GetLFListName(pHP->host.lflist));
			hoststr.append(L"][");
			hoststr.append(dataManager.GetSysString(pHP->host.rule + 1240));
			hoststr.append(L"][");
			hoststr.append(dataManager.GetSysString(pHP->host.mode + 1244));
			hoststr.append(L"][");
			hoststr.append(fmt::format(L"{:X}.0{:X}.{:X}", pHP->version >> 12, (pHP->version >> 4) & 0xff, pHP->version & 0xf));
			hoststr.append(L"][");
			int rule;
			if(pHP->host.handshake == SERVER_HANDSHAKE) {
				mainGame->GetMasterRule(pHP->host.duel_flag & ~DUEL_RELAY, pHP->host.forbiddentypes, &rule);
			} else
				rule = pHP->host.duel_rule;
			if(rule == 5)
				hoststr.append(L"Custom MR");
			else
				hoststr.append(fmt::format(L"MR {}", (rule == 0) ? 3 : rule));
			hoststr.append(L"][");
			if(pHP->host.draw_count == 1 && pHP->host.start_hand == 5 && pHP->host.start_lp == 8000
			        && !pHP->host.no_check_deck && !pHP->host.no_shuffle_deck 
					&& rule == DEFAULT_DUEL_RULE && pHP->host.extra_rules==0)
				hoststr.append(dataManager.GetSysString(1280));
			else hoststr.append(dataManager.GetSysString(1281));
			hoststr.append(L"]");
			wchar_t gamename[20];
			BufferIO::CopyWStr(pHP->name, gamename, 20);
			hoststr.append(gamename);
			mainGame->lstHostList->addItem(hoststr.c_str());
			mainGame->gMutex.unlock();
		}
	}
}
void DuelClient::ReplayPrompt(bool need_header) {
	if(need_header) {
		ReplayHeader pheader;
		pheader.id = REPLAY_YRPX;
		pheader.version = PRO_VERSION;
		pheader.flag = REPLAY_LUA64 | REPLAY_NEWREPLAY;
		last_replay.BeginRecord(false);
		last_replay.WriteHeader(pheader);
		last_replay.Write<int32_t>(mainGame->dInfo.clientname.size(), false);
		for(auto& name : mainGame->dInfo.clientname) {
			last_replay.WriteData(name.c_str(), 40, false);
		}
		last_replay.Write<int32_t>(mainGame->dInfo.hostname.size(), false);
		for(auto& name : mainGame->dInfo.hostname) {
			last_replay.WriteData(name.c_str(), 40, false);
		}
		last_replay.Write<int32_t>(mainGame->dInfo.duel_field | mainGame->dInfo.extraval >> 8);
		last_replay.WriteStream(replay_stream);
		last_replay.EndRecord();
	}
	replay_stream.clear();
	mainGame->gMutex.lock();
	mainGame->wPhase->setVisible(false);
	if(mainGame->dInfo.player_type < 7)
		mainGame->btnLeaveGame->setVisible(false);
	mainGame->btnChainIgnore->setVisible(false);
	mainGame->btnChainAlways->setVisible(false);
	mainGame->btnChainWhenAvail->setVisible(false);
	mainGame->btnCancelOrFinish->setVisible(false);
	time_t nowtime = time(NULL);
	struct tm *localedtime = localtime(&nowtime);
	char timebuf[40];
	strftime(timebuf, 40, "%Y-%m-%d %H-%M-%S", localedtime);
	size_t size = strlen(timebuf) + 1;
	wchar_t timetext[80];
	mbstowcs(timetext, timebuf, size);
	mainGame->ebRSName->setText(timetext);
	mainGame->wReplaySave->setText(dataManager.GetSysString(1340).c_str());
	mainGame->PopupElement(mainGame->wReplaySave);
	mainGame->gMutex.unlock();
	mainGame->replaySignal.Reset();
	mainGame->replaySignal.Wait();
	if(mainGame->saveReplay || !is_host) {
		if(mainGame->saveReplay)
			last_replay.SaveReplay(Utils::ParseFilename(mainGame->ebRSName->getText()));
		else last_replay.SaveReplay(TEXT("_LastReplay"));
	}

}
}
