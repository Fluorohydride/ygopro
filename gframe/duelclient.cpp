#include "duelclient.h"
#include "game.h"

namespace ygo {

unsigned DuelClient::connect_state = 0;
int DuelClient::responseI;
unsigned char DuelClient::responseB[64];
bool DuelClient::is_responseB;
unsigned char DuelClient::response_len;

bool DuelClient::StartClient(unsigned int ip, unsigned short port) {
	if(connect_state)
		return false;
	event_base* evbase;
	bufferevent* bev;
	sockaddr_in sin;
	evbase = event_base_new();
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(ip);
	sin.sin_port = htons(port);
	bev = bufferevent_socket_new(evbase, -1, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev, ClientRead, NULL, ClientEvent, NULL);
	if (bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
		bufferevent_free(bev);
		return false;
	}
	connect_state = 1;
	Thread::NewThread(ClientThread, evbase);
	return true;
}
void DuelClient::ClientRead(bufferevent* bev, void* ctx) {
}
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ptr) {
	if (events & BEV_EVENT_CONNECTED) {
		connect_state = 2;
	} else if (events & BEV_EVENT_ERROR) {

	}
}
int DuelClient::ClientThread(void* param) {
	event_base* evbase = (event_base*)param;
	event_base_dispatch(evbase);
	event_base_loopexit(evbase, NULL);
	connect_state = 0;
}
void DuelClient::HandleSTOCPacket(char* data, unsigned int len) {
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
