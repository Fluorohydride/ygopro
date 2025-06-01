#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include <vector>
#include <set>
#include "network.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

class DuelClient {
private:
	static unsigned int connect_state;
	static unsigned char response_buf[SIZE_RETURN_VALUE];
	static size_t response_len;
	static unsigned int watching;
	static bool is_host;
	static event_base* client_base;
	static bufferevent* client_bev;
	static unsigned char duel_client_write[SIZE_NETWORK_BUFFER];
	static bool is_closing;
	static bool is_swapping;
	static int select_hint;
	static int select_unselect_hint;
	static int last_select_hint;
	static unsigned char last_successful_msg[SIZE_NETWORK_BUFFER];
	static size_t last_successful_msg_length;
	static wchar_t event_string[256];
	static std::mt19937 rnd;
	static std::uniform_real_distribution<float> real_dist;
	static bool is_refreshing;
	static int match_kill;
	static event* resp_event;
	static std::set<std::pair<unsigned int, unsigned short>> remotes;

public:
	static unsigned char selftype;
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent* bev, short events, void* ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(unsigned char* data, int len);
	static bool ClientAnalyze(unsigned char* msg, int len);
	static void SwapField();
	static void SetResponseI(int32_t respI);
	static void SetResponseB(void* respB, size_t len);
	static void SendResponse();
	static void SendPacketToServer(unsigned char proto) {
		auto p = duel_client_write;
		buffer_write<uint16_t>(p, 1);
		buffer_write<uint8_t>(p, proto);
		bufferevent_write(client_bev, duel_client_write, 3);
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, const ST& st) {
		auto p = duel_client_write;
		if (sizeof(ST) > MAX_DATA_SIZE)
			return;
		buffer_write<uint16_t>(p, (uint16_t)(1 + sizeof(ST)));
		buffer_write<uint8_t>(p, proto);
		std::memcpy(p, &st, sizeof(ST));
		bufferevent_write(client_bev, duel_client_write, sizeof(ST) + 3);
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		auto p = duel_client_write;
		if (len > MAX_DATA_SIZE)
			len = MAX_DATA_SIZE;
		buffer_write<uint16_t>(p, (uint16_t)(1 + len));
		buffer_write<uint8_t>(p, proto);
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
