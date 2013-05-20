#include "duel_manager.h"
#include "player.h"
#include "duel_single.h"
#include "duel_match.h"
#include "duel_tag.h"

namespace ygopro
{

	DuelManager duelManager;

	DuelManager::DuelManager() {

	}

	DuelManager::~DuelManager() {

	}

	void DuelManager::StartDuelThread(int count) {
		for(int i = 0; i < count; ++i) {
			auto thread = DuelThread::NewDuelThread(9810 + i);
			threads.insert(thread);
		}
	}

	DuelBase* DuelManager::CreateDuel(Player* player, unsigned int mode) {
		if(player->current_duel)
			return nullptr;
		DuelThread* thread = *threads.begin();
		threads.erase(thread);
		thread->duel_count++;
		threads.insert(thread);
		DuelBase* db = nullptr;
		switch(mode) {
		case 1: db = new DuelSingle(); break;
		case 2: db = new DuelMatch(); break;
		case 3: db = new DuelTag(); break;
		default: db = new DuelSingle(); break;
		}
		db->exec_thread = thread;
		return db;
	}

	void DuelManager::RemoveDuel(DuelBase* pbase) {
		DuelThread* thread = pbase->exec_thread;
		if(!thread)
			return;
		threads.erase(thread);
		thread->duel_count--;
		threads.insert(thread);
	}

}
