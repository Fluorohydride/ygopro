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
	static unsigned char net_server_read[SIZE_NETWORK_BUFFER];
	static unsigned char net_server_write[SIZE_NETWORK_BUFFER];
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
	static int ServerThread();
	static void DisconnectPlayer(DuelPlayer* dp);
	static void HandleCTOSPacket(DuelPlayer* dp, unsigned char* data, unsigned int len);
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto) {
		auto p = net_server_write;
		BufferIO::WriteInt16(p, 1);
		BufferIO::WriteInt8(p, proto);
		last_sent = 3;
		if(!dp)
			return;
		bufferevent_write(dp->bev, net_server_write, 3);
	}
	template<typename ST>
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto, ST& st) {
		auto p = net_server_write;
		int blen = sizeof(ST);
		if (blen > MAX_DATA_SIZE)
			blen = MAX_DATA_SIZE;
		BufferIO::WriteInt16(p, (short)(1 + blen));
		BufferIO::WriteInt8(p, proto);
		memcpy(p, &st, blen);
		last_sent = blen + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, blen + 3);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, unsigned char proto, void* buffer, size_t len) {
		auto p = net_server_write;
		int blen = len;
		if (blen < 0)
			return;
		if (blen > MAX_DATA_SIZE)
			blen = MAX_DATA_SIZE;
		BufferIO::WriteInt16(p, (short)(1 + blen));
		BufferIO::WriteInt8(p, proto);
		memcpy(p, buffer, blen);
		last_sent = blen + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, blen + 3);
	}
	static void ReSendToPlayer(DuelPlayer* dp) {
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
};

}

#endif //NETSERVER_H
