#ifndef _PROTOCOLS_H_
#define _PROTOCOLS_H_
//========================= server to client =============================

//	unsigned int response_seed;
//	unsigned int server_state;
#define STOC_CONNECT_SUCCESS 0x1

#define SS_REGISTER_DISABLED 0x1

//	unsigned int reason;
#define STOC_REQUEST_SUCCESS 0x2

#define RS_REGISTER_SUCCESS 1

//	unsigned int reason;
#define STOC_REQUEST_FAIL 0x3

#define RF_SERVER_MAINTAINANCE		1
#define RF_LOGIN_INVALID_ACC		10
#define RF_LOGIN_INVALID_PASS		11
#define RF_LOGIN_ACC_BANED			12
#define RF_LOGIN_ALREADY_LOGIN		13
#define RF_LOGIN_VERSION_ERR		14
#define RF_REGISTER_FORBIDDEN		20
#define RF_REGISTER_ACC_EXISTS		21
#define RF_REGISTER_INVALID_ACC		22
#define RF_REGISTER_INVALID_PASS	23

//	unsigned int uid;
//	unsigned int last_login_ip;
//	unsigned int last_login_time;
#define STOC_LOGIN_SUCCESS 0x4

//	unsigned int msg_type
//	string msg
#define STOC_SYS_MESSAGE 0x5

//	unsigned int msg_type
//	unsigned int playerid
//	string name
//	string msg
#define STOC_PLAYER_MESSAGE 0x6

//	unsigned int msg_id
//	repeated params
#define STOC_LOCAL_MESSAGE 0x7

#define MSGTYPE_ADMIN 1
#define MSGTYPE_SYSMSG 2
#define MSGTYPE_LOBBY 3
#define MSGTYPE_CLAN 4
#define MSGTYPE_PRIVATE 10
#define MSGTYPE_DUEL_PLAYER 11
#define MSGTYPE_DUEL_OBSERVER 12

//	unsigned int resource_id;
//	unsigned int file_size;
#define STOC_FILE_TRANSFER_START 0x10

//	unsigned char data[];
#define STOC_FILE_CONTENT 0x11

//	unsigned int resource_id;
#define STOC_FILE_CONFIRM 0x12

//	unsigned int resource_id;
#define STOC_FILE_FETCH_FAIL 0x13

//	unsigned int left_time
//	unsigned int waiting_seed
#define STOC_WAITING_RESPONSE 0x1a

//	int uid;
//	int duel_count;
//	int drop_count;
//	int single_count;
//	int single_win;
//	int match_count;
//	int match_win;
//	int duel_point;
//	int gold;
//	int reg_time;
//	int privilege;
//	int status;
//	int avatar;
//	int sleeve;
//	string name;
//	string sign;
//	string email;
#define STOC_PLAYER_INFO 0x20

//	unsigned int flag;
//	misc datas;
#define STOC_PLAYER_INFO_CHANGE 0x21

#define PIC_DUEL_COUNT		0x1
#define PIC_DROP_COUNT		0x2
#define PIC_SINGLE_COUNT	0x4
#define PIC_SINGLE_WIN		0x8
#define PIC_MATCH_COUNT		0x10
#define PIC_MATCH_WIN		0x20
#define PIC_DUEL_POINT		0x40
#define PIC_GOLD			0x80
#define PIC_REG_TIME		0x100
#define PIC_PRIVILEGE		0x200
#define PIC_STATUS			0x400
#define PIC_CLANID			0x800
#define PIC_AVATAR			0x1000
#define PIC_SLEEVE			0x2000
#define PIC_NAME			0x4000
#define PIC_SIGN			0x8000
#define PIC_EMAIL			0x1000

//	repeated
//	>> int uid
//	>> string name
#define STOC_PLAYER_NAME 0x22

//	repeated
//	>> int uid
//	>> string name
#define STOC_CLAN_NAME 0x23

//	repeated list
//	>> unsigned int playerid
//	>> string name
//	>> unsigned int relation
//	>> unsigned int public_status
#define STOC_FRIEND_LIST 0x40

//	unsigned int playerid
//	string name
//	unsigned int relation
#define STOC_FRIEND_NEW 0x41

//	unsigned int playerid
//	unsigned int relation
#define STOC_FRIEND_ALTER 0x42

//	unsigned int playerid
//	unsigned int public_status
#define STOC_FRIEND_STATUS 0x43

#define STOC_DUEL_ENTER
#define STOC_DUEL_EXIT

#define STOC_DUEL_PLAYER_ENTER
#define STOC_DUEL_PLAYER_LEAVE
#define STOC_DUEL_OBSERVER_ENTER
#define STOC_DUEL_OBSERVER_LEAVE

//========================= client to server =============================

//	unsigned int version;
//	string name;
//	unsigned int pass;
#define CTOS_LOGIN_NAME 0x1

//	unsigned int version;
//	unsigned int uid;
//	unsigned int pass;
#define CTOS_LOGIN_UID 0x2

//	string name;
//	unsigned int pass;
#define CTOS_REGISTER 0x4

//	unsigned int msg_type
//	unsigned int msg_id
//	string msg
#define CTOS_MESSAGE 0x5

//	unsigned int reason;
//	unsigned int file_size;
#define CTOS_FILE_TRANSFER_START 0x10

#define CTOS_FTREASON_AVATAR 0x1
#define CTOS_FTREASON_SLEEVE 0x2

//	char data[];
#define CTOS_FILE_CONTENT 0x11

//	unsigned int resourceid
#define CTOS_FILE_FETCH 0x12

//	unsigned int waiting_seed
#define CTOS_WAITING_CONFIRM 0x1a

//	unsigned int waiting_seed
//	char len
//	char data[]
#define CTOS_DUEL_RESPONSE 0x1b

//	unsigned int uid;
#define CTOS_QUERY_PLAYER 0x20

//	repeated
//	>> unsigned int uid;
#define CTOS_QUERY_PLAYER_NAME 0x21

//	repeated
//	>> unsigned int uid;
#define CTOS_QUERY_CLAN_NAME 0x22

//	unsigned int playerid
#define CTOS_ADD_FRIEND 0x40

//	unsigned int playerid
#define CTOS_REMOVE_FRIEND 0x41

#endif
