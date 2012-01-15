#include "duelclient.h"
#include "game.h"

namespace ygo {

unsigned DuelClient::connect_state = 0;
unsigned char DuelClient::response_buf[64];
bool DuelClient::is_responseB;
unsigned char DuelClient::response_len;
event_base* DuelClient::client_base = 0;
bufferevent* DuelClient::client_bev = 0;
char DuelClient::duel_client_read[0x2000];
char DuelClient::duel_client_write[0x2000];
bool DuelClient::is_closing = false;

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
	if (bufferevent_socket_connect(client_bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
		bufferevent_free(client_bev);
		event_base_free(client_base);
		client_bev = 0;
		client_base = 0;
		return false;
	}
	connect_state = 1;
	Thread::NewThread(ClientThread, 0);
	return true;
}
void DuelClient::StopClient(bool is_exiting) {
	if(connect_state != 2)
		return;
	is_closing = is_exiting;
	if(!is_closing) {

	}
	event_base_loopexit(client_base, NULL);
}
void DuelClient::ClientRead(bufferevent* bev, void* ctx) {
	evbuffer* input = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(input);
	unsigned short packet_len = 0;
	while(true) {
		if(len < 2)
			return;
		evbuffer_copyout(input, &packet_len, 2);
		if(len < packet_len + 2)
			return;
		evbuffer_remove(input, duel_client_read, packet_len + 2);
		if(packet_len)
			HandleSTOCPacketLan(&duel_client_read[2], packet_len);
		len -= packet_len + 2;
	}
}
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ctx) {
	if (events & BEV_EVENT_CONNECTED) {
		bool create_game = (bool)ctx;
		CTOS_PlayerInfo cspi;
		BufferIO::CopyWStr(mainGame->ebNickName->getText(), cspi.name, 20);
		SendPacketToServer(CTOS_PLAYER_INFO, cspi);
		if(create_game) {
			CTOS_CreateGame cscg;
			BufferIO::CopyWStr(mainGame->ebServerName->getText(), cscg.name, 20);
			BufferIO::CopyWStr(mainGame->ebServerPass->getText(), cscg.pass, 20);
			cscg.info.rule = mainGame->cbRule->getSelected();
			cscg.info.mode = mainGame->cbMatchMode->getSelected();
			cscg.info.start_hand = _wtoi(mainGame->ebStartHand->getText());
			cscg.info.start_lp = _wtoi(mainGame->ebStartLP->getText());
			cscg.info.draw_count = _wtoi(mainGame->ebDrawCount->getText());
			cscg.info.lflist = mainGame->cbLFlist->getItemData(mainGame->cbLFlist->getSelected());
			cscg.info.enable_priority = mainGame->chkEnablePriority->isChecked();
			cscg.info.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
			cscg.info.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
			SendPacketToServer(CTOS_CREATE_GAME, cscg);
		} else {
			CTOS_JoinGame csjg;
			csjg.gameid = 0;
			BufferIO::CopyWStr(mainGame->ebJoinPass->getText(), csjg.pass, 20);
			SendPacketToServer(CTOS_JOIN_GAME, csjg);
		}
		bufferevent_enable(bev, EV_READ);
		connect_state = 2;
	} else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_disable(bev, EV_READ);
		if(!is_closing) {
			if(connect_state == 1) {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->gMutex.Lock();
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1400));
				mainGame->gMutex.Unlock();
			} else if(connect_state == 2) {
				if(!mainGame->dInfo.isStarted) {
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->HideElement(mainGame->wHostSingle);
					mainGame->WaitFrameSignal(10);
					mainGame->ShowElement(mainGame->wLanWindow);
					mainGame->WaitFrameSignal(10);
					mainGame->gMutex.Lock();
					if(events & BEV_EVENT_EOF)
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1401));
					else mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
					mainGame->gMutex.Unlock();
				} else {

				}
			}
		}
		event_base_loopexit(client_base, NULL);
	}
}
int DuelClient::ClientThread(void* param) {
	event_base_dispatch(client_base);
	bufferevent_free(client_bev);
	event_base_free(client_base);
	client_bev = 0;
	client_base = 0;
	connect_state = 0;
}
void DuelClient::HandleSTOCPacketLan(char* data, unsigned int len) {
	char* pdata = data;
	static unsigned int watching = 0;
	static unsigned char selftype = 0;
	static bool is_host = false;
	unsigned char pktType = BufferIO::ReadUInt8(pdata);
	switch(pktType) {
	case STOC_GAME_MSG: {
		break;
	}
	case STOC_ERROR_MSG: {
		STOC_ErrorMsg* pkt = (STOC_ErrorMsg*)pdata;
		switch(pkt->msg) {
		case ERRMSG_JOINERROR: {
			mainGame->btnCreateHost->setEnabled(true);
			mainGame->btnJoinHost->setEnabled(true);
			mainGame->btnJoinCancel->setEnabled(true);
			mainGame->gMutex.Lock();
			if(pkt->code == 0)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1403));
			else if(pkt->code == 1)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1404));
			else if(pkt->code == 2)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1405));
			mainGame->gMutex.Unlock();
			event_base_loopexit(client_base, NULL);
			break;
		}
		case ERRMSG_DECKERROR: {
			mainGame->gMutex.Lock();
			if(pkt->code == 1)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1406));
			else {
				wchar_t msgbuf[64];
				myswprintf(msgbuf, dataManager.GetSysString(1407), dataManager.GetName(pkt->code));
				mainGame->env->addMessageBox(L"", msgbuf);
			}
			mainGame->gMutex.Unlock();
			break;
		}
		}
		break;
	}
	case STOC_JOIN_GAME: {
		STOC_JoinGame* pkt = (STOC_JoinGame*)pdata;
		std::wstring str;
		wchar_t msgbuf[256];
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1226), deckManager.GetLFListName(pkt->info.lflist));
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1225), dataManager.GetSysString(1240 + pkt->info.rule));
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1227), dataManager.GetSysString(1244 + pkt->info.mode));
		str.append(msgbuf);
		if(pkt->info.enable_priority) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1236));
			str.append(msgbuf);
		}
		str.append(L"==========\n");
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1231), pkt->info.start_lp);
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1232), pkt->info.start_hand);
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1233), pkt->info.draw_count);
		str.append(msgbuf);
		if(pkt->info.no_check_deck) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1229));
			str.append(msgbuf);
		}
		if(pkt->info.no_shuffle_deck) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1230));
			str.append(msgbuf);
		}
		mainGame->gMutex.Lock();
		mainGame->SetStaticText(mainGame->stHostSingleRule, 180, mainGame->guiFont, (wchar_t*)str.c_str());
		mainGame->RefreshDeck(mainGame->cbDeckSelect);
		mainGame->gMutex.Unlock();
		if(mainGame->wCreateHost->isVisible())
			mainGame->HideElement(mainGame->wCreateHost);
		else if (mainGame->wLanWindow->isVisible())
			mainGame->HideElement(mainGame->wLanWindow);
		mainGame->WaitFrameSignal(10);
		mainGame->ShowElement(mainGame->wHostSingle);
		mainGame->WaitFrameSignal(10);
		break;
	}
	case STOC_TYPE_CHANGE: {
		STOC_TypeChange* pkt = (STOC_TypeChange*)pdata;
		selftype = pkt->type & 0xf;
		is_host = (pkt->type >> 4) & 0xf;
		if(is_host) {
			mainGame->btnHostSingleStart->setVisible(true);
			mainGame->btnHostSingleKick[0]->setEnabled(true);
			mainGame->btnHostSingleKick[1]->setEnabled(true);
		} else {
			mainGame->btnHostSingleStart->setVisible(false);
			mainGame->btnHostSingleKick[0]->setEnabled(false);
			mainGame->btnHostSingleKick[1]->setEnabled(false);
		}
		mainGame->chkHostSingleReady[0]->setEnabled(false);
		mainGame->chkHostSingleReady[0]->setChecked(false);
		mainGame->chkHostSingleReady[1]->setEnabled(false);
		mainGame->chkHostSingleReady[1]->setChecked(false);
		if(selftype < 2) {
			mainGame->chkHostSingleReady[selftype]->setEnabled(true);
			mainGame->btnHostSingleDuelist->setEnabled(false);
			mainGame->btnHostSingleOB->setEnabled(true);
		} else {
			mainGame->btnHostSingleDuelist->setEnabled(true);
			mainGame->btnHostSingleOB->setEnabled(false);
		}
		break;
	}
	case STOC_GAME_START: {
		break;
	}
	case STOC_HS_PLAYER_ENTER: {
		STOC_HS_PlayerEnter* pkt = (STOC_HS_PlayerEnter*)pdata;
		if(pkt->pos > 1)
			break;
		wchar_t name[20];
		BufferIO::CopyWStr(pkt->name, name, 20);
		mainGame->gMutex.Lock();
		mainGame->stHostSingleDuelist[pkt->pos]->setText(name);
		mainGame->gMutex.Unlock();
		break;
	}
	case STOC_HS_PLAYER_CHANGE: {
		STOC_HS_PlayerChange* pkt = (STOC_HS_PlayerChange*)pdata;
		unsigned char pos = (pkt->status >> 4) & 0xf;
		unsigned char state = pkt->status & 0xf;
		if(pos > 1)
			break;
		mainGame->gMutex.Lock();
		if(state == PLAYERCHANGE_READY) {
			mainGame->chkHostSingleReady[pos]->setChecked(true);
		} else if(state == PLAYERCHANGE_NOTREADY) {
			mainGame->chkHostSingleReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_LEAVE) {
			mainGame->stHostSingleDuelist[pos]->setText(L"");
			mainGame->chkHostSingleReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_OBSERVE) {
			watching++;
			wchar_t watchbuf[32];
			myswprintf(watchbuf, L"%ls %d", dataManager.GetSysString(1253), watching);
			mainGame->stHostSingleDuelist[pos]->setText(L"");
			mainGame->chkHostSingleReady[pos]->setChecked(false);
			mainGame->stHostSingleOB->setText(watchbuf);
		}
		mainGame->gMutex.Unlock();
		break;
	}
	case STOC_HS_WATCH_CHANGE:
		STOC_HS_WatchChange* pkt = (STOC_HS_WatchChange*)pdata;
		watching = pkt->watch_count;
		wchar_t watchbuf[32];
		myswprintf(watchbuf, L"%ls %d", dataManager.GetSysString(1253), watching);
		mainGame->gMutex.Lock();
		mainGame->stHostSingleOB->setText(watchbuf);
		mainGame->gMutex.Unlock();
		break;
	}
}
void DuelClient::SetResponseI(int respI) {
	*((int*)response_buf) = respI;
	is_responseB = false;
}
void DuelClient::SetResponseB(unsigned char* respB, unsigned char len) {
	memcpy(response_buf, respB, len);
	is_responseB = true;
	response_len = len;
}

}
