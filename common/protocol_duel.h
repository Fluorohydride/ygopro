#ifndef _PROTOCOL_DUEL_H_
#define _PROTOCOL_DUEL_H_
//========================= server to client =============================

#define DUELSC_CONNECT_SUCCESS 0x1

//	repeated message
#define DUELSC_GAME_MSG 0x2

//========================= client to server =============================

//	unsigned int playerid;
#define DUELCS_IDENTIFY 0x1

//	repeated response
#define DUELCS_RESPONSE 0x2


#endif
