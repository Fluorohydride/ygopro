#ifndef NETSERVER_H
#define NETSERVER_H

#include "bufferio.h"
#include "network.h"

namespace ygo {

class NetServer {
private:
	static unsigned char net_server_write[SIZE_NETWORK_BUFFER];
	static size_t last_sent;

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
	static void ServerThread();
	static void DisconnectPlayer(DuelPlayer* dp);
	static void HandleCTOSPacket(DuelPlayer* dp, unsigned char* data, size_t len);
	static size_t CreateChatPacket(unsigned char* src, int src_size, unsigned char* dst, uint16_t dst_player_type);
	static inline bool ShouldHideFacedownCode(uint8_t position) {
		return (position & POS_FACEDOWN) != 0 && (position & POS_REVEAL) == 0;
	}
	// TODO: remove this function in the next protocol version, let the client handle the POS_REVEAL flag instead.
	static inline uint8_t StripRevealFlag(unsigned char* qbuf, size_t offset) {
		uint32_t info = 0;
		std::memcpy(&info, qbuf + offset, sizeof info);
		info &= ~(static_cast<uint32_t>(POS_REVEAL) << 24);
		std::memcpy(qbuf + offset, &info, sizeof info);
		return static_cast<uint8_t>(info >> 24);
	}
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto) {
		auto p = net_server_write;
		BufferIO::Write<uint16_t>(p, 1);
		BufferIO::Write<uint8_t>(p, proto);
		last_sent = 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, 3);
	}
	template<typename ST>
	static void SendPacketToPlayer(DuelPlayer* dp, unsigned char proto, const ST& st) {
		auto p = net_server_write;
		static_assert(sizeof(ST) <= MAX_DATA_SIZE, "Packet size is too large.");
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + sizeof(ST)));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, &st, sizeof(ST));
		last_sent = sizeof(ST) + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, sizeof(ST) + 3);
	}
	static void SendBufferToPlayer(DuelPlayer* dp, unsigned char proto, void* buffer, size_t len) {
		auto p = net_server_write;
		if (len > MAX_DATA_SIZE)
			len = MAX_DATA_SIZE;
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + len));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, buffer, len);
		last_sent = len + 3;
		if (dp)
			bufferevent_write(dp->bev, net_server_write, len + 3);
	}
	static void ReSendToPlayer(DuelPlayer* dp) {
		if(dp)
			bufferevent_write(dp->bev, net_server_write, last_sent);
	}
};

}

#endif //NETSERVER_H
