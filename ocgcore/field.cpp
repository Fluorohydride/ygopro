/*
 * field.cpp
 *
 *  Created on: 2010-7-21
 *      Author: Argon
 */

#include "field.h"
#include "duel.h"
#include "card.h"
#include "group.h"
#include "effect.h"
#include "interpreter.h"
#include <iostream>
#include <cstring>
#include <map>

int32 field::field_used_count[32] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5};

bool chain::chain_operation_sort(const chain& c1, const chain& c2) {
	return c1.triggering_effect->id < c2.triggering_effect->id;
}
bool tevent::operator< (const tevent& v) const {
	return memcmp(this, &v, sizeof(tevent)) < 0;
}
field::field(duel* pduel) {
	this->pduel = pduel;
	infos.field_id = 1;
	infos.copy_id = 1;
	infos.shuffle_count = 0;
	infos.turn_id = 0;
	infos.card_id = 1;
	infos.phase = 0;
	infos.turn_player = 0;
	for (int i = 0; i < 2; ++i) {
		cost[i].count = 0;
		cost[i].amount = 0;
		core.hint_timing[i] = 0;
		player[i].lp = 8000;
		player[i].start_count = 5;
		player[i].draw_count = 1;
		player[i].disabled_location = 0;
		player[i].used_location = 0;
		player[i].list_mzone.reserve(5);
		player[i].list_szone.reserve(8);
		player[i].list_main.reserve(45);
		player[i].list_hand.reserve(10);
		player[i].list_grave.reserve(30);
		player[i].list_remove.reserve(30);
		player[i].list_extra.reserve(15);
		for(int j = 0; j < 5; ++j)
			player[i].list_mzone.push_back(0);
		for(int j = 0; j < 8; ++j)
			player[i].list_szone.push_back(0);
		core.shuffle_deck_check[i] = FALSE;
		core.shuffle_hand_check[i] = FALSE;
	}
	core.pre_field[0] = 0;
	core.pre_field[1] = 0;
	for (int i = 0; i < 5; ++i)
		core.opp_mzone[i] = 0;
	core.summoning_card = 0;
	core.summon_depth = 0;
	core.chain_limit = 0;
	core.chain_limit_p = 0;
	core.chain_solving = FALSE;
	core.win_player = 5;
	core.win_reason = 0;
	core.reason_effect = 0;
	core.reason_player = PLAYER_NONE;
	core.selfdes_disabled = FALSE;
	core.flip_delayed = FALSE;
	core.overdraw[0] = FALSE;
	core.overdraw[1] = FALSE;
	core.check_level = 0;
	core.limit_tuner = 0;
	core.limit_xyz = 0;
	core.limit_syn = 0;
	core.duel_options = 0;
	core.attacker = 0;
	core.attack_target = 0;
	core.deck_reversed = FALSE;
	core.remove_brainwashing = FALSE;
	core.effect_damage_step = FALSE;
	core.shuffle_check_disabled = FALSE;
	core.global_flag = 0;
	nil_event.event_code = 0;
	nil_event.event_cards = 0;
	nil_event.event_player = PLAYER_NONE;
	nil_event.event_value = 0;
	nil_event.reason = 0;
	nil_event.reason_effect = 0;
	nil_event.reason_player = PLAYER_NONE;
}
field::~field() {

}
void field::reload_field_info() {
	pduel->write_buffer8(MSG_RELOAD_FIELD);
	card* pcard;
	for(int playerid = 0; playerid < 2; ++playerid) {
		pduel->write_buffer32(player[playerid].lp);
		for(uint32 i = 0; i < 5; ++i) {
			pcard = player[playerid].list_mzone[i];
			if(pcard) {
				pduel->write_buffer8(1);
				pduel->write_buffer8(pcard->current.position);
				pduel->write_buffer8(pcard->xyz_materials.size());
			} else {
				pduel->write_buffer8(0);
			}
		}
		for(uint32 i = 0; i < 8; ++i) {
			pcard = player[playerid].list_szone[i];
			if(pcard) {
				pduel->write_buffer8(1);
				pduel->write_buffer8(pcard->current.position);
			} else {
				pduel->write_buffer8(0);
			}
		}
		pduel->write_buffer8(player[playerid].list_main.size());
		pduel->write_buffer8(player[playerid].list_hand.size());
		pduel->write_buffer8(player[playerid].list_grave.size());
		pduel->write_buffer8(player[playerid].list_remove.size());
		pduel->write_buffer8(player[playerid].list_extra.size());
	}
	pduel->write_buffer8(core.current_chain.size());
	for(auto chit = core.current_chain.begin(); chit != core.current_chain.end(); ++chit) {
		effect* peffect = chit->triggering_effect;
		pduel->write_buffer32(peffect->handler->data.code);
		pduel->write_buffer32(peffect->handler->get_info_location());
		pduel->write_buffer8(chit->triggering_controler);
		pduel->write_buffer8(chit->triggering_location);
		pduel->write_buffer8(chit->triggering_sequence);
		pduel->write_buffer32(peffect->description);
	}
}

void field::add_card(uint8 playerid, card* pcard, uint8 location, uint8 sequence) {
	if (pcard->current.location != 0)
		return;
	if (!is_location_useable(playerid, location, sequence))
		return;
	if ((pcard->data.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)) && (location == LOCATION_HAND || location == LOCATION_DECK)) {
		location = LOCATION_EXTRA;
		pcard->operation_param = (pcard->operation_param & 0x00ffffff) | (POS_FACEDOWN_DEFENCE << 24);
	}
	if ((pcard->data.type & TYPE_PENDULUM) && (location == LOCATION_GRAVE)
	        && !pcard->is_affected_by_effect(EFFECT_CANNOT_TO_DECK) && is_player_can_send_to_deck(playerid, pcard)
	        && (((pcard->previous.location == LOCATION_MZONE) && !pcard->is_status(STATUS_SUMMON_DISABLED))
	        || ((pcard->previous.location == LOCATION_SZONE) && !pcard->is_status(STATUS_ACTIVATE_DISABLED)))) {
		location = LOCATION_EXTRA;
		pcard->operation_param = (pcard->operation_param & 0x00ffffff) | (POS_FACEUP_DEFENCE << 24);
	}
	pcard->current.controler = playerid;
	pcard->current.location = location;
	switch (location) {
	case LOCATION_MZONE:
		player[playerid].list_mzone[sequence] = pcard;
		pcard->current.sequence = sequence;
		break;
	case LOCATION_SZONE:
		player[playerid].list_szone[sequence] = pcard;
		pcard->current.sequence = sequence;
		break;
	case LOCATION_DECK:
		if (sequence == 0) {		//deck top
			player[playerid].list_main.push_back(pcard);
			pcard->current.sequence = player[playerid].list_main.size() - 1;
			pcard->current.position = POS_FACEUP_ATTACK;
		} else if (sequence == 1) {		//deck button
			player[playerid].list_main.insert(player[playerid].list_main.begin(), pcard);
			reset_sequence(playerid, LOCATION_DECK);
			pcard->current.position = POS_FACEDOWN;
		} else {		//deck top & shuffle
			player[playerid].list_main.push_back(pcard);
			pcard->current.sequence = player[playerid].list_main.size() - 1;
			if(!core.shuffle_check_disabled)
				core.shuffle_deck_check[playerid] = TRUE;
			pcard->current.position = POS_FACEDOWN;
		}
		break;
	case LOCATION_HAND:
		player[playerid].list_hand.push_back(pcard);
		pcard->current.sequence = player[playerid].list_hand.size() - 1;
		if(!(pcard->current.reason & REASON_DRAW) && !core.shuffle_check_disabled)
			core.shuffle_hand_check[playerid] = TRUE;
		break;
	case LOCATION_GRAVE:
		player[playerid].list_grave.push_back(pcard);
		pcard->current.sequence = player[playerid].list_grave.size() - 1;
		break;
	case LOCATION_REMOVED:
		player[playerid].list_remove.push_back(pcard);
		pcard->current.sequence = player[playerid].list_remove.size() - 1;
		break;
	case LOCATION_EXTRA:
		player[playerid].list_extra.push_back(pcard);
		pcard->current.sequence = player[playerid].list_extra.size() - 1;
		break;
	}
	pcard->apply_field_effect();
	pcard->fieldid = infos.field_id++;
	pcard->fieldid_r = pcard->fieldid;
	pcard->turnid = infos.turn_id;
	if (location == LOCATION_MZONE)
		player[playerid].used_location |= 1 << sequence;
	if (location == LOCATION_SZONE)
		player[playerid].used_location |= 256 << sequence;
}
void field::remove_card(card* pcard) {
	if (pcard->current.controler == PLAYER_NONE || pcard->current.location == 0)
		return;
	uint8 playerid = pcard->current.controler;
	switch (pcard->current.location) {
	case LOCATION_MZONE:
		player[playerid].list_mzone[pcard->current.sequence] = 0;
		break;
	case LOCATION_SZONE:
		player[playerid].list_szone[pcard->current.sequence] = 0;
		break;
	case LOCATION_DECK:
		player[playerid].list_main.erase(player[playerid].list_main.begin() + pcard->current.sequence);
		reset_sequence(playerid, LOCATION_DECK);
		if(!core.shuffle_check_disabled)
			core.shuffle_deck_check[playerid] = TRUE;
		break;
	case LOCATION_HAND:
		player[playerid].list_hand.erase(player[playerid].list_hand.begin() + pcard->current.sequence);
		reset_sequence(playerid, LOCATION_HAND);
		break;
	case LOCATION_GRAVE:
		player[playerid].list_grave.erase(player[playerid].list_grave.begin() + pcard->current.sequence);
		reset_sequence(playerid, LOCATION_GRAVE);
		break;
	case LOCATION_REMOVED:
		player[playerid].list_remove.erase(player[playerid].list_remove.begin() + pcard->current.sequence);
		reset_sequence(playerid, LOCATION_REMOVED);
		break;
	case LOCATION_EXTRA:
		player[playerid].list_extra.erase(player[playerid].list_extra.begin() + pcard->current.sequence);
		reset_sequence(playerid, LOCATION_EXTRA);
		break;
	}
	pcard->cancel_field_effect();
	if (pcard->current.location == LOCATION_MZONE)
		player[playerid].used_location &= ~(1 << pcard->current.sequence);
	if (pcard->current.location == LOCATION_SZONE)
		player[playerid].used_location &= ~(256 << pcard->current.sequence);
	pcard->previous.controler = pcard->current.controler;
	pcard->previous.location = pcard->current.location;
	pcard->previous.sequence = pcard->current.sequence;
	pcard->previous.position = pcard->current.position;
	pcard->current.controler = PLAYER_NONE;
	pcard->current.location = 0;
	pcard->current.sequence = 0;
}
void field::move_card(uint8 playerid, card* pcard, uint8 location, uint8 sequence) {
	if (!is_location_useable(playerid, location, sequence))
		return;
	uint8 preplayer = pcard->current.controler;
	uint8 presequence = pcard->current.sequence;
	if (pcard->current.location) {
		if (pcard->current.location == location) {
			if (pcard->current.location == LOCATION_DECK) {
				if(preplayer == playerid) {
					pduel->write_buffer8(MSG_MOVE);
					pduel->write_buffer32(pcard->data.code);
					pduel->write_buffer32(pcard->get_info_location());
					player[preplayer].list_main.erase(player[preplayer].list_main.begin() + pcard->current.sequence);
					if (sequence == 0) {		//deck top
						player[playerid].list_main.push_back(pcard);
					} else if (sequence == 1) {
						player[playerid].list_main.insert(player[playerid].list_main.begin(), pcard);
					} else {
						player[playerid].list_main.push_back(pcard);
						if(!core.shuffle_check_disabled)
							core.shuffle_deck_check[playerid] = true;
					}
					reset_sequence(playerid, LOCATION_DECK);
					pcard->previous.controler = preplayer;
					pcard->current.controler = playerid;
					pduel->write_buffer32(pcard->get_info_location());
					pduel->write_buffer32(pcard->current.reason);
					return;
				} else
					remove_card(pcard);
			} else if(location & LOCATION_ONFIELD) {
				if (playerid == preplayer && sequence == presequence)
					return;
				if((location == LOCATION_MZONE && (sequence < 0 || sequence > 4 || player[playerid].list_mzone[sequence]))
				        || (location == LOCATION_SZONE && (sequence < 0 || sequence > 4 || player[playerid].list_szone[sequence])))
					return;
				if(preplayer == playerid) {
					pduel->write_buffer8(MSG_MOVE);
					pduel->write_buffer32(pcard->data.code);
					pduel->write_buffer32(pcard->get_info_location());
				}
				pcard->previous.controler = pcard->current.controler;
				pcard->previous.location = pcard->current.location;
				pcard->previous.sequence = pcard->current.sequence;
				pcard->previous.position = pcard->current.position;
				if (location == LOCATION_MZONE) {
					player[preplayer].list_mzone[presequence] = 0;
					player[preplayer].used_location &= ~(1 << presequence);
					player[playerid].list_mzone[sequence] = pcard;
					player[playerid].used_location |= 1 << sequence;
					pcard->current.controler = playerid;
					pcard->current.sequence = sequence;
				} else {
					player[preplayer].list_szone[presequence] = 0;
					player[preplayer].used_location &= ~(256 << presequence);
					player[playerid].list_szone[sequence] = pcard;
					player[playerid].used_location |= 256 << sequence;
					pcard->current.controler = playerid;
					pcard->current.sequence = sequence;
				}
				if(preplayer == playerid) {
					pduel->write_buffer32(pcard->get_info_location());
					pduel->write_buffer32(pcard->current.reason);
				}
				return;
			} else if(location == LOCATION_HAND) {
				if(preplayer == playerid)
					return;
				remove_card(pcard);
			} else {
				if(location == LOCATION_GRAVE) {
					if(pcard->current.sequence == player[pcard->current.controler].list_grave.size() - 1)
						return;
					pduel->write_buffer8(MSG_MOVE);
					pduel->write_buffer32(pcard->data.code);
					pduel->write_buffer32(pcard->get_info_location());
					player[pcard->current.controler].list_grave.erase(player[pcard->current.controler].list_grave.begin() + pcard->current.sequence);
					player[pcard->current.controler].list_grave.push_back(pcard);
					reset_sequence(pcard->current.controler, LOCATION_GRAVE);
					pduel->write_buffer32(pcard->get_info_location());
					pduel->write_buffer32(pcard->current.reason);
				} else if(location == LOCATION_REMOVED) {
					if(pcard->current.sequence == player[pcard->current.controler].list_remove.size() - 1)
						return;
					pduel->write_buffer8(MSG_MOVE);
					pduel->write_buffer32(pcard->data.code);
					pduel->write_buffer32(pcard->get_info_location());
					player[pcard->current.controler].list_remove.erase(player[pcard->current.controler].list_remove.begin() + pcard->current.sequence);
					player[pcard->current.controler].list_remove.push_back(pcard);
					reset_sequence(pcard->current.controler, LOCATION_REMOVED);
					pduel->write_buffer32(pcard->get_info_location());
					pduel->write_buffer32(pcard->current.reason);
				} else {
					pduel->write_buffer8(MSG_MOVE);
					pduel->write_buffer32(pcard->data.code);
					pduel->write_buffer32(pcard->get_info_location());
					player[pcard->current.controler].list_extra.erase(player[pcard->current.controler].list_extra.begin() + pcard->current.sequence);
					player[pcard->current.controler].list_extra.push_back(pcard);
					reset_sequence(pcard->current.controler, LOCATION_EXTRA);
					pduel->write_buffer32(pcard->get_info_location());
					pduel->write_buffer32(pcard->current.reason);
				}
				return;
			}
		} else
			remove_card(pcard);
	}
	add_card(playerid, pcard, location, sequence);
}
void field::set_control(card* pcard, uint8 playerid, uint16 reset_phase, uint8 reset_count) {
	if(core.remove_brainwashing || pcard->refresh_control_status() == playerid)
		return;
	effect* peffect = pduel->new_effect();
	if(core.reason_effect)
		peffect->owner = core.reason_effect->handler;
	else
		peffect->owner = pcard;
	peffect->handler = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_SET_CONTROL;
	peffect->value = playerid;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT | 0xc6c0000;
	if(reset_count) {
		peffect->reset_flag |= RESET_PHASE | reset_phase;
		if(!(peffect->reset_flag & (RESET_SELF_TURN | RESET_OPPO_TURN)))
			peffect->reset_flag |= (RESET_SELF_TURN | RESET_OPPO_TURN);
		peffect->reset_count |= reset_count & 0xff;
	}
	pcard->add_effect(peffect);
	pcard->current.controler = playerid;
}

card* field::get_field_card(uint8 playerid, uint8 location, uint8 sequence) {
	switch(location) {
	case LOCATION_MZONE: {
		if(sequence < 5)
			return player[playerid].list_mzone[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_SZONE: {
		if(sequence < 8)
			return player[playerid].list_szone[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_DECK: {
		if(sequence < player[playerid].list_main.size())
			return player[playerid].list_main[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_HAND: {
		if(sequence < player[playerid].list_hand.size())
			return player[playerid].list_hand[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_GRAVE: {
		if(sequence < player[playerid].list_grave.size())
			return player[playerid].list_grave[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_REMOVED: {
		if(sequence < player[playerid].list_remove.size())
			return player[playerid].list_remove[sequence];
		else
			return 0;
		break;
	}
	case LOCATION_EXTRA: {
		if(sequence < player[playerid].list_extra.size())
			return player[playerid].list_extra[sequence];
		else
			return 0;
		break;
	}
	}
	return 0;
}
int32 field::is_location_useable(uint8 playerid, uint8 location, uint8 sequence) {
	if (location != LOCATION_MZONE && location != LOCATION_SZONE)
		return TRUE;
	int32 flag = player[playerid].disabled_location | player[playerid].used_location;
	if (location == LOCATION_MZONE && flag & (1 << sequence))
		return FALSE;
	if (location == LOCATION_SZONE && flag & (256 << sequence))
		return FALSE;
	return TRUE;
}
int32 field::get_useable_count(uint8 playerid, uint8 location, uint8 uplayer, uint32 reason, uint32* list) {
	if (location != LOCATION_MZONE && location != LOCATION_SZONE)
		return 0;
	uint32 flag = player[playerid].disabled_location | player[playerid].used_location;
	uint32 used_flag = player[playerid].used_location;
	effect_set eset;
	if (location == LOCATION_MZONE) {
		flag = (flag & 0x1f);
		used_flag = (used_flag & 0x1f);
		if(uplayer < 2)
			filter_player_effect(playerid, EFFECT_MAX_MZONE, &eset);
	} else {
		flag = (flag & 0x1f00) >> 8;
		used_flag = (used_flag & 0x1f00) >> 8;
		if(uplayer < 2)
			filter_player_effect(playerid, EFFECT_MAX_SZONE, &eset);
	}
	if(list)
		*list = flag;
	if(eset.size()) {
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(uplayer, PARAM_TYPE_INT);
		pduel->lua->add_param(reason, PARAM_TYPE_INT);
		int32 max = eset.get_last()->get_value(3);
		int32 block = 5 - field_used_count[flag];
		int32 limit = max - field_used_count[used_flag];
		return block < limit ? block : limit;
	} else {
		return 5 - field_used_count[flag];
	}
}
void field::shuffle(uint8 playerid, uint8 location) {
	if(!(location & (LOCATION_HAND | LOCATION_DECK)))
		return;
	card_vector& svector = (location == LOCATION_HAND) ? player[playerid].list_hand : player[playerid].list_main;
	if(svector.size() == 0)
		return;
	if(location == LOCATION_HAND) {
		bool shuffle = false;
		for(auto cit = svector.begin(); cit != svector.end(); ++cit)
			if(!(*cit)->is_status(STATUS_IS_PUBLIC))
				shuffle = true;
		if(!shuffle) {
			core.shuffle_hand_check[playerid] = FALSE;
			return;
		}
	}
	if(location == LOCATION_HAND || !(core.duel_options & DUEL_PSEUDO_SHUFFLE)) {
		if(svector.size() > 1) {
			uint32 i = 0, s = svector.size(), r;
			for(i = 0; i < s - 1; ++i) {
				r = pduel->get_next_integer(i, s - 1);
				card* t = svector[i];
				svector[i] = svector[r];
				svector[r] = t;
			}
			reset_sequence(playerid, location);
		}
	}
	if(location == LOCATION_HAND) {
		pduel->write_buffer8(MSG_SHUFFLE_HAND);
		pduel->write_buffer8(playerid);
		pduel->write_buffer8(player[playerid].list_hand.size());
		for(auto cit = svector.begin(); cit != svector.end(); ++cit)
			pduel->write_buffer32((*cit)->data.code);
		core.shuffle_hand_check[playerid] = FALSE;
	} else {
		pduel->write_buffer8(MSG_SHUFFLE_DECK);
		pduel->write_buffer8(playerid);
		core.shuffle_deck_check[playerid] = FALSE;
		if(core.global_flag & GLOBALFLAG_DECK_REVERSE_CHECK) {
			card* ptop = svector.back();
			if(core.deck_reversed || (ptop->current.position == POS_FACEUP_DEFENCE)) {
				pduel->write_buffer8(MSG_DECK_TOP);
				pduel->write_buffer8(playerid);
				pduel->write_buffer8(0);
				if(ptop->current.position != POS_FACEUP_DEFENCE)
					pduel->write_buffer32(ptop->data.code);
				else
					pduel->write_buffer32(ptop->data.code | 0x80000000);
			}
		}
	}
}
void field::reset_sequence(uint8 playerid, uint8 location) {
	if(location & (LOCATION_ONFIELD))
		return;
	card_vector::iterator cit;
	uint32 i = 0;
	switch(location) {
	case LOCATION_DECK:
		for(cit = player[playerid].list_main.begin(), i = 0; cit != player[playerid].list_main.end(); ++cit, ++i)
			(*cit)->current.sequence = i;
		break;
	case LOCATION_HAND:
		for(cit = player[playerid].list_hand.begin(), i = 0; cit != player[playerid].list_hand.end(); ++cit, ++i)
			(*cit)->current.sequence = i;
		break;
	case LOCATION_EXTRA:
		for(cit = player[playerid].list_extra.begin(), i = 0; cit != player[playerid].list_extra.end(); ++cit, ++i)
			(*cit)->current.sequence = i;
		break;
	case LOCATION_GRAVE:
		for(cit = player[playerid].list_grave.begin(), i = 0; cit != player[playerid].list_grave.end(); ++cit, ++i)
			(*cit)->current.sequence = i;
		break;
	case LOCATION_REMOVED:
		for(cit = player[playerid].list_remove.begin(), i = 0; cit != player[playerid].list_remove.end(); ++cit, ++i)
			(*cit)->current.sequence = i;
		break;
	}
}
void field::swap_deck_and_grave(uint8 playerid) {
	card_vector::iterator clit;
	for(clit = player[playerid].list_grave.begin(); clit != player[playerid].list_grave.end(); ++clit) {
		(*clit)->previous.location = LOCATION_GRAVE;
		(*clit)->previous.sequence = (*clit)->current.sequence;
		(*clit)->enable_field_effect(false);
		(*clit)->cancel_field_effect();
	}
	for(clit = player[playerid].list_main.begin(); clit != player[playerid].list_main.end(); ++clit) {
		(*clit)->previous.location = LOCATION_DECK;
		(*clit)->previous.sequence = (*clit)->current.sequence;
		(*clit)->enable_field_effect(false);
		(*clit)->cancel_field_effect();
	}
	player[playerid].list_grave.swap(player[playerid].list_main);
	card_vector ex;
	for(clit = player[playerid].list_main.begin(); clit != player[playerid].list_main.end(); ) {
		if((*clit)->data.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)) {
			ex.push_back(*clit);
			clit = player[playerid].list_main.erase(clit);
		} else
			++clit;
	}
	for(clit = player[playerid].list_grave.begin(); clit != player[playerid].list_grave.end(); ++clit) {
		(*clit)->current.location = LOCATION_GRAVE;
		(*clit)->current.reason = REASON_EFFECT;
		(*clit)->current.reason_effect = core.reason_effect;
		(*clit)->current.reason_player = core.reason_player;
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
		(*clit)->reset(RESET_TOGRAVE, RESET_EVENT);
	}
	for(clit = player[playerid].list_main.begin(); clit != player[playerid].list_main.end(); ++clit) {
		(*clit)->current.location = LOCATION_DECK;
		(*clit)->current.reason = REASON_EFFECT;
		(*clit)->current.reason_effect = core.reason_effect;
		(*clit)->current.reason_player = core.reason_player;
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
		(*clit)->reset(RESET_TODECK, RESET_EVENT);
	}
	for(clit = ex.begin(); clit != ex.end(); ++clit) {
		(*clit)->current.location = LOCATION_EXTRA;
		(*clit)->current.reason = REASON_EFFECT;
		(*clit)->current.reason_effect = core.reason_effect;
		(*clit)->current.reason_player = core.reason_player;
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
		(*clit)->reset(RESET_TODECK, RESET_EVENT);
	}
	player[playerid].list_extra.insert(player[playerid].list_extra.end(), ex.begin(), ex.end());
	reset_sequence(playerid, LOCATION_GRAVE);
	reset_sequence(playerid, LOCATION_EXTRA);
	pduel->write_buffer8(MSG_SWAP_GRAVE_DECK);
	pduel->write_buffer8(playerid);
	shuffle(playerid, LOCATION_DECK);
}
void field::reverse_deck(uint8 playerid) {
	int32 count = player[playerid].list_main.size();
	if(count == 0)
		return;
	for(int i = 0; i < count / 2; ++i) {
		card* tmp = player[playerid].list_main[i];
		tmp->current.sequence = count - 1 - i;
		player[playerid].list_main[count - 1 - i]->current.sequence = i;
		player[playerid].list_main[i] = player[playerid].list_main[count - 1 - i];
		player[playerid].list_main[count - 1 - i] = tmp;
	}
}
void field::tag_swap(uint8 playerid) {
	card_vector::iterator clit;
	//main
	for(clit = player[playerid].list_main.begin(); clit != player[playerid].list_main.end(); ++clit) {
		(*clit)->enable_field_effect(false);
		(*clit)->cancel_field_effect();
	}
	std::swap(player[playerid].list_main, player[playerid].tag_list_main);
	for(clit = player[playerid].list_main.begin(); clit != player[playerid].list_main.end(); ++clit) {
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
	}
	//hand
	for(clit = player[playerid].list_hand.begin(); clit != player[playerid].list_hand.end(); ++clit) {
		(*clit)->enable_field_effect(false);
		(*clit)->cancel_field_effect();
	}
	std::swap(player[playerid].list_hand, player[playerid].tag_list_hand);
	for(clit = player[playerid].list_hand.begin(); clit != player[playerid].list_hand.end(); ++clit) {
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
	}
	//extra
	for(clit = player[playerid].list_extra.begin(); clit != player[playerid].list_extra.end(); ++clit) {
		(*clit)->enable_field_effect(false);
		(*clit)->cancel_field_effect();
	}
	std::swap(player[playerid].list_extra, player[playerid].tag_list_extra);
	for(clit = player[playerid].list_extra.begin(); clit != player[playerid].list_extra.end(); ++clit) {
		(*clit)->apply_field_effect();
		(*clit)->enable_field_effect(true);
	}
	pduel->write_buffer8(MSG_TAG_SWAP);
	pduel->write_buffer8(playerid);
	pduel->write_buffer8(player[playerid].list_main.size());
	pduel->write_buffer8(player[playerid].list_extra.size());
	pduel->write_buffer8(player[playerid].list_hand.size());
	if(core.deck_reversed && player[playerid].list_main.size())
		pduel->write_buffer32(player[playerid].list_main.back()->data.code);
	else
		pduel->write_buffer32(0);
	for(auto cit = player[playerid].list_hand.begin(); cit != player[playerid].list_hand.end(); ++cit)
		pduel->write_buffer32((*cit)->data.code | ((*cit)->is_status(STATUS_IS_PUBLIC) ? 0x80000000 : 0));
}
void field::add_effect(effect* peffect, uint8 owner_player) {
	if (!peffect->handler) {
		peffect->flag |= EFFECT_FLAG_FIELD_ONLY;
		peffect->handler = peffect->owner;
		peffect->effect_owner = owner_player;
		peffect->id = infos.field_id++;
	}
	peffect->card_type = peffect->owner->data.type;
	effect_container::iterator it;
	if (!(peffect->type & EFFECT_TYPE_ACTIONS))
		it = effects.aura_effect.insert(make_pair(peffect->code, peffect));
	else {
		if (peffect->type & EFFECT_TYPE_IGNITION)
			it = effects.ignition_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_ACTIVATE)
			it = effects.activate_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_TRIGGER_O && peffect->type & EFFECT_TYPE_FIELD)
			it = effects.trigger_o_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_TRIGGER_F && peffect->type & EFFECT_TYPE_FIELD)
			it = effects.trigger_f_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_QUICK_O)
			it = effects.quick_o_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_QUICK_F)
			it = effects.quick_f_effect.insert(make_pair(peffect->code, peffect));
		else if (peffect->type & EFFECT_TYPE_CONTINUOUS)
			it = effects.continuous_effect.insert(make_pair(peffect->code, peffect));
	}
	effects.indexer.insert(make_pair(peffect, it));
	if((peffect->flag & EFFECT_FLAG_FIELD_ONLY)) {
		if(peffect->flag & EFFECT_FLAG_OATH)
			effects.oath.insert(make_pair(peffect, core.reason_effect));
		if(peffect->reset_flag & RESET_PHASE)
			effects.pheff.insert(peffect);
		if(peffect->reset_flag & RESET_CHAIN)
			effects.cheff.insert(peffect);
		if(peffect->flag & EFFECT_FLAG_COUNT_LIMIT)
			effects.rechargeable.insert(peffect);
	}
}
void field::remove_effect(effect* peffect) {
	auto eit = effects.indexer.find(peffect);
	if (eit == effects.indexer.end())
		return;
	auto it = eit->second;
	if (!(peffect->type & EFFECT_TYPE_ACTIONS))
		effects.aura_effect.erase(it);
	else {
		if (peffect->type & EFFECT_TYPE_IGNITION)
			effects.ignition_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_ACTIVATE)
			effects.activate_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_TRIGGER_O)
			effects.trigger_o_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_TRIGGER_F)
			effects.trigger_f_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_QUICK_O)
			effects.quick_o_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_QUICK_F)
			effects.quick_f_effect.erase(it);
		else if (peffect->type & EFFECT_TYPE_CONTINUOUS)
			effects.continuous_effect.erase(it);
	}
	effects.indexer.erase(peffect);
	if((peffect->flag & EFFECT_FLAG_FIELD_ONLY)) {
		if(peffect->flag & EFFECT_FLAG_OATH)
			effects.oath.erase(peffect);
		if(peffect->reset_flag & RESET_PHASE)
			effects.pheff.erase(peffect);
		if(peffect->reset_flag & RESET_CHAIN)
			effects.cheff.erase(peffect);
		if(peffect->flag & EFFECT_FLAG_COUNT_LIMIT)
			effects.rechargeable.erase(peffect);
		core.reseted_effects.insert(peffect);
	}
}
void field::remove_oath_effect(effect* reason_effect) {
	for(auto oeit = effects.oath.begin(); oeit != effects.oath.end();) {
		auto rm = oeit++;
		if(rm->second == reason_effect) {
			effect* peffect = rm->first;
			effects.oath.erase(rm);
			if(peffect->flag & EFFECT_FLAG_FIELD_ONLY)
				remove_effect(peffect);
			else
				peffect->handler->remove_effect(peffect);
		}
	}
}
void field::reset_effect(uint32 id, uint32 reset_type) {
	int32 result;
	for (auto it = effects.indexer.begin(); it != effects.indexer.end();) {
		auto rm = it++;
		auto peffect = rm->first;
		auto pit = rm->second;
		if (!(peffect->flag & EFFECT_FLAG_FIELD_ONLY))
			continue;
		result = peffect->reset(id, reset_type);
		if (result) {
			if (!(peffect->type & EFFECT_TYPE_ACTIONS)) {
				if (peffect->is_disable_related())
					update_disable_check_list(peffect);
				effects.aura_effect.erase(pit);
			} else {
				if (peffect->type & EFFECT_TYPE_IGNITION)
					effects.ignition_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_ACTIVATE)
					effects.activate_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_TRIGGER_O)
					effects.trigger_o_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_TRIGGER_F)
					effects.trigger_f_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_QUICK_O)
					effects.quick_o_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_QUICK_F)
					effects.quick_f_effect.erase(pit);
				else if (peffect->type & EFFECT_TYPE_CONTINUOUS)
					effects.continuous_effect.erase(pit);
			}
			effects.indexer.erase(peffect);
			pduel->delete_effect(peffect);
		}
	}
}
void field::reset_phase(uint32 phase) {
	for(auto eit = effects.pheff.begin(); eit != effects.pheff.end();) {
		auto rm = eit++;
		if((*rm)->reset(phase, RESET_PHASE)) {
			if((*rm)->flag & EFFECT_FLAG_FIELD_ONLY)
				remove_effect((*rm));
			else
				(*rm)->handler->remove_effect((*rm));
		}
	}
}
void field::reset_chain() {
	for(auto eit = effects.cheff.begin(); eit != effects.cheff.end();) {
		auto rm = eit++;
		if((*rm)->flag & EFFECT_FLAG_FIELD_ONLY)
			remove_effect((*rm));
		else
			(*rm)->handler->remove_effect((*rm));
	}
}
void field::add_effect_code(uint32 code, uint32 playerid) {
	auto& count_map = (code & EFFECT_COUNT_CODE_DUEL) ? core.effect_count_code_duel : core.effect_count_code;
	count_map[code + (playerid << 30)]++;
}
uint32 field::get_effect_code(uint32 code, uint32 playerid) {
	auto& count_map = (code & EFFECT_COUNT_CODE_DUEL) ? core.effect_count_code_duel : core.effect_count_code;
	auto iter = count_map.find(code + (playerid << 30));
	if(iter == count_map.end())
		return 0;
	return iter->second;
}
void field::dec_effect_code(uint32 code, uint32 playerid) {
	auto& count_map = (code & EFFECT_COUNT_CODE_DUEL) ? core.effect_count_code_duel : core.effect_count_code;
	auto iter = count_map.find(code + (playerid << 30));
	if(iter == count_map.end())
		return;
	if(iter->second > 0)
		iter->second--;
}
void field::filter_field_effect(uint32 code, effect_set* eset, uint8 sort) {
	effect* peffect;
	auto rg = effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ) {
		peffect = rg.first->second;
		++rg.first;
		if (peffect->is_available())
			eset->add_item(peffect);
	}
	if(sort)
		eset->sort();
}
void field::filter_affected_cards(effect* peffect, card_set* cset) {
	if ((peffect->type & EFFECT_TYPE_ACTIONS) || !(peffect->type & EFFECT_TYPE_FIELD) || (peffect->flag & EFFECT_FLAG_PLAYER_TARGET))
		return;
	uint8 self = peffect->get_handler_player();
	if(self == PLAYER_NONE)
		return;
	card* pcard;
	card_vector::iterator it;
	uint16 range = peffect->s_range;
	for(uint32 p = 0; p < 2; ++p) {
		if (range & LOCATION_MZONE) {
			for (int i = 0; i < 5; ++i) {
				pcard = player[self].list_mzone[i];
				if (pcard && peffect->is_target(pcard))
					cset->insert(pcard);
			}
		}
		if (range & LOCATION_SZONE) {
			for (int i = 0; i < 8; ++i) {
				pcard = player[self].list_szone[i];
				if (pcard && peffect->is_target(pcard))
					cset->insert(pcard);
			}
		}
		if (range & LOCATION_GRAVE) {
			for (it = player[self].list_grave.begin(); it != player[self].list_grave.end(); ++it) {
				pcard = *it;
				if (peffect->is_target(pcard))
					cset->insert(pcard);
			}
		}
		if (range & LOCATION_REMOVED) {
			for (it = player[self].list_remove.begin(); it != player[self].list_remove.end(); ++it) {
				pcard = *it;
				if (peffect->is_target(pcard))
					cset->insert(pcard);
			}
		}
		if (range & LOCATION_HAND) {
			for (it = player[self].list_hand.begin(); it != player[self].list_hand.end(); ++it) {
				pcard = *it;
				if (peffect->is_target(pcard))
					cset->insert(pcard);
			}
		}
		range = peffect->o_range;
		self = 1 - self;
	}
}
void field::filter_player_effect(uint8 playerid, uint32 code, effect_set* eset, uint8 sort) {
	auto rg = effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		effect* peffect = rg.first->second;
		if (peffect->is_target_player(playerid) && peffect->is_available())
			eset->add_item(peffect);
	}
	if(sort)
		eset->sort();
}
int32 field::filter_matching_card(int32 findex, uint8 self, uint32 location1, uint32 location2, group* pgroup, card* pexception, uint32 extraargs, card** pret, int32 fcount, int32 is_target) {
	if(self != 0 && self != 1)
		return FALSE;
	card* pcard;
	int32 count = 0;
	uint32 location = location1;
	for(uint32 p = 0; p < 2; ++p) {
		if(location & LOCATION_MZONE) {
			for(uint32 i = 0; i < 5; ++i) {
				pcard = player[self].list_mzone[i];
				if(pcard && !pcard->is_status(STATUS_SUMMONING) && !pcard->is_status(STATUS_SUMMON_DISABLED) && pcard != pexception
				        && pduel->lua->check_matching(pcard, findex, extraargs) && (!is_target || pcard->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = pcard;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(pcard);
					}
				}
			}
		}
		if(location & LOCATION_SZONE) {
			for(uint32 i = 0; i < 8; ++i) {
				pcard = player[self].list_szone[i];
				if(pcard && pcard != pexception && pduel->lua->check_matching(pcard, findex, extraargs)
				        && (!is_target || pcard->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = pcard;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(pcard);
					}
				}
			}
		}
		if(location & LOCATION_DECK) {
			for(auto cit = player[self].list_main.rbegin(); cit != player[self].list_main.rend(); ++cit) {
				if(*cit != pexception && pduel->lua->check_matching(*cit, findex, extraargs)
				        && (!is_target || (*cit)->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = *cit;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(*cit);
					}
				}
			}
		}
		if(location & LOCATION_EXTRA) {
			for(auto cit = player[self].list_extra.rbegin(); cit != player[self].list_extra.rend(); ++cit) {
				if(*cit != pexception && pduel->lua->check_matching(*cit, findex, extraargs)
				        && (!is_target || (*cit)->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = *cit;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(*cit);
					}
				}
			}
		}
		if(location & LOCATION_HAND) {
			for(auto cit = player[self].list_hand.begin(); cit != player[self].list_hand.end(); ++cit) {
				if(*cit != pexception && pduel->lua->check_matching(*cit, findex, extraargs)
				        && (!is_target || (*cit)->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = *cit;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(*cit);
					}
				}
			}
		}
		if(location & LOCATION_GRAVE) {
			for(auto cit = player[self].list_grave.rbegin(); cit != player[self].list_grave.rend(); ++cit) {
				if(*cit != pexception && pduel->lua->check_matching(*cit, findex, extraargs)
				        && (!is_target || (*cit)->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = *cit;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(*cit);
					}
				}
			}
		}
		if(location & LOCATION_REMOVED) {
			for(auto cit = player[self].list_remove.rbegin(); cit != player[self].list_remove.rend(); ++cit) {
				if(*cit != pexception && pduel->lua->check_matching(*cit, findex, extraargs)
				        && (!is_target || (*cit)->is_capable_be_effect_target(core.reason_effect, core.reason_player))) {
					if(pret) {
						*pret = *cit;
						return TRUE;
					}
					count ++;
					if(fcount && count >= fcount)
						return TRUE;
					if(pgroup) {
						pgroup->container.insert(*cit);
					}
				}
			}
		}
		location = location2;
		self = 1 - self;
	}
	return FALSE;
}
int32 field::filter_field_card(uint8 self, uint32 location1, uint32 location2, group* pgroup) {
	if(self != 0 && self != 1)
		return 0;
	uint32 location = location1;
	uint32 count = 0;
	card* pcard;
	for(uint32 p = 0; p < 2; ++p) {
		if(location & LOCATION_MZONE) {
			for(int i = 0; i < 5; ++i) {
				pcard = player[self].list_mzone[i];
				if(pcard && !pcard->is_status(STATUS_SUMMONING)) {
					if(pgroup)
						pgroup->container.insert(pcard);
					count++;
				}
			}
		}
		if(location & LOCATION_SZONE) {
			for(int i = 0; i < 8; ++i) {
				pcard = player[self].list_szone[i];
				if(pcard) {
					if(pgroup)
						pgroup->container.insert(pcard);
					count++;
				}
			}
		}
		if(location & LOCATION_HAND) {
			if(pgroup)
				pgroup->container.insert(player[self].list_hand.begin(), player[self].list_hand.end());
			count += player[self].list_hand.size();
		}
		if(location & LOCATION_DECK) {
			if(pgroup)
				pgroup->container.insert(player[self].list_main.rbegin(), player[self].list_main.rend());
			count += player[self].list_main.size();
		}
		if(location & LOCATION_EXTRA) {
			if(pgroup)
				pgroup->container.insert(player[self].list_extra.rbegin(), player[self].list_extra.rend());
			count += player[self].list_extra.size();
		}
		if(location & LOCATION_GRAVE) {
			if(pgroup)
				pgroup->container.insert(player[self].list_grave.rbegin(), player[self].list_grave.rend());
			count += player[self].list_grave.size();
		}
		if(location & LOCATION_REMOVED) {
			if(pgroup)
				pgroup->container.insert(player[self].list_remove.rbegin(), player[self].list_remove.rend());
			count += player[self].list_remove.size();
		}
		location = location2;
		self = 1 - self;
	}
	return count;
}
effect* field::is_player_affected_by_effect(uint8 playerid, uint32 code) {
	auto rg = effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		effect* peffect = rg.first->second;
		if (peffect->is_target_player(playerid) && peffect->is_available())
			return peffect;
	}
	return 0;
}
int32 field::get_release_list(uint8 playerid, card_set* release_list, card_set* ex_list, int32 use_con, int32 use_hand, int32 fun, int32 exarg, card* exp) {
	card* pcard;
	uint32 rcount = 0;
	for(uint32 i = 0; i < 5; ++i) {
		pcard = player[playerid].list_mzone[i];
		if(pcard && pcard != exp && pcard->is_releasable_by_nonsummon(playerid)
		        && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
			if(release_list)
				release_list->insert(pcard);
			pcard->operation_param = 1;
			rcount++;
		}
	}
	if(use_hand) {
		for(uint32 i = 0; i < player[playerid].list_hand.size(); ++i) {
			pcard = player[playerid].list_hand[i];
			if(pcard && pcard != exp && pcard->is_releasable_by_nonsummon(playerid)
			        && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
				if(release_list)
					release_list->insert(pcard);
				pcard->operation_param = 1;
				rcount++;
			}
		}
	}
	for(uint32 i = 0; i < 5; ++i) {
		pcard = player[1 - playerid].list_mzone[i];
		if(pcard && pcard != exp && (pcard->is_position(POS_FACEUP) || !use_con) && pcard->is_affected_by_effect(EFFECT_EXTRA_RELEASE)
		        && pcard->is_releasable_by_nonsummon(playerid) && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
			if(ex_list)
				ex_list->insert(pcard);
			pcard->operation_param = 1;
			rcount++;
		}
	}
	return rcount;
}
int32 field::check_release_list(uint8 playerid, int32 count, int32 use_con, int32 use_hand, int32 fun, int32 exarg, card* exp) {
	card* pcard;
	for(uint32 i = 0; i < 5; ++i) {
		pcard = player[playerid].list_mzone[i];
		if(pcard && pcard != exp && pcard->is_releasable_by_nonsummon(playerid)
		        && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
			count--;
			if(count == 0)
				return TRUE;
		}
	}
	if(use_hand) {
		for(uint32 i = 0; i < player[playerid].list_hand.size(); ++i) {
			pcard = player[playerid].list_hand[i];
			if(pcard && pcard != exp && pcard->is_releasable_by_nonsummon(playerid)
			        && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
				count--;
				if(count == 0)
					return TRUE;
			}
		}
	}
	for(uint32 i = 0; i < 5; ++i) {
		pcard = player[1 - playerid].list_mzone[i];
		if(pcard && pcard != exp && (!use_con || pcard->is_position(POS_FACEUP)) && pcard->is_affected_by_effect(EFFECT_EXTRA_RELEASE)
		        && pcard->is_releasable_by_nonsummon(playerid) && (!use_con || pduel->lua->check_matching(pcard, fun, exarg))) {
			count--;
			if(count == 0)
				return TRUE;
		}
	}
	return FALSE;
}
int32 field::get_summon_release_list(card* target, card_set* release_list, card_set* ex_list, card_set* ex_list_sum, group* mg) {
	uint8 p = target->current.controler;
	card* pcard;
	uint32 rcount = 0;
	for(int i = 0; i < 5; ++i) {
		pcard = player[p].list_mzone[i];
		if(pcard && pcard->is_releasable_by_summon(p, target)) {
			if(mg && !mg->has_card(pcard))
				continue;
			if(release_list)
				release_list->insert(pcard);
			if(pcard->is_affected_by_effect(EFFECT_DOUBLE_TRIBUTE, target))
				pcard->operation_param = 2;
			else
				pcard->operation_param = 1;
			rcount += pcard->operation_param;
		}
	}
	uint32 ex_sum_max = 0;
	for(int i = 0; i < 5; ++i) {
		pcard = player[1 - p].list_mzone[i];
		if(!(pcard && pcard->is_releasable_by_summon(p, target)))
			continue;
		if(mg && !mg->has_card(pcard))
			continue;
		if(pcard->is_affected_by_effect(EFFECT_EXTRA_RELEASE)) {
			if(ex_list)
				ex_list->insert(pcard);
			if(pcard->is_affected_by_effect(EFFECT_DOUBLE_TRIBUTE, target))
				pcard->operation_param = 2;
			else
				pcard->operation_param = 1;
			rcount += pcard->operation_param;
		} else {
			effect* peffect = pcard->is_affected_by_effect(EFFECT_EXTRA_RELEASE_SUM);
			if(!peffect || ((peffect->flag & EFFECT_FLAG_COUNT_LIMIT) && (peffect->reset_count & 0xf00) == 0))
				continue;
			if(ex_list_sum)
				ex_list_sum->insert(pcard);
			if(pcard->is_affected_by_effect(EFFECT_DOUBLE_TRIBUTE, target))
				pcard->operation_param = 2;
			else
				pcard->operation_param = 1;
			if(ex_sum_max < pcard->operation_param)
				ex_sum_max = pcard->operation_param;
		}
	}
	return rcount + ex_sum_max;
}
int32 field::get_summon_count_limit(uint8 playerid) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_SET_SUMMON_COUNT_LIMIT, &eset);
	int32 count = 1, c;
	for(int32 i = 0; i < eset.size(); ++i) {
		c = eset[i]->get_value();
		if(c > count)
			count = c;
	}
	return count;
}
int32 field::get_draw_count(uint8 playerid) {
	effect_set eset;
	filter_player_effect(infos.turn_player, EFFECT_DRAW_COUNT, &eset);
	int32 count = player[playerid].draw_count;
	if(eset.size())
		count = eset.get_last()->get_value();
	return count;
}
void field::get_ritual_material(uint8 playerid, effect* peffect, card_set* material) {
	card* pcard;
	for(int i = 0; i < 5; ++i) {
		pcard = player[playerid].list_mzone[i];
		if(pcard && pcard->get_level() && pcard->is_affect_by_effect(core.reason_effect)
		        && pcard->is_releasable_by_nonsummon(playerid) && pcard->is_releasable_by_effect(playerid, peffect))
			material->insert(pcard);
	}
	for(int i = 0; i < 5; ++i) {
		pcard = player[1 - playerid].list_mzone[i];
		if(pcard && pcard->get_level() && pcard->is_affect_by_effect(core.reason_effect)
		        && pcard->is_affected_by_effect(EFFECT_EXTRA_RELEASE)
		        && pcard->is_releasable_by_nonsummon(playerid) && pcard->is_releasable_by_effect(playerid, peffect))
			material->insert(pcard);
	}
	for(auto cit = player[playerid].list_hand.begin(); cit != player[playerid].list_hand.end(); ++cit)
		if(((*cit)->data.type & TYPE_MONSTER) && (*cit)->is_releasable_by_nonsummon(playerid))
			material->insert((*cit));
	for(auto cit = player[playerid].list_grave.begin(); cit != player[playerid].list_grave.end(); ++cit)
		if(((*cit)->data.type & TYPE_MONSTER) && (*cit)->is_affected_by_effect(EFFECT_EXTRA_RITUAL_MATERIAL) && (*cit)->is_removeable(playerid))
			material->insert((*cit));
}
void field::ritual_release(card_set* material) {
	card_set rel;
	card_set rem;
	for(auto cit = material->begin(); cit != material->end(); ++cit) {
		if((*cit)->current.location == LOCATION_GRAVE)
			rem.insert(*cit);
		else
			rel.insert(*cit);
	}
	release(&rel, core.reason_effect, REASON_RITUAL + REASON_EFFECT + REASON_MATERIAL, core.reason_player);
	send_to(&rem, core.reason_effect, REASON_RITUAL + REASON_EFFECT + REASON_MATERIAL, core.reason_player, PLAYER_NONE, LOCATION_REMOVED, 0, POS_FACEUP);
}
void field::get_xyz_material(card* scard, int32 findex, uint32 lv, int32 maxc) {
	card* pcard = 0;
	int32 playerid = scard->current.controler;
	core.xmaterial_lst.clear();
	uint32 xyz_level;
	for(int i = 0; i < 5; ++i) {
		pcard = player[playerid].list_mzone[i];
		if(pcard && pcard->is_position(POS_FACEUP) && pcard->is_can_be_xyz_material(scard) && (xyz_level = pcard->check_xyz_level(scard, lv))
				&& (findex == 0 || pduel->lua->check_matching(pcard, findex, 0)))
			core.xmaterial_lst.insert(std::make_pair((xyz_level >> 12) & 0xf, pcard));
	}
	for(int i = 0; i < 5; ++i) {
		pcard = player[1 - playerid].list_mzone[i];
		if(pcard && pcard->is_position(POS_FACEUP) && pcard->is_can_be_xyz_material(scard) && (xyz_level = pcard->check_xyz_level(scard, lv))
		        && pcard->is_affected_by_effect(EFFECT_XYZ_MATERIAL) && (findex == 0 || pduel->lua->check_matching(pcard, findex, 0)))
			core.xmaterial_lst.insert(std::make_pair((xyz_level >> 12) & 0xf, pcard));
	}
	if(core.global_flag & GLOBALFLAG_XMAT_COUNT_LIMIT) {
		auto iter = core.xmaterial_lst.begin();
		while((iter != core.xmaterial_lst.end()) && ((iter->first > (int32)core.xmaterial_lst.size()) || (iter->first > maxc)))
			core.xmaterial_lst.erase(iter++);
	}
}
void field::get_overlay_group(uint8 self, uint8 s, uint8 o, card_set* pset) {
	uint8 c = s;
	card* pcard;
	for(int p = 0; p < 2; ++p) {
		if(c) {
			for(int i = 0; i < 5; ++i) {
				pcard = player[self].list_mzone[i];
				if(pcard && !pcard->is_status(STATUS_SUMMONING) && pcard->xyz_materials.size())
					pset->insert(pcard->xyz_materials.begin(), pcard->xyz_materials.end());
			}
		}
		self = 1 - self;
		c = o;
	}
}
int32 field::get_overlay_count(uint8 self, uint8 s, uint8 o) {
	uint8 c = s;
	uint32 count = 0;
	for(int p = 0; p < 2; ++p) {
		if(c) {
			for(int i = 0; i < 5; ++i) {
				card* pcard = player[self].list_mzone[i];
				if(pcard && !pcard->is_status(STATUS_SUMMONING))
					count += pcard->xyz_materials.size();
			}
		}
		self = 1 - self;
		c = o;
	}
	return count;
}
void field::update_disable_check_list(effect* peffect) {
	card_set cset;
	card_set::iterator it;
	filter_affected_cards(peffect, &cset);
	for (it = cset.begin(); it != cset.end(); ++it)
		add_to_disable_check_list(*it);
}
void field::add_to_disable_check_list(card* pcard) {
	if (effects.disable_check_set.find(pcard) != effects.disable_check_set.end())
		return;
	effects.disable_check_set.insert(pcard);
	effects.disable_check_list.push_back(pcard);
}
void field::adjust_disable_check_list() {
	card* checking;
	int32 pre_disable, new_disable;
	if (!effects.disable_check_list.size())
		return;
	card_set checked;
	do {
		checked.clear();
		while (effects.disable_check_list.size()) {
			checking = effects.disable_check_list.front();
			effects.disable_check_list.pop_front();
			effects.disable_check_set.erase(checking);
			checked.insert(checking);
			if (checking->is_status(STATUS_TO_ENABLE + STATUS_TO_DISABLE))
				continue;
			pre_disable = checking->is_status(STATUS_DISABLED);
			checking->refresh_disable_status();
			new_disable = checking->is_status(STATUS_DISABLED);
			if (pre_disable != new_disable && checking->is_status(STATUS_EFFECT_ENABLED)) {
				checking->filter_disable_related_cards();
				if (pre_disable)
					checking->set_status(STATUS_TO_ENABLE, TRUE);
				else
					checking->set_status(STATUS_TO_DISABLE, TRUE);
			}
		}
		for (card_set::iterator it = checked.begin(); it != checked.end(); ++it) {
			if((*it)->is_status(STATUS_DISABLED) && (*it)->is_status(STATUS_TO_DISABLE) && !(*it)->is_status(STATUS_TO_ENABLE))
				(*it)->reset(RESET_DISABLE, RESET_EVENT);
			(*it)->set_status(STATUS_TO_ENABLE + STATUS_TO_DISABLE, FALSE);
		}
	} while(effects.disable_check_list.size());
}

void field::add_unique_card(card* pcard) {
	uint8 con = pcard->current.controler;
	if(pcard->unique_pos[0])
		core.unique_cards[con].insert(pcard);
	if(pcard->unique_pos[1])
		core.unique_cards[1 - con].insert(pcard);
	pcard->unique_uid = infos.copy_id++;
}

void field::remove_unique_card(card* pcard) {
	uint8 con = pcard->current.controler;
	if(con == PLAYER_NONE)
		return;
	if(pcard->unique_pos[0])
		core.unique_cards[con].erase(pcard);
	if(pcard->unique_pos[1])
		core.unique_cards[1 - con].erase(pcard);
}

effect* field::check_unique_onfield(card* pcard, uint8 controler) {
	if(!pcard->unique_code)
		return 0;
	for(auto iter = core.unique_cards[controler].begin(); iter != core.unique_cards[controler].end(); ++iter) {
		card* ucard = *iter;
		if((ucard != pcard) && ucard->get_status(STATUS_EFFECT_ENABLED) && (ucard->unique_code == pcard->unique_code)
			&& (!(pcard->current.location & LOCATION_ONFIELD) || pcard->is_position(POS_FACEDOWN) || (ucard->unique_uid < pcard->unique_uid)))
			return pcard->unique_effect;
	}
	return 0;
}

int32 field::check_spsummon_once(card* pcard, uint8 playerid) {
	if(pcard->spsummon_code == 0)
		return TRUE;
	auto iter = core.spsummon_once_map[playerid].find(pcard->spsummon_code);
	return (iter == core.spsummon_once_map[playerid].end()) || (iter->second == 0);
}

void field::check_card_counter(card* pcard, int32 counter_type, int32 playerid) {
	auto& counter_map = (counter_type == 1) ? core.summon_counter :
						(counter_type == 2) ? core.normalsummon_counter :
						(counter_type == 3) ? core.spsummon_counter :
						(counter_type == 4) ? core.flipsummon_counter : core.attack_counter;
	for(auto iter = counter_map.begin(); iter != counter_map.end(); ++iter) {
		auto& info = iter->second;
		if((playerid == 0) && (info.second & 0xffff) != 0)
			continue;
		if((playerid == 1) && (info.second & 0xffff0000) != 0)
			continue;
		if(info.first) {
			pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
			if(!pduel->lua->check_condition(info.first, 1)) {
				if(playerid == 0)
					info.second += 0x1;
				else
					info.second += 0x10000;
			}
		}
	}
}
void field::check_chain_counter(effect* peffect, int32 playerid, int32 chainid, bool cancel) {
	for(auto iter = core.chain_counter.begin(); iter != core.chain_counter.end(); ++iter) {
		auto& info = iter->second;
		if(info.first) {
			pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(playerid, PARAM_TYPE_INT);
			pduel->lua->add_param(chainid, PARAM_TYPE_INT);
			if(!pduel->lua->check_condition(info.first, 3)) {
				if(playerid == 0) {
					if(!cancel)
						info.second += 0x1;
					else if(info.second & 0xffff)
						info.second -= 0x1;
				} else {
					if(!cancel)
						info.second += 0x10000;
					else if(info.second & 0xffff0000)
						info.second -= 0x10000;
				}
				break;
			}
		}
	}
}
void field::set_spsummon_counter(uint8 playerid, bool add, bool chain) {
	if(add) {
		core.spsummon_state_count[playerid]++;
		if(chain)
			core.spsummon_state_count_rst[playerid]++;
	} else {
		if(chain) {
			core.spsummon_state_count[playerid] -= core.spsummon_state_count_rst[playerid];
			core.spsummon_state_count_rst[playerid] = 0;
		} else
			core.spsummon_state_count[playerid]--;
	}
	if(core.global_flag & GLOBALFLAG_SPSUMMON_COUNT) {
		for(auto iter = effects.spsummon_count_eff.begin(); iter != effects.spsummon_count_eff.end(); ++iter) {
			effect* peffect = *iter;
			card* pcard = peffect->handler;
			if(add) {
				if(pcard->is_status(STATUS_EFFECT_ENABLED) && !pcard->is_status(STATUS_DISABLED) && pcard->is_position(POS_FACEUP)) {
					if(((playerid == pcard->current.controler) && peffect->s_range) || ((playerid != pcard->current.controler) && peffect->o_range)) {
						pcard->spsummon_counter[playerid]++;
						if(chain)
							pcard->spsummon_counter_rst[playerid]++;
					}
				}
			} else {
				pcard->spsummon_counter[playerid] -= pcard->spsummon_counter_rst[playerid];
				pcard->spsummon_counter_rst[playerid] = 0;
			}
		}
	}
}
int32 field::check_spsummon_counter(uint8 playerid, uint8 ct) {
	if(core.global_flag & GLOBALFLAG_SPSUMMON_COUNT) {
		for(auto iter = effects.spsummon_count_eff.begin(); iter != effects.spsummon_count_eff.end(); ++iter) {
			effect* peffect = *iter;
			card* pcard = peffect->handler;
			uint16 val = (uint16)peffect->value;
			if(pcard->is_status(STATUS_EFFECT_ENABLED) && !pcard->is_status(STATUS_DISABLED) && pcard->is_position(POS_FACEUP)) {
				if(pcard->spsummon_counter[playerid] + ct > val)
					return FALSE;
			}
		}
	}
	return TRUE;
}
int32 field::check_lp_cost(uint8 playerid, uint32 lp) {
	effect_set eset;
	int32 val = lp;
	if(lp == 0)
		return TRUE;
	filter_player_effect(playerid, EFFECT_LPCOST_CHANGE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(core.reason_effect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(val, PARAM_TYPE_INT);
		val = eset[i]->get_value(3);
		if(val <= 0)
			return TRUE;
	}
	tevent e;
	e.event_cards = 0;
	e.event_player = playerid;
	e.event_value = lp;
	e.reason = 0;
	e.reason_effect = core.reason_effect;
	e.reason_player = playerid;
	if(effect_replace_check(EFFECT_LPCOST_REPLACE, e))
		return true;
	cost[playerid].amount += val;
	if(cost[playerid].amount <= player[playerid].lp)
		return TRUE;
	return FALSE;
}
void field::save_lp_cost() {
	for(uint8 playerid = 0; playerid < 2; ++playerid) {
		if(cost[playerid].count < 8)
			cost[playerid].lpstack[cost[playerid].count] = cost[playerid].amount;
		cost[playerid].count++;
	}
}
void field::restore_lp_cost() {
	for(uint8 playerid = 0; playerid < 2; ++playerid) {
		cost[playerid].count--;
		if(cost[playerid].count < 8)
			cost[playerid].amount = cost[playerid].lpstack[cost[playerid].count];
	}
}
uint32 field::get_field_counter(uint8 self, uint8 s, uint8 o, uint16 countertype) {
	uint8 c = s;
	uint32 count = 0;
	for(int p = 0; p < 2; ++p) {
		if(c) {
			for(int i = 0; i < 5; ++i) {
				if(player[self].list_mzone[i])
					count += player[self].list_mzone[i]->get_counter(countertype);
			}
			for(int i = 0; i < 8; ++i) {
				if(player[self].list_szone[i])
					count += player[self].list_szone[i]->get_counter(countertype);
			}
		}
		self = 1 - self;
		c = o;
	}
	return count;
}
int32 field::effect_replace_check(uint32 code, const tevent& e) {
	auto pr = effects.continuous_effect.equal_range(code);
	for (; pr.first != pr.second; ++pr.first) {
		effect* peffect = pr.first->second;
		if(peffect->is_activateable(peffect->get_handler_player(), e))
			return TRUE;
	}
	return FALSE;
}
int32 field::get_attack_target(card* pcard, card_vector* v, uint8 chain_attack) {
	uint8 p = pcard->current.controler;
	effect* peffect;
	card* atarget;
	pcard->operation_param = 0;
	card_vector must_be_attack;
	card_vector* pv;
	card_vector::iterator cit;
	for(uint32 i = 0; i < 5; ++i) {
		atarget = player[1 - p].list_mzone[i];
		if(atarget && atarget->is_affected_by_effect(EFFECT_MUST_BE_ATTACKED))
			must_be_attack.push_back(atarget);
	}
	if(pcard->attack_all_target && (peffect = pcard->is_affected_by_effect(EFFECT_ATTACK_ALL))) {
		if(pcard->announced_cards.size()) {
			if(must_be_attack.size())
				pv = &must_be_attack;
			else
				pv = &player[1 - p].list_mzone;
			for(cit = pv->begin(); cit != pv->end(); ++cit) {
				atarget = *cit;
				if(!atarget || pcard->announced_cards.count(atarget->fieldid_r))
					continue;
				if(atarget->is_affected_by_effect(EFFECT_IGNORE_BATTLE_TARGET))
					continue;
				if(atarget->is_affected_by_effect(EFFECT_CANNOT_BE_BATTLE_TARGET, pcard))
					continue;
				pduel->lua->add_param(atarget, PARAM_TYPE_CARD);
				if(!peffect->check_value_condition(1))
					continue;
				v->push_back(atarget);
			}
			return must_be_attack.size() ? TRUE : FALSE;
		}
	} else if(!chain_attack) {
		uint32 extrac = 0;
		if((peffect = pcard->is_affected_by_effect(EFFECT_EXTRA_ATTACK)))
			extrac = peffect->get_value(pcard);
		if(pcard->announce_count >= extrac + 1)
			return FALSE;
	}
	uint32 mcount = 0;
	if(must_be_attack.size())
		pv = &must_be_attack;
	else
		pv = &player[1 - p].list_mzone;
	for(cit = pv->begin(); cit != pv->end(); ++cit) {
		atarget = *cit;
		if(!atarget)
			continue;
		if(atarget->is_affected_by_effect(EFFECT_IGNORE_BATTLE_TARGET))
			continue;
		mcount++;
		if(atarget->is_affected_by_effect(EFFECT_CANNOT_BE_BATTLE_TARGET, pcard))
			continue;
		v->push_back(atarget);
	}
	if(must_be_attack.size())
		return TRUE;
	if((mcount == 0 || pcard->is_affected_by_effect(EFFECT_DIRECT_ATTACK)) && !pcard->is_affected_by_effect(EFFECT_CANNOT_DIRECT_ATTACK))
		pcard->operation_param = 1;
	return must_be_attack.size() ? TRUE : FALSE;
}
void field::attack_all_target_check() {
	if(!core.attacker)
		return;
	if(!core.attack_target) {
		core.attacker->attack_all_target = FALSE;
		return;
	}
	effect* peffect = core.attacker->is_affected_by_effect(EFFECT_ATTACK_ALL);
	if(!peffect)
		return;
	pduel->lua->add_param(core.attack_target, PARAM_TYPE_CARD);
	if(!peffect->check_value_condition(1))
		core.attacker->attack_all_target = FALSE;
}
int32 field::check_synchro_material(card* pcard, int32 findex1, int32 findex2, int32 min, int32 max, card* smat, group* mg) {
	card* tuner;
	if(core.global_flag & GLOBALFLAG_MUST_BE_SMATERIAL) {
		effect_set eset;
		filter_player_effect(pcard->current.controler, EFFECT_MUST_BE_SMATERIAL, &eset);
		if(eset.size())
			return check_tuner_material(pcard, eset[0]->handler, findex1, findex2, min, max, smat, mg);
	}
	for(uint8 p = 0; p < 2; ++p) {
		for(int32 i = 0; i < 5; ++i) {
			tuner = player[p].list_mzone[i];
			if(check_tuner_material(pcard, tuner, findex1, findex2, min, max, smat, mg))
				return TRUE;
		}
	}
	return FALSE;
}
int32 field::check_tuner_material(card* pcard, card* tuner, int32 findex1, int32 findex2, int32 min, int32 max, card* smat, group* mg) {
	effect* peffect;
	if(tuner && tuner->is_position(POS_FACEUP) && (tuner->get_type()&TYPE_TUNER) && tuner->is_can_be_synchro_material(pcard)) {
		effect* pcheck = tuner->is_affected_by_effect(EFFECT_SYNCHRO_CHECK);
		if(pcheck)
			pcheck->get_value(tuner);
		if((mg && !mg->has_card(tuner)) || !pduel->lua->check_matching(tuner, findex1, 0)) {
			pduel->restore_assumes();
			return FALSE;
		}
		if((peffect = tuner->is_affected_by_effect(EFFECT_SYNCHRO_MATERIAL_CUSTOM, pcard))) {
			if(!peffect->target) {
				pduel->restore_assumes();
				return FALSE;
			}
			pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
			pduel->lua->add_param(findex2, PARAM_TYPE_INDEX);
			pduel->lua->add_param(min, PARAM_TYPE_INT);
			pduel->lua->add_param(max, PARAM_TYPE_INT);
			if(pduel->lua->check_condition(peffect->target, 5)) {
				pduel->restore_assumes();
				return TRUE;
			}
		} else {
			int32 l = tuner->get_synchro_level(pcard);
			int32 l1 = l & 0xffff;
			//int32 l2 = l >> 16;
			int32 lv = pcard->get_level();
			lv -= l1;
			if(lv <= 0) {
				pduel->restore_assumes();
				return FALSE;
			}
			if(smat) {
				if(pcheck)
					pcheck->get_value(smat);
				if(!smat->is_position(POS_FACEUP) || !smat->is_can_be_synchro_material(pcard, tuner) || !pduel->lua->check_matching(smat, findex2, 0)) {
					pduel->restore_assumes();
					return FALSE;
				}
				l = smat->get_synchro_level(pcard);
				l1 = l & 0xffff;
				lv -= l1;
				min--;
				max--;
				if(lv <= 0) {
					pduel->restore_assumes();
					if(lv == 0 && min == 0)
						return TRUE;
					return FALSE;
				}
				if(max == 0) {
					pduel->restore_assumes();
					return FALSE;
				}
			}
			card_vector nsyn;
			card* pm;
			if(mg) {
				for(auto cit = mg->container.begin(); cit != mg->container.end(); ++cit) {
					pm = *cit;
					if(pm != tuner && pm != smat && pm->is_can_be_synchro_material(pcard, tuner)) {
						if(pcheck)
							pcheck->get_value(pm);
						if(pm->current.location == LOCATION_MZONE && !pm->is_position(POS_FACEUP))
							continue;
						if(!pduel->lua->check_matching(pm, findex2, 0))
							continue;
						nsyn.push_back(pm);
						pm->operation_param = pm->get_synchro_level(pcard);
					}
				}
			} else {
				for(uint8 p = 0; p < 2; ++p) {
					for(int32 i = 0; i < 5; ++i) {
						pm = player[p].list_mzone[i];
						if(pm && pm != tuner && pm != smat && pm->is_position(POS_FACEUP) && pm->is_can_be_synchro_material(pcard, tuner)) {
							if(pcheck)
								pcheck->get_value(pm);
							if(!pduel->lua->check_matching(pm, findex2, 0))
								continue;
							nsyn.push_back(pm);
							pm->operation_param = pm->get_synchro_level(pcard);
						}
					}
				}
			}
			if(check_with_sum_limit(&nsyn, lv, 0, 1, min, max)) {
				pduel->restore_assumes();
				return TRUE;
			}
		}
	}
	pduel->restore_assumes();
	return FALSE;
}
int32 field::check_with_sum_limit(card_vector* mats, int32 acc, int32 index, int32 count, int32 min, int32 max) {
	if((uint32)index >= mats->size())
		return FALSE;
	int32 op1 = mats->at(index)->operation_param & 0xffff;
	int32 op2 = (mats->at(index)->operation_param >> 16) & 0xffff;
	if((op1 == acc || op2 == acc) && count >= min && count <= max)
		return TRUE;
	return (acc > op1 && check_with_sum_limit(mats, acc - op1, index + 1, count + 1, min, max))
	       || (op2 && acc > op2 && check_with_sum_limit(mats, acc - op2, index + 1, count + 1, min, max))
	       || check_with_sum_limit(mats, acc, index + 1, count, min, max);
}
int32 field::check_xyz_material(card* scard, int32 findex, int32 lv, int32 min, int32 max, group* mg) {
	if(mg) {
		uint32 xyz_level;
		core.xmaterial_lst.clear();
		for (auto cit = mg->container.begin(); cit != mg->container.end(); ++cit) {
			if((xyz_level = (*cit)->check_xyz_level(scard, lv)) && (findex == 0 || pduel->lua->check_matching(*cit, findex, 0)))
				core.xmaterial_lst.insert(std::make_pair((xyz_level >> 12) & 0xf, *cit));
		}
		if(core.global_flag & GLOBALFLAG_XMAT_COUNT_LIMIT) {
			auto iter = core.xmaterial_lst.begin();
			while((iter != core.xmaterial_lst.end()) && ((iter->first > (int32)core.xmaterial_lst.size()) || (iter->first > max)))
				core.xmaterial_lst.erase(iter++);
		}
	} else
		get_xyz_material(scard, findex, lv, max);
	return (int32)core.xmaterial_lst.size() >= min;
}
int32 field::is_player_can_draw(uint8 playerid) {
	return !is_player_affected_by_effect(playerid, EFFECT_CANNOT_DRAW);
}
int32 field::is_player_can_discard_deck(uint8 playerid, int32 count) {
	if(player[playerid].list_main.size() < (uint32)count)
		return FALSE;
	return !is_player_affected_by_effect(playerid, EFFECT_CANNOT_DISCARD_DECK);
}
int32 field::is_player_can_discard_deck_as_cost(uint8 playerid, int32 count) {
	if(player[playerid].list_main.size() < (uint32)count)
		return FALSE;
	if(is_player_affected_by_effect(playerid, EFFECT_CANNOT_DISCARD_DECK))
		return FALSE;
	if((count == 1) && core.deck_reversed)
		return player[playerid].list_main.back()->is_capable_cost_to_grave(playerid);
	effect_set eset;
	filter_field_effect(EFFECT_TO_GRAVE_REDIRECT, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		uint32 redirect = eset[i]->get_value();
		if((redirect & LOCATION_REMOVED) && player[playerid].list_main.back()->is_affected_by_effect(EFFECT_CANNOT_REMOVE))
			continue;
		uint8 p = eset[i]->get_handler_player();
		if((p == playerid && eset[i]->s_range & LOCATION_DECK) || (p != playerid && eset[i]->o_range & LOCATION_DECK))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_discard_hand(uint8 playerid, card * pcard, effect * peffect, uint32 reason) {
	if(pcard->current.location != LOCATION_HAND)
		return FALSE;
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_DISCARD_HAND, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(reason, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 4))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_summon(uint32 sumtype, uint8 playerid, card * pcard) {
	effect_set eset;
	sumtype |= SUMMON_TYPE_NORMAL;
	filter_player_effect(playerid, EFFECT_CANNOT_SUMMON, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(sumtype, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 4))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_mset(uint32 sumtype, uint8 playerid, card * pcard) {
	effect_set eset;
	sumtype |= SUMMON_TYPE_NORMAL;
	filter_player_effect(playerid, EFFECT_CANNOT_MSET, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(sumtype, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 4))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_sset(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_SSET, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_spsummon(uint8 playerid) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_SPECIAL_SUMMON, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_spsummon(effect * peffect, uint32 sumtype, uint8 sumpos, uint8 playerid, uint8 toplayer, card * pcard) {
	effect_set eset;
	sumtype |= SUMMON_TYPE_SPECIAL;
	if(sumpos & POS_FACEDOWN && is_player_affected_by_effect(playerid, EFFECT_DEVINE_LIGHT))
		sumpos = (sumpos & POS_FACEUP) | (sumpos >> 1);
	filter_player_effect(playerid, EFFECT_CANNOT_SPECIAL_SUMMON, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(sumtype, PARAM_TYPE_INT);
		pduel->lua->add_param(sumpos, PARAM_TYPE_INT);
		pduel->lua->add_param(toplayer, PARAM_TYPE_INT);
		pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
		if (pduel->lua->check_condition(eset[i]->target, 7))
			return FALSE;
	}
	if(!check_spsummon_once(pcard, playerid))
		return FALSE;
	if(!check_spsummon_counter(playerid))
		return FALSE;
	return TRUE;
}
int32 field::is_player_can_flipsummon(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_FLIP_SUMMON, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_spsummon_monster(uint8 playerid, uint8 toplayer, uint8 sumpos, card_data * pdata) {
	temp_card->data = *pdata;
	if(!is_player_can_spsummon(core.reason_effect, SUMMON_TYPE_SPECIAL, sumpos, playerid, toplayer, temp_card))
		return FALSE;
	return temp_card->is_affected_by_effect(EFFECT_CANNOT_SPECIAL_SUMMON) ? FALSE : TRUE;
}
int32 field::is_player_can_release(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_RELEASE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_spsummon_count(uint8 playerid, uint32 count) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_LEFT_SPSUMMON_COUNT, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(core.reason_effect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		int32 v = eset[i]->get_value(2);
		if(v < (int32)count)
			return FALSE;
	}
	return check_spsummon_counter(playerid, count);
}
int32 field::is_player_can_place_counter(uint8 playerid, card * pcard, uint16 countertype, uint16 count) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_PLACE_COUNTER, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_remove_counter(uint8 playerid, card * pcard, uint8 s, uint8 o, uint16 countertype, uint16 count, uint32 reason) {
	if((pcard && pcard->get_counter(countertype) >= count) || (!pcard && get_field_counter(playerid, s, o, countertype) >= count))
		return TRUE;
	pair<effect_container::iterator, effect_container::iterator> pr;
	pr = effects.continuous_effect.equal_range(EFFECT_RCOUNTER_REPLACE + countertype);
	effect* peffect;
	tevent e;
	e.event_cards = 0;
	e.event_player = playerid;
	e.event_value = count;
	e.reason = reason;
	e.reason_effect = core.reason_effect;
	e.reason_player = playerid;
	for (; pr.first != pr.second; ++pr.first) {
		peffect = pr.first->second;
		if(peffect->is_activateable(peffect->get_handler_player(), e))
			return TRUE;
	}
	return FALSE;
}
int32 field::is_player_can_remove_overlay_card(uint8 playerid, card * pcard, uint8 s, uint8 o, uint16 min, uint32 reason) {
	if((pcard && pcard->xyz_materials.size() >= min) || (!pcard && get_overlay_count(playerid, s, o) >= min))
		return TRUE;
	pair<effect_container::iterator, effect_container::iterator> pr;
	pr = effects.continuous_effect.equal_range(EFFECT_OVERLAY_REMOVE_REPLACE);
	effect* peffect;
	tevent e;
	e.event_cards = 0;
	e.event_player = playerid;
	e.event_value = min;
	e.reason = reason;
	e.reason_effect = core.reason_effect;
	e.reason_player = playerid;
	for (; pr.first != pr.second; ++pr.first) {
		peffect = pr.first->second;
		if(peffect->is_activateable(peffect->get_handler_player(), e))
			return TRUE;
	}
	return FALSE;
}
int32 field::is_player_can_send_to_grave(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_TO_GRAVE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_send_to_hand(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_TO_HAND, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_send_to_deck(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_TO_DECK, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_player_can_remove(uint8 playerid, card * pcard) {
	effect_set eset;
	filter_player_effect(playerid, EFFECT_CANNOT_REMOVE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		if(!eset[i]->target)
			return FALSE;
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if (pduel->lua->check_condition(eset[i]->target, 3))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_chain_negatable(uint8 chaincount, uint8 naga_check) {
	effect_set eset;
	if(chaincount < 0 || chaincount > core.current_chain.size())
		return FALSE;
	effect* peffect;
	if(chaincount == 0)
		peffect = core.current_chain.back().triggering_effect;
	else
		peffect = core.current_chain[chaincount - 1].triggering_effect;
	if(naga_check && peffect->flag & EFFECT_FLAG_NAGA)
		return FALSE;
	if(peffect->flag & EFFECT_FLAG_CANNOT_DISABLE)
		return FALSE;
	filter_field_effect(EFFECT_CANNOT_INACTIVATE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(chaincount, PARAM_TYPE_INT);
		if(eset[i]->check_value_condition(1))
			return FALSE;
	}
	return TRUE;
}
int32 field::is_chain_disablable(uint8 chaincount, uint8 naga_check) {
	effect_set eset;
	if(chaincount < 0 || chaincount > core.current_chain.size())
		return FALSE;
	effect* peffect;
	if(chaincount == 0)
		peffect = core.current_chain.back().triggering_effect;
	else
		peffect = core.current_chain[chaincount - 1].triggering_effect;
	if(naga_check && peffect->flag & EFFECT_FLAG_NAGA)
		return FALSE;
	if(peffect->flag & EFFECT_FLAG_CANNOT_DISABLE)
		return FALSE;
	filter_field_effect(EFFECT_CANNOT_DISEFFECT, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(chaincount, PARAM_TYPE_INT);
		if(eset[i]->check_value_condition(1))
			return FALSE;
	}
	return TRUE;
}
int32 field::check_chain_target(uint8 chaincount, card * pcard) {
	if(chaincount < 0 || chaincount > core.current_chain.size())
		return FALSE;
	chain* pchain;
	if(chaincount == 0)
		pchain = &core.current_chain.back();
	else
		pchain = &core.current_chain[chaincount - 1];
	effect* peffect = pchain->triggering_effect;
	uint8 tp = pchain->triggering_player;
	if(!(peffect->flag & EFFECT_FLAG_CARD_TARGET) || !peffect->target)
		return FALSE;
	if(!pcard->is_capable_be_effect_target(peffect, tp))
		return false;
	pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
	pduel->lua->add_param(tp, PARAM_TYPE_INT);
	pduel->lua->add_param(pchain->evt.event_cards , PARAM_TYPE_GROUP);
	pduel->lua->add_param(pchain->evt.event_player, PARAM_TYPE_INT);
	pduel->lua->add_param(pchain->evt.event_value, PARAM_TYPE_INT);
	pduel->lua->add_param(pchain->evt.reason_effect , PARAM_TYPE_EFFECT);
	pduel->lua->add_param(pchain->evt.reason, PARAM_TYPE_INT);
	pduel->lua->add_param(pchain->evt.reason_player, PARAM_TYPE_INT);
	pduel->lua->add_param((ptr)0, PARAM_TYPE_INT);
	pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
	return pduel->lua->check_condition(peffect->target, 10);
}
