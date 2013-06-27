#include "player.h"
#include "player_mgr.h"
#include "clan.h"
#include "clan_mgr.h"
#include "dbadapter.h"
#include "../common/tcpserver.h"
#include <algorithm>

namespace ygopro
{

	PlayerMgr playerMgr;

	PlayerMgr::PlayerMgr() {
		status = 0;
	}

	PlayerMgr::~PlayerMgr() {
		for(auto &iter : uidPlayerMap) 
			delete iter.second;
	}

	void PlayerMgr::LoadPlayers() {
		MongoQuery mq = MongoExecutor(dbAdapter).orderby("_id", 1).queryFrom("ygo.player");
		max_uid = 0;
		while(mq++) {
			Player* new_player = new Player;
			mq("_id", new_player->uid)
				("duel_count", new_player->duel_count)
				("drop_count", new_player->drop_count)
				("single_count", new_player->single_count)
				("single_win", new_player->single_win)
				("match_count", new_player->match_count)
				("match_win", new_player->match_win)
				("duel_point", new_player->duel_point)
				("gold", new_player->gold)
				("reg_time", new_player->reg_time)
				("privilege", new_player->privilege)
				("status", new_player->status)
				("clan_id", new_player->clan_id)
				("avatar", new_player->avatar)
				("sleeve", new_player->sleeve)
				("name", new_player->name)
				("sign", new_player->sign)
				("email", new_player->email)
				("password", new_player->password)
				("last_login_ip", new_player->last_login_ip)
				("last_login_time", new_player->last_login_time);
			uidPlayerMap[new_player->uid] = new_player;
			std::string namekey = new_player->name;
			std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
			namePlayerMap[namekey] = new_player;
			max_uid = new_player->uid;
			if(new_player->clan_id) {
				Clan* pclan = static_cast<Clan*>(clanMgr.FindClan(new_player->clan_id));
				if(pclan)
					pclan->members.insert(new_player);
				else
					new_player->clan_id = 0;
			}
		}
	}

	void PlayerMgr::LoadFriendships() {
		MongoQuery mq = MongoExecutor(dbAdapter).orderby("player1", 1).queryFrom("ygo.friend");
		int player1, player2, current = 0, relation = 0;
		Player* pcurrent = nullptr;
		while(mq++) {
			mq("player1", player1)("player2", player2)("relation", relation);
			if(player1 != current) {
				current = player1;
				pcurrent = playerMgr.FindPlayer(player1);
			}
			if(pcurrent) {
				Player* ptarget = playerMgr.FindPlayer(player2);
				if(ptarget)
					pcurrent->friends[ptarget] = relation;
			}
		}
	}

	Player* PlayerMgr::FindPlayer(std::string& name) {
		std::string namekey = name;
		std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
		auto iter = namePlayerMap.find(namekey);
		if(iter == namePlayerMap.end())
			return nullptr;
		return iter->second;
	}

	Player* PlayerMgr::FindPlayer(int uid) {
		auto iter = uidPlayerMap.find(uid);
		if(iter == uidPlayerMap.end())
			return nullptr;
		return iter->second;
	}

	Player* PlayerMgr::NewPlayer(unsigned int pass, std::string& name) {
		std::string namekey = name;
		std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
		auto iter = namePlayerMap.find(namekey);
		if(iter != namePlayerMap.end())
			return nullptr;
		max_uid++;
		Player* new_player = new Player;
		new_player->uid = max_uid;
		new_player->name = name;
		new_player->password = pass;
		new_player->reg_time = (int)time(0);
		new_player->DBNewObject("name", name, "password", pass, "reg_time", new_player->reg_time);
		uidPlayerMap[new_player->uid] = new_player;
		namePlayerMap[namekey] = new_player;
		return new_player;
	}

	void PlayerMgr::RemovePlayer(std::string& name) {
		std::string namekey = name;
		std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
		auto iter = namePlayerMap.find(namekey);
		if(iter == namePlayerMap.end())
			return;
		Player* player = iter->second;
		player->DBDeleteObject();
		uidPlayerMap.erase(player->uid);
		namePlayerMap.erase(namekey);
		delete player;
	}

	void PlayerMgr::RemovePlayer(int uid) {
		Player* player = FindPlayer(uid);
		if(player) {
			player->DBDeleteObject();
			uidPlayerMap.erase(player->uid);
			namePlayerMap.erase(player->name);
			delete player;
		}
	}

	int PlayerMgr::LoginPlayer(std::string& name, unsigned int pass, unsigned int version, Player*& pplayer) {
		if(version != proto_version)
			return RF_LOGIN_VERSION_ERR;
		Player* player = FindPlayer(name);
		if(!player)
			return RF_LOGIN_INVALID_ACC;
		if(player->password != pass)
			return RF_LOGIN_INVALID_PASS;
		if(player->status & PLAYER_STATUS_LOGIN)
			return RF_LOGIN_ALREADY_LOGIN;
		if(player->status & PLAYER_STATUS_BAN)
			return RF_LOGIN_ACC_BANED;
		player->status |= PLAYER_STATUS_LOGIN;
		pplayer = player;
		return 0;
	}

	int PlayerMgr::LoginPlayer(int uid, unsigned int pass, unsigned int version, Player*& pplayer) {
		if(version != proto_version)
			return RF_LOGIN_VERSION_ERR;
		Player* player = FindPlayer(uid);
		if(!player)
			return RF_LOGIN_INVALID_ACC;
		if(player->password != pass)
			return RF_LOGIN_INVALID_PASS;
		if(player->status & PLAYER_STATUS_LOGIN)
			return RF_LOGIN_ALREADY_LOGIN;
		if(player->status & PLAYER_STATUS_BAN)
			return RF_LOGIN_ACC_BANED;
		player->status |= PLAYER_STATUS_LOGIN;
		pplayer = player;
		player->Login();
		return 0;
	}

	void PlayerMgr::LogoutPlayer(Player* player) {
		if(!player || !(player->status & PLAYER_STATUS_LOGIN))
			return;
		player->status &= ~PLAYER_STATUS_LOGIN;
		player->Logout();
	}

	void PlayerMgr::KickPlayer(Player* player) {

	}

	void PlayerMgr::SendPacket(unsigned char proto, void* buffer, size_t len) {
		for(auto& iter : uidPlayerMap) {
			if(iter.second->connection)
				iter.second->connection->SendPacket(proto, buffer, len);
		}
	}

	void PlayerMgr::SendPacket(PacketWriter& pkt) {
		for(auto& iter : uidPlayerMap) {
			if(iter.second->connection)
				iter.second->connection->SendPacket(pkt);
		}
	}

}
