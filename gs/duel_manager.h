#ifndef _DUEL_MANAGER_H_
#define _DUEL_MANAGER_H_

#include "../common/common.h"
#include "duel_thread.h"
#include <unordered_map>
#include <set>
#include <queue>

namespace ygopro
{

	class Player;
	class DuelBase;

	class DuelManager {
	public:
		DuelManager();
		~DuelManager();

		void StartDuelThread(int count);
		DuelBase* CreateDuel(Player* player, unsigned int mode);
		void RemoveDuel(DuelBase* pbase);

	protected:
		std::set<DuelThread*, DuelThreadSort> threads;
		std::unordered_map<unsigned int, DuelBase*> duel_maps;
	};

	extern DuelManager duelManager;

}

#endif
