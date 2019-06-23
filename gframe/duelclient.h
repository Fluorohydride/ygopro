#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include "config.h"
#include <vector>
#include <set>
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
	static unsigned char response_buf[64];
	static unsigned char response_len;
	static unsigned int watching;
	static unsigned char selftype;
	static bool is_host;
	static event_base* client_base;
	static bufferevent* client_bev;
	static char duel_client_read[0x2000];
	static char duel_client_write[0x2000];
	static bool is_closing;
	static int select_hint;
	static int select_unselect_hint;
	static int last_select_hint;
	static char last_successful_msg[2048];
	static unsigned int last_successful_msg_length;
	static wchar_t event_string[256];
	static mtrandom rnd;
public:
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ctx);
	static int ClientThread(void* param);
	static void HandleSTOCPacketLan(char* data, unsigned int len);
	static int ClientAnalyze(char* msg, unsigned int len);
	static void SetResponseI(int respI);
	static void SetResponseB(void* respB, unsigned char len);
	static void SendResponse();
	static void SendPacketToServer(unsigned char proto) {
		char* p = duel_client_write;
		BufferIO::WriteInt16(p, 1);
		BufferIO::WriteInt8(p, proto);
		bufferevent_write(client_bev, duel_client_write, 3);
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, ST& st) {
		char* p = duel_client_write;
		BufferIO::WriteInt16(p, 1 + sizeof(ST));
		BufferIO::WriteInt8(p, proto);
		memcpy(p, &st, sizeof(ST));
		bufferevent_write(client_bev, duel_client_write, sizeof(ST) + 3);
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		char* p = duel_client_write;
		BufferIO::WriteInt16(p, 1 + len);
		BufferIO::WriteInt8(p, proto);
		memcpy(p, buffer, len);
		bufferevent_write(client_bev, duel_client_write, len + 3);
	}
	
protected:
	static bool is_refreshing;
	static int match_kill;
	static event* resp_event;
	static std::set<unsigned int> remotes;
public:
	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(void* arg);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};

}

#endif //DUELCLIENT_H
