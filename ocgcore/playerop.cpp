/*
 * playerop.cpp
 *
 *  Created on: 2010-12-22
 *      Author: Argon
 */

#include "field.h"
#include "duel.h"
#include "effect.h"
#include "card.h"

#include <memory.h>
#include <algorithm>

int32 field::select_battle_command(uint16 step, uint8 playerid) {
	if(step == 0) {
		pduel->write_buffer8(MSG_SELECT_BATTLECMD);
		pduel->write_buffer8(playerid);
		uint32 i;
		card* pcard;
		effect* peffect;
		//Activatable
		pduel->write_buffer8(core.select_chains.size());
		std::sort(core.select_chains.begin(), core.select_chains.end(), chain::chain_operation_sort);
		for(i = 0; i < core.select_chains.size(); ++i) {
			peffect = core.select_chains[i].triggering_effect;
			pcard = peffect->handler;
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer32(peffect->description);
		}
		//Attackable
		pduel->write_buffer8(core.attackable_cards.size());
		for(i = 0; i < core.attackable_cards.size(); ++i) {
			pcard = core.attackable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer8(pcard->operation_param);
		}
		//M2, EP
		if(core.to_m2)
			pduel->write_buffer8(1);
		else
			pduel->write_buffer8(0);
		if(core.to_ep)
			pduel->write_buffer8(1);
		else
			pduel->write_buffer8(0);
		return FALSE;
	} else {
		uint32 t = returns.ivalue[0] & 0xffff;
		uint32 s = returns.ivalue[0] >> 16;
		if(t < 0 || t > 3 || s < 0
		        || (t == 0 && s >= core.select_chains.size())
		        || (t == 1 && s >= core.attackable_cards.size())
		        || (t == 2 && !core.to_m2)
		        || (t == 3 && !core.to_ep)) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_idle_command(uint16 step, uint8 playerid) {
	if(step == 0) {
		uint32 i;
		card* pcard;
		effect* peffect;
		pduel->write_buffer8(MSG_SELECT_IDLECMD);
		pduel->write_buffer8(playerid);
		//idle summon
		pduel->write_buffer8(core.summonable_cards.size());
		for(i = 0; i < core.summonable_cards.size(); ++i) {
			pcard = core.summonable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		//idle spsummon
		pduel->write_buffer8(core.spsummonable_cards.size());
		for(i = 0; i < core.spsummonable_cards.size(); ++i) {
			pcard = core.spsummonable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		//idle pos change
		pduel->write_buffer8(core.repositionable_cards.size());
		for(i = 0; i < core.repositionable_cards.size(); ++i) {
			pcard = core.repositionable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		//idle mset
		pduel->write_buffer8(core.msetable_cards.size());
		for(i = 0; i < core.msetable_cards.size(); ++i) {
			pcard = core.msetable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		//idle sset
		pduel->write_buffer8(core.ssetable_cards.size());
		for(i = 0; i < core.ssetable_cards.size(); ++i) {
			pcard = core.ssetable_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		//idle activate
		pduel->write_buffer8(core.select_chains.size());
		std::sort(core.select_chains.begin(), core.select_chains.end(), chain::chain_operation_sort);
		for(i = 0; i < core.select_chains.size(); ++i) {
			peffect = core.select_chains[i].triggering_effect;
			pcard = peffect->handler;
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer32(peffect->description);
		}
		//To BP
		if(infos.phase == PHASE_MAIN1 && core.to_bp)
			pduel->write_buffer8(1);
		else
			pduel->write_buffer8(0);
		if(core.to_ep)
			pduel->write_buffer8(1);
		else
			pduel->write_buffer8(0);
		if(infos.shuffle_count < 1 && player[playerid].list_hand.size() > 1)
			pduel->write_buffer8(1);
		else
			pduel->write_buffer8(0);
		return FALSE;
	} else {
		uint32 t = returns.ivalue[0] & 0xffff;
		uint32 s = returns.ivalue[0] >> 16;
		if(t < 0 || t > 8 || s < 0
		        || (t == 0 && s >= core.summonable_cards.size())
		        || (t == 1 && s >= core.spsummonable_cards.size())
		        || (t == 2 && s >= core.repositionable_cards.size())
		        || (t == 3 && s >= core.msetable_cards.size())
		        || (t == 4 && s >= core.ssetable_cards.size())
		        || (t == 5 && s >= core.select_chains.size())
		        || (t == 6 && (infos.phase != PHASE_MAIN1 || !core.to_bp))
		        || (t == 7 && !core.to_ep)
		        || (t == 8 && !(infos.shuffle_count < 1 && player[playerid].list_hand.size() > 1))) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_effect_yes_no(uint16 step, uint8 playerid, card* pcard) {
	if(step == 0) {
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.ivalue[0] = 1;
			return TRUE;
		}
		pduel->write_buffer8(MSG_SELECT_EFFECTYN);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(pcard->data.code);
		pduel->write_buffer32(pcard->get_info_location());
		returns.ivalue[0] = -1;
		return FALSE;
	} else {
		if(returns.ivalue[0] != 0 && returns.ivalue[0] != 1) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_yes_no(uint16 step, uint8 playerid, uint32 description) {
	if(step == 0) {
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.ivalue[0] = 1;
			return TRUE;
		}
		pduel->write_buffer8(MSG_SELECT_YESNO);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(description);
		returns.ivalue[0] = -1;
		return FALSE;
	} else {
		if(returns.ivalue[0] != 0 && returns.ivalue[0] != 1) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_option(uint16 step, uint8 playerid) {
	if(step == 0) {
		returns.ivalue[0] = -1;
		if(core.select_options.size() == 0)
			return TRUE;
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.ivalue[0] = 0;
			return TRUE;
		}
		pduel->write_buffer8(MSG_SELECT_OPTION);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(core.select_options.size());
		for(uint32 i = 0; i < core.select_options.size(); ++i)
			pduel->write_buffer32(core.select_options[i]);
		return FALSE;
	} else {
		if(returns.ivalue[0] < 0 || returns.ivalue[0] >= (int32)core.select_options.size()) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_card(uint16 step, uint8 playerid, uint8 cancelable, uint8 min, uint8 max) {
	if(step == 0) {
		returns.bvalue[0] = 0;
		if(max == 0 || core.select_cards.empty())
			return TRUE;
		if(max > 63)
			max = 63;
		if(max > core.select_cards.size())
			max = core.select_cards.size();
		if(min > max)
			min = max;
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.bvalue[0] = min;
			for(uint8 i = 0; i < min; ++i)
				returns.bvalue[i + 1] = i;
			return TRUE;
		}
		core.units.begin()->arg2 = ((uint32)min) + (((uint32)max) << 16);
		pduel->write_buffer8(MSG_SELECT_CARD);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(cancelable);
		pduel->write_buffer8(min);
		pduel->write_buffer8(max);
		pduel->write_buffer8(core.select_cards.size());
		card* pcard;
		std::sort(core.select_cards.begin(), core.select_cards.end(), card::card_operation_sort);
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			pcard = core.select_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer32(pcard->get_info_location());
		}
		return FALSE;
	} else {
		if(cancelable && returns.ivalue[0] == -1)
			return TRUE;
		byte c[64];
		memset(c, 0, 64);
		if(returns.bvalue[0] < min || returns.bvalue[0] > max) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		uint8 m = core.select_cards.size(), v = 0;
		for(int32 i = 0; i < returns.bvalue[0]; ++i) {
			v = returns.bvalue[i + 1];
			if(v < 0 || v >= m || v >= 63 || c[v]) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			c[v] = 1;
		}
		return TRUE;
	}
}
int32 field::select_chain(uint16 step, uint8 playerid, uint8 spe_count, uint8 forced) {
	if(step == 0) {
		returns.ivalue[0] = -1;
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			if(core.select_chains.size() == 0)
				returns.ivalue[0] = -1;
			else {
				bool act = true;
				for(auto chit = core.current_chain.begin(); chit != core.current_chain.end(); ++chit)
					if(chit->triggering_player == 1)
						act = false;
				if(act)
					returns.ivalue[0] = 0;
				else
					returns.ivalue[0] = -1;
			}
			return TRUE;
		}
		pduel->write_buffer8(MSG_SELECT_CHAIN);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(core.select_chains.size());
		pduel->write_buffer8(spe_count);
		pduel->write_buffer8(forced);
		pduel->write_buffer32(pduel->game_field->core.hint_timing[playerid]);
		pduel->write_buffer32(pduel->game_field->core.hint_timing[1 - playerid]);
		std::sort(core.select_chains.begin(), core.select_chains.end(), chain::chain_operation_sort);
		for(uint32 i = 0; i < core.select_chains.size(); ++i) {
			effect* peffect = core.select_chains[i].triggering_effect;
			card* pcard = peffect->handler;
			if(!(peffect->type & EFFECT_TYPE_ACTIONS))
				pcard = peffect->owner;
			if(peffect->flag & EFFECT_FLAG_FIELD_ONLY)
				pduel->write_buffer32(1000000000 + pcard->data.code);
			else
				pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer32(pcard->get_info_location());
			pduel->write_buffer32(peffect->description);
		}
		return FALSE;
	} else {
		if((returns.ivalue[0] < 0 && forced) || returns.ivalue[0] >= (int32)core.select_chains.size()) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_place(uint16 step, uint8 playerid, uint32 flag, uint8 count) {
	if(step == 0) {
		if(count == 0)
			return TRUE;
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.bvalue[0] = 1;
			flag = ~flag;
			if(flag & 0x1f) {
				returns.bvalue[1] = LOCATION_MZONE;
				if(flag & 0x4) returns.bvalue[2] = 2;
				else if(flag & 0x2) returns.bvalue[2] = 1;
				else if(flag & 0x8) returns.bvalue[2] = 3;
				else if(flag & 0x1) returns.bvalue[2] = 0;
				else returns.bvalue[2] = 4;
			} else {
				returns.bvalue[1] = LOCATION_SZONE;
				if(flag & 0x400) returns.bvalue[2] = 2;
				else if(flag & 0x200) returns.bvalue[2] = 1;
				else if(flag & 0x800) returns.bvalue[2] = 3;
				else if(flag & 0x100) returns.bvalue[2] = 0;
				else returns.bvalue[2] = 4;
			}
			return true;
		}
		if(core.units.begin()->type == PROCESSOR_SELECT_PLACE)
			pduel->write_buffer8(MSG_SELECT_PLACE);
		else
			pduel->write_buffer8(MSG_SELECT_DISFIELD);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(count);
		pduel->write_buffer32(flag);
		returns.bvalue[0] = 0;
		return FALSE;
	} else {
		uint8 pt = 0, p, l, s;
		for(int8 i = 0; i < count; ++i) {
			p = returns.bvalue[pt];
			l = returns.bvalue[pt + 1];
			s = returns.bvalue[pt + 2];
			if((p != 0 && p != 1) || ((l != LOCATION_MZONE) && (l != LOCATION_SZONE)) || s >= 5
			        || ((1 << s) & (flag >> (((p == playerid) ? 0 : 16) + ((l == LOCATION_MZONE) ? 0 : 8))))) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			pt += 3;
		}
		return TRUE;
	}
}
int32 field::select_position(uint16 step, uint8 playerid, uint32 code, uint8 positions) {
	if(step == 0) {
		if(positions == 0) {
			returns.ivalue[0] = POS_FACEUP_ATTACK;
			return TRUE;
		}
		positions &= 0xf;
		if(positions == 0x1 || positions == 0x2 || positions == 0x4 || positions == 0x8) {
			returns.ivalue[0] = positions;
			return TRUE;
		}
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			if(positions & 0x4)
				returns.ivalue[0] = 0x4;
			else if(positions & 0x1)
				returns.ivalue[0] = 0x1;
			else if(positions & 0x8)
				returns.ivalue[0] = 0x8;
			else
				returns.ivalue[0] = 0x2;
			return TRUE;
		}
		pduel->write_buffer8(MSG_SELECT_POSITION);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(code);
		pduel->write_buffer8(positions);
		returns.ivalue[0] = 0;
		return FALSE;
	} else {
		uint32 pos = returns.ivalue[0];
		if(pos != 0x1 && pos != 0x2 && pos != 0x4 && pos != 0x8 && !(pos & positions)) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_tribute(uint16 step, uint8 playerid, uint8 cancelable, uint8 min, uint8 max) {
	if(step == 0) {
		returns.bvalue[0] = 0;
		if(max == 0 || core.select_cards.empty())
			return TRUE;
		uint8 tm = 0;
		for(uint32 i = 0; i < core.select_cards.size(); ++i)
			tm += core.select_cards[i]->operation_param;
		if(max > 5)
			max = 5;
		if(max > tm)
			max = tm;
		if(min > max)
			min = max;
		core.units.begin()->arg2 = ((uint32)min) + (((uint32)max) << 16);
		pduel->write_buffer8(MSG_SELECT_TRIBUTE);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(cancelable);
		pduel->write_buffer8(min);
		pduel->write_buffer8(max);
		pduel->write_buffer8(core.select_cards.size());
		card* pcard;
		std::sort(core.select_cards.begin(), core.select_cards.end(), card::card_operation_sort);
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			pcard = core.select_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer8(pcard->operation_param);
		}
		return FALSE;
	} else {
		if(cancelable && returns.ivalue[0] == -1)
			return TRUE;
		byte c[64];
		memset(c, 0, 64);
		if(returns.bvalue[0] > max) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		uint8 m = core.select_cards.size(), v = 0, tt = 0;
		for(int32 i = 0; i < returns.bvalue[0]; ++i) {
			v = returns.bvalue[i + 1];
			if(v < 0 || v >= m || v >= 6 || c[v]) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			c[v] = 1;
			tt += core.select_cards[v]->operation_param;
		}
		if(tt < min) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		return TRUE;
	}
}
int32 field::select_counter(uint16 step, uint8 playerid, uint16 countertype, uint16 count) {
	if(step == 0) {
		if(core.select_cards.empty() || count == 0)
			return TRUE;
		uint8 tm = 0;
		for(uint32 i = 0; i < core.select_cards.size(); ++i)
			tm += core.select_cards[i]->operation_param;
		if(count > tm) {
			count = tm;
			core.units.begin()->arg2 = countertype + (((uint32)count) << 16);
		}
		pduel->write_buffer8(MSG_SELECT_COUNTER);
		pduel->write_buffer8(playerid);
		pduel->write_buffer16(countertype);
		pduel->write_buffer8((uint8)count);
		pduel->write_buffer8(core.select_cards.size());
		card* pcard;
		std::sort(core.select_cards.begin(), core.select_cards.end(), card::card_operation_sort);
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			pcard = core.select_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer8(pcard->operation_param);
		}
		return FALSE;
	} else {
		uint16 ct = 0;
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			if(core.select_cards[i]->operation_param < (uint32)returns.bvalue[i]) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			ct += returns.bvalue[i];
		}
		if(ct != count) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
	}
	return TRUE;
}
int32 select_sum_check1(int32* oparam, int32 size, int32 index, int32 count, int32 acc, int32 min) {
	if(acc == 0 || index == size)
		return FALSE;
	int32 o1 = oparam[index] & 0xffff;
	int32 o2 = oparam[index] >> 16;
	if((o1 == acc || o2 == acc) && index == size - 1 && count >= min)
		return TRUE;
	return (acc > o1 && select_sum_check1(oparam, size, index + 1, count + 1, acc - o1, min))
	       || (o2 > 0 && acc > o2 && select_sum_check1(oparam, size, index + 1, count + 1, acc - o2, min));
}
int32 field::select_with_sum_limit(int16 step, uint8 playerid, int32 acc, int32 min, int32 max) {
	if(step == 0) {
		returns.bvalue[0] = 0;
		if(core.select_cards.empty())
			return TRUE;
		pduel->write_buffer8(MSG_SELECT_SUM);
		if(max)
			pduel->write_buffer8(0);
		else
			pduel->write_buffer8(1);
		if(max < min)
			max = min;
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(acc & 0xffff);
		pduel->write_buffer8(min);
		pduel->write_buffer8(max);
		pduel->write_buffer8(core.select_cards.size());
		card* pcard;
		std::sort(core.select_cards.begin(), core.select_cards.end(), card::card_operation_sort);
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			pcard = core.select_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer32(pcard->operation_param);
		}
		return FALSE;
	} else {
		byte c[64];
		memset(c, 0, 64);
		if(min) {
			int32 oparam[16];
			if(returns.bvalue[0] < min || returns.bvalue[0] > max) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			uint8 m = core.select_cards.size(), v = 0;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				v = returns.bvalue[i + 1];
				if(v < 0 || v >= m || c[v]) {
					pduel->write_buffer8(MSG_RETRY);
					return FALSE;
				}
				c[v] = 1;
				oparam[i] = core.select_cards[v]->operation_param;
			}
			if(!select_sum_check1(oparam, returns.bvalue[0], 0, 1, acc, min)) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			return TRUE;
		} else {
			uint8 m = core.select_cards.size(), v = 0;
			int32 op, o1, o2, sum = 0, mx = 0, mn = 0x7fffffff;
			int16 ms[16];
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				v = returns.bvalue[i + 1];
				if(v < 0 || v >= m || c[v]) {
					pduel->write_buffer8(MSG_RETRY);
					return FALSE;
				}
				c[v] = 1;
				op = core.select_cards[v]->operation_param;
				o1 = op & 0xffff;
				o2 = op >> 16;
				sum += ms[i] = (o2 && o2 < o1) ? o2 : o1;
				mx += (o2 > o1) ? o2 : o1;
				if(ms[i] < mn)
					mn = ms[i];
			}
			if(mx < acc || sum - mn >= acc) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			return TRUE;
		}
	}
	return TRUE;
}
int32 field::sort_card(int16 step, uint8 playerid, uint8 is_chain) {
	if(step == 0) {
		returns.bvalue[0] = 0;
		if((playerid == 1) && (core.duel_options & DUEL_SIMPLE_AI)) {
			returns.ivalue[0] = -1;
			return TRUE;
		}
		if(core.select_cards.empty())
			return TRUE;
		if(is_chain)
			pduel->write_buffer8(MSG_SORT_CHAIN);
		else
			pduel->write_buffer8(MSG_SORT_CARD);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(core.select_cards.size());
		card* pcard;
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			pcard = core.select_cards[i];
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
		}
		return FALSE;
	} else {
		if(returns.bvalue[0] == -1)
			return TRUE;
		uint8 seq[64];
		memset(seq, 0, 64);
		for(uint32 i = 0; i < core.select_cards.size(); ++i) {
			if(returns.bvalue[i] < 0 || returns.bvalue[i] >= (int32)core.select_cards.size() || seq[(int32)returns.bvalue[i]]) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			seq[(int32)returns.bvalue[i]] = 1;
		}
		return TRUE;
	}
	return TRUE;
}
int32 field::announce_race(int16 step, uint8 playerid, int32 count, int32 available) {
	if(step == 0) {
		int32 scount = 0;
		for(int32 ft = 0x1; ft != 0x1000000; ft <<= 1) {
			if(ft & available)
				scount++;
		}
		if(scount <= count) {
			count = scount;
			core.units.begin()->arg1 = (count << 16) + playerid;
		}
		pduel->write_buffer8(MSG_ANNOUNCE_RACE);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(count);
		pduel->write_buffer32(available);
		return FALSE;
	} else {
		int32 rc = returns.ivalue[0];
		int32 sel = 0;
		for(int32 ft = 0x1; ft != 0x1000000; ft <<= 1) {
			if(!(ft & rc)) continue;
			if(!(ft & available)) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			sel++;
		}
		if(sel != count) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_RACE);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(returns.ivalue[0]);
		return TRUE;
	}
	return TRUE;
}
int32 field::announce_attribute(int16 step, uint8 playerid, int32 count, int32 available) {
	if(step == 0) {
		int32 scount = 0;
		for(int32 ft = 0x1; ft != 0x80; ft <<= 1) {
			if(ft & available)
				scount++;
		}
		if(scount <= count) {
			count = scount;
			core.units.begin()->arg1 = (count << 16) + playerid;
		}
		pduel->write_buffer8(MSG_ANNOUNCE_ATTRIB);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(count);
		pduel->write_buffer32(available);
		return FALSE;
	} else {
		int32 rc = returns.ivalue[0];
		int32 sel = 0;
		for(int32 ft = 0x1; ft != 0x80; ft <<= 1) {
			if(!(ft & rc)) continue;
			if(!(ft & available)) {
				pduel->write_buffer8(MSG_RETRY);
				return FALSE;
			}
			sel++;
		}
		if(sel != count) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_ATTRIB);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(returns.ivalue[0]);
		return TRUE;
	}
	return TRUE;
}
int32 field::announce_card(int16 step, uint8 playerid) {
	if(step == 0) {
		pduel->write_buffer8(MSG_ANNOUNCE_CARD);
		pduel->write_buffer8(playerid);
		return FALSE;
	} else {
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_CODE);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(returns.ivalue[0]);
		return TRUE;
	}
	return TRUE;
}
int32 field::announce_number(int16 step, uint8 playerid) {
	if(step == 0) {
		pduel->write_buffer8(MSG_ANNOUNCE_NUMBER);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(core.select_options.size());
		for(uint32 i = 0; i < core.select_options.size(); ++i)
			pduel->write_buffer32(core.select_options[i]);
		return FALSE;
	} else {
		int32 ret = returns.ivalue[0];
		if(ret < 0 || ret >= (int32)core.select_options.size() || ret >= 63) {
			pduel->write_buffer8(MSG_RETRY);
			return FALSE;
		}
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_NUMBER);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(core.select_options[returns.ivalue[0]]);
		return TRUE;
	}
}
