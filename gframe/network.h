#ifndef NETWORK_H
#define NETWORK_H

#include "config.h"
#include "deck_manager.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

namespace ygo {

struct HostInfo {
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
	unsigned short name[20];
	unsigned short pass[20];
};
struct CTOS_JoinGame {
	unsigned int gameid;
	unsigned short pass[20];
};
struct STOC_DeckError {
	unsigned char reason;
	unsigned int code;
};
struct STOC_CreateGame {
	unsigned int gameid;
};
struct STOC_JoinGame {
	HostInfo info;
	unsigned char type;
};
struct STOC_ExitGame {
	unsigned pos;
};
struct STOC_JoinFail {
	//0 - common error
	//1 - password incorrect
	//2 - host denied
	unsigned int reason;
};
struct STOC_HS_PlayerEnter {
	unsigned short name[20];
	unsigned char pos;
};
struct STOC_HS_PlayerChange {
	//pos<<4 | state
	unsigned char status;
};
struct STOC_HS_WatchChange {
	unsigned short watch_count;
};

class DuelMode;

struct DuelPlayer {
	unsigned short name[20];
	DuelMode* game;
	unsigned char type;
	unsigned char state;
	bufferevent* bev;
	DuelPlayer() {
		game = 0;
		type = 0;
		state = 0;
		bev = 0;
	}
};

class DuelMode {
public:
	DuelMode() {
		for(int i = 0; i < 6; ++i) {
			players[i] = 0;
			ready[i] = false;
		}
		host_player = 0;
		pduel = 0;
	}
	virtual ~DuelMode() {}
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) = 0;
	virtual void LeaveGame(DuelPlayer* dp) = 0;
	virtual void ToDuelist(DuelPlayer* dp) = 0;
	virtual void ToObserver(DuelPlayer* dp) = 0;
	virtual void PlayerReady(DuelPlayer* dp) = 0;
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos) = 0;
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata) = 0;
	virtual void StartDuel(DuelPlayer* dp) = 0;
	virtual void Process() = 0;
	virtual void EndDuel() = 0;

public:
	DuelPlayer* players[6];
	bool ready[6];
	Deck pdeck[6];
	std::set<DuelPlayer*> observers;
	DuelPlayer* host_player;
	HostInfo host_info;
	unsigned long pduel;
	wchar_t name[20];
	wchar_t pass[20];
};

}

#define NETWORK_SERVER_ID	0x7428
#define NETWORK_CLIENT_ID	0xdef6

#define NETPLAYER_TYPE_PLAYER1		0
#define NETPLAYER_TYPE_PLAYER2		1
#define NETPLAYER_TYPE_PLAYER3		2
#define NETPLAYER_TYPE_PLAYER4		3
#define NETPLAYER_TYPE_PLAYER5		4
#define NETPLAYER_TYPE_PLAYER6		5
#define NETPLAYER_TYPE_OBSERVER		7

#define CTOS_RESPONSE		0x1
#define CTOS_CHANGEDECK		0x2
#define CTOS_PLAYER_INFO	0x10
#define CTOS_CREATE_GAME	0x11
#define CTOS_JOIN_GAME		0x12
#define CTOS_EXIT_GAME		0x13
#define CTOS_HS_TODUELIST	0x20
#define CTOS_HS_TOOBSERVER	0x21
#define CTOS_HS_READY		0x22
#define CTOS_HS_KICK1		0x23
#define CTOS_HS_KICK2		0x24
#define CTOS_HS_START		0x25

#define STOC_GAME_MSG		0x1
#define STOC_DECK_ERROR		0x2
#define STOC_CREATE_GAME	0x11
#define STOC_JOIN_GAME		0x12
#define STOC_EXIT_GAME		0x13
#define STOC_JOIN_FAIL		0x14
#define STOC_GAME_START		0x15
#define STOC_HS_PLAYER_ENTER	0x20
#define STOC_HS_PLAYER_CHANGE	0x21
#define STOC_HS_WATCH_CHANGE	0x22

#define PLAYERCHANGE_READY		0x1
#define PLAYERCHANGE_NOTREADY	0x2
#define PLAYERCHANGE_LEAVE		0x3
#define PLAYERCHANGE_OBSERVE	0x4

#define MODE_SINGLE		0x0
#define MODE_MATCH		0x1
#endif //NETWORK_H
