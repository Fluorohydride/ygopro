/*
 * libduel.cpp
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
#include "ocgapi.h"

int32 scriptlib::duel_enable_global_flag(lua_State *L) {
	check_param_count(L, 1);
	int32 flag = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.global_flag |= flag;
	return 0;
}

int32 scriptlib::duel_get_lp(lua_State *L) {
	check_param_count(L, 1);
	int32 p = lua_tointeger(L, 1);
	if(p != 0 && p != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->player[p].lp);
	return 1;
}
int32 scriptlib::duel_set_lp(lua_State *L) {
	check_param_count(L, 2);
	int32 p = lua_tointeger(L, 1);
	int32 lp = lua_tointeger(L, 2);
	if(lp < 0) lp = 0;
	if(p != 0 && p != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->player[p].lp = lp;
	pduel->write_buffer8(MSG_LPUPDATE);
	pduel->write_buffer8(p);
	pduel->write_buffer32(lp);
	return 0;
}
int32 scriptlib::duel_get_turn_player(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->infos.turn_player);
	return 1;
}
int32 scriptlib::duel_get_turn_count(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->infos.turn_id);
	return 1;
}
int32 scriptlib::duel_get_draw_count(lua_State *L) {
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	lua_pushinteger(L, pduel->game_field->get_draw_count(playerid));
	return 1;
}
int32 scriptlib::duel_register_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**)lua_touserdata(L, 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = peffect->pduel;
	pduel->game_field->add_effect(peffect, playerid);
	return 0;
}
int32 scriptlib::duel_register_flag_effect(lua_State *L) {
	check_param_count(L, 5);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	int32 reset = lua_tointeger(L, 3);
	int32 flag = lua_tointeger(L, 4);
	int32 count = lua_tointeger(L, 5);
	if(count == 0)
		count = 1;
	if(reset & (RESET_PHASE) && !(reset & (RESET_SELF_TURN | RESET_OPPO_TURN)))
		reset |= (RESET_SELF_TURN | RESET_OPPO_TURN);
	duel* pduel = interpreter::get_duel_info(L);
	effect* peffect = pduel->new_effect();
	peffect->effect_owner = playerid;
	peffect->owner = pduel->game_field->temp_card;
	peffect->handler = 0;
	peffect->type = EFFECT_TYPE_FIELD;
	peffect->code = code;
	peffect->reset_flag = reset;
	peffect->flag = flag | EFFECT_FLAG_CANNOT_DISABLE | EFFECT_FLAG_PLAYER_TARGET | EFFECT_FLAG_FIELD_ONLY;
	peffect->s_range = 1;
	peffect->o_range = 0;
	peffect->reset_count |= count & 0xff;
	pduel->game_field->add_effect(peffect, playerid);
	interpreter::effect2value(L, peffect);
	return 1;
}
int32 scriptlib::duel_get_flag_effect(lua_State *L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	duel* pduel = interpreter::get_duel_info(L);
	effect_set eset;
	pduel->game_field->filter_player_effect(playerid, code, &eset);
	lua_pushinteger(L, eset.size());
	return 1;
}
int32 scriptlib::duel_reset_flag_effect(lua_State *L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 code = (lua_tointeger(L, 2) & 0xfffffff) | 0x10000000;
	duel* pduel = interpreter::get_duel_info(L);
	std::pair<field::effect_container::iterator, field::effect_container::iterator> pr;
	effect* peffect;
	pr = pduel->game_field->effects.aura_effect.equal_range(code);
	for(; pr.first != pr.second; ) {
		auto rm = pr.first++;
		peffect = rm->second;
		if(peffect->code == code)
			pduel->game_field->remove_effect(peffect);
	}
	return 0;
}
int32 scriptlib::duel_destroy(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 reason = lua_tointeger(L, 2);
	uint32 dest = LOCATION_GRAVE;
	if(lua_gettop(L) >= 3)
		dest = lua_tointeger(L, 3);
	if(pcard)
		pduel->game_field->destroy(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, dest, 0);
	else
		pduel->game_field->destroy(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, dest, 0);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_DESTROY_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_remove(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 pos = lua_tointeger(L, 2);
	uint32 reason = lua_tointeger(L, 3);
	if(pcard)
		pduel->game_field->send_to(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, LOCATION_REMOVED, 0, pos);
	else
		pduel->game_field->send_to(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, LOCATION_REMOVED, 0, pos);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SENDTO_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_sendto_grave(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 reason = lua_tointeger(L, 2);
	if(pcard)
		pduel->game_field->send_to(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
	else
		pduel->game_field->send_to(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SENDTO_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_summon(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 2);
	effect* peffect = 0;
	if(!lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_EFFECT, 4);
		peffect = *(effect**)lua_touserdata(L, 4);
	}
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**)lua_touserdata(L, 2);
	uint32 ignore_count = lua_toboolean(L, 3);
	uint32 min_tribute = 0;
	if(lua_gettop(L) > 4)
		min_tribute = lua_tointeger(L, 5);
	duel * pduel = pcard->pduel;
	pduel->game_field->summon(playerid, pcard, peffect, ignore_count, min_tribute);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_special_summon_rule(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**)lua_touserdata(L, 2);
	duel * pduel = pcard->pduel;
	pduel->game_field->special_summon_rule(playerid, pcard, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_synchro_summon(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**)lua_touserdata(L, 2);
	card* tuner = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_CARD, 3);
		tuner = *(card**)lua_touserdata(L, 3);
	}
	group* mg = 0;
	if(lua_gettop(L) >= 4) {
		if(!lua_isnil(L, 4)) {
			check_param(L, PARAM_TYPE_GROUP, 4);
			mg = *(group**) lua_touserdata(L, 4);
		}
	}
	duel * pduel = pcard->pduel;
	pduel->game_field->core.limit_tuner = tuner;
	pduel->game_field->core.limit_syn = mg;
	pduel->game_field->special_summon_rule(playerid, pcard, SUMMON_TYPE_SYNCHRO);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_xyz_summon(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**)lua_touserdata(L, 2);
	group* materials = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_GROUP, 3);
		materials = *(group**)lua_touserdata(L, 3);
	}
	duel * pduel = pcard->pduel;
	pduel->game_field->core.limit_xyz = materials;
	pduel->game_field->special_summon_rule(playerid, pcard, SUMMON_TYPE_XYZ);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_setm(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 2);
	effect* peffect = 0;
	if(!lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_EFFECT, 4);
		peffect = *(effect**)lua_touserdata(L, 4);
	}
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**)lua_touserdata(L, 2);
	uint32 ignore_count = lua_toboolean(L, 3);
	uint32 min_tribute = 0;
	if(lua_gettop(L) > 4)
		min_tribute = lua_tointeger(L, 5);
	duel * pduel = pcard->pduel;
	pduel->game_field->add_process(PROCESSOR_MSET, 0, peffect, (group*)pcard, playerid, ignore_count + (min_tribute << 8));
	return lua_yield(L, 0);
}
int32 scriptlib::duel_sets(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 toplayer = playerid;
	if(lua_gettop(L) > 2)
		toplayer = lua_tointeger(L, 3);
	if(toplayer != 0 && toplayer != 1)
		toplayer = playerid;
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 2, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 2);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 2, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 2);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 2);
	if(pcard)
		pduel->game_field->add_process(PROCESSOR_SSET, 0, 0, (group*)pcard, playerid, toplayer);
	else
		pduel->game_field->add_process(PROCESSOR_SSET_G, 0, 0, pgroup, playerid, toplayer);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_create_token(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	int32 code = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	card* pcard = pduel->new_card(code);
	pcard->owner = playerid;
	pcard->current.location = 0;
	pcard->current.controler = playerid;
	interpreter::card2value(L, pcard);
	return 1;
}
int32 scriptlib::duel_special_summon(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 7);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 sumtype = lua_tointeger(L, 2);
	uint32 sumplayer = lua_tointeger(L, 3);
	uint32 playerid = lua_tointeger(L, 4);
	uint32 nocheck = lua_toboolean(L, 5);
	uint32 nolimit = lua_toboolean(L, 6);
	uint32 positions = lua_tointeger(L, 7);
	if(pcard) {
		field::card_set cset;
		cset.insert(pcard);
		pduel->game_field->special_summon(&cset, sumtype, sumplayer, playerid, nocheck, nolimit, positions);
	} else
		pduel->game_field->special_summon(&(pgroup->container), sumtype, sumplayer, playerid, nocheck, nolimit, positions);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SPSUMMON_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_special_summon_step(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 7);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	uint32 sumtype = lua_tointeger(L, 2);
	uint32 sumplayer = lua_tointeger(L, 3);
	uint32 playerid = lua_tointeger(L, 4);
	uint32 nocheck = lua_toboolean(L, 5);
	uint32 nolimit = lua_toboolean(L, 6);
	uint32 positions = lua_tointeger(L, 7);
	pduel->game_field->special_summon_step(pcard, sumtype, sumplayer, playerid, nocheck, nolimit, positions);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SPSUMMON_STEP_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_special_summon_complete(lua_State *L) {
	check_action_permission(L);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->special_summon_complete(pduel->game_field->core.reason_effect, pduel->game_field->core.reason_player);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_sendto_hand(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(lua_isnil(L, 2) || (playerid != 0 && playerid != 1))
		playerid = PLAYER_NONE;
	uint32 reason = lua_tointeger(L, 3);
	if(pcard)
		pduel->game_field->send_to(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, LOCATION_HAND, 0, POS_FACEUP);
	else
		pduel->game_field->send_to(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, LOCATION_HAND, 0, POS_FACEUP);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SENDTO_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_sendto_deck(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 4);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(lua_isnil(L, 2) || (playerid != 0 && playerid != 1))
		playerid = PLAYER_NONE;
	uint32 sequence = lua_tointeger(L, 3);
	uint32 reason = lua_tointeger(L, 4);
	if(pcard)
		pduel->game_field->send_to(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, LOCATION_DECK, sequence, POS_FACEUP);
	else
		pduel->game_field->send_to(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, LOCATION_DECK, sequence, POS_FACEUP);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SENDTO_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_get_operated_group(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group(pduel->game_field->core.operated_set);
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::duel_remove_counter(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	uint32 rplayer = lua_tointeger(L, 1);
	if(rplayer != 0 && rplayer != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	uint32 countertype = lua_tointeger(L, 4);
	uint32 count = lua_tointeger(L, 5);
	uint32 reason = lua_tointeger(L, 6);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->remove_counter(reason, 0, rplayer, s, o, countertype, count);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_is_can_remove_counter(lua_State *L) {
	check_param_count(L, 6);
	uint32 rplayer = lua_tointeger(L, 1);
	if(rplayer != 0 && rplayer != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	uint32 countertype = lua_tointeger(L, 4);
	uint32 count = lua_tointeger(L, 5);
	uint32 reason = lua_tointeger(L, 6);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_remove_counter(rplayer, 0, s, o, countertype, count, reason));
	return 1;
}
int32 scriptlib::duel_get_counter(lua_State *L) {
	check_param_count(L, 4);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	uint32 countertype = lua_tointeger(L, 4);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->get_field_counter(playerid, s, o, countertype));
	return 1;
}
int32 scriptlib::duel_change_form(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 au = lua_tointeger(L, 2);
	uint32 ad = au, du = au, dd = au, noflip = 0;
	uint32 top = lua_gettop(L);
	if(top > 2) ad = lua_tointeger(L, 3);
	if(top > 3) du = lua_tointeger(L, 4);
	if(top > 4) dd = lua_tointeger(L, 5);
	if(top > 5) noflip = lua_toboolean(L, 6);
	if(pcard) {
		field::card_set cset;
		cset.insert(pcard);
		pduel->game_field->change_position(&cset, pduel->game_field->core.reason_effect, pduel->game_field->core.reason_player, au, ad, du, dd, noflip, TRUE);
	} else
		pduel->game_field->change_position(&(pgroup->container), pduel->game_field->core.reason_effect, pduel->game_field->core.reason_player, au, ad, du, dd, noflip, TRUE);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_CHANGEPOS_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_release(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	uint32 reason = lua_tointeger(L, 2);
	if(pcard)
		pduel->game_field->release(pcard, pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player);
	else
		pduel->game_field->release(&(pgroup->container), pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_RELEASE_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_move_to_field(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 move_player = lua_tointeger(L, 2);
	uint32 playerid = lua_tointeger(L, 3);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 destination = lua_tointeger(L, 4);
	uint32 positions = lua_tointeger(L, 5);
	uint32 enable = lua_toboolean(L, 6);
	duel* pduel = pcard->pduel;
	pcard->enable_field_effect(FALSE);
	pduel->game_field->adjust_instant();
	pduel->game_field->move_to_field(pcard, move_player, playerid, destination, positions, enable);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_MOVETOFIELD_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_return_to_field(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	if(!(pcard->current.reason & REASON_TEMPORARY))
		return 0;
	int32 pos = pcard->previous.position;
	if(lua_gettop(L) > 1)
		pos = lua_tointeger(L, 2);
	duel* pduel = pcard->pduel;
	pcard->enable_field_effect(FALSE);
	pduel->game_field->adjust_instant();
	pduel->game_field->refresh_location_info_instant();
	pduel->game_field->move_to_field(pcard, pcard->previous.controler, pcard->previous.controler, pcard->previous.location, pos, TRUE, 1);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_MOVETOFIELD_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_move_sequence(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	int32 seq = lua_tointeger(L, 2);
	duel* pduel = pcard->pduel;
	pduel->game_field->move_card(pcard->current.controler, pcard, pcard->current.location, seq);
	return 0;
}
int32 scriptlib::duel_set_chain_limit(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 1);
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.chain_limit)
		luaL_unref(L, LUA_REGISTRYINDEX, pduel->game_field->core.chain_limit);
	int32 f = interpreter::get_function_handle(L, 1);
	pduel->game_field->core.chain_limit = f;
	pduel->game_field->core.chain_limp = pduel->game_field->core.reason_player;
	return 0;
}
int32 scriptlib::duel_set_chain_limit_p(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 1);
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.chain_limit_p)
		luaL_unref(L, LUA_REGISTRYINDEX, pduel->game_field->core.chain_limit_p);
	int32 f = interpreter::get_function_handle(L, 1);
	pduel->game_field->core.chain_limit_p = f;
	pduel->game_field->core.chain_limp_p = pduel->game_field->core.reason_player;
	return 0;
}
int32 scriptlib::duel_get_chain_material(lua_State *L) {
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	effect_set eset;
	pduel->game_field->filter_player_effect(playerid, EFFECT_CHAIN_MATERIAL, &eset);
	if(!eset.size())
		return 0;
	interpreter::effect2value(L, eset[0]);
	return 1;
}
int32 scriptlib::duel_confirm_decktop(lua_State *L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 count = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	if(count >= pduel->game_field->player[playerid].list_main.size())
		count = pduel->game_field->player[playerid].list_main.size();
	else if(pduel->game_field->player[playerid].list_main.size() > count) {
		if(pduel->game_field->core.global_flag & GLOBALFLAG_DECK_REVERSE_CHECK) {
			card* pcard = *(pduel->game_field->player[playerid].list_main.rbegin() + count);
			if(pduel->game_field->core.deck_reversed) {
				pduel->write_buffer8(MSG_DECK_TOP);
				pduel->write_buffer8(playerid);
				pduel->write_buffer8(count);
				if(pcard->current.position != POS_FACEUP_DEFENCE)
					pduel->write_buffer32(pcard->data.code);
				else
					pduel->write_buffer32(pcard->data.code | 0x80000000);
			}
		}
	}
	auto cit = pduel->game_field->player[playerid].list_main.rbegin();
	pduel->write_buffer8(MSG_CONFIRM_DECKTOP);
	pduel->write_buffer8(playerid);
	pduel->write_buffer8(count);
	for(uint32 i = 0; i < count && cit != pduel->game_field->player[playerid].list_main.rend(); ++i, ++cit) {
		pduel->write_buffer32((*cit)->data.code);
		pduel->write_buffer8((*cit)->current.controler);
		pduel->write_buffer8((*cit)->current.location);
		pduel->write_buffer8((*cit)->current.sequence);
	}
	pduel->game_field->add_process(PROCESSOR_WAIT, 0, 0, 0, 0, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_confirm_cards(lua_State *L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 2, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 2);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 2, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 2);
		if(pgroup->container.size() == 0)
			return 0;
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 2);
	pduel->write_buffer8(MSG_CONFIRM_CARDS);
	pduel->write_buffer8(playerid);
	if(pcard) {
		pduel->write_buffer8(1);
		pduel->write_buffer32(pcard->data.code);
		pduel->write_buffer8(pcard->current.controler);
		pduel->write_buffer8(pcard->current.location);
		pduel->write_buffer8(pcard->current.sequence);
	} else {
		pduel->write_buffer8(pgroup->container.size());
		for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
			pduel->write_buffer32((*cit)->data.code);
			pduel->write_buffer8((*cit)->current.controler);
			pduel->write_buffer8((*cit)->current.location);
			pduel->write_buffer8((*cit)->current.sequence);
		}
	}
	pduel->game_field->add_process(PROCESSOR_WAIT, 0, 0, 0, 0, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_sort_decktop(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	uint32 sort_player = lua_tointeger(L, 1);
	uint32 target_player = lua_tointeger(L, 2);
	uint32 count = lua_tointeger(L, 3);
	if(sort_player != 0 && sort_player != 1)
		return 0;
	if(target_player != 0 && target_player != 1)
		return 0;
	if(count < 1 || count > 16)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_SORT_DECK_S, 0, 0, 0, sort_player + (target_player << 16), count);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_check_event(lua_State *L) {
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 ev = lua_tointeger(L, 1);
	int32 get_info = lua_toboolean(L, 2);
	if(!get_info) {
		lua_pushboolean(L, pduel->game_field->check_event(ev));
		return 1;
	} else {
		tevent pe;
		if(pduel->game_field->check_event(ev, &pe)) {
			lua_pushboolean(L, 1);
			interpreter::group2value(L, pe.event_cards);
			lua_pushinteger(L, pe.event_player);
			lua_pushinteger(L, pe.event_value);
			interpreter::effect2value(L, pe.reason_effect);
			lua_pushinteger(L, pe.reason);
			lua_pushinteger(L, pe.reason_player);
			return 7;
		} else {
			lua_pushboolean(L, 0);
			return 1;
		}
	}
}
int32 scriptlib::duel_raise_event(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 7);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		return luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	check_param(L, PARAM_TYPE_EFFECT, 3);
	uint32 code = lua_tointeger(L, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 3);
	uint32 r = lua_tointeger(L, 4);
	uint32 rp = lua_tointeger(L, 5);
	uint32 ep = lua_tointeger(L, 6);
	uint32 ev = lua_tointeger(L, 7);
	if(pcard)
		pduel->game_field->raise_event(pcard, code, peffect, r, rp, ep, ev);
	else
		pduel->game_field->raise_event(&pgroup->container, code, peffect, r, rp, ep, ev);
	pduel->game_field->process_instant_event();
	return lua_yield(L, 0);
}
int32 scriptlib::duel_raise_single_event(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 7);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_EFFECT, 3);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 code = lua_tointeger(L, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 3);
	uint32 r = lua_tointeger(L, 4);
	uint32 rp = lua_tointeger(L, 5);
	uint32 ep = lua_tointeger(L, 6);
	uint32 ev = lua_tointeger(L, 7);
	duel* pduel = pcard->pduel;
	pduel->game_field->raise_single_event(pcard, 0, code, peffect, r, rp, ep, ev);
	pduel->game_field->process_single_event();
	return lua_yield(L, 0);
}
int32 scriptlib::duel_check_timing(lua_State *L) {
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 tm = lua_tointeger(L, 1);
	lua_pushboolean(L, (pduel->game_field->core.hint_timing[0]&tm) || (pduel->game_field->core.hint_timing[1]&tm));
	return 1;
}
int32 scriptlib::duel_get_environment(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	effect_set eset;
	card* pcard = pduel->game_field->player[0].list_szone[5];
	int32 code = 0;
	int32 p = 2;
	if(pcard == 0 || pcard->is_position(POS_FACEDOWN) || !pcard->get_status(STATUS_EFFECT_ENABLED))
		pcard = pduel->game_field->player[1].list_szone[5];
	if(pcard == 0 || pcard->is_position(POS_FACEDOWN) || !pcard->get_status(STATUS_EFFECT_ENABLED)) {
		pduel->game_field->filter_field_effect(EFFECT_CHANGE_ENVIRONMENT, &eset);
		if(eset.size()) {
			effect* peffect = eset.get_last();
			code = peffect->get_value();
			p = peffect->get_handler_player();
		}
	} else {
		code = pcard->get_code();
		p = pcard->current.controler;
	}
	lua_pushinteger(L, code);
	lua_pushinteger(L, p);
	return 2;
}
int32 scriptlib::duel_is_environment(lua_State *L) {
	check_param_count(L, 1);
	uint32 code = lua_tointeger(L, 1);
	uint32 playerid = PLAYER_ALL;
	if(lua_gettop(L) >= 2)
		playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1 && playerid != PLAYER_ALL)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	int32 ret = 0, fc = 0;
	card* pcard = pduel->game_field->player[0].list_szone[5];
	if(pcard && pcard->is_position(POS_FACEUP) && pcard->get_status(STATUS_EFFECT_ENABLED)) {
		fc = 1;
		if(code == pcard->get_code() && (playerid == 0 || playerid == PLAYER_ALL))
			ret = 1;
	}
	pcard = pduel->game_field->player[1].list_szone[5];
	if(pcard && pcard->is_position(POS_FACEUP) && pcard->get_status(STATUS_EFFECT_ENABLED)) {
		fc = 1;
		if(code == pcard->get_code() && (playerid == 1 || playerid == PLAYER_ALL))
			ret = 1;
	}
	if(!fc) {
		effect_set eset;
		pduel->game_field->filter_field_effect(EFFECT_CHANGE_ENVIRONMENT, &eset);
		if(eset.size()) {
			effect* peffect = eset.get_last();
			if(code == (uint32)peffect->get_value() && (playerid == peffect->get_handler_player() || playerid == PLAYER_ALL))
				ret = 1;
		}
	}
	lua_pushboolean(L, ret);
	return 1;
}
int32 scriptlib::duel_win(lua_State *L) {
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 reason = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1 && playerid != 2)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.win_player == 5) {
		pduel->game_field->core.win_player = playerid;
		pduel->game_field->core.win_reason = reason;
	}
	return 0;
}
int32 scriptlib::duel_draw(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 count = lua_tointeger(L, 2);
	uint32 reason = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->draw(pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, count);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_DRAW_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_damage(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 amount = lua_tointeger(L, 2);
	if(amount < 0)
		amount = 0;
	uint32 reason = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->damage(pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, 0, playerid, amount);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_DAMAGE_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_recover(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 amount = lua_tointeger(L, 2);
	if(amount < 0)
		amount = 0;
	uint32 reason = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->recover(pduel->game_field->core.reason_effect, reason, pduel->game_field->core.reason_player, playerid, amount);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_RECOVER_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_equip(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 2);
	check_param(L, PARAM_TYPE_CARD, 3);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* equip_card = *(card**) lua_touserdata(L, 2);
	card* target = *(card**) lua_touserdata(L, 3);
	uint32 up = TRUE;
	if(lua_gettop(L) > 3)
		up = lua_toboolean(L, 4);
	uint32 step = FALSE;
	if(lua_gettop(L) > 4)
		step = lua_toboolean(L, 5);
	duel* pduel = target->pduel;
	pduel->game_field->equip(playerid, equip_card, target, up, step);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_EQUIP_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_equip_complete(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	field::card_set etargets;
	for(auto cit = pduel->game_field->core.equiping_cards.begin(); cit != pduel->game_field->core.equiping_cards.end(); ++cit) {
		card* equip_card = *cit;
		if(equip_card->is_position(POS_FACEUP))
			equip_card->enable_field_effect(TRUE);
		etargets.insert(equip_card->equiping_target);
	}
	pduel->game_field->adjust_instant();
	for(auto cit = etargets.begin(); cit != etargets.end(); ++cit)
		pduel->game_field->raise_single_event(*cit, &pduel->game_field->core.equiping_cards, EVENT_EQUIP,
		                                      pduel->game_field->core.reason_effect, 0, pduel->game_field->core.reason_player, PLAYER_NONE, 0);
	pduel->game_field->raise_event(&pduel->game_field->core.equiping_cards, EVENT_EQUIP,
	                               pduel->game_field->core.reason_effect, 0, pduel->game_field->core.reason_player, PLAYER_NONE, 0);
	pduel->game_field->core.hint_timing[0] |= TIMING_EQUIP;
	pduel->game_field->core.hint_timing[1] |= TIMING_EQUIP;
	pduel->game_field->process_single_event();
	pduel->game_field->process_instant_event();
	return lua_yield(L, 0);
}
int32 scriptlib::duel_get_control(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 reset_phase = 0;
	uint32 reset_count = 0;
	if(lua_gettop(L) > 2) {
		reset_phase = lua_tointeger(L, 3) & 0x3ff;
		reset_count = lua_tointeger(L, 4) & 0xff;
	}
	duel* pduel = target->pduel;
	pduel->game_field->get_control(pduel->game_field->core.reason_effect, pduel->game_field->core.reason_player, target, playerid, reset_phase, reset_count);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_GET_CONTROL_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_swap_control(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard1 = *(card**) lua_touserdata(L, 1);
	card* pcard2 = *(card**) lua_touserdata(L, 2);
	uint32 reset_phase = 0;
	uint32 reset_count = 0;
	if(lua_gettop(L) > 2) {
		reset_phase = lua_tointeger(L, 3) & 0x3ff;
		reset_count = lua_tointeger(L, 4) & 0xff;
	}
	duel* pduel = pcard1->pduel;
	pduel->game_field->swap_control(pduel->game_field->core.reason_effect, pduel->game_field->core.reason_player, pcard1, pcard2, reset_phase, reset_count);
	pduel->game_field->core.subunits.begin()->type = PROCESSOR_SWAP_CONTROL_S;
	return lua_yield(L, 0);
}
int32 scriptlib::duel_check_lp_cost(lua_State *L) {
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 cost = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->check_lp_cost(playerid, cost));
	return 1;
}
int32 scriptlib::duel_pay_lp_cost(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 cost = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_PAY_LPCOST, 0, 0, 0, playerid, cost);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_discard_deck(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 count = lua_tointeger(L, 2);
	uint32 reason = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_DISCARD_DECK_S, 0, 0, 0, playerid + (count << 16), reason);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_discard_hand(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	if(!lua_isnil(L, 2))
		check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(lua_gettop(L) >= 6) {
		if(!lua_isnil(L, 6)) {
			check_param(L, PARAM_TYPE_CARD, 6);
			pexception = *(card**) lua_touserdata(L, 6);
		}
		extraargs = lua_gettop(L) - 6;
	}
	duel* pduel = interpreter::get_duel_info(L);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 min = lua_tointeger(L, 3);
	uint32 max = lua_tointeger(L, 4);
	uint32 reason = lua_tointeger(L, 5);
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_matching_card(2, playerid, LOCATION_HAND, 0, pgroup, pexception, extraargs);
	pduel->game_field->core.select_cards.assign(pgroup->container.begin(), pgroup->container.end());
	if(pduel->game_field->core.select_cards.size() == 0) {
		lua_pushinteger(L, 0);
		return 1;
	}
	pduel->game_field->add_process(PROCESSOR_DISCARD_HAND_S, 0, 0, (group*)(size_t)reason, playerid, min + (max << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::duel_disable_shuffle_check(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	uint8 disable = TRUE;
	if(lua_gettop(L) > 0)
		disable = lua_toboolean(L, 1);
	pduel->game_field->core.shuffle_check_disabled = disable;
	return 0;
}
int32 scriptlib::duel_shuffle_deck(lua_State *L) {
	check_param_count(L, 1);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->shuffle(playerid, LOCATION_DECK);
	return 0;
}
int32 scriptlib::duel_shuffle_hand(lua_State *L) {
	check_param_count(L, 1);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->shuffle(playerid, LOCATION_HAND);
	return 0;
}
int32 scriptlib::duel_shuffle_setcard(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**)lua_touserdata(L, 1);
	if(pgroup->container.size() <= 1)
		return 0;
	duel* pduel = pgroup->pduel;
	group::card_set::iterator cit, rm;
	card* ms[5];
	uint8 seq[5];
	uint8 tp = 2;
	uint8 ct = 0;
	card* pcard = 0;
	for(cit = pgroup->container.begin(); cit != pgroup->container.end();) {
		rm = cit++;
		pcard = *rm;
		if(pcard->current.location != LOCATION_MZONE || (pcard->current.position & POS_FACEUP) || (tp != 2 && (pcard->current.controler != tp)))
			return 0;
		tp = pcard->current.controler;
		ms[ct] = pcard;
		seq[ct] = pcard->current.sequence;
		ct++;
	}
	for(uint32 p = 0; p < 2; ++p) {
		for(uint32 i = 0; i < ct; ++i) {
			uint8 s = pduel->get_next_integer(0, ct - 1);
			pcard = ms[s];
			ms[s] = ms[i];
			ms[i] = pcard;
		}
	}
	pduel->write_buffer8(MSG_SHUFFLE_SET_CARD);
	pduel->write_buffer8(ct);
	for(uint32 i = 0; i < ct; ++i) {
		pduel->write_buffer32(ms[i]->get_info_location());
		pduel->game_field->player[tp].list_mzone[seq[i]] = ms[i];
		ms[i]->current.sequence = seq[i];
	}
	for(uint32 i = 0; i < ct; ++i) {
		if(ms[i]->xyz_materials.size())
			pduel->write_buffer32(ms[i]->get_info_location());
		else
			pduel->write_buffer32(0);
	}
	return 0;
}
int32 scriptlib::duel_change_attacker(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	duel* pduel = target->pduel;
	card* attacker = pduel->game_field->core.attacker;
	card* attack_target = pduel->game_field->core.attack_target;
	attacker->announce_count++;
	attacker->attacked_count++;
	if(attack_target) {
		attacker->announced_cards[attack_target->fieldid_r] = attack_target;
	} else {
		attacker->announced_cards[0] = 0;
	}
	pduel->game_field->core.sub_attacker = target;
	return 0;
}
int32 scriptlib::duel_replace_attacker(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	duel* pduel = target->pduel;
	pduel->game_field->core.sub_attacker = target;
	return 0;
}
int32 scriptlib::duel_change_attack_target(lua_State *L) {
	check_param_count(L, 1);
	if(lua_isnil(L, 1)) {
		duel* pduel = interpreter::get_duel_info(L);
		pduel->game_field->core.sub_attack_target = 0;
	} else {
		check_param(L, PARAM_TYPE_CARD, 1);
		card* target = *(card**) lua_touserdata(L, 1);
		duel* pduel = target->pduel;
		pduel->game_field->core.sub_attack_target = target;
	}
	return 0;
}
int32 scriptlib::duel_replace_attack_target(lua_State *L) {
	return 0;
}
int32 scriptlib::duel_calculate_damage(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* attacker = *(card**)lua_touserdata(L, 1);
	card* attack_target = *(card**)lua_touserdata(L, 2);
	int32 skip_timing = lua_toboolean(L, 3);
	if(attacker == attack_target)
		return 0;
	attacker->pduel->game_field->add_process(PROCESSOR_DAMAGE_STEP, 0, (effect*)attacker, (group*)attack_target, 0, skip_timing);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_get_battle_damage(lua_State *L) {
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	lua_pushinteger(L, pduel->game_field->core.battle_damage[playerid]);
	return 1;
}
int32 scriptlib::duel_change_battle_damage(lua_State *L) {
	check_param_count(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	int32 dam = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 check = TRUE;
	if(lua_gettop(L) >= 3)
		check = lua_toboolean(L, 3);
	if(check && pduel->game_field->core.battle_damage[playerid] == 0)
		return 0;
	pduel->game_field->core.battle_damage[playerid] = dam;
	return 0;
}
int32 scriptlib::duel_change_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 2);
	uint32 count = lua_tointeger(L, 1);
	group* pgroup = *(group**)lua_touserdata(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->change_target(count, pgroup);
	return 0;
}
int32 scriptlib::duel_change_target_player(lua_State *L) {
	check_param_count(L, 2);
	uint32 count = lua_tointeger(L, 1);
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->change_target_player(count, playerid);
	return 0;
}
int32 scriptlib::duel_change_target_param(lua_State *L) {
	check_param_count(L, 2);
	uint32 count = lua_tointeger(L, 1);
	uint32 param = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->change_target_param(count, param);
	return 0;
}
int32 scriptlib::duel_break_effect(lua_State *L) {
	check_action_permission(L);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->break_effect();
	return lua_yield(L, 0);
}
int32 scriptlib::duel_change_effect(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 count = lua_tointeger(L, 1);
	int32 pf = interpreter::get_function_handle(L, 2);
	pduel->game_field->change_chain_effect(count, pf);
	return 0;
}
int32 scriptlib::duel_negate_activate(lua_State *L) {
	check_param_count(L, 1);
	uint32 c = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->negate_chain(c));
	return 1;
}
int32 scriptlib::duel_negate_effect(lua_State *L) {
	check_param_count(L, 1);
	uint32 c = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->disable_chain(c));
	return 1;
}
int32 scriptlib::duel_negate_related_chain(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**)lua_touserdata(L, 1);
	uint32 reset_flag = lua_tointeger(L, 2);
	duel* pduel = pcard->pduel;
	if(pduel->game_field->core.current_chain.size() < 2)
		return FALSE;
	if(!pcard->is_affect_by_effect(pduel->game_field->core.reason_effect))
		return 0;
	effect* negeff = pduel->new_effect();
	negeff->owner = pduel->game_field->core.reason_effect->handler;
	negeff->type = EFFECT_TYPE_SINGLE;
	negeff->code = EFFECT_DISABLE_CHAIN;
	negeff->reset_flag = RESET_CHAIN | RESET_EVENT | reset_flag;
	pcard->add_effect(negeff);
	return 0;
}
int32 scriptlib::duel_disable_summon(lua_State *L) {
	check_param_count(L, 1);
	card* pcard = 0;
	group* pgroup = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE))
		pcard = *(card**) lua_touserdata(L, 1);
	else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE))
		pgroup = *(group**) lua_touserdata(L, 1);
	else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	if(pcard) {
		pcard->set_status(STATUS_SUMMONING, FALSE);
		pcard->set_status(STATUS_SUMMON_DISABLED, TRUE);
		pcard->set_status(STATUS_PROC_COMPLETE, FALSE);
	} else {
		for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
			(*cit)->set_status(STATUS_SUMMONING, FALSE);
			(*cit)->set_status(STATUS_SUMMON_DISABLED, TRUE);
			(*cit)->set_status(STATUS_PROC_COMPLETE, FALSE);
		}
	}
	return 0;
}
int32 scriptlib::duel_increase_summon_count(lua_State *L) {
	card* pcard = 0;
	effect* pextra = 0;
	if(lua_gettop(L) > 0) {
		check_param(L, PARAM_TYPE_CARD, 1);
		pcard = *(card**) lua_touserdata(L, 1);
	}
	duel* pduel = interpreter::get_duel_info(L);
	uint32 playerid = pduel->game_field->core.reason_player;
	if(pcard && (pextra = pcard->is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT)))
		pextra->get_value(pcard);
	else
		pduel->game_field->core.summon_count[playerid]++;
	return 0;
}
int32 scriptlib::duel_check_summon_count(lua_State *L) {
	card* pcard = 0;
	if(lua_gettop(L) > 0) {
		check_param(L, PARAM_TYPE_CARD, 1);
		pcard = *(card**) lua_touserdata(L, 1);
	}
	duel* pduel = interpreter::get_duel_info(L);
	uint32 playerid = pduel->game_field->core.reason_player;
	if((pcard && pcard->is_affected_by_effect(EFFECT_EXTRA_SUMMON_COUNT))
	        || pduel->game_field->core.summon_count[playerid] < pduel->game_field->get_summon_count_limit(playerid))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::duel_get_location_count(lua_State *L) {
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 location = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 uplayer = pduel->game_field->core.reason_player;
	uint32 reason = LOCATION_REASON_TOFIELD;
	if(lua_gettop(L) > 2)
		uplayer = lua_tointeger(L, 3);
	if(lua_gettop(L) > 3)
		reason = lua_tointeger(L, 4);
	lua_pushinteger(L, pduel->game_field->get_useable_count(playerid, location, uplayer, reason));
	return 1;
}
int32 scriptlib::duel_get_field_card(lua_State *L) {
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 location = lua_tointeger(L, 2);
	uint32 sequence = lua_tointeger(L, 3);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	card* pcard = pduel->game_field->get_field_card(playerid, location, sequence);
	if(!pcard || pcard->is_status(STATUS_SUMMONING))
		return 0;
	interpreter::card2value(L, pcard);
	return 1;
}
int32 scriptlib::duel_check_location(lua_State *L) {
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 location = lua_tointeger(L, 2);
	uint32 sequence = lua_tointeger(L, 3);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_location_useable(playerid, location, sequence));
	return 1;
}
int32 scriptlib::duel_get_current_chain(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->core.current_chain.size());
	return 1;
}
int32 scriptlib::duel_get_chain_info(lua_State *L) {
	check_param_count(L, 1);
	uint32 c = lua_tointeger(L, 1);
	uint32 flag;
	uint32 args = lua_gettop(L) - 1;
	duel* pduel = interpreter::get_duel_info(L);
	chain* ch;
	if(c == 0 && pduel->game_field->core.continuous_chain.size())
		ch = &pduel->game_field->core.continuous_chain.back();
	else {
		if(c > pduel->game_field->core.current_chain.size() || c < 1)
			c = pduel->game_field->core.current_chain.size();
		if(c == 0)
			return 0;
		ch = &pduel->game_field->core.current_chain[c - 1];
	}
	for(uint32 i = 0; i < args; ++i) {
		flag = lua_tointeger(L, 2 + i);
		switch(flag) {
		case CHAININFO_CHAIN_COUNT:
			lua_pushinteger(L, ch->chain_count);
			break;
		case CHAININFO_TRIGGERING_EFFECT:
			interpreter::effect2value(L, ch->triggering_effect);
			break;
		case CHAININFO_TRIGGERING_PLAYER:
			lua_pushinteger(L, ch->triggering_player);
			break;
		case CHAININFO_TRIGGERING_CONTROLER:
			lua_pushinteger(L, ch->triggering_controler);
			break;
		case CHAININFO_TRIGGERING_LOCATION:
			lua_pushinteger(L, ch->triggering_location);
			break;
		case CHAININFO_TRIGGERING_SEQUENCE:
			lua_pushinteger(L, ch->triggering_sequence);
			break;
		case CHAININFO_TARGET_CARDS:
			interpreter::group2value(L, ch->target_cards);
			break;
		case CHAININFO_TARGET_PLAYER:
			lua_pushinteger(L, ch->target_player);
			break;
		case CHAININFO_TARGET_PARAM:
			lua_pushinteger(L, ch->target_param);
			break;
		case CHAININFO_DISABLE_REASON:
			interpreter::effect2value(L, ch->disable_reason);
			break;
		case CHAININFO_DISABLE_PLAYER:
			lua_pushinteger(L, ch->disable_player);
			break;
		case CHAININFO_CHAIN_ID:
			lua_pushinteger(L, ch->chain_id);
			break;
		case CHAININFO_TYPE:
			if((ch->triggering_effect->card_type & 0x7) == (TYPE_TRAP | TYPE_MONSTER))
				lua_pushinteger(L, TYPE_MONSTER);
			else lua_pushinteger(L, (ch->triggering_effect->card_type & 0x7));
			break;
		case CHAININFO_EXTTYPE:
			lua_pushinteger(L, ch->triggering_effect->card_type);
			break;
		default:
			lua_pushnil(L);
			break;
		}
	}
	return args;
}
int32 scriptlib::duel_get_first_target(lua_State *L) {
	chain* ch;
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.continuous_chain.size())
		ch = &pduel->game_field->core.continuous_chain.back();
	else if(pduel->game_field->core.current_chain.size())
		ch = &pduel->game_field->core.current_chain.back();
	else return 0;
	if(!ch->target_cards || ch->target_cards->container.size() == 0)
		return 0;
	for(auto iter = ch->target_cards->container.begin(); iter != ch->target_cards->container.end(); ++iter)
		interpreter::card2value(L, *iter);
	return ch->target_cards->container.size();
}
int32 scriptlib::duel_get_current_phase(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->infos.phase);
	return 1;
}
int32 scriptlib::duel_skip_phase(lua_State *L) {
	check_param_count(L, 4);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 phase = lua_tointeger(L, 2);
	uint32 reset = lua_tointeger(L, 3);
	uint32 count = lua_tointeger(L, 4);
	uint32 value = lua_tointeger(L, 5);
	if(count <= 0)
		count = 1;
	duel* pduel = interpreter::get_duel_info(L);
	int32 code = 0;
	if(phase == PHASE_DRAW)
		code = EFFECT_SKIP_DP;
	else if(phase == PHASE_STANDBY)
		code = EFFECT_SKIP_SP;
	else if(phase == PHASE_MAIN1)
		code = EFFECT_SKIP_M1;
	else if(phase == PHASE_BATTLE)
		code = EFFECT_SKIP_BP;
	else if(phase == PHASE_MAIN2)
		code = EFFECT_SKIP_M2;
	else
		return 0;
	effect* peffect = pduel->new_effect();
	peffect->owner = pduel->game_field->temp_card;
	peffect->effect_owner = playerid;
	peffect->type = EFFECT_TYPE_FIELD;
	peffect->code = code;
	peffect->reset_flag = (reset & 0xff) | RESET_PHASE | RESET_SELF_TURN;
	peffect->flag = EFFECT_FLAG_CANNOT_DISABLE | EFFECT_FLAG_PLAYER_TARGET;
	peffect->s_range = 1;
	peffect->o_range = 0;
	peffect->reset_count |= count & 0xff;
	peffect->value = value;
	pduel->game_field->add_effect(peffect, playerid);
	return 0;
}
int32 scriptlib::duel_is_damage_calculated(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->core.damage_calculated);
	return 1;
}
int32 scriptlib::duel_get_attacker(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	card* pcard = pduel->game_field->core.attacker;
	interpreter::card2value(L, pcard);
	return 1;
}
int32 scriptlib::duel_get_attack_target(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	card* pcard = pduel->game_field->core.attack_target;
	interpreter::card2value(L, pcard);
	return 1;
}
int32 scriptlib::duel_disable_attack(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_ATTACK_DISABLE, 0, 0, 0, 0, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_chain_attack(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.chain_attack = TRUE;
	if(lua_gettop(L) > 0) {
		check_param(L, PARAM_TYPE_CARD, 1);
		pduel->game_field->core.chain_attack_target = *(card**) lua_touserdata(L, 1);
	}
	return 0;
}
int32 scriptlib::duel_readjust(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	card* adjcard = pduel->game_field->core.reason_effect->handler;
	pduel->game_field->core.readjust_map[adjcard]++;
	if(pduel->game_field->core.readjust_map[adjcard] > 3) {
		pduel->game_field->send_to(adjcard, 0, REASON_RULE, pduel->game_field->core.reason_player, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		pduel->game_field->core.subunits.begin()->type = PROCESSOR_SENDTO_S;
		return lua_yield(L, 0);
	}
	pduel->game_field->core.re_adjust = TRUE;
	return 0;
}
int32 scriptlib::duel_adjust_instantly(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	if(lua_gettop(L) > 0) {
		check_param(L, PARAM_TYPE_CARD, 1);
		card* pcard = *(card**) lua_touserdata(L, 1);
		pcard->filter_disable_related_cards();
	}
	pduel->game_field->adjust_instant();
	return 0;
}
/**
 * \brief Duel.GetFieldGroup
 * \param playerid, location1, location2
 * \return Group
 */
int32 scriptlib::duel_get_field_group(lua_State *L) {
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 location1 = lua_tointeger(L, 2);
	uint32 location2 = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_field_card(playerid, location1, location2, pgroup);
	interpreter::group2value(L, pgroup);
	return 1;
}
/**
 * \brief Duel.GetFieldGroupCount
 * \param playerid, location1, location2
 * \return Integer
 */
int32 scriptlib::duel_get_field_group_count(lua_State *L) {
	check_param_count(L, 3);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 location1 = lua_tointeger(L, 2);
	uint32 location2 = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 count = pduel->game_field->filter_field_card(playerid, location1, location2, 0);
	lua_pushinteger(L, count);
	return 1;
}
/**
 * \brief Duel.GetDeckTop
 * \param playerid, count
 * \return Group
 */
int32 scriptlib::duel_get_decktop_group(lua_State *L) {
	check_param_count(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 count = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	auto cit = pduel->game_field->player[playerid].list_main.rbegin();
	for(uint32 i = 0; i < count && cit != pduel->game_field->player[playerid].list_main.rend(); ++i, ++cit)
		pgroup->container.insert(*cit);
	interpreter::group2value(L, pgroup);
	return 1;
}
/**
* \brief Duel.GetMatchingGroup
* \param filter_func, self, location1, location2, exception card, (extraargs...)
* \return Group
*/
int32 scriptlib::duel_get_matching_group(lua_State *L) {
	check_param_count(L, 5);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, pgroup, pexception, extraargs);
	interpreter::group2value(L, pgroup);
	return 1;
}
/**
* \brief Duel.GetMatchingGroupCount
* \param filter_func, self, location1, location2, exception card, (extraargs...)
* \return Integer
*/
int32 scriptlib::duel_get_matching_count(lua_State *L) {
	check_param_count(L, 5);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, pgroup, pexception, extraargs);
	uint32 count = pgroup->container.size();
	lua_pushinteger(L, count);
	return 1;
}
/**
* \brief Duel.GetFirstMatchingCard
* \param filter_func, self, location1, location2, exception card, (extraargs...)
* \return Card | nil
*/
int32 scriptlib::duel_get_first_matching_card(lua_State *L) {
	check_param_count(L, 5);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	card* pret = 0;
	pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, 0, pexception, extraargs, &pret);
	if(pret)
		interpreter::card2value(L, pret);
	else lua_pushnil(L);
	return 1;
}
/**
* \brief Duel.IsExistingMatchingCard
* \param filter_func, self, location1, location2, count, exception card, (extraargs...)
* \return boolean
*/
int32 scriptlib::duel_is_existing_matching_card(lua_State *L) {
	check_param_count(L, 6);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 6)) {
		check_param(L, PARAM_TYPE_CARD, 6);
		pexception = *(card**) lua_touserdata(L, 6);
	}
	extraargs = lua_gettop(L) - 6;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	uint32 fcount = lua_tointeger(L, 5);
	lua_pushboolean(L, pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, 0, pexception, extraargs, 0, fcount));
	return 1;
}
/**
* \brief Duel.SelectMatchingCards
* \param playerid, filter_func, self, location1, location2, min, max, exception card, (extraargs...)
* \return Group
*/
int32 scriptlib::duel_select_matching_cards(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 8);
	if(!lua_isnil(L, 2))
		check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 8)) {
		check_param(L, PARAM_TYPE_CARD, 8);
		pexception = *(card**) lua_touserdata(L, 8);
	}
	extraargs = lua_gettop(L) - 8;
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 3);
	uint32 location1 = lua_tointeger(L, 4);
	uint32 location2 = lua_tointeger(L, 5);
	uint32 min = lua_tointeger(L, 6);
	uint32 max = lua_tointeger(L, 7);
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_matching_card(2, (uint8)self, location1, location2, pgroup, pexception, extraargs);
	pduel->game_field->core.select_cards.assign(pgroup->container.begin(), pgroup->container.end());
	pduel->game_field->add_process(PROCESSOR_SELECT_CARD_S, 0, 0, 0, playerid, min + (max << 16));
	return lua_yield(L, 0);
}
/**
* \brief Duel.GetReleaseGroup
* \param playerid
* \return Group
*/
int32 scriptlib::duel_get_release_group(lua_State *L) {
	check_param_count(L, 1);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 hand = FALSE;
	if(lua_gettop(L) > 1)
		hand = lua_toboolean(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	pduel->game_field->get_release_list(playerid, &(pgroup->container), &(pgroup->container), FALSE, hand, 0, 0, 0);
	interpreter::group2value(L, pgroup);
	return 1;
}
/**
* \brief Duel.GetReleaseGroupCount
* \param playerid
* \return Integer
*/
int32 scriptlib::duel_get_release_group_count(lua_State *L) {
	check_param_count(L, 1);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 hand = FALSE;
	if(lua_gettop(L) > 1)
		hand = lua_toboolean(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->get_release_list(playerid, 0, 0, FALSE, hand, 0, 0, 0));
	return 1;
}
int32 scriptlib::duel_check_release_group(lua_State *L) {
	check_param_count(L, 4);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 use_con = FALSE;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_FUNCTION, 2);
		use_con = TRUE;
	}
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_CARD, 4);
		pexception = *(card**) lua_touserdata(L, 4);
	}
	extraargs = lua_gettop(L) - 4;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 fcount = lua_tointeger(L, 3);
	lua_pushboolean(L, pduel->game_field->check_release_list(playerid, fcount, use_con, FALSE, 2, extraargs, pexception));
	return 1;
}
int32 scriptlib::duel_select_release_group(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 use_con = FALSE;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_FUNCTION, 2);
		use_con = TRUE;
	}
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 min = lua_tointeger(L, 3);
	uint32 max = lua_tointeger(L, 4);
	pduel->game_field->core.release_cards.clear();
	pduel->game_field->core.release_cards_ex.clear();
	pduel->game_field->get_release_list(playerid, &pduel->game_field->core.release_cards, &pduel->game_field->core.release_cards_ex, use_con, FALSE, 2, extraargs, pexception);
	pduel->game_field->add_process(PROCESSOR_SELECT_RELEASE_S, 0, 0, 0, playerid, (max << 16) + min);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_check_release_group_ex(lua_State *L) {
	check_param_count(L, 4);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 use_con = FALSE;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_FUNCTION, 2);
		use_con = TRUE;
	}
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_CARD, 4);
		pexception = *(card**) lua_touserdata(L, 4);
	}
	extraargs = lua_gettop(L) - 4;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 fcount = lua_tointeger(L, 3);
	lua_pushboolean(L, pduel->game_field->check_release_list(playerid, fcount, use_con, TRUE, 2, extraargs, pexception));
	return 1;
}
int32 scriptlib::duel_select_release_group_ex(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 use_con = FALSE;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_FUNCTION, 2);
		use_con = TRUE;
	}
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 min = lua_tointeger(L, 3);
	uint32 max = lua_tointeger(L, 4);
	pduel->game_field->core.release_cards.clear();
	pduel->game_field->core.release_cards_ex.clear();
	pduel->game_field->get_release_list(playerid, &pduel->game_field->core.release_cards, &pduel->game_field->core.release_cards_ex, use_con, TRUE, 2, extraargs, pexception);
	pduel->game_field->add_process(PROCESSOR_SELECT_RELEASE_S, 0, 0, 0, playerid, (max << 16) + min);
	return lua_yield(L, 0);
}
/**
* \brief Duel.GetTributeGroup
* \param targetcard
* \return Group
*/
int32 scriptlib::duel_get_tribute_group(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	pduel->game_field->get_summon_release_list(target, &(pgroup->container), &(pgroup->container), 0);
	interpreter::group2value(L, pgroup);
	return 1;
}
/**
* \brief Duel.GetTributeCount
* \param targetcard
* \return Integer
*/
int32 scriptlib::duel_get_tribute_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	group* mg = 0;
	if(lua_gettop(L) >= 2 && !lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_GROUP, 2);
		mg = *(group**) lua_touserdata(L, 2);
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->get_summon_release_list(target, 0, 0, 0, mg));
	return 1;
}
int32 scriptlib::duel_select_tribute(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_CARD, 2);
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* target = *(card**) lua_touserdata(L, 2);
	uint32 min = lua_tointeger(L, 3);
	uint32 max = lua_tointeger(L, 4);
	group* mg = 0;
	if(lua_gettop(L) >= 5 && !lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_GROUP, 5);
		mg = *(group**) lua_touserdata(L, 5);
	}
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.release_cards.clear();
	pduel->game_field->core.release_cards_ex.clear();
	pduel->game_field->core.release_cards_ex_sum.clear();
	pduel->game_field->get_summon_release_list(target, &pduel->game_field->core.release_cards, &pduel->game_field->core.release_cards_ex, &pduel->game_field->core.release_cards_ex_sum, mg);
	pduel->game_field->add_process(PROCESSOR_SELECT_TRIBUTE_S, 0, 0, 0, playerid, (max << 16) + min);
	return lua_yield(L, 0);
}
/**
* \brief Duel.GetTargetCount
* \param filter_func, self, location1, location2, exception card, (extraargs...)
* \return Group
*/
int32 scriptlib::duel_get_target_count(lua_State *L) {
	check_param_count(L, 5);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	extraargs = lua_gettop(L) - 5;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	group* pgroup = pduel->new_group();
	uint32 count = 0;
	pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, pgroup, pexception, extraargs, 0, 0, TRUE);
	count = pgroup->container.size();
	lua_pushinteger(L, count);
	return 1;
}
/**
* \brief Duel.IsExistingTarget
* \param filter_func, self, location1, location2, count, exception card, (extraargs...)
* \return boolean
*/
int32 scriptlib::duel_is_existing_target(lua_State *L) {
	check_param_count(L, 6);
	if(!lua_isnil(L, 1))
		check_param(L, PARAM_TYPE_FUNCTION, 1);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 6)) {
		check_param(L, PARAM_TYPE_CARD, 6);
		pexception = *(card**) lua_touserdata(L, 6);
	}
	extraargs = lua_gettop(L) - 6;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	uint32 count = lua_tointeger(L, 5);
	lua_pushboolean(L, pduel->game_field->filter_matching_card(1, (uint8)self, location1, location2, 0, pexception, extraargs, 0, count, TRUE));
	return 1;
}
/**
* \brief Duel.SelectTarget
* \param playerid, filter_func, self, location1, location2, min, max, exception card, (extraargs...)
* \return Group
*/
int32 scriptlib::duel_select_target(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 8);
	if(!lua_isnil(L, 2))
		check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	uint32 extraargs = 0;
	if(!lua_isnil(L, 8)) {
		check_param(L, PARAM_TYPE_CARD, 8);
		pexception = *(card**) lua_touserdata(L, 8);
	}
	extraargs = lua_gettop(L) - 8;
	uint32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	uint32 self = lua_tointeger(L, 3);
	uint32 location1 = lua_tointeger(L, 4);
	uint32 location2 = lua_tointeger(L, 5);
	uint32 min = lua_tointeger(L, 6);
	uint32 max = lua_tointeger(L, 7);
	if(pduel->game_field->core.current_chain.size() == 0)
		return 0;
	group* pgroup = pduel->new_group();
	pduel->game_field->filter_matching_card(2, (uint8)self, location1, location2, pgroup, pexception, extraargs, 0, 0, TRUE);
	pduel->game_field->core.select_cards.assign(pgroup->container.begin(), pgroup->container.end());
	pduel->game_field->add_process(PROCESSOR_SELECT_TARGET, 0, 0, 0, playerid, min + (max << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::duel_select_fusion_material(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 3);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	check_param(L, PARAM_TYPE_CARD, 2);
	check_param(L, PARAM_TYPE_GROUP, 3);
	card* cg = 0;
	int32 chkf = PLAYER_NONE;
	if(lua_gettop(L) > 3 && !lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_CARD, 4);
		cg = *(card**) lua_touserdata(L, 4);
	}
	if(lua_gettop(L) > 4)
		chkf = lua_tointeger(L, 5);
	card* pcard = *(card**) lua_touserdata(L, 2);
	group* pgroup = *(group**) lua_touserdata(L, 3);
	pcard->fusion_select(playerid, pgroup, cg, chkf);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_set_fusion_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	pduel->game_field->core.fusion_materials = pgroup->container;
	return 0;
}
int32 scriptlib::duel_set_synchro_material(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	pduel->game_field->core.synchro_materials = pgroup->container;
	return 0;
}
int32 scriptlib::duel_select_synchro_material(lua_State *L) {
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**) lua_touserdata(L, 2);
	duel* pduel = pcard->pduel;
	if(!lua_isnil(L, 3))
		check_param(L, PARAM_TYPE_FUNCTION, 3);
	if(!lua_isnil(L, 4))
		check_param(L, PARAM_TYPE_FUNCTION, 4);
	int32 min = lua_tointeger(L, 5);
	int32 max = lua_tointeger(L, 6);
	card* smat = 0;
	group* mg = 0;
	if(lua_gettop(L) >= 7 && !lua_isnil(L, 7)) {
		check_param(L, PARAM_TYPE_CARD, 7);
		smat = *(card**) lua_touserdata(L, 7);
	}
	if(lua_gettop(L) >= 8 && !lua_isnil(L, 8)) {
		check_param(L, PARAM_TYPE_GROUP, 8);
		mg = *(group**) lua_touserdata(L, 8);
	}
	if(mg)
		pduel->game_field->add_process(PROCESSOR_SELECT_SYNCHRO, 0, (effect*)mg, (group*)pcard, playerid, min + (max << 16));
	else
		pduel->game_field->add_process(PROCESSOR_SELECT_SYNCHRO, 0, (effect*)smat, (group*)pcard, playerid + 0x10000, min + (max << 16));
	lua_pushvalue(L, 3);
	lua_pushvalue(L, 4);
	return lua_yield(L, 2);
}
int32 scriptlib::duel_check_synchro_material(lua_State *L) {
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	if(!lua_isnil(L, 2))
		check_param(L, PARAM_TYPE_FUNCTION, 2);
	if(!lua_isnil(L, 3))
		check_param(L, PARAM_TYPE_FUNCTION, 3);
	int32 min = lua_tointeger(L, 4);
	int32 max = lua_tointeger(L, 5);
	card* smat = 0;
	group* mg = 0;
	if(lua_gettop(L) >= 6 && !lua_isnil(L, 6)) {
		check_param(L, PARAM_TYPE_CARD, 6);
		smat = *(card**) lua_touserdata(L, 6);
	}
	if(lua_gettop(L) >= 7 && !lua_isnil(L, 7)) {
		check_param(L, PARAM_TYPE_GROUP, 7);
		mg = *(group**) lua_touserdata(L, 7);
	}
	lua_pushboolean(L, pduel->game_field->check_synchro_material(pcard, 2, 3, min, max, smat, mg));
	return 1;
}
int32 scriptlib::duel_select_tuner_material(lua_State *L) {
	check_param_count(L, 7);
	check_param(L, PARAM_TYPE_CARD, 2);
	check_param(L, PARAM_TYPE_CARD, 3);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**) lua_touserdata(L, 2);
	card* tuner = *(card**) lua_touserdata(L, 3);
	duel* pduel = pcard->pduel;
	if(!lua_isnil(L, 4))
		check_param(L, PARAM_TYPE_FUNCTION, 4);
	if(!lua_isnil(L, 5))
		check_param(L, PARAM_TYPE_FUNCTION, 5);
	int32 min = lua_tointeger(L, 6);
	int32 max = lua_tointeger(L, 7);
	group* mg = 0;
	if(lua_gettop(L) >= 8 && !lua_isnil(L, 8)) {
		check_param(L, PARAM_TYPE_GROUP, 8);
		mg = *(group**) lua_touserdata(L, 8);
	}
	if(!pduel->game_field->check_tuner_material(pcard, tuner, 4, 5, min, max, 0, mg))
		return 0;
	pduel->game_field->core.select_cards.clear();
	pduel->game_field->core.select_cards.push_back(tuner);
	pduel->game_field->returns.bvalue[1] = 0;
	pduel->game_field->add_process(PROCESSOR_SELECT_SYNCHRO, 1, (effect*)mg, (group*)pcard, playerid, min + (max << 16));
	lua_pushvalue(L, 4);
	lua_pushvalue(L, 5);
	return lua_yield(L, 2);
}
int32 scriptlib::duel_check_tuner_material(lua_State *L) {
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* tuner = *(card**) lua_touserdata(L, 2);
	duel* pduel = pcard->pduel;
	if(pduel->game_field->core.global_flag & GLOBALFLAG_MUST_BE_SMATERIAL) {
		effect_set eset;
		pduel->game_field->filter_player_effect(pcard->current.controler, EFFECT_MUST_BE_SMATERIAL, &eset);
		if(eset.size() && eset[0]->handler != tuner) {
			lua_pushboolean(L, false);
			return 1;
		}
	}
	if(!lua_isnil(L, 3))
		check_param(L, PARAM_TYPE_FUNCTION, 3);
	if(!lua_isnil(L, 4))
		check_param(L, PARAM_TYPE_FUNCTION, 4);
	int32 min = lua_tointeger(L, 5);
	int32 max = lua_tointeger(L, 6);
	group* mg = 0;
	if(lua_gettop(L) >= 7 && !lua_isnil(L, 7)) {
		check_param(L, PARAM_TYPE_GROUP, 7);
		mg = *(group**) lua_touserdata(L, 7);
	}
	lua_pushboolean(L, pduel->game_field->check_tuner_material(pcard, tuner, 3, 4, min, max, 0, mg));
	return 1;
}
int32 scriptlib::duel_get_ritual_material(lua_State *L) {
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	pduel->game_field->get_ritual_material(playerid, pduel->game_field->core.reason_effect, &pgroup->container);
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::duel_release_ritual_material(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	pgroup->pduel->game_field->ritual_release(&pgroup->container);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_set_target_card(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	card* pcard = 0;
	group* pgroup = 0;
	duel* pduel = 0;
	if(check_param(L, PARAM_TYPE_CARD, 1, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 1);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 1, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 1);
		pduel = pgroup->pduel;
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 1);
	if(pduel->game_field->core.continuous_chain.size()) {
		if(!pduel->game_field->core.continuous_chain.rbegin()->target_cards) {
			pduel->game_field->core.continuous_chain.rbegin()->target_cards = pduel->new_group();
			pduel->game_field->core.continuous_chain.rbegin()->target_cards->is_readonly = TRUE;
		}
		if(pcard)
			pduel->game_field->core.continuous_chain.rbegin()->target_cards->container.insert(pcard);
		else
			pduel->game_field->core.continuous_chain.rbegin()->target_cards->container = pgroup->container;
	} else if(pduel->game_field->core.current_chain.size()) {
		effect* peffect = pduel->game_field->core.current_chain.rbegin()->triggering_effect;
		if(!pduel->game_field->core.current_chain.rbegin()->target_cards) {
			pduel->game_field->core.current_chain.rbegin()->target_cards = pduel->new_group();
			pduel->game_field->core.current_chain.rbegin()->target_cards->is_readonly = TRUE;
		}
		group* targets = pduel->game_field->core.current_chain.rbegin()->target_cards;
		if(pcard) {
			targets->container.insert(pcard);
			pcard->create_relation(peffect);
		} else {
			targets->container.insert(pgroup->container.begin(), pgroup->container.end());
			for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit)
				(*cit)->create_relation(peffect);
		}
		if(peffect->flag & EFFECT_FLAG_CARD_TARGET) {
			if(pcard) {
				if(pcard->current.location & 0x30)
					pduel->game_field->move_card(pcard->current.controler, pcard, pcard->current.location, 0);
				pduel->write_buffer8(MSG_BECOME_TARGET);
				pduel->write_buffer8(1);
				pduel->write_buffer32(pcard->get_info_location());
			} else {
				for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
					if((*cit)->current.location & 0x30)
						pduel->game_field->move_card((*cit)->current.controler, (*cit), (*cit)->current.location, 0);
					pduel->write_buffer8(MSG_BECOME_TARGET);
					pduel->write_buffer8(1);
					pduel->write_buffer32((*cit)->get_info_location());
				}
			}
		}
	}
	return 0;
}
int32 scriptlib::duel_clear_target_card(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.continuous_chain.size()) {
		if(pduel->game_field->core.continuous_chain.rbegin()->target_cards)
			pduel->game_field->core.continuous_chain.rbegin()->target_cards->container.clear();
	} else if(pduel->game_field->core.current_chain.size()) {
		if(pduel->game_field->core.current_chain.rbegin()->target_cards)
			pduel->game_field->core.current_chain.rbegin()->target_cards->container.clear();
	}
	return 0;
}
int32 scriptlib::duel_set_target_player(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	uint32 playerid = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.continuous_chain.size()) {
		pduel->game_field->core.continuous_chain.rbegin()->target_player = playerid;
	} else if(pduel->game_field->core.current_chain.size()) {
		pduel->game_field->core.current_chain.rbegin()->target_player = playerid;
	}
	return 0;
}
int32 scriptlib::duel_set_target_param(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	uint32 param = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.continuous_chain.size()) {
		pduel->game_field->core.continuous_chain.rbegin()->target_param = param;
	} else if(pduel->game_field->core.current_chain.size()) {
		pduel->game_field->core.current_chain.rbegin()->target_param = param;
	}
	return 0;
}
/**
* \brief Duel.SetOperationInfo
* \param target_group, target_count, target_player, targ
* \return N/A
*/
int32 scriptlib::duel_set_operation_info(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	group* pgroup = 0;
	card* pcard = 0;
	group* pg = 0;
	uint32 ct = lua_tointeger(L, 1);
	uint32 cate = lua_tointeger(L, 2);
	uint32 count = lua_tointeger(L, 4);
	uint32 playerid = lua_tointeger(L, 5);
	uint32 param = lua_tointeger(L, 6);
	duel* pduel;
	if(check_param(L, PARAM_TYPE_CARD, 3, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 3);
		pduel = pcard->pduel;
	} else if(check_param(L, PARAM_TYPE_GROUP, 3, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 3);
		pduel = pgroup->pduel;
	} else
		pduel = interpreter::get_duel_info(L);
	if(!pduel->game_field->core.continuous_chain.size() && !pduel->game_field->core.current_chain.size())
		return 0;
	if( ct && !pduel->game_field->core.current_chain.size())
		return 0;
	if(pgroup) {
		pg = pduel->new_group(pgroup->container);
		pg->is_readonly = TRUE;
	} else if(pcard) {
		pg = pduel->new_group(pcard);
		pg->is_readonly = TRUE;
	} else
		pg = 0;
	optarget opt;
	opt.op_cards = pg;
	opt.op_count = count;
	opt.op_player = playerid;
	opt.op_param = param;
	if(ct == 0 && pduel->game_field->core.continuous_chain.size()) {
		field::chain_list::reverse_iterator clit = pduel->game_field->core.continuous_chain.rbegin();
		chain::opmap::iterator omit = clit->opinfos.find(cate);
		if(omit != clit->opinfos.end() && omit->second.op_cards)
			pduel->delete_group(omit->second.op_cards);
		clit->opinfos[cate] = opt;
	} else {
		if (pduel->game_field->core.current_chain.size() == 0)
			return 0;
		if(ct < 1 || ct > pduel->game_field->core.current_chain.size()) {
			field::chain_array::reverse_iterator cait = pduel->game_field->core.current_chain.rbegin();
			chain::opmap::iterator omit = cait->opinfos.find(cate);
			if(omit != cait->opinfos.end() && omit->second.op_cards)
				pduel->delete_group(omit->second.op_cards);
			cait->opinfos[cate] = opt;
		} else {
			chain* ch = &pduel->game_field->core.current_chain[ct - 1];
			chain::opmap::iterator omit = ch->opinfos.find(cate);
			if(omit != ch->opinfos.end() && omit->second.op_cards)
				pduel->delete_group(omit->second.op_cards);
			ch->opinfos[cate] = opt;
		}
	}
	return 0;
}
int32 scriptlib::duel_get_operation_info(lua_State *L) {
	check_param_count(L, 2);
	uint32 ct = lua_tointeger(L, 1);
	uint32 cate = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	if(!pduel->game_field->core.continuous_chain.size() && !pduel->game_field->core.current_chain.size())
		return 0;
	if( ct && !pduel->game_field->core.current_chain.size())
		return 0;
	bool found = false;
	chain::opmap::iterator oit;
	optarget opt;
	if(ct == 0 && pduel->game_field->core.continuous_chain.size()) {
		if((oit = pduel->game_field->core.continuous_chain.rbegin()->opinfos.find(cate)) != pduel->game_field->core.continuous_chain.rbegin()->opinfos.end()) {
			opt = oit->second;
			found = true;
		}
	} else {
		if(ct < 1 || ct > pduel->game_field->core.current_chain.size()) {
			if((oit = pduel->game_field->core.current_chain.rbegin()->opinfos.find(cate)) != pduel->game_field->core.current_chain.rbegin()->opinfos.end()) {
				opt = oit->second;
				found = true;
			}
		} else {
			if((oit = pduel->game_field->core.current_chain[ct - 1].opinfos.find(cate)) != pduel->game_field->core.current_chain[ct - 1].opinfos.end()) {
				opt = oit->second;
				found = true;
			}
		}
	}
	if(!found) {
		lua_pushboolean(L, 0);
		return 1;
	} else {
		lua_pushboolean(L, 1);
		if(opt.op_cards)
			interpreter::group2value(L, opt.op_cards);
		else
			lua_pushnil(L);
		lua_pushinteger(L, opt.op_count);
		lua_pushinteger(L, opt.op_player);
		lua_pushinteger(L, opt.op_param);
		return 5;
	}
}
int32 scriptlib::duel_get_operation_count(lua_State *L) {
	check_param_count(L, 1);
	uint32 ct = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	if(!pduel->game_field->core.continuous_chain.size() && !pduel->game_field->core.current_chain.size())
		return 0;
	if( ct && !pduel->game_field->core.current_chain.size())
		return 0;
	if(ct == 0 && pduel->game_field->core.continuous_chain.size()) {
		lua_pushinteger(L, pduel->game_field->core.continuous_chain.rbegin()->opinfos.size());
	} else {
		if(ct < 1 || ct > pduel->game_field->core.current_chain.size())
			lua_pushinteger(L, pduel->game_field->core.current_chain.rbegin()->opinfos.size());
		else
			lua_pushinteger(L, pduel->game_field->core.current_chain[ct - 1].opinfos.size());
	}
	return 1;
}
int32 scriptlib::duel_check_xyz_material(lua_State *L) {
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 1);
	uint32 findex = 0;
	if(!lua_isnil(L, 2)) {
		check_param(L, PARAM_TYPE_FUNCTION, 2);
		findex = 2;
	}
	card* scard = *(card**) lua_touserdata(L, 1);
	uint32 lv = lua_tointeger(L, 3);
	uint32 minc = lua_tointeger(L, 4);
	uint32 maxc = lua_tointeger(L, 5);
	group* mg = nullptr;
	if(!lua_isnil(L, 6)) {
		check_param(L, PARAM_TYPE_GROUP, 6);
		mg = *(group**) lua_touserdata(L, 6);
	}
	lua_pushboolean(L, scard->pduel->game_field->check_xyz_material(scard, findex, lv, minc, maxc, mg));
	return 1;
}
int32 scriptlib::duel_select_xyz_material(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_CARD, 2);
	uint32 findex = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_FUNCTION, 3);
		findex = 3;
	}
	card* scard = *(card**) lua_touserdata(L, 2);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 lv = lua_tointeger(L, 4);
	uint32 minc = lua_tointeger(L, 5);
	uint32 maxc = lua_tointeger(L, 6);
	duel* pduel = scard->pduel;
	pduel->game_field->get_xyz_material(scard, findex, lv, maxc);
	scard->pduel->game_field->add_process(PROCESSOR_SELECT_XMATERIAL, 0, 0, (group*)scard, playerid + (lv << 16), minc + (maxc << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::duel_overlay(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* target = *(card**) lua_touserdata(L, 1);
	card* pcard = 0;
	group* pgroup = 0;
	if(check_param(L, PARAM_TYPE_CARD, 2, TRUE)) {
		pcard = *(card**) lua_touserdata(L, 2);
	} else if(check_param(L, PARAM_TYPE_GROUP, 2, TRUE)) {
		pgroup = *(group**) lua_touserdata(L, 2);
	} else
		luaL_error(L, "Parameter %d should be \"Card\" or \"Group\".", 2);
	if(pcard) {
		card::card_set cset;
		cset.insert(pcard);
		target->xyz_overlay(&cset);
	} else
		target->xyz_overlay(&pgroup->container);
	if(target->current.location == LOCATION_MZONE)
		target->pduel->game_field->adjust_all();
	return lua_yield(L, 0);
}
int32 scriptlib::duel_get_overlay_group(lua_State *L) {
	check_param_count(L, 3);
	uint32 rplayer = lua_tointeger(L, 1);
	if(rplayer != 0 && rplayer != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	pduel->game_field->get_overlay_group(rplayer, s, o, &pgroup->container);
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::duel_get_overlay_count(lua_State *L) {
	check_param_count(L, 3);
	uint32 rplayer = lua_tointeger(L, 1);
	if(rplayer != 0 && rplayer != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushinteger(L, pduel->game_field->get_overlay_count(rplayer, s, o));
	return 1;
}
int32 scriptlib::duel_check_remove_overlay_card(lua_State *L) {
	check_param_count(L, 5);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	int32 count = lua_tointeger(L, 4);
	int32 reason = lua_tointeger(L, 5);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_remove_overlay_card(playerid, 0, s, o, count, reason));
	return 1;
}
int32 scriptlib::duel_remove_overlay_card(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 s = lua_tointeger(L, 2);
	uint32 o = lua_tointeger(L, 3);
	int32 min = lua_tointeger(L, 4);
	int32 max = lua_tointeger(L, 5);
	int32 reason = lua_tointeger(L, 6);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->remove_overlay_card(reason, 0, playerid, s, o, min, max);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_hint(lua_State * L) {
	check_param_count(L, 3);
	int32 htype = lua_tointeger(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 desc = lua_tointeger(L, 3);
	if(htype == HINT_OPSELECTED)
		playerid = 1 - playerid;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->write_buffer8(MSG_HINT);
	pduel->write_buffer8(htype);
	pduel->write_buffer8(playerid);
	pduel->write_buffer32(desc);
	return 0;
}
int32 scriptlib::duel_hint_selection(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
		card* pcard = *cit;
		if(pcard->current.location & 0x30)
			pduel->game_field->move_card(pcard->current.controler, pcard, pcard->current.location, 0);
		pduel->write_buffer8(MSG_BECOME_TARGET);
		pduel->write_buffer8(1);
		pduel->write_buffer32(pcard->get_info_location());
	}
	return 0;
}
int32 scriptlib::duel_select_effect_yesno(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	card* pcard = *(card**) lua_touserdata(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_SELECT_EFFECTYN_S, 0, 0, (group*)pcard, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_select_yesno(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 desc = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_SELECT_YESNO_S, 0, 0, 0, playerid, desc);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_select_option(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 1);
	uint32 count = lua_gettop(L) - 1;
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.select_options.clear();
	for(uint32 i = 0; i < count; ++i)
		pduel->game_field->core.select_options.push_back(lua_tointeger(L, i + 2));
	pduel->game_field->add_process(PROCESSOR_SELECT_OPTION_S, 0, 0, 0, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_select_sequence(lua_State * L) {
	check_action_permission(L);
	return 0;
}
int32 scriptlib::duel_select_position(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 2);
	int32 playerid = lua_tointeger(L, 1);
	card* pcard = *(card**) lua_touserdata(L, 2);
	uint32 positions = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_SELECT_POSITION_S, 0, 0, 0, playerid + (positions << 16), pcard->data.code);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_select_disable_field(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 5);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 count = lua_tointeger(L, 2);
	uint32 location1 = lua_tointeger(L, 3);
	uint32 location2 = lua_tointeger(L, 4);
	uint32 filter = lua_tointeger(L, 5);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 ct1 = 0, ct2 = 0, ct3 = 0, ct4 = 0, plist = 0, flag = 0xffffffff;
	if(location1 & LOCATION_MZONE) {
		ct1 = pduel->game_field->get_useable_count(playerid, LOCATION_MZONE, PLAYER_NONE, 0, &plist);
		flag = (flag & 0xffffff00) | plist;
	}
	if(location1 & LOCATION_SZONE) {
		ct2 = pduel->game_field->get_useable_count(playerid, LOCATION_SZONE, PLAYER_NONE, 0, &plist);
		flag = (flag & 0xffff00ff) | (plist << 8);
	}
	if(location2 & LOCATION_MZONE) {
		ct3 = pduel->game_field->get_useable_count(1 - playerid, LOCATION_MZONE, PLAYER_NONE, 0, &plist);
		flag = (flag & 0xff00ffff) | (plist << 16);
	}
	if(location2 & LOCATION_SZONE) {
		ct4 = pduel->game_field->get_useable_count(1 - playerid, LOCATION_SZONE, PLAYER_NONE, 0, &plist);
		flag = (flag & 0xffffff) | (plist << 24);
	}
	flag |= filter | 0xe0e0e0e0;
	if(count > ct1 + ct2 + ct3 + ct4)
		count = ct1 + ct2 + ct3 + ct4;
	if(count == 0)
		return 0;
	pduel->game_field->add_process(PROCESSOR_SELECT_DISFIELD_S, 0, 0, 0, playerid + (count << 16), flag);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_race(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 3);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	int32 available = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_ANNOUNCE_RACE, 0, 0, 0, playerid + (count << 16), available);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_attribute(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 3);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	int32 available = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_ANNOUNCE_ATTRIB, 0, 0, 0, playerid + (count << 16), available);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_level(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_ANNOUNCE_NUMBER, 0, 0, 0, playerid + 0x10000, 0xc0001);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_card(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->add_process(PROCESSOR_ANNOUNCE_CARD, 0, 0, 0, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_type(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	pduel->game_field->core.select_options.clear();
	pduel->game_field->core.select_options.push_back(70);
	pduel->game_field->core.select_options.push_back(71);
	pduel->game_field->core.select_options.push_back(72);
	pduel->game_field->add_process(PROCESSOR_SELECT_OPTION_S, 0, 0, 0, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_number(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.select_options.clear();
	for(int32 i = 2; i <= lua_gettop(L); ++i)
		pduel->game_field->core.select_options.push_back(lua_tointeger(L, i));
	pduel->game_field->add_process(PROCESSOR_ANNOUNCE_NUMBER, 0, 0, 0, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_announce_coin(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	pduel->game_field->core.select_options.clear();
	pduel->game_field->core.select_options.push_back(60);
	pduel->game_field->core.select_options.push_back(61);
	pduel->game_field->add_process(PROCESSOR_SELECT_OPTION_S, 0, 0, 0, playerid, 0);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_toss_coin(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	if((playerid != 0 && playerid != 1) || count <= 0)
		return 0;
	if(count > 5)
		count = 5;
	pduel->game_field->add_process(PROCESSOR_TOSS_COIN, 0, pduel->game_field->core.reason_effect, 0, (pduel->game_field->core.reason_player << 16) + playerid, count);
	return lua_yield(L, 0);
}
int32 scriptlib::duel_toss_dice(lua_State * L) {
	check_action_permission(L);
	check_param_count(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	int32 count1 = lua_tointeger(L, 2);
	int32 count2 = 0;
	if(lua_gettop(L) > 2)
		count2 = lua_tointeger(L, 3);
	if((playerid != 0 && playerid != 1) || count1 <= 0 || count2 < 0)
		return 0;
	if(count1 > 5)
		count1 = 5;
	if(count2 > 5 - count1)
		count2 = 5 - count1;
	pduel->game_field->add_process(PROCESSOR_TOSS_DICE, 0, pduel->game_field->core.reason_effect, 0, (pduel->game_field->core.reason_player << 16) + playerid, count1 + (count2 << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::duel_get_coin_result(lua_State * L) {
	duel* pduel = interpreter::get_duel_info(L);
	for(int32 i = 0; i < 5; ++i)
		lua_pushinteger(L, pduel->game_field->core.coin_result[i]);
	return 5;
}
int32 scriptlib::duel_get_dice_result(lua_State * L) {
	duel* pduel = interpreter::get_duel_info(L);
	for(int32 i = 0; i < 5; ++i)
		lua_pushinteger(L, pduel->game_field->core.dice_result[i]);
	return 5;
}
int32 scriptlib::duel_set_coin_result(lua_State * L) {
	duel* pduel = interpreter::get_duel_info(L);
	int32 res;
	for(int32 i = 0; i < 5; ++i) {
		res = lua_tointeger(L, i + 1);
		if(res != 0 && res != 1)
			res = 0;
		pduel->game_field->core.coin_result[i] = res;
	}
	return 0;
}
int32 scriptlib::duel_set_dice_result(lua_State * L) {
	duel* pduel = interpreter::get_duel_info(L);
	int32 res;
	for(int32 i = 0; i < 5; ++i) {
		res = lua_tointeger(L, i + 1);
		if(res < 1 || res > 6)
			res = 1;
		pduel->game_field->core.dice_result[i] = res;
	}
	return 0;
}
int32 scriptlib::duel_is_player_affected_by_effect(lua_State *L) {
	check_param_count(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1) {
		lua_pushnil(L);
		return 1;
	}
	int32 code = lua_tointeger(L, 2);
	effect* peffect = pduel->game_field->is_player_affected_by_effect(playerid, code);
	interpreter::effect2value(L, peffect);
	return 1;
}
int32 scriptlib::duel_is_player_can_draw(lua_State * L) {
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	uint32 count = 0;
	if(lua_gettop(L) > 1)
		count = lua_tointeger(L, 2);
	duel* pduel = interpreter::get_duel_info(L);
	if(count == 0)
		lua_pushboolean(L, pduel->game_field->is_player_can_draw(playerid));
	else
		lua_pushboolean(L, pduel->game_field->is_player_can_draw(playerid)
		                && (pduel->game_field->player[playerid].list_main.size() >= count));
	return 1;
}
int32 scriptlib::duel_is_player_can_discard_deck(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_discard_deck(playerid, count));
	return 1;
}
int32 scriptlib::duel_is_player_can_discard_deck_as_cost(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_discard_deck_as_cost(playerid, count));
	return 1;
}
int32 scriptlib::duel_is_player_can_summon(lua_State * L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_CARD, 3);
	card* pcard = *(card**) lua_touserdata(L, 3);
	int32 playerid = lua_tointeger(L, 1);
	int32 sumtype = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_summon(sumtype, playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_spsummon(lua_State * L) {
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	if(lua_gettop(L) == 1)
		lua_pushboolean(L, pduel->game_field->is_player_can_spsummon(playerid));
	else {
		check_param_count(L, 5);
		check_param(L, PARAM_TYPE_CARD, 5);
		int32 sumtype = lua_tointeger(L, 2);
		int32 sumpos = lua_tointeger(L, 3);
		int32 toplayer = lua_tointeger(L, 4);
		card* pcard = *(card**) lua_touserdata(L, 5);
		lua_pushboolean(L, pduel->game_field->is_player_can_spsummon(pduel->game_field->core.reason_effect, sumtype, sumpos, playerid, toplayer, pcard));
	}
	return 1;
}
int32 scriptlib::duel_is_player_can_flipsummon(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_flipsummon(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_spsummon_monster(lua_State * L) {
	check_param_count(L, 9);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	int32 code = lua_tointeger(L, 2);
	card_data dat;
	::read_card(code, &dat);
	dat.code = code;
	dat.alias = 0;
	if(!lua_isnil(L, 3))
		dat.setcode = lua_tointeger(L, 3);
	if(!lua_isnil(L, 4))
		dat.type = lua_tointeger(L, 4);
	if(!lua_isnil(L, 5))
		dat.attack = lua_tointeger(L, 5);
	if(!lua_isnil(L, 6))
		dat.defence = lua_tointeger(L, 6);
	if(!lua_isnil(L, 7))
		dat.level = lua_tointeger(L, 7);
	if(!lua_isnil(L, 8))
		dat.race = lua_tointeger(L, 8);
	if(!lua_isnil(L, 9))
		dat.attribute = lua_tointeger(L, 9);
	int32 pos = POS_FACEUP;
	int32 toplayer = playerid;
	if(lua_gettop(L) >= 10)
		pos = lua_tointeger(L, 10);
	if(lua_gettop(L) >= 11)
		toplayer = lua_tointeger(L, 11);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_spsummon_monster(playerid, toplayer, pos, &dat));
	return 1;
}
int32 scriptlib::duel_is_player_can_spsummon_count(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	int32 count = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_spsummon_count(playerid, count));
	return 1;
}
int32 scriptlib::duel_is_player_can_release(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_release(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_remove(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_remove(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_send_to_hand(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_send_to_hand(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_send_to_grave(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_send_to_grave(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_player_can_send_to_deck(lua_State * L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(playerid != 0 && playerid != 1) {
		lua_pushboolean(L, 0);
		return 1;
	}
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_player_can_send_to_deck(playerid, pcard));
	return 1;
}
int32 scriptlib::duel_is_chain_negatable(lua_State * L) {
	check_param_count(L, 1);
	int32 chaincount = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_chain_negatable(chaincount, TRUE));
	return 1;
}
int32 scriptlib::duel_is_chain_disablable(lua_State * L) {
	check_param_count(L, 1);
	int32 chaincount = lua_tointeger(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_chain_disablable(chaincount, TRUE));
	return 1;
}
int32 scriptlib::duel_check_chain_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	int32 chaincount = lua_tointeger(L, 1);
	card* pcard = *(card**) lua_touserdata(L, 2);
	lua_pushboolean(L, pcard->pduel->game_field->check_chain_target(chaincount, pcard));
	return 1;
}
int32 scriptlib::duel_check_chain_uniqueness(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->game_field->core.current_chain.size() == 0) {
		lua_pushboolean(L, 1);
		return 1;
	}
	std::set<uint32> er;
	for(auto cait = pduel->game_field->core.current_chain.begin(); cait != pduel->game_field->core.current_chain.end(); ++cait)
		er.insert(cait->triggering_effect->handler->get_code());
	if(er.size() == pduel->game_field->core.current_chain.size())
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::duel_get_activity_count(lua_State *L) {
	check_param_count(L, 2);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	int32 retct = lua_gettop(L) - 1;
	for(int32 i = 0; i < retct; ++i) {
		int32 activity_type = lua_tointeger(L, 2 + i);
		switch(activity_type) {
			case 1:
				lua_pushinteger(L, pduel->game_field->core.summon_state_count[playerid]);
				break;
			case 2:
				lua_pushinteger(L, pduel->game_field->core.normalsummon_state_count[playerid]);
				break;
			case 3:
				lua_pushinteger(L, pduel->game_field->core.spsummon_state_count[playerid]);
				break;
			case 4:
				lua_pushinteger(L, pduel->game_field->core.flipsummon_state_count[playerid]);
				break;
			case 5:
				lua_pushinteger(L, pduel->game_field->core.attack_state_count[playerid]);
				break;
			case 6:
				lua_pushinteger(L, pduel->game_field->core.battle_phase_count[playerid]);
				break;
			default:
				lua_pushinteger(L, 0);
				break;
		}
	}
	return retct;
}
int32 scriptlib::duel_check_phase_activity(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->core.phase_action);
	return 1;
}
int32 scriptlib::duel_add_custom_activity_counter(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_FUNCTION, 3);
	int32 counter_id = lua_tointeger(L, 1);
	int32 activity_type = lua_tointeger(L, 2);
	int32 counter_filter = interpreter::get_function_handle(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	switch(activity_type) {
		case 1: {
			auto iter = pduel->game_field->core.summon_counter.find(counter_id);
			if(iter != pduel->game_field->core.summon_counter.end())
				break;
			pduel->game_field->core.summon_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		case 2: {
			auto iter = pduel->game_field->core.normalsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.normalsummon_counter.end())
				break;
			pduel->game_field->core.normalsummon_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		case 3: {
			auto iter = pduel->game_field->core.spsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.spsummon_counter.end())
				break;
			pduel->game_field->core.spsummon_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		case 4: {
			auto iter = pduel->game_field->core.flipsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.flipsummon_counter.end())
				break;
			pduel->game_field->core.flipsummon_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		case 5: {
			auto iter = pduel->game_field->core.attack_counter.find(counter_id);
			if(iter != pduel->game_field->core.attack_counter.end())
				break;
			pduel->game_field->core.attack_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		case 6: break;
		case 7: {
			auto iter = pduel->game_field->core.chain_counter.find(counter_id);
			if(iter != pduel->game_field->core.chain_counter.end())
				break;
			pduel->game_field->core.chain_counter[counter_id] = std::make_pair(counter_filter, 0);
			break;
		}
		default:
			break;
	}
	return 0;
}
int32 scriptlib::duel_get_custom_activity_count(lua_State *L) {
	check_param_count(L, 3);
	int32 counter_id = lua_tointeger(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	int32 activity_type = lua_tointeger(L, 3);
	duel* pduel = interpreter::get_duel_info(L);
	int32 val = 0;
	switch(activity_type) {
		case 1: {
			auto iter = pduel->game_field->core.summon_counter.find(counter_id);
			if(iter != pduel->game_field->core.summon_counter.end())
				val = iter->second.second;
			break;
		}
		case 2: {
			auto iter = pduel->game_field->core.normalsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.normalsummon_counter.end())
				val = iter->second.second;
			break;
		}
		case 3: {
			auto iter = pduel->game_field->core.spsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.spsummon_counter.end())
				val = iter->second.second;
			break;
		}
		case 4: {
			auto iter = pduel->game_field->core.flipsummon_counter.find(counter_id);
			if(iter != pduel->game_field->core.flipsummon_counter.end())
				val = iter->second.second;
			break;
		}
		case 5: {
			auto iter = pduel->game_field->core.attack_counter.find(counter_id);
			if(iter != pduel->game_field->core.attack_counter.end())
				val = iter->second.second;
			break;
		}
		case 6:
			break;
		case 7: {
			auto iter = pduel->game_field->core.chain_counter.find(counter_id);
			if(iter != pduel->game_field->core.chain_counter.end())
				val = iter->second.second;
			break;
		}
		default:
			break;
	}
	if(playerid == 0)
		lua_pushinteger(L, val & 0xffff);
	else
		lua_pushinteger(L, (val >> 16) & 0xffff);
	return 1;
}
int32 scriptlib::duel_is_able_to_enter_bp(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_pushboolean(L, pduel->game_field->is_able_to_enter_bp());
	return 1;
}
int32 scriptlib::duel_venom_swamp_check(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(pcard->get_counter(0x9) == 0 || pcard->is_affected_by_effect(EFFECT_SWAP_AD) || pcard->is_affected_by_effect(EFFECT_REVERSE_UPDATE)) {
		lua_pushboolean(L, 0);
		return 1;
	}
	uint32 base = pcard->get_base_attack();
	pcard->temp.base_attack = base;
	pcard->temp.attack = base;
	int32 up = 0, upc = 0;
	effect_set eset;
	effect* peffect = 0;
	pcard->filter_effect(EFFECT_UPDATE_ATTACK, &eset, FALSE);
	pcard->filter_effect(EFFECT_SET_ATTACK, &eset, FALSE);
	pcard->filter_effect(EFFECT_SET_ATTACK_FINAL, &eset);
	for (int32 i = 0; i < eset.size(); ++i) {
		switch (eset[i]->code) {
		case EFFECT_UPDATE_ATTACK: {
			if (eset[i]->type & EFFECT_TYPE_SINGLE && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up += eset[i]->get_value(pcard);
			else
				upc += eset[i]->get_value(pcard);
			if(pcard->temp.attack > 0)
				peffect = eset[i];
			break;
		}
		case EFFECT_SET_ATTACK:
			base = eset[i]->get_value(pcard);
			if (eset[i]->type & EFFECT_TYPE_SINGLE && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE))
				up = 0;
			break;
		case EFFECT_SET_ATTACK_FINAL:
			if (eset[i]->type & EFFECT_TYPE_SINGLE && !(eset[i]->flag & EFFECT_FLAG_SINGLE_RANGE)) {
				base = eset[i]->get_value(pcard);
				up = 0;
				upc = 0;
				peffect = 0;
			}
			break;
		}
		pcard->temp.attack = base + up + upc;
	}
	int32 atk = pcard->temp.attack;
	pcard->temp.base_attack = 0xffffffff;
	pcard->temp.attack = 0xffffffff;
	if((atk <= 0) && peffect && (peffect->handler->get_code() == 54306223))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::duel_swap_deck_and_grave(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 1);
	int32 playerid = lua_tointeger(L, 1);
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->swap_deck_and_grave(playerid);
	return 0;
}
int32 scriptlib::duel_majestic_copy(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* pcard = *(card**) lua_touserdata(L, 1);
	card* ccard = *(card**) lua_touserdata(L, 2);
	duel* pduel = pcard->pduel;
	card::effect_container::iterator eit;
	for(eit = ccard->single_effect.begin(); eit != ccard->field_effect.end(); ++eit) {
		if(eit == ccard->single_effect.end()) {
			eit = ccard->field_effect.begin();
			if(eit == ccard->field_effect.end())
				break;
		}
		effect* peffect = eit->second;
		if(!(peffect->type & 0x7c)) continue;
		if(!(peffect->flag & EFFECT_FLAG_INITIAL)) continue;
		effect* ceffect = pduel->new_effect();
		int32 ref = ceffect->ref_handle;
		*ceffect = *peffect;
		ceffect->ref_handle = ref;
		ceffect->owner = pcard;
		ceffect->handler = 0;
		ceffect->flag &= ~EFFECT_FLAG_INITIAL;
		ceffect->effect_owner = PLAYER_NONE;
		if(peffect->condition) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, peffect->condition);
			ceffect->condition = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		if(peffect->cost) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, peffect->cost);
			ceffect->cost = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		if(peffect->target) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, peffect->target);
			ceffect->target = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		if(peffect->operation) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, peffect->operation);
			ceffect->operation = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		ceffect->reset_flag = RESET_EVENT + 0x1fe0000 + RESET_PHASE + PHASE_END + RESET_SELF_TURN + RESET_OPPO_TURN;
		ceffect->reset_count = (ceffect->reset_count & 0xff00) | 0x1;
		ceffect->recharge();
		if(ceffect->type & EFFECT_TYPE_TRIGGER_F) {
			ceffect->type &= ~EFFECT_TYPE_TRIGGER_F;
			ceffect->type |= EFFECT_TYPE_TRIGGER_O;
			ceffect->flag |= EFFECT_FLAG_DELAY;
		}
		if(ceffect->type & EFFECT_TYPE_QUICK_F) {
			ceffect->type &= ~EFFECT_TYPE_QUICK_F;
			ceffect->type |= EFFECT_TYPE_QUICK_O;
		}
		pcard->add_effect(ceffect);
	}
	return 0;
}
