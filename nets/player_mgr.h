#ifndef PLAYER_MGR_H
#define PLAYER_MGR_H

#include "../common/common.h"
#include "../common/packet_handler.h"
#include <unordered_map>

namespace ygopro
{

	class Player;

	class PlayerMgr : public PacketStream {
	public:
		PlayerMgr();
		~PlayerMgr();

		void LoadPlayers();
		void LoadFriendships();

		Player* FindPlayer(std::string& name);
		Player* FindPlayer(int uid);
		Player* NewPlayer(unsigned int pass, std::string& name);
		void RemovePlayer(std::string& name);
		void RemovePlayer(int uid);
		int LoginPlayer(std::string& name, unsigned int pass, unsigned int version, Player*& pplayer);
		int LoginPlayer(int uid, unsigned int pass, unsigned int version, Player*& pplayer);
		void LogoutPlayer(Player* player);
		void KickPlayer(Player* player);

		virtual void SendPacket(unsigned char proto, void* buffer, size_t len);
		virtual void SendPacket(PacketWriter& pkt);

	protected:
		int max_uid;
		unsigned int status;
		std::unordered_map<int, Player*> uidPlayerMap;
		std::unordered_map<std::string, Player*> namePlayerMap;
	};

	extern PlayerMgr playerMgr;

}

#endif
