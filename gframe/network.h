#ifndef NETWORK_H
#define NETWORK_H

#include "config.h"
#include <set>
#include <vector>

namespace ygo {

struct HostInfo {
	unsigned short identifier;
	unsigned short version;
	unsigned int address;
	unsigned short port;
	wchar_t name[20];
	bool no_check_deck;
	bool no_shuffle_deck;
	bool no_shuffle_player;
	bool attack_ft;
	bool no_chain_hint;
	int start_lp;
	unsigned char start_hand;
	unsigned char draw_count;
	wchar_t lflist[20];
	bool is_match;
	unsigned char lfindex;
	unsigned char time_limit;
};

struct HostRequest {
	unsigned short identifier;
};

class NetManager {
public:
	unsigned int local_addr[8];
	unsigned short serv_port;
	unsigned int remote_addr;
	unsigned short remote_port;
	bool is_creating_host;
	SOCKET sBHost;
	SOCKET sBClient;
	SOCKET sListen;
	SOCKET sRemote;
	HostInfo hInfo;
	HostRequest hReq;
	std::vector<HostInfo> hosts;
	char* send_buffer_ptr;
	char send_buf[4096];
	char recv_buf[4096];

	bool CreateHost(int ipindex);
	bool CancelHost();
	bool RefreshHost(int ipindex);
	bool JoinHost();
	bool SendtoRemote(char* buf, int len);
	bool WaitClientResponse();

	inline static int ReadInt32(char*& p) {
		int ret = *(int*)p;
		p += 4;
		return ret;
	}
	inline static short ReadInt16(char*& p) {
		short ret = *(short*)p;
		p += 2;
		return ret;
	}
	inline static char ReadInt8(char*& p) {
		char ret = *(char*)p;
		p++;
		return ret;
	}
	inline static unsigned char ReadUInt8(char*& p) {
		unsigned char ret = *(unsigned char*)p;
		p++;
		return ret;
	}
	inline static void WriteInt32(char*& p, int val) {
		(*(int*)p) = val;
		p += 4;
	}
	inline static void WriteInt16(char*& p, short val) {
		(*(short*)p) = val;
		p += 2;
	}
	inline static void WriteInt8(char*& p, char val) {
		*p = val;
		p++;
	}

	int GetLocalAddress();
	static int BroadcastServer(void*);
	static int BroadcastClient(void*);
	static int ListenThread(void*);
	static int JoinThread(void*);

};

extern const unsigned short PROTO_VERSION;

}

#define NETWORK_SERVER_ID	0x7428
#define NETWORK_CLIENT_ID	0xdef6
#endif //NETWORK_H
