#ifndef _TICKET_H_
#define _TICKET_H_

#include <unordered_set>
#include <unordered_map>

namespace ygopro
{

	class Player;

	struct TicketInfo {
		unsigned int duel_uid;
		unsigned int price;
		std::unordered_set<unsigned int> player_paid;
	};

	class TicketMgr {
	public:
		void LoadTicketInfos();
		TicketInfo* NewTicket(unsigned int duel_uid, unsigned int price);
		void RemoveTicket(unsigned int duel_uid);
		bool CheckTicket(unsigned int duel_uid, Player* player);
		void BuyTicket(unsigned int duel_uid, Player* player);

	private:
		std::unordered_map<unsigned int, TicketInfo*>  ticket_pool;
	};

	extern TicketMgr ticketMgr;
}

#endif
