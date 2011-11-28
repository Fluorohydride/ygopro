/*
 * operations.cpp
 *
 *  Created on: 2010-9-18
 *      Author: Argon
 */
#include "field.h"
#include <algorithm>

int32 field::negate_chain(uint8 chaincount) {
	if(core.current_chain.size() == 0)
		return FALSE;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	chain& pchain = core.current_chain[chaincount - 1];
	if(!(pchain.flag & CHAIN_DISABLE_ACTIVATE) && is_chain_inactivatable(pchain.chain_count)
	        && pchain.triggering_effect->handler->is_affect_by_effect(core.reason_effect) ) {
		pchain.flag |= CHAIN_DISABLE_ACTIVATE | CHAIN_NEGATED;
		pchain.disable_reason = core.reason_effect;
		pchain.disable_player = core.reason_player;
		if((pchain.triggering_effect->type & EFFECT_TYPE_ACTIVATE) && (pchain.triggering_effect->handler->current.location == LOCATION_SZONE)) {
			pchain.triggering_effect->handler->set_status(STATUS_LEAVE_CONFIRMED, TRUE);
			pchain.triggering_effect->handler->set_status(STATUS_ACTIVATE_DISABLED, TRUE);
		}
		pduel->write_buffer8(MSG_CHAIN_INACTIVATED);
		pduel->write_buffer8(chaincount);
		return TRUE;
	}
	return FALSE;
}
int32 field::disable_chain(uint8 chaincount) {
	if(core.current_chain.size() == 0)
		return FALSE;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	chain& pchain = core.current_chain[chaincount - 1];
	if(!(pchain.flag & CHAIN_DISABLE_EFFECT) && is_chain_disablable(pchain.chain_count)
	        && pchain.triggering_effect->handler->is_affect_by_effect(core.reason_effect)) {
		core.current_chain[chaincount - 1].flag |= CHAIN_DISABLE_EFFECT | CHAIN_NEGATED;
		core.current_chain[chaincount - 1].disable_reason = core.reason_effect;
		core.current_chain[chaincount - 1].disable_player = core.reason_player;
		pduel->write_buffer8(MSG_CHAIN_DISABLED);
		pduel->write_buffer8(chaincount);
		return TRUE;
	}
	return FALSE;
}
void field::change_chain_effect(uint8 chaincount, int32 rep_op) {
	if(core.current_chain.size() == 0)
		return;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	core.current_chain[chaincount - 1].replace_op = rep_op;
}
void field::change_target(uint8 chaincount, group* targets) {
	if(core.current_chain.size() == 0)
		return;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	group* ot = core.current_chain[chaincount - 1].target_cards;
	effect* te = core.current_chain[chaincount - 1].triggering_effect;
	if(ot) {
		card_set::iterator cit;
		for(cit = ot->container.begin(); cit != ot->container.end(); ++cit)
			(*cit)->release_relation(te);
		ot->container = targets->container;
		for(cit = ot->container.begin(); cit != ot->container.end(); ++cit)
			(*cit)->create_relation(te);
		if(te->flag & EFFECT_FLAG_CARD_TARGET) {
			for(cit = ot->container.begin(); cit != ot->container.end(); ++cit) {
				if((*cit)->current.location & 0x30)
					move_card((*cit)->current.controler, (*cit), (*cit)->current.location, 0);
				pduel->write_buffer8(MSG_BECOME_TARGET);
				pduel->write_buffer8(1);
				pduel->write_buffer32((*cit)->get_info_location());
			}
		}
	}
}
void field::change_target_player(uint8 chaincount, uint8 playerid) {
	if(core.current_chain.size() == 0)
		return;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	core.current_chain[chaincount - 1].target_player = playerid;
}
void field::change_target_param(uint8 chaincount, int32 param) {
	if(core.current_chain.size() == 0)
		return;
	if(chaincount > core.current_chain.size() || chaincount < 1)
		chaincount = core.current_chain.size();
	core.current_chain[chaincount - 1].target_param = param;
}
void field::remove_counter(uint32 reason, card* pcard, uint32 rplayer, uint32 s, uint32 o, uint32 countertype, uint32 count) {
	add_process(PROCESSOR_REMOVE_COUNTER, 0, (effect*) reason, (group*)pcard, (rplayer << 16) + (s << 8) + o, countertype + (count << 16));
}
void field::remove_overlay_card(uint32 reason, card* pcard, uint32 rplayer, uint32 s, uint32 o, uint16 min, uint16 max) {
	add_process(PROCESSOR_REMOVEOL_S, 0, (effect*) reason, (group*)pcard, (rplayer << 16) + (s << 8) + o, (max << 16) + min);
}
void field::get_control(effect* reason_effect, uint32 reason_player, card* pcard, uint32 playerid, uint32 reset_phase, uint32 reset_count) {
	add_process(PROCESSOR_GET_CONTROL, 0, reason_effect, (group*)pcard, 0, (reason_player << 24) + (playerid << 16) + (reset_phase << 8) + reset_count);
}
void field::swap_control(effect* reason_effect, uint32 reason_player, card* pcard1, card* pcard2, uint32 reset_phase, uint32 reset_count) {
	add_process(PROCESSOR_SWAP_CONTROL, 0, reason_effect, (group*)pcard1, (ptr)pcard2, (reason_player << 24) + (reset_phase << 8) + reset_count);
}
void field::equip(uint32 equip_player, card* equip_card, card* target, uint32 up) {
	add_process(PROCESSOR_EQUIP, 0, 0, (group*)target, (ptr)equip_card, equip_player + (up << 16));
}
void field::draw(effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 count) {
	add_process(PROCESSOR_DRAW, 0, reason_effect, 0, reason, (reason_player << 28) + (playerid << 24) + (count & 0xffffff));
}
void field::damage(effect* reason_effect, uint32 reason, uint32 reason_player, card* pcard, uint32 playerid, uint32 amount) {
	if(reason & REASON_BATTLE)
		add_process(PROCESSOR_DAMAGE, 0, (effect*)pcard, 0, reason, (reason_player << 28) + (playerid << 24) + (amount & 0xffffff));
	else
		add_process(PROCESSOR_DAMAGE, 0, reason_effect, 0, reason, (reason_player << 28) + (playerid << 24) + (amount & 0xffffff));
}
void field::recover(effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 amount) {
	add_process(PROCESSOR_RECOVER, 0, reason_effect, 0, reason, (reason_player << 28) + (playerid << 24) + (amount & 0xffffff));
}
void field::summon(uint32 sumplayer, card* target, effect* proc, uint32 ignore_count) {
	add_process(PROCESSOR_SUMMON_RULE, 0, proc, (group*)target, sumplayer, ignore_count);
}
void field::special_summon_rule(uint32 sumplayer, card* target) {
	add_process(PROCESSOR_SPSUMMON_RULE, 0, 0, (group*)target, sumplayer, 0);
}
void field::special_summon(card_set* target, uint32 sumtype, uint32 sumplayer, uint32 playerid, uint32 nocheck, uint32 nolimit, uint32 positions) {
	if((positions & POS_FACEDOWN) && is_player_affected_by_effect(sumplayer, EFFECT_DEVINE_LIGHT))
		positions = (positions & POS_FACEUP) | (positions >> 1);
	card_set::iterator cit;
	for(cit = target->begin(); cit != target->end(); ++cit) {
		(*cit)->temp.reason = (*cit)->current.reason;
		(*cit)->temp.reason_effect = (*cit)->current.reason_effect;
		(*cit)->temp.reason_player = (*cit)->current.reason_player;
		(*cit)->summon_type = (sumtype & 0xfffffff) | SUMMON_TYPE_SPECIAL;
		(*cit)->summon_player = sumplayer;
		(*cit)->current.reason = REASON_SPSUMMON;
		(*cit)->current.reason_effect = core.reason_effect;
		(*cit)->current.reason_player = core.reason_player;
		(*cit)->operation_param = (playerid << 24) + (nocheck << 16) + (nolimit << 8) + positions;
	}
	group* pgroup = pduel->new_group();
	pgroup->is_readonly = TRUE;
	pgroup->container = *target;
	add_process(PROCESSOR_SPSUMMON, 0, core.reason_effect, pgroup, core.reason_player, 0);
}
void field::special_summon_step(card* target, uint32 sumtype, uint32 sumplayer, uint32 playerid, uint32 nocheck, uint32 nolimit, uint32 positions) {
	if((positions & POS_FACEDOWN) && is_player_affected_by_effect(sumplayer, EFFECT_DEVINE_LIGHT))
		positions = (positions & POS_FACEUP) | (positions >> 1);
	target->temp.reason = target->current.reason;
	target->temp.reason_effect = target->current.reason_effect;
	target->temp.reason_player = target->current.reason_player;
	target->summon_type = (sumtype & 0xfffffff) | SUMMON_TYPE_SPECIAL;
	target->summon_player = sumplayer;
	target->current.reason = REASON_SPSUMMON;
	target->current.reason_effect = core.reason_effect;
	target->current.reason_player = core.reason_player;
	target->operation_param = (playerid << 24) + (nocheck << 16) + (nolimit << 8) + positions;
	add_process(PROCESSOR_SPSUMMON_STEP, 0, core.reason_effect, 0, 0, (ptr)target);
}
void field::special_summon_complete(effect* reason_effect, uint8 reason_player) {
	if(core.special_summoning.size() == 0)
		return;
	group* ng = pduel->new_group();
	ng->container = core.special_summoning;
	ng->is_readonly = TRUE;
	core.special_summoning.clear();
	add_process(PROCESSOR_SPSUMMON_COMP_S, 0, reason_effect, ng, reason_player, 0);
}
void field::destroy(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence) {
	card_set::iterator cit;
	uint32 p;
	for(cit = targets->begin(); cit != targets->end();) {
		if((*cit)->is_status(STATUS_DESTROY_CONFIRMED)) {
			targets->erase(cit++);
			continue;
		}
		(*cit)->temp.reason = (*cit)->current.reason;
		(*cit)->temp.reason_effect = (*cit)->current.reason_effect;
		(*cit)->temp.reason_player = (*cit)->current.reason_player;
		(*cit)->current.reason = reason;
		(*cit)->current.reason_effect = reason_effect;
		(*cit)->current.reason_player = reason_player;
		p = playerid;
		if(!(destination & (LOCATION_HAND + LOCATION_DECK + LOCATION_REMOVED)))
			destination = LOCATION_GRAVE;
		if(destination && p == PLAYER_NONE)
			p = (*cit)->owner;
		if(destination & (LOCATION_GRAVE + LOCATION_REMOVED))
			p = (*cit)->owner;
		(*cit)->set_status(STATUS_DESTROY_CONFIRMED, TRUE);
		(*cit)->operation_param = (POS_FACEUP << 24) + (p << 16) + (destination << 8) + sequence;
		++cit;
	}
	group* ng = pduel->new_group();
	ng->container = *targets;
	ng->is_readonly = TRUE;
	add_process(PROCESSOR_DESTROY, 0, reason_effect, ng, reason, reason_player);
}
void field::destroy(card* target, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence) {
	card_set tset;
	tset.insert(target);
	destroy(&tset, reason_effect, reason, reason_player, playerid, destination, sequence);
}
void field::release(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player) {
	card_set::iterator cit;
	for(cit = targets->begin(); cit != targets->end(); ++cit) {
		(*cit)->temp.reason = (*cit)->current.reason;
		(*cit)->temp.reason_effect = (*cit)->current.reason_effect;
		(*cit)->temp.reason_player = (*cit)->current.reason_player;
		(*cit)->current.reason = reason;
		(*cit)->current.reason_effect = reason_effect;
		(*cit)->current.reason_player = reason_player;
		(*cit)->operation_param = (POS_FACEUP << 24) + ((uint32)((*cit)->owner) << 16) + (LOCATION_GRAVE << 8);
	}
	group* ng = pduel->new_group();
	ng->container = *targets;
	ng->is_readonly = TRUE;
	add_process(PROCESSOR_RELEASE, 0, reason_effect, ng, reason, reason_player);
}
void field::release(card* target, effect* reason_effect, uint32 reason, uint32 reason_player) {
	card_set tset;
	tset.insert(target);
	release(&tset, reason_effect, reason, reason_player);
}
void field::send_to(card_set* targets, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence, uint32 position) {
	card_set::iterator cit;
	if(!(destination & (LOCATION_HAND + LOCATION_DECK + LOCATION_GRAVE + LOCATION_REMOVED)))
		return;
	uint32 p, pos;
	for(cit = targets->begin(); cit != targets->end(); ++cit) {
		(*cit)->temp.reason = (*cit)->current.reason;
		(*cit)->temp.reason_effect = (*cit)->current.reason_effect;
		(*cit)->temp.reason_player = (*cit)->current.reason_player;
		(*cit)->current.reason = reason;
		(*cit)->current.reason_effect = reason_effect;
		(*cit)->current.reason_player = reason_player;
		p = playerid;
		if(destination & (LOCATION_GRAVE + LOCATION_REMOVED) || p == PLAYER_NONE)
			p = (*cit)->owner;
		if(destination != LOCATION_REMOVED)
			pos = POS_FACEUP;
		else if(position == 0)
			pos = (*cit)->current.position;
		else pos = position;
		(*cit)->operation_param = (pos << 24) + (p << 16) + (destination << 8) + (sequence);
	}
	group* ng = pduel->new_group();
	ng->container = *targets;
	ng->is_readonly = TRUE;
	add_process(PROCESSOR_SENDTO, 0, reason_effect, ng, reason, reason_player);
}
void field::send_to(card* target, effect* reason_effect, uint32 reason, uint32 reason_player, uint32 playerid, uint32 destination, uint32 sequence, uint32 position) {
	card_set tset;
	tset.insert(target);
	send_to(&tset, reason_effect, reason, reason_player, playerid, destination, sequence, position);
}
void field::move_to_field(card* target, uint32 move_player, uint32 playerid, uint32 destination, uint32 positions, uint32 enable, uint32 ret) {
	if(!(destination & (LOCATION_MZONE + LOCATION_SZONE)) || !positions)
		return;
	if(destination == target->current.location && playerid == target->current.controler)
		return;
	target->operation_param = (move_player << 24) + (playerid << 16) + (destination << 8) + positions;
	add_process(PROCESSOR_MOVETOFIELD, 0, 0, (group*)target, enable, ret);
}
void field::change_position(card_set* targets, effect* reason_effect, uint32 reason_player, uint32 au, uint32 ad, uint32 du, uint32 dd, uint32 noflip, uint32 enable) {
	group* ng = pduel->new_group();
	ng->container = *targets;
	ng->is_readonly = TRUE;
	card_set::iterator cit;
	for(cit = targets->begin(); cit != targets->end(); ++cit) {
		if((*cit)->current.position == POS_FACEUP_ATTACK) (*cit)->operation_param = au;
		else if((*cit)->current.position == POS_FACEDOWN_DEFENCE) (*cit)->operation_param = dd;
		else if((*cit)->current.position == POS_FACEUP_DEFENCE) (*cit)->operation_param = du;
		else (*cit)->operation_param = ad;
		if(noflip)
			(*cit)->operation_param |= NO_FLIP_EFFECT;
	}
	add_process(PROCESSOR_CHANGEPOS, 0, reason_effect, ng, reason_player, enable);
}
void field::change_position(card* target, effect* reason_effect, uint32 reason_player, uint32 npos, uint32 noflip, uint32 enable) {
	group* ng = pduel->new_group();
	ng->container.insert(target);
	ng->is_readonly = TRUE;
	target->operation_param = npos;
	if(noflip)
		target->operation_param |= NO_FLIP_EFFECT;
	add_process(PROCESSOR_CHANGEPOS, 0, reason_effect, ng, reason_player, enable);
}
int32 field::draw(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 playerid, uint32 count) {
	switch(step) {
	case 0: {
		card* pcard;
		card_set cset;
		card_set::iterator cit;
		card_vector cv;
		uint32 drawed = 0;
		if(!(reason & REASON_RULE) && !is_player_can_draw(playerid)) {
			returns.ivalue[0] = 0;
			return TRUE;
		}
		core.overdraw[playerid] = FALSE;
		for(uint32 i = 0; i < count; ++i) {
			if(player[playerid].list_main.size() == 0) {
				core.overdraw[playerid] = TRUE;
				break;
			}
			drawed ++;
			pcard = *(player[playerid].list_main.rbegin());
			pcard->enable_field_effect(FALSE);
			pcard->cancel_field_effect();
			player[playerid].list_main.pop_back();
			pcard->previous.controler = pcard->current.controler;
			pcard->previous.location = pcard->current.location;
			pcard->previous.sequence = pcard->current.sequence;
			pcard->previous.position = pcard->current.position;
			pcard->current.controler = PLAYER_NONE;
			pcard->current.reason_effect = reason_effect;
			pcard->current.reason_player = reason_player;
			pcard->current.reason = reason | REASON_DRAW;
			pcard->current.location = 0;
			add_card(playerid, pcard, LOCATION_HAND, 0);
			pcard->enable_field_effect(TRUE);
			cv.push_back(pcard);
			cset.insert(pcard);
		}
		core.hint_timing[playerid] |= TIMING_DRAW + TIMING_TOHAND;
		adjust_instant();
		core.units.begin()->arg2 = (core.units.begin()->arg2 & 0xff000000) + drawed;
		if(drawed > 0) {
			pduel->write_buffer8(MSG_DRAW);
			pduel->write_buffer8(playerid);
			pduel->write_buffer8(drawed);
			for(uint32 i = 0; i < drawed; ++i)
				pduel->write_buffer32(cv[i]->data.code);
		}
		if(cset.size()) {
			for(cit = cset.begin(); cit != cset.end(); ++cit)
				raise_single_event((*cit), EVENT_TO_HAND, reason_effect, reason, reason_player, playerid, 0);
			process_single_event();
			raise_event(&cset, EVENT_DRAW, reason_effect, reason, reason_player, playerid, drawed);
			raise_event(&cset, EVENT_TO_HAND, reason_effect, reason, reason_player, playerid, drawed);
			process_instant_event();
		}
		return FALSE;
	}
	case 1: {
		core.operated_set.clear();
		auto clit = player[playerid].list_hand.rbegin();
		for(uint32 i = 0; i < count; ++i) {
			core.operated_set.insert(*clit);
			clit++;
		}
		returns.ivalue[0] = count;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::damage(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, card* reason_card, uint8 playerid, uint32 amount) {
	switch(step) {
	case 0: {
		effect_set eset;
		returns.ivalue[0] = amount;
		if(amount <= 0)
			return TRUE;
		if(!(reason & REASON_RDAMAGE)) {
			filter_player_effect(playerid, EFFECT_REVERSE_DAMAGE, &eset);
			for(int32 i = 0; i < eset.count; ++i) {
				pduel->lua->add_param(reason_effect, PARAM_TYPE_EFFECT);
				pduel->lua->add_param(reason, PARAM_TYPE_INT);
				pduel->lua->add_param(reason_player, PARAM_TYPE_INT);
				pduel->lua->add_param(reason_card, PARAM_TYPE_CARD);
				if(eset[i]->check_value_condition(4)) {
					recover(reason_effect, (reason & 0x18000) | 0x8040, reason_player, playerid, amount);
					core.units.begin()->step = 1;
					return FALSE;
				}
			}
		}
		uint32 val = amount;
		eset.clear();
		filter_player_effect(playerid, EFFECT_CHANGE_DAMAGE, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(reason_effect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(val, PARAM_TYPE_INT);
			pduel->lua->add_param(reason, PARAM_TYPE_INT);
			pduel->lua->add_param(reason_player, PARAM_TYPE_INT);
			pduel->lua->add_param(reason_card, PARAM_TYPE_CARD);
			val = eset[i]->get_value(5);
			returns.ivalue[0] = val;
			if(val == 0)
				return TRUE;
		}
		eset.clear();
		filter_player_effect(playerid, EFFECT_REFLECT_DAMAGE, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(reason_effect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(val, PARAM_TYPE_INT);
			pduel->lua->add_param(reason, PARAM_TYPE_INT);
			pduel->lua->add_param(reason_player, PARAM_TYPE_INT);
			pduel->lua->add_param(reason_card, PARAM_TYPE_CARD);
			if(eset[i]->check_value_condition(5)) {
				playerid = 1 - playerid;
				core.units.begin()->step = 1;
				break;
			}
		}
		core.hint_timing[playerid] |= TIMING_DAMAGE;
		player[playerid].lp -= val;
		pduel->write_buffer8(MSG_DAMAGE);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(val);
		core.units.begin()->arg2 = (core.units.begin()->arg2 & 0xff000000) + (val & 0xffffff);
		raise_event(reason_card, EVENT_DAMAGE, reason_effect, reason, reason_player, playerid, val);
		process_instant_event();
		return FALSE;
	}
	case 1: {
		returns.ivalue[0] = amount;
		return TRUE;
	}
	case 2: {
		returns.ivalue[0] = 0;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::recover(uint16 step, effect* reason_effect, uint32 reason, uint8 reason_player, uint8 playerid, uint32 amount) {
	switch(step) {
	case 0: {
		effect_set eset;
		returns.ivalue[0] = amount;
		if(amount <= 0)
			return TRUE;
		if(!(reason & REASON_RRECOVER)) {
			filter_player_effect(playerid, EFFECT_REVERSE_RECOVER, &eset);
			for(int32 i = 0; i < eset.count; ++i) {
				pduel->lua->add_param(reason_effect, PARAM_TYPE_EFFECT);
				pduel->lua->add_param(reason, PARAM_TYPE_INT);
				pduel->lua->add_param(reason_player, PARAM_TYPE_INT);
				if(eset[i]->check_value_condition(3)) {
					damage(reason_effect, (reason & 0x18000) | 0x10040, reason_player, 0, playerid, amount);
					core.units.begin()->step = 1;
					return FALSE;
				}
			}
		}
		core.hint_timing[playerid] |= TIMING_RECOVER;
		player[playerid].lp += amount;
		pduel->write_buffer8(MSG_RECOVER);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(amount);
		raise_event((card*)0, EVENT_RECOVER, reason_effect, reason, reason_player, playerid, amount);
		process_instant_event();
		return FALSE;
	}
	case 1: {
		returns.ivalue[0] = amount;
		return TRUE;
	}
	case 2: {
		returns.ivalue[0] = 0;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::pay_lp_cost(uint32 step, uint8 playerid, uint32 cost) {
	switch(step) {
	case 0: {
		effect_set eset;
		int32 val = cost;
		filter_player_effect(playerid, EFFECT_LPCOST_CHANGE, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(core.reason_effect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(playerid, PARAM_TYPE_INT);
			pduel->lua->add_param(val, PARAM_TYPE_INT);
			val = eset[i]->get_value(3);
			if(val <= 0)
				return TRUE;
		}
		core.units.begin()->arg2 = val;
		event e;
		e.event_cards = 0;
		e.event_player = playerid;
		e.event_value = val;
		e.reason = 0;
		e.reason_effect = core.reason_effect;
		e.reason_player = playerid;
		core.select_options.clear();
		core.select_effects.clear();
		if(val < player[playerid].lp) {
			core.select_options.push_back(11);
			core.select_effects.push_back(0);
		}
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_LPCOST_REPLACE);
		effect* peffect;
		for (; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(peffect->is_activateable(peffect->get_handler_player(), e)) {
				core.select_options.push_back(peffect->description);
				core.select_effects.push_back(peffect);
			}
		}
		if(core.select_options.size() == 0)
			return TRUE;
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else if(core.select_effects[0] == 0 && core.select_effects.size() == 2)
			add_process(PROCESSOR_SELECT_EFFECTYN, 0, 0, (group*)core.select_effects[1]->handler, playerid, 0);
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, playerid, 0);
		return FALSE;
	}
	case 1: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		if(!peffect) {
			player[playerid].lp -= cost;
			pduel->write_buffer8(MSG_PAY_LPCOST);
			pduel->write_buffer8(playerid);
			pduel->write_buffer32(cost);
			return TRUE;
		}
		event e;
		e.event_cards = 0;
		e.event_player = playerid;
		e.event_value = cost;
		e.reason = 0;
		e.reason_effect = core.reason_effect;
		e.reason_player = playerid;
		core.sub_solving_event.push_back(e);
		add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, peffect, 0, playerid, 0);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::remove_counter(uint16 step, uint32 reason, card* pcard, uint8 rplayer, uint8 s, uint8 o, uint16 countertype, uint16 count) {
	switch(step) {
	case 0: {
		core.select_options.clear();
		core.select_effects.clear();
		if((pcard && pcard->get_counter(countertype) >= count) || (!pcard && get_field_counter(rplayer, s, o, countertype))) {
			core.select_options.push_back(10);
			core.select_effects.push_back(0);
		}
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_RCOUNTER_REPLACE + countertype);
		effect* peffect;
		event e;
		e.event_cards = 0;
		e.event_player = rplayer;
		e.event_value = count;
		e.reason = reason;
		e.reason_effect = core.reason_effect;
		e.reason_player = rplayer;
		for (; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(peffect->is_activateable(peffect->get_handler_player(), e)) {
				core.select_options.push_back(peffect->description);
				core.select_effects.push_back(peffect);
			}
		}
		returns.ivalue[0] = 0;
		if(core.select_options.size() == 0)
			return TRUE;
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else if(core.select_effects[0] == 0 && core.select_effects.size() == 2)
			add_process(PROCESSOR_SELECT_EFFECTYN, 0, 0, (group*)core.select_effects[1]->handler, rplayer, 0);
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, rplayer, 0);
		return FALSE;
	}
	case 1: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		if(peffect) {
			event e;
			e.event_cards = 0;
			e.event_player = rplayer;
			e.event_value = count;
			e.reason = reason;
			e.reason_effect = core.reason_effect;
			e.reason_player = rplayer;
			core.sub_solving_event.push_back(e);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, peffect, 0, rplayer, 0);
			core.units.begin()->step = 3;
			return FALSE;
		}
		if(pcard) {
			returns.ivalue[0] = pcard->remove_counter(countertype, count);
			core.units.begin()->step = 2;
			return FALSE;
		}
		card* pcard;
		core.select_cards.clear();
		uint8 fc = s;
		uint8 fp = rplayer;
		for(int p = 0; p < 2; ++p) {
			if(fc) {
				for(uint32 j = 0; j < 5; ++j) {
					pcard = player[fp].list_mzone[j];
					if(pcard && pcard->get_counter(countertype)) {
						core.select_cards.push_back(pcard);
						pcard->operation_param = pcard->get_counter(countertype);
					}
				}
				for(uint32 j = 0; j < 6; ++j) {
					pcard = player[fp].list_szone[j];
					if(pcard && pcard->get_counter(countertype)) {
						core.select_cards.push_back(pcard);
						pcard->operation_param = pcard->get_counter(countertype);
					}
				}
			}
			fp = 1 - fp;
			fc = o;
		}
		add_process(PROCESSOR_SELECT_COUNTER, 0, 0, 0, rplayer, countertype + (((uint32)count) << 16));
		return FALSE;
	}
	case 2: {
		for(uint32 i = 0; i < core.select_cards.size(); ++i)
			if(returns.bvalue[i] > 0)
				core.select_cards[i]->remove_counter(countertype, returns.bvalue[i]);
		return FALSE;
	}
	case 3: {
		raise_event((card*)0, EVENT_REMOVE_COUNTER + countertype, core.reason_effect, reason, rplayer, rplayer, count);
		process_instant_event();
		return FALSE;
	}
	case 4: {
		returns.ivalue[0] = 1;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::remove_overlay_card(uint16 step, uint32 reason, card* pcard, uint8 rplayer, uint8 s, uint8 o, uint16 min, uint16 max) {
	switch(step) {
	case 0: {
		core.select_options.clear();
		core.select_effects.clear();
		if((pcard && pcard->xyz_materials.size() >= min) || (!pcard && get_overlay_count(rplayer, s, o) >= min)) {
			core.select_options.push_back(12);
			core.select_effects.push_back(0);
		}
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_OVERLAY_REMOVE_REPLACE);
		effect* peffect;
		event e;
		e.event_cards = 0;
		e.event_player = rplayer;
		e.event_value = min;
		e.reason = reason;
		e.reason_effect = core.reason_effect;
		e.reason_player = rplayer;
		for (; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(peffect->is_activateable(peffect->get_handler_player(), e)) {
				core.select_options.push_back(peffect->description);
				core.select_effects.push_back(peffect);
			}
		}
		returns.ivalue[0] = 0;
		if(core.select_options.size() == 0)
			return TRUE;
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else if(core.select_effects[0] == 0 && core.select_effects.size() == 2)
			add_process(PROCESSOR_SELECT_EFFECTYN, 0, 0, (group*)core.select_effects[1]->handler, rplayer, 0);
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, rplayer, 0);
		return FALSE;
	}
	case 1: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		if(peffect) {
			event e;
			e.event_cards = 0;
			e.event_player = rplayer;
			e.event_value = min + (max << 16);;
			e.reason = reason;
			e.reason_effect = core.reason_effect;
			e.reason_player = rplayer;
			core.sub_solving_event.push_back(e);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, peffect, 0, rplayer, 0);
			core.units.begin()->step = 3;
			return FALSE;
		}
		pduel->game_field->core.select_cards.clear();
		if(pcard) {
			for(auto cit = pcard->xyz_materials.begin(); cit != pcard->xyz_materials.end(); ++cit)
				pduel->game_field->core.select_cards.push_back(*cit);
		} else {
			card_set cset;
			pduel->game_field->get_overlay_group(rplayer, s, o, &cset);
			for(auto cit = cset.begin(); cit != cset.end(); ++cit)
				pduel->game_field->core.select_cards.push_back(*cit);
		}
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_SELECTMSG);
		pduel->write_buffer8(rplayer);
		pduel->write_buffer32(519);
		add_process(PROCESSOR_SELECT_CARD, 0, 0, 0, rplayer, min + (max << 16));
		return FALSE;
	}
	case 2: {
		card_set cset;
		for(int32 i = 0; i < returns.bvalue[0]; ++i)
			cset.insert(core.select_cards[returns.bvalue[i + 1]]);
		send_to(&cset, core.reason_effect, reason, rplayer, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		return FALSE;
	}
	case 3: {
		return FALSE;
	}
	case 4: {
		returns.ivalue[0] = 1;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::get_control(uint16 step, effect * reason_effect, uint8 reason_player, card * pcard, uint8 playerid, uint8 reset_phase, uint8 reset_count) {
	switch(step) {
	case 0: {
		returns.ivalue[0] = 0;
		if(pcard->overlay_target)
			return TRUE;
		if(pcard->current.controler == PLAYER_NONE || pcard->current.controler == playerid)
			return TRUE;
		if(pcard->current.location != LOCATION_MZONE)
			return TRUE;
		if(get_useable_count(playerid, LOCATION_MZONE) == 0)
			return TRUE;
		if(pcard->data.type & TYPE_TRAPMONSTER && get_useable_count(playerid, LOCATION_SZONE) == 0)
			return TRUE;
		if(!pcard->is_capable_change_control())
			return TRUE;
		if(!pcard->is_affect_by_effect(reason_effect))
			return TRUE;
		pcard->filter_disable_related_cards();
		move_to_field(pcard, playerid, playerid, LOCATION_MZONE, pcard->current.position);
		return FALSE;
	}
	case 1: {
		set_control(pcard, playerid, reset_phase, reset_count);
		pcard->reset(RESET_CONTROL, RESET_EVENT);
		pcard->filter_disable_related_cards();
		adjust_instant();
		return FALSE;
	}
	case 2: {
		raise_single_event(pcard, EVENT_CONTROL_CHANGED, reason_effect, 0, reason_player, playerid, 0);
		process_single_event();
		raise_event(pcard, EVENT_CONTROL_CHANGED, reason_effect, 0, reason_player, playerid, 0);
		process_instant_event();
		return FALSE;
	}
	case 3: {
		returns.ivalue[0] = 1;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::swap_control(uint16 step, effect * reason_effect, uint8 reason_player, card * pcard1, card * pcard2, uint8 reset_phase, uint8 reset_count) {
	switch(step) {
	case 0: {
		uint8 p1 = pcard1->current.controler, p2 = pcard2->current.controler;
		uint8 l1 = pcard1->current.location, l2 = pcard2->current.location;
		uint8 s1 = pcard1->current.sequence, s2 = pcard2->current.sequence;
		returns.ivalue[0] = 0;
		if(pcard1->overlay_target || pcard2->overlay_target)
			return TRUE;
		if(p1 == p2 || p1 == PLAYER_NONE || p2 == PLAYER_NONE)
			return TRUE;
		if(l1 != LOCATION_MZONE || l2 != LOCATION_MZONE)
			return TRUE;
		if(!pcard1->is_capable_change_control() || !pcard2->is_capable_change_control())
			return TRUE;
		if(!pcard1->is_affect_by_effect(reason_effect) || !pcard2->is_affect_by_effect(reason_effect))
			return TRUE;
		pcard1->filter_disable_related_cards();
		pcard2->filter_disable_related_cards();
		remove_card(pcard1);
		remove_card(pcard2);
		add_card(p2, pcard1, l2, s2);
		add_card(p1, pcard2, l1, s1);
		set_control(pcard1, p2, reset_phase, reset_count);
		set_control(pcard2, p1, reset_phase, reset_count);
		pcard1->reset(RESET_CONTROL, RESET_EVENT);
		pcard1->reset(RESET_CONTROL, RESET_EVENT);
		pcard1->filter_disable_related_cards();
		pcard2->filter_disable_related_cards();
		adjust_instant();
		return FALSE;
	}
	case 1: {
		pduel->write_buffer8(MSG_SWAP);
		pduel->write_buffer32(pcard1->data.code);
		pduel->write_buffer8(pcard2->current.controler);
		pduel->write_buffer8(pcard2->current.location);
		pduel->write_buffer8(pcard2->current.sequence);
		pduel->write_buffer8(pcard2->current.position);
		pduel->write_buffer32(pcard2->data.code);
		pduel->write_buffer8(pcard1->current.controler);
		pduel->write_buffer8(pcard1->current.location);
		pduel->write_buffer8(pcard1->current.sequence);
		pduel->write_buffer8(pcard1->current.position);
		raise_single_event(pcard1, EVENT_CONTROL_CHANGED, reason_effect, 0, reason_player, pcard1->current.controler, 0);
		raise_single_event(pcard2, EVENT_CONTROL_CHANGED, reason_effect, 0, reason_player, pcard2->current.controler, 0);
		process_single_event();
		card_set cset;
		cset.insert(pcard1);
		cset.insert(pcard2);
		raise_event(&cset, EVENT_CONTROL_CHANGED, reason_effect, 0, reason_player, 0, 0);
		process_instant_event();
		return FALSE;
	}
	case 2: {
		returns.ivalue[0] = 1;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::equip(uint16 step, uint8 equip_player, card * equip_card, card * target, uint32 up) {
	switch(step) {
	case 0: {
		returns.ivalue[0] = FALSE;
		if(!equip_card->is_affect_by_effect(core.reason_effect))
			return TRUE;
		if(equip_card == target || target->current.location != LOCATION_MZONE)
			return TRUE;
		if(equip_card->equiping_target) {
			equip_card->unequip();
			return FALSE;
		}
		if(equip_card->current.location == LOCATION_SZONE) {
			if(up && equip_card->is_position(POS_FACEDOWN))
				change_position(equip_card, 0, equip_player, POS_FACEUP, 0);
			return FALSE;
		}
		if(get_useable_count(equip_player, LOCATION_SZONE) == 0)
			return TRUE;
		equip_card->enable_field_effect(FALSE);
		move_to_field(equip_card, equip_player, equip_player, LOCATION_SZONE, (up || equip_card->is_position(POS_FACEUP)) ? POS_FACEUP : POS_FACEDOWN);
		return FALSE;
	}
	case 1: {
		equip_card->equip(target);
		if(!(equip_card->data.type & TYPE_EQUIP)) {
			effect* peffect = pduel->new_effect();
			peffect->owner = equip_card;
			peffect->handler = equip_card;
			peffect->type = EFFECT_TYPE_SINGLE;
			peffect->code = EFFECT_CHANGE_TYPE;
			if(equip_card->data.type & TYPE_TRAP)
				peffect->value = TYPE_EQUIP + equip_card->data.type;
			else if(equip_card->data.type & TYPE_UNION)
				peffect->value = TYPE_EQUIP + TYPE_SPELL + TYPE_UNION;
			else
				peffect->value = TYPE_EQUIP + TYPE_SPELL;
			peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
			peffect->reset_flag = RESET_EVENT + 0x1fe0000;
			equip_card->add_effect(peffect);
		}
		if(equip_card->is_position(POS_FACEUP))
			equip_card->enable_field_effect(TRUE);
		equip_card->effect_target_cards.insert(target);
		target->effect_target_owner.insert(equip_card);
		adjust_instant();
		raise_event(equip_card, EVENT_EQUIP, core.reason_effect, 0, core.reason_player, PLAYER_NONE, 0);
		return FALSE;
	}
	case 2: {
		returns.ivalue[0] = TRUE;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::summon(uint16 step, uint8 sumplayer, card* target, effect* proc, uint8 ignore_count) {
	switch(step) {
	case 0: {
		if(!(target->data.type & TYPE_MONSTER))
			return TRUE;
		if(target->current.location == LOCATION_MZONE) {
			if(target->is_position(POS_FACEDOWN))
				return TRUE;
			if(!ignore_count && core.summon_count[sumplayer] >= get_summon_count_limit(sumplayer))
				return TRUE;
			if(!target->is_affected_by_effect(EFFECT_DUAL_SUMMONABLE))
				return TRUE;
			if(target->is_affected_by_effect(EFFECT_DUAL_STATUS))
				return TRUE;
			if(!is_player_can_summon(SUMMON_TYPE_DUAL, sumplayer, target))
				return TRUE;
		}
		if(target->is_affected_by_effect(EFFECT_CANNOT_SUMMON))
			return TRUE;
		core.summon_depth++;
		target->material_cards.clear();
		effect_set eset;
		target->filter_effect(EFFECT_SUMMON_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(sumplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, sumplayer, 0);
			}
		}
		if(target->current.location == LOCATION_MZONE)
			core.units.begin()->step = 6;
		return FALSE;
	}
	case 1: {
		effect_set eset;
		int32 res = target->filter_summon_procedure(sumplayer, &eset, ignore_count);
		if(proc && res < 0)
			return TRUE;
		if(proc) {
			core.units.begin()->step = 3;
			return FALSE;
		}
		if(res == -1) {
			core.units.begin()->peffect = eset[0];
			core.units.begin()->step = 3;
			return FALSE;
		}
		core.select_effects.clear();
		core.select_options.clear();
		if(res) {
			core.select_effects.push_back(0);
			core.select_options.push_back(1);
		}
		for(int32 i = 0; i < eset.count; ++i) {
			core.select_effects.push_back(eset[i]);
			core.select_options.push_back(eset[i]->description);
		}
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, sumplayer, 0);
		return FALSE;
	}
	case 2: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		if(peffect) {
			core.units.begin()->peffect = peffect;
			core.units.begin()->step = 3;
			return FALSE;
		}
		core.select_cards.clear();
		uint32 required = target->get_summon_tribute_count();
		uint32 min = required & 0xffff;
		uint32 max = required >> 16;
		uint32 adv = is_player_can_summon(SUMMON_TYPE_ADVANCE, sumplayer, target);
		if(max == 0 || !adv) {
			returns.bvalue[0] = 0;
		} else {
			card_set cset;
			int32 rcount = get_summon_release_list(target, &cset);
			if(rcount == 0)
				returns.bvalue[0] = 0;
			else {
				for(card_set::iterator cit = cset.begin(); cit != cset.end(); ++cit)
					core.select_cards.push_back(*cit);
				if(min == 0 && get_useable_count(sumplayer, LOCATION_MZONE) > 0 ) {
					add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, sumplayer, 90);
					core.temp_var[0] = (void*)required;
					core.units.begin()->step = 19;
				} else
					add_process(PROCESSOR_SELECT_TRIBUTE, 0, 0, 0, sumplayer, required);
			}
		}
		return FALSE;
	}
	case 3: {
		if(returns.bvalue[0]) {
			card_set tributes;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				tributes.insert(core.select_cards[returns.bvalue[i + 1]]);
				core.select_cards[returns.bvalue[i + 1]]->current.reason_card = target;
			}
			target->set_material(&tributes);
			release(&tributes, 0, REASON_SUMMON | REASON_MATERIAL, sumplayer);
			if(tributes.size())
				target->summon_type = SUMMON_TYPE_NORMAL | SUMMON_TYPE_ADVANCE;
			else
				target->summon_type = SUMMON_TYPE_NORMAL;
			adjust_all();
		} else
			target->summon_type = SUMMON_TYPE_NORMAL;
		target->current.reason_effect = 0;
		target->current.reason_player = sumplayer;
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 4: {
		target->summon_type = (proc->get_value(target) & 0xfffffff) | SUMMON_TYPE_NORMAL;
		target->current.reason_effect = proc;
		target->current.reason_player = sumplayer;
		if(proc->operation) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			core.sub_solving_event.push_back(nil_event);
			add_process(PROCESSOR_EXECUTE_OPERATION, 0, proc, 0, sumplayer, 0);
		}
		return FALSE;
	}
	case 5: {
		core.summon_depth--;
		if(core.summon_depth)
			return TRUE;
		break_effect();
		core.temp_var[0] = 0;
		if(!ignore_count) {
			returns.ivalue[0] = FALSE;
			effect* pextra = target->is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT);
			if(pextra) {
				core.temp_var[0] = pextra;
				if(core.summon_count[sumplayer] < get_summon_count_limit(sumplayer))
					add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, sumplayer, 91);
				else
					returns.ivalue[0] = TRUE;
			}
		} else
			returns.ivalue[0] = TRUE;
		return FALSE;
	}
	case 6: {
		if(!returns.ivalue[0])
			core.summon_count[sumplayer]++;
		else if(core.temp_var[0]) {
			effect* pextra = (effect*)core.temp_var[0];
			pextra->get_value(target);
		}
		target->enable_field_effect(FALSE);
		if(is_player_affected_by_effect(sumplayer, EFFECT_DEVINE_LIGHT))
			move_to_field(target, sumplayer, sumplayer, LOCATION_MZONE, POS_FACEUP);
		else
			move_to_field(target, sumplayer, sumplayer, LOCATION_MZONE, POS_FACEUP_ATTACK);
		core.summoning_card = target;
		core.units.begin()->step = 8;
		return FALSE;
	}
	case 7: {
		core.summon_depth--;
		if(core.summon_depth)
			return TRUE;
		target->enable_field_effect(FALSE);
		target->current.reason_effect = 0;
		target->current.reason_player = sumplayer;
		effect* deffect = pduel->new_effect();
		deffect->owner = target;
		deffect->code = EFFECT_DUAL_STATUS;
		deffect->type = EFFECT_TYPE_SINGLE;
		deffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
		deffect->reset_flag = RESET_EVENT + 0x1fe0000;
		target->add_effect(deffect);
		core.temp_var[0] = 0;
		if(!ignore_count) {
			returns.ivalue[0] = FALSE;
			effect* pextra = target->is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT);
			if(pextra) {
				core.temp_var[0] = pextra;
				if(core.summon_count[sumplayer] < get_summon_count_limit(sumplayer))
					add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, sumplayer, 91);
				else
					returns.ivalue[0] = TRUE;
			}
		} else
			returns.ivalue[0] = TRUE;
		return FALSE;
	}
	case 8: {
		if(!returns.ivalue[0])
			core.summon_count[sumplayer]++;
		else if(core.temp_var[0]) {
			effect* pextra = (effect*)core.temp_var[0];
			pextra->get_value(target);
		}
		core.summoning_card = target;
		return FALSE;
	}
	case 9: {
		if(target->owner != sumplayer)
			set_control(target, sumplayer, 0, 0);
		core.phase_action = TRUE;
		target->current.reason = REASON_SUMMON;
		target->summon_player = sumplayer;
		if(core.current_chain.size() > 1 || target->is_affected_by_effect(EFFECT_CANNOT_DISABLE_SUMMON)) {
			core.units.begin()->step = 14;
			return FALSE;
		} else if(core.current_chain.size() == 0) {
			core.units.begin()->step = 9;
			return FALSE;
		}
		core.reserved = core.units.front();
		return TRUE;
	}
	case 10: {
		target->set_status(STATUS_SUMMONING, TRUE);
		target->set_status(STATUS_SUMMON_DISABLED, FALSE);
		pduel->write_buffer8(MSG_SUMMONING);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		core.summoning_card = 0;
		core.summon_state[sumplayer] = TRUE;
		core.normalsummon_state[sumplayer] = TRUE;
		raise_event(target, EVENT_SUMMON, proc, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, TRUE, TRUE);
		return FALSE;
	}
	case 11: {
		if(target->is_status(STATUS_SUMMONING)) {
			core.units.begin()->step = 14;
			return FALSE;
		}
		if(proc)
			remove_oath_effect(proc);
		adjust_instant();
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		return TRUE;
	}
	case 15: {
		if(proc) {
			oath_effects::iterator oeit;
			for(oeit = effects.oath.begin(); oeit != effects.oath.end(); ++oeit)
				if(oeit->second == proc)
					oeit->second = 0;
		}
		target->set_status(STATUS_SUMMONING, FALSE);
		target->enable_field_effect(TRUE);
		target->set_status(STATUS_SUMMON_TURN, TRUE);
		core.summoning_card = 0;
		return FALSE;
	}
	case 16: {
		pduel->write_buffer8(MSG_SUMMONED);
		adjust_instant();
		if(target->material_cards.size()) {
			for(auto mit = target->material_cards.begin(); mit != target->material_cards.end(); ++mit)
				raise_single_event(*mit, EVENT_BE_MATERIAL, proc, 0, sumplayer, sumplayer, 0);
		}
		process_single_event();
		return false;
	}
	case 17: {
		raise_single_event(target, EVENT_SUMMON_SUCCESS, proc, 0, sumplayer, sumplayer, 0);
		process_single_event();
		raise_event(target, EVENT_SUMMON_SUCCESS, proc, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		adjust_all();
		if(core.current_chain.size() == 0) {
			core.hint_timing[sumplayer] |= TIMING_SUMMON;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		}
		return TRUE;
	}
	case 20: {
		if(returns.ivalue[0])
			returns.bvalue[0] = 0;
		else
			add_process(PROCESSOR_SELECT_TRIBUTE, 0, 0, 0, sumplayer, (ptr)(core.temp_var[0]) + 1);
		core.units.begin()->step = 2;
		return false;
	}
	}
	return TRUE;
}
int32 field::flip_summon(uint16 step, uint8 sumplayer, card * target) {
	switch(step) {
	case 0: {
		if(target->current.location != LOCATION_MZONE)
			return TRUE;
		if(!(target->current.position & POS_FACEDOWN))
			return TRUE;
		effect_set eset;
		target->filter_effect(EFFECT_FLIPSUMMON_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(sumplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, sumplayer, 0);
			}
		}
		return FALSE;
	}
	case 1: {
		target->previous.position = target->current.position;
		target->current.position = POS_FACEUP_ATTACK;
		core.phase_action = TRUE;
		core.flipsummon_state[sumplayer] = TRUE;
		pduel->write_buffer8(MSG_FLIPSUMMONING);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		target->set_status(STATUS_SUMMONING, TRUE);
		target->set_status(STATUS_SUMMON_DISABLED, FALSE);
		raise_event(target, EVENT_FLIP_SUMMON, 0, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, TRUE, TRUE);
		return FALSE;
	}
	case 2: {
		if(target->is_status(STATUS_SUMMONING)) {
			return FALSE;
		}
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		return TRUE;
	}
	case 3: {
		target->set_status(STATUS_SUMMONING, FALSE);
		target->enable_field_effect(TRUE);
		target->set_status(STATUS_SUMMON_TURN, TRUE);
		return FALSE;
	}
	case 4: {
		pduel->write_buffer8(MSG_FLIPSUMMONED);
		adjust_instant();
		raise_single_event(target, EVENT_FLIP, 0, 0, sumplayer, sumplayer, 0);
		raise_single_event(target, EVENT_FLIP_SUMMON_SUCCESS, 0, 0, sumplayer, sumplayer, 0);
		raise_single_event(target, EVENT_CHANGE_POS, 0, 0, sumplayer, sumplayer, 0);
		process_single_event();
		raise_event(target, EVENT_FLIP, 0, 0, sumplayer, sumplayer, 0);
		raise_event(target, EVENT_FLIP_SUMMON_SUCCESS, 0, 0, sumplayer, sumplayer, 0);
		raise_event(target, EVENT_CHANGE_POS, 0, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		adjust_all();
		if(core.current_chain.size() == 0) {
			core.hint_timing[sumplayer] |= TIMING_FLIPSUMMON;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::mset(uint16 step, uint8 setplayer, card * target, effect * proc, uint8 ignore_count) {
	switch(step) {
	case 0: {
		if(target->is_status(STATUS_REVIVE_LIMIT))
			return TRUE;
		if(target->current.location != LOCATION_HAND)
			return TRUE;
		if(!(target->data.type & TYPE_MONSTER))
			return TRUE;
		if(target->is_affected_by_effect(EFFECT_CANNOT_MSET))
			return TRUE;
		effect_set eset;
		target->filter_effect(EFFECT_MSET_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(setplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, setplayer, 0);
			}
		}
		return FALSE;
	}
	case 1: {
		effect_set eset;
		target->material_cards.clear();
		int32 res = target->filter_set_procedure(setplayer, &eset, ignore_count);
		if(proc && res < 0)
			return TRUE;
		if(proc) {
			core.units.begin()->step = 3;
			return FALSE;
		}
		if(res == -1) {
			core.units.begin()->peffect = eset[0];
			core.units.begin()->step = 3;
			return FALSE;
		}
		core.select_effects.clear();
		core.select_options.clear();
		if(res) {
			core.select_effects.push_back(0);
			core.select_options.push_back(1);
		}
		for(int32 i = 0; i < eset.count; ++i) {
			core.select_effects.push_back(eset[i]);
			core.select_options.push_back(eset[i]->description);
		}
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, setplayer, 0);
		return FALSE;
	}
	case 2: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		if(peffect) {
			core.units.begin()->peffect = peffect;
			core.units.begin()->step = 3;
			return FALSE;
		}
		core.select_cards.clear();
		uint32 required = target->get_set_tribute_count();
		uint32 min = required & 0xffff;
		uint32 max = required >> 16;
		uint32 adv = is_player_can_mset(SUMMON_TYPE_ADVANCE, setplayer, target);
		if(max == 0 || !adv)
			returns.bvalue[0] = 0;
		else {
			card_set cset;
			int32 rcount = get_summon_release_list(target, &cset);
			if(rcount == 0)
				returns.bvalue[0] = 0;
			else {
				for(card_set::iterator cit = cset.begin(); cit != cset.end(); ++cit)
					core.select_cards.push_back(*cit);
				if(min == 0 && get_useable_count(setplayer, LOCATION_MZONE) > 0) {
					add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, setplayer, 90);
					core.temp_var[0] = (void*)required;
					core.units.begin()->step = 9;
				} else
					add_process(PROCESSOR_SELECT_TRIBUTE, 0, 0, 0, setplayer, required);
			}
		}
		return FALSE;
	}
	case 3: {
		if(returns.bvalue[0]) {
			card_set tributes;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				tributes.insert(core.select_cards[returns.bvalue[i + 1]]);
				core.select_cards[returns.bvalue[i + 1]]->current.reason_card = target;
			}
			target->set_material(&tributes);
			release(&tributes, 0, REASON_SUMMON | REASON_MATERIAL, setplayer);
			if(tributes.size())
				target->summon_type = SUMMON_TYPE_NORMAL | SUMMON_TYPE_ADVANCE;
			else
				target->summon_type = SUMMON_TYPE_NORMAL;
			adjust_all();
		} else
			target->summon_type = SUMMON_TYPE_NORMAL;
		target->summon_player = setplayer;
		break_effect();
		target->current.reason_effect = 0;
		target->current.reason_player = setplayer;
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 4: {
		target->summon_type = (proc->get_value(target) & 0xfffffff) | SUMMON_TYPE_NORMAL;
		target->current.reason_effect = proc;
		target->current.reason_player = setplayer;
		pduel->lua->add_param(target, PARAM_TYPE_CARD);
		core.sub_solving_event.push_back(nil_event);
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, proc, 0, setplayer, 0);
		return FALSE;
	}
	case 5: {
		if(!ignore_count) {
			returns.ivalue[0] = FALSE;
			effect* pextra = target->is_affected_by_effect(EFFECT_EXTRA_SET_COUNT);
			if(pextra) {
				core.temp_var[0] = pextra;
				if(core.summon_count[setplayer] < get_summon_count_limit(setplayer))
					add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, setplayer, 91);
				else
					returns.ivalue[0] = TRUE;
			}
		} else
			returns.ivalue[0] = TRUE;
		return FALSE;
	}
	case 6: {
		if(!returns.ivalue[0])
			core.summon_count[setplayer]++;
		else if(core.temp_var[0]) {
			effect* pextra = (effect*)core.temp_var[0];
			pextra->get_value(target);
		}
		target->enable_field_effect(FALSE);
		move_to_field(target, setplayer, setplayer, LOCATION_MZONE, POS_FACEDOWN_DEFENCE);
		if(!ignore_count)
			core.summon_count[setplayer]++;
		return FALSE;
	}
	case 7: {
		if(target->owner != setplayer)
			set_control(target, setplayer, 0, 0);
		core.phase_action = TRUE;
		core.normalsummon_state[setplayer] = TRUE;
		target->set_status(STATUS_SUMMON_TURN, TRUE);
		pduel->write_buffer8(MSG_SET);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		adjust_instant();
		raise_event(target, EVENT_MSET, proc, 0, setplayer, setplayer, 0);
		process_instant_event();
		adjust_all();
		if(core.current_chain.size() == 0) {
			core.hint_timing[setplayer] |= TIMING_MSET;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, infos.turn_player);
		}
		return TRUE;
	}
	case 10: {
		if(returns.ivalue[0])
			returns.bvalue[0] = 0;
		else
			add_process(PROCESSOR_SELECT_TRIBUTE, 0, 0, 0, setplayer, (ptr)(core.temp_var[0]) + 1);
		core.units.begin()->step = 2;
		return false;
	}
	}
	return TRUE;
}
int32 field::sset(uint16 step, uint8 setplayer, card * target) {
	switch(step) {
	case 0: {
		if(!(target->data.type & TYPE_FIELD) && get_useable_count(setplayer, LOCATION_SZONE) == 0)
			return TRUE;
		if(target->data.type & TYPE_MONSTER && !target->is_affected_by_effect(EFFECT_MONSTER_SSET))
			return TRUE;
		if(target->current.location == LOCATION_SZONE)
			return TRUE;
		if(!is_player_can_sset(setplayer, target))
			return TRUE;
		if(target->is_affected_by_effect(EFFECT_CANNOT_SSET))
			return TRUE;
		effect_set eset;
		target->filter_effect(EFFECT_SSET_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(setplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, setplayer, 0);
			}
		}
		return FALSE;
	}
	case 1: {
		target->enable_field_effect(FALSE);
		move_to_field(target, setplayer, setplayer, LOCATION_SZONE, POS_FACEDOWN);
		return FALSE;
	}
	case 2: {
		core.phase_action = TRUE;
		target->set_status(STATUS_SET_TURN, TRUE);
		pduel->write_buffer8(MSG_SET);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		adjust_instant();
		raise_event(target, EVENT_SSET, 0, 0, setplayer, setplayer, 0);
		process_instant_event();
		adjust_all();
		if(core.current_chain.size() == 0) {
			core.hint_timing[setplayer] |= TIMING_SSET;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, infos.turn_player);
		}
	}
	}
	return TRUE;
}
int32 field::special_summon_rule(uint16 step, uint8 sumplayer, card * target) {
	switch(step) {
	case 0: {
		if(!(target->data.type & TYPE_MONSTER))
			return FALSE;
		if(target->is_affected_by_effect(EFFECT_CANNOT_SPECIAL_SUMMON))
			return FALSE;
		if(target->current.location & (LOCATION_GRAVE + LOCATION_REMOVED) && !target->is_status(STATUS_REVIVE_LIMIT) && target->is_affected_by_effect(EFFECT_REVIVE_LIMIT))
			return FALSE;
		effect_set eset;
		target->filter_effect(EFFECT_SPSUMMON_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(sumplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, sumplayer, 0);
			}
		}
		return FALSE;
	}
	case 1: {
		effect_set eset;
		target->material_cards.clear();
		card* tuner = core.limit_tuner;
		target->filter_spsummon_procedure(sumplayer, &eset);
		core.limit_tuner = tuner;
		if(!eset.count)
			return TRUE;
		core.select_effects.clear();
		core.select_options.clear();
		for(int32 i = 0; i < eset.count; ++i) {
			core.select_effects.push_back(eset[i]);
			core.select_options.push_back(eset[i]->description);
		}
		if(core.select_options.size() == 1)
			returns.ivalue[0] = 0;
		else
			add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, sumplayer, 0);
		return FALSE;
	}
	case 2: {
		effect* peffect = core.select_effects[returns.ivalue[0]];
		core.units.begin()->peffect = peffect;
		target->summon_type = (peffect->get_value(target) & 0xfffffff) | SUMMON_TYPE_SPECIAL;
		if(peffect->operation) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			if(core.limit_tuner) {
				pduel->lua->add_param(core.limit_tuner, PARAM_TYPE_CARD);
				core.limit_tuner = 0;
			}
			core.sub_solving_event.push_back(nil_event);
			add_process(PROCESSOR_EXECUTE_OPERATION, 0, peffect, 0, sumplayer, 0);
		}
		return FALSE;
	}
	case 3: {
		effect* peffect = core.units.begin()->peffect;
		uint8 targetplayer = sumplayer;
		uint8 positions = POS_FACEUP;
		if(peffect->flag & EFFECT_FLAG_SPSUM_PARAM) {
			positions = peffect->s_range;
			if(peffect->o_range == 0)
				targetplayer = sumplayer;
			else
				targetplayer = 1 - sumplayer;
		}
		if(positions == 0)
			positions = POS_FACEUP_ATTACK;
		target->enable_field_effect(FALSE);
		move_to_field(target, sumplayer, targetplayer, LOCATION_MZONE, positions);
		target->current.reason = REASON_SPSUMMON;
		target->current.reason_effect = peffect;
		target->current.reason_player = sumplayer;
		target->summon_player = sumplayer;
		break_effect();
		return FALSE;
	}
	case 4: {
		uint8 targetplayer = target->current.controler;
		if(target->owner != targetplayer)
			set_control(target, targetplayer, 0, 0);
		core.phase_action = TRUE;
		target->current.reason_effect = core.units.begin()->peffect;
		card_set::iterator cit;
		target->set_status(STATUS_SUMMONING, TRUE);
		target->set_status(STATUS_SUMMON_DISABLED, FALSE);
		core.spsummoning_card = target;
		if(core.current_chain.size() > 1 || target->is_affected_by_effect(EFFECT_CANNOT_DISABLE_SPSUMMON)) {
			core.units.begin()->step = 14;
			return FALSE;
		} else if(core.current_chain.size() == 0) {
			core.units.begin()->step = 9;
			return FALSE;
		}
		core.reserved = core.units.front();
		return TRUE;
	}
	case 10: {
		pduel->write_buffer8(MSG_SPSUMMONING);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		core.spsummoning_card = 0;
		core.spsummon_state[sumplayer] = TRUE;
		raise_event(target, EVENT_SPSUMMON, core.units.begin()->peffect, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, TRUE, TRUE);
		return FALSE;
	}
	case 11: {
		if(target->is_status(STATUS_SUMMONING)) {
			core.units.begin()->step = 14;
			return FALSE;
		}
		remove_oath_effect(core.units.begin()->peffect);
		adjust_instant();
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		return TRUE;
	}
	case 15: {
		oath_effects::iterator oeit;
		for(oeit = effects.oath.begin(); oeit != effects.oath.end(); ++oeit)
			if(oeit->second == core.units.begin()->peffect)
				oeit->second = 0;
		target->set_status(STATUS_SUMMONING, FALSE);
		target->enable_field_effect(TRUE);
		target->set_status(STATUS_PROC_COMPLETE | STATUS_SUMMON_TURN, TRUE);
		core.spsummoning_card = 0;
		return FALSE;
	}
	case 16: {
		pduel->write_buffer8(MSG_SPSUMMONED);
		adjust_instant();
		if(target->material_cards.size()) {
			for(auto mit = target->material_cards.begin(); mit != target->material_cards.end(); ++mit)
				raise_single_event(*mit, EVENT_BE_MATERIAL, core.units.begin()->peffect, 0, sumplayer, sumplayer, 0);
		}
		process_single_event();
		return false;
	}
	case 17: {
		raise_single_event(target, EVENT_SPSUMMON_SUCCESS, core.units.begin()->peffect, 0, sumplayer, sumplayer, 0);
		process_single_event();
		raise_event(target, EVENT_SPSUMMON_SUCCESS, core.units.begin()->peffect, 0, sumplayer, sumplayer, 0);
		process_instant_event();
		adjust_all();
		if(core.current_chain.size() == 0) {
			core.hint_timing[sumplayer] |= TIMING_SPSUMMON;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::special_summon_step(uint16 step, group * targets, card * target) {
	uint8 playerid = (target->operation_param >> 24) & 0xf;
	uint8 nocheck = (target->operation_param >> 16) & 0xff;
	uint8 nolimit = (target->operation_param >> 8) & 0xff;
	uint8 positions = target->operation_param & 0xff;
	switch(step) {
	case 0: {
		returns.ivalue[0] = FALSE;
		uint32 result = TRUE;
		effect_set eset;
		if(target->is_status(STATUS_REVIVE_LIMIT) && !target->is_status(STATUS_PROC_COMPLETE)) {
			if((!nolimit && (target->current.location & 0x38)) || (!nocheck && (target->current.location & 0x3)))
				result = FALSE;
		}
		if(!result || (target->current.location == LOCATION_MZONE)
		        || !is_player_can_spsummon(core.reason_effect, target->summon_type, positions, target->summon_player, playerid, target)
		        || target->is_affected_by_effect(EFFECT_CANNOT_SPECIAL_SUMMON)
		        || get_useable_count(playerid, LOCATION_MZONE) == 0
		        || (!nocheck && !(target->data.type & TYPE_MONSTER)))
			result = FALSE;
		if(result && !nocheck) {
			target->filter_effect(EFFECT_SPSUMMON_CONDITION, &eset);
			for(int32 i = 0; i < eset.count; ++i) {
				pduel->lua->add_param(core.reason_effect, PARAM_TYPE_EFFECT);
				pduel->lua->add_param(target->summon_player, PARAM_TYPE_INT);
				pduel->lua->add_param(target->summon_type, PARAM_TYPE_INT);
				pduel->lua->add_param(positions, PARAM_TYPE_INT);
				pduel->lua->add_param(playerid, PARAM_TYPE_INT);
				if(!eset[i]->check_value_condition(5)) {
					result = FALSE;
					break;
				}
			}
		}
		if(!result) {
			target->current.reason = target->temp.reason;
			target->current.reason_effect = target->temp.reason_effect;
			target->current.reason_player = target->temp.reason_player;
			if(targets)
				targets->container.erase(target);
			return TRUE;
		}
		eset.clear();
		target->filter_effect(EFFECT_SPSUMMON_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(target, PARAM_TYPE_CARD);
			pduel->lua->add_param(target->summon_player, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(nil_event);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, target->summon_player, 0);
			}
		}
		return FALSE;
	}
	case 1: {
		if(!targets)
			core.special_summoning.insert(target);
		target->enable_field_effect(FALSE);
		core.spsummon_state[target->summon_player] = TRUE;
		core.hint_timing[target->summon_player] |= TIMING_SPSUMMON;
		move_to_field(target, target->summon_player, playerid, LOCATION_MZONE, positions);
		return FALSE;
	}
	case 2: {
		pduel->write_buffer8(MSG_SPSUMMONING);
		pduel->write_buffer32(target->data.code);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		return FALSE;
	}
	case 3: {
		returns.ivalue[0] = TRUE;
		if(target->owner != target->current.controler)
			set_control(target, target->current.controler, 0, 0);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::special_summon(uint16 step, effect * reason_effect, uint8 reason_player, group * targets) {
	card_set::iterator cit;
	switch(step) {
	case 0: {
		card_vector cv;
		card_vector::iterator cvit;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit)
			cv.push_back(*cit);
		if(cv.size() > 1)
			std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for(cvit = cv.begin(); cvit != cv.end(); ++cvit)
			add_process(PROCESSOR_SPSUMMON_STEP, 0, 0, targets, 0, (ptr)(*cvit));
		return FALSE;
	}
	case 1: {
		if(targets->container.size() == 0) {
			returns.ivalue[0] = 0;
			core.operated_set.clear();
			pduel->delete_group(targets);
			return TRUE;
		}
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			(*cit)->set_status(STATUS_SUMMON_TURN, TRUE);
			if((*cit)->is_position(POS_FACEUP))
				(*cit)->enable_field_effect(TRUE);
		}
		adjust_instant();
		return FALSE;
	}
	case 2: {
		pduel->write_buffer8(MSG_SPSUMMONED);
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			if(!((*cit)->current.position & POS_FACEDOWN))
				raise_single_event(*cit, EVENT_SPSUMMON_SUCCESS, (*cit)->current.reason_effect, 0, (*cit)->current.reason_player, (*cit)->summon_player, 0);
			if(((*cit)->summon_type & 0x3000000) && (*cit)->material_cards.size()) {
				card_set::iterator mit;
				for(mit = (*cit)->material_cards.begin(); mit != (*cit)->material_cards.end(); ++mit)
					raise_single_event(*mit, EVENT_BE_MATERIAL, core.reason_effect, 0, core.reason_player, (*cit)->summon_player, 0);
			}
		}
		process_single_event();
		return FALSE;
	}
	case 3: {
		raise_event(&targets->container, EVENT_SPSUMMON_SUCCESS, reason_effect, 0, reason_player, PLAYER_NONE, 0);
		process_instant_event();
		return FALSE;
	}
	case 4: {
		core.operated_set.clear();
		core.operated_set = targets->container;
		returns.ivalue[0] = targets->container.size();
		pduel->delete_group(targets);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::destroy(uint16 step, group * targets, card * target, uint8 battle) {
	if(target->current.location & (LOCATION_GRAVE | LOCATION_REMOVED)) {
		target->current.reason = target->temp.reason;
		target->current.reason_effect = target->temp.reason_effect;
		target->current.reason_player = target->temp.reason_player;
		target->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
		targets->container.erase(target);
		return TRUE;
	}
	if(targets->container.find(target) == targets->container.end())
		return TRUE;
	if(!(target->current.reason & REASON_RULE)) {
		returns.ivalue[0] = FALSE;
		effect_set eset;
		target->filter_single_continuous_effect(EFFECT_DESTROY_REPLACE, &eset);
		if(!battle)
			for (int32 i = 0; i < eset.count; ++i)
				add_process(PROCESSOR_OPERATION_REPLACE, 0, eset[i], targets, (ptr)target, 1);
		else
			for (int32 i = 0; i < eset.count; ++i)
				add_process(PROCESSOR_OPERATION_REPLACE, 10, eset[i], targets, (ptr)target, 1);
	}
	return TRUE;
}
int32 field::destroy(uint16 step, group * targets, effect * reason_effect, uint32 reason, uint8 reason_player) {
	switch (step) {
	case 0: {
		card_set extra;
		card_set::iterator cit, rm;
		effect_set eset;
		for (cit = targets->container.begin(); cit != targets->container.end();) {
			rm = cit++;
			if (!((*rm)->current.reason & REASON_RULE)) {
				if (!(*rm)->is_destructable() || !(*rm)->is_affect_by_effect(reason_effect)
				        || !(*rm)->is_destructable_by_effect(reason_effect, reason_player)) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					(*rm)->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
					targets->container.erase(*rm);
					continue;
				}
			}
			(*rm)->filter_effect(EFFECT_DESTROY_SUBSTITUTE, &eset);
			if (eset.count) {
				bool sub = false;
				for (int32 i = 0; i < eset.count; ++i) {
					pduel->lua->add_param((*rm)->current.reason_effect, PARAM_TYPE_EFFECT);
					pduel->lua->add_param((*rm)->current.reason, PARAM_TYPE_INT);
					pduel->lua->add_param((*rm)->current.reason_player, PARAM_TYPE_INT);
					if(eset[i]->check_value_condition(3)) {
						extra.insert(eset[i]->handler);
						sub = true;
					}
				}
				if(sub) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					core.destroy_canceled.insert(*rm);
					targets->container.erase(*rm);
				}
			}
			eset.clear();
		}
		for (cit = extra.begin(); cit != extra.end(); ++cit) {
			card* rep = *cit;
			if(targets->container.count(rep) == 0) {
				rep->temp.reason = rep->current.reason;
				rep->temp.reason_effect = rep->current.reason_effect;
				rep->temp.reason_player = rep->current.reason_player;
				rep->current.reason = REASON_EFFECT | REASON_DESTROY | REASON_REPLACE;
				rep->current.reason_effect = 0;
				rep->current.reason_player = rep->current.controler;
				rep->operation_param = (POS_FACEUP << 24) + (((int32)rep->owner) << 16) + (LOCATION_GRAVE << 8);
				targets->container.insert(rep);
			}
		}
		if(reason & REASON_RULE)
			return FALSE;
		auto pr = effects.continuous_effect.equal_range(EFFECT_DESTROY_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 5, pr.first->second, targets, 0, 1);
		return FALSE;
	}
	case 1: {
		card_set::iterator cit;
		for (cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			add_process(PROCESSOR_DESTROY_STEP, 0, 0, targets, (ptr) (*cit), 0);
		}
		return FALSE;
	}
	case 2: {
		card_set::iterator cit;
		for (cit = core.destroy_canceled.begin(); cit != core.destroy_canceled.end(); ++cit)
			(*cit)->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
		core.destroy_canceled.clear();
		return FALSE;
	}
	case 3: {
		if(!targets->container.size()) {
			returns.ivalue[0] = 0;
			core.operated_set.clear();
			pduel->delete_group(targets);
			return TRUE;
		}
		card_vector cv;
		card_vector::iterator cvit;
		card_set::iterator cit;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit)
			cv.push_back(*cit);
		if(cv.size() > 1)
			std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for (cvit = cv.begin(); cvit != cv.end(); ++cvit) {
			if((*cvit)->current.location & (LOCATION_GRAVE | LOCATION_REMOVED)) {
				(*cvit)->current.reason = (*cvit)->temp.reason;
				(*cvit)->current.reason_effect = (*cvit)->temp.reason_effect;
				(*cvit)->current.reason_player = (*cvit)->temp.reason_player;
				targets->container.erase((*cvit));
				continue;
			}
			(*cvit)->current.reason |= REASON_DESTROY;
			pduel->write_buffer8(MSG_DESTROY);
			pduel->write_buffer32((*cvit)->data.code);
			pduel->write_buffer8((*cvit)->current.controler);
			pduel->write_buffer8((*cvit)->current.location);
			pduel->write_buffer8((*cvit)->current.sequence);
			pduel->write_buffer8((*cvit)->current.position);
			core.hint_timing[(*cvit)->current.controler] |= TIMING_DESTROY;
			raise_single_event(*cvit, EVENT_DESTROY, (*cvit)->current.reason_effect, (*cvit)->current.reason, (*cvit)->current.reason_player, 0, 0);
		}
		adjust_instant();
		process_single_event();
		raise_event(&targets->container, EVENT_DESTROY, reason_effect, reason, reason_player, 0, 0);
		process_instant_event();
		return FALSE;
	}
	case 4: {
		card_set::iterator cit;
		group* sendtargets = pduel->new_group();
		sendtargets->is_readonly = TRUE;
		sendtargets->container = targets->container;
		uint32 dest;
		for(cit = sendtargets->container.begin(); cit != sendtargets->container.end(); ++cit) {
			(*cit)->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
			dest = ((*cit)->operation_param >> 8) & 0xff;
			if(!dest)
				dest = LOCATION_GRAVE;
			if((dest == LOCATION_HAND && (*cit)->is_affected_by_effect(EFFECT_CANNOT_TO_HAND))
			        || (dest == LOCATION_DECK && (*cit)->is_affected_by_effect(EFFECT_CANNOT_TO_DECK))
			        || (dest == LOCATION_REMOVED && (*cit)->is_affected_by_effect(EFFECT_CANNOT_REMOVE)))
				dest = LOCATION_GRAVE;
			(*cit)->operation_param = ((*cit)->operation_param & 0xffff00ff) + (dest << 8);
		}
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_SEND_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 5, pr.first->second, sendtargets, 0, 0);
		add_process(PROCESSOR_SENDTO, 1, 0, sendtargets, reason + REASON_DESTROY, reason_player);
		return FALSE;
	}
	case 5: {
		core.operated_set.clear();
		core.operated_set = targets->container;
		returns.ivalue[0] = targets->container.size();
		pduel->delete_group(targets);
		return TRUE;
	}
	case 10: {
		card_set::iterator cit, rm;
		effect_set eset;
		for (cit = targets->container.begin(); cit != targets->container.end();) {
			rm = cit++;
			if (!((*rm)->current.reason & REASON_RULE)) {
				if (!(*rm)->is_destructable()) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					(*rm)->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
					targets->container.erase(*rm);
					continue;
				}
			}
			(*rm)->filter_effect(EFFECT_DESTROY_SUBSTITUTE, &eset);
			if (eset.count) {
				bool sub = false;
				for (int32 i = 0; i < eset.count; ++i) {
					pduel->lua->add_param((*rm)->current.reason_effect, PARAM_TYPE_EFFECT);
					pduel->lua->add_param((*rm)->current.reason, PARAM_TYPE_INT);
					pduel->lua->add_param((*rm)->current.reason_player, PARAM_TYPE_INT);
					if(eset[i]->check_value_condition(3)) {
						core.battle_destroy_rep.insert(eset[i]->handler);
						sub = true;
					}
				}
				if(sub) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					core.destroy_canceled.insert(*rm);
					targets->container.erase(*rm);
				}
			}
			eset.clear();
		}
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_DESTROY_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 12, pr.first->second, targets, 0, 1);
		return FALSE;
	}
	case 11: {
		card_set::iterator cit;
		for (cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			add_process(PROCESSOR_DESTROY_STEP, 0, 0, targets, (ptr) (*cit), TRUE);
		}
		return FALSE;
	}
	case 12: {
		card_set::iterator cit;
		for (cit = core.destroy_canceled.begin(); cit != core.destroy_canceled.end(); ++cit)
			(*cit)->set_status(STATUS_DESTROY_CONFIRMED, FALSE);
		core.destroy_canceled.clear();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::release(uint16 step, group * targets, card * target) {
	if(!(target->current.location & (LOCATION_ONFIELD | LOCATION_HAND))) {
		target->current.reason = target->temp.reason;
		target->current.reason_effect = target->temp.reason_effect;
		target->current.reason_player = target->temp.reason_player;
		targets->container.erase(target);
		return TRUE;
	}
	if(targets->container.find(target) == targets->container.end())
		return TRUE;
	if(!(target->current.reason & REASON_RULE)) {
		returns.ivalue[0] = FALSE;
		effect_set eset;
		target->filter_single_continuous_effect(EFFECT_RELEASE_REPLACE, &eset);
		for (int32 i = 0; i < eset.count; ++i)
			add_process(PROCESSOR_OPERATION_REPLACE, 0, eset[i], targets, (ptr)target, 0);
	}
	return TRUE;
}
int32 field::release(uint16 step, group * targets, effect * reason_effect, uint32 reason, uint8 reason_player) {
	switch (step) {
	case 0: {
		card_set extra;
		card_set::iterator cit, rm;
		for (cit = targets->container.begin(); cit != targets->container.end();) {
			rm = cit++;
			if ((*rm)->is_status(STATUS_SUMMONING)
			        || ((reason & REASON_SUMMON) && !(*rm)->is_releaseable_by_summon(reason_player, (*rm)->current.reason_card))
			        || (!((*rm)->current.reason & (REASON_RULE | REASON_SUMMON | REASON_COST))
			            && (!(*rm)->is_affect_by_effect((*rm)->current.reason_effect) || !(*rm)->is_releaseable_by_nonsummon(reason_player)))) {
				(*rm)->current.reason = (*rm)->temp.reason;
				(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
				(*rm)->current.reason_player = (*rm)->temp.reason_player;
				targets->container.erase(*rm);
				continue;
			}
		}
		if(reason & REASON_RULE)
			return FALSE;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_RELEASE_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 5, pr.first->second, targets, 0, 0);
		return FALSE;
	}
	case 1: {
		card_set::iterator cit;
		for (cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			add_process(PROCESSOR_RELEASE_STEP, 0, 0, targets, (ptr) (*cit), 0);
		}
		return FALSE;
	}
	case 2: {
		if(!targets->container.size()) {
			returns.ivalue[0] = 0;
			core.operated_set.clear();
			pduel->delete_group(targets);
			return TRUE;
		}
		card_vector cv;
		card_vector::iterator cvit;
		card_set::iterator cit;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit)
			cv.push_back(*cit);
		if(cv.size() > 1)
			std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for (cvit = cv.begin(); cvit != cv.end(); ++cvit) {
			if(!((*cvit)->current.location & (LOCATION_ONFIELD | LOCATION_HAND))) {
				(*cvit)->current.reason = (*cvit)->temp.reason;
				(*cvit)->current.reason_effect = (*cvit)->temp.reason_effect;
				(*cvit)->current.reason_player = (*cvit)->temp.reason_player;
				targets->container.erase((*cvit));
				continue;
			}
			(*cvit)->current.reason |= REASON_RELEASE;
			pduel->write_buffer8(MSG_RELEASE);
			pduel->write_buffer32((*cvit)->data.code);
			pduel->write_buffer8((*cvit)->current.controler);
			pduel->write_buffer8((*cvit)->current.location);
			pduel->write_buffer8((*cvit)->current.sequence);
			pduel->write_buffer8((*cvit)->current.position);
			raise_single_event(*cvit, EVENT_RELEASE, (*cvit)->current.reason_effect, (*cvit)->current.reason, (*cvit)->current.reason_player, 0, 0);
		}
		adjust_instant();
		process_single_event();
		raise_event(&targets->container, EVENT_RELEASE, reason_effect, reason, reason_player, 0, 0);
		process_instant_event();
		return FALSE;
	}
	case 3: {
		card_set::iterator cit;
		group* sendtargets = pduel->new_group();
		sendtargets->is_readonly = TRUE;
		sendtargets->container = targets->container;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_SEND_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 5, pr.first->second, sendtargets, 0, 0);
		add_process(PROCESSOR_SENDTO, 1, 0, sendtargets, reason + REASON_RELEASE, reason_player);
		return FALSE;
	}
	case 4: {
		core.operated_set.clear();
		core.operated_set = targets->container;
		returns.ivalue[0] = targets->container.size();
		pduel->delete_group(targets);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::send_to(uint16 step, group * targets, card * target) {
	//uint8 playerid = (target->operation_param >> 16) & 0xff;
	uint8 dest = (target->operation_param >> 8) & 0xff;
	//uint8 seq = (target->operation_param) & 0xff;
	if(targets->container.find(target) == targets->container.end())
		return TRUE;
	if(target->current.location == dest) {
		target->current.reason = target->temp.reason;
		target->current.reason_effect = target->temp.reason_effect;
		target->current.reason_player = target->temp.reason_player;
		targets->container.erase(target);
		return TRUE;
	}
	if(!(target->current.reason & REASON_RULE)) {
		returns.ivalue[0] = FALSE;
		effect_set eset;
		target->filter_single_continuous_effect(EFFECT_SEND_REPLACE, &eset);
		for (int32 i = 0; i < eset.count; ++i)
			add_process(PROCESSOR_OPERATION_REPLACE, 0, eset[i], targets, (ptr)target, 0);
	}
	return TRUE;
}
int32 field::send_to(uint16 step, group * targets, effect * reason_effect, uint32 reason, uint8 reason_player) {
	card_set::iterator cit, rm;
	switch(step) {
	case 0: {
		uint8 dest;
		for(cit = targets->container.begin(); cit != targets->container.end();) {
			rm = cit++;
			dest = ((*rm)->operation_param >> 8) & 0xff;
			if(!(reason & REASON_RULE) &&
			        ((*rm)->is_status(STATUS_SUMMONING)
			         || (!((*rm)->current.reason & REASON_COST) && !(*rm)->is_affect_by_effect((*rm)->current.reason_effect))
			         || (dest == LOCATION_HAND && !(*rm)->is_capable_send_to_hand(core.reason_player))
			         || (dest == LOCATION_DECK && !(*rm)->is_capable_send_to_deck(core.reason_player))
			         || (dest == LOCATION_REMOVED && !(*rm)->is_removeable(core.reason_player))
			         || (dest == LOCATION_GRAVE && !(*rm)->is_capable_send_to_grave(core.reason_player)))) {
				(*rm)->current.reason = (*rm)->temp.reason;
				(*rm)->current.reason_player = (*rm)->temp.reason_player;
				(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
				targets->container.erase(*rm);
				continue;
			}
		}
		if(reason & REASON_RULE)
			return FALSE;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(EFFECT_SEND_REPLACE);
		for (; pr.first != pr.second; ++pr.first)
			add_process(PROCESSOR_OPERATION_REPLACE, 5, pr.first->second, targets, 0, 0);
		return FALSE;
	}
	case 1: {
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			add_process(PROCESSOR_SENDTO_STEP, 0, 0, targets, (ptr)(*cit), 0);
		}
		return FALSE;
	}
	case 2: {
		if(!targets->container.size()) {
			returns.ivalue[0] = 0;
			core.operated_set.clear();
			pduel->delete_group(targets);
			return TRUE;
		}
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			(*cit)->enable_field_effect(FALSE);
		}
		adjust_instant();
		return FALSE;
	}
	case 3: {
		uint32 redirect, dest;
		card_set leave_p;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			dest = ((*cit)->operation_param >> 8) & 0xff;
			redirect = 0;
			if(((*cit)->current.location & LOCATION_ONFIELD) && !(*cit)->is_status(STATUS_SUMMON_DISABLED))
				redirect = (*cit)->leave_field_redirect();
			if(redirect) {
				(*cit)->current.reason &= ~REASON_TEMPORARY;
				(*cit)->current.reason |= REASON_REDIRECT;
				(*cit)->operation_param = ((*cit)->operation_param & 0xffff0000) + (redirect << 8) + (redirect >> 16);
				dest = redirect;
			}
			redirect = (*cit)->destination_redirect(dest);
			if(redirect && (*cit)->current.location != redirect) {
				(*cit)->current.reason |= REASON_REDIRECT;
				(*cit)->operation_param = ((*cit)->operation_param & 0xffff0000) + (redirect << 8) + (redirect >> 16);
				dest = redirect;
			}
			if(((*cit)->current.location & LOCATION_ONFIELD) && !(*cit)->is_status(STATUS_SUMMON_DISABLED)) {
				raise_single_event(*cit, EVENT_LEAVE_FIELD_P, (*cit)->current.reason_effect, (*cit)->current.reason, (*cit)->current.reason_player, 0, 0);
				leave_p.insert(*cit);
			}
		}
		if(leave_p.size())
			raise_event(&leave_p, EVENT_LEAVE_FIELD_P, reason_effect, reason, reason_player, 0, 0);
		process_single_event();
		process_instant_event();
		return FALSE;
	}
	case 4: {
		card_set leave, discard;
		uint8 oloc, playerid, dest, seq;
		card_vector cv;
		card_vector::iterator cvit;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit)
			cv.push_back(*cit);
		if(cv.size() > 1)
			std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for (cvit = cv.begin(); cvit != cv.end(); ++cvit) {
			oloc = (*cvit)->current.location;
			playerid = ((*cvit)->operation_param >> 16) & 0xff;
			dest = ((*cvit)->operation_param >> 8) & 0xff;
			seq = ((*cvit)->operation_param) & 0xff;
			(*cvit)->enable_field_effect(FALSE);
			if((*cvit)->equiping_target)
				(*cvit)->unequip();
			if((*cvit)->data.type & TYPE_TOKEN) {
				pduel->write_buffer8(MSG_MOVE);
				pduel->write_buffer32((*cvit)->data.code);
				pduel->write_buffer8((*cvit)->current.controler);
				pduel->write_buffer8((*cvit)->current.location);
				pduel->write_buffer8((*cvit)->current.sequence);
				pduel->write_buffer8((*cvit)->current.position);
				pduel->write_buffer8(0);
				pduel->write_buffer8(0);
				pduel->write_buffer8(0);
				pduel->write_buffer8(0);
				(*cvit)->previous.controler = (*cvit)->current.controler;
				(*cvit)->previous.location = (*cvit)->current.location;
				(*cvit)->previous.sequence = (*cvit)->current.sequence;
				(*cvit)->previous.position = (*cvit)->current.position;
				(*cvit)->current.reason &= ~REASON_TEMPORARY;
				(*cvit)->fieldid = infos.field_id++;
				(*cvit)->reset(RESET_LEAVE, RESET_EVENT);
				remove_card(*cvit);
				continue;
			}
			if(dest == LOCATION_GRAVE) {
				core.hint_timing[(*cvit)->current.controler] |= TIMING_TOGRAVE;
			} else if(dest == LOCATION_HAND) {
				(*cvit)->set_status(STATUS_PROC_COMPLETE, FALSE);
				core.hint_timing[(*cvit)->current.controler] |= TIMING_TOHAND;
			} else if(dest == LOCATION_DECK) {
				(*cvit)->set_status(STATUS_PROC_COMPLETE, FALSE);
				core.hint_timing[(*cvit)->current.controler] |= TIMING_TODECK;
			} else if(dest == LOCATION_REMOVED) {
				core.hint_timing[(*cvit)->current.controler] |= TIMING_REMOVE;
			}
			pduel->write_buffer8(MSG_MOVE);
			pduel->write_buffer32((*cvit)->data.code);
			if((*cvit)->overlay_target) {
				pduel->write_buffer8((*cvit)->overlay_target->current.controler);
				pduel->write_buffer8((*cvit)->overlay_target->current.location | LOCATION_OVERLAY);
				pduel->write_buffer8((*cvit)->overlay_target->current.sequence);
				pduel->write_buffer8((*cvit)->current.sequence);
				(*cvit)->overlay_target->xyz_remove(*cvit);
			} else {
				pduel->write_buffer8((*cvit)->current.controler);
				pduel->write_buffer8((*cvit)->current.location);
				pduel->write_buffer8((*cvit)->current.sequence);
				pduel->write_buffer8((*cvit)->current.position);
			}
			move_card(playerid, *cvit, dest, seq);
			(*cvit)->current.position = ((*cvit)->operation_param >> 24);
			pduel->write_buffer8((*cvit)->current.controler);
			pduel->write_buffer8((*cvit)->current.location);
			pduel->write_buffer8((*cvit)->current.sequence);
			pduel->write_buffer8((*cvit)->current.position);
			(*cvit)->set_status(STATUS_LEAVE_CONFIRMED, FALSE);
			if((*cvit)->status & (STATUS_SUMMON_DISABLED | STATUS_ACTIVATE_DISABLED)) {
				(*cvit)->set_status(STATUS_SUMMON_DISABLED | STATUS_ACTIVATE_DISABLED, FALSE);
				(*cvit)->previous.location = 0;
			} else if(oloc & LOCATION_ONFIELD) {
				(*cvit)->reset(RESET_LEAVE, RESET_EVENT);
				raise_single_event(*cvit, EVENT_LEAVE_FIELD, (*cvit)->current.reason_effect, (*cvit)->current.reason, (*cvit)->current.reason_player, 0, 0);
				leave.insert(*cvit);
			}
			if((*cvit)->current.reason & REASON_DISCARD) {
				raise_single_event(*cvit, EVENT_DISCARD, (*cvit)->current.reason_effect, (*cvit)->current.reason, (*cvit)->current.reason_player, 0, 0);
				discard.insert(*cvit);
			}
		}
		adjust_instant();
		process_single_event();
		if(leave.size()) {
			raise_event(&leave, EVENT_LEAVE_FIELD, reason_effect, reason, reason_player, 0, 0);
			process_instant_event();
		}
		if(discard.size()) {
			raise_event(&discard, EVENT_DISCARD, reason_effect, reason, reason_player, 0, 0);
			process_instant_event();
		}
		return FALSE;
	}
	case 5: {
		uint8 nloc;
		card_set tohand, todeck, tograve, remove;
		card_set equipings, overlays;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit) {
			nloc = (*cit)->current.location;
			if((*cit)->equiping_cards.size()) {
				card_set::iterator csit, rm;
				for(csit = (*cit)->equiping_cards.begin(); csit != (*cit)->equiping_cards.end();) {
					rm = csit++;
					(*rm)->unequip();
					equipings.insert(*rm);
				}
			}
			if((*cit)->data.type & TYPE_TOKEN)
				continue;
			(*cit)->enable_field_effect(TRUE);
			if(nloc == LOCATION_HAND) {
				tohand.insert(*cit);
				(*cit)->reset(RESET_TOHAND, RESET_EVENT);
				raise_single_event(*cit, EVENT_TO_HAND, (*cit)->current.reason_effect, (*cit)->current.reason, (*cit)->current.reason_player, 0, 0);
			} else if(nloc == LOCATION_DECK || nloc == LOCATION_EXTRA) {
				todeck.insert(*cit);
				(*cit)->reset(RESET_TODECK, RESET_EVENT);
				raise_single_event(*cit, EVENT_TO_DECK, (*cit)->current.reason_effect, (*cit)->current.reason, (*cit)->current.reason_player, 0, 0);
			} else if(nloc == LOCATION_GRAVE) {
				tograve.insert(*cit);
				(*cit)->reset(RESET_TOGRAVE, RESET_EVENT);
				raise_single_event(*cit, EVENT_TO_GRAVE, (*cit)->current.reason_effect, (*cit)->current.reason, (*cit)->current.reason_player, 0, 0);
			} else if(nloc == LOCATION_REMOVED) {
				remove.insert(*cit);
				if((*cit)->current.reason & REASON_TEMPORARY)
					(*cit)->reset(RESET_TEMP_REMOVE, RESET_EVENT);
				else
					(*cit)->reset(RESET_REMOVE, RESET_EVENT);
				raise_single_event(*cit, EVENT_REMOVE, (*cit)->current.reason_effect, (*cit)->current.reason, (*cit)->current.reason_player, 0, 0);
			}
			if((*cit)->xyz_materials.size()) {
				for(auto clit = (*cit)->xyz_materials.begin(); clit != (*cit)->xyz_materials.end(); ++clit)
					overlays.insert(*clit);
			}
		}
		if(tohand.size())
			raise_event(&tohand, EVENT_TO_HAND, reason_effect, reason, reason_player, 0, 0);
		if(todeck.size())
			raise_event(&todeck, EVENT_TO_DECK, reason_effect, reason, reason_player, 0, 0);
		if(tograve.size())
			raise_event(&tograve, EVENT_TO_GRAVE, reason_effect, reason, reason_player, 0, 0);
		if(remove.size())
			raise_event(&remove, EVENT_REMOVE, reason_effect, reason, reason_player, 0, 0);
		process_single_event();
		process_instant_event();
		if(equipings.size())
			destroy(&equipings, 0, REASON_LOST_TARGET, PLAYER_NONE);
		if(overlays.size())
			send_to(&overlays, 0, REASON_LOST_TARGET, PLAYER_NONE, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		adjust_instant();
		return FALSE;
	}
	case 6: {
		core.operated_set.clear();
		core.operated_set = targets->container;
		returns.ivalue[0] = targets->container.size();
		pduel->delete_group(targets);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::discard_deck(uint16 step, uint8 playerid, uint8 count, uint32 reason) {
	switch(step) {
	case 0: {
		if(is_player_affected_by_effect(playerid, EFFECT_CANNOT_DISCARD_DECK)) {
			core.operated_set.clear();
			returns.ivalue[0] = 0;
			return TRUE;
		}
		uint32 redirect, dest;
		int32 i = 0;
		for(auto cit = player[playerid].list_main.rbegin(); i < count && cit != player[playerid].list_main.rend(); ++cit, ++i) {
			dest = LOCATION_GRAVE;
			(*cit)->operation_param = LOCATION_GRAVE;
			(*cit)->current.reason_effect = core.reason_effect;
			(*cit)->current.reason_player = core.reason_player;
			(*cit)->current.reason = reason;
			redirect = (*cit)->destination_redirect(dest);
			if(redirect) {
				(*cit)->operation_param = redirect;
				dest = redirect;
			}
		}
		return FALSE;
	}
	case 1: {
		uint8 dest;
		card* pcard;
		card_set tohand, todeck, tograve, remove;
		core.discarded_set.clear();
		for (int32 i = 0; i < count; ++i) {
			if(player[playerid].list_main.size() == 0)
				break;
			pcard = player[playerid].list_main.back();
			dest = pcard->operation_param;
			if(dest == LOCATION_GRAVE)
				pcard->reset(RESET_TOGRAVE, RESET_EVENT);
			else if(dest == LOCATION_HAND) {
				pcard->reset(RESET_TOHAND, RESET_EVENT);
				pcard->set_status(STATUS_PROC_COMPLETE, FALSE);
			} else if(dest == LOCATION_DECK) {
				pcard->reset(RESET_TODECK, RESET_EVENT);
				pcard->set_status(STATUS_PROC_COMPLETE, FALSE);
			} else if(dest == LOCATION_REMOVED) {
				if(pcard->current.reason & REASON_TEMPORARY)
					pcard->reset(RESET_TEMP_REMOVE, RESET_EVENT);
				else
					pcard->reset(RESET_REMOVE, RESET_EVENT);
			}
			pduel->write_buffer8(MSG_MOVE);
			pduel->write_buffer32(pcard->data.code);
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer8(pcard->current.position);
			pcard->enable_field_effect(FALSE);
			pcard->cancel_field_effect();
			player[playerid].list_main.pop_back();
			pcard->previous.controler = pcard->current.controler;
			pcard->previous.location = pcard->current.location;
			pcard->previous.sequence = pcard->current.sequence;
			pcard->previous.position = pcard->current.position;
			pcard->current.controler = PLAYER_NONE;
			pcard->current.location = 0;
			add_card(pcard->owner, pcard, dest, 0);
			pcard->enable_field_effect(TRUE);
			pcard->current.position = POS_FACEUP;
			pduel->write_buffer8(pcard->current.controler);
			pduel->write_buffer8(pcard->current.location);
			pduel->write_buffer8(pcard->current.sequence);
			pduel->write_buffer8(pcard->current.position);
			if(dest == LOCATION_HAND) {
				tohand.insert(pcard);
				raise_single_event(pcard, EVENT_TO_HAND, pcard->current.reason_effect, pcard->current.reason, pcard->current.reason_player, 0, 0);
			} else if(dest == LOCATION_DECK || dest == LOCATION_EXTRA) {
				todeck.insert(pcard);
				raise_single_event(pcard, EVENT_TO_DECK, pcard->current.reason_effect, pcard->current.reason, pcard->current.reason_player, 0, 0);
			} else if(dest == LOCATION_GRAVE) {
				tograve.insert(pcard);
				raise_single_event(pcard, EVENT_TO_GRAVE, pcard->current.reason_effect, pcard->current.reason, pcard->current.reason_player, 0, 0);
			} else if(dest == LOCATION_REMOVED) {
				remove.insert(pcard);
				raise_single_event(pcard, EVENT_REMOVE, pcard->current.reason_effect, pcard->current.reason, pcard->current.reason_player, 0, 0);
			}
			core.discarded_set.insert(pcard);
		}
		if(tohand.size())
			raise_event(&tohand, EVENT_TO_HAND, core.reason_effect, reason, core.reason_player, 0, 0);
		if(todeck.size())
			raise_event(&todeck, EVENT_TO_DECK, core.reason_effect, reason, core.reason_player, 0, 0);
		if(tograve.size())
			raise_event(&tograve, EVENT_TO_GRAVE, core.reason_effect, reason, core.reason_player, 0, 0);
		if(remove.size())
			raise_event(&remove, EVENT_REMOVE, core.reason_effect, reason, core.reason_player, 0, 0);
		process_single_event();
		process_instant_event();
		adjust_instant();
		return FALSE;
	}
	case 2: {
		core.operated_set.clear();
		core.operated_set = core.discarded_set;
		returns.ivalue[0] = core.discarded_set.size();
		core.discarded_set.clear();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::move_to_field(uint16 step, card * target, uint32 enable, uint32 ret) {
	uint32 move_player = (target->operation_param >> 24) & 0xff;
	uint32 playerid = (target->operation_param >> 16) & 0xff;
	uint32 location = (target->operation_param >> 8) & 0xff;
	uint32 positions = (target->operation_param) & 0xff;
	switch(step) {
	case 0: {
		if(ret && !(target->current.reason & REASON_TEMPORARY))
			return TRUE;
		if(!(location == LOCATION_SZONE && (target->data.type & TYPE_FIELD) && (target->data.type & TYPE_SPELL))) {
			uint32 flag;
			uint32 ct = get_useable_count(playerid, location, &flag);
			if(ret && (ct == 0 || !(target->data.type & TYPE_MONSTER))) {
				send_to(target, core.reason_effect, REASON_EFFECT, core.reason_player, PLAYER_NONE, LOCATION_GRAVE, 0, 0);
				return TRUE;
			}
			if(!ct)
				return TRUE;
			if(move_player == playerid) {
				if(location == LOCATION_SZONE)
					flag = ((flag << 8) & 0xff00) | 0xffff00ff;
				else
					flag = (flag & 0xff) | 0xffffff00;
			} else {
				if(location == LOCATION_SZONE)
					flag = ((flag << 24) & 0xff000000) | 0xffffff;
				else
					flag = ((flag << 16) & 0xff0000) | 0xff00ffff;
			}
			add_process(PROCESSOR_SELECT_PLACE, 0, 0, 0, 0x10000 + move_player, flag);
		} else {
			card* pcard;
			if((pcard = get_field_card(playerid, LOCATION_SZONE, 5))) {
				destroy(pcard, 0, REASON_RULE, PLAYER_NONE);
				adjust_all();
			}
		}
		return FALSE;
	}
	case 1: {
		uint32 seq = returns.bvalue[2];
		if(location == LOCATION_SZONE && (target->data.type & TYPE_FIELD) && (target->data.type & TYPE_SPELL))
			seq = 5;
		if(!ret) {
			if(location != target->current.location) {
				if(location & LOCATION_ONFIELD)
					target->reset(RESET_TOFIELD, RESET_EVENT);
				if(target->current.location & LOCATION_ONFIELD)
					target->reset(RESET_LEAVE, RESET_EVENT);
			}
		}
		target->temp.sequence = seq;
		if(location != LOCATION_MZONE) {
			returns.ivalue[0] = positions;
			return FALSE;
		}
		add_process(PROCESSOR_SELECT_POSITION, 0, 0, 0, (positions << 16) + move_player, target->data.code);
		return FALSE;
	}
	case 2: {
		pduel->write_buffer8(MSG_MOVE);
		pduel->write_buffer32(target->data.code);
		if(target->overlay_target) {
			pduel->write_buffer8(target->overlay_target->current.controler);
			pduel->write_buffer8(target->overlay_target->current.location | LOCATION_OVERLAY);
			pduel->write_buffer8(target->overlay_target->current.sequence);
			pduel->write_buffer8(target->current.sequence);
			target->overlay_target->xyz_remove(target);
		} else {
			pduel->write_buffer8(target->current.controler);
			pduel->write_buffer8(target->current.location);
			pduel->write_buffer8(target->current.sequence);
			pduel->write_buffer8(target->current.position);
		}
		move_card(playerid, target, location, target->temp.sequence);
		target->current.position = returns.ivalue[0];
		target->set_status(STATUS_LEAVE_CONFIRMED | STATUS_ACTIVATED, FALSE);
		pduel->write_buffer8(target->current.controler);
		pduel->write_buffer8(target->current.location);
		pduel->write_buffer8(target->current.sequence);
		pduel->write_buffer8(target->current.position);
		if((target->current.location != LOCATION_MZONE)) {
			if(target->equiping_cards.size()) {
				card_set::iterator csit, rm;
				destroy(&target->equiping_cards, 0, REASON_LOST_TARGET, PLAYER_NONE);
				for(csit = target->equiping_cards.begin(); csit != target->equiping_cards.end();) {
					rm = csit++;
					(*rm)->unequip();
				}
			}
			if(target->xyz_materials.size()) {
				card_set overlays;
				for(auto clit = target->xyz_materials.begin(); clit != target->xyz_materials.end(); ++clit)
					overlays.insert(*clit);
				send_to(&overlays, 0, REASON_LOST_TARGET, PLAYER_NONE, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
			}
		}
		if((target->previous.location == LOCATION_SZONE) && target->equiping_target)
			target->unequip();
		if(enable || ret)
			target->enable_field_effect(TRUE);
		adjust_instant();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::change_position(uint16 step, group * targets, effect * reason_effect, uint8 reason_player, uint32 enable) {
	switch(step) {
	case 0: {
		card_set::iterator cit;
		card_set equipings;
		card_set pos_changed;
		uint8 npos, opos, noflip;
		card_vector cv;
		card_vector::iterator cvit;
		for(cit = targets->container.begin(); cit != targets->container.end(); ++cit)
			cv.push_back(*cit);
		if(cv.size() > 1)
			std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for(cvit = cv.begin(); cvit != cv.end(); ++cvit) {
			npos = (*cvit)->operation_param & 0xff;
			opos = (*cvit)->current.position;
			noflip = (*cvit)->operation_param >> 16;
			if((*cvit)->is_status(STATUS_SUMMONING) || (*cvit)->overlay_target || !((*cvit)->current.location & LOCATION_ONFIELD)
			        || !(*cvit)->is_affect_by_effect(reason_effect) || npos == opos
			        || ((opos & POS_FACEUP) &&  (npos & POS_FACEDOWN) && !(*cvit)->is_capable_turn_set(reason_player))) {
				targets->container.erase((*cvit));
			} else {
				if(((*cvit)->data.type & TYPE_TOKEN) && (npos & POS_FACEDOWN))
					npos = POS_FACEUP_DEFENCE;
				(*cvit)->previous.position = opos;
				(*cvit)->current.position = npos;
				if(npos & POS_DEFENCE)
					(*cvit)->set_status(STATUS_ATTACK_CANCELED, TRUE);
				(*cvit)->set_status(STATUS_JUST_POS, TRUE);
				pduel->write_buffer8(MSG_POS_CHANGE);
				pduel->write_buffer32((*cvit)->data.code);
				pduel->write_buffer8((*cvit)->current.controler);
				pduel->write_buffer8((*cvit)->current.location);
				pduel->write_buffer8((*cvit)->current.sequence);
				pduel->write_buffer8((*cvit)->previous.position);
				pduel->write_buffer8((*cvit)->current.position);
				core.hint_timing[(*cvit)->current.controler] |= TIMING_POS_CHANGE;
				if((opos & POS_FACEDOWN) && (npos & POS_FACEUP)) {
					(*cvit)->fieldid = infos.field_id++;
					if((*cvit)->current.location == LOCATION_MZONE) {
						raise_single_event((*cvit), EVENT_FLIP, reason_effect, 0, reason_player, 0, noflip);
						if(infos.phase == PHASE_BATTLE && (*cvit)->current.controler != infos.turn_player)
							core.pre_field[(*cvit)->current.sequence] = (*cvit)->fieldid;
					}
					if(enable)
						(*cvit)->enable_field_effect(TRUE);
					else
						(*cvit)->refresh_disable_status();
				}
				if((*cvit)->current.location == LOCATION_MZONE) {
					raise_single_event(*cvit, EVENT_CHANGE_POS, reason_effect, 0, reason_player, 0, 0);
					pos_changed.insert(*cvit);
				}
				if((opos & POS_FACEUP) && (npos & POS_FACEDOWN)) {
					(*cvit)->reset(RESET_TURN_SET, RESET_EVENT);
					(*cvit)->set_status(STATUS_SET_TURN, TRUE);
					(*cvit)->enable_field_effect(FALSE);
				}
				if((npos & POS_FACEDOWN) && (*cvit)->equiping_cards.size()) {
					card_set::iterator csit, erm;
					for(csit = (*cvit)->equiping_cards.begin(); csit != (*cvit)->equiping_cards.end();) {
						erm = csit++;
						(*erm)->unequip();
						equipings.insert(*erm);
					}
				}
				if((npos & POS_FACEDOWN) && (*cvit)->equiping_target)
					(*cvit)->unequip();
				if((npos & POS_FACEDOWN) && ((*cvit)->get_type()&TYPE_TRAPMONSTER)) {
					refresh_location_info_instant();
					move_to_field(*cvit, (*cvit)->current.controler, (*cvit)->current.controler, LOCATION_SZONE, POS_FACEDOWN);
				}
			}
		}
		adjust_instant();
		process_single_event();
		if(pos_changed.size())
			raise_event(&pos_changed, EVENT_CHANGE_POS, reason_effect, 0, reason_player, 0, 0);
		process_instant_event();
		if(equipings.size())
			destroy(&equipings, 0, REASON_LOST_TARGET, PLAYER_NONE);
		return FALSE;
	}
	case 1: {
		core.operated_set.clear();
		core.operated_set = targets->container;
		returns.ivalue[0] = targets->container.size();
		pduel->delete_group(targets);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::operation_replace(uint16 step, effect * replace_effect, group * targets, ptr arg1, ptr arg2) {
	switch (step) {
	case 0: {
		if(returns.ivalue[0])
			return TRUE;
		if(!replace_effect->target)
			return TRUE;
		card* target = (card*)arg1;
		event e;
		e.event_cards = targets;
		e.event_player = replace_effect->get_handler_player();
		e.event_value = 0;
		e.reason = target->current.reason;
		e.reason_effect = target->current.reason_effect;
		e.reason_player = target->current.reason_player;
		if(!replace_effect->is_activateable(replace_effect->get_handler_player(), e))
			return TRUE;
		chain newchain;
		newchain.chain_id = 0;
		newchain.chain_count = 0;
		newchain.triggering_effect = replace_effect;
		newchain.triggering_player = e.event_player;
		newchain.evt = e;
		newchain.target_cards = 0;
		newchain.target_player = PLAYER_NONE;
		newchain.target_param = 0;
		newchain.disable_player = PLAYER_NONE;
		newchain.disable_reason = 0;
		newchain.flag = 0;
		core.solving_event.push_front(e);
		core.sub_solving_event.push_back(e);
		core.continuous_chain.push_back(newchain);
		add_process(PROCESSOR_EXECUTE_TARGET, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 1: {
		card* target = (card*)arg1;
		if (returns.ivalue[0]) {
			targets->container.erase(target);
			target->current.reason = target->temp.reason;
			target->current.reason_effect = target->temp.reason_effect;
			target->current.reason_player = target->temp.reason_player;
			if(arg2)
				core.destroy_canceled.insert(target);
			replace_effect->dec_count();
		} else
			core.units.begin()->step = 2;
		return FALSE;
	}
	case 2: {
		if(!replace_effect->operation)
			return FALSE;
		core.sub_solving_event.push_back(*core.solving_event.begin());
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 3: {
		if(core.continuous_chain.rbegin()->target_cards)
			pduel->delete_group(core.continuous_chain.rbegin()->target_cards);
		chain::opmap::iterator oit;
		for(oit = core.continuous_chain.rbegin()->opinfos.begin(); oit != core.continuous_chain.rbegin()->opinfos.end(); ++oit) {
			if(oit->second.op_cards)
				pduel->delete_group(oit->second.op_cards);
		}
		core.continuous_chain.pop_back();
		core.solving_event.pop_front();
		return TRUE;
	}
	case 5: {
		event e;
		e.event_cards = targets;
		e.event_player = replace_effect->get_handler_player();
		e.event_value = 0;
		if(targets->container.size() == 0)
			return TRUE;
		card* pc = *targets->container.begin();
		e.reason = pc->current.reason;
		e.reason_effect = pc->current.reason_effect;
		e.reason_player = pc->current.reason_player;
		if(!replace_effect->is_activateable(replace_effect->get_handler_player(), e) || !replace_effect->value)
			return TRUE;
		chain newchain;
		newchain.chain_id = 0;
		newchain.chain_count = 0;
		newchain.triggering_effect = replace_effect;
		newchain.triggering_player = e.event_player;
		newchain.evt = e;
		newchain.target_cards = 0;
		newchain.target_player = PLAYER_NONE;
		newchain.target_param = 0;
		newchain.disable_player = PLAYER_NONE;
		newchain.disable_reason = 0;
		newchain.flag = 0;
		core.solving_event.push_front(e);
		core.sub_solving_event.push_back(e);
		core.continuous_chain.push_back(newchain);
		add_process(PROCESSOR_EXECUTE_TARGET, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 6: {
		if (returns.ivalue[0]) {
			card_set::iterator cit, rm;
			uint32 is_destroy = arg2;
			for (cit = targets->container.begin(); cit != targets->container.end();) {
				rm = cit++;
				if (!((*rm)->current.reason & REASON_RULE) && replace_effect->get_value(*rm)) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					if(is_destroy)
						core.destroy_canceled.insert(*rm);
					targets->container.erase(rm);
				}
			}
			replace_effect->dec_count();
		} else
			core.units.begin()->step = 7;
		return FALSE;
	}
	case 7: {
		if(!replace_effect->operation)
			return FALSE;
		core.sub_solving_event.push_back(*core.solving_event.begin());
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 8: {
		if(core.continuous_chain.rbegin()->target_cards)
			pduel->delete_group(core.continuous_chain.rbegin()->target_cards);
		chain::opmap::iterator oit;
		for(oit = core.continuous_chain.rbegin()->opinfos.begin(); oit != core.continuous_chain.rbegin()->opinfos.end(); ++oit) {
			if(oit->second.op_cards)
				pduel->delete_group(oit->second.op_cards);
		}
		core.continuous_chain.pop_back();
		core.solving_event.pop_front();
		return TRUE;
	}
	case 10: {
		if(returns.ivalue[0])
			return TRUE;
		if(!replace_effect->target)
			return TRUE;
		card* target = (card*)arg1;
		event e;
		e.event_cards = targets;
		e.event_player = replace_effect->get_handler_player();
		e.event_value = 0;
		e.reason = target->current.reason;
		e.reason_effect = target->current.reason_effect;
		e.reason_player = target->current.reason_player;
		if(!replace_effect->is_activateable(replace_effect->get_handler_player(), e))
			return TRUE;
		chain newchain;
		newchain.chain_id = 0;
		newchain.chain_count = 0;
		newchain.triggering_effect = replace_effect;
		newchain.triggering_player = e.event_player;
		newchain.evt = e;
		newchain.target_cards = 0;
		newchain.target_player = PLAYER_NONE;
		newchain.target_param = 0;
		newchain.disable_player = PLAYER_NONE;
		newchain.disable_reason = 0;
		newchain.flag = 0;
		core.sub_solving_event.push_back(e);
		core.continuous_chain.push_back(newchain);
		add_process(PROCESSOR_EXECUTE_TARGET, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 11: {
		card* target = (card*)arg1;
		if (returns.ivalue[0]) {
			targets->container.erase(target);
			target->current.reason = target->temp.reason;
			target->current.reason_effect = target->temp.reason_effect;
			target->current.reason_player = target->temp.reason_player;
			if(arg2)
				core.destroy_canceled.insert(target);
			replace_effect->dec_count();
			core.desrep_chain.push_back(core.continuous_chain.front());
		}
		core.continuous_chain.pop_front();
		return TRUE;
	}
	case 12: {
		event e;
		e.event_cards = targets;
		e.event_player = replace_effect->get_handler_player();
		e.event_value = 0;
		if(targets->container.size() == 0)
			return TRUE;
		card* pc = *targets->container.begin();
		e.reason = pc->current.reason;
		e.reason_effect = pc->current.reason_effect;
		e.reason_player = pc->current.reason_player;
		if(!replace_effect->is_activateable(replace_effect->get_handler_player(), e) || !replace_effect->value)
			return TRUE;
		chain newchain;
		newchain.chain_id = 0;
		newchain.chain_count = 0;
		newchain.triggering_effect = replace_effect;
		newchain.triggering_player = e.event_player;
		newchain.evt = e;
		newchain.target_cards = 0;
		newchain.target_player = PLAYER_NONE;
		newchain.target_param = 0;
		newchain.disable_player = PLAYER_NONE;
		newchain.disable_reason = 0;
		newchain.flag = 0;
		core.continuous_chain.push_back(newchain);
		core.sub_solving_event.push_back(e);
		add_process(PROCESSOR_EXECUTE_TARGET, 0, replace_effect, 0, replace_effect->get_handler_player(), 0);
		return FALSE;
	}
	case 13: {
		if (returns.ivalue[0]) {
			card_set::iterator cit, rm;
			uint32 is_destroy = arg2;
			for (cit = targets->container.begin(); cit != targets->container.end();) {
				rm = cit++;
				if (!((*rm)->current.reason & REASON_RULE) && replace_effect->get_value(*rm)) {
					(*rm)->current.reason = (*rm)->temp.reason;
					(*rm)->current.reason_effect = (*rm)->temp.reason_effect;
					(*rm)->current.reason_player = (*rm)->temp.reason_player;
					if(is_destroy)
						core.destroy_canceled.insert(*rm);
					targets->container.erase(rm);
				}
			}
			replace_effect->dec_count();
			core.desrep_chain.push_back(core.continuous_chain.front());
		}
		core.continuous_chain.pop_front();
		return TRUE;
	}
	case 15: {
		if(core.desrep_chain.size() == 0)
			return TRUE;
		core.continuous_chain.push_back(core.desrep_chain.front());
		core.desrep_chain.pop_front();
		effect* reffect = core.continuous_chain.back().triggering_effect;
		if(!reffect->operation)
			return FALSE;
		core.sub_solving_event.push_back(core.continuous_chain.back().evt);
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, reffect, 0, reffect->get_handler_player(), 0);
		return FALSE;
	}
	case 16: {
		if(core.continuous_chain.rbegin()->target_cards)
			pduel->delete_group(core.continuous_chain.rbegin()->target_cards);
		chain::opmap::iterator oit;
		for(oit = core.continuous_chain.rbegin()->opinfos.begin(); oit != core.continuous_chain.rbegin()->opinfos.end(); ++oit) {
			if(oit->second.op_cards)
				pduel->delete_group(oit->second.op_cards);
		}
		core.continuous_chain.pop_back();
		core.units.begin()->step = 14;
		return FALSE;
	}
	}
	return TRUE;
}
int32 field::select_synchro_material(int16 step, uint8 playerid, card* pcard, int32 min) {
	switch(step) {
	case 0: {
		core.select_cards.clear();
		card* tuner;
		effect* peffect;
		for(uint8 p = 0; p < 2; ++p) {
			for(int32 i = 0; i < 5; ++i) {
				tuner = player[p].list_mzone[i];
				if(tuner && tuner->is_position(POS_FACEUP) && (tuner->get_type()&TYPE_TUNER) && pduel->lua->check_matching(tuner, -2, 0)
				        && tuner->is_can_be_synchro_material(pcard)) {
					if((peffect = tuner->is_affected_by_effect(EFFECT_SYNCHRO_MATERIAL_CUSTOM, pcard))) {
						if(!peffect->target)
							continue;
						pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
						pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
						pduel->lua->add_param(-1, PARAM_TYPE_INDEX);
						pduel->lua->add_param(min, PARAM_TYPE_INT);
						if(pduel->lua->check_condition(peffect->target, 4))
							core.select_cards.push_back(tuner);
					} else {
						card_vector nsyn;
						card* pm;
						for(uint8 np = 0; np < 2; ++np) {
							for(int32 j = 0; j < 5; ++j) {
								pm = player[np].list_mzone[j];
								if(pm && pm != tuner && pm->is_position(POS_FACEUP) && pduel->lua->check_matching(pm, -1, 0)
								        && pm->is_can_be_synchro_material(pcard)) {
									nsyn.push_back(pm);
									pm->operation_param = pm->get_synchro_level(pcard);
								}
							}
						}
						int32 l = tuner->get_synchro_level(pcard);
						int32 l1 = l & 0xffff;
						//int32 l2 = l >> 16;
						int32 lv = pcard->get_level();
						if(lv == l1)
							continue;
						if(check_with_sum_limit(&nsyn, lv - l1, 0, 1, min))
							core.select_cards.push_back(tuner);
					}
				}
			}
		}
		if(core.select_cards.size() == 0)
			return TRUE;
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_SELECTMSG);
		pduel->write_buffer8(playerid);
		pduel->write_buffer32(512);
		add_process(PROCESSOR_SELECT_CARD, 0, 0, 0, playerid, 0x10001);
		return FALSE;
	}
	case 1: {
		card* tuner = core.select_cards[returns.bvalue[1]];
		core.units.begin()->ptarget = (group*)tuner;
		effect* peffect;
		if((peffect = tuner->is_affected_by_effect(EFFECT_SYNCHRO_MATERIAL_CUSTOM, pcard))) {
			if(!peffect->operation)
				return FALSE;
			core.synchro_materials.clear();
			pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
			pduel->lua->add_param(-1, PARAM_TYPE_INDEX);
			pduel->lua->add_param(min, PARAM_TYPE_INT);
			core.sub_solving_event.push_back(nil_event);
			add_process(PROCESSOR_EXECUTE_OPERATION, 0, peffect, 0, playerid, 0);
		} else {
			core.select_cards.clear();
			for(uint8 np = 0; np < 2; ++np) {
				for(int32 i = 0; i < 5; ++i) {
					card* pm = player[np].list_mzone[i];
					if(pm && pm != tuner && pm->is_position(POS_FACEUP) && pduel->lua->check_matching(pm, -1, 0)
					        && pm->is_can_be_synchro_material(pcard)) {
						core.select_cards.push_back(pm);
						pm->operation_param = pm->get_synchro_level(pcard);
					}
				}
			}
			int32 l = tuner->get_synchro_level(pcard);
			int32 l1 = l & 0xffff;
			//int32 l2 = l >> 16;
			int32 lv = pcard->get_level();
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_SELECTMSG);
			pduel->write_buffer8(playerid);
			pduel->write_buffer32(512);
			add_process(PROCESSOR_SELECT_SUM, 0, 0, 0, lv - l1, playerid + (min << 16));
			core.units.begin()->step = 2;
		}
		return FALSE;
	}
	case 2: {
		lua_pop(pduel->lua->current_state, 2);
		group* pgroup = pduel->new_group();
		pgroup->container = core.synchro_materials;
		pgroup->container.insert((card*)core.units.begin()->ptarget);
		pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
		return TRUE;
	}
	case 3: {
		lua_pop(pduel->lua->current_state, 2);
		group* pgroup = pduel->new_group();
		card* pcard;
		for(int32 i = 0; i < returns.bvalue[0]; ++i) {
			pcard = core.select_cards[returns.bvalue[i + 1]];
			pgroup->container.insert(pcard);
		}
		pgroup->container.insert((card*)core.units.begin()->ptarget);
		pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::toss_coin(uint16 step, uint8 playerid, uint8 count) {
	switch(step) {
	case 0: {
		effect_set eset;
		effect* peffect = 0;
		event e;
		e.event_cards = 0;
		e.reason_effect = core.reason_effect;
		e.reason_player = core.reason_player;
		e.event_player = playerid;
		e.event_value = count;
		for(uint8 i = 0; i < 5; ++i)
			core.coin_result[i] = 0;
		filter_field_effect(EFFECT_TOSS_COIN_REPLACE, &eset);
		for(int32 i = eset.count - 1; i >= 0; --i) {
			if(eset[i]->is_activateable(eset[i]->get_handler_player(), e)) {
				peffect = eset[i];
				break;
			}
		}
		if(!peffect) {
			for(uint8 i = 0; i < count; ++i)
				core.coin_result[i] = pduel->get_next_integer(0, 1);
			return FALSE;
		} else {
			core.sub_solving_event.push_back(e);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, peffect, 0, peffect->get_handler_player(), 0);
			return TRUE;
		}
	}
	case 1: {
		raise_event((card*)0, EVENT_TOSS_COIN, core.reason_effect, 0, core.reason_player, playerid, count);
		process_instant_event();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::toss_dice(uint16 step, uint8 playerid, uint8 count) {
	switch(step) {
	case 0: {
		effect_set eset;
		effect* peffect = 0;
		event e;
		e.event_cards = 0;
		e.reason_effect = core.reason_effect;
		e.reason_player = core.reason_player;
		e.event_player = playerid;
		e.event_value = count;
		for(uint8 i = 0; i < 5; ++i)
			core.dice_result[i] = 0;
		filter_field_effect(EFFECT_TOSS_DICE_REPLACE, &eset);
		for(int32 i = eset.count - 1; i >= 0; --i) {
			if(eset[i]->is_activateable(eset[i]->get_handler_player(), e)) {
				peffect = eset[i];
				break;
			}
		}
		if(!peffect) {
			for(uint8 i = 0; i < count; ++i)
				core.dice_result[i] = pduel->get_next_integer(1, 6);
			return FALSE;
		} else {
			core.sub_solving_event.push_back(e);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, peffect, 0, peffect->get_handler_player(), 0);
			return TRUE;
		}
	}
	case 1: {
		raise_event((card*)0, EVENT_TOSS_DICE, core.reason_effect, 0, core.reason_player, playerid, count);
		process_instant_event();
		return FALSE;
	}
	}
	return TRUE;
}
