#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include "config.h"
#include <vector>
#include <set>
#include <utility>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

class DuelClient {
private:
	static unsigned int connect_state;
	static unsigned char response_buf[SIZE_RETURN_VALUE];
	static unsigned int response_len;
	static unsigned int watching;
	static bool is_host;
	static event_base* client_base;
	static bufferevent* client_bev;
	static unsigned char duel_client_read[SIZE_NETWORK_BUFFER];
	static int read_len;
	static unsigned char duel_client_write[SIZE_NETWORK_BUFFER];
	static bool is_closing;
	static bool is_swapping;
	static int select_hint;
	static int select_unselect_hint;
	static int last_select_hint;
	static unsigned char last_successful_msg[0x2000];
	static unsigned int last_successful_msg_length;
	static wchar_t event_string[256];
	static mt19937 rnd;
public:
	static unsigned char selftype;
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(unsigned char* data, unsigned int len);
	static int ClientAnalyze(unsigned char* msg, unsigned int len);
	static void SwapField();
	static void SetResponseI(int respI);
	static void SetResponseB(void* respB, unsigned int len);
	static void SendResponse();
	static void SendPacketToServer(unsigned char proto) {
		auto p = duel_client_write;
		BufferIO::WriteInt16(p, 1);
		BufferIO::WriteInt8(p, proto);
		bufferevent_write(client_bev, duel_client_write, 3);
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, ST& st) {
		auto p = duel_client_write;
		int blen = sizeof(ST);
		if (blen > MAX_DATA_SIZE)
			return;
		BufferIO::WriteInt16(p, (short)(1 + blen));
		BufferIO::WriteInt8(p, proto);
		std::memcpy(p, &st, blen);
		bufferevent_write(client_bev, duel_client_write, blen + 3);
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		auto p = duel_client_write;
		int blen = len;
		if (blen < 0)
			return;
		if (blen > MAX_DATA_SIZE)
			blen = MAX_DATA_SIZE;
		BufferIO::WriteInt16(p, (short)(1 + blen));
		BufferIO::WriteInt8(p, proto);
		std::memcpy(p, buffer, blen);
		bufferevent_write(client_bev, duel_client_write, blen + 3);
	}
	
protected:
	static bool is_refreshing;
	static int match_kill;
	static event* resp_event;
	static std::set<std::pair<unsigned int, unsigned short>> remotes;
public:
	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};

}

#endif //DUELCLIENT_H
