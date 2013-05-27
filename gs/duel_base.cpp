#include "duel_base.h"
#include "duel_thread.h"
#include "player.h"
#include "../ocgcore/ocgapi.h"
#include "../common/packet_handler.h"
#include "nets_impl.h"

namespace ygopro
{

	DuelBase::DuelBase(unsigned int lp, unsigned int hand, unsigned int draw, unsigned int option, unsigned int max_ob, unsigned int mtime, unsigned int ttime):
		exec_thread(nullptr), waiting_player(nullptr), master_player(nullptr), timer_handler(nullptr), waiting_seed(0), duel_stage(0), pduel_id(0) {
		conf_start_lp = lp;
		conf_start_hand = hand;
		conf_draw = draw;
		conf_option = option;
		max_observer = max_ob;
		max_time = mtime;
		turn_time = ttime;
		max_time_left[0] = max_time_left[1] = max_time;
		turn_time_left[0] = turn_time_left[1] = turn_time;
	}

	DuelBase::~DuelBase() {
		if(pduel_id)
			end_duel(pduel_id);
	}

	bool DuelBase::PlayerCheckEnter(Player* player) {
		return true;
	}

	void DuelBase::DuelBegin() {
		pduel_id = create_duel(time(0));
		start_duel(pduel_id, 0);
	}

	void DuelBase::DuelLoop() {
		char engineBuffer[0x1000];
		unsigned int engFlag = 0, engLen = 0;
		int stop = 0;
		while (!stop) {
			if (engFlag == 2)
				break;
			int result = process(pduel_id);
			engLen = result & 0xffff;
			engFlag = result >> 16;
			if (engLen > 0) {
				get_message(pduel_id, (byte*)&engineBuffer);
//				stop = Analyze(engineBuffer, engLen);
			}
		}
//		if(stop == 2)
//			DuelEndProc();
	}

	void DuelBase::SetResponse(Player* player, unsigned int seed, unsigned char resp[], unsigned int size) {
		if(!pduel_id || player != waiting_player || seed != waiting_seed)
			return;
		unsigned char respbuf[64] = {0};
		memcpy(respbuf, resp, size);
		set_responseb(pduel_id, respbuf);
		waiting_player = nullptr;
		waiting_seed = 0;
	}

	void DuelBase::WaitResponse(unsigned int id) {
		waiting_player = playing_player[id];
		waiting_seed = time(0);
		unsigned int left_time = turn_time_left[id];
		if(left_time > max_time_left[id])
			left_time = max_time_left[id];
		PacketWriter(STOC_WAITING_RESPONSE) << left_time << waiting_seed >> waiting_player;
	}

	void DuelBase::WaitConfirm(Player* player, unsigned int seed) {
		if(player != waiting_player || seed != waiting_seed)
			return;
		unsigned int id = player->current_duel_state & 0xf;
		unsigned int left_time = turn_time_left[id];
		if(left_time > max_time_left[id])
			left_time = max_time_left[id];
		waiting_begin = clock();
		TimerBegin(0);
	}

	void DuelBase::TimeoutCallback(short events) {
	}

	bool DuelBase::ObserverCheckEnter(Player* player) {
		return true;
	}

	void DuelBase::ObserverEnter(Player* player) {
		if(observers.size() >= max_observer)
			return;
		if(observers.find(player) != observers.end())
			return;
		observers.insert(player);
	}

	void DuelBase::ObserverLeave(Player* player) {
		if(observers.find(player) == observers.end())
			return;
		observers.erase(player);
	}

	void DuelBase::ObserverChat(Player* player, const std::string& msg) {
		
	}

}
