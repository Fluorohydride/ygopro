#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include <vector>
#include "bufferio.h"
#include "deck.h"
#include "network.h"

namespace ygo {

class DuelClient {
private:
	static bufferevent* client_bev;
	static unsigned char duel_client_write[SIZE_NETWORK_BUFFER];

public:
	static unsigned char selftype;
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent* bev, short events, void* ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(unsigned char* data, size_t len);
	static bool ClientAnalyze(unsigned char* msg, size_t len);
	static void SwapField();
	static void SetResponseI(int32_t respI);
	static void SetResponseB(void* respB, size_t len);
	static void SendResponse();
	static void SendUpdateDeck(const Deck& deck);
	static void SendPacketToServer(unsigned char proto) {
		auto p = duel_client_write;
		BufferIO::Write<uint16_t>(p, 1);
		BufferIO::Write<uint8_t>(p, proto);
		bufferevent_write(client_bev, duel_client_write, 3);
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, const ST& st) {
		auto p = duel_client_write;
		static_assert(sizeof(ST) <= MAX_DATA_SIZE, "Packet size is too large.");
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + sizeof(ST)));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, &st, sizeof(ST));
		bufferevent_write(client_bev, duel_client_write, sizeof(ST) + 3);
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		auto p = duel_client_write;
		if (len > MAX_DATA_SIZE)
			len = MAX_DATA_SIZE;
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + len));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, buffer, len);
		bufferevent_write(client_bev, duel_client_write, len + 3);
	}

	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};

}

#endif //DUELCLIENT_H
