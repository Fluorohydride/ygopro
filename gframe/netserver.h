#ifndef NETSERVER_H
#define NETSERVER_H

#include <set>
#include <unordered_map>
#include "config.h"
#include "core_utils.h"
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"

namespace ygo {

class NetServer {
private:
	static std::unordered_map<bufferevent*, DuelPlayer> users;
	static uint16_t server_port;
	static event_base* net_evbase;
	static event* broadcast_ev;
	static evconnlistener* listener;
	static DuelMode* duel_mode;
	static uint8_t net_server_read[0x20000];
	static uint8_t net_server_write[0x20000];
	static uint16_t last_sent;

public:
	static bool StartServer(uint16_t port);
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
	static void HandleCTOSPacket(DuelPlayer* dp, uint8_t* data, uint32_t len);
	static void SendPacketToPlayer(DuelPlayer* dp, uint8_t proto) {
		auto* p = net_server_write;
		BufferIO::Write<uint16_t>(p, 1u);
		BufferIO::Write<uint8_t>(p, proto);
		last_sent = 3u;
		if(!dp)
			return;
		bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	template<typename ST>
	static void SendPacketToPlayer(DuelPlayer* dp, uint8_t proto, ST& st) {
		auto* p = net_server_write;
		BufferIO::Write<uint16_t>(p, static_cast<uint16_t>(1u + sizeof(ST)));
		BufferIO::Write<uint8_t>(p, proto);
		memcpy(p, &st, sizeof(ST));
		last_sent = static_cast<uint16_t>(sizeof(ST) + 3u);
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, uint8_t proto, void* buffer, size_t len) {
		auto* p = net_server_write;
		BufferIO::Write<uint16_t>(p, static_cast<uint16_t>(1u + len));
		BufferIO::Write<uint8_t>(p, proto);
		memcpy(p, buffer, len);
		last_sent = static_cast<uint16_t>(len + 3u);
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, uint8_t proto, const std::vector<uint8_t>& buffer) {
		auto* p = net_server_write;
		BufferIO::Write<uint16_t>(p, static_cast<uint16_t>(1u + buffer.size()));
		BufferIO::Write<uint8_t>(p, proto);
		memcpy(p, buffer.data(), buffer.size());
		last_sent = static_cast<uint16_t>(buffer.size() + 3u);
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
	static void SendCoreUtilsPacketToPlayer(DuelPlayer* dp, uint8_t proto, const CoreUtils::Packet& packet) {
		auto* p = net_server_write;
		BufferIO::Write<uint16_t>(p, static_cast<uint16_t>(sizeof(uint8_t) + packet.size()));
		BufferIO::Write<uint8_t>(p, proto);

		BufferIO::Write<uint8_t>(p, packet.message);
		memcpy(p, packet.data(), packet.buff_size());

		last_sent = static_cast<uint16_t>(packet.size() + sizeof(uint16_t) + sizeof(uint8_t));
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
