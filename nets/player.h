#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "dbobject.h"
#include "file_receiver.h"
#include "../common/packet_handler.h"
#include <string>
#include <unordered_set>
#include <unordered_map>

#define PLAYER_STATUS_LOGIN 0x1
#define PLAYER_STATUS_BAN 0x2
#define PLAYER_STATUS_PUBLIC_F 0x10000

#define DUEL_STATE_PLAYER_1 0x0
#define DUEL_STATE_PLAYER_2 0x1
#define DUEL_STATE_PLAYER_3 0x2
#define DUEL_STATE_PLAYER_4 0x3
#define DUEL_STATE_OBSERVER 0xF
#define DUEL_STATE_FINGER1 0x10
#define DUEL_STATE_FINGER2 0x20
#define DUEL_STATE_FINGER3 0x30
#define DUEL_STATE_READY 0x80
#define DUEL_STATE_SIDE_READY 0x100

#define RELATION_NONE 0
#define RELATION_FRIEND 1
#define RELATION_FOLLOW 2
#define RELATION_PENDING 3

#define PUBLIC_STATUS_UNKNOWN
#define PUBLIC_STATUS_MENU
#define PUBLIC_STATUS_WAITING
#define PUBLIC_STATUS_DUELING
#define PUBLIC_STATUS_OVSERVING

#define PRIVILEGE_WATCH				0x1
#define PRIVILEGE_DUEL				0x2
#define PRIVILEGE_MATCHING			0x4
#define PRIVILEGE_AVATAR			0x8
#define PRIVILEGE_SLEEVE			0x10
#define PRIVILEGE_KICK_PLAYER		0x10000
#define PRIVILEGE_BAN_PLAYER		0x20000
#define PRIVILEGE_GIVE_PRIVILEGE	0x40000

class ClientConnection;

namespace ygopro
{	

	class DuelBase;
	class Deck;
	class Player;

	struct ContactCollection : public PacketStream  {
		ContactCollection(std::unordered_map<Player*, unsigned int>* p1, std::unordered_set<Player*>* p2) : pfriends(p1), pfollowers(p2) {}
		virtual void SendPacket(unsigned char proto, void* buffer, size_t len);
		virtual void SendPacket(PacketWriter& pkt);

		std::unordered_map<Player*, unsigned int>* pfriends;
		std::unordered_set<Player*>* pfollowers;
	};

	class Player: public FileReceiver, public DBObject, public PacketStream, public PacketObject {

	public:
		Player();
		~Player();

		void Login();
		void Logout();
		bool IsOnline();
		void SendPlayerInfo(Player* toplayer);
		bool CheckPrivilege(int priv);

		//contact
		void PublicStatusChange(int new_status);
		void SendFriendList();	
		void AddFriend(Player* player);
		void RemoveFriend(Player* player);
		bool IsFriend(Player* player);
		ContactCollection GetFriendCollection(bool isFriend);

		virtual void SendPacket(unsigned char proto, void* buffer, size_t len);
		virtual void SendPacket(PacketWriter& pkt);
		virtual void WriteToPacket(PacketWriter& _writer);
		virtual const char* GetCollection() { return "ygo.player"; }
		virtual bool FileReceiveEnd(bool save = true);

	public:
		int duel_count;
		int drop_count;
		int single_count;
		int single_win;
		int match_count;
		int match_win;
		int duel_point;
		int gold;
		int reg_time;
		int privilege;
		int status;
		int clan_id;
		int avatar;
		int sleeve;
		std::string name;
		std::string sign;
		std::string email;

		unsigned int password;
		unsigned int last_login_ip;
		unsigned int last_login_time;

		ClientConnection* connection;
		DuelBase* current_duel;
		unsigned int current_duel_state;
		Deck* current_deck;

		unsigned int public_status;
		std::unordered_map<Player*, unsigned int> friends;
		std::unordered_set<Player*> followers;

	};

}

#endif
