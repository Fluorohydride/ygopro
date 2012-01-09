#include "netserver.h"
#include "single_duel.h"
#include "match_duel.h"

namespace ygo {
std::unordered_map<bufferevent*, DuelPlayer> NetServer::users;
event_base* NetServer::net_evbase = 0;
DuelMode* NetServer::duel_mode = 0;
char NetServer::net_buffer_read[0x2000];
char NetServer::net_buffer_write[0x2000];

bool NetServer::StartServer(unsigned short port) {
	if(net_evbase)
		return false;
	net_evbase = event_base_new();
	if(!net_evbase)
		return false;
#ifdef _WIN32
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port = htons(port);
	evconnlistener* listener = evconnlistener_new_bind(net_evbase, ServerAccept, NULL,
	                           LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&sin, sizeof(sin));
	if(!listener)
		return false;
	evconnlistener_set_error_cb(listener, ServerAcceptError);
	Thread::NewThread(ServerThread, net_evbase);
	return true;
}
void NetServer::StopServer() {
	if(!net_evbase)
		return;
	event_base_loopexit(net_evbase, NULL);
}
void NetServer::ServerAccept(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx) {
	bufferevent* bev = bufferevent_socket_new(net_evbase, fd, BEV_OPT_CLOSE_ON_FREE);
	DuelPlayer dp;
	dp.output = bufferevent_get_output(bev);
	users[bev] = dp;
	bufferevent_setcb(bev, ServerEchoRead, NULL, ServerEchoEvent, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}
void NetServer::ServerAcceptError(evconnlistener* listener, void* ctx) {
	event_base_loopexit(net_evbase, NULL);
}
void NetServer::ServerEchoRead(bufferevent *bev, void *ctx) {
	evbuffer* input = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(input);
	unsigned short packet_len = 0;
	while(true) {
		if(len < 2)
			return;
		evbuffer_copyout(input, &packet_len, 2);
		if(len < packet_len + 2)
			return;
		evbuffer_remove(input, net_buffer_read, packet_len + 2);
		if(len)
			HandleCTOSPacket(users[bev], &net_buffer_read[2], len);
	}
}
void NetServer::ServerEchoEvent(bufferevent* bev, short events, void* ctx) {
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_free(bev);
		users.erase(bev);
	}
}
int NetServer::ServerThread(void* param) {
	event_base_dispatch(net_evbase);
	event_base_free(net_evbase);
	net_evbase = 0;
	return 0;
}
void NetServer::HandleCTOSPacket(DuelPlayer& dp, char* data, unsigned int len) {
	char* pdata = data;
	unsigned char pktType = NetManager::ReadUInt8(pdata);
	if(dp.state && dp.state != pktType)
		return;
	switch(pktType) {
	case CTOS_RESPONSE:
		break;
	case CTOS_CHANGEDECK:
		break;
	case CTOS_PLAYER_INFO: {
		CTOS_PlayerInfo* pkt = (CTOS_PlayerInfo*)pdata;
		for(int i = 0; i < 20; ++i)
			dp.name[i] = pkt->name[i];
		break;
	}
	case CTOS_CREATE_GAME: {
		if(dp.game || len < sizeof(CTOS_CreateGame))
			return;
		CTOS_CreateGame* pkt = (CTOS_CreateGame*)pdata;
		if(pkt->info.mode == MODE_SINGLE) {
			dp.game = new SingleDuel;
		} else {
			dp.game = new MatchDuel;
		}
		dp.game->host_info = pkt->info;
		duel_mode = dp.game;
		break;
	}
	case CTOS_JOIN_GAME: {
		if(dp.game)
			return;
		if(!duel_mode)
			return;
		if(!duel_mode->players[0])
			duel_mode->players[0] = &dp;
		else if(!duel_mode->players[1])
			duel_mode->players[1] = &dp;
		else duel_mode->observers.insert(&dp);
		break;
	}
	case CTOS_EXIT_GAME:
		break;
	case CTOS_HS_TODUELIST:
		break;
	case CTOS_HS_TOOBSERVER:
		break;
	case CTOS_HS_READY:
		break;
	case CTOS_HS_KICK1:
		break;
	case CTOS_HS_KICK2:
		break;
	case CTOS_HS_START:
		break;
	}
}

}
