/*
 * libeffect.cpp
 *
 *  Created on: 2010-7-20
 *      Author: Argon
 */

#include "scriptlib.h"
#include "duel.h"
#include "field.h"
#include "card.h"
#include "effect.h"

int32 scriptlib::effect_new(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_CARD, 1);
	card* pcard = *(card**) lua_touserdata(L, 1);
	duel* pduel = pcard->pduel;
	effect* peffect = pduel->new_effect();
	peffect->effect_owner = pduel->game_field->core.reason_player;
	peffect->owner = pcard;
	interpreter::effect2value(L, peffect);
	return 1;
}
int32 scriptlib::effect_newex(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	effect* peffect = pduel->new_effect();
	peffect->effect_owner = 0;
	peffect->owner = pduel->game_field->temp_card;
	interpreter::effect2value(L, peffect);
	return 1;
}
int32 scriptlib::effect_clone(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	duel* pduel = peffect->pduel;
	effect* ceffect = pduel->new_effect();
	int32 ref = ceffect->ref_handle;
	*ceffect = *peffect;
	ceffect->ref_handle = ref;
	ceffect->handler = 0;
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
	if(peffect->value && (peffect->flag & EFFECT_FLAG_FUNC_VALUE)) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, peffect->value);
		ceffect->value = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	interpreter::effect2value(L, ceffect);
	return 1;
}
int32 scriptlib::effect_reset(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->owner == 0)
		return 0;
	if(peffect->flag & EFFECT_FLAG_FIELD_ONLY)
		peffect->pduel->game_field->remove_effect(peffect);
	else
		peffect->handler->remove_effect(peffect);
	return 0;
}
int32 scriptlib::effect_get_field_id(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	lua_pushinteger(L, peffect->id);
	return 1;
}
int32 scriptlib::effect_set_description(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->description = v;
	return 0;
}
int32 scriptlib::effect_set_code(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->code = v;
	return 0;
}
int32 scriptlib::effect_set_range(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->range = v;
	return 0;
}
int32 scriptlib::effect_set_target_range(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 s = lua_tointeger(L, 2);
	int32 o = lua_tointeger(L, 3);
	peffect->s_range = s;
	peffect->o_range = o;
	return 0;
}
int32 scriptlib::effect_set_absolute_range(lua_State *L) {
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	int32 s = lua_tointeger(L, 3);
	int32 o = lua_tointeger(L, 4);
	if(playerid == 0) {
		peffect->s_range = s;
		peffect->o_range = o;
	} else {
		peffect->s_range = o;
		peffect->o_range = s;
	}
	peffect->flag |= EFFECT_FLAG_ABSOLUTE_TARGET;
	return 0;
}
int32 scriptlib::effect_set_count_limit(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	uint32 code = 0;
	if(lua_gettop(L) >= 3)
		code = lua_tointeger(L, 3);
	if(v == 0)
		v = 1;
	peffect->flag |= EFFECT_FLAG_COUNT_LIMIT;
	peffect->reset_count |= ((v << 12) & 0xf000) | ((v << 8) & 0xf00);
	peffect->count_code = code;
	return 0;
}
int32 scriptlib::effect_set_reset(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	int32 c = lua_tointeger(L, 3);
	if(c == 0)
		c = 1;
	if(v & (RESET_PHASE) && !(v & (RESET_SELF_TURN | RESET_OPPO_TURN)))
		v |= (RESET_SELF_TURN | RESET_OPPO_TURN);
	peffect->reset_flag = v;
	peffect->reset_count = (peffect->reset_count & 0xff00) | (c & 0xff);
	return 0;
}
int32 scriptlib::effect_set_type(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	if (v & 0x0ff0)
		v |= EFFECT_TYPE_ACTIONS;
	else
		v &= ~EFFECT_TYPE_ACTIONS;
	if(v & 0x550)
		v |= EFFECT_TYPE_FIELD;
	if(v & EFFECT_TYPE_ACTIVATE)
		peffect->range = LOCATION_SZONE + LOCATION_HAND;
	if(v & EFFECT_TYPE_FLIP)
		peffect->code = EVENT_FLIP;
	peffect->type = v;
	return 0;
}
int32 scriptlib::effect_set_property(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->flag |= v & 0xfffffff0;
	return 0;
}
int32 scriptlib::effect_set_label(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->label = v;
	return 0;
}
int32 scriptlib::effect_set_label_object(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(lua_isnil(L, 2)) {
		peffect->label_object = 0;
		return 0;
	}
	if(!lua_isuserdata(L, 2))
		luaL_error(L, "Parameter 2 should be \"Card\" or \"Effect\" or \"Group\".");
	void* p = *(void**)lua_touserdata(L, 2);
	peffect->label_object = p;
	return 0;
}
int32 scriptlib::effect_set_category(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 v = lua_tointeger(L, 2);
	peffect->category = v;
	return 0;
}
int32 scriptlib::effect_set_hint_timing(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 vs = lua_tointeger(L, 2);
	int32 vo = vs;
	if(lua_gettop(L) >= 3)
		vo = lua_tointeger(L, 3);
	peffect->hint_timing[0] = vs;
	peffect->hint_timing[1] = vo;
	return 0;
}
int32 scriptlib::effect_set_condition(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->condition)
		luaL_unref(L, LUA_REGISTRYINDEX, peffect->condition);
	peffect->condition = interpreter::get_function_handle(L, 2);
	return 0;
}
int32 scriptlib::effect_set_target(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->target)
		luaL_unref(L, LUA_REGISTRYINDEX, peffect->target);
	peffect->target = interpreter::get_function_handle(L, 2);
	return 0;
}
int32 scriptlib::effect_set_cost(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->cost)
		luaL_unref(L, LUA_REGISTRYINDEX, peffect->cost);
	peffect->cost = interpreter::get_function_handle(L, 2);
	return 0;
}
int32 scriptlib::effect_set_value(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->value && (peffect->flag & EFFECT_FLAG_FUNC_VALUE))
		luaL_unref(L, LUA_REGISTRYINDEX, peffect->value);
	if (lua_isfunction(L, 2)) {
		peffect->value = interpreter::get_function_handle(L, 2);
		peffect->flag |= EFFECT_FLAG_FUNC_VALUE;
	} else {
		peffect->flag &= ~EFFECT_FLAG_FUNC_VALUE;
		if(lua_isboolean(L, 2))
			peffect->value = lua_toboolean(L, 2);
		else
			peffect->value = lua_tointeger(L, 2);
	}
	return 0;
}
int32 scriptlib::effect_set_operation(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->operation)
		luaL_unref(L, LUA_REGISTRYINDEX, peffect->operation);
	peffect->operation = interpreter::get_function_handle(L, 2);
	return 0;
}
int32 scriptlib::effect_set_owner_player(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	int32 p = lua_tointeger(L, 2);
	if(p != 0 && p != 1)
		return 0;
	peffect->effect_owner = p;
	return 0;
}
int32 scriptlib::effect_get_description(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->description);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_code(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->code);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_type(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->type);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_property(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->flag);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_label(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->label);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_label_object(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (!peffect->label_object) {
		lua_pushnil(L);
		return 1;
	}
	int32 type = *(int32*)peffect->label_object;
	if(type == 1)
		interpreter::card2value(L, (card*)peffect->label_object);
	else if(type == 2)
		interpreter::group2value(L, (group*)peffect->label_object);
	else if(type == 3)
		interpreter::effect2value(L, (effect*)peffect->label_object);
	else lua_pushnil(L);
	return 1;
}
int32 scriptlib::effect_get_category(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if (peffect) {
		lua_pushinteger(L, peffect->category);
		return 1;
	}
	return 0;
}
int32 scriptlib::effect_get_owner(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::card2value(L, peffect->owner);
	return 1;
}
int32 scriptlib::effect_get_handler(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::card2value(L, peffect->handler);
	return 1;
}
int32 scriptlib::effect_get_owner_player(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	lua_pushinteger(L, peffect->get_owner_player());
	return 1;
}
int32 scriptlib::effect_get_handler_player(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	lua_pushinteger(L, peffect->get_handler_player());
	return 1;
}
int32 scriptlib::effect_get_condition(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::function2value(L, peffect->condition);
	return 1;
}
int32 scriptlib::effect_get_target(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::function2value(L, peffect->target);
	return 1;
}
int32 scriptlib::effect_get_cost(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::function2value(L, peffect->cost);
	return 1;
}
int32 scriptlib::effect_get_value(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if(peffect->flag & EFFECT_FLAG_FUNC_VALUE)
		interpreter::function2value(L, peffect->value);
	else
		lua_pushinteger(L, (int32)peffect->value);
	return 1;
}
int32 scriptlib::effect_get_operation(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	interpreter::function2value(L, peffect->operation);
	return 1;
}
int32 scriptlib::effect_get_active_type(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	if((peffect->type & EFFECT_TYPE_ACTIVATE) && (peffect->handler->data.type & TYPE_PENDULUM))
		lua_pushinteger(L, TYPE_PENDULUM + TYPE_SPELL);
	else if(peffect->type & 0x7f0)
		lua_pushinteger(L, peffect->card_type);
	else
		lua_pushinteger(L, peffect->owner->get_type());
	return 1;
}
int32 scriptlib::effect_is_active_type(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	uint32 tpe = lua_tointeger(L, 2);
	uint32 atype;
	if((peffect->type & EFFECT_TYPE_ACTIVATE) && (peffect->handler->data.type & TYPE_PENDULUM))
		atype = TYPE_PENDULUM + TYPE_SPELL;
	else if(peffect->type & 0x7f0)
		atype = peffect->card_type;
	else
		atype = peffect->owner->get_type();
	lua_pushboolean(L, atype & tpe);
	return 1;
}
int32 scriptlib::effect_is_has_property(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	uint32 tflag = lua_tointeger(L, 2);
	if (peffect && (peffect->flag & tflag))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::effect_is_has_category(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	uint32 tcate = lua_tointeger(L, 2);
	if (peffect && (peffect->category & tcate))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::effect_is_has_type(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	uint32 ttype = lua_tointeger(L, 2);
	if (peffect && (peffect->type & ttype))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::effect_is_activatable(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	uint32 playerid = lua_tointeger(L, 2);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	lua_pushboolean(L, peffect->is_activateable(playerid, peffect->pduel->game_field->nil_event));
	return 1;
}

int32 scriptlib::effect_get_activate_location(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_EFFECT, 1);
	effect* peffect = *(effect**) lua_touserdata(L, 1);
	lua_pushinteger(L, peffect->s_range);
	return 1;
}