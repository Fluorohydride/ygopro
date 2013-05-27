#include "player.h"
#include "../common/tcpserver.h"

namespace ygopro
{

	Player::Player() {
		uid = 0;
		duel_count = 0;
		drop_count = 0;
		single_count = 0;
		single_win = 0;
		match_count = 0;
		match_win = 0;
		duel_point = 0;
		gold = 0;
		reg_time = 0;
		privilege = 0;
		status = 0;
		clan_id = 0;
		avatar = 0;
		sleeve = 0;
		password = 0;
		last_login_ip = 0;
		last_login_time = 0;

		connection = nullptr;
		current_duel = nullptr;
		current_duel_state = 0;
		current_deck = nullptr;
	}

	Player::~Player() {
		
	}

	void Player::Login() {
		SendPlayerInfo(this);
		for(auto& iter : friends)
			iter.first->followers.insert(this);
		SendFriendList();
	}

	void Player::Logout() {
		FileReceiveEnd(false);
		for(auto& iter : friends)
			iter.first->followers.erase(this);
	}

	bool Player::IsOnline() {
		return (status | PLAYER_STATUS_LOGIN) != 0;
	}

	void Player::SendPlayerInfo(Player* toplayer) {
		PacketWriter(STOC_PLAYER_INFO) << this >> toplayer;
	}

	bool Player::CheckPrivilege(int priv) {
		return true;
	}

	void Player::SendPacket(unsigned char proto, void* buffer, size_t len) {
		if(connection)
			connection->SendPacket(proto, buffer, len);
	}

	void Player::SendPacket(PacketWriter& pkt) {
		if(connection)
			pkt >> connection;
	}

	void Player::WriteToPacket(PacketWriter& _writer) {
		_writer << uid << duel_count << drop_count << single_count << single_win << match_count << match_win << duel_point
			<< gold << reg_time << privilege << status << clan_id << avatar << sleeve << name << sign << email;
	}

	bool Player::FileReceiveEnd(bool save) {
		if(!FileReceiver::FileReceiveEnd(save))
			return false;
		switch (file_receive_type)
		{
		case CTOS_FTREASON_AVATAR:
			{
				avatar = resource_id;
				SetField("avatar", avatar);
				PacketWriter(STOC_PLAYER_INFO_CHANGE) << PIC_AVATAR << avatar >> connection;
				break;
			}
		case CTOS_FTREASON_SLEEVE:
			{
				sleeve = resource_id;
				SetField("sleeve", sleeve);
				PacketWriter(STOC_PLAYER_INFO_CHANGE) << PIC_SLEEVE << sleeve >> connection;
				break;
			}
		}
		return true;
	}

}
