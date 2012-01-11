#include "netserver.h"
#include "single_duel.h"
#include "match_duel.h"

namespace ygo {
std::unordered_map<bufferevent*, DuelPlayer> NetServer::users;
event_base* NetServer::net_evbase = 0;
evconnlistener* NetServer::listener = 0;
DuelMode* NetServer::duel_mode = 0;
char NetServer::net_server_read[0x2000];
char NetServer::net_server_write[0x2000];
unsigned short NetServer::last_sent = 0;

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
	listener = evconnlistener_new_bind(net_evbase, ServerAccept, NULL,
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
	dp.name[0] = 0;
	dp.type = 0xff;
	dp.bev = bev;
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
		evbuffer_remove(input, net_server_read, packet_len + 2);
		if(packet_len)
			HandleCTOSPacket(&users[bev], &net_server_read[2], packet_len);
		len -= packet_len + 2;
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
	for(auto bit = users.begin(); bit != users.end(); ++bit)
		bufferevent_free(bit->first);
	net_evbase = 0;
	return 0;
}
void NetServer::DisconnectPlayer(DuelPlayer* dp) {
	auto bit = users.find(dp->bev);
	if(bit != users.end()) {
		users.erase(bit);
		bufferevent_free(dp->bev);
	}
}
void NetServer::HandleCTOSPacket(DuelPlayer* dp, char* data, unsigned int len) {
	char* pdata = data;
	unsigned char pktType = BufferIO::ReadUInt8(pdata);
	if(dp->state == 0xff || (dp->state && dp->state != pktType))
		return;
	switch(pktType) {
	case CTOS_RESPONSE:
		break;
	case CTOS_CHANGEDECK:
		break;
	case CTOS_PLAYER_INFO: {
		CTOS_PlayerInfo* pkt = (CTOS_PlayerInfo*)pdata;
		for(int i = 0; i < 20; ++i)
			dp->name[i] = pkt->name[i];
		break;
	}
	case CTOS_CREATE_GAME: {
		if(dp->game || duel_mode || len < sizeof(CTOS_CreateGame))
			return;
		CTOS_CreateGame* pkt = (CTOS_CreateGame*)pdata;
		if(pkt->info.mode == MODE_SINGLE) {
			duel_mode = new SingleDuel;
		} else {
			duel_mode = new MatchDuel;
		}
		duel_mode->host_info = pkt->info;
		for(int i = 0; i < 20; ++i) {
			duel_mode->name[i] = pkt->name[i];
			duel_mode->pass[i] = pkt->pass[i];
		}
	}
	case CTOS_JOIN_GAME: {
		if(pktType != CTOS_CREATE_GAME) {
			if((dp->game && dp->type != 0xff) || !duel_mode) {
				STOC_JoinFail scjf;
				scjf.reason = 0;
				SendPacketToPlayer(dp, STOC_JOIN_FAIL, scjf);
				break;
			}
			CTOS_JoinGame* pkt = (CTOS_JoinGame*)pdata;
			wchar_t jpass[20];
			for(int i = 0; i < 20; ++i) jpass[i] = pkt->pass[i];
			jpass[20] = 0;
			if(wcscmp(jpass, duel_mode->pass)) {
				STOC_JoinFail scjf;
				scjf.reason = 1;
				SendPacketToPlayer(dp, STOC_JOIN_FAIL, scjf);
				break;
			}
		}
		dp->game = duel_mode;
		if(!duel_mode->players[0] && !duel_mode->players[1] && duel_mode->observers.size() == 0)
			duel_mode->host_player = dp;
		STOC_JoinGame scjg;
		scjg.info = duel_mode->host_info;
		scjg.type = (duel_mode->host_player == dp) ? 0x10 : 0;
		if(!duel_mode->players[0] || !duel_mode->players[1]) {
			STOC_HS_PlayerEnter scpe;
			for(int i = 0; i < 20; ++i)
				scpe.name[i] = dp->name[1];
			if(duel_mode->players[0])
				NetServer::SendPacketToPlayer(duel_mode->players[0], STOC_HS_PLAYER_ENTER, scpe);
			if(duel_mode->players[1])
				NetServer::SendPacketToPlayer(duel_mode->players[1], STOC_HS_PLAYER_ENTER, scpe);
			for(auto pit = duel_mode->observers.begin(); pit != duel_mode->observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
			if(!duel_mode->players[0]) {
				duel_mode->players[0] = dp;
				dp->type = NETPLAYER_TYPE_PLAYER1;
				scjg.type |= NETPLAYER_TYPE_PLAYER1;
			} else {
				duel_mode->players[1] = dp;
				dp->type = NETPLAYER_TYPE_PLAYER2;
				scjg.type |= NETPLAYER_TYPE_PLAYER2;
			}
		} else {
			duel_mode->observers.insert(dp);
			dp->type = NETPLAYER_TYPE_OBSERVER;
			scjg.type |= NETPLAYER_TYPE_OBSERVER;
			STOC_HS_WatchChange scwc;
			scwc.watch_count = duel_mode->observers.size();
			if(duel_mode->players[0])
				NetServer::SendPacketToPlayer(duel_mode->players[0], STOC_HS_WATCH_CHANGE, scwc);
			if(duel_mode->players[1])
				NetServer::SendPacketToPlayer(duel_mode->players[1], STOC_HS_WATCH_CHANGE, scwc);
			for(auto pit = duel_mode->observers.begin(); pit != duel_mode->observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
		NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
		if(duel_mode->players[0]) {
			STOC_HS_PlayerEnter scpe;
			for(int i = 0; i < 20; ++i)
				scpe.name[i] = duel_mode->players[0]->name[1];
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
			if(duel_mode->ready[0]) {
				STOC_HS_PlayerChange scpc;
				scpc.status = PLAYERCHANGE_READY;
				NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
			}
		}
		if(duel_mode->players[1]) {
			STOC_HS_PlayerEnter scpe;
			for(int i = 0; i < 20; ++i)
				scpe.name[i] = duel_mode->players[1]->name[1];
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
			if(duel_mode->ready[1]) {
				STOC_HS_PlayerChange scpc;
				scpc.status = 0x10 | PLAYERCHANGE_READY;
				NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
			}
		}
		if(duel_mode->observers.size()) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = duel_mode->observers.size();
			NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
		}
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
	case CTOS_HS_START: {
		evconnlistener_disable(listener);
		break;
	}
	}
}

}
