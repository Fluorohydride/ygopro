/*
 * libcard.cpp
 *
 *  Created on: 2010-5-6
 *      Author: Argon
 */

#include "scriptlib.h"
#include "duel.h"
#include "field.h"
#include "card.h"
#include "effect.h"
#include "group.h"
#include <iostream>

int32 scriptlib::card_get_code(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_code());
	uint32 otcode = pcard->get_another_code();
	if(otcode) {
		lua_pushinteger(L, otcode);
		return 2;
	}
	return 1;
}
int32 scriptlib::card_get_origin_code(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->data.alias) {
		int32 dif = pcard->data.code - pcard->data.alias;
		if(dif > -10 && dif < 10)
			lua_pushinteger(L, pcard->data.alias);
		else
			lua_pushinteger(L, pcard->data.code);
	} else
		lua_pushinteger(L, pcard->data.code);
	return 1;
}
int32 scriptlib::card_is_set_card(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 set_code = lua_tointeger(L, 2);
	lua_pushboolean(L, pcard->is_set_card(set_code));
	return 1;
}
int32 scriptlib::card_get_type(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_type());
	return 1;
}
int32 scriptlib::card_get_origin_type(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->data.type);
	return 1;
}
int32 scriptlib::card_get_level(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_level());
	return 1;
}
int32 scriptlib::card_get_rank(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_rank());
	return 1;
}
int32 scriptlib::card_get_synchro_level(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* scard = *(card**) lua_touserdata(L, 2);
	lua_pushinteger(L, pcard->get_synchro_level(scard));
	return 1;
}
int32 scriptlib::card_get_ritual_level(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* scard = *(card**) lua_touserdata(L, 2);
	lua_pushinteger(L, pcard->get_ritual_level(scard));
	return 1;
}
int32 scriptlib::card_get_origin_level(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->data.type & TYPE_XYZ)
		lua_pushinteger(L, 0);
	else
		lua_pushinteger(L, pcard->data.level);
	return 1;
}
int32 scriptlib::card_is_xyz_level(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* xyzcard = *(card**) lua_touserdata(L, 2);
	uint32 lv = lua_tointeger(L, 3);
	lua_pushboolean(L, pcard->is_xyz_level(xyzcard, lv));
	return 1;
}
int32 scriptlib::card_get_attribute(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_attribute());
	return 1;
}
int32 scriptlib::card_get_origin_attribute(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->data.attribute);
	return 1;
}
int32 scriptlib::card_get_race(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_race());
	return 1;
}
int32 scriptlib::card_get_origin_race(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->data.race);
	return 1;
}
int32 scriptlib::card_get_attack(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_attack());
	return 1;
}
int32 scriptlib::card_get_origin_attack(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_base_attack());
	return 1;
}
int32 scriptlib::card_get_text_attack(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->data.attack);
	return 1;
}
int32 scriptlib::card_get_defence(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_defence());
	return 1;
}
int32 scriptlib::card_get_origin_defence(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_base_defence());
	return 1;
}
int32 scriptlib::card_get_text_defence(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->data.defence);
	return 1;
}
int32 scriptlib:: card_get_previous_code_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.code);
	return 1;
}
int32 scriptlib::card_get_previous_type_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.type);
	return 1;
}
int32 scriptlib::card_get_previous_level_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.level);
	return 1;
}
int32 scriptlib::card_get_previous_rank_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.rank);
	return 1;
}
int32 scriptlib::card_get_previous_attribute_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.attribute);
	return 1;
}
int32 scriptlib::card_get_previous_race_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.race);
	return 1;
}
int32 scriptlib::card_get_previous_attack_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.attack);
	return 1;
}
int32 scriptlib::card_get_previous_defence_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.defence);
	return 1;
}
int32 scriptlib::card_get_owner(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->owner);
	return 1;
}
int32 scriptlib::card_get_controler(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->current.controler);
	return 1;
}
int32 scriptlib::card_get_previous_controler(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.controler);
	return 1;
}
int32 scriptlib::card_get_reason(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->current.reason);
	return 1;
}
int32 scriptlib::card_get_reason_card(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	interpreter::card2value(L, pcard->current.reason_card);
	return 1;
}
int32 scriptlib::card_get_reason_player(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->current.reason_player);
	return 1;
}
int32 scriptlib::card_get_reason_effect(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	interpreter::effect2value(L, pcard->current.reason_effect);
	return 1;
}
int32 scriptlib::card_get_position(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->current.position);
	return 1;
}
int32 scriptlib::card_get_previous_position(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.position);
	return 1;
}
int32 scriptlib::card_get_battle_position(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->temp.position);
	return 1;
}
int32 scriptlib::card_get_location(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->is_status(STATUS_SUMMONING) || pcard->is_status(STATUS_SUMMON_DISABLED))
		lua_pushinteger(L, 0);
	else
		lua_pushinteger(L, pcard->current.location);
	return 1;
}
int32 scriptlib::card_get_previous_location(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.location);
	return 1;
}
int32 scriptlib::card_get_sequence(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->current.sequence);
	return 1;
}
int32 scriptlib::card_get_previous_sequence(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->previous.sequence);
	return 1;
}
int32 scriptlib::card_get_summon_type(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->summon_type);
	return 1;
}
int32 scriptlib::card_get_summon_player(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->summon_player);
	return 1;
}
int32 scriptlib::card_get_destination(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, (pcard->operation_param >> 8) & 0xff);
	return 1;
}
int32 scriptlib::card_get_leave_field_dest(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->leave_field_redirect(REASON_EFFECT));
	return 1;
}
int32 scriptlib::card_get_turnid(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->turnid);
	return 1;
}
int32 scriptlib::card_get_fieldid(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->fieldid);
	return 1;
}
int32 scriptlib::card_get_fieldidr(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->fieldid_r);
	return 1;
}
int32 scriptlib::card_is_code(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 tcode = lua_tointeger(L, 2);
	if(pcard->get_code() == tcode || pcard->get_another_code() == tcode)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_type(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 ttype = lua_tointeger(L, 2);
	if(pcard->get_type() & ttype)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_race(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 trace = lua_tointeger(L, 2);
	if(pcard->get_race() & trace)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_attribute(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 tattrib = lua_tointeger(L, 2);
	if(pcard->get_attribute() & tattrib)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_reason(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 treason = lua_tointeger(L, 2);
	if(pcard->current.reason & treason)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_status(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 tstatus = lua_tointeger(L, 2);
	if(pcard->status & tstatus)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_not_tuner(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 type = pcard->get_type();
	if(!(type & TYPE_TUNER) || pcard->is_affected_by_effect(EFFECT_NONTUNER))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_set_status(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->status & STATUS_COPYING_EFFECT)
		return 0;
	uint32 tstatus = lua_tointeger(L, 2);
	int32 enable = lua_toboolean(L, 3);
	pcard->set_status(tstatus, enable);
	return 0;
}
int32 scriptlib::card_is_dual_state(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, (ptr)pcard->is_affected_by_effect(EFFECT_DUAL_STATUS));
	return 1;
}
int32 scriptlib::card_enable_dual_state(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	effect* deffect = pduel->new_effect();
	deffect->owner = pcard;
	deffect->code = EFFECT_DUAL_STATUS;
	deffect->type = EFFECT_TYPE_SINGLE;
	deffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	deffect->reset_flag = RESET_EVENT + 0x1fe0000;
	pcard->add_effect(deffect);
	return 0;
}
int32 scriptlib::card_set_turn_counter(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 ct = lua_tointeger(L, 2);
	pcard->count_turn(ct);
	return 0;
}
int32 scriptlib::card_get_turn_counter(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->turn_counter);
	return 1;
}
int32 scriptlib::card_set_material(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_GROUP, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = *(group**) lua_touserdata(L, 2);
	pcard->set_material(&pgroup->container);
	return 0;
}
int32 scriptlib::card_get_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	pgroup->container.insert(pcard->material_cards.begin(), pcard->material_cards.end());
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_material_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->material_cards.size());
	return 1;
}
int32 scriptlib::card_get_equip_group(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	pgroup->container.insert(pcard->equiping_cards.begin(), pcard->equiping_cards.end());
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_equip_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->equiping_cards.size());
	return 1;
}
int32 scriptlib::card_get_equip_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	interpreter::card2value(L, pcard->equiping_target);
	return 1;
}
int32 scriptlib::card_get_pre_equip_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	interpreter::card2value(L, pcard->pre_equip_target);
	return 1;
}
int32 scriptlib::card_check_equip_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* target = *(card**) lua_touserdata(L, 2);
	if(pcard->is_affected_by_effect(EFFECT_EQUIP_LIMIT, target)
	        && (!pcard->is_status(STATUS_UNION) || target->get_union_count() == 0))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_get_union_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->get_union_count());
	return 1;
}
int32 scriptlib::card_get_overlay_group(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	pgroup->container.insert(pcard->xyz_materials.begin(), pcard->xyz_materials.end());
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_overlay_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->xyz_materials.size());
	return 1;
}
int32 scriptlib::card_get_overlay_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	interpreter::card2value(L, pcard->overlay_target);
	return 1;
}
int32 scriptlib::card_check_remove_overlay_card(lua_State *L) {
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 count = lua_tointeger(L, 3);
	int32 reason = lua_tointeger(L, 4);
	duel* pduel = pcard->pduel;
	lua_pushboolean(L, pduel->game_field->is_player_can_remove_overlay_card(playerid, pcard, 0, 0, count, reason));
	return 1;
}
int32 scriptlib::card_remove_overlay_card(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 min = lua_tointeger(L, 3);
	int32 max = lua_tointeger(L, 4);
	int32 reason = lua_tointeger(L, 5);
	duel* pduel = pcard->pduel;
	pduel->game_field->remove_overlay_card(reason, pcard, playerid, 0, 0, min, max);
	return lua_yield(L, 0);
}
int32 scriptlib::card_get_attacked_group(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	card::attacker_map::iterator cit;
	for(cit = pcard->attacked_cards.begin(); cit != pcard->attacked_cards.end(); ++cit) {
		if(cit->second)
			pgroup->container.insert(cit->second);
	}
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_attacked_group_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->attacked_cards.size());
	return 1;
}
int32 scriptlib::card_get_attacked_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->attacked_count);
	return 1;
}
int32 scriptlib::card_get_battled_group(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	card::attacker_map::iterator cit;
	for(cit = pcard->battled_cards.begin(); cit != pcard->battled_cards.end(); ++cit) {
		if(cit->second)
			pgroup->container.insert(cit->second);
	}
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_battled_group_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->battled_cards.size());
	return 1;
}
int32 scriptlib::card_get_attack_announced_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->announce_count);
	return 1;
}
int32 scriptlib::card_is_direct_attacked(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	bool ret = false;
	for(auto cit = pcard->attacked_cards.begin(); cit != pcard->attacked_cards.end(); ++cit)
		if(cit->first == 0)
			ret = true;
	lua_pushboolean(L, ret);
	return 1;
}
int32 scriptlib::card_set_card_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* ocard = *(card**) lua_touserdata(L, 2);
	pcard->add_card_target(ocard);
	return 0;
}
int32 scriptlib::card_get_card_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	pgroup->container = pcard->effect_target_cards;
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_first_card_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->effect_target_cards.size())
		interpreter::card2value(L, *pcard->effect_target_cards.begin());
	else lua_pushnil(L);
	return 1;
}
int32 scriptlib::card_get_card_target_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->effect_target_cards.size());
	return 1;
}
int32 scriptlib::card_is_has_card_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* rcard = *(card**) lua_touserdata(L, 2);
	lua_pushboolean(L, pcard->effect_target_cards.count(rcard));
	return 1;
}
int32 scriptlib::card_cancel_card_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* rcard = *(card**) lua_touserdata(L, 2);
	pcard->cancel_card_target(rcard);
	return 0;
}
int32 scriptlib::card_get_owner_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	group* pgroup = pcard->pduel->new_group();
	pgroup->container = pcard->effect_target_owner;
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::card_get_owner_target_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushinteger(L, pcard->effect_target_owner.size());
	return 1;
}
int32 scriptlib::card_get_activate_effect(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 count = 0;
	for(auto eit = pcard->field_effect.begin(); eit != pcard->field_effect.end(); ++eit) {
		if(eit->second->type & EFFECT_TYPE_ACTIVATE) {
			interpreter::effect2value(L, eit->second);
			count++;
		}
	}
	return count;
}
int32 scriptlib::card_check_activate_effect(lua_State *L) {
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 neglect_con = lua_toboolean(L, 2);
	int32 neglect_cost = lua_toboolean(L, 3);
	int32 copy_info = lua_toboolean(L, 4);
	duel* pduel = pcard->pduel;
	tevent pe;
	for(auto eit = pcard->field_effect.begin(); eit != pcard->field_effect.end(); ++eit) {
		effect* peffect = eit->second;
		if((peffect->type & EFFECT_TYPE_ACTIVATE)
		        && pduel->game_field->check_event_c(peffect, pduel->game_field->core.reason_player, neglect_con, neglect_cost, copy_info, &pe)) {
			if(!copy_info || (peffect->code == EVENT_FREE_CHAIN)) {
				interpreter::effect2value(L, peffect);
				return 1;
			} else {
				interpreter::effect2value(L, peffect);
				interpreter::group2value(L, pe.event_cards);
				lua_pushinteger(L, pe.event_player);
				lua_pushinteger(L, pe.event_value);
				interpreter::effect2value(L, pe.reason_effect);
				lua_pushinteger(L, pe.reason);
				lua_pushinteger(L, pe.reason_player);
				return 7;
			}
		}
	}
	return 0;
}
int32 scriptlib::card_register_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	int32 forced = lua_toboolean(L, 3);
	duel* pduel = pcard->pduel;
	if(peffect->owner == pduel->game_field->temp_card)
		return 0;
	if(!forced && pduel->game_field->core.reason_effect && !pcard->is_affect_by_effect(pduel->game_field->core.reason_effect)) {
		pduel->game_field->core.reseted_effects.insert(peffect);
		return 0;
	}
	int32 id;
	if (peffect->handler)
		id = -1;
	else
		id = pcard->add_effect(peffect);
	lua_pushinteger(L, id);
	return 1;
}
int32 scriptlib::card_is_has_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 code = lua_tointeger(L, 2);
	if(pcard && pcard->is_affected_by_effect(code))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_reset_effect(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 code = lua_tointeger(L, 2);
	uint32 type = lua_tointeger(L, 3);
	pcard->reset(code, type);
	return 0;
}
int32 scriptlib::card_get_effect_count(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 code = lua_tointeger(L, 2);
	effect_set eset;
	pcard->filter_effect(code, &eset);
	lua_pushinteger(L, eset.count);
	return 1;
}
int32 scriptlib::card_register_flag_effect(lua_State *L) {
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	int32 reset = lua_tointeger(L, 3);
	int32 flag = lua_tointeger(L, 4);
	int32 count = lua_tointeger(L, 5);
	int32 lab = 0;
	int32 desc = 0;
	if(lua_gettop(L) >= 6)
		lab = lua_tointeger(L, 6);
	if(lua_gettop(L) >= 7)
		desc = lua_tointeger(L, 7);
	if(count == 0)
		count = 1;
	if(reset & (RESET_PHASE) && !(reset & (RESET_SELF_TURN | RESET_OPPO_TURN)))
		reset |= (RESET_SELF_TURN | RESET_OPPO_TURN);
	duel* pduel = pcard->pduel;
	effect* peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->handler = 0;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = code;
	peffect->reset_flag = reset;
	peffect->flag = flag | EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_count |= count & 0xff;
	peffect->label = lab;
	peffect->description = desc;
	pcard->add_effect(peffect);
	interpreter::effect2value(L, peffect);
	return 1;
}
int32 scriptlib::card_get_flag_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	lua_pushinteger(L, pcard->single_effect.count(code));
	return 1;
}
int32 scriptlib::card_reset_flag_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	pcard->reset(code, RESET_CODE);
	return 0;
}
int32 scriptlib::card_set_flag_effect_label(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	int lab = lua_tointeger(L, 3);
	auto eit = pcard->single_effect.find(code);
	if(eit == pcard->single_effect.end())
		lua_pushboolean(L, FALSE);
	else {
		eit->second->label = lab;
		lua_pushboolean(L, TRUE);
	}
	return 1;
}
int32 scriptlib::card_get_flag_effect_label(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	auto eit = pcard->single_effect.find(code);
	if(eit == pcard->single_effect.end())
		lua_pushnil(L);
	else
		lua_pushinteger(L, eit->second->label);
	return 1;
}
int32 scriptlib::card_create_relation(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* rcard = *(card**) lua_touserdata(L, 2);
	uint32 reset = lua_tointeger(L, 3);
	pcard->create_relation(rcard, reset);
	return 0;
}
int32 scriptlib::card_release_relation(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* rcard = *(card**) lua_touserdata(L, 2);
	pcard->release_relation(rcard);
	return 0;
}
int32 scriptlib::card_create_effect_relation(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	pcard->create_relation(peffect);
	return 0;
}
int32 scriptlib::card_release_effect_relation(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	pcard->release_relation(peffect);
	return 0;
}
int32 scriptlib::card_clear_effect_relation(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	pcard->relate_effect.clear();
	return 0;
}
int32 scriptlib::card_is_relate_to_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	if(pcard && pcard->is_has_relation(peffect))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_relate_to_card(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* rcard = *(card**) lua_touserdata(L, 2);
	if(pcard && pcard->is_has_relation(rcard))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_relate_to_battle(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	if(pcard->fieldid_r == pduel->game_field->core.pre_field[0] || pcard->fieldid_r == pduel->game_field->core.pre_field[1])
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_copy_effect(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 code = lua_tointeger(L, 2);
	uint32 reset = lua_tointeger(L, 3);
	uint32 count = lua_tointeger(L, 4);
	if(count == 0)
		count = 1;
	if(reset & RESET_PHASE && !(reset & (RESET_SELF_TURN | RESET_OPPO_TURN)))
		reset |= (RESET_SELF_TURN | RESET_OPPO_TURN);
	lua_pushinteger(L, pcard->copy_effect(code, reset, count));
	return 1;
}
int32 scriptlib::card_enable_revive_limit(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(!pcard->is_status(STATUS_COPYING_EFFECT))
		pcard->set_status(STATUS_REVIVE_LIMIT, TRUE);
	return 0;
}
int32 scriptlib::card_complete_procedure(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	pcard->set_status(STATUS_PROC_COMPLETE, TRUE);
	return 0;
}
int32 scriptlib::card_is_disabled(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_status(STATUS_DISABLED));
	return 1;
}
int32 scriptlib::card_is_destructable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	effect* peffect = 0;
	if(lua_gettop(L) > 1) {
		check_param(L, PARAM_TYPE_EFFECT, 2);
		peffect = *(effect**) lua_touserdata(L, 2);
	}
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(peffect)
		lua_pushboolean(L, pcard->is_destructable_by_effect(peffect, pcard->pduel->game_field->core.reason_player));
	else
		lua_pushboolean(L, pcard->is_destructable());
	return 1;
}
int32 scriptlib::card_is_summonable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card * pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_summonable());
	return 1;
}
int32 scriptlib::card_is_msetable(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	uint32 ign = lua_toboolean(L, 2);
	effect* peffect = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_EFFECT, 3);
		peffect = *(effect**)lua_touserdata(L, 3);
	}
	lua_pushboolean(L, pcard->is_setable_mzone(p, ign, peffect));
	return 1;
}
int32 scriptlib::card_is_ssetable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	uint32 ign = FALSE;
	if(lua_gettop(L) >= 2)
		ign = lua_toboolean(L, 2);
	lua_pushboolean(L, pcard->is_setable_szone(p, ign));
	return 1;
}
int32 scriptlib::card_is_special_summonable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	lua_pushboolean(L, pcard->is_special_summonable(p));
	return 1;
}
int32 scriptlib::card_is_synchro_summonable(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(!(pcard->data.type & TYPE_SYNCHRO))
		return 0;
	card* tuner = 0;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_CARD, 2);
		tuner = *(card**) lua_touserdata(L, 2);
	}
	uint32 p = pcard->pduel->game_field->core.reason_player;
	pcard->pduel->game_field->core.limit_tuner = tuner;
	lua_pushboolean(L, pcard->is_special_summonable(p));
	return 1;
}
int32 scriptlib::card_is_xyz_summonable(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(!(pcard->data.type & TYPE_XYZ))
		return 0;
	group* materials = 0;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_GROUP, 2);
		materials = *(group**) lua_touserdata(L, 2);
	}
	uint32 p = pcard->pduel->game_field->core.reason_player;
	pcard->pduel->game_field->core.limit_xyz = materials;
	lua_pushboolean(L, pcard->is_special_summonable(p));
	return 1;
}
int32 scriptlib::card_is_can_be_summoned(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	uint32 ign = lua_toboolean(L, 2);
	effect* peffect = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_EFFECT, 3);
		peffect = *(effect**)lua_touserdata(L, 3);
	}
	lua_pushboolean(L, pcard->is_can_be_summoned(p, ign, peffect));
	return 1;
}
int32 scriptlib::card_is_can_be_special_summoned(lua_State *L) {
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	uint32 sumtype = lua_tointeger(L, 3);
	uint32 sumplayer = lua_tointeger(L, 4);
	uint32 nocheck = lua_toboolean(L, 5);
	uint32 nolimit = lua_toboolean(L, 6);
	uint32 sumpos = POS_FACEUP;
	uint32 toplayer = sumplayer;
	if(lua_gettop(L) > 6)
		sumpos = lua_tointeger(L, 7);
	if(lua_gettop(L) > 7)
		toplayer = lua_tointeger(L, 8);
	if(pcard->is_can_be_special_summoned(peffect, sumtype, sumpos, sumplayer, toplayer, nocheck, nolimit))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_hand(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_send_to_hand(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_grave(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_send_to_grave(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_deck(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_send_to_deck(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_extra(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_send_to_extra(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_remove(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(lua_gettop(L) >= 2)
		p = lua_tointeger(L, 2);
	if(pcard->is_removeable(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_hand_as_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_cost_to_hand(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_grave_as_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_cost_to_grave(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_deck_as_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_cost_to_deck(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_extra_as_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_capable_cost_to_extra(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_remove_as_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_removeable_as_cost(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_releasable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	if(pcard->is_releasable_by_nonsummon(p))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_releasable_by_effect(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	effect* re = pcard->pduel->game_field->core.reason_effect;
	if(pcard->is_releasable_by_effect(p, re))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_discardable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 p = pcard->pduel->game_field->core.reason_player;
	effect* pe = pcard->pduel->game_field->core.reason_effect;
	uint32 reason = REASON_COST;
	if(lua_gettop(L) > 1)
		reason = lua_tointeger(L, 2);
	if((reason != REASON_COST || !pcard->is_affected_by_effect(EFFECT_CANNOT_USE_AS_COST))
	        && pcard->pduel->game_field->is_player_can_discard_hand(p, pcard, pe, reason))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_attackable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_capable_attack());
	return 1;
}
int32 scriptlib::card_is_chain_attackable(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	int32 monsteronly = FALSE;
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	int32 ac = 2;
	if(lua_gettop(L) > 1)
		ac = lua_tointeger(L, 2);
	if(lua_gettop(L) > 2)
		monsteronly = lua_toboolean(L, 3);
	card* attacker = pduel->game_field->core.attacker;
	if(pduel->game_field->core.effect_damage_step
	        || attacker->is_status(STATUS_BATTLE_DESTROYED)
			|| attacker->current.controler != pduel->game_field->infos.turn_player
	        || attacker->fieldid_r != pduel->game_field->core.pre_field[0]
	        || !attacker->is_capable_attack_announce(pduel->game_field->infos.turn_player)
	        || attacker->announce_count >= ac) {
		lua_pushboolean(L, 0);
		return 1;
	}
	pduel->game_field->core.select_cards.clear();
	pduel->game_field->get_attack_target(attacker, &pduel->game_field->core.select_cards, TRUE);
	if(pduel->game_field->core.select_cards.size() == 0 && (monsteronly || attacker->operation_param == 0))
		lua_pushboolean(L, 0);
	else
		lua_pushboolean(L, 1);
	return 1;
}
int32 scriptlib::card_is_faceup(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_position(POS_FACEUP));
	return 1;
}
int32 scriptlib::card_is_attack_pos(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_position(POS_ATTACK));
	return 1;
}
int32 scriptlib::card_is_facedown(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_position(POS_FACEDOWN));
	return 1;
}
int32 scriptlib::card_is_defence_pos(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_position(POS_DEFENCE));
	return 1;
}
int32 scriptlib::card_is_position(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 pos = lua_tointeger(L, 2);
	lua_pushboolean(L, pcard->is_position(pos));
	return 1;
}
int32 scriptlib::card_is_pre_position(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 pos = lua_tointeger(L, 2);
	lua_pushboolean(L, pcard->previous.position & pos);
	return 1;
}
int32 scriptlib::card_is_controler(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 con = lua_tointeger(L, 2);
	if(pcard->current.controler == con)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_onfield(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if((pcard->current.location & LOCATION_ONFIELD) && !pcard->is_status(STATUS_SUMMONING) && !pcard->is_status(STATUS_SUMMON_DISABLED))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_location(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 loc = lua_tointeger(L, 2);
	if(pcard->current.location == LOCATION_MZONE) {
		if((loc & LOCATION_MZONE) && !pcard->is_status(STATUS_SUMMONING) && !pcard->is_status(STATUS_SUMMON_DISABLED))
			lua_pushboolean(L, 1);
		else
			lua_pushboolean(L, 0);
	} else
		lua_pushboolean(L, pcard->current.location & loc);
	return 1;
}
int32 scriptlib::card_is_pre_location(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 loc = lua_tointeger(L, 2);
	lua_pushboolean(L, pcard->previous.location & loc);
	return 1;
}
int32 scriptlib::card_is_level_below(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 lvl = lua_tointeger(L, 2);
	if((pcard->data.type & TYPE_XYZ) || (!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE)))
		lua_pushboolean(L, 0);
	else
		lua_pushboolean(L, pcard->get_level() <= lvl);
	return 1;
}
int32 scriptlib::card_is_level_above(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 lvl = lua_tointeger(L, 2);
	if((pcard->data.type & TYPE_XYZ) || (!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE)))
		lua_pushboolean(L, 0);
	else
		lua_pushboolean(L, pcard->get_level() >= lvl);
	return 1;
}
int32 scriptlib::card_is_rank_below(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 rnk = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_XYZ))
		lua_pushboolean(L, 0);
	else
		lua_pushboolean(L, pcard->get_rank() <= rnk);
	return 1;
}
int32 scriptlib::card_is_rank_above(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 rnk = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_XYZ))
		lua_pushboolean(L, 0);
	else
		lua_pushboolean(L, pcard->get_rank() >= rnk);
	return 1;
}
int32 scriptlib::card_is_attack_below(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 atk = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE))
		lua_pushboolean(L, 0);
	else {
		int _atk = pcard->get_attack();
		lua_pushboolean(L, _atk >= 0 && _atk <= atk);
	}
	return 1;
}
int32 scriptlib::card_is_attack_above(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 atk = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE))
		lua_pushboolean(L, 0);
	else {
		int _atk = pcard->get_attack();
		lua_pushboolean(L, _atk >= atk);
	}
	return 1;
}
int32 scriptlib::card_is_defence_below(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 def = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE))
		lua_pushboolean(L, 0);
	else {
		int _def = pcard->get_defence();
		lua_pushboolean(L, _def >= 0 && _def <= def);
	}
	return 1;
}
int32 scriptlib::card_is_defence_above(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 def = lua_tointeger(L, 2);
	if(!(pcard->data.type & TYPE_MONSTER) && !(pcard->current.location & LOCATION_MZONE))
		lua_pushboolean(L, 0);
	else {
		int _def = pcard->get_defence();
		lua_pushboolean(L, _def >= def);
	}
	return 1;
}
int32 scriptlib::card_is_public(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->is_status(STATUS_IS_PUBLIC))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_forbidden(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->is_affected_by_effect(EFFECT_FORBIDDEN))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_able_to_change_controler(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->is_capable_change_control())
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_is_controler_can_be_changed(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->is_control_can_be_changed())
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_add_counter(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 countertype = lua_tointeger(L, 2);
	uint32 count = lua_tointeger(L, 3);
	if(pcard->is_affect_by_effect(pcard->pduel->game_field->core.reason_effect))
		lua_pushboolean(L, pcard->add_counter(countertype, count));
	else lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::card_remove_counter(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_CARD, 1);
	card * pcard = *(card**) lua_touserdata(L, 1);
	uint32 rplayer = lua_tointeger(L, 2);
	uint32 countertype = lua_tointeger(L, 3);
	uint32 count = lua_tointeger(L, 4);
	uint32 reason = lua_tointeger(L, 5);
	if(countertype == 0) {
		for(auto cmit = pcard->counters.begin(); cmit != pcard->counters.end(); ++cmit) {
			pcard->pduel->write_buffer8(MSG_REMOVE_COUNTER);
			pcard->pduel->write_buffer16(cmit->first);
			pcard->pduel->write_buffer8(pcard->current.controler);
			pcard->pduel->write_buffer8(pcard->current.location);
			pcard->pduel->write_buffer8(pcard->current.sequence);
			pcard->pduel->write_buffer8(cmit->second);
		}
		pcard->counters.clear();
		return 0;
	} else {
		pcard->pduel->game_field->remove_counter(reason, pcard, rplayer, 0, 0, countertype, count);
		return lua_yield(L, 0);
	}
}
int32 scriptlib::card_get_counter(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 countertype = lua_tointeger(L, 2);
	if(countertype == 0)
		lua_pushinteger(L, pcard->counters.size());
	else
		lua_pushinteger(L, pcard->get_counter(countertype));
	return 1;
}
int32 scriptlib::card_enable_counter_permit(lua_State *L) {
	check_param_count(L, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 countertype = lua_tointeger(L, 2);
	effect* peffect = pcard->pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_COUNTER_PERMIT | countertype;
	peffect->flag = EFFECT_FLAG_SINGLE_RANGE;
	if(pcard->data.type & TYPE_MONSTER)
		peffect->range = LOCATION_MZONE;
	else
		peffect->range = LOCATION_SZONE;
	pcard->add_effect(peffect);
	return 0;
}
int32 scriptlib::card_set_counter_limit(lua_State *L) {
	check_param_count(L, 3);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 countertype = lua_tointeger(L, 2);
	int32 limit = lua_tointeger(L, 3);
	effect* peffect = pcard->pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_COUNTER_LIMIT | countertype;
	peffect->value = limit;
	pcard->add_effect(peffect);
	return 0;
}
int32 scriptlib::card_is_can_turn_set(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	lua_pushboolean(L, pcard->is_capable_turn_set(pcard->pduel->game_field->core.reason_player));
	return 1;
}
int32 scriptlib::card_is_can_add_counter(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 countertype = lua_tointeger(L, 2);
	uint32 count = lua_tointeger(L, 3);
	lua_pushboolean(L, pcard->is_can_add_counter(countertype, count));
	return 1;
}
int32 scriptlib::card_is_can_remove_counter(lua_State *L) {
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 countertype = lua_tointeger(L, 3);
	uint32 count = lua_tointeger(L, 4);
	uint32 reason = lua_tointeger(L, 5);
	lua_pushboolean(L, pcard->pduel->game_field->is_player_can_remove_counter(playerid, pcard, 0, 0, countertype, count, reason));
	return 1;
}
int32 scriptlib::card_is_can_be_fusion_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 ign = FALSE;
	if(lua_gettop(L) >= 2)
		ign = lua_toboolean(L, 2);
	lua_pushboolean(L, pcard->is_can_be_fusion_material(ign));
	return 1;
}
int32 scriptlib::card_is_can_be_synchro_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* scard = 0;
	card* tuner = 0;
	if(lua_gettop(L) >= 2) {
		check_param(L, PARAM_TYPE_CARD, 2);
		scard = *(card**) lua_touserdata(L, 2);
	}
	if(lua_gettop(L) >= 3) {
		check_param(L, PARAM_TYPE_CARD, 3);
		tuner = *(card**) lua_touserdata(L, 3);
	}
	lua_pushboolean(L, pcard->is_can_be_synchro_material(scard, tuner));
	return 1;
}
int32 scriptlib::card_is_can_be_xyz_material(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* scard = 0;
	if(lua_gettop(L) >= 2) {
		check_param(L, PARAM_TYPE_CARD, 2);
		scard = *(card**) lua_touserdata(L, 2);
	}
	lua_pushboolean(L, pcard->is_can_be_xyz_material(scard));
	return 1;
}
int32 scriptlib::card_check_fusion_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 chkf = PLAYER_NONE;
	group* pgroup = 0;
	if(lua_gettop(L) > 1 && !lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_GROUP, 2);
		pgroup = *(group**) lua_touserdata(L, 2);
	}
	card* cg = 0;
	if(lua_gettop(L) > 2 && !lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_CARD, 3);
		cg = *(card**) lua_touserdata(L, 3);
	}
	if(lua_gettop(L) > 3)
		chkf = lua_tointeger(L, 4);
	lua_pushboolean(L, pcard->fusion_check(pgroup, cg, chkf));
	return 1;
}
int32 scriptlib::card_is_immune_to_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 2);
	lua_pushboolean(L, !pcard->is_affect_by_effect(peffect));
	return 1;
}
int32 scriptlib::card_is_can_be_effect_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	effect* peffect = pduel->game_field->core.reason_effect;
	if(lua_gettop(L) > 1) {
		check_param(L, PARAM_TYPE_EFFECT, 2);
		peffect = *(effect**) lua_touserdata(L, 2);
	}
	lua_pushboolean(L, pcard->is_capable_be_effect_target(peffect, pduel->game_field->core.reason_player));
	return 1;
}
int32 scriptlib::card_is_can_be_battle_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* bcard = *(card**) lua_touserdata(L, 2);
	lua_pushboolean(L, pcard->is_capable_be_battle_target(bcard));
	return 1;
}
int32 scriptlib::card_add_trap_monster_attribute(lua_State *L) {
	check_param_count(L, 7);
	check_param(L, PARAM_TYPE_CARD, 1);
	int32 extra_type = lua_tointeger(L, 2);
	int32 attribute = lua_tointeger(L, 3);
	int32 race = lua_tointeger(L, 4);
	int32 level = lua_tointeger(L, 5);
	int32 atk = lua_tointeger(L, 6);
	int32 def = lua_tointeger(L, 7);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	//type
	effect* peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_ADD_TYPE;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = TYPE_MONSTER | TYPE_TRAPMONSTER | extra_type;
	pcard->add_effect(peffect);
	//attribute
	peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_CHANGE_ATTRIBUTE;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = attribute;
	pcard->add_effect(peffect);
	//race
	peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_CHANGE_RACE;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = race;
	pcard->add_effect(peffect);
	//level
	peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_CHANGE_LEVEL;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = level;
	pcard->add_effect(peffect);
	//atk
	peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_SET_BASE_ATTACK;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = atk;
	pcard->add_effect(peffect);
	//def
	peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_SET_BASE_DEFENCE;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x47e0000;
	peffect->value = def;
	pcard->add_effect(peffect);
	return 0;
}
int32 scriptlib::card_trap_monster_block(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	//extra block
	effect* peffect = pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_FIELD;
	peffect->range = LOCATION_MZONE;
	peffect->code = EFFECT_USE_EXTRA_SZONE;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE;
	peffect->reset_flag = RESET_EVENT + 0x5fe0000;
	peffect->value = 1 + (0x10000 << pcard->previous.sequence);
	pcard->add_effect(peffect);
	return 0;
}
int32 scriptlib::card_cancel_to_grave(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	bool cancel = true;
	if(lua_gettop(L) > 1)
		cancel = lua_toboolean(L, 2) != 0;
	if(cancel)
		pcard->set_status(STATUS_LEAVE_CONFIRMED, FALSE);
	else {
		pcard->pduel->game_field->core.leave_confirmed.insert(pcard);
		pcard->set_status(STATUS_LEAVE_CONFIRMED, TRUE);
	}
	return 0;
}
int32 scriptlib::card_get_tribute_requirement(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 rcount = pcard->get_summon_tribute_count();
	lua_pushinteger(L, rcount & 0xffff);
	lua_pushinteger(L, (rcount >> 16) & 0xffff);
	return 2;
}
int32 scriptlib::card_get_battle_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	if(pduel->game_field->core.attacker == pcard)
		interpreter::card2value(L, pduel->game_field->core.attack_target);
	else if(pduel->game_field->core.attack_target == pcard)
		interpreter::card2value(L, pduel->game_field->core.attacker);
	else lua_pushnil(L);
	return 1;
}
int32 scriptlib::card_get_attackable_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	field::card_vector targets;
	pduel->game_field->get_attack_target(pcard, &targets);
	group* newgroup = pduel->new_group();
	newgroup->container.insert(targets.begin(), targets.end());
	interpreter::group2value(L, newgroup);
	lua_pushboolean(L, pcard->operation_param);
	return 2;
}
int32 scriptlib::card_set_hint(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	uint32 type = lua_tointeger(L, 2);
	uint32 value = lua_tointeger(L, 3);
	if(type >= CHINT_DESC_ADD)
		return 0;
	pduel->write_buffer8(MSG_CARD_HINT);
	pduel->write_buffer32(pcard->get_info_location());
	pduel->write_buffer8(type);
	pduel->write_buffer32(value);
	return 0;
}
int32 scriptlib::card_reverse_in_deck(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(pcard->current.location != LOCATION_DECK)
		return 0;
	pcard->current.position = POS_FACEUP_DEFENCE;
	duel* pduel = pcard->pduel;
	if(pcard->current.sequence == pduel->game_field->player[pcard->current.controler].list_main.size() - 1) {
		pduel->write_buffer8(MSG_DECK_TOP);
		pduel->write_buffer8(pcard->current.controler);
		pduel->write_buffer8(0);
		pduel->write_buffer32(pcard->data.code | 0x80000000);
	}
	return 0;
}
int32 scriptlib::card_set_unique_onfield(lua_State *L) {
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	pcard->unique_pos[0] = lua_tointeger(L, 2);
	pcard->unique_pos[1] = lua_tointeger(L, 3);
	pcard->unique_code = lua_tointeger(L, 4);
	effect* peffect = pcard->pduel->new_effect();
	peffect->owner = pcard;
	peffect->type = EFFECT_TYPE_SINGLE;
	peffect->code = EFFECT_UNIQUE_CHECK;
	peffect->flag = EFFECT_FLAG_COPY_INHERIT;
	pcard->add_effect(peffect);
	pcard->unique_effect = peffect;
	if(pcard->current.location & LOCATION_ONFIELD)
		pcard->pduel->game_field->add_unique_card(pcard);
	return 0;
}
int32 scriptlib::card_check_unique_onfield(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 check_player = lua_tointeger(L, 2);
	lua_pushboolean(L, pcard->pduel->game_field->check_unique_onfield(pcard, check_player) ? 0 : 1);
	return 1;
}
int32 scriptlib::card_reset_negate_effect(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 count = lua_gettop(L) - 1;
	for(int32 i = 0; i < count; ++i)
		pcard->reset(lua_tointeger(L, i + 2), RESET_CARD);
	return 0;
}
int32 scriptlib::card_assume_prop(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	pcard->assume_type = lua_tointeger(L, 2);
	pcard->assume_value = lua_tointeger(L, 3);
	pcard->pduel->assumes.insert(pcard);
	return 0;
}
