#ifndef NETSERVER_H
#define NETSERVER_H

#include "config.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"
#include <array>
#include <set>
#include <unordered_map>

#define NETPLAYER_TYPE_PLAYER1		0
#define NETPLAYER_TYPE_PLAYER2		1
#define NETPLAYER_TYPE_PLAYER3		2
#define NETPLAYER_TYPE_PLAYER4		3
#define NETPLAYER_TYPE_PLAYER5		4
#define NETPLAYER_TYPE_PLAYER6		5
#define NETPLAYER_TYPE_OBSERVER		7

namespace ygo {

class DuelMode;
	
struct DuelPlayer {
	unsigned short name[20];
	DuelMode* game;
	unsigned char type;
	unsigned char state;
	unsigned char pos;
	unsigned int uid;
	evbuffer* output;
	DuelPlayer() {
		uid = 0;
		pos = 0;
		game = 0;
		type = 0;
		state = 0;
		output = 0;
	}
};

class DuelMode {
public:
	DuelMode() {
		for(int i = 0; i < 6; ++i){
			players[i] = 0;
			ready[i] = false;
		}
		host_player = 0;
		pduel = 0;
	}
	void Start();
	void Process();
	void End();
	
public:
	DuelPlayer* players[6];
	bool ready[6];
	Deck pdeck[6];
	std::set<DuelPlayer*> observers;
	DuelPlayer* host_player;
	HostInfo host_info;
	unsigned long pduel;
};

class NetServer {
private:
	static std::unordered_map<bufferevent*, DuelPlayer> users;
	static event_base* net_evbase;
	static DuelMode* duel_mode;
	static char net_buffer_read[0x2000];
	static char net_buffer_write[0x2000];
public:
	static bool StartServer(unsigned short port);
	static void StopServer();
	static void ServerAccept(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx);
	static void ServerAcceptError(evconnlistener *listener, void* ctx);
	static void ServerEchoRead(bufferevent* bev, void* ctx);
	static void ServerEchoEvent(bufferevent* bev, short events, void* ctx);
	static int ServerThread(void* param);
	static void HandleCTOSPacket(DuelPlayer& dp, char* data, unsigned int len);
};

}

#endif //NETSERVER_H
