#include "duel_single.h"
#include "player.h"

namespace ygopro
{

	DuelSingle::DuelSingle() {
		memset(game_player, 0, sizeof(game_player));
	}

	DuelSingle::~DuelSingle() {
	}

	void DuelSingle::PlayerEnter(Player* player, bool match) {
		if(duel_stage != DUEL_STAGE_BEGIN)
			return;
		if(game_player[0] && game_player[1]) {
			PacketWriter(STOC_LOCAL_MESSAGE) << 101 >> player;
			return;
		}
		if(game_player[0] == nullptr) {
			game_player[0] = player;
			player->current_duel_state = 0;
		} else {
			game_player[1] = player;
			player->current_duel_state = 1;
		}

	}

	void DuelSingle::PlayerLeave(Player* player) {
		game_player[player->current_duel_state & 0xf] = nullptr;
	}

	void DuelSingle::PlayerMove(Player* player) {
		if(duel_stage != DUEL_STAGE_BEGIN)
			return;
		if(game_player[0] && game_player[1])
			return;
		if(game_player[0] == player)
			player->current_duel_state = 1;
		else
			player->current_duel_state = 0;
	}

	void DuelSingle::PlayerChat(Player* player, const std::string& msg) {
	}

	void DuelSingle::PlayerReady(Player* player) {
		if(duel_stage != DUEL_STAGE_BEGIN)
			return;
		if(player->current_duel_state & DUEL_STATE_READY)
			player->current_duel_state &= ~DUEL_STATE_READY;
		else
			player->current_duel_state |= DUEL_STATE_READY;
	}

	void DuelSingle::PlayerHandSelect(Player* player) {
	}

	void DuelSingle::PlayerSeqSelect(Player* player) {
	}

	void DuelSingle::GameBegin() {
	}

	void DuelSingle::DuelBegin() {
	}

	void DuelSingle::DuelLoop() {
	}

	void DuelSingle::ObserverEnter(Player* player) {
	}

	void DuelSingle::ObserverLeave(Player* player) {
	}

	void DuelSingle::ObserverChat(Player* player, const std::string& msg) {
	}
	
	void DuelSingle::SendPacket(unsigned char proto, void* buffer, size_t len) {
		game_player[0]->SendPacket(proto, buffer, len);
		game_player[1]->SendPacket(proto, buffer, len);
		for(auto& obiter : observers)
			obiter->SendPacket(proto, buffer, len);
	}

	void DuelSingle::SendPacket(PacketWriter& pkt) {
		game_player[0]->SendPacket(pkt);
		game_player[1]->SendPacket(pkt);
		for(auto& obiter : observers)
			obiter->SendPacket(pkt);
	}
}
