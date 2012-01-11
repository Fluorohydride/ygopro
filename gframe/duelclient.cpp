#include "duelclient.h"
#include "game.h"

namespace ygo {

unsigned DuelClient::connect_state = 0;
int DuelClient::responseI;
unsigned char DuelClient::responseB[64];
bool DuelClient::is_responseB;
unsigned char DuelClient::response_len;
event_base* DuelClient::client_base = 0;
bufferevent* DuelClient::client_bev = 0;
char DuelClient::duel_client_read[0x2000];
char DuelClient::duel_client_write[0x2000];
bool DuelClient::is_host = false;

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
void DuelClient::StopClient() {
	if(connect_state != 2)
		return;
	irr::SEvent sevt;
	sevt.EventType = irr::EET_USER_EVENT;
	sevt.UserEvent.UserData1 = UEVENT_EXIT;
	sevt.UserEvent.UserData2 = 1;
	mainGame->device->postEventFromUser(sevt);
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
			HandleSTOCPacket(&duel_client_read[2], packet_len);
		len -= packet_len + 2;
	}
}
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ptr) {
	if (events & BEV_EVENT_CONNECTED) {
		mainGame->HideElement(mainGame->wCreateHost);
		mainGame->WaitFrameSignal(10);
		CTOS_CreateGame cscg;
		const wchar_t* phstr = mainGame->ebServerName->getText();
		int i = 0;
		while(i < 19 && phstr[i])
			cscg.name[i] = phstr[i++];
		cscg.name[i] = 0;
		phstr = mainGame->ebServerPass->getText();
		i = 0;
		while(i < 19 && phstr[i])
			cscg.pass[i] = phstr[i++];
		cscg.pass[i] = 0;
		cscg.info.rule = mainGame->cbRule->getSelected();
		cscg.info.mode = mainGame->cbMatchMode->getSelected();
		cscg.info.start_hand = _wtoi(mainGame->ebStartHand->getText());
		cscg.info.start_lp = _wtoi(mainGame->ebStartLP->getText());
		cscg.info.draw_count = _wtoi(mainGame->ebDrawCount->getText());
		cscg.info.lflist = mainGame->cbLFlist->getItemData(mainGame->cbLFlist->getSelected());
		cscg.info.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
		cscg.info.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
		SendPacketToServer(CTOS_CREATE_GAME, cscg);
		connect_state = 2;
	} else if (events & BEV_EVENT_ERROR) {
		mainGame->env->addMessageBox(L"", L"无法连接到主机。");
		mainGame->btnCreateHost->setEnabled(true);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		event_base_loopexit(client_base, NULL);
	}
}
int DuelClient::ClientThread(void* param) {
	event_base_dispatch(client_base);
	event_base_free(client_base);
	bufferevent_free(client_bev);
	client_base = 0;
	connect_state = 0;
}
void DuelClient::HandleSTOCPacket(char* data, unsigned int len) {
	char* pdata = data;
	unsigned char pktType = BufferIO::ReadUInt8(pdata);
	switch(pktType) {
	case STOC_JOIN_GAME: {
		STOC_JoinGame* pkt = (STOC_JoinGame*)pdata;
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
	case STOC_HS_PLAYER_ENTER:
		break;
	case STOC_HS_PLAYER_CHANGE:
		break;
	case STOC_HS_WATCH_CHANGE:
		break;
	}
}
void DuelClient::SetResponseI(int respI) {
	responseI = respI;
	is_responseB = false;
}
void DuelClient::SetResponseB(unsigned char* respB, unsigned char len) {
	memcpy(responseB, respB, len);
	is_responseB = true;
	response_len = len;
}

}
