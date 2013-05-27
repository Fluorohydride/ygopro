#include "duel_tag.h"
#include "player.h"

namespace ygopro
{

	DuelTag::DuelTag() {
		memset(game_player, 0, sizeof(game_player));
	}

	DuelTag::~DuelTag() {
	}

	void DuelTag::PlayerEnter(Player* player, bool match) {
	}

	void DuelTag::PlayerLeave(Player* player) {
	}

	void DuelTag::PlayerMove(Player* player) {
	}

	void DuelTag::PlayerChat(Player* player, const std::string& msg) {
	}

	void DuelTag::PlayerReady(Player* player) {

	}

	void DuelTag::PlayerHandSelect(Player* player) {
	}

	void DuelTag::PlayerSeqSelect(Player* player) {
	}

	void DuelTag::GameBegin() {
	}

	void DuelTag::DuelBegin() {
	}

	void DuelTag::DuelLoop() {
	}

	void DuelTag::ObserverEnter(Player* player) {
	}

	void DuelTag::ObserverLeave(Player* player) {
	}

	void DuelTag::ObserverChat(Player* player, const std::string& msg) {
	}
	
	void DuelTag::SendPacket(unsigned char proto, void* buffer, size_t len) {
		game_player[0]->SendPacket(proto, buffer, len);
		game_player[1]->SendPacket(proto, buffer, len);
		game_player[2]->SendPacket(proto, buffer, len);
		game_player[3]->SendPacket(proto, buffer, len);
		for(auto& obiter : observers)
			obiter->SendPacket(proto, buffer, len);
	}

	void DuelTag::SendPacket(PacketWriter& pkt) {
		game_player[0]->SendPacket(pkt);
		game_player[1]->SendPacket(pkt);
		game_player[2]->SendPacket(pkt);
		game_player[3]->SendPacket(pkt);
		for(auto& obiter : observers)
			obiter->SendPacket(pkt);
	}

}
