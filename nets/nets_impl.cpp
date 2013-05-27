#include "nets_impl.h"
#include "player.h"
#include "player_mgr.h"
#include "clan.h"
#include "clan_mgr.h"
#include "res_mgr.h"

namespace ygopro {

	// packet format: 
	// |  packet_size |      response_seed     |              packet               |
	// |              |                        |   proto(8bit)   |       data      |
	// |    16 bits   |         32 bits        |          packet_size bits         |
	// packet_size must less then 1024
	void PlayerConnection::HandleData(bufferevent* bev) {
		evbuffer* input = bufferevent_get_input(bev);
		size_t len = evbuffer_get_length(input);
		unsigned short packet_len = 0;
		while(len >= 6) {
			evbuffer_copyout(input, &packet_len, 2);
			if(len < (size_t)(packet_len + 6))
				return;
			unsigned char buffer[2048];
			evbuffer_remove(input, buffer, packet_len + 6);
			unsigned int resp_seed = *(unsigned int*)&buffer[2];
			if(packet_len && (resp_seed == response_seed)) {
				HandlePacket(buffer[6], &buffer[7], packet_len - 1);
				response_seed = (resp_seed * 0x1fd45cc2 + 0xdee89c3a) ^ 0xe7262ca9;
			}
			len -= packet_len + 6;
		}
	}

	void PlayerConnection::HandlePacket(unsigned char proto, unsigned char data[], unsigned int size) {
		PacketReader pr(data, size);
		if(!ref_player) {
			switch(proto) {
			case CTOS_LOGIN_NAME:
			case CTOS_LOGIN_UID:
				{
					Player* player = nullptr;
					int result;
					if(proto == CTOS_LOGIN_NAME) {
						unsigned int version, pass;
						std::string name;
						pr >> version >> name >> pass;
						result = playerMgr.LoginPlayer(name, pass ^ response_seed, version, player);
					} else {
						unsigned int version, pass, uid;
						pr >> version >> uid >> pass;
						result = playerMgr.LoginPlayer(static_cast<int>(uid), pass ^ response_seed, version, player);
					}
					if(result) {
						PacketWriter(STOC_REQUEST_FAIL) << result >> this;
					} else {
						ref_player = player;
						PacketWriter(STOC_LOGIN_SUCCESS) << player->uid << player->last_login_ip << player->last_login_time >> this;
						player->connection = this;
						player->last_login_ip = address;
						player->last_login_time = time(0);
						player->SetField("last_login_ip", player->last_login_ip, "last_login_time", player->last_login_time);
						player->Login();
					}
					break;
				}
			case CTOS_REGISTER:
				{
					std::string name;
					unsigned int pass;
					pr >> name >> pass;
					if(!playerMgr.NewPlayer(pass ^ response_seed, name)) {
						PacketWriter(STOC_REQUEST_FAIL) << RF_REGISTER_ACC_EXISTS >> this;
					} else {
						PacketWriter(STOC_REQUEST_SUCCESS) << RS_REGISTER_SUCCESS >> this;
					}
					break;
				}
			}
			return;
		}
		switch(proto) {
		case CTOS_FILE_TRANSFER_START:
			{
				unsigned int reason, size;
				pr >> reason >> size;
				if(!ref_player->CheckPrivilege(reason) || size > 0xffff)
					return;
				int res_id = ref_player->FileReceiveBegin(reason, size);
				PacketWriter(STOC_FILE_CONFIRM) << res_id >> this;
				break;
			}
		case CTOS_FILE_CONTENT:
			{
				ref_player->FileReceiveAppend(data, size);
				break;
			}
		case CTOS_FILE_FETCH:
			{
				unsigned int res_id = pr.ReadUInt32();
				ResourceInfo& res = resourceMgr.GetResource(res_id);
				if(res.data) {
					PacketWriter(STOC_FILE_TRANSFER_START) << res_id << res.size >> this;
					unsigned int sz = 0;
					for(; sz < res.size - 1024; sz += 1024)
						SendPacket(STOC_FILE_CONTENT, &res.data[sz], 1024);
					SendPacket(STOC_FILE_CONTENT, &res.data[sz], res.size - sz);
				} else {
					PacketWriter(STOC_FILE_FETCH_FAIL) << res_id >> this;
				}
				break;
			}
		case CTOS_QUERY_PLAYER:
			{
				Player* qplayer = playerMgr.FindPlayer(pr.ReadUInt32());
				if(qplayer)
					qplayer->SendPlayerInfo(ref_player);
				break;
			}
		case CTOS_QUERY_PLAYER_NAME:
			{
				unsigned int uid, count = 0;
				PacketWriter pw(STOC_PLAYER_NAME);
				while(count++ < 10 && !pr.IsEnd()) {
					pr >> uid;
					Player* player = playerMgr.FindPlayer(uid);
					if(player)
						pw << uid << player->name;
					else
						pw << uid << "";
				}
				pw >> this;
				break;
			}
		case CTOS_QUERY_CLAN_NAME:
			{
				unsigned int uid, count = 0;
				PacketWriter pw(STOC_CLAN_NAME);
				while(count++ < 10 && !pr.IsEnd()) {
					pr >> uid;
					Clan* clan = clanMgr.FindClan(uid);
					if(clan)
						pw << uid << clan->name;
					else
						pw << uid << "";
				}
				pw >> this;
				break;
			}
		case CTOS_ADD_FRIEND:
			{
				Player* qplayer = playerMgr.FindPlayer(pr.ReadUInt32());
				if(qplayer)
					ref_player->AddFriend(qplayer);
				break;
			}
		case CTOS_REMOVE_FRIEND:
			{
				Player* qplayer = playerMgr.FindPlayer(pr.ReadUInt32());
				if(qplayer)
					ref_player->RemoveFriend(qplayer);
				break;
			}
		}
	}

	void PlayerConnection::Connected() {
		response_seed = globalRandom.GetUInt32();
		PacketWriter(STOC_CONNECT_SUCCESS) << response_seed << (unsigned int)0 >> this;
	}

	void PlayerConnection::Disconnected() {
		if(ref_player) {
			ref_player->connection = nullptr;
			playerMgr.LogoutPlayer(ref_player);
			ref_player = nullptr;
		}
	}

}
