/*
 * card.cpp
 *
 *  Created on: 2010-5-7
 *      Author: Argon
 */

#include "card.h"
#include "field.h"
#include "effect.h"
#include "duel.h"
#include "group.h"
#include "interpreter.h"
#include "ocgapi.h"
#include <memory.h>
#include <iostream>
#include <algorithm>

bool card_sort::operator()(void* const & p1, void* const & p2) const {
	card* c1 = (card*)p1;
	card* c2 = (card*)p2;
	return c1->cardid < c2->cardid;
}
bool card::card_operation_sort(card* c1, card* c2) {
	duel* pduel = c1->pduel;
	int32 cp1 = c1->overlay_target ? c1->overlay_target->current.controler : c1->current.controler;
	int32 cp2 = c2->overlay_target ? c2->overlay_target->current.controler : c2->current.controler;
	if(cp1 != cp2) {
		if(cp1 == PLAYER_NONE || cp2 == PLAYER_NONE)
			return cp1 < cp2;
		if(pduel->game_field->infos.turn_player == 0)
			return cp1 < cp2;
		else
			return cp1 > cp2;
	}
	if(c1->current.location != c2->current.location)
		return c1->current.location < c2->current.location;
	if(c1->current.location & LOCATION_OVERLAY) {
		if(c1->overlay_target->current.sequence != c2->overlay_target->current.sequence)
			return c1->overlay_target->current.sequence < c2->overlay_target->current.sequence;
		else return c1->current.sequence < c2->current.sequence;
	} else {
		if(c1->current.location & 0x71)
			return c1->current.sequence > c2->current.sequence;
		else
			return c1->current.sequence < c2->current.sequence;
	}
}
card::card(duel* pd) {
	scrtype = 1;
	ref_handle = 0;
	pduel = pd;
	owner = PLAYER_NONE;
	operation_param = 0;
	status = 0;
	memset(&q_cache, 0xff, sizeof(query_cache));
	equiping_target = 0;
	pre_equip_target = 0;
	overlay_target = 0;
	memset(&current, 0, sizeof(card_state));
	memset(&previous, 0, sizeof(card_state));
	memset(&temp, 0xff, sizeof(card_state));
	unique_pos[0] = unique_pos[1] = 0;
	spsummon_counter[0] = spsummon_counter[1] = 0;
	spsummon_counter_rst[0] = spsummon_counter_rst[1] = 0;
	unique_code = 0;
	assume_type = 0;
	assume_value = 0;
	spsummon_code = 0;
	current.controler = PLAYER_NONE;
}
card::~card() {
	indexer.clear();
	relations.clear();
	counters.clear();
	equiping_cards.clear();
	material_cards.clear();
	single_effect.clear();
	field_effect.clear();
	equip_effect.clear();
	relate_effect.clear();
}
uint32 card::get_infos(byte* buf, int32 query_flag, int32 use_cache) {
	int32* p = (int32*)buf;
	int32 tdata = 0;
	p += 2;
	if(query_flag & QUERY_CODE) *p++ = data.code;
	if(query_flag & QUERY_POSITION) *p++ = get_info_location();
	if(!use_cache) {
		if(query_flag & QUERY_ALIAS) q_cache.code = *p++ = get_code();
		if(query_flag & QUERY_TYPE) q_cache.type = *p++ = get_type();
		if(query_flag & QUERY_LEVEL) q_cache.level = *p++ = get_level();
		if(query_flag & QUERY_RANK) q_cache.rank = *p++ = get_rank();
		if(query_flag & QUERY_ATTRIBUTE) q_cache.attribute = *p++ = get_attribute();
		if(query_flag & QUERY_RACE) q_cache.race = *p++ = get_race();
		if(query_flag & QUERY_ATTACK) q_cache.attack = *p++ = get_attack();
		if(query_flag & QUERY_DEFENCE) q_cache.defence = *p++ = get_defence();
		if(query_flag & QUERY_BASE_ATTACK) q_cache.base_attack = *p++ = get_base_attack();
		if(query_flag & QUERY_BASE_DEFENCE) q_cache.base_defence = *p++ = get_base_defence();
		if(query_flag & QUERY_REASON) q_cache.reason = *p++ = current.reason;
	} else {
		if((query_flag & QUERY_ALIAS) && ((uint32)(tdata = get_code()) != q_cache.alias)) {
			q_cache.alias = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_ALIAS;
		if((query_flag & QUERY_TYPE) && ((uint32)(tdata = get_type()) != q_cache.type)) {
			q_cache.type = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_TYPE;
		if((query_flag & QUERY_LEVEL) && ((uint32)(tdata = get_level()) != q_cache.level)) {
			q_cache.level = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_LEVEL;
		if((query_flag & QUERY_RANK) && ((uint32)(tdata = get_rank()) != q_cache.rank)) {
			q_cache.rank = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_RANK;
		if((query_flag & QUERY_ATTRIBUTE) && ((uint32)(tdata = get_attribute()) != q_cache.attribute)) {
			q_cache.attribute = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_ATTRIBUTE;
		if((query_flag & QUERY_RACE) && ((uint32)(tdata = get_race()) != q_cache.race)) {
			q_cache.race = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_RACE;
		if((query_flag & QUERY_ATTACK) && ((tdata = get_attack()) != q_cache.attack)) {
			q_cache.attack = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_ATTACK;
		if((query_flag & QUERY_DEFENCE) && ((tdata = get_defence()) != q_cache.defence)) {
			q_cache.defence = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_DEFENCE;
		if((query_flag & QUERY_BASE_ATTACK) && ((tdata = get_base_attack()) != q_cache.base_attack)) {
			q_cache.base_attack = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_BASE_ATTACK;
		if((query_flag & QUERY_BASE_DEFENCE) && ((tdata = get_base_defence()) != q_cache.base_defence)) {
			q_cache.base_defence = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_BASE_DEFENCE;
		if((query_flag & QUERY_REASON) && ((uint32)(tdata = current.reason) != q_cache.reason)) {
			q_cache.reason = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_REASON;
	}
	if(query_flag & QUERY_REASON_CARD)
		*p++ = current.reason_card ? current.reason_card->get_info_location() : 0;
	if(query_flag & QUERY_EQUIP_CARD) {
		if(equiping_target)
			*p++ = equiping_target->get_info_location();
		else
			query_flag &= ~QUERY_EQUIP_CARD;
	}
	if(query_flag & QUERY_TARGET_CARD) {
		*p++ = effect_target_cards.size();
		card_set::iterator cit;
		for(cit = effect_target_cards.begin(); cit != effect_target_cards.end(); ++cit)
			*p++ = (*cit)->get_info_location();
	}
	if(query_flag & QUERY_OVERLAY_CARD) {
		*p++ = xyz_materials.size();
		for(auto clit = xyz_materials.begin(); clit != xyz_materials.end(); ++clit)
			*p++ = (*clit)->data.code;
	}
	if(query_flag & QUERY_COUNTERS) {
		*p++ = counters.size();
		counter_map::iterator cmit;
		for(cmit = counters.begin(); cmit != counters.end(); ++cmit)
			*p++ = cmit->first + (cmit->second << 16);
	}
	if(query_flag & QUERY_OWNER)
		*p++ = owner;
	if(query_flag & QUERY_IS_DISABLED) {
		tdata = (status & STATUS_DISABLED) ? 1 : 0;
		if(!use_cache || (tdata != q_cache.is_disabled)) {
			q_cache.is_disabled = tdata;
			*p++ = tdata;
		} else
			query_flag &= ~QUERY_IS_DISABLED;
	}
	if(query_flag & QUERY_IS_PUBLIC)
		*p++ = (status & STATUS_IS_PUBLIC) ? 1 : 0;
	if(!use_cache) {
		if(query_flag & QUERY_LSCALE) q_cache.lscale = *p++ = get_lscale();
		if(query_flag & QUERY_RSCALE) q_cache.rscale = *p++ = get_rscale();
	} else {
		if((query_flag & QUERY_LSCALE) && ((uint32)(tdata = get_lscale()) != q_cache.lscale)) {
			q_cache.lscale = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_LSCALE;
		if((query_flag & QUERY_RSCALE) && ((uint32)(tdata = get_rscale()) != q_cache.rscale)) {
			q_cache.rscale = tdata;
			*p++ = tdata;
		} else query_flag &= ~QUERY_RSCALE;
	}
	*(uint32*)buf = (byte*)p - buf;
	*(uint32*)(buf + 4) = query_flag;
	return (byte*)p - buf;
}
uint32 card::get_info_location() {
	if(overlay_target) {
		uint32 c = overlay_target->current.controler;
		uint32 l = overlay_target->current.location | LOCATION_OVERLAY;
		uint32 s = overlay_target->current.sequence;
		uint32 ss = current.sequence;
		return c + (l << 8) + (s << 16) + (ss << 24);
	} else {
		uint32 c = current.controler;
		uint32 l = current.location;
		uint32 s = current.sequence;
		uint32 ss = current.position;
		return c + (l << 8) + (s << 16) + (ss << 24);
	}
}
uint32 card::get_code() {
	if(assume_type == ASSUME_CODE)
		return assume_value;
	if(!(current.location & 0x1c)) {
		if(data.alias)
			return data.alias;
		return data.code;
	}
	if (temp.code != 0xffffffff)
		return temp.code;
	effect_set effects;
	uint32 code = data.code;
	temp.code = data.code;
	filter_effect(EFFECT_CHANGE_CODE, &effects);
	if (effects.size())
		code = effects.get_last()->get_value(this);
	temp.code = 0xffffffff;
	if (code == data.code) {
		if(data.alias)
			code = data.alias;
	} else {
		card_data dat;
		read_card(code, &dat);
		if (dat.alias)
			code = dat.alias;
	}
	return code;
}
uint32 card::get_another_code() {
	effect_set eset;
	filter_effect(EFFECT_ADD_CODE, &eset);
	if(!eset.size())
		return 0;
	uint32 otcode = eset.get_last()->get_value(this);
	if(get_code() != otcode)
		return otcode;
	if(data.alias == otcode)
		return data.code;
	return 0;
}
int32 card::is_set_card(uint32 set_code) {
	uint32 code = get_code();
	uint64 setcode;
	if (code == data.code) {
		setcode = data.setcode;
	} else {
		card_data dat;
		::read_card(code, &dat);
		setcode = dat.setcode;
	}
	uint32 settype = set_code & 0xfff;
	uint32 setsubtype = set_code & 0xf000;
	while(setcode) {
		if ((setcode & 0xfff) == settype && (setcode & 0xf000 & setsubtype) == setsubtype)
			return TRUE;
		setcode = setcode >> 16;
	}
	return FALSE;
}
uint32 card::get_type() {
	if(assume_type == ASSUME_TYPE)
		return assume_value;
	if(!(current.location & 0x1e))
		return data.type;
	if((current.location == LOCATION_SZONE) && (current.sequence >= 6))
		return TYPE_PENDULUM + TYPE_SPELL;
	if (temp.type != 0xffffffff)
		return temp.type;
	effect_set effects;
	int32 type = data.type;
	temp.type = data.type;
	filter_effect(EFFECT_ADD_TYPE, &effects, FALSE);
	filter_effect(EFFECT_REMOVE_TYPE, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_TYPE, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		if (effects[i]->code == EFFECT_ADD_TYPE)
			type |= effects[i]->get_value(this);
		else if (effects[i]->code == EFFECT_REMOVE_TYPE)
			type &= ~(effects[i]->get_value(this));
		else
			type = effects[i]->get_value(this);
		temp.type = type;
	}
	temp.type = 0xffffffff;
	return type;
}
int32 card::get_base_attack(uint8 swap) {
	if (current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if (current.location != LOCATION_MZONE)
		return data.attack;
	if (temp.base_attack != -1)
		return temp.base_attack;
	if(!swap && is_affected_by_effect(EFFECT_SWAP_BASE_AD))
		return get_base_defence(TRUE);
	int32 batk = data.attack;
	temp.base_attack = data.attack;
	if(temp.base_attack < 0)
		temp.base_attack = 0;
	effect_set effects;
	filter_effect(EFFECT_SET_BASE_ATTACK, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		batk = effects[i]->get_value(this);
		if (batk < 0)
			batk = 0;
		temp.base_attack = batk;
	}
	if (batk < 0)
		batk = 0;
	temp.base_attack = -1;
	return batk;
}
int32 card::get_attack(uint8 swap) {
	if(assume_type == ASSUME_ATTACK)
		return assume_value;
	if (current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if (current.location != LOCATION_MZONE)
		return data.attack;
	if (temp.attack != -1)
		return temp.attack;
	if(!swap && is_affected_by_effect(EFFECT_SWAP_AD))
		return get_defence(TRUE);
	uint32 base = get_base_attack();
	temp.base_attack = base;
	temp.attack = base;
	int32 up = 0, upc = 0, final = -1, atk, rev = FALSE;
	effect_set eset;
	effect_set effects;
	filter_effect(EFFECT_UPDATE_ATTACK, &eset, FALSE);
	filter_effect(EFFECT_SET_ATTACK, &eset, FALSE);
	filter_effect(EFFECT_SET_ATTACK_FINAL, &eset);
	if (is_affected_by_effect(EFFECT_REVERSE_UPDATE))
		rev = TRUE;
	for (int32 i = 0; i < eset.size(); ++i) {
		switch (eset[i]->code) {
		case EFFECT_UPDATE_ATTACK:
			if ((eset[i]->type & EFFECT_TYPE_SINGLE) && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE)) {
				for (int32 j = 0; j < effects.size(); ++j) {
					if (effects[j]->flag & EFFECT_FLAG_REPEAT) {
						base = effects[j]->get_value(this);
						up = 0;
						upc = 0;
						temp.attack = base;
					}
				}
				up += eset[i]->get_value(this);
			} else
				upc += eset[i]->get_value(this);
			break;
		case EFFECT_SET_ATTACK:
			base = eset[i]->get_value(this);
			if (!(eset[i]->type & EFFECT_TYPE_SINGLE))
				up = 0;
			break;
		case EFFECT_SET_ATTACK_FINAL:
			if ((eset[i]->type & EFFECT_TYPE_SINGLE) && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE)) {
				base = eset[i]->get_value(this);
				up = 0;
				upc = 0;
			} else
				effects.add_item(eset[i]);
			break;
		}
		if (!rev)
			temp.attack = base + up + upc;
		else
			temp.attack = base - up - upc;
	}
	for (int32 i = 0; i < effects.size(); ++i) {
		final = effects[i]->get_value(this);
		temp.attack = final;
	}
	if (final == -1) {
		if (!rev)
			atk = base + up + upc;
		else
			atk = base - up - upc;
	} else
		atk = final;
	if (atk < 0)
		atk = 0;
	temp.base_attack = -1;
	temp.attack = -1;
	return atk;
}
int32 card::get_base_defence(uint8 swap) {
	if (current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if (current.location != LOCATION_MZONE)
		return data.defence;
	if (temp.base_defence != -1)
		return temp.base_defence;
	if(!swap && is_affected_by_effect(EFFECT_SWAP_BASE_AD))
		return get_base_attack(TRUE);
	int32 bdef = data.defence;
	temp.base_defence = data.defence;
	if(temp.base_defence < 0)
		temp.base_defence = 0;
	effect_set effects;
	filter_effect(EFFECT_SET_BASE_DEFENCE, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		bdef = effects[i]->get_value(this);
		if (bdef < 0)
			bdef = 0;
		temp.base_defence = bdef;
	}
	if (bdef < 0)
		bdef = 0;
	temp.base_defence = -1;
	return bdef;
}
int32 card::get_defence(uint8 swap) {
	if(assume_type == ASSUME_DEFENCE)
		return assume_value;
	if (current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if (current.location != LOCATION_MZONE)
		return data.defence;
	if (temp.defence != -1)
		return temp.defence;
	if(!swap && is_affected_by_effect(EFFECT_SWAP_AD))
		return get_attack(TRUE);
	uint32 base = get_base_defence();
	temp.base_defence = base;
	temp.defence = base;
	int32 up = 0, upc = 0, final = -1, def, rev = FALSE;
	effect_set eset;
	effect_set effects;
	filter_effect(EFFECT_UPDATE_DEFENCE, &eset, FALSE);
	filter_effect(EFFECT_SET_DEFENCE, &eset, FALSE);
	filter_effect(EFFECT_SET_DEFENCE_FINAL, &eset);
	if (is_affected_by_effect(EFFECT_REVERSE_UPDATE))
		rev = TRUE;
	for (int32 i = 0; i < eset.size(); ++i) {
		switch (eset[i]->code) {
		case EFFECT_UPDATE_DEFENCE:
			if ((eset[i]->type & EFFECT_TYPE_SINGLE) && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE)) {
				for (int32 j = 0; j < effects.size(); ++j) {
					if (effects[j]->flag & EFFECT_FLAG_REPEAT) {
						base = effects[j]->get_value(this);
						up = 0;
						upc = 0;
						temp.defence = base;
					}
				}
				up += eset[i]->get_value(this);
			} else
				upc += eset[i]->get_value(this);
			break;
		case EFFECT_SET_DEFENCE:
			base = eset[i]->get_value(this);
			if (!(eset[i]->type & EFFECT_TYPE_SINGLE))
				up = 0;
			break;
		case EFFECT_SET_DEFENCE_FINAL:
			if ((eset[i]->type & EFFECT_TYPE_SINGLE) && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE)) {
				base = eset[i]->get_value(this);
				up = 0;
				upc = 0;
			} else
				effects.add_item(eset[i]);
			break;
		}
		if (!rev)
			temp.defence = base + up + upc;
		else
			temp.defence = base - up - upc;
	}
	for (int32 i = 0; i < effects.size(); ++i) {
		final = effects[i]->get_value(this);
		temp.defence = final;
	}
	if (final == -1) {
		if (!rev)
			def = base + up + upc;
		else
			def = base - up - upc;
	} else
		def = final;
	if (def < 0)
		def = 0;
	temp.base_defence = -1;
	temp.defence = -1;
	return def;
}
uint32 card::get_level() {
	if((data.type & TYPE_XYZ) || (status & STATUS_NO_LEVEL) 
	        || (current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER)))
		return 0;
	if(assume_type == ASSUME_LEVEL)
		return assume_value;
	if(!(current.location & LOCATION_MZONE) && !(data.type & TYPE_MONSTER))
		return data.level;
	if (temp.level != 0xffffffff)
		return temp.level;
	effect_set effects;
	int32 level = data.level;
	temp.level = level;
	int32 up = 0, upc = 0;
	filter_effect(EFFECT_UPDATE_LEVEL, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_LEVEL, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_LEVEL_FINAL, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		switch (effects[i]->code) {
		case EFFECT_UPDATE_LEVEL:
			if ((effects[i]->type & EFFECT_TYPE_SINGLE) && !(effects[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up += effects[i]->get_value(this);
			else
				upc += effects[i]->get_value(this);
			break;
		case EFFECT_CHANGE_LEVEL:
			level = effects[i]->get_value(this);
			up = 0;
			break;
		case EFFECT_CHANGE_LEVEL_FINAL:
			level = effects[i]->get_value(this);
			up = 0;
			upc = 0;
			break;
		}
		temp.level = level + up + upc;
	}
	level += up + upc;
	if(level < 1 && (get_type() & TYPE_MONSTER))
		level = 1;
	temp.level = 0xffffffff;
	return level;
}
uint32 card::get_rank() {
	if(!(data.type & TYPE_XYZ) || (status & STATUS_NO_LEVEL))
		return 0;
	if(assume_type == ASSUME_RANK)
		return assume_value;
	if(!(current.location & LOCATION_MZONE))
		return data.level;
	if (temp.level != 0xffffffff)
		return temp.level;
	effect_set effects;
	int32 rank = data.level;
	temp.level = rank;
	int32 up = 0, upc = 0;
	filter_effect(EFFECT_UPDATE_RANK, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_RANK, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_RANK_FINAL, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		switch (effects[i]->code) {
		case EFFECT_UPDATE_RANK:
			if ((effects[i]->type & EFFECT_TYPE_SINGLE) && !(effects[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up += effects[i]->get_value(this);
			else
				upc += effects[i]->get_value(this);
			break;
		case EFFECT_CHANGE_RANK:
			rank = effects[i]->get_value(this);
			up = 0;
			break;
		case EFFECT_CHANGE_RANK_FINAL:
			rank = effects[i]->get_value(this);
			up = 0;
			upc = 0;
			break;
		}
		temp.level = rank + up + upc;
	}
	rank += up + upc;
	if(rank < 1 && (get_type() & TYPE_MONSTER))
		rank = 1;
	temp.level = 0xffffffff;
	return rank;
}
uint32 card::get_synchro_level(card* pcard) {
	if((data.type & TYPE_XYZ) || (status & STATUS_NO_LEVEL))
		return 0;
	uint32 lev;
	effect_set eset;
	filter_effect(EFFECT_SYNCHRO_LEVEL, &eset);
	if(eset.size())
		lev = eset[0]->get_value(pcard);
	else
		lev = get_level();
	return lev;
}
uint32 card::get_ritual_level(card* pcard) {
	if((data.type & TYPE_XYZ) || (status & STATUS_NO_LEVEL))
		return 0;
	uint32 lev;
	effect_set eset;
	filter_effect(EFFECT_RITUAL_LEVEL, &eset);
	if(eset.size())
		lev = eset[0]->get_value(pcard);
	else
		lev = get_level();
	return lev;
}
uint32 card::check_xyz_level(card* pcard, uint32 lv) {
	if(status & STATUS_NO_LEVEL)
		return 0;
	uint32 lev;
	effect_set eset;
	filter_effect(EFFECT_XYZ_LEVEL, &eset);
	if(!eset.size()) {
		lev = get_level();
		if(lev == lv)
			return lev;
		return 0;
	}
	pduel->lua->add_param(this, PARAM_TYPE_CARD);
	pduel->lua->add_param(pcard, PARAM_TYPE_CARD);
	lev = eset[0]->get_value(2);
	if(((lev & 0xfff) == lv))
		return lev & 0xffff;
	if(((lev >> 16) & 0xfff) == lv)
		return (lev >> 16) & 0xffff;
	return 0;
}
uint32 card::get_attribute() {
	if(assume_type == ASSUME_ATTRIBUTE)
		return assume_value;
	if(current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if(!(current.location & (LOCATION_MZONE)) && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_TRAPMONSTER))
		return data.attribute;
	if (temp.attribute != 0xffffffff)
		return temp.attribute;
	effect_set effects;
	effect_set effects2;
	int32 attribute = data.attribute;
	temp.attribute = data.attribute;
	filter_effect(EFFECT_ADD_ATTRIBUTE, &effects, FALSE);
	filter_effect(EFFECT_REMOVE_ATTRIBUTE, &effects);
	filter_effect(EFFECT_CHANGE_ATTRIBUTE, &effects2);
	for (int32 i = 0; i < effects.size(); ++i) {
		if (effects[i]->code == EFFECT_ADD_ATTRIBUTE)
			attribute |= effects[i]->get_value(this);
		else
			attribute &= ~(effects[i]->get_value(this));
		temp.attribute = attribute;
	}
	for (int32 i = 0; i < effects2.size(); ++i) {
		attribute = effects2[i]->get_value(this);
		temp.attribute = attribute;
	}
	temp.attribute = 0xffffffff;
	return attribute;
}
uint32 card::get_race() {
	if(assume_type == ASSUME_RACE)
		return assume_value;
	if(current.location != LOCATION_MZONE && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_MONSTER))
		return 0;
	if(!(current.location & (LOCATION_MZONE)) && !(data.type & TYPE_MONSTER) && !(get_type() & TYPE_TRAPMONSTER))
		return data.race;
	if (temp.race != 0xffffffff)
		return temp.race;
	effect_set effects;
	effect_set effects2;
	int32 race = data.race;
	temp.race = data.race;
	filter_effect(EFFECT_ADD_RACE, &effects, FALSE);
	filter_effect(EFFECT_REMOVE_RACE, &effects);
	filter_effect(EFFECT_CHANGE_RACE, &effects2);
	for (int32 i = 0; i < effects.size(); ++i) {
		if (effects[i]->code == EFFECT_ADD_RACE)
			race |= effects[i]->get_value(this);
		else
			race &= ~(effects[i]->get_value(this));
		temp.race = race;
	}
	for (int32 i = 0; i < effects2.size(); ++i) {
		race = effects2[i]->get_value(this);
		temp.race = race;
	}
	temp.race = 0xffffffff;
	return race;
}
uint32 card::get_lscale() {
	if(!(current.location & LOCATION_SZONE))
		return data.lscale;
	if (temp.lscale != 0xffffffff)
		return temp.lscale;
	effect_set effects;
	int32 lscale = data.lscale;
	temp.lscale = data.lscale;
	int32 up = 0, upc = 0;
	filter_effect(EFFECT_UPDATE_LSCALE, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_LSCALE, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		if (effects[i]->code == EFFECT_UPDATE_LSCALE) {
			if ((effects[i]->type & EFFECT_TYPE_SINGLE) && !(effects[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up += effects[i]->get_value(this);
			else
				upc += effects[i]->get_value(this);
		} else {
			lscale = effects[i]->get_value(this);
			up = 0;
		}
		temp.lscale = lscale;
	}
	lscale += up + upc;
	temp.lscale = 0xffffffff;
	return lscale;
}
uint32 card::get_rscale() {
	if(!(current.location & LOCATION_SZONE))
		return data.rscale;
	if (temp.rscale != 0xffffffff)
		return temp.rscale;
	effect_set effects;
	int32 rscale = data.rscale;
	temp.rscale = data.rscale;
	int32 up = 0, upc = 0;
	filter_effect(EFFECT_UPDATE_RSCALE, &effects, FALSE);
	filter_effect(EFFECT_CHANGE_RSCALE, &effects);
	for (int32 i = 0; i < effects.size(); ++i) {
		if (effects[i]->code == EFFECT_UPDATE_RSCALE) {
			if ((effects[i]->type & EFFECT_TYPE_SINGLE) && !(effects[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up += effects[i]->get_value(this);
			else
				upc += effects[i]->get_value(this);
		} else {
			rscale = effects[i]->get_value(this);
			up = 0;
		}
		temp.rscale = rscale;
	}
	rscale += up + upc;
	temp.rscale = 0xffffffff;
	return rscale;
}
int32 card::is_position(int32 pos) {
	return current.position & pos;
}
void card::set_status(uint32 status, int32 enabled) {
	if (enabled)
		this->status |= status;
	else
		this->status &= ~status;
}
int32 card::get_status(uint32 status) {
	return this->status & status;
}
int32 card::is_status(uint32 status) {
	if ((this->status & status) == status)
		return TRUE;
	return FALSE;
}
void card::equip(card *target, uint32 send_msg) {
	if (equiping_target)
		return;
	target->equiping_cards.insert(this);
	equiping_target = target;
	for (auto it = equip_effect.begin(); it != equip_effect.end(); ++it) {
		if (it->second->is_disable_related())
			pduel->game_field->add_to_disable_check_list(equiping_target);
	}
	if(send_msg) {
		pduel->write_buffer8(MSG_EQUIP);
		pduel->write_buffer32(get_info_location());
		pduel->write_buffer32(target->get_info_location());
	}
	return;
}
void card::unequip() {
	if (!equiping_target)
		return;
	for (auto it = equip_effect.begin(); it != equip_effect.end(); ++it) {
		if (it->second->is_disable_related())
			pduel->game_field->add_to_disable_check_list(equiping_target);
	}
	equiping_target->equiping_cards.erase(this);
	pre_equip_target = equiping_target;
	equiping_target = 0;
	return;
}
int32 card::get_union_count() {
	card_set::iterator cit;
	int32 count = 0;
	for(cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		if(((*cit)->data.type & TYPE_UNION) && (*cit)->is_status(STATUS_UNION))
			count++;
	}
	return count;
}
void card::xyz_overlay(card_set* materials) {
	if(materials->size() == 0)
		return;
	card_set des;
	if(materials->size() == 1) {
		card* pcard = *materials->begin();
		pcard->reset(RESET_LEAVE + RESET_OVERLAY, RESET_EVENT);
		if(pcard->unique_code)
			pduel->game_field->remove_unique_card(pcard);
		if(pcard->equiping_target)
			pcard->unequip();
		xyz_add(pcard, &des);
	} else {
		field::card_vector cv;
		for(auto cit = materials->begin(); cit != materials->end(); ++cit)
			cv.push_back(*cit);
		std::sort(cv.begin(), cv.end(), card::card_operation_sort);
		for(auto cvit = cv.begin(); cvit != cv.end(); ++cvit) {
			(*cvit)->reset(RESET_LEAVE + RESET_OVERLAY, RESET_EVENT);
			if((*cvit)->unique_code)
				pduel->game_field->remove_unique_card(*cvit);
			if((*cvit)->equiping_target)
				(*cvit)->unequip();
			xyz_add(*cvit, &des);
		}
	}
	if(des.size())
		pduel->game_field->destroy(&des, 0, REASON_LOST_TARGET + REASON_RULE, PLAYER_NONE);
}
void card::xyz_add(card* mat, card_set* des) {
	if(mat->overlay_target == this)
		return;
	pduel->write_buffer8(MSG_MOVE);
	pduel->write_buffer32(mat->data.code);
	if(mat->overlay_target) {
		pduel->write_buffer8(mat->overlay_target->current.controler);
		pduel->write_buffer8(mat->overlay_target->current.location | LOCATION_OVERLAY);
		pduel->write_buffer8(mat->overlay_target->current.sequence);
		pduel->write_buffer8(mat->current.sequence);
		mat->overlay_target->xyz_remove(mat);
	} else {
		pduel->write_buffer8(mat->current.controler);
		pduel->write_buffer8(mat->current.location);
		pduel->write_buffer8(mat->current.sequence);
		pduel->write_buffer8(mat->current.position);
		mat->enable_field_effect(false);
		pduel->game_field->remove_card(mat);
		pduel->game_field->add_to_disable_check_list(mat);
	}
	pduel->write_buffer8(current.controler);
	pduel->write_buffer8(current.location | LOCATION_OVERLAY);
	pduel->write_buffer8(current.sequence);
	pduel->write_buffer8(current.position);
	pduel->write_buffer32(REASON_XYZ + REASON_MATERIAL);
	xyz_materials.push_back(mat);
	for(auto cit = mat->equiping_cards.begin(); cit != mat->equiping_cards.end();) {
		auto rm = cit++;
		des->insert(*rm);
		(*rm)->unequip();
	}
	mat->overlay_target = this;
	mat->current.controler = PLAYER_NONE;
	mat->current.location = LOCATION_OVERLAY;
	mat->current.sequence = xyz_materials.size() - 1;
	mat->current.reason = REASON_XYZ + REASON_MATERIAL;
}
void card::xyz_remove(card* mat) {
	if(mat->overlay_target != this)
		return;
	xyz_materials.erase(xyz_materials.begin() + mat->current.sequence);
	mat->previous.controler = mat->current.controler;
	mat->previous.location = mat->current.location;
	mat->previous.sequence = mat->current.sequence;
	mat->current.controler = PLAYER_NONE;
	mat->current.location = 0;
	mat->current.sequence = 0;
	mat->overlay_target = 0;
	for(auto clit = xyz_materials.begin(); clit != xyz_materials.end(); ++clit)
		(*clit)->current.sequence = clit - xyz_materials.begin();
}
void card::apply_field_effect() {
	if (current.controler == PLAYER_NONE)
		return;
	for (auto it = field_effect.begin(); it != field_effect.end(); ++it) {
		if (it->second->in_range(current.location, current.sequence) || ((it->second->range & LOCATION_HAND)
		        && (it->second->type & EFFECT_TYPE_TRIGGER_O) && !(it->second->code & EVENT_PHASE))) {
			pduel->game_field->add_effect(it->second);
		}
	}
	if(unique_code && (current.location & LOCATION_ONFIELD))
		pduel->game_field->add_unique_card(this);
	spsummon_counter[0] = spsummon_counter[1] = 0;
	spsummon_counter_rst[0] = spsummon_counter_rst[1] = 0;
}
void card::cancel_field_effect() {
	if (current.controler == PLAYER_NONE)
		return;
	for (auto it = field_effect.begin(); it != field_effect.end(); ++it) {
		if (it->second->in_range(current.location, current.sequence) || ((it->second->range & LOCATION_HAND)
		        && (it->second->type & EFFECT_TYPE_TRIGGER_O) && !(it->second->code & EVENT_PHASE))) {
			pduel->game_field->remove_effect(it->second);
		}
	}
	if(unique_code && (current.location & LOCATION_ONFIELD))
		pduel->game_field->remove_unique_card(this);
}
void card::enable_field_effect(int32 enabled) {
	if (current.location == 0)
		return;
	if ((enabled && get_status(STATUS_EFFECT_ENABLED)) || (!enabled && !get_status(STATUS_EFFECT_ENABLED)))
		return;
	refresh_disable_status();
	if (enabled) {
		set_status(STATUS_EFFECT_ENABLED, TRUE);
		effect_container::iterator it;
		for (it = single_effect.begin(); it != single_effect.end(); ++it) {
			if ((it->second->flag & EFFECT_FLAG_SINGLE_RANGE) && it->second->in_range(current.location, current.sequence))
				it->second->id = pduel->game_field->infos.field_id++;
		}
		for (it = field_effect.begin(); it != field_effect.end(); ++it) {
			if (it->second->in_range(current.location, current.sequence))
				it->second->id = pduel->game_field->infos.field_id++;
		}
		if(current.location == LOCATION_SZONE) {
			for (it = equip_effect.begin(); it != equip_effect.end(); ++it)
				it->second->id = pduel->game_field->infos.field_id++;
		}
		if (get_status(STATUS_DISABLED))
			reset(RESET_DISABLE, RESET_EVENT);
	} else
		set_status(STATUS_EFFECT_ENABLED, FALSE);
	filter_immune_effect();
	if (get_status(STATUS_DISABLED))
		return;
	filter_disable_related_cards();
}
int32 card::add_effect(effect* peffect) {
	effect_container::iterator it, rm;
	if (get_status(STATUS_COPYING_EFFECT) && (peffect->flag & EFFECT_FLAG_UNCOPYABLE)) {
		pduel->uncopy.insert(peffect);
		return 0;
	}
	if (indexer.find(peffect) != indexer.end())
		return 0;
	card* check_target = this;
	if (peffect->type & EFFECT_TYPE_SINGLE) {
		if(peffect->code == EFFECT_SET_ATTACK && !(peffect->flag & EFFECT_FLAG_SINGLE_RANGE)) {
			for(it = single_effect.begin(); it != single_effect.end();) {
				rm = it++;
				if((rm->second->code == EFFECT_SET_ATTACK || rm->second->code == EFFECT_SET_ATTACK_FINAL)
				        && !(rm->second->flag & EFFECT_FLAG_SINGLE_RANGE))
					remove_effect(rm->second);
			}
		}
		if(peffect->code == EFFECT_SET_ATTACK_FINAL && !(peffect->flag & EFFECT_FLAG_SINGLE_RANGE)) {
			for(it = single_effect.begin(); it != single_effect.end();) {
				rm = it++;
				if((rm->second->code == EFFECT_UPDATE_ATTACK || rm->second->code == EFFECT_SET_ATTACK
				        || rm->second->code == EFFECT_SET_ATTACK_FINAL) && !(rm->second->flag & EFFECT_FLAG_SINGLE_RANGE))
					remove_effect(rm->second);
			}
		}
		if(peffect->code == EFFECT_SET_DEFENCE && !(peffect->flag & EFFECT_FLAG_SINGLE_RANGE)) {
			for(it = single_effect.begin(); it != single_effect.end();) {
				rm = it++;
				if((rm->second->code == EFFECT_SET_DEFENCE || rm->second->code == EFFECT_SET_DEFENCE_FINAL)
				        && !(rm->second->flag & EFFECT_FLAG_SINGLE_RANGE))
					remove_effect(rm->second);
			}
		}
		if(peffect->code == EFFECT_SET_DEFENCE_FINAL && !(peffect->flag & EFFECT_FLAG_SINGLE_RANGE)) {
			for(it = single_effect.begin(); it != single_effect.end();) {
				rm = it++;
				if((rm->second->code == EFFECT_UPDATE_DEFENCE || rm->second->code == EFFECT_SET_DEFENCE
				        || rm->second->code == EFFECT_SET_DEFENCE_FINAL) && !(rm->second->flag & EFFECT_FLAG_SINGLE_RANGE))
					remove_effect(rm->second);
			}
		}
		it = single_effect.insert(make_pair(peffect->code, peffect));
	} else if (peffect->type & EFFECT_TYPE_FIELD)
		it = field_effect.insert(make_pair(peffect->code, peffect));
	else if (peffect->type & EFFECT_TYPE_EQUIP) {
		it = equip_effect.insert(make_pair(peffect->code, peffect));
		if (equiping_target)
			check_target = equiping_target;
		else
			check_target = 0;
	} else
		return 0;
	peffect->id = pduel->game_field->infos.field_id++;
	peffect->card_type = data.type;
	if(get_status(STATUS_INITIALIZING))
		peffect->flag |= EFFECT_FLAG_INITIAL;
	if (get_status(STATUS_COPYING_EFFECT)) {
		peffect->copy_id = pduel->game_field->infos.copy_id;
		peffect->reset_flag |= pduel->game_field->core.copy_reset;
		peffect->reset_count = (peffect->reset_count & 0xffffff00) | pduel->game_field->core.copy_reset_count;
	}
	if((peffect->flag & EFFECT_FLAG_COPY_INHERIT) && pduel->game_field->core.reason_effect
	        && (pduel->game_field->core.reason_effect->copy_id)) {
		peffect->copy_id = pduel->game_field->core.reason_effect->copy_id;
		peffect->reset_flag |= pduel->game_field->core.reason_effect->reset_flag;
		if((peffect->reset_count & 0xff) > (pduel->game_field->core.reason_effect->reset_count & 0xff))
			peffect->reset_count = (peffect->reset_count & 0xffffff00) | (pduel->game_field->core.reason_effect->reset_count & 0xff);
	}
	indexer.insert(make_pair(peffect, it));
	peffect->handler = this;
	if (peffect->in_range(current.location, current.sequence) && (peffect->type & EFFECT_TYPE_FIELD))
		pduel->game_field->add_effect(peffect);
	if (current.controler != PLAYER_NONE && check_target) {
		if (peffect->is_disable_related())
			pduel->game_field->add_to_disable_check_list(check_target);
	}
	if(peffect->flag & EFFECT_FLAG_OATH) {
		effect* reason_effect = pduel->game_field->core.reason_effect;
		pduel->game_field->effects.oath.insert(make_pair(peffect, reason_effect));
	}
	if(peffect->reset_flag & RESET_PHASE) {
		pduel->game_field->effects.pheff.insert(peffect);
		if((peffect->reset_count & 0xff) == 0)
			peffect->reset_count += 1;
	}
	if(peffect->reset_flag & RESET_CHAIN)
		pduel->game_field->effects.cheff.insert(peffect);
	if(peffect->flag & EFFECT_FLAG_COUNT_LIMIT)
		pduel->game_field->effects.rechargeable.insert(peffect);
	if(peffect->flag & EFFECT_FLAG_CLIENT_HINT) {
		pduel->write_buffer8(MSG_CARD_HINT);
		pduel->write_buffer32(get_info_location());
		pduel->write_buffer8(CHINT_DESC_ADD);
		pduel->write_buffer32(peffect->description);
	}
	return peffect->id;
}
void card::remove_effect(effect* peffect) {
	auto it = indexer.find(peffect);
	if (it == indexer.end())
		return;
	remove_effect(peffect, it->second);
}
void card::remove_effect(effect* peffect, effect_container::iterator it) {
	card* check_target = this;
	if (peffect->type & EFFECT_TYPE_SINGLE)
		single_effect.erase(it);
	else if (peffect->type & EFFECT_TYPE_FIELD) {
		check_target = 0;
		if (peffect->in_range(current.location, current.sequence) && get_status(STATUS_EFFECT_ENABLED) && !get_status(STATUS_DISABLED)) {
			if (peffect->is_disable_related())
				pduel->game_field->update_disable_check_list(peffect);
		}
		field_effect.erase(it);
		if (peffect->in_range(current.location, current.sequence))
			pduel->game_field->remove_effect(peffect);
	} else if (peffect->type & EFFECT_TYPE_EQUIP) {
		equip_effect.erase(it);
		if (equiping_target)
			check_target = equiping_target;
		else
			check_target = 0;
	}
	if ((current.controler != PLAYER_NONE) && !get_status(STATUS_DISABLED) && check_target) {
		if (peffect->is_disable_related())
			pduel->game_field->add_to_disable_check_list(check_target);
	}
	indexer.erase(peffect);
	if(peffect->flag & EFFECT_FLAG_OATH)
		pduel->game_field->effects.oath.erase(peffect);
	if(peffect->reset_flag & RESET_PHASE)
		pduel->game_field->effects.pheff.erase(peffect);
	if(peffect->reset_flag & RESET_CHAIN)
		pduel->game_field->effects.cheff.erase(peffect);
	if(peffect->flag & EFFECT_FLAG_COUNT_LIMIT)
		pduel->game_field->effects.rechargeable.erase(peffect);
	if(((peffect->code & 0xf0000) == EFFECT_COUNTER_PERMIT) && (peffect->type & EFFECT_TYPE_SINGLE)) {
		auto cmit = counters.find(peffect->code & 0xffff);
		if(cmit != counters.end()) {
			pduel->write_buffer8(MSG_REMOVE_COUNTER);
			pduel->write_buffer16(cmit->first);
			pduel->write_buffer8(current.controler);
			pduel->write_buffer8(current.location);
			pduel->write_buffer8(current.sequence);
			pduel->write_buffer8(cmit->second);
			counters.erase(cmit);
		}
	}
	if(peffect->flag & EFFECT_FLAG_CLIENT_HINT) {
		pduel->write_buffer8(MSG_CARD_HINT);
		pduel->write_buffer32(get_info_location());
		pduel->write_buffer8(CHINT_DESC_REMOVE);
		pduel->write_buffer32(peffect->description);
	}
	if(peffect->code == EFFECT_UNIQUE_CHECK) {
		pduel->game_field->remove_unique_card(this);
		unique_pos[0] = unique_pos[1] = 0;
		unique_code = 0;
	}
	pduel->game_field->core.reseted_effects.insert(peffect);
}
int32 card::copy_effect(uint32 code, uint32 reset, uint32 count) {
	card_data cdata;
	read_card(code, &cdata);
	if(cdata.type & TYPE_NORMAL)
		return -1;
	set_status(STATUS_COPYING_EFFECT, TRUE);
	uint32 cr = pduel->game_field->core.copy_reset;
	uint8 crc = pduel->game_field->core.copy_reset_count;
	pduel->game_field->core.copy_reset = reset;
	pduel->game_field->core.copy_reset_count = count;
	pduel->lua->add_param(this, PARAM_TYPE_CARD);
	pduel->lua->call_code_function(code, (char*) "initial_effect", 1, 0);
	pduel->game_field->infos.copy_id++;
	set_status(STATUS_COPYING_EFFECT, FALSE);
	pduel->game_field->core.copy_reset = cr;
	pduel->game_field->core.copy_reset_count = crc;
	for(auto eit = pduel->uncopy.begin(); eit != pduel->uncopy.end(); ++eit)
		pduel->delete_effect(*eit);
	pduel->uncopy.clear();
	return pduel->game_field->infos.copy_id - 1;
}
void card::reset(uint32 id, uint32 reset_type) {
	effect* peffect;
	if (reset_type != RESET_EVENT && reset_type != RESET_PHASE && reset_type != RESET_CODE && reset_type != RESET_COPY && reset_type != RESET_CARD)
		return;
	if (reset_type == RESET_EVENT) {
		for (auto rit = relations.begin(); rit != relations.end();) {
			auto rrm = rit++;
			if (rrm->second & 0xffff0000 & id)
				relations.erase(rrm);
		}
		if(id & 0x47c0000)
			relate_effect.clear();
		if(id & 0x5fc0000) {
			announced_cards.clear();
			attacked_cards.clear();
			announce_count = 0;
			attacked_count = 0;
			attack_all_target = TRUE;
		}
		if(id & 0x5fe0000) {
			battled_cards.clear();
			reset_effect_count();
			auto pr = field_effect.equal_range(EFFECT_DISABLE_FIELD);
			for(; pr.first != pr.second; ++pr.first)
				pr.first->second->value = 0;
			set_status(STATUS_UNION, FALSE);
		}
		if(id & 0x57e0000) {
			counters.clear();
			for(auto cit = effect_target_owner.begin(); cit != effect_target_owner.end(); ++cit)
				(*cit)->effect_target_cards.erase(this);
			for(auto cit = effect_target_cards.begin(); cit != effect_target_cards.end(); ++cit) {
				card* pcard = *cit;
				pcard->effect_target_owner.erase(this);
				for(auto it = pcard->single_effect.begin(); it != pcard->single_effect.end();) {
					auto rm = it++;
					peffect = rm->second;
					if((peffect->owner == this) && (peffect->flag & EFFECT_FLAG_OWNER_RELATE))
						pcard->remove_effect(peffect, rm);
				}
			}
			effect_target_owner.clear();
			effect_target_cards.clear();
		}
		if(id & 0x3fe0000) {
			auto pr = field_effect.equal_range(EFFECT_USE_EXTRA_MZONE);
			for(; pr.first != pr.second; ++pr.first)
				pr.first->second->value = pr.first->second->value & 0xffff;
			pr = field_effect.equal_range(EFFECT_USE_EXTRA_SZONE);
			for(; pr.first != pr.second; ++pr.first)
				pr.first->second->value = pr.first->second->value & 0xffff;
		}
		if(id & RESET_DISABLE) {
			for(auto cmit = counters.begin(); cmit != counters.end();) {
				auto rm = cmit++;
				if(rm->first & COUNTER_NEED_ENABLE) {
					pduel->write_buffer8(MSG_REMOVE_COUNTER);
					pduel->write_buffer16(rm->first);
					pduel->write_buffer8(current.controler);
					pduel->write_buffer8(current.location);
					pduel->write_buffer8(current.sequence);
					pduel->write_buffer8(rm->second);
					counters.erase(rm);
				}
			}
		}
		if(id & RESET_TURN_SET) {
			effect* peffect = check_equip_control_effect();
			if(peffect) {
				effect* new_effect = pduel->new_effect();
				new_effect->id = peffect->id;
				new_effect->owner = this;
				new_effect->handler = this;
				new_effect->type = EFFECT_TYPE_SINGLE;
				new_effect->code = EFFECT_SET_CONTROL;
				new_effect->value = current.controler;
				new_effect->flag = EFFECT_FLAG_CANNOT_DISABLE;
				new_effect->reset_flag = RESET_EVENT | 0xec0000;
				this->add_effect(new_effect);
			}
		}
	}
	for (auto i = indexer.begin(); i != indexer.end();) {
		auto rm = i++;
		peffect = rm->first;
		auto it = rm->second;
		if (peffect->reset(id, reset_type))
			remove_effect(peffect, it);
	}
}
void card::reset_effect_count() {
	for (auto i = indexer.begin(); i != indexer.end(); ++i) {
		effect* peffect = i->first;
		if (peffect->flag & EFFECT_FLAG_COUNT_LIMIT)
			peffect->recharge();
	}
}
int32 card::refresh_disable_status() {
	int32 pre_dis = is_status(STATUS_DISABLED);
	filter_immune_effect();
	if (!is_affected_by_effect(EFFECT_CANNOT_DISABLE) && is_affected_by_effect(EFFECT_DISABLE))
		set_status(STATUS_DISABLED, TRUE);
	else
		set_status(STATUS_DISABLED, FALSE);
	int32 cur_dis = is_status(STATUS_DISABLED);
	if(pre_dis != cur_dis)
		filter_immune_effect();
	return is_status(STATUS_DISABLED);
}
uint8 card::refresh_control_status() {
	uint8 final = owner;
	if(pduel->game_field->core.remove_brainwashing)
		return final;
	effect_set eset;
	filter_effect(EFFECT_SET_CONTROL, &eset);
	if(eset.size())
		final = (uint8) (eset.get_last()->get_value(this, 0));
	return final;
}
void card::count_turn(uint16 ct) {
	turn_counter = ct;
	pduel->write_buffer8(MSG_CARD_HINT);
	pduel->write_buffer32(get_info_location());
	pduel->write_buffer8(CHINT_TURN);
	pduel->write_buffer32(ct);
}
void card::create_relation(card* target, uint32 reset) {
	if (relations.find(target) != relations.end())
		return;
	relations[target] = reset;
}
void card::create_relation(effect* peffect) {
	auto it = relate_effect.find(peffect);
	if (it != relate_effect.end())
		++it->second;
	else
		relate_effect[peffect] = 1;
}
int32 card::is_has_relation(card* target) {
	if (relations.find(target) != relations.end())
		return TRUE;
	return FALSE;
}
int32 card::is_has_relation(effect* peffect) {
	if (relate_effect.find(peffect) != relate_effect.end())
		return TRUE;
	return FALSE;
}
void card::release_relation(card* target) {
	if (relations.find(target) == relations.end())
		return;
	relations.erase(target);
}
void card::release_relation(effect* peffect) {
	auto it = relate_effect.find(peffect);
	if (it != relate_effect.end() && --it->second == 0)
		relate_effect.erase(it);
}
int32 card::leave_field_redirect(uint32 reason) {
	effect_set es;
	uint32 redirect;
	if(data.type & TYPE_TOKEN)
		return 0;
	filter_effect(EFFECT_LEAVE_FIELD_REDIRECT, &es);
	for(int32 i = 0; i < es.size(); ++i) {
		redirect = es[i]->get_value(this, 0);
		if((redirect & LOCATION_HAND) && !is_affected_by_effect(EFFECT_CANNOT_TO_HAND) && pduel->game_field->is_player_can_send_to_hand(current.controler, this))
			return redirect;
		else if((redirect & LOCATION_DECK) && !is_affected_by_effect(EFFECT_CANNOT_TO_DECK) && pduel->game_field->is_player_can_send_to_deck(current.controler, this))
			return redirect;
		else if((redirect & LOCATION_REMOVED) && !is_affected_by_effect(EFFECT_CANNOT_REMOVE) && pduel->game_field->is_player_can_remove(current.controler, this))
			return redirect;
	}
	return 0;
}
int32 card::destination_redirect(uint8 destination, uint32 reason) {
	effect_set es;
	uint32 redirect;
	if(data.type & TYPE_TOKEN)
		return 0;
	if(destination == LOCATION_HAND)
		filter_effect(EFFECT_TO_HAND_REDIRECT, &es);
	else if(destination == LOCATION_DECK)
		filter_effect(EFFECT_TO_DECK_REDIRECT, &es);
	else if(destination == LOCATION_GRAVE)
		filter_effect(EFFECT_TO_GRAVE_REDIRECT, &es);
	else if(destination == LOCATION_REMOVED)
		filter_effect(EFFECT_REMOVE_REDIRECT, &es);
	else return 0;
	for(int32 i = 0; i < es.size(); ++i) {
		redirect = es[i]->get_value(this, 0);
		if((redirect & LOCATION_HAND) && !is_affected_by_effect(EFFECT_CANNOT_TO_HAND) && pduel->game_field->is_player_can_send_to_hand(current.controler, this))
			return redirect;
		if((redirect & LOCATION_DECK) && !is_affected_by_effect(EFFECT_CANNOT_TO_DECK) && pduel->game_field->is_player_can_send_to_deck(current.controler, this))
			return redirect;
		if((redirect & LOCATION_REMOVED) && !is_affected_by_effect(EFFECT_CANNOT_REMOVE) && pduel->game_field->is_player_can_remove(current.controler, this))
			return redirect;
	}
	return 0;
}
int32 card::add_counter(uint8 playerid, uint16 countertype, uint16 count) {
	if(!is_can_add_counter(playerid, countertype, count))
		return FALSE;
	counters[countertype] += count;
	pduel->write_buffer8(MSG_ADD_COUNTER);
	pduel->write_buffer16(countertype);
	pduel->write_buffer8(current.controler);
	pduel->write_buffer8(current.location);
	pduel->write_buffer8(current.sequence);
	pduel->write_buffer8(count);
	return TRUE;
}
int32 card::remove_counter(uint16 countertype, uint16 count) {
	counter_map::iterator cmit = counters.find(countertype);
	if(cmit == counters.end())
		return FALSE;
	if(cmit->second <= count)
		counters.erase(cmit);
	else cmit->second -= count;
	pduel->write_buffer8(MSG_REMOVE_COUNTER);
	pduel->write_buffer16(countertype);
	pduel->write_buffer8(current.controler);
	pduel->write_buffer8(current.location);
	pduel->write_buffer8(current.sequence);
	pduel->write_buffer8(count);
	return TRUE;
}
int32 card::is_can_add_counter(uint8 playerid, uint16 countertype, uint16 count) {
	effect_set eset;
	if(!pduel->game_field->is_player_can_place_counter(playerid, this, countertype, count))
		return FALSE;
	if(!(current.location & LOCATION_ONFIELD) || !is_position(POS_FACEUP))
		return FALSE;
	if((countertype & COUNTER_NEED_ENABLE) && is_status(STATUS_DISABLED))
		return FALSE;
	if((countertype & COUNTER_NEED_PERMIT) && !is_affected_by_effect(EFFECT_COUNTER_PERMIT + (countertype & 0xffff)))
		return FALSE;
	int32 limit = -1;
	int32 cur = 0;
	counter_map::iterator cmit = counters.find(countertype);
	if(cmit != counters.end())
		cur = cmit->second;
	filter_effect(EFFECT_COUNTER_LIMIT + countertype, &eset);
	for(int32 i = 0; i < eset.size(); ++i)
		limit = eset[i]->get_value();
	if(limit > 0 && (cur + count > limit))
		return FALSE;
	return TRUE;
}
int32 card::get_counter(uint16 countertype) {
	counter_map::iterator cmit = counters.find(countertype);
	if(cmit == counters.end())
		return 0;
	return cmit->second;
}
void card::set_material(card_set* materials) {
	material_cards = *materials;
	card_set::iterator cit;
	for(cit = material_cards.begin(); cit != material_cards.end(); ++cit)
		(*cit)->current.reason_card = this;
	effect_set eset;
	filter_effect(EFFECT_MATERIAL_CHECK, &eset);
	for(int i = 0; i < eset.size(); ++i) {
		eset[i]->get_value(this);
	}
}
void card::add_card_target(card* pcard) {
	effect_target_cards.insert(pcard);
	pcard->effect_target_owner.insert(this);
	pduel->write_buffer8(MSG_CARD_TARGET);
	pduel->write_buffer32(get_info_location());
	pduel->write_buffer32(pcard->get_info_location());
}
void card::cancel_card_target(card* pcard) {
	auto cit = effect_target_cards.find(pcard);
	if(cit != effect_target_cards.end()) {
		effect_target_cards.erase(cit);
		pcard->effect_target_owner.erase(this);
		pduel->write_buffer8(MSG_CANCEL_TARGET);
		pduel->write_buffer32(get_info_location());
		pduel->write_buffer32(pcard->get_info_location());
	}
}
void card::filter_effect(int32 code, effect_set* eset, uint8 sort) {
	effect* peffect;
	auto rg = single_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (peffect->is_available() && (!(peffect->flag & EFFECT_FLAG_SINGLE_RANGE) || is_affect_by_effect(peffect)))
			eset->add_item(peffect);
	}
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		rg = (*cit)->equip_effect.equal_range(code);
		for (; rg.first != rg.second; ++rg.first) {
			peffect = rg.first->second;
			if (peffect->is_available() && is_affect_by_effect(peffect))
				eset->add_item(peffect);
		}
	}
	rg = pduel->game_field->effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (!(peffect->flag & EFFECT_FLAG_PLAYER_TARGET) && peffect->is_available()
		        && peffect->is_target(this) && is_affect_by_effect(peffect))
			eset->add_item(peffect);
	}
	if(sort)
		eset->sort();
}
void card::filter_single_continuous_effect(int32 code, effect_set* eset, uint8 sort) {
	auto rg = single_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first)
		eset->add_item(rg.first->second);
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		rg = (*cit)->equip_effect.equal_range(code);
		for (; rg.first != rg.second; ++rg.first)
			eset->add_item(rg.first->second);
	}
	if(sort)
		eset->sort();
}
void card::filter_immune_effect() {
	effect* peffect;
	immune_effect.clear();
	auto rg = single_effect.equal_range(EFFECT_IMMUNE_EFFECT);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (peffect->is_available())
			immune_effect.add_item(peffect);
	}
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		rg = (*cit)->equip_effect.equal_range(EFFECT_IMMUNE_EFFECT);
		for (; rg.first != rg.second; ++rg.first) {
			peffect = rg.first->second;
			if (peffect->is_available())
				immune_effect.add_item(peffect);
		}
	}
	rg = pduel->game_field->effects.aura_effect.equal_range(EFFECT_IMMUNE_EFFECT);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (peffect->is_target(this) && peffect->is_available())
			immune_effect.add_item(peffect);
	}
	immune_effect.sort();
}
void card::filter_disable_related_cards() {
	for (auto it = indexer.begin(); it != indexer.end(); ++it) {
		effect* peffect = it->first;
		if (peffect->is_disable_related()) {
			if (peffect->type & EFFECT_TYPE_FIELD)
				pduel->game_field->update_disable_check_list(peffect);
			else if ((peffect->type & EFFECT_TYPE_EQUIP) && equiping_target)
				pduel->game_field->add_to_disable_check_list(equiping_target);
		}
	}
}
int32 card::filter_summon_procedure(uint8 playerid, effect_set* peset, uint8 ignore_count, uint8 min_tribute) {
	effect_set eset;
	effect* proc;
	filter_effect(EFFECT_LIMIT_SUMMON_PROC, &eset);
	if(eset.size() > 1)
		return FALSE;
	if(eset.size()) {
		proc = eset[0];
		if(proc->check_count_limit(playerid) && is_summonable(proc, min_tribute)
		        && pduel->game_field->is_player_can_summon(proc->get_value(this), playerid, this)) {
			peset->add_item(eset[0]);
			return -1;
		}
		return -2;
	}
	eset.clear();
	filter_effect(EFFECT_SUMMON_PROC, &eset);
	for(int32 i = 0; i < eset.size(); ++i)
		if(eset[i]->check_count_limit(playerid) && is_summonable(eset[i], min_tribute)
		        && pduel->game_field->is_player_can_summon(eset[i]->get_value(this), playerid, this))
			peset->add_item(eset[i]);
	if(!pduel->game_field->is_player_can_summon(SUMMON_TYPE_NORMAL, playerid, this))
		return FALSE;
	int32 rcount = get_summon_tribute_count();
	int32 min = rcount & 0xffff, max = (rcount >> 16) & 0xffff;
	if(min > 0 && !pduel->game_field->is_player_can_summon(SUMMON_TYPE_ADVANCE, playerid, this))
		return FALSE;
	int32 fcount = pduel->game_field->get_useable_count(current.controler, LOCATION_MZONE, current.controler, LOCATION_REASON_TOFIELD);
	if(max <= -fcount)
		return FALSE;
	if(min < -fcount + 1)
		min = -fcount + 1;
	if(max < min_tribute)
		return FALSE;
	if(min < min_tribute)
		min = min_tribute;
	if(min == 0)
		return TRUE;
	int32 m = pduel->game_field->get_summon_release_list(this, 0, 0, 0);
	if(m >= min)
		return TRUE;
	return FALSE;
}
int32 card::filter_set_procedure(uint8 playerid, effect_set* peset, uint8 ignore_count, uint8 min_tribute) {
	effect_set eset;
	effect* proc;
	filter_effect(EFFECT_LIMIT_SET_PROC, &eset);
	if(eset.size() > 1)
		return FALSE;
	if(eset.size()) {
		proc = eset[0];
		if(proc->check_count_limit(playerid) && is_summonable(proc, min_tribute)
		        && pduel->game_field->is_player_can_mset(proc->get_value(this), playerid, this)) {
			peset->add_item(eset[0]);
			return -1;
		}
		return -2;
	}
	eset.clear();
	filter_effect(EFFECT_SET_PROC, &eset);
	for(int32 i = 0; i < eset.size(); ++i)
		if(eset[i]->check_count_limit(playerid) && is_summonable(eset[i], min_tribute)
		        && pduel->game_field->is_player_can_mset(eset[i]->get_value(this), playerid, this))
			peset->add_item(eset[i]);
	if(!pduel->game_field->is_player_can_mset(SUMMON_TYPE_NORMAL, playerid, this))
		return FALSE;
	int32 rcount = get_set_tribute_count();
	int32 min = rcount & 0xffff, max = (rcount >> 16) & 0xffff;
	if(min > 0 && !pduel->game_field->is_player_can_mset(SUMMON_TYPE_ADVANCE, playerid, this))
		return FALSE;
	int32 fcount = pduel->game_field->get_useable_count(current.controler, LOCATION_MZONE, current.controler, LOCATION_REASON_TOFIELD);
	if(max <= -fcount)
		return FALSE;
	if(min < -fcount + 1)
		min = -fcount + 1;
	if(max < min_tribute)
		return FALSE;
	if(min < min_tribute)
		min = min_tribute;
	if(min == 0)
		return TRUE;
	int32 m = pduel->game_field->get_summon_release_list(this, 0, 0, 0);
	if(m >= min)
		return TRUE;
	return FALSE;
}
void card::filter_spsummon_procedure(uint8 playerid, effect_set* peset, uint32 summon_type) {
	auto pr = field_effect.equal_range(EFFECT_SPSUMMON_PROC);
	uint8 toplayer;
	uint8 topos;
	effect* peffect;
	for(; pr.first != pr.second; ++pr.first) {
		peffect = pr.first->second;
		if(peffect->flag & EFFECT_FLAG_SPSUM_PARAM) {
			topos = peffect->s_range;
			if(peffect->o_range == 0)
				toplayer = playerid;
			else
				toplayer = 1 - playerid;
		} else {
			topos = POS_FACEUP;
			toplayer = playerid;
		}
		if(peffect->is_available() && peffect->check_count_limit(playerid) && is_summonable(peffect) 
				&& !pduel->game_field->check_unique_onfield(this, toplayer)) {
			uint32 sumtype = peffect->get_value(this);
			if((!summon_type || summon_type == sumtype)
			        && pduel->game_field->is_player_can_spsummon(peffect, sumtype, topos, playerid, toplayer, this))
				peset->add_item(peffect);
		}
	}
}
void card::filter_spsummon_procedure_g(uint8 playerid, effect_set* peset) {
	auto pr = field_effect.equal_range(EFFECT_SPSUMMON_PROC_G);
	for(; pr.first != pr.second; ++pr.first) {
		effect* peffect = pr.first->second;
		if(!peffect->is_available() || !peffect->check_count_limit(playerid))
			continue;
		if(current.controler != playerid && !(peffect->flag & EFFECT_FLAG_BOTH_SIDE))
			continue;
		effect* oreason = pduel->game_field->core.reason_effect;
		uint8 op = pduel->game_field->core.reason_player;
		pduel->game_field->core.reason_effect = peffect;
		pduel->game_field->core.reason_player = this->current.controler;
		pduel->game_field->save_lp_cost();
		pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		if(pduel->lua->check_condition(peffect->condition, 2))
			peset->add_item(peffect);
		pduel->game_field->restore_lp_cost();
		pduel->game_field->core.reason_effect = oreason;
		pduel->game_field->core.reason_player = op;
	}
}
effect* card::is_affected_by_effect(int32 code) {
	effect* peffect;
	auto rg = single_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (peffect->is_available() && (!(peffect->flag & EFFECT_FLAG_SINGLE_RANGE) || is_affect_by_effect(peffect)))
			return peffect;
	}
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		rg = (*cit)->equip_effect.equal_range(code);
		for (; rg.first != rg.second; ++rg.first) {
			peffect = rg.first->second;
			if (peffect->is_available() && is_affect_by_effect(peffect))
				return peffect;
		}
	}
	rg = pduel->game_field->effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (!(peffect->flag & EFFECT_FLAG_PLAYER_TARGET) && peffect->is_available()
		        && peffect->is_target(this) && is_affect_by_effect(peffect))
			return peffect;
	}
	return 0;
}
effect* card::is_affected_by_effect(int32 code, card* target) {
	effect* peffect;
	auto rg = single_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (peffect->is_available() && (!(peffect->flag & EFFECT_FLAG_SINGLE_RANGE) || is_affect_by_effect(peffect))
		        && peffect->get_value(target))
			return peffect;
	}
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		rg = (*cit)->equip_effect.equal_range(code);
		for (; rg.first != rg.second; ++rg.first) {
			peffect = rg.first->second;
			if (peffect->is_available() && is_affect_by_effect(peffect) && peffect->get_value(target))
				return peffect;
		}
	}
	rg = pduel->game_field->effects.aura_effect.equal_range(code);
	for (; rg.first != rg.second; ++rg.first) {
		peffect = rg.first->second;
		if (!(peffect->flag & EFFECT_FLAG_PLAYER_TARGET) && peffect->is_available()
		        && peffect->is_target(this) && is_affect_by_effect(peffect) && peffect->get_value(target))
			return peffect;
	}
	return 0;
}
effect* card::check_equip_control_effect() {
	effect* ret_effect = 0;
	for (auto cit = equiping_cards.begin(); cit != equiping_cards.end(); ++cit) {
		auto rg = (*cit)->equip_effect.equal_range(EFFECT_SET_CONTROL);
		for (; rg.first != rg.second; ++rg.first) {
			effect* peffect = rg.first->second;
			if(!ret_effect || peffect->id > ret_effect->id)
				ret_effect = peffect;
		}
	}
	return ret_effect;
}
int32 card::fusion_check(group* fusion_m, card* cg, int32 chkf) {
	auto ecit = single_effect.find(EFFECT_FUSION_MATERIAL);
	if(ecit == single_effect.end())
		return FALSE;
	effect* peffect = ecit->second;
	if(!peffect->condition)
		return FALSE;
	pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
	pduel->lua->add_param(fusion_m, PARAM_TYPE_GROUP);
	pduel->lua->add_param(cg, PARAM_TYPE_CARD);
	pduel->lua->add_param(chkf, PARAM_TYPE_INT);
	return pduel->lua->check_condition(peffect->condition, 4);
}
void card::fusion_select(uint8 playerid, group* fusion_m, card* cg, int32 chkf) {
	effect* peffect = 0;
	auto ecit = single_effect.find(EFFECT_FUSION_MATERIAL);
	if(ecit != single_effect.end())
		peffect = ecit->second;
	pduel->game_field->add_process(PROCESSOR_SELECT_FUSION, 0, peffect, fusion_m, playerid + (chkf << 16), (ptr)cg);
}
int32 card::is_equipable(card* pcard) {
	effect_set eset;
	if(this == pcard || pcard->current.location != LOCATION_MZONE)
		return FALSE;
	filter_effect(EFFECT_EQUIP_LIMIT, &eset);
	if(eset.size() == 0)
		return FALSE;
	for(int32 i = 0; i < eset.size(); ++i)
		if(eset[i]->get_value(pcard))
			return TRUE;
	return FALSE;
}
int32 card::is_summonable() {
	if(!(data.type & TYPE_MONSTER))
		return FALSE;
	return !(status & (STATUS_REVIVE_LIMIT | STATUS_UNSUMMONABLE_CARD));
}
int32 card::is_summonable(effect* peffect) {
	effect* oreason = pduel->game_field->core.reason_effect;
	uint8 op = pduel->game_field->core.reason_player;
	pduel->game_field->core.reason_effect = peffect;
	pduel->game_field->core.reason_player = this->current.controler;
	pduel->game_field->save_lp_cost();
	pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
	pduel->lua->add_param(this, PARAM_TYPE_CARD);
	uint32 result = FALSE;
	if(pduel->game_field->core.limit_tuner || pduel->game_field->core.limit_syn) {
		pduel->lua->add_param(pduel->game_field->core.limit_tuner, PARAM_TYPE_CARD);
		pduel->lua->add_param(pduel->game_field->core.limit_syn, PARAM_TYPE_GROUP);
		if(pduel->lua->check_condition(peffect->condition, 4))
			result = TRUE;
	} else if(pduel->game_field->core.limit_xyz) {
		pduel->lua->add_param(pduel->game_field->core.limit_xyz, PARAM_TYPE_GROUP);
		if(pduel->lua->check_condition(peffect->condition, 3))
			result = TRUE;
	} else {
		if(pduel->lua->check_condition(peffect->condition, 2))
			result = TRUE;
	}
	pduel->game_field->restore_lp_cost();
	pduel->game_field->core.reason_effect = oreason;
	pduel->game_field->core.reason_player = op;
	return result;
}
int32 card::is_summonable(effect* peffect, uint8 min_tribute) {
	effect* oreason = pduel->game_field->core.reason_effect;
	uint8 op = pduel->game_field->core.reason_player;
	pduel->game_field->core.reason_effect = peffect;
	pduel->game_field->core.reason_player = this->current.controler;
	pduel->game_field->save_lp_cost();
	uint32 result = FALSE;
	pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
	pduel->lua->add_param(this, PARAM_TYPE_CARD);
	pduel->lua->add_param(min_tribute, PARAM_TYPE_INT);
	if(pduel->lua->check_condition(peffect->condition, 3))
		result = TRUE;
	pduel->game_field->restore_lp_cost();
	pduel->game_field->core.reason_effect = oreason;
	pduel->game_field->core.reason_player = op;
	return result;
}
int32 card::is_can_be_summoned(uint8 playerid, uint8 ignore_count, effect* peffect, uint8 min_tribute) {
	if(!is_summonable())
		return FALSE;
	if(pduel->game_field->check_unique_onfield(this, playerid))
		return FALSE;
	if(!ignore_count && (pduel->game_field->core.extra_summon[playerid] || !is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT))
	        && pduel->game_field->core.summon_count[playerid] >= pduel->game_field->get_summon_count_limit(playerid))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_SUMMON_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	if(current.location == LOCATION_MZONE) {
		if(is_position(POS_FACEDOWN)
		        || !is_affected_by_effect(EFFECT_DUAL_SUMMONABLE)
		        || is_affected_by_effect(EFFECT_DUAL_STATUS)
		        || !pduel->game_field->is_player_can_summon(SUMMON_TYPE_DUAL, playerid, this)
		        || is_affected_by_effect(EFFECT_CANNOT_SUMMON)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	} else if(current.location == LOCATION_HAND) {
		if(is_status(STATUS_REVIVE_LIMIT) || is_affected_by_effect(EFFECT_CANNOT_SUMMON)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
		if(!ignore_count && !pduel->game_field->core.extra_summon[playerid]
		        && pduel->game_field->core.summon_count[playerid] >= pduel->game_field->get_summon_count_limit(playerid)) {
			effect* pextra = is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT);
			if(pextra && !(pextra->flag & EFFECT_FLAG_FUNC_VALUE)) {
				int32 count = pextra->get_value();
				if(min_tribute < count)
					min_tribute = count;
			}
		}
		effect_set proc;
		int32 res = filter_summon_procedure(playerid, &proc, ignore_count, min_tribute);
		if((peffect && res < 0) || (!peffect && (!res || res == -2) && !proc.size())
		        || (peffect && (proc.size() == 0) && !pduel->game_field->is_player_can_summon(peffect->get_value(), playerid, this))) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::get_summon_tribute_count() {
	int32 min = 0, max = 0;
	int32 minul = 0, maxul = 0;
	int32 level = get_level();
	if(level < 5)
		return 0;
	else if(level < 7)
		min = max = 1;
	else
		min = max = 2;
	effect_set eset;
	filter_effect(EFFECT_DECREASE_TRIBUTE, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		int32 dec = eset[i]->get_value(this);
		if(!(eset[i]->flag & EFFECT_FLAG_COUNT_LIMIT)) {
			if(minul < (dec & 0xffff))
				minul = dec & 0xffff;
			if(maxul < (dec >> 16))
				maxul = dec >> 16;
		} else if((eset[i]->reset_count & 0xf00) > 0) {
			min -= dec & 0xffff;
			max -= dec >> 16;
		}
	}
	min -= minul;
	max -= maxul;
	if(min < 0) min = 0;
	if(max < min) max = min;
	return min + (max << 16);
}
int32 card::get_set_tribute_count() {
	int32 min = 0, max = 0;
	int32 level = get_level();
	if(level < 5)
		return 0;
	else if(level < 7)
		min = max = 1;
	else
		min = max = 2;
	effect_set eset;
	filter_effect(EFFECT_DECREASE_TRIBUTE_SET, &eset);
	if(eset.size()) {
		int32 dec = eset.get_last()->get_value(this);
		min -= dec & 0xffff;
		max -= dec >> 16;
	}
	if(min < 0) min = 0;
	if(max < min) max = min;
	return min + (max << 16);
}
int32 card::is_can_be_flip_summoned(uint8 playerid) {
	if(is_status(STATUS_SUMMON_TURN) || is_status(STATUS_FLIP_SUMMON_TURN) || is_status(STATUS_FORM_CHANGED))
		return FALSE;
	if(announce_count > 0)
		return FALSE;
	if(current.location != LOCATION_MZONE)
		return FALSE;
	if(!(current.position & POS_FACEDOWN))
		return FALSE;
	if(pduel->game_field->check_unique_onfield(this, playerid))
		return FALSE;
	if(!pduel->game_field->is_player_can_flipsummon(playerid, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_FLIP_SUMMON))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_CHANGE_POSITION))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_FLIPSUMMON_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::is_special_summonable(uint8 playerid, uint32 summon_type) {
	if(!(data.type & TYPE_MONSTER))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_SPECIAL_SUMMON))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_SPSUMMON_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	eset.clear();
	filter_spsummon_procedure(playerid, &eset, summon_type);
	pduel->game_field->core.limit_tuner = 0;
	pduel->game_field->core.limit_xyz = 0;
	pduel->game_field->core.limit_syn = 0;
	pduel->game_field->restore_lp_cost();
	return eset.size();
}
int32 card::is_can_be_special_summoned(effect * reason_effect, uint32 sumtype, uint8 sumpos, uint8 sumplayer, uint8 toplayer, uint8 nocheck, uint8 nolimit) {
	if(current.location == LOCATION_MZONE)
		return FALSE;
	if(current.location == LOCATION_REMOVED && (current.position & POS_FACEDOWN))
		return FALSE;
	if(is_status(STATUS_REVIVE_LIMIT) && !is_status(STATUS_PROC_COMPLETE)) {
		if((!nolimit && (current.location & 0x38)) || (!nocheck && !nolimit && (current.location & 0x3)))
			return FALSE;
	}
	if(((sumpos & POS_FACEDOWN) == 0) && pduel->game_field->check_unique_onfield(this, toplayer))
		return FALSE;
	sumtype |= SUMMON_TYPE_SPECIAL;
	if((sumplayer == 0 || sumplayer == 1) && !pduel->game_field->is_player_can_spsummon(reason_effect, sumtype, sumpos, sumplayer, toplayer, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_SPECIAL_SUMMON))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_SPSUMMON_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(sumplayer, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	if(!nocheck) {
		eset.clear();
		if(!(data.type & TYPE_MONSTER)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
		filter_effect(EFFECT_SPSUMMON_CONDITION, &eset);
		for(int32 i = 0; i < eset.size(); ++i) {
			pduel->lua->add_param(reason_effect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(sumplayer, PARAM_TYPE_INT);
			pduel->lua->add_param(sumtype, PARAM_TYPE_INT);
			pduel->lua->add_param(sumpos, PARAM_TYPE_INT);
			pduel->lua->add_param(toplayer, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(5)) {
				pduel->game_field->restore_lp_cost();
				return FALSE;
			}
		}
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::is_setable_mzone(uint8 playerid, uint8 ignore_count, effect* peffect, uint8 min_tribute) {
	if(!(data.type & TYPE_MONSTER))
		return FALSE;
	if(status & (STATUS_REVIVE_LIMIT | STATUS_UNSUMMONABLE_CARD))
		return FALSE;
	if(current.location != LOCATION_HAND)
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_MSET))
		return FALSE;
	if(!ignore_count && (pduel->game_field->core.extra_summon[playerid] || !is_affected_by_effect(EFFECT_EXTRA_SET_COUNT))
	        && pduel->game_field->core.summon_count[playerid] >= pduel->game_field->get_summon_count_limit(playerid))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_MSET_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	if(!ignore_count && !pduel->game_field->core.extra_summon[playerid]
	        && pduel->game_field->core.summon_count[playerid] >= pduel->game_field->get_summon_count_limit(playerid)) {
		effect* pextra = is_affected_by_effect(EFFECT_EXTRA_SET_COUNT);
		if(pextra && !(pextra->flag & EFFECT_FLAG_FUNC_VALUE)) {
			int32 count = pextra->get_value();
			if(min_tribute < count)
				min_tribute = count;
		}
	}
	eset.clear();
	int32 res = filter_set_procedure(playerid, &eset, ignore_count, min_tribute);
	if((peffect && res < 0) || (!peffect && (!res || res == -2) && !eset.size())
	        || (peffect && (eset.size() == 0) && !pduel->game_field->is_player_can_mset(peffect->get_value(), playerid, this))) {
		pduel->game_field->restore_lp_cost();
		return FALSE;
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::is_setable_szone(uint8 playerid, uint8 ignore_fd) {
	if(!(data.type & TYPE_FIELD) && !ignore_fd && pduel->game_field->get_useable_count(current.controler, LOCATION_SZONE, current.controler, LOCATION_REASON_TOFIELD) <= 0)
		return FALSE;
	if(data.type & TYPE_MONSTER && !is_affected_by_effect(EFFECT_MONSTER_SSET))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_SSET))
		return FALSE;
	if(!pduel->game_field->is_player_can_sset(playerid, this))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	filter_effect(EFFECT_SSET_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::is_affect_by_effect(effect* peffect) {
	if(is_status(STATUS_SUMMONING))
		return FALSE;
	if(!peffect || (peffect->flag & EFFECT_FLAG_IGNORE_IMMUNE))
		return TRUE;
	if(peffect->is_immuned(&immune_effect))
		return FALSE;
	return TRUE;
}
int32 card::is_destructable() {
	if(overlay_target)
		return FALSE;
	if(current.location & (LOCATION_GRAVE + LOCATION_REMOVED))
		return FALSE;
	if(is_affected_by_effect(EFFECT_INDESTRUCTABLE))
		return FALSE;
	return TRUE;
}
int32 card::is_destructable_by_battle(card * pcard) {
	if(is_affected_by_effect(EFFECT_INDESTRUCTABLE_BATTLE, pcard))
		return FALSE;
	return TRUE;
}
effect* card::check_indestructable_by_effect(effect* peffect, uint8 playerid) {
	if(!peffect)
		return 0;
	effect_set eset;
	filter_effect(EFFECT_INDESTRUCTABLE_EFFECT, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		if(eset[i]->check_value_condition(3))
			return eset[i];
	}
	return 0;
}
int32 card::is_destructable_by_effect(effect* peffect, uint8 playerid) {
	return !check_indestructable_by_effect(peffect, playerid);
}
int32 card::is_removeable(uint8 playerid) {
	if(!pduel->game_field->is_player_can_remove(playerid, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_REMOVE))
		return FALSE;
	return TRUE;
}
int32 card::is_removeable_as_cost(uint8 playerid) {
	if(is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
		return FALSE;
	if(!pduel->game_field->is_player_can_remove(playerid, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_REMOVE))
		return FALSE;
	return TRUE;
}
int32 card::is_releasable_by_summon(uint8 playerid, card *pcard) {
	if(is_status(STATUS_SUMMONING))
		return FALSE;
	if(overlay_target)
		return FALSE;
	if(current.location & (LOCATION_GRAVE + LOCATION_REMOVED))
		return FALSE;
	if(!pduel->game_field->is_player_can_release(playerid, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_UNRELEASABLE_SUM, pcard))
		return FALSE;
	if(pcard->is_affected_by_effect(EFFECT_TRIBUTE_LIMIT, this))
		return FALSE;
	return TRUE;
}
int32 card::is_releasable_by_nonsummon(uint8 playerid) {
	if(is_status(STATUS_SUMMONING))
		return FALSE;
	if(overlay_target)
		return FALSE;
	if(current.location & (LOCATION_GRAVE + LOCATION_REMOVED))
		return FALSE;
	if((current.location == LOCATION_HAND) && (data.type & (TYPE_SPELL | TYPE_TRAP)))
		return FALSE;
	if(!pduel->game_field->is_player_can_release(playerid, this))
		return FALSE;
	if(is_affected_by_effect(EFFECT_UNRELEASABLE_NONSUM))
		return FALSE;
	return TRUE;
}
int32 card::is_releasable_by_effect(uint8 playerid, effect* peffect) {
	if(!peffect)
		return TRUE;
	effect_set eset;
	filter_effect(EFFECT_UNRELEASABLE_EFFECT, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(peffect, PARAM_TYPE_EFFECT);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		if(eset[i]->check_value_condition(3))
			return FALSE;
	}
	return TRUE;
}
int32 card::is_capable_send_to_grave(uint8 playerid) {
	if(is_affected_by_effect(EFFECT_CANNOT_TO_GRAVE))
		return FALSE;
	if(!pduel->game_field->is_player_can_send_to_grave(playerid, this))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_send_to_hand(uint8 playerid) {
	if(is_status(STATUS_LEAVE_CONFIRMED))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_TO_HAND))
		return FALSE;
	if(!pduel->game_field->is_player_can_send_to_hand(playerid, this))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_send_to_deck(uint8 playerid) {
	if(is_status(STATUS_LEAVE_CONFIRMED))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_TO_DECK))
		return FALSE;
	if(!pduel->game_field->is_player_can_send_to_deck(playerid, this))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_send_to_extra(uint8 playerid) {
	if(!(data.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_TO_DECK))
		return FALSE;
	if(!pduel->game_field->is_player_can_send_to_deck(playerid, this))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_cost_to_grave(uint8 playerid) {
	uint32 redirect = 0;
	uint32 dest = LOCATION_GRAVE;
	if(data.type & TYPE_TOKEN)
		return FALSE;
	if((data.type & TYPE_PENDULUM) && (current.location & LOCATION_ONFIELD))
		return FALSE;
	if(current.location == LOCATION_GRAVE)
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
		return FALSE;
	if(!is_capable_send_to_grave(playerid))
		return FALSE;
	uint32 op_param = operation_param;
	operation_param = dest << 8;
	if(current.location & LOCATION_ONFIELD)
		redirect = leave_field_redirect(REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	redirect = destination_redirect(dest, REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	operation_param = op_param;
	if(dest != LOCATION_GRAVE)
		return FALSE;
	return TRUE;
}
int32 card::is_capable_cost_to_hand(uint8 playerid) {
	uint32 redirect = 0;
	uint32 dest = LOCATION_HAND;
	if(data.type & (TYPE_TOKEN | TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ))
		return FALSE;
	if(current.location == LOCATION_HAND)
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
		return FALSE;
	if(!is_capable_send_to_hand(playerid))
		return FALSE;
	uint32 op_param = operation_param;
	operation_param = dest << 8;
	if(current.location & LOCATION_ONFIELD)
		redirect = leave_field_redirect(REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	redirect = destination_redirect(dest, REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	operation_param = op_param;
	if(dest != LOCATION_HAND)
		return FALSE;
	return TRUE;
}
int32 card::is_capable_cost_to_deck(uint8 playerid) {
	uint32 redirect = 0;
	uint32 dest = LOCATION_DECK;
	if(data.type & (TYPE_TOKEN | TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ))
		return FALSE;
	if(current.location == LOCATION_DECK)
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
		return FALSE;
	if(!is_capable_send_to_deck(playerid))
		return FALSE;
	uint32 op_param = operation_param;
	operation_param = dest << 8;
	if(current.location & LOCATION_ONFIELD)
		redirect = leave_field_redirect(REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	redirect = destination_redirect(dest, REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	operation_param = op_param;
	if(dest != LOCATION_DECK)
		return FALSE;
	return TRUE;
}
int32 card::is_capable_cost_to_extra(uint8 playerid) {
	uint32 redirect = 0;
	uint32 dest = LOCATION_DECK;
	if(!(data.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)))
		return FALSE;
	if(current.location == LOCATION_EXTRA)
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
		return FALSE;
	if(!is_capable_send_to_deck(playerid))
		return FALSE;
	uint32 op_param = operation_param;
	operation_param = dest << 8;
	if(current.location & LOCATION_ONFIELD)
		redirect = leave_field_redirect(REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	redirect = destination_redirect(dest, REASON_COST) & 0xffff;
	if(redirect) dest = redirect;
	operation_param = op_param;
	if(dest != LOCATION_DECK)
		return FALSE;
	return TRUE;
}
int32 card::is_capable_attack() {
	if(!is_position(POS_FACEUP_ATTACK) && !(is_position(POS_FACEUP_DEFENCE) && is_affected_by_effect(EFFECT_DEFENCE_ATTACK)))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_ATTACK))
		return FALSE;
	if(is_affected_by_effect(EFFECT_ATTACK_DISABLED))
		return FALSE;
	if(pduel->game_field->is_player_affected_by_effect(pduel->game_field->infos.turn_player, EFFECT_SKIP_BP))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_attack_announce(uint8 playerid) {
	if(!is_capable_attack())
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_ATTACK_ANNOUNCE))
		return FALSE;
	pduel->game_field->save_lp_cost();
	effect_set eset;
	pduel->game_field->filter_player_effect(playerid, EFFECT_ATTACK_COST, &eset, FALSE);
	filter_effect(EFFECT_ATTACK_COST, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(eset[i], PARAM_TYPE_EFFECT);
		pduel->lua->add_param(this, PARAM_TYPE_CARD);
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(!pduel->lua->check_condition(eset[i]->cost, 3)) {
			pduel->game_field->restore_lp_cost();
			return FALSE;
		}
	}
	pduel->game_field->restore_lp_cost();
	return TRUE;
}
int32 card::is_capable_change_position(uint8 playerid) {
	if(is_status(STATUS_SUMMON_TURN) || is_status(STATUS_FLIP_SUMMON_TURN) || is_status(STATUS_FORM_CHANGED))
		return FALSE;
	if(announce_count > 0)
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_CHANGE_POSITION))
		return FALSE;
	if(pduel->game_field->is_player_affected_by_effect(playerid, EFFECT_CANNOT_CHANGE_POSITION))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_turn_set(uint8 playerid) {
	if(data.type & TYPE_TOKEN)
		return FALSE;
	if(is_position(POS_FACEDOWN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_TURN_SET))
		return FALSE;
	if(pduel->game_field->is_player_affected_by_effect(playerid, EFFECT_CANNOT_TURN_SET))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_change_control() {
	if(is_affected_by_effect(EFFECT_CANNOT_CHANGE_CONTROL))
		return FALSE;
	return TRUE;
}
int32 card::is_control_can_be_changed() {
	if(current.controler == PLAYER_NONE)
		return FALSE;
	if(current.location != LOCATION_MZONE)
		return FALSE;
	if(pduel->game_field->get_useable_count(1 - current.controler, LOCATION_MZONE, current.controler, LOCATION_REASON_CONTROL) <= 0)
		return FALSE;
	if((data.type & TYPE_TRAPMONSTER) && pduel->game_field->get_useable_count(1 - current.controler, LOCATION_MZONE, current.controler, LOCATION_REASON_CONTROL) <= 0)
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_CHANGE_CONTROL))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_be_battle_target(card* pcard) {
	if(is_affected_by_effect(EFFECT_CANNOT_BE_BATTLE_TARGET, pcard))
		return FALSE;
	if(is_affected_by_effect(EFFECT_IGNORE_BATTLE_TARGET))
		return FALSE;
	return TRUE;
}
int32 card::is_capable_be_effect_target(effect* peffect, uint8 playerid) {
	if(is_status(STATUS_SUMMONING) || is_status(STATUS_BATTLE_DESTROYED))
		return FALSE;
	effect_set eset;
	filter_effect(EFFECT_CANNOT_BE_EFFECT_TARGET, &eset);
	for(int32 i = 0; i < eset.size(); ++i) {
		pduel->lua->add_param(playerid, PARAM_TYPE_INT);
		if(eset[i]->get_value(peffect, 1))
			return FALSE;
	}
	return TRUE;
}
int32 card::is_can_be_fusion_material(uint8 ignore_mon) {
	if(!ignore_mon && !(get_type() & TYPE_MONSTER))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	if(is_affected_by_effect(EFFECT_CANNOT_BE_FUSION_MATERIAL))
		return FALSE;
	return TRUE;
}
int32 card::is_can_be_synchro_material(card* scard, card* tuner) {
	if(data.type & TYPE_XYZ)
		return FALSE;
	if(!(get_type()&TYPE_MONSTER))
		return FALSE;
	if(scard && current.controler != scard->current.controler && !is_affected_by_effect(EFFECT_SYNCHRO_MATERIAL))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	//special fix for scrap chimera, not perfect yet
	if(tuner && (pduel->game_field->core.global_flag & GLOBALFLAG_SCRAP_CHIMERA)) {
		if(is_affected_by_effect(EFFECT_SCRAP_CHIMERA, tuner))
			return false;
	}
	effect_set eset;
	filter_effect(EFFECT_CANNOT_BE_SYNCHRO_MATERIAL, &eset);
	for(int32 i = 0; i < eset.size(); ++i)
		if(eset[i]->get_value(scard))
			return FALSE;
	return TRUE;
}
int32 card::is_can_be_xyz_material(card* scard) {
	if(data.type & TYPE_TOKEN)
		return FALSE;
	if(!(get_type()&TYPE_MONSTER))
		return FALSE;
	if(is_affected_by_effect(EFFECT_FORBIDDEN))
		return FALSE;
	effect_set eset;
	filter_effect(EFFECT_CANNOT_BE_XYZ_MATERIAL, &eset);
	for(int32 i = 0; i < eset.size(); ++i)
		if(eset[i]->get_value(scard))
			return FALSE;
	return TRUE;
}
