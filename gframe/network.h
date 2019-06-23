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
	unsigned int lflist;
	unsigned char rule;
	unsigned char mode;
	unsigned char duel_rule;
	bool no_check_deck;
	bool no_shuffle_deck;
	unsigned int start_lp;
	unsigned char start_hand;
	unsigned char draw_count;
	unsigned short time_limit;
};
struct HostPacket {
	unsigned short identifier;
	unsigned short version;
	unsigned short port;
	unsigned int ipaddr;
	unsigned short name[20];
	HostInfo host;
};
struct HostRequest {
	unsigned short identifier;
};
struct CTOS_HandResult {
	unsigned char res;
};
struct CTOS_TPResult {
	unsigned char res;
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
	unsigned short version;
	unsigned int gameid;
	unsigned short pass[20];
};
struct CTOS_Kick {
	unsigned char pos;
};
struct STOC_ErrorMsg {
	unsigned char msg;
	unsigned int code;
};
struct STOC_HandResult {
	unsigned char res1;
	unsigned char res2;
};
struct STOC_CreateGame {
	unsigned int gameid;
};
struct STOC_JoinGame {
	HostInfo info;
};
struct STOC_TypeChange {
	unsigned char type;
};
struct STOC_ExitGame {
	unsigned char pos;
};
struct STOC_TimeLimit {
	unsigned char player;
	unsigned short left_time;
};
struct STOC_Chat {
	unsigned short player;
	unsigned short msg[256];
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
	DuelMode(): host_player(0), pduel(0) {}
	virtual ~DuelMode() {}
	virtual void Chat(DuelPlayer* dp, void* pdata, int len) {}
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {}
	virtual void LeaveGame(DuelPlayer* dp) {}
	virtual void ToDuelist(DuelPlayer* dp) {}
	virtual void ToObserver(DuelPlayer* dp) {}
	virtual void PlayerReady(DuelPlayer* dp, bool is_ready) {}
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos) {}
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata, unsigned int len) {}
	virtual void StartDuel(DuelPlayer* dp) {}
	virtual void HandResult(DuelPlayer* dp, unsigned char res) {}
	virtual void TPResult(DuelPlayer* dp, unsigned char tp) {}
	virtual void Process() {}
	virtual int Analyze(char* msgbuffer, unsigned int len) {
		return 0;
	}
	virtual void Surrender(DuelPlayer* dp) {}
	virtual void GetResponse(DuelPlayer* dp, void* pdata, unsigned int len) {}
	virtual void TimeConfirm(DuelPlayer* dp) {}
	virtual void EndDuel() {};

public:
	event* etimer;
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
#define CTOS_UPDATE_DECK	0x2
#define CTOS_HAND_RESULT	0x3
#define CTOS_TP_RESULT		0x4
#define CTOS_PLAYER_INFO	0x10
#define CTOS_CREATE_GAME	0x11
#define CTOS_JOIN_GAME		0x12
#define CTOS_LEAVE_GAME		0x13
#define CTOS_SURRENDER		0x14
#define CTOS_TIME_CONFIRM	0x15
#define CTOS_CHAT			0x16
#define CTOS_HS_TODUELIST	0x20
#define CTOS_HS_TOOBSERVER	0x21
#define CTOS_HS_READY		0x22
#define CTOS_HS_NOTREADY	0x23
#define CTOS_HS_KICK		0x24
#define CTOS_HS_START		0x25

#define STOC_GAME_MSG		0x1
#define STOC_ERROR_MSG		0x2
#define STOC_SELECT_HAND	0x3
#define STOC_SELECT_TP		0x4
#define STOC_HAND_RESULT	0x5
#define STOC_TP_RESULT		0x6
#define STOC_CHANGE_SIDE	0x7
#define STOC_WAITING_SIDE	0x8
#define STOC_CREATE_GAME	0x11
#define STOC_JOIN_GAME		0x12
#define STOC_TYPE_CHANGE	0x13
#define STOC_LEAVE_GAME		0x14
#define STOC_DUEL_START		0x15
#define STOC_DUEL_END		0x16
#define STOC_REPLAY			0x17
#define STOC_TIME_LIMIT		0x18
#define STOC_CHAT			0x19
#define STOC_HS_PLAYER_ENTER	0x20
#define STOC_HS_PLAYER_CHANGE	0x21
#define STOC_HS_WATCH_CHANGE	0x22

#define PLAYERCHANGE_OBSERVE	0x8
#define PLAYERCHANGE_READY		0x9
#define PLAYERCHANGE_NOTREADY	0xa
#define PLAYERCHANGE_LEAVE		0xb

#define ERRMSG_JOINERROR	0x1
#define ERRMSG_DECKERROR	0x2
#define ERRMSG_SIDEERROR	0x3
#define ERRMSG_VERERROR		0x4

#define DECKERROR_LFLIST		0x1
#define DECKERROR_OCGONLY		0x2
#define DECKERROR_TCGONLY		0x3
#define DECKERROR_UNKNOWNCARD	0x4
#define DECKERROR_CARDCOUNT		0x5
#define DECKERROR_MAINCOUNT		0x6
#define DECKERROR_EXTRACOUNT	0x7
#define DECKERROR_SIDECOUNT		0x8

#define MODE_SINGLE		0x0
#define MODE_MATCH		0x1
#define MODE_TAG		0x2
#endif //NETWORK_H
