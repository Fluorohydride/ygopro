#ifndef NETWORK_H
#define NETWORK_H

#include "dllinterface.h"
#include "config.h"
#include "core_utils.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

namespace ygo {

struct ClientVersion {
	struct {
		uint8_t major;
		uint8_t minor;
	} client, core;
};
bool operator==(const ClientVersion& ver1, const ClientVersion& ver2);
bool operator!=(const ClientVersion& ver1, const ClientVersion& ver2);

struct HostInfo {
	uint32_t lflist;
	uint8_t rule;
	uint8_t mode;
	uint8_t duel_rule;
	bool no_check_deck;
	bool no_shuffle_deck;
	uint32_t start_lp;
	uint8_t start_hand;
	uint8_t draw_count;
	uint16_t time_limit;
	uint32_t : 32; //padding to account for the previous 64 bit value
	uint32_t handshake;
	ClientVersion version;
	int32_t team1;
	int32_t team2;
	int32_t best_of;
	uint32_t duel_flag;
	int32_t forbiddentypes;
	uint16_t extra_rules;
};
struct HostPacket {
	uint16_t identifier;
	uint16_t version;
	uint16_t port;
	uint32_t ipaddr;
	uint16_t name[20];
	HostInfo host;
};
struct HostRequest {
	uint16_t identifier;
};
struct CTOS_HandResult {
	uint8_t res;
};
struct CTOS_TPResult {
	uint8_t res;
};
struct CTOS_PlayerInfo {
	uint16_t name[20];
};
struct CTOS_CreateGame {
	HostInfo info;
	uint16_t name[20];
	uint16_t pass[20];
	char notes[200];
};

struct CTOS_JoinGame {
	uint16_t version;
	uint32_t gameid;
	uint16_t pass[20];
	ClientVersion version2;
};
struct CTOS_Kick {
	uint8_t pos;
};
struct CTOS_RematchResponse {
	uint8_t rematch;
};
enum ERROR_TYPE : uint8_t {
	JOINERROR = 0x1,
	DECKERROR,
	SIDEERROR,
	VERERROR,
	VERERROR2
};
struct DeckError {
	ERROR_TYPE etype = ERROR_TYPE::DECKERROR;
	enum DERR_TYPE : uint32_t {
		NONE,
		LFLIST,
		OCGONLY,
		TCGONLY,
		UNKNOWNCARD,
		CARDCOUNT,
		MAINCOUNT,
		EXTRACOUNT,
		SIDECOUNT,
		FORBTYPE,
		UNOFFICIALCARD,
		INVALIDSIZE
	};
	DERR_TYPE type = DERR_TYPE::NONE;
	struct {
		uint32_t current;
		uint32_t minimum;
		uint32_t maximum;
	} count;
	uint32_t code;
	DeckError(DERR_TYPE _type) :type(_type) {};
};
struct JoinError {
	ERROR_TYPE etype = ERROR_TYPE::JOINERROR;
	enum JERR_TYPE : uint32_t {
		JERR_UNABLE,
		JERR_PASSWORD,
		JERR_REFUSED
	};
	JERR_TYPE error;
	JoinError(JERR_TYPE type) :error(type) {};
};
struct VersionError {
	ERROR_TYPE etype = ERROR_TYPE::VERERROR2;
	char : 8; //padding to keep the client version in
	char : 8; //the same place as the other error codes
	char : 8;
	enum JERR_TYPE : uint32_t {
		JERR_UNABLE,
		JERR_PASSWORD,
		JERR_REFUSED
	};
	ClientVersion version;
	VersionError(ClientVersion _version) :version(_version) {};
};
struct STOC_ErrorMsg {
	ERROR_TYPE type;
	uint32_t code;
};
struct STOC_HandResult {
	uint8_t res1;
	uint8_t res2;
};
struct STOC_CreateGame {
	uint32_t gameid;
};
struct STOC_JoinGame {
	HostInfo info;
};
struct STOC_TypeChange {
	uint8_t type;
};
struct STOC_ExitGame {
	uint8_t pos;
};
struct STOC_TimeLimit {
	uint8_t player;
	uint16_t left_time;
};
struct STOC_Chat {
	uint16_t player;
	uint16_t msg[256];
};
struct STOC_HS_PlayerEnter {
	uint16_t name[20];
	uint8_t pos;
};
struct STOC_HS_PlayerChange {
	//pos<<4 | state
	uint8_t status;
};
struct STOC_HS_WatchChange {
	uint16_t watch_count;
};

class DuelMode;

struct DuelPlayer {
	uint16_t name[20];
	DuelMode* game;
	uint8_t type;
	uint8_t state;
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
	DuelMode(): host_player(0), pduel(0), duel_stage(0) {}
	virtual ~DuelMode() {}
	virtual void Chat(DuelPlayer* dp, void* pdata, int32_t len) {}
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {}
	virtual void LeaveGame(DuelPlayer* dp) {}
	virtual void ToDuelist(DuelPlayer* dp) {}
	virtual void ToObserver(DuelPlayer* dp) {}
	virtual void PlayerReady(DuelPlayer* dp, bool is_ready) {}
	virtual void PlayerKick(DuelPlayer* dp, uint8_t pos) {}
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata, uint32_t len) {}
	virtual void StartDuel(DuelPlayer* dp) {}
	virtual void HandResult(DuelPlayer* dp, uint8_t res) {}
	virtual void RematchResult(DuelPlayer* dp, uint8_t rematch) {}
	virtual void TPResult(DuelPlayer* dp, uint8_t tp) {}
	virtual void Process() {}
	virtual int32_t Analyze(CoreUtils::Packet packet) {
		return 0;
	}
	virtual void Surrender(DuelPlayer* dp) {}
	virtual void GetResponse(DuelPlayer* dp, void* pdata, uint32_t len) {}
	virtual void TimeConfirm(DuelPlayer* dp) {}
	virtual void EndDuel() {};

public:
	event* etimer;
	DuelPlayer* host_player;
	HostInfo host_info;
	int32_t duel_stage;
	OCG_Duel pduel;
	bool seeking_rematch;
	wchar_t name[20];
	wchar_t pass[20];
};

}

#define NETWORK_SERVER_ID	0x7428
#define NETWORK_CLIENT_ID	0xdef6

#define SERVER_HANDSHAKE 4043399681u

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

#define CTOS_REMATCH_RESPONSE 0xf0

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

#define STOC_CATCHUP		0xf0
#define STOC_REMATCH		0xf1
#define STOC_WAITING_REMATCH	0xf2

#define STOC_NEW_REPLAY			0x30

#define PLAYERCHANGE_OBSERVE	0x8
#define PLAYERCHANGE_READY		0x9
#define PLAYERCHANGE_NOTREADY	0xa
#define PLAYERCHANGE_LEAVE		0xb

#define ERRMSG_JOINERROR	0x1
#define ERRMSG_DECKERROR	0x2
#define ERRMSG_SIDEERROR	0x3
#define ERRMSG_VERERROR		0x4
#define ERRMSG_VERERROR2	0x5

#define MODE_SINGLE		0x0
#define MODE_MATCH		0x1
#define MODE_TAG		0x2
#define MODE_RELAY		0x3
#define MODE_ARBITRARY	0x4

#define SEALED_DUEL         0x1
#define BOOSTER_DUEL        0x2
#define DESTINY_DRAW        0x4
#define CONCENTRATION_DUEL  0x8
#define BOSS_DUEL           0x10
#define BATTLE_CITY         0x20
#define DUELIST_KINGDOM     0x40
#define DIMENSION_DUEL      0x80
#define TURBO_DUEL          0x100
#define DOUBLE_DECK         0x200
#define COMMAND_DUEL        0x400
#define DECK_MASTER         0x800
#define ACTION_DUEL         0x1000
#define DECK_LIMIT_20       0x2000

#define DUEL_STAGE_BEGIN		0
#define DUEL_STAGE_FINGER		1
#define DUEL_STAGE_FIRSTGO		2
#define DUEL_STAGE_DUELING		3
#define DUEL_STAGE_SIDING		4
#define DUEL_STAGE_END			5
#endif //NETWORK_H
