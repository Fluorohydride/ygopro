#include "ticket.h"
#include "player.h"
#include "player_mgr.h"

namespace ygopro
{

	TicketMgr ticketMgr;

	void TicketMgr::LoadTicketInfos() {
		MongoQuery mq = MongoExecutor(dbAdapter)("$query")()("$orderby", "duel_uid", 1) << "ygo.ticket";
		unsigned int duel_uid = 0, current = 0, playerid = 0;
		TicketInfo* pticket = nullptr;
		while(mq++) {
			Player* new_player = new Player;
			mq("duel_uid", duel_uid)("playerid", playerid);
			if(current != duel_uid) {
				current = duel_uid;
				pticket = new TicketInfo();
				pticket->duel_uid = current;
				ticket_pool[current] = pticket;
			}
			pticket->player_paid.insert(playerid);
		}
	}

	TicketInfo* TicketMgr::NewTicket(unsigned int duel_uid, unsigned int price) {
		auto iter = ticket_pool.find(duel_uid);
		if(iter == ticket_pool.end()) {
			TicketInfo* pticket = new TicketInfo;
			pticket->duel_uid = duel_uid;
			pticket->price = price;
			ticket_pool[duel_uid] = pticket;
			return pticket;
		} else {
			iter->second->price = price;
			return iter->second;
		}
	}

	void TicketMgr::RemoveTicket(unsigned int duel_uid) {
		ticket_pool.erase(duel_uid);
		MongoExecutor(dbAdapter)("duel_uid", duel_uid) - "ygo.ticket";
	}

	bool TicketMgr::CheckTicket(unsigned int duel_uid, Player* player) {
		auto iter = ticket_pool.find(duel_uid);
		if(iter == ticket_pool.end())
			return false;
		return iter->second->player_paid.find(player->uid) != iter->second->player_paid.end();
	}

	void TicketMgr::BuyTicket(unsigned int duel_uid, Player* player) {
		auto iter = ticket_pool.find(duel_uid);
		if(iter == ticket_pool.end())
			return;
		if(iter->second->player_paid.find(player->uid) != iter->second->player_paid.end())
			return;
		iter->second->player_paid.insert(player->uid);
		MongoExecutor(dbAdapter, true)("duel_uid", duel_uid)("playerid", player->uid) + "ygo.ticket";
	}

}
