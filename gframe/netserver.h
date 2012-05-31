#ifndef NETSERVER_H
#define NETSERVER_H

#include "config.h"
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"
#include <set>
#include <unordered_map>

namespace ygo {

class NetServer {
private:
	static std::unordered_map<bufferevent*, DuelPlayer> users;
	static unsigned short server_port;
	static event_base* net_evbase;
	static event* broadcast_ev;
	static evconnlistener* listener;
	static DuelMode* duel_mode;
	static char net_server_read[0x2000];
	static char net_server_write[0x2000];
	static unsigned short last_sent;

public:
	static bool StartServer(unsigned short port);
	static bool StartBroadcast();
	static void StopServer();
	static void StopBroadcast();
	static void StopListen();
	static void BroadcastEvent(evutil_socket_t fd, short events, void* arg);
	static void ServerAccept(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void ServerAcceptError(evconnlistener *listener, void* ctx);
	static void ServerEchoRead(bufferevent* bev, void* ctx);
	static void ServerEchoEvent(bufferevent* bev, short events, void* ctx);
	static int ServerThread(void* param);
	static void DisconnectPlayer(DuelPlayer* dp);
	static void HandleCTOSPacket(DuelPlayer* dp, char* data, unsigned int len);
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto) {
		char* p = net_server_write;
		BufferIO::WriteInt16(p, 1);
		BufferIO::WriteInt8(p, proto);
		last_sent = 3;
		if(!dp)
			return;
		bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	template<typename ST>
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto, ST& st) {
		char* p = net_server_write;
		BufferIO::WriteInt16(p, 1 + sizeof(ST));
		BufferIO::WriteInt8(p, proto);
		memcpy(p, &st, sizeof(ST));
		last_sent = sizeof(ST) + 3;
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, unsigned char proto, void* buffer, size_t len) {
		char* p = net_server_write;
		BufferIO::WriteInt16(p, 1 + len);
		BufferIO::WriteInt8(p, proto);
		memcpy(p, buffer, len);
		last_sent = len + 3;
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	static void ReSendToPlayer(DuelPlayer* dp) {
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
};

}

#endif //NETSERVER_H
