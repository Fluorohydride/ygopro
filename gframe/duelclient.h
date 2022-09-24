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

class HostResult {
public:
	unsigned int host;
	unsigned short port;
	bool isValid() {
		return host > 0 && port > 0;
	}
	HostResult() {
		host = 0;
		port = 0;
	}
};

#ifndef _WIN32
#include <resolv.h>
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
class RetrivedSRVRecord {
public:
	bool valid;
	unsigned short priority;
	unsigned short weight;
	unsigned short port;
	char host[100];
	RetrivedSRVRecord(ns_msg nsMsg, int i) {
		valid = false;
		ns_rr rr;
		if (ns_parserr(&nsMsg, ns_s_an, i, &rr) < 0 || ns_rr_type(rr) != T_SRV)
			return;
		priority = ns_get16(ns_rr_rdata(rr));
		weight   = ns_get16(ns_rr_rdata(rr) + NS_INT16SZ);
		port     = ns_get16(ns_rr_rdata(rr) + 2 * NS_INT16SZ);
		if (dn_expand(ns_msg_base(nsMsg), ns_msg_end(nsMsg), ns_rr_rdata(rr) + 3 * NS_INT16SZ, host, sizeof(host)) < 0)
			return;
		valid = true;
	}
};
#endif

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
	static bool is_swapping;
	static int select_hint;
	static int select_unselect_hint;
	static int last_select_hint;
	static char last_successful_msg[0x2000];
	static unsigned int last_successful_msg_length;
	static wchar_t event_string[256];
	static mt19937 rnd;
public:
	static bool StartClient(unsigned int ip, unsigned short port, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(char* data, unsigned int len);
	static int ClientAnalyze(char* msg, unsigned int len);
	static void SwapField();
	static void SetResponseI(int respI);
	static void SetResponseB(void* respB, unsigned char len);
	static void SendResponse();
	static unsigned int LookupHost(char *host);
	static bool LookupSRV(char *hostname, HostResult* result);
	static bool CheckHostnameSplitter(char* hostname, HostResult* result)
	static HostResult ParseHost(char *hostname, unsigned short port);
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
	static std::set<std::pair<unsigned int, unsigned short>> remotes;
public:
	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};
}

#endif //DUELCLIENT_H
