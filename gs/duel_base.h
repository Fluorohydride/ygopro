#ifndef _DUEL_BASE_H_
#define _DUEL_BASE_H_

#include "../common/common.h"
#include "../common/packet_handler.h"
#include "timer_object.h"
#include <set>

#define DUEL_STAGE_BEGIN 0
#define DUEL_STAGE_FINGER 1
#define DUEL_STAGE_FIRSTGO 2
#define DUEL_STAGE_DUELING 3
#define DUEL_STAGE_SIDING 4

namespace ygopro
{

	class DuelThread;
	class Player;

	class DuelBase : public TimerObject, public PacketStream {

		friend class DuelManager;

	public:
		DuelBase(unsigned int lp = 8000, unsigned int hand = 5, unsigned int draw = 1, unsigned int option = 0, unsigned int max_ob = 0, unsigned int max_time = 900, unsigned int turn_time = 180);
		virtual ~DuelBase();

		virtual bool PlayerCheckEnter(Player* player);
		virtual void PlayerEnter(Player* player, bool match) = 0;
		virtual void PlayerLeave(Player* player) = 0;
		virtual void PlayerMove(Player* player) = 0;
		virtual void PlayerChat(Player* player, const std::string& msg) = 0;
		virtual void PlayerReady(Player* player) = 0;
		virtual void PlayerHandSelect(Player* player) = 0;
		virtual void PlayerSeqSelect(Player* player) = 0;
		virtual void GameBegin() = 0;
		virtual void DuelBegin();
		virtual void DuelLoop();
		virtual void WaitResponse(unsigned int id);
		virtual void WaitConfirm(Player* player, unsigned int seed);
		virtual void SetResponse(Player* player, unsigned int seed, unsigned char resp[], unsigned int size);
		virtual void TimeoutCallback(short events);
		virtual bool ObserverCheckEnter(Player* player);
		virtual void ObserverEnter(Player* player);
		virtual void ObserverLeave(Player* player);
		virtual void ObserverChat(Player* player, const std::string& msg);
		
	protected: 
		DuelThread* exec_thread;
		Player* playing_player[2];
		Player* waiting_player;
		Player* master_player;
		void* timer_handler;
		std::set<Player*> observers;
		unsigned int duel_uid;
		unsigned int max_observer;
		unsigned int waiting_seed;
		unsigned int duel_stage;
		unsigned long pduel_id;
		unsigned int conf_start_lp;
		unsigned int conf_start_hand;
		unsigned int conf_draw;
		unsigned int conf_option;
		unsigned int max_time;
		unsigned int turn_time;
		unsigned int max_time_left[2];
		unsigned int turn_time_left[2];
		clock_t waiting_begin;
	};

}

#endif
