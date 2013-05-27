#ifndef _DUEL_SINGLE_H_
#define _DUEL_SINGLE_H_

#include "duel_base.h"

namespace ygopro
{

	class DuelSingle: public DuelBase {

	public:
		DuelSingle();
		virtual ~DuelSingle();

		virtual void PlayerEnter(Player* player, bool match);
		virtual void PlayerLeave(Player* player);
		virtual void PlayerMove(Player* player);
		virtual void PlayerChat(Player* player, const std::string& msg);
		virtual void PlayerReady(Player* player);
		virtual void PlayerHandSelect(Player* player);
		virtual void PlayerSeqSelect(Player* player);
		virtual void GameBegin();
		virtual void DuelBegin();
		virtual void DuelLoop();
		virtual void ObserverEnter(Player* player);
		virtual void ObserverLeave(Player* player);
		virtual void ObserverChat(Player* player, const std::string& msg);

		virtual void SendPacket(unsigned char proto, void* buffer, size_t len);
		virtual void SendPacket(PacketWriter& pkt);

	protected:
		Player* game_player[2];

	};

}

#endif
