#ifndef _PROTOCOL_DUEL_H_
#define _PROTOCOL_DUEL_H_
//========================= server to client =============================

#define DUELSC_CONNECT_SUCCESS 0x1

//	repeated message
#define DUELSC_GAME_MSG 0x2

//	unsigned int code
#define GSREP_REG_SUCCESS 0x80

//	unsigned int duelid
#define GSREP_CREATE_SUCCESS 0x81

//	unsigned int uid
#define GSREP_JOIN_SUCCESS 0x82

//========================= client to server =============================

//	unsigned int playerid;
#define DUELCS_IDENTIFY 0x1

//	repeated response
#define DUELCS_RESPONSE 0x2

// 
#define DUELCS_CHECKTIME 0x3

//	unsigned int code
#define GSCMD_SERVERREG 0x80

//	unsigned int duelid
//	unsigned int uid1
//	unsigned int uid2
//	unsigned int uid3
//	unsigned int uid4
//	unsigned int mode
//	unsigned int lp
//	unsigned int hand
//	unsigned int draw
//	unsigned int pool
//	unsigned int turntime
//	unsigned int maxtime
//	unsigned int option
//	unsigned int fdlist
#define GSCMD_CREATE_DUEL 0x81

//	unsigned int duelid
//	unsigned int uid
#define GSCMD_OBSERVER_JOIN 0x82

#define GSCMD_QUERY_STATUS 0x90
#define GSCMD_RELOAD_DATA 0x91
#define GSCMD_RESTART_GS 0x92

#endif
