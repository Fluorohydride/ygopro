#ifndef NETWORK_H
#define NETWORK_H

#include "config.h"
#include <event2/event.h>
#include <set>
#include <vector>

namespace ygo {

struct HostInfo {
	unsigned short name[20];
	unsigned char rule;
	unsigned char mode;
	unsigned int lflist;
	bool no_check_deck;
	bool no_shuffle_deck;
	unsigned int start_lp;
	unsigned char start_hand;
	unsigned char draw_count;
};

struct HostPacket {
	unsigned short identifier;
	unsigned short version;
	unsigned int address;
	unsigned short port;
	HostInfo host;
};

struct HostRequest {
	unsigned short identifier;
};

struct CTOS_Response {
};
struct CTOS_ChangeDeck {
};
struct CTOS_PlayerInfo {
	unsigned short name[20];
};
struct CTOS_CreateGame {
	HostInfo info;
};
struct CTOS_JoinGame {
	unsigned int gameid;
};
struct CTOS_ExitGame {
};
struct CTOS_HS_Command {
	unsigned char cmd;
};

struct STOC_CreateGame {
	unsigned int gameid;
};
struct STOC_JoinGame {
};
struct STOC_ExitGame {
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

#define CTOS_RESPONSE		0x1
#define CTOS_CHANGEDECK		0x2
#define CTOS_PLAYER_INFO	0x10
#define CTOS_CREATE_GAME	0x11
#define CTOS_JOIN_GAME		0x12
#define CTOS_EXIT_GAME		0x13
#define CTOS_HS_COMMAND		0x14
#define HS_COMMAND_TODUELIST	0x1
#define HS_COMMAND_TOOBSERVER	0x2
#define HS_COMMAND_READY		0x3
#define HS_COMMAND_KICK1		0x4
#define HS_COMMAND_KICK2		0x5
#define HS_COMMAND_START		0x6

#define STOC_CREATE_GAME	0x11
#define STOC_JION_GAME		0x12
#define STOC_EXIT_GAME		0x13
#define STOC_

#define MODE_SINGLE		0x1
#define MODE_MATCH		0x2
#endif //NETWORK_H
