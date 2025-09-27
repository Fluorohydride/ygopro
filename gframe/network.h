#ifndef NETWORK_H
#define NETWORK_H

#include <cstdint>
#include <cstring>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <type_traits>
#include "deck_manager.h"

#define check_trivially_copyable(T) static_assert(std::is_trivially_copyable<T>::value == true && std::is_standard_layout<T>::value == true, "not trivially copyable")

namespace ygo {

constexpr int SIZE_NETWORK_BUFFER = 0x20000;
constexpr int MAX_DATA_SIZE = UINT16_MAX - 1;

struct HostInfo {
	uint32_t lflist{};
	uint8_t rule{};
	uint8_t mode{};
	uint8_t duel_rule{};
	uint8_t no_check_deck{};
	uint8_t no_shuffle_deck{};
	// byte padding[3]

	int32_t start_lp{};
	uint8_t start_hand{};
	uint8_t draw_count{};
	uint16_t time_limit{};
};
check_trivially_copyable(HostInfo);
static_assert(sizeof(HostInfo) == 20, "size mismatch: HostInfo");

struct HostPacket {
	uint16_t identifier{};
	uint16_t version{};
	uint16_t port{};
	// byte padding[2]

	uint32_t ipaddr{};
	uint16_t name[20]{};
	HostInfo host;
};
check_trivially_copyable(HostPacket);
static_assert(sizeof(HostPacket) == 72, "size mismatch: HostPacket");

struct HostRequest {
	uint16_t identifier{};
};
check_trivially_copyable(HostRequest);
static_assert(sizeof(HostRequest) == 2, "size mismatch: HostRequest");

struct CTOS_DeckData {
	int32_t mainc{};
	int32_t sidec{};
	uint32_t list[MAINC_MAX + SIDEC_MAX]{};
};
check_trivially_copyable(CTOS_DeckData);

struct CTOS_HandResult {
	uint8_t res{};
};
check_trivially_copyable(CTOS_HandResult);
static_assert(sizeof(CTOS_HandResult) == 1, "size mismatch: CTOS_HandResult");

struct CTOS_TPResult {
	uint8_t res{};
};
check_trivially_copyable(CTOS_TPResult);
static_assert(sizeof(CTOS_TPResult) == 1, "size mismatch: CTOS_TPResult");

struct CTOS_PlayerInfo {
	uint16_t name[20]{};
};
check_trivially_copyable(CTOS_PlayerInfo);
static_assert(sizeof(CTOS_PlayerInfo) == 40, "size mismatch: CTOS_PlayerInfo");

struct CTOS_CreateGame {
	HostInfo info;
	uint16_t name[20]{};
	uint16_t pass[20]{};
};
check_trivially_copyable(CTOS_CreateGame);
static_assert(sizeof(CTOS_CreateGame) == 100, "size mismatch: CTOS_CreateGame");

struct CTOS_JoinGame {
	uint16_t version{};
	// byte padding[2]

	uint32_t gameid{};
	uint16_t pass[20]{};
};
check_trivially_copyable(CTOS_JoinGame);
static_assert(sizeof(CTOS_JoinGame) == 48, "size mismatch: CTOS_JoinGame");

struct CTOS_Kick {
	uint8_t pos{};
};
check_trivially_copyable(CTOS_Kick);
static_assert(sizeof(CTOS_Kick) == 1, "size mismatch: CTOS_Kick");

/*
* CTOS_ExternalAddress
* uint32_t real_ip; (IPv4 address, BE, alway 0 in normal client)
* uint16_t hostname[256]; (UTF-16 string)
*/

constexpr int LEN_HOSTNAME = 256;

// STOC
struct STOC_ErrorMsg {
	uint8_t msg{};
	// byte padding[3]

	uint32_t code{};
};
check_trivially_copyable(STOC_ErrorMsg);
static_assert(sizeof(STOC_ErrorMsg) == 8, "size mismatch: STOC_ErrorMsg");

struct STOC_HandResult {
	uint8_t res1{};
	uint8_t res2{};
};
check_trivially_copyable(STOC_HandResult);
static_assert(sizeof(STOC_HandResult) == 2, "size mismatch: STOC_HandResult");

// reserved for STOC_CREATE_GAME
struct STOC_CreateGame {
	uint32_t gameid{};
};
check_trivially_copyable(STOC_CreateGame);
static_assert(sizeof(STOC_CreateGame) == 4, "size mismatch: STOC_CreateGame");

struct STOC_JoinGame {
	HostInfo info;
};
check_trivially_copyable(STOC_JoinGame);
static_assert(sizeof(STOC_JoinGame) == 20, "size mismatch: STOC_JoinGame");

struct STOC_TypeChange {
	uint8_t type{};
};
check_trivially_copyable(STOC_TypeChange);
static_assert(sizeof(STOC_TypeChange) == 1, "size mismatch: STOC_TypeChange");

// reserved for STOC_LEAVE_GAME
struct STOC_ExitGame {
	uint8_t pos{};
};
check_trivially_copyable(STOC_ExitGame);
static_assert(sizeof(STOC_ExitGame) == 1, "size mismatch: STOC_ExitGame");

struct STOC_TimeLimit {
	uint8_t player{};
	// byte padding[1]

	uint16_t left_time{};
};
check_trivially_copyable(STOC_TimeLimit);
static_assert(sizeof(STOC_TimeLimit) == 4, "size mismatch: STOC_TimeLimit");

/*
* STOC_Chat
* uint16_t player_type;
* uint16_t msg[256]; (UTF-16 string)
*/
constexpr int LEN_CHAT_PLAYER = 1;
constexpr int LEN_CHAT_MSG = 256;
constexpr int SIZE_STOC_CHAT = (LEN_CHAT_PLAYER + LEN_CHAT_MSG) * sizeof(uint16_t);

struct STOC_HS_PlayerEnter {
	uint16_t name[20]{};
	uint8_t pos{};
	// byte padding[1]
};
check_trivially_copyable(STOC_HS_PlayerEnter);
//static_assert(sizeof(STOC_HS_PlayerEnter) == 42, "size mismatch: STOC_HS_PlayerEnter");
constexpr int STOC_HS_PlayerEnter_size = 41;	//workwround

struct STOC_HS_PlayerChange {
	//pos<<4 | state
	uint8_t status{};
};
check_trivially_copyable(STOC_HS_PlayerChange);
static_assert(sizeof(STOC_HS_PlayerChange) == 1, "size mismatch: STOC_HS_PlayerChange");

struct STOC_HS_WatchChange {
	uint16_t watch_count{};
};
check_trivially_copyable(STOC_HS_WatchChange);
static_assert(sizeof(STOC_HS_WatchChange) == 2, "size mismatch: STOC_HS_WatchChange");

class DuelMode;

struct DuelPlayer {
	uint16_t name[20]{};
	DuelMode* game{};
	uint8_t type{};
	uint8_t state{};
	bufferevent* bev{};
};

inline unsigned int GetPosition(unsigned char* qbuf, size_t offset) {
	unsigned int info = 0;
	std::memcpy(&info, qbuf + offset, sizeof info);
	return info >> 24;
}

class DuelMode {
public:
	DuelMode() = default;
	virtual ~DuelMode() = default;
	virtual void Chat(DuelPlayer* dp, unsigned char* pdata, int len) = 0;
	virtual void JoinGame(DuelPlayer* dp, unsigned char* pdata, bool is_creater) = 0;
	virtual void LeaveGame(DuelPlayer* dp) = 0;
	virtual void ToDuelist(DuelPlayer* dp) = 0;
	virtual void ToObserver(DuelPlayer* dp) = 0;
	virtual void PlayerReady(DuelPlayer* dp, bool is_ready) = 0;
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos) = 0;
	virtual void UpdateDeck(DuelPlayer* dp, unsigned char* pdata, int len) = 0;
	virtual void StartDuel(DuelPlayer* dp) = 0;
	virtual void HandResult(DuelPlayer* dp, unsigned char res) = 0;
	virtual void TPResult(DuelPlayer* dp, unsigned char tp) = 0;
	virtual void Process() = 0;
	virtual int Analyze(unsigned char* msgbuffer, unsigned int len) = 0;
	virtual void Surrender(DuelPlayer* dp) = 0;
	virtual void GetResponse(DuelPlayer* dp, unsigned char* pdata, unsigned int len) = 0;
	virtual void TimeConfirm(DuelPlayer* dp) = 0;
	virtual void EndDuel() = 0;

public:
	event* etimer { nullptr };
	DuelPlayer* host_player{ nullptr };
	HostInfo host_info;
	int duel_stage{};
	intptr_t pduel{};
	wchar_t name[20]{};
	wchar_t pass[20]{};
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

#define CTOS_RESPONSE		0x1		// byte array
#define CTOS_UPDATE_DECK	0x2		// CTOS_DeckData
#define CTOS_HAND_RESULT	0x3		// CTOS_HandResult
#define CTOS_TP_RESULT		0x4		// CTOS_TPResult
#define CTOS_PLAYER_INFO	0x10	// CTOS_PlayerInfo
#define CTOS_CREATE_GAME	0x11	// CTOS_CreateGame
#define CTOS_JOIN_GAME		0x12	// CTOS_JoinGame
#define CTOS_LEAVE_GAME		0x13	// no data
#define CTOS_SURRENDER		0x14	// no data
#define CTOS_TIME_CONFIRM	0x15	// no data
#define CTOS_CHAT			0x16	// uint16_t array
#define CTOS_EXTERNAL_ADDRESS	0x17	// CTOS_ExternalAddress
#define CTOS_HS_TODUELIST	0x20	// no data
#define CTOS_HS_TOOBSERVER	0x21	// no data
#define CTOS_HS_READY		0x22	// no data
#define CTOS_HS_NOTREADY	0x23	// no data
#define CTOS_HS_KICK		0x24	// CTOS_Kick
#define CTOS_HS_START		0x25	// no data
#define CTOS_REQUEST_FIELD	0x30

#define STOC_GAME_MSG		0x1		// byte array
#define STOC_ERROR_MSG		0x2		// STOC_ErrorMsg
#define STOC_SELECT_HAND	0x3		// no data
#define STOC_SELECT_TP		0x4		// no data
#define STOC_HAND_RESULT	0x5		// STOC_HandResult
#define STOC_TP_RESULT		0x6		// reserved
#define STOC_CHANGE_SIDE	0x7		// no data
#define STOC_WAITING_SIDE	0x8		// no data
#define STOC_DECK_COUNT		0x9		// int16_t[6]
#define STOC_CREATE_GAME	0x11	// reserved
#define STOC_JOIN_GAME		0x12	// STOC_JoinGame
#define STOC_TYPE_CHANGE	0x13	// STOC_TypeChange
#define STOC_LEAVE_GAME		0x14	// reserved
#define STOC_DUEL_START		0x15	// no data
#define STOC_DUEL_END		0x16	// no data
#define STOC_REPLAY			0x17	// ExtendedReplayHeader + byte array
#define STOC_TIME_LIMIT		0x18	// STOC_TimeLimit
#define STOC_CHAT			0x19	// uint16_t + uint16_t array
#define STOC_HS_PLAYER_ENTER	0x20	// STOC_HS_PlayerEnter
#define STOC_HS_PLAYER_CHANGE	0x21	// STOC_HS_PlayerChange
#define STOC_HS_WATCH_CHANGE	0x22	// STOC_HS_WatchChange
#define STOC_TEAMMATE_SURRENDER	0x23	// no data
#define STOC_FIELD_FINISH		0x30

// STOC_GAME_MSG header
#define MSG_WAITING				3
#define MSG_START				4
#define MSG_UPDATE_DATA			6	// flag=0: clear
#define MSG_UPDATE_CARD			7	// flag=QUERY_CODE, code=0: clear
#define MSG_REQUEST_DECK		8
#define MSG_REFRESH_DECK		34
#define MSG_CARD_SELECTED		80
#define MSG_UNEQUIP				95
#define MSG_BE_CHAIN_TARGET		121
#define MSG_CREATE_RELATION		122
#define MSG_RELEASE_RELATION	123

#define PLAYERCHANGE_OBSERVE	0x8
#define PLAYERCHANGE_READY		0x9
#define PLAYERCHANGE_NOTREADY	0xa
#define PLAYERCHANGE_LEAVE		0xb

#define ERRMSG_JOINERROR	0x1
#define ERRMSG_DECKERROR	0x2
#define ERRMSG_SIDEERROR	0x3
#define ERRMSG_VERERROR		0x4

#define DECKERROR_LFLIST		0x1U
#define DECKERROR_OCGONLY		0x2U
#define DECKERROR_TCGONLY		0x3U
#define DECKERROR_UNKNOWNCARD	0x4U
#define DECKERROR_CARDCOUNT		0x5U
#define DECKERROR_MAINCOUNT		0x6U
#define DECKERROR_EXTRACOUNT	0x7U
#define DECKERROR_SIDECOUNT		0x8U
#define DECKERROR_NOTAVAIL		0x9U

#define MODE_SINGLE		0x0
#define MODE_MATCH		0x1
#define MODE_TAG		0x2

#define DUEL_STAGE_BEGIN		0
#define DUEL_STAGE_FINGER		1
#define DUEL_STAGE_FIRSTGO		2
#define DUEL_STAGE_DUELING		3
#define DUEL_STAGE_SIDING		4
#define DUEL_STAGE_END			5
#endif //NETWORK_H
