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

bool DuelClient::StartClient(unsigned int ip, unsigned short port) {
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
	bufferevent_setcb(client_bev, ClientRead, NULL, ClientEvent, NULL);
	if (bufferevent_socket_connect(client_bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
		bufferevent_free(client_bev);
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
		irr::SEvent sevt;
		sevt.EventType = irr::EET_USER_EVENT;
		sevt.UserEvent.UserData1 = UEVENT_EXIT;
		sevt.UserEvent.UserData2 = 1;
		mainGame->device->postEventFromUser(sevt);
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
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ptr) {
	if (events & BEV_EVENT_CONNECTED) {
		mainGame->HideElement(mainGame->wCreateHost);
		mainGame->WaitFrameSignal(10);
		CTOS_PlayerInfo cspi;
		BufferIO::CopyWStr(mainGame->ebNickName->getText(), cspi.name, 20);
		SendPacketToServer(CTOS_PLAYER_INFO, cspi);
		CTOS_CreateGame cscg;
		BufferIO::CopyWStr(mainGame->ebServerName->getText(), cscg.name, 20);
		BufferIO::CopyWStr(mainGame->ebServerPass->getText(), cscg.pass, 20);
		cscg.info.rule = mainGame->cbRule->getSelected();
		cscg.info.mode = mainGame->cbMatchMode->getSelected();
		cscg.info.start_hand = _wtoi(mainGame->ebStartHand->getText());
		cscg.info.start_lp = _wtoi(mainGame->ebStartLP->getText());
		cscg.info.draw_count = _wtoi(mainGame->ebDrawCount->getText());
		cscg.info.lflist = mainGame->cbLFlist->getItemData(mainGame->cbLFlist->getSelected());
		cscg.info.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
		cscg.info.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
		SendPacketToServer(CTOS_CREATE_GAME, cscg);
		bufferevent_enable(bev, EV_READ);
		connect_state = 2;
	} else if (events & BEV_EVENT_ERROR) {
		if(!is_closing) {
			if(connect_state == 1) {
				mainGame->env->addMessageBox(L"", L"无法连接到主机。");
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
			} else if(connect_state == 2) {
				mainGame->env->addMessageBox(L"", L"已断开连接。");
				irr::SEvent sevt;
				sevt.EventType = irr::EET_USER_EVENT;
				sevt.UserEvent.UserData1 = UEVENT_EXIT;
				sevt.UserEvent.UserData2 = 2;
				mainGame->device->postEventFromUser(sevt);
			}
		}
		event_base_loopexit(client_base, NULL);
	}
}
int DuelClient::ClientThread(void* param) {
	event_base_dispatch(client_base);
	bufferevent_free(client_bev);
	event_base_free(client_base);
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
	case STOC_DECK_ERROR: {
		STOC_DeckError* pkt = (STOC_DeckError*)pdata;
		break;
	}
	case STOC_JOIN_GAME: {
		STOC_JoinGame* pkt = (STOC_JoinGame*)pdata;
		mainGame->btnHostSingleStart->setEnabled(true);
		mainGame->btnHostSingleCancel->setEnabled(true);
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
		if(selftype < 2)
			mainGame->chkHostSingleReady[selftype]->setEnabled(true);
		mainGame->WaitFrameSignal(10);
		mainGame->ShowElement(mainGame->wHostSingle);
		mainGame->WaitFrameSignal(10);
		break;
	}
	case STOC_JOIN_FAIL: {
		STOC_JoinFail* pkt = (STOC_JoinFail*)pdata;
		if(pkt->reason == 0)
			mainGame->env->addMessageBox(L"", L"无法加入主机。");
		else if(pkt->reason == 1)
			mainGame->env->addMessageBox(L"", L"密码错误。");
		else if(pkt->reason == 2)
			mainGame->env->addMessageBox(L"", L"主机拒绝了连接。");
		mainGame->btnCreateHost->setEnabled(true);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		break;
	}
	case STOC_HS_PLAYER_ENTER: {
		STOC_HS_PlayerEnter* pkt = (STOC_HS_PlayerEnter*)pdata;
		if(pkt->pos > 1)
			break;
		wchar_t name[20];
		BufferIO::CopyWStr(pkt->name, name, 20);
		mainGame->stHostSingleDuelist[pkt->pos]->setText(name);
		break;
	}
	case STOC_HS_PLAYER_CHANGE: {
		STOC_HS_PlayerChange* pkt = (STOC_HS_PlayerChange*)pdata;
		unsigned char pos = (pkt->status >> 4) & 0xf;
		unsigned char state = pkt->status & 0xf;
		if(pos > 1)
			break;
		if(state == PLAYERCHANGE_READY) {
			mainGame->chkHostSingleReady[pos]->setChecked(true);
		} else if(state == PLAYERCHANGE_NOTREADY) {
			mainGame->chkHostSingleReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_LEAVE) {
			mainGame->stHostSingleDuelist[pos]->setText(L"");
			mainGame->chkHostSingleReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_OBSERVE) {
			mainGame->stHostSingleDuelist[pos]->setText(L"");
			mainGame->chkHostSingleReady[pos]->setChecked(false);
			watching++;
			wchar_t watchbuf[32];
			myswprintf(watchbuf, L"%ls %d", dataManager.GetSysString(1253), watching);
		}
		break;
	}
	case STOC_HS_WATCH_CHANGE:
		STOC_HS_WatchChange* pkt = (STOC_HS_WatchChange*)pdata;
		watching = pkt->watch_count;
		wchar_t watchbuf[32];
		myswprintf(watchbuf, L"当前观战人数：%d", watching);
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
