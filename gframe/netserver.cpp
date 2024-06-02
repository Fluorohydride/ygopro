#include "netserver.h"
#include "single_duel.h"
#include "tag_duel.h"

namespace ygo {
std::unordered_map<bufferevent*, DuelPlayer> NetServer::users;
unsigned short NetServer::server_port = 0;
event_base* NetServer::net_evbase = 0;
event* NetServer::broadcast_ev = 0;
evconnlistener* NetServer::listener = 0;
DuelMode* NetServer::duel_mode = 0;
unsigned char NetServer::net_server_read[SIZE_NETWORK_BUFFER];
int NetServer::read_len = 0;
unsigned char NetServer::net_server_write[SIZE_NETWORK_BUFFER];
unsigned short NetServer::last_sent = 0;

#ifdef YGOPRO_SERVER_MODE
extern unsigned short replay_mode;
extern HostInfo game_info;

void NetServer::InitDuel()
{
	if(game_info.mode == MODE_SINGLE) {
		duel_mode = new SingleDuel(false);
		duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, SingleDuel::SingleTimer, duel_mode);
	} else if(game_info.mode == MODE_MATCH) {
		duel_mode = new SingleDuel(true);
		duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, SingleDuel::SingleTimer, duel_mode);
	} else if(game_info.mode == MODE_TAG) {
		duel_mode = new TagDuel();
		duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, TagDuel::TagTimer, duel_mode);
	}

	CTOS_CreateGame* pkt = new CTOS_CreateGame;
	
	pkt->info.mode = game_info.mode;
	pkt->info.start_hand = game_info.start_hand;
	pkt->info.start_lp = game_info.start_lp;
	pkt->info.draw_count = game_info.draw_count;
	pkt->info.no_check_deck = game_info.no_check_deck;
	pkt->info.no_shuffle_deck = game_info.no_shuffle_deck;
	pkt->info.duel_rule = game_info.duel_rule;
	pkt->info.rule = game_info.rule;
	pkt->info.time_limit = game_info.time_limit;

	if(game_info.lflist == 999)
		pkt->info.lflist = 0;
	else if(game_info.lflist >= deckManager._lfList.size())
		pkt->info.lflist = deckManager._lfList[0].hash;
	else
		pkt->info.lflist = deckManager._lfList[game_info.lflist].hash;
	
	duel_mode->host_info = pkt->info;
	
	BufferIO::CopyWStr(pkt->name, duel_mode->name, 20);
	BufferIO::CopyWStr(pkt->pass, duel_mode->pass, 20);
}

bool NetServer::IsCanIncreaseTime(unsigned short gameMsg, void *pdata, unsigned int len) {
	int32* ivalue = (int32*)pdata;
	switch(gameMsg) {
		case MSG_RETRY:
		case MSG_SELECT_UNSELECT_CARD:
			return false;
		case MSG_SELECT_CHAIN:
			return ivalue[0] != -1;
		case MSG_SELECT_IDLECMD: {
			int32 idleChoice = ivalue[0] & 0xffff;
			return idleChoice <= 5; // no shuffle hand, enter other phases
		}
		case MSG_SELECT_BATTLECMD: {
			int32 battleChoice = ivalue[0] & 0xffff;
			return battleChoice <= 1; // attack only
		}
		default:
			return true;
	}
}

unsigned short NetServer::StartServer(unsigned short port) {
#else
bool NetServer::StartServer(unsigned short port) {
#endif //YGOPRO_SERVER_MODE
	if(net_evbase)
		return false;
	net_evbase = event_base_new();
	if(!net_evbase)
		return false;
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	server_port = port;
	sin.sin_family = AF_INET;
#ifdef SERVER_PRO2_SUPPORT
	sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
#else
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	sin.sin_port = htons(port);
	listener = evconnlistener_new_bind(net_evbase, ServerAccept, NULL,
	                                   LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&sin, sizeof(sin));
	if(!listener) {
		event_base_free(net_evbase);
		net_evbase = 0;
		return false;
	}
	evconnlistener_set_error_cb(listener, ServerAcceptError);
	std::thread(ServerThread).detach();
#ifdef YGOPRO_SERVER_MODE
	evutil_socket_t fd = evconnlistener_get_fd(listener);
	socklen_t addrlen = sizeof(sockaddr);
	sockaddr_in addr;
	getsockname(fd, (sockaddr*)&addr, &addrlen);
	return ntohs(addr.sin_port);
#else
	return true;
#endif //YGOPRO_SERVER_MODE
}
bool NetServer::StartBroadcast() {
	if(!net_evbase)
		return false;
	SOCKET udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int opt = TRUE;
	setsockopt(udp, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof opt);
	setsockopt(udp, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof opt);
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7920);
	addr.sin_addr.s_addr = 0;
	if(bind(udp, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		closesocket(udp);
		return false;
	}
	broadcast_ev = event_new(net_evbase, udp, EV_READ | EV_PERSIST, BroadcastEvent, NULL);
	event_add(broadcast_ev, NULL);
	return true;
}
void NetServer::StopServer() {
	if(!net_evbase)
		return;
	if(duel_mode)
		duel_mode->EndDuel();
#ifdef YGOPRO_SERVER_MODE // For solving the problem of connection lost after duel. See https://github.com/Fluorohydride/ygopro/issues/2067 for details.
	timeval etv = { 0, 1 };
	event_base_loopexit(net_evbase, &etv);
#else
	event_base_loopexit(net_evbase, 0);
#endif
}
void NetServer::StopBroadcast() {
	if(!net_evbase || !broadcast_ev)
		return;
	event_del(broadcast_ev);
	evutil_socket_t fd;
	event_get_assignment(broadcast_ev, 0, &fd, 0, 0, 0);
	evutil_closesocket(fd);
	event_free(broadcast_ev);
	broadcast_ev = 0;
}
void NetServer::StopListen() {
	evconnlistener_disable(listener);
	StopBroadcast();
}
void NetServer::BroadcastEvent(evutil_socket_t fd, short events, void* arg) {
	sockaddr_in bc_addr;
	socklen_t sz = sizeof(sockaddr_in);
	char buf[256];
	int ret = recvfrom(fd, buf, 256, 0, (sockaddr*)&bc_addr, &sz);
	if(ret == -1)
		return;
	HostRequest packet;
	std::memcpy(&packet, buf, sizeof packet);
	const HostRequest* pHR = &packet;
	if(pHR->identifier == NETWORK_CLIENT_ID) {
		SOCKADDR_IN sockTo;
		sockTo.sin_addr.s_addr = bc_addr.sin_addr.s_addr;
		sockTo.sin_family = AF_INET;
		sockTo.sin_port = htons(7921);
		HostPacket hp;
		hp.identifier = NETWORK_SERVER_ID;
		hp.port = server_port;
		hp.version = PRO_VERSION;
		hp.host = duel_mode->host_info;
		BufferIO::CopyWStr(duel_mode->name, hp.name, 20);
		sendto(fd, (const char*)&hp, sizeof(HostPacket), 0, (sockaddr*)&sockTo, sizeof(sockTo));
	}
}
void NetServer::ServerAccept(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx) {
	bufferevent* bev = bufferevent_socket_new(net_evbase, fd, BEV_OPT_CLOSE_ON_FREE);
	DuelPlayer dp;
	dp.name[0] = 0;
	dp.type = 0xff;
	dp.bev = bev;
	users[bev] = dp;
	bufferevent_setcb(bev, ServerEchoRead, NULL, ServerEchoEvent, NULL);
	bufferevent_enable(bev, EV_READ);
}
void NetServer::ServerAcceptError(evconnlistener* listener, void* ctx) {
	event_base_loopexit(net_evbase, 0);
}
/*
* packet_len: 2 bytes
* proto: 1 byte
* [data]: (packet_len - 1) bytes
*/
void NetServer::ServerEchoRead(bufferevent *bev, void *ctx) {
	evbuffer* input = bufferevent_get_input(bev);
	int len = evbuffer_get_length(input);
	unsigned short packet_len = 0;
	while(true) {
		if(len < 2)
			return;
		evbuffer_copyout(input, &packet_len, 2);
		if (packet_len + 2 > SIZE_NETWORK_BUFFER) {
			ServerEchoEvent(bev, BEV_EVENT_ERROR, 0);
			return;
		}
		if (len < packet_len + 2)
			return;
		if (packet_len < 1)
			return;
		read_len = evbuffer_remove(input, net_server_read, packet_len + 2);
		if (read_len >= 3)
			HandleCTOSPacket(&users[bev], &net_server_read[2], read_len - 2);
		len -= packet_len + 2;
	}
}
void NetServer::ServerEchoEvent(bufferevent* bev, short events, void* ctx) {
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		DuelPlayer* dp = &users[bev];
		DuelMode* dm = dp->game;
		if(dm)
			dm->LeaveGame(dp);
		else
			DisconnectPlayer(dp);
	}
}
int NetServer::ServerThread() {
	event_base_dispatch(net_evbase);
	for(auto bit = users.begin(); bit != users.end(); ++bit) {
		bufferevent_disable(bit->first, EV_READ);
		bufferevent_free(bit->first);
	}
	users.clear();
	evconnlistener_free(listener);
	listener = 0;
	if(broadcast_ev) {
		evutil_socket_t fd;
		event_get_assignment(broadcast_ev, 0, &fd, 0, 0, 0);
		evutil_closesocket(fd);
		event_free(broadcast_ev);
		broadcast_ev = 0;
	}
	if(duel_mode) {
		event_free(duel_mode->etimer);
		delete duel_mode;
	}
	duel_mode = 0;
	event_base_free(net_evbase);
	net_evbase = 0;
	return 0;
}
void NetServer::DisconnectPlayer(DuelPlayer* dp) {
	auto bit = users.find(dp->bev);
	if(bit != users.end()) {
		bufferevent_flush(dp->bev, EV_WRITE, BEV_FLUSH);
		bufferevent_disable(dp->bev, EV_READ);
		bufferevent_free(dp->bev);
		users.erase(bit);
	}
}
void NetServer::HandleCTOSPacket(DuelPlayer* dp, unsigned char* data, int len) {
	auto pdata = data;
	unsigned char pktType = BufferIO::ReadUInt8(pdata);
#ifdef YGOPRO_SERVER_MODE
	if((pktType != CTOS_SURRENDER) && (pktType != CTOS_CHAT) && (pktType != CTOS_REQUEST_FIELD) && (dp->state == 0xff || (dp->state && dp->state != pktType)))
#else
	if((pktType != CTOS_SURRENDER) && (pktType != CTOS_CHAT) && (dp->state == 0xff || (dp->state && dp->state != pktType)))
#endif
		return;
	switch(pktType) {
	case CTOS_RESPONSE: {
		if(!dp->game || !duel_mode->pduel)
			return;
		if (len < 1 + (int)sizeof(unsigned char))
			return;
		duel_mode->GetResponse(dp, pdata, len - 1);
		break;
	}
	case CTOS_TIME_CONFIRM: {
		if(!dp->game || !duel_mode->pduel)
			return;
		duel_mode->TimeConfirm(dp);
		break;
	}
	case CTOS_CHAT: {
		if(!dp->game)
			return;
		if (len < 1 + (int)sizeof(unsigned char))
			return;
		duel_mode->Chat(dp, pdata, len - 1);
		break;
	}
	case CTOS_UPDATE_DECK: {
		if(!dp->game)
			return;
		if (len < 1 + (int)sizeof(unsigned char))
			return;
		duel_mode->UpdateDeck(dp, pdata, len - 1);
		break;
	}
	case CTOS_HAND_RESULT: {
		if(!dp->game)
			return;
		if (len < 1 + (int)sizeof(CTOS_HandResult))
			return;
		CTOS_HandResult packet;
		std::memcpy(&packet, pdata, sizeof packet);
		const auto* pkt = &packet;
		dp->game->HandResult(dp, pkt->res);
		break;
	}
	case CTOS_TP_RESULT: {
		if(!dp->game)
			return;
		if (len < 1 + (int)sizeof(CTOS_TPResult))
			return;
		CTOS_TPResult packet;
		std::memcpy(&packet, pdata, sizeof packet);
		const auto* pkt = &packet;
		dp->game->TPResult(dp, pkt->res);
		break;
	}
	case CTOS_PLAYER_INFO: {
		if (len < 1 + (int)sizeof(CTOS_PlayerInfo))
			return;
		CTOS_PlayerInfo packet;
		std::memcpy(&packet, pdata, sizeof packet);
		const auto* pkt = &packet;
		BufferIO::CopyWStr(pkt->name, dp->name, 20);
		break;
	}
	case CTOS_CREATE_GAME: {
		if(dp->game || duel_mode)
			return;
		if (len < 1 + (int)sizeof(CTOS_CreateGame))
			return;
		CTOS_CreateGame packet;
		std::memcpy(&packet, pdata, sizeof packet);
		auto pkt = &packet;
		if(pkt->info.mode == MODE_SINGLE) {
			duel_mode = new SingleDuel(false);
			duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, SingleDuel::SingleTimer, duel_mode);
		} else if(pkt->info.mode == MODE_MATCH) {
			duel_mode = new SingleDuel(true);
			duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, SingleDuel::SingleTimer, duel_mode);
		} else if(pkt->info.mode == MODE_TAG) {
			duel_mode = new TagDuel();
			duel_mode->etimer = event_new(net_evbase, 0, EV_TIMEOUT | EV_PERSIST, TagDuel::TagTimer, duel_mode);
		}
		if(pkt->info.rule > 5)
			pkt->info.rule = 5;
		if(pkt->info.mode > 2)
			pkt->info.mode = 0;
		unsigned int hash = 1;
		for(auto lfit = deckManager._lfList.begin(); lfit != deckManager._lfList.end(); ++lfit) {
			if(pkt->info.lflist == lfit->hash) {
				hash = pkt->info.lflist;
				break;
			}
		}
		if(hash == 1)
			pkt->info.lflist = deckManager._lfList[0].hash;
		std::memcpy(pdata, &packet, sizeof packet);
		duel_mode->host_info = pkt->info;
		BufferIO::CopyWStr(pkt->name, duel_mode->name, 20);
		BufferIO::CopyWStr(pkt->pass, duel_mode->pass, 20);
		duel_mode->JoinGame(dp, 0, true);
		StartBroadcast();
		break;
	}
	case CTOS_JOIN_GAME: {
		if (!duel_mode)
			return;
		if (len < 1 + (int)sizeof(CTOS_JoinGame))
			return;
		duel_mode->JoinGame(dp, pdata, false);
		break;
	}
	case CTOS_LEAVE_GAME: {
		if (!duel_mode)
			return;
		duel_mode->LeaveGame(dp);
		break;
	}
	case CTOS_SURRENDER: {
		if (!duel_mode)
			return;
		duel_mode->Surrender(dp);
		break;
	}
	case CTOS_HS_TODUELIST: {
		if (!duel_mode || duel_mode->pduel)
			return;
		duel_mode->ToDuelist(dp);
		break;
	}
	case CTOS_HS_TOOBSERVER: {
		if (!duel_mode || duel_mode->pduel)
			return;
		duel_mode->ToObserver(dp);
		break;
	}
	case CTOS_HS_READY:
	case CTOS_HS_NOTREADY: {
		if (!duel_mode || duel_mode->pduel)
			return;
		duel_mode->PlayerReady(dp, (CTOS_HS_NOTREADY - pktType) != 0);
		break;
	}
	case CTOS_HS_KICK: {
		if (!duel_mode || duel_mode->pduel)
			return;
		if (len < 1 + (int)sizeof(CTOS_Kick))
			return;
		CTOS_Kick packet;
		std::memcpy(&packet, pdata, sizeof packet);
		const auto* pkt = &packet;
		duel_mode->PlayerKick(dp, pkt->pos);
		break;
	}
	case CTOS_HS_START: {
		if (!duel_mode || duel_mode->pduel)
			return;
		duel_mode->StartDuel(dp);
		break;
	}
#ifdef YGOPRO_SERVER_MODE
	case CTOS_REQUEST_FIELD: {
		if(!dp->game || !duel_mode->pduel)
			break;
		duel_mode->RequestField(dp);
		break;
	}
#endif
	}
}
size_t NetServer::CreateChatPacket(unsigned char* src, int src_size, unsigned char* dst, uint16_t dst_player_type) {
	if (!check_msg_size(src_size))
		return 0;
	uint16_t src_msg[LEN_CHAT_MSG];
	std::memcpy(src_msg, src, src_size);
	const int src_len = src_size / sizeof(uint16_t);
	if (src_msg[src_len - 1] != 0)
		return 0;
	// STOC_Chat packet
	auto pdst = dst;
	buffer_write<uint16_t>(pdst, dst_player_type);
	buffer_write_block(pdst, src_msg, src_size);
	return sizeof(dst_player_type) + src_size;
}

}
