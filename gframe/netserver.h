#ifndef NETSERVER_H
#define NETSERVER_H

#include "config.h"
#include <event2/event.h>
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
	DuelMode* game;
	unsigned char type;
	unsigned char state;
	unsigned char pos;
	unsigned int uid;
	DuelPlayer() {
		uid = 0;
		pos = 0;
		game = 0;
		type = 0;
		state = 0;
	}
};

class DuelMode {
private:
	std::array<DuelPlayer*, 6> players;
	std::vector<DuelPlayer*> observers;
	bool ready[6];
	unsigned int host_uid;
	unsigned long pduel;
public:
	DuelMode() {
		for(int i = 0; i < 6; ++i){
			players[i] = 0;
			ready[i] = false;
		}
		for(int i = 0; i < 8; ++i)
			observers[i] = 0;
		host_uid = 0;
		pduel = 0;
	}
	void Start();
	void Process();
	void End();
};

class NetServer {
private:
	static std::unordered_map<int, DuelPlayer> users;
	static bool server_started;
	static unsigned int uid;
public:
	static bool StartServer(unsigned short port);
};

}

#endif //NETSERVER_H
