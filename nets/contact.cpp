#include "player.h"

namespace ygopro
{

	void ContactCollection::SendPacket(unsigned char proto, void* buffer, size_t len) {
		if(pfriends) {
			for(auto& iter : *pfriends) {
				if(iter.second == RELATION_FRIEND)
					iter.first->SendPacket(proto, buffer, len);
			}
		} else {
			for(auto& iter : *pfollowers)
				iter->SendPacket(proto, buffer, len);
		}
	}

	void ContactCollection::SendPacket(PacketWriter& pkt) {
		if(pfriends) {
			for(auto& iter : *pfriends) {
				if(iter.second == RELATION_FRIEND)
					iter.first->SendPacket(pkt);
			}
		} else {
			for(auto& iter : *pfollowers)
				iter->SendPacket(pkt);
		}
	}

	void Player::PublicStatusChange(int new_status) {
		if(new_status == public_status)
			return;
		if(status & PLAYER_STATUS_PUBLIC_F)
			PacketWriter(STOC_FRIEND_STATUS) << uid << public_status >> GetFriendCollection(true);
		else
			PacketWriter(STOC_FRIEND_STATUS) << uid << public_status >> GetFriendCollection(false);
	}

	void Player::SendFriendList() {
		PacketWriter pw(STOC_FRIEND_LIST);
		for(auto& iter : friends)
			pw << iter.first->uid << iter.first->name << iter.second << iter.first->public_status;
		pw >> this;
	}

	void Player::AddFriend(Player* player) {
		auto iter = friends.find(player);
		if(iter == friends.end()) {
			friends[player] = RELATION_FRIEND;
			PacketWriter(STOC_FRIEND_NEW) << player->uid << player->name << RELATION_FRIEND >> this;
			MongoExecutor(dbAdapter, true)("player1", this->uid)("player2", player->uid)("relation", RELATION_FRIEND) + "ygo.friend";
		} else {
			if(iter->second == RELATION_FRIEND)
				return;
			iter->second = RELATION_FRIEND;
			PacketWriter(STOC_FRIEND_ALTER) << player->uid << RELATION_FRIEND >> this;
			MongoExecutor(dbAdapter)("player1", this->uid)("player2", player->uid).Op()("relation", RELATION_FRIEND) >> "ygo.friend";
		}
		if(IsOnline())
			player->followers.insert(this);
		auto iter2 = player->friends.find(this);
		if(iter2 == player->friends.end()) {
			player->friends[this] = RELATION_PENDING;
			PacketWriter(STOC_FRIEND_NEW) << this->uid << this->name << RELATION_PENDING >> player;
			MongoExecutor(dbAdapter, true)("player1", player->uid)("player2", this->uid)("relation", RELATION_PENDING) + "ygo.friend";
		}
	}

	void Player::RemoveFriend(Player* player) {
		auto iter = friends.find(player);
		if(iter == friends.end())
			return;
		friends.erase(player);
		PacketWriter(STOC_FRIEND_ALTER) << player->uid << RELATION_NONE >> this;
		MongoExecutor(dbAdapter)("player1", this->uid)("player2", player->uid) - "ygo.friend";
	}

	bool Player::IsFriend(Player* player) {
		auto iter = friends.find(player);
		return (iter != friends.end()) && (iter->second == RELATION_FRIEND);
	}

	ContactCollection Player::GetFriendCollection(bool isFriend) {
		if(isFriend)
			return ContactCollection(&friends, nullptr);
		else
			return ContactCollection(nullptr, &followers);
	}

}
