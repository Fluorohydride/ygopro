/*
 * libdebug.cpp
 *
 *  Created on: 2012-2-8
 *      Author: Argon
 */

#include <string.h>
#include "scriptlib.h"
#include "duel.h"
#include "field.h"
#include "card.h"
#include "effect.h"
#include "ocgapi.h"

int32 scriptlib::debug_message(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_getglobal(L, "tostring");
	lua_pushvalue(L, -2);
	lua_pcall(L, 1, 1, 0);
	sprintf(pduel->strbuffer, "%s", lua_tostring(L, -1));
	handle_message(pduel, 2);
	return 0;
}
int32 scriptlib::debug_add_card(lua_State *L) {
	check_param_count(L, 6);
	duel* pduel = interpreter::get_duel_info(L);
	int32 code = lua_tointeger(L, 1);
	int32 owner = lua_tointeger(L, 2);
	int32 playerid = lua_tointeger(L, 3);
	int32 location = lua_tointeger(L, 4);
	int32 sequence = lua_tointeger(L, 5);
	int32 position = lua_tointeger(L, 6);
	int32 proc = lua_toboolean(L, 7);
	if(owner != 0 && owner != 1)
		return 0;
	if(playerid != 0 && playerid != 1)
		return 0;
	if(pduel->game_field->is_location_useable(playerid, location, sequence)) {
		card* pcard = pduel->new_card(code);
		pcard->owner = owner;
		pduel->game_field->add_card(playerid, pcard, location, sequence);
		pcard->current.position = position;
		if(!(location & LOCATION_ONFIELD) || (position & POS_FACEUP)) {
			pcard->enable_field_effect(TRUE);
			pduel->game_field->adjust_instant();
		}
		if((pcard->data.type & TYPE_PENDULUM) && (location == LOCATION_EXTRA) && (position & POS_FACEUP))
			pduel->game_field->player[playerid].extra_p_count += 1;
		if(proc)
			pcard->set_status(STATUS_PROC_COMPLETE, TRUE);
		interpreter::card2value(L, pcard);
		return 1;
	} else if(location == LOCATION_MZONE) {
		card* pcard = pduel->new_card(code);
		pcard->owner = owner;
		card* fcard = pduel->game_field->get_field_card(playerid, location, sequence);
		fcard->xyz_materials.push_back(pcard);
		pcard->overlay_target = fcard;
		pcard->current.controler = PLAYER_NONE;
		pcard->current.location = LOCATION_OVERLAY;
		pcard->current.sequence = fcard->xyz_materials.size() - 1;
		interpreter::card2value(L, pcard);
		return 1;
	}
	return 0;
}
int32 scriptlib::debug_set_player_info(lua_State *L) {
	check_param_count(L, 4);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 lp = lua_tointeger(L, 2);
	uint32 startcount = lua_tointeger(L, 3);
	uint32 drawcount = lua_tointeger(L, 4);
	if(playerid != 0 && playerid != 1)
		return 0;
	pduel->game_field->player[playerid].lp = lp;
	pduel->game_field->player[playerid].start_count = startcount;
	pduel->game_field->player[playerid].draw_count = drawcount;
	return 0;
}
int32 scriptlib::debug_pre_equip(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* equip_card = *(card**) lua_touserdata(L, 1);
	card* target = *(card**) lua_touserdata(L, 2);
	if((equip_card->current.location != LOCATION_SZONE)
	        || (target->current.location != LOCATION_MZONE)
	        || (target->current.position & POS_FACEDOWN))
		lua_pushboolean(L, 0);
	else {
		equip_card->equip(target, FALSE);
		equip_card->effect_target_cards.insert(target);
		target->effect_target_owner.insert(equip_card);
		lua_pushboolean(L, 1);
	}
	return 1;
}
int32 scriptlib::debug_pre_set_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	card* t_card = *(card**) lua_touserdata(L, 1);
	card* target = *(card**) lua_touserdata(L, 2);
	t_card->add_card_target(target);
	return 0;
}
int32 scriptlib::debug_pre_add_counter(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	uint32 ctype = lua_tointeger(L, 2);
	uint32 ccount = lua_tointeger(L, 3);
	pcard->counters[ctype] += ccount;
	return 0;
}
int32 scriptlib::debug_reload_field_begin(lua_State *L) {
	check_param_count(L, 1);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 flag = lua_tointeger(L, 1);
	pduel->clear();
	pduel->game_field->core.duel_options = flag;
	return 0;
}
int32 scriptlib::debug_reload_field_end(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	pduel->game_field->core.shuffle_hand_check[0] = FALSE;
	pduel->game_field->core.shuffle_hand_check[1] = FALSE;
	pduel->game_field->core.shuffle_deck_check[0] = FALSE;
	pduel->game_field->core.shuffle_deck_check[1] = FALSE;
	pduel->game_field->reload_field_info();
	return 0;
}
int32 scriptlib::debug_set_ai_name(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_STRING, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->write_buffer8(MSG_AI_NAME);
	const char* pstr = lua_tostring(L, 1);
	int len = strlen(pstr);
	if(len > 100)
		len = 100;
	pduel->write_buffer16(len);
	memcpy(pduel->bufferp, pstr, len);
	pduel->bufferp += len;
	pduel->bufferlen += len;
	pduel->write_buffer8(0);
	return 0;
}
int32 scriptlib::debug_show_hint(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_STRING, 1);
	duel* pduel = interpreter::get_duel_info(L);
	pduel->write_buffer8(MSG_SHOW_HINT);
	const char* pstr = lua_tostring(L, 1);
	int len = strlen(pstr);
	if(len > 1024)
		len = 1024;
	pduel->write_buffer16(len);
	memcpy(pduel->bufferp, pstr, len);
	pduel->bufferp += len;
	pduel->bufferlen += len;
	pduel->write_buffer8(0);
	return 0;
}
