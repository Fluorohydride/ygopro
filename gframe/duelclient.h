#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include <vector>
#include "bufferio.h"
#include "deck.h"
#include "network.h"

namespace ygo {

#define CONNECT_STATE_NONE			0x0
#define CONNECT_STATE_CONNECTING	0x1
#define CONNECT_STATE_CONNECTED		0x2
#define CONNECT_STATE_JOINED		0x4

#define CLIENT_CLOSE_REASON_NONE	0
#define CLIENT_CLOSE_REASON_STOP	1
#define CLIENT_CLOSE_REASON_EXIT	2

class DuelClient {
private:
	static bufferevent* client_bev;
	static unsigned char duel_client_write[SIZE_NETWORK_BUFFER];
	static int WriteBufferEvent(bufferevent* bufev, const void* data, size_t size);

public:
	static unsigned char selftype;
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(EventSocket fd, short events, void* arg);
	static void StopClient(unsigned reason = CLIENT_CLOSE_REASON_STOP);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent* bev, short events, void* ctx);
	static void ClientThread();
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
		WriteBufferEvent(client_bev, duel_client_write, 3);
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, const ST& st) {
		auto p = duel_client_write;
		static_assert(sizeof(ST) <= MAX_DATA_SIZE, "Packet size is too large.");
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + sizeof(ST)));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, &st, sizeof(ST));
		WriteBufferEvent(client_bev, duel_client_write, sizeof(ST) + 3);
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		auto p = duel_client_write;
		if (len > MAX_DATA_SIZE)
			len = MAX_DATA_SIZE;
		BufferIO::Write<uint16_t>(p, (uint16_t)(1 + len));
		BufferIO::Write<uint8_t>(p, proto);
		std::memcpy(p, buffer, len);
		WriteBufferEvent(client_bev, duel_client_write, len + 3);
	}

	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(EventSocket fd, short events, void* arg);

	static unsigned int ResolveHostName(const char* hostname, const char* port);
};

}

#endif //DUELCLIENT_H
