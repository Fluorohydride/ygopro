/*
 * libgroup.cpp
 *
 *  Created on: 2010-5-6
 *      Author: Argon
 */

#include "scriptlib.h"
#include "group.h"
#include "card.h"
#include "effect.h"
#include "duel.h"

int32 scriptlib::group_new(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::group_clone(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	group* newgroup = pduel->new_group(pgroup->container);
	interpreter::group2value(L, newgroup);
	return 1;
}
int32 scriptlib::group_from_cards(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = pduel->new_group();
	card* pcard;
	void* p;
	for(int32 i = 0; i < lua_gettop(L); ++i) {
		p = lua_touserdata(L, i + 1);
		if(p) {
			pcard = *(card**)p;
			pgroup->container.insert(pcard);
		}
	}
	interpreter::group2value(L, pgroup);
	return 1;
}
int32 scriptlib::group_delete(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	if(pgroup->is_readonly != 2)
		return 0;
	pgroup->is_readonly = 0;
	pduel->sgroups.insert(pgroup);
	return 0;
}
int32 scriptlib::group_keep_alive(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	if(pgroup->is_readonly == 1)
		return 0;
	pgroup->is_readonly = 2;
	pduel->sgroups.erase(pgroup);
	return 0;
}
int32 scriptlib::group_clear(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	if (pgroup->is_readonly != 1) {
		pgroup->container.clear();
	}
	return 0;
}
int32 scriptlib::group_add_card(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if (pgroup->is_readonly != 1) {
		pgroup->container.insert(pcard);
	}
	return 0;
}
int32 scriptlib::group_remove_card(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if (pgroup->is_readonly != 1) {
		pgroup->container.erase(pcard);
	}
	return 0;
}
int32 scriptlib::group_get_next(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	if(pgroup->it == pgroup->container.end())
		lua_pushnil(L);
	else {
		++pgroup->it;
		if (pgroup->it == pgroup->container.end())
			lua_pushnil(L);
		else
			interpreter::card2value(L, (*(pgroup->it)));
	}
	return 1;
}
int32 scriptlib::group_get_first(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	if (pgroup->container.size()) {
		pgroup->it = pgroup->container.begin();
		interpreter::card2value(L, (*(pgroup->it)));
	} else
		lua_pushnil(L);
	return 1;
}
int32 scriptlib::group_get_count(lua_State *L) {
	check_param_count(L, 1);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	lua_pushinteger(L, pgroup->container.size());
	return 1;
}
int32 scriptlib::group_for_each(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	duel* pduel = interpreter::get_duel_info(L);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	int32 f = interpreter::get_function_handle(L, 2);
	group::card_set::iterator it;
	for (it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		pduel->lua->add_param((*it), PARAM_TYPE_CARD);
		pduel->lua->call_function(f, 1, 0);
	}
	return 0;
}
int32 scriptlib::group_filter(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_CARD, 3);
		pexception = *(card**) lua_touserdata(L, 3);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	group* new_group = pduel->new_group();
	uint32 extraargs = lua_gettop(L) - 3;
	group::card_set::iterator it;
	for (it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		if((*it) != pexception && pduel->lua->check_matching(*it, 2, extraargs)) {
			new_group->container.insert(*it);
		}
	}
	interpreter::group2value(L, new_group);
	return 1;
}
int32 scriptlib::group_filter_count(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_CARD, 3);
		pexception = *(card**) lua_touserdata(L, 3);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 extraargs = lua_gettop(L) - 3;
	uint32 count = 0;
	group::card_set::iterator it;
	for (it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		if((*it) != pexception && pduel->lua->check_matching(*it, 2, extraargs))
			count++;
	}
	lua_pushinteger(L, count);
	return 1;
}
int32 scriptlib::group_filter_select(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 3);
	card* pexception = 0;
	if(!lua_isnil(L, 6)) {
		check_param(L, PARAM_TYPE_CARD, 6);
		pexception = *(card**) lua_touserdata(L, 6);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 min = lua_tointeger(L, 4);
	uint32 max = lua_tointeger(L, 5);
	uint32 extraargs = lua_gettop(L) - 6;
	pduel->game_field->core.select_cards.clear();
	group::card_set::iterator it;
	for (it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		if((*it) != pexception && pduel->lua->check_matching(*it, 3, extraargs))
			pduel->game_field->core.select_cards.push_back(*it);
	}
	pduel->game_field->add_process(PROCESSOR_SELECT_CARD_S, 0, 0, 0, playerid, min + (max << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::group_select(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_GROUP, 1);
	card* pexception = 0;
	if(!lua_isnil(L, 5)) {
		check_param(L, PARAM_TYPE_CARD, 5);
		pexception = *(card**) lua_touserdata(L, 5);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	uint32 min = lua_tointeger(L, 3);
	uint32 max = lua_tointeger(L, 4);
	pduel->game_field->core.select_cards.clear();
	group::card_set::iterator it;
	for (it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		if((*it) != pexception)
			pduel->game_field->core.select_cards.push_back(*it);
	}
	pduel->game_field->add_process(PROCESSOR_SELECT_CARD_S, 0, 0, 0, playerid, min + (max << 16));
	return lua_yield(L, 0);
}
int32 scriptlib::group_random_select(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_GROUP, 1);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	int32 playerid = lua_tointeger(L, 2);
	int32 count = lua_tointeger(L, 3);
	pgroup->pduel->game_field->add_process(PROCESSOR_RANDOM_SELECT_S, 0, 0, pgroup, playerid, count);
	return lua_yield(L, 0);
}
int32 scriptlib::group_is_exists(lua_State *L) {
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pcard = 0;
	if(!lua_isnil(L, 4)) {
		check_param(L, PARAM_TYPE_CARD, 4);
		pcard = *(card**) lua_touserdata(L, 4);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 count = lua_tointeger(L, 3);
	uint32 extraargs = lua_gettop(L) - 4;
	uint32 fcount = 0;
	uint32 result = FALSE;
	for (auto it = pgroup->container.begin(); it != pgroup->container.end(); ++it) {
		if((*it) != pcard && pduel->lua->check_matching(*it, 2, extraargs)) {
			fcount++;
			if(fcount >= count) {
				result = TRUE;
				break;
			}
		}
	}
	lua_pushboolean(L, result);
	return 1;
}
int32 scriptlib::group_check_with_sum_equal(lua_State *L) {
	check_param_count(L, 5);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 acc = lua_tointeger(L, 3);
	int32 min = lua_tointeger(L, 4);
	int32 max = lua_tointeger(L, 5);
	if(min <= 0)
		min = 1;
	if(max < min)
		max = min;
	int32 extraargs = lua_gettop(L) - 5;
	field::card_vector cv;
	for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
		(*cit)->operation_param = pduel->lua->get_operation_value(*cit, 2, extraargs);
		cv.push_back(*cit);
	}
	lua_pushboolean(L, pduel->game_field->check_with_sum_limit(&cv, acc, 0, 1, min, max));
	return 1;
}
int32 scriptlib::group_select_with_sum_equal(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 6);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 3);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 acc = lua_tointeger(L, 4);
	int32 min = lua_tointeger(L, 5);
	int32 max = lua_tointeger(L, 6);
	if(min <= 0)
		min = 1;
	if(max < min)
		max = min;
	int32 extraargs = lua_gettop(L) - 6;
	pduel->game_field->core.select_cards.clear();
	for(auto cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
		(*cit)->operation_param = pduel->lua->get_operation_value(*cit, 3, extraargs);
		pduel->game_field->core.select_cards.push_back(*cit);
	}
	if(!pduel->game_field->check_with_sum_limit(&pduel->game_field->core.select_cards, acc, 0, 1, min, max))
		return 0;
	pduel->game_field->add_process(PROCESSOR_SELECT_SUM_S, 0, 0, 0, acc, playerid + (min << 16) + (max << 24));
	return lua_yield(L, 0);
}
int32 scriptlib::group_check_with_sum_greater(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 acc = lua_tointeger(L, 3);
	int32 extraargs = lua_gettop(L) - 3;
	int32 sum = 0, op, op1, op2;
	group::card_set::iterator cit;
	for(cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
		op = pduel->lua->get_operation_value(*cit, 2, extraargs);
		op1 = op & 0xffff;
		op2 = op >> 16;
		sum += op1 > op2 ? op1 : op2;
	}
	if(sum >= acc)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::group_select_with_sum_greater(lua_State *L) {
	check_action_permission(L);
	check_param_count(L, 4);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 3);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 playerid = lua_tointeger(L, 2);
	if(playerid != 0 && playerid != 1)
		return 0;
	int32 acc = lua_tointeger(L, 4);
	int32 extraargs = lua_gettop(L) - 4;
	field::card_set::iterator cit;
	pduel->game_field->core.select_cards.clear();
	int32 sum = 0, op, op1, op2;
	for(cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit) {
		op = pduel->lua->get_operation_value(*cit, 3, extraargs);
		op1 = op & 0xffff;
		op2 = op >> 16;
		sum += op1 > op2 ? op1 : op2;
		(*cit)->operation_param = op;
		pduel->game_field->core.select_cards.push_back(*cit);
	}
	if(sum < acc)
		return 0;
	pduel->game_field->add_process(PROCESSOR_SELECT_SUM_S, 0, 0, 0, acc, playerid);
	return lua_yield(L, 0);
}
int32 scriptlib::group_get_min_group(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	if(pgroup->container.size() == 0)
		return 0;
	group* newgroup = pduel->new_group();
	int32 min, op;
	int32 extraargs = lua_gettop(L) - 2;
	field::card_set::iterator cit = pgroup->container.begin();
	min = pduel->lua->get_operation_value(*cit, 2, extraargs);
	newgroup->container.insert(*cit);
	++cit;
	for(; cit != pgroup->container.end(); ++cit) {
		op = pduel->lua->get_operation_value(*cit, 2, extraargs);
		if(op == min)
			newgroup->container.insert(*cit);
		else if(op < min) {
			newgroup->container.clear();
			newgroup->container.insert(*cit);
			min = op;
		}
	}
	interpreter::group2value(L, newgroup);
	lua_pushinteger(L, min);
	return 2;
}
int32 scriptlib::group_get_max_group(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	if(pgroup->container.size() == 0)
		return 0;
	group* newgroup = pduel->new_group();
	int32 max, op;
	int32 extraargs = lua_gettop(L) - 2;
	field::card_set::iterator cit = pgroup->container.begin();
	max = pduel->lua->get_operation_value(*cit, 2, extraargs);
	newgroup->container.insert(*cit);
	++cit;
	for(; cit != pgroup->container.end(); ++cit) {
		op = pduel->lua->get_operation_value(*cit, 2, extraargs);
		if(op == max)
			newgroup->container.insert(*cit);
		else if(op > max) {
			newgroup->container.clear();
			newgroup->container.insert(*cit);
			max = op;
		}
	}
	interpreter::group2value(L, newgroup);
	lua_pushinteger(L, max);
	return 2;
}
int32 scriptlib::group_get_sum(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 extraargs = lua_gettop(L) - 2;
	int32 sum = 0;
	field::card_set::iterator cit = pgroup->container.begin();
	for(; cit != pgroup->container.end(); ++cit) {
		sum += pduel->lua->get_operation_value(*cit, 2, extraargs);
	}
	lua_pushinteger(L, sum);
	return 1;
}
int32 scriptlib::group_get_class_count(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	int32 extraargs = lua_gettop(L) - 2;
	std::set<uint32> er;
	field::card_set::iterator cit = pgroup->container.begin();
	for(; cit != pgroup->container.end(); ++cit) {
		er.insert(pduel->lua->get_operation_value(*cit, 2, extraargs));
	}
	lua_pushinteger(L, er.size());
	return 1;
}
int32 scriptlib::group_remove(lua_State *L) {
	check_param_count(L, 3);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	card* pexception = 0;
	if(!lua_isnil(L, 3)) {
		check_param(L, PARAM_TYPE_CARD, 3);
		pexception = *(card**) lua_touserdata(L, 3);
	}
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 extraargs = lua_gettop(L) - 3;
	if(pgroup->is_readonly == 1)
		return 0;
	group::card_set::iterator cit, rm;
	for (cit = pgroup->container.begin(); cit != pgroup->container.end();) {
		rm = cit++;
		if((*rm) != pexception && pduel->lua->check_matching(*rm, 2, extraargs)) {
			pgroup->container.erase(rm);
		}
	}
	return 0;
}
int32 scriptlib::group_merge(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_GROUP, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	group* mgroup = *(group**) lua_touserdata(L, 2);
	if(pgroup->is_readonly == 1)
		return 0;
	pgroup->container.insert(mgroup->container.begin(), mgroup->container.end());
	return 0;
}
int32 scriptlib::group_sub(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_GROUP, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	group* sgroup = *(group**) lua_touserdata(L, 2);
	if(pgroup->is_readonly == 1)
		return 0;
	group::card_set::iterator cit;
	for (cit = sgroup->container.begin(); cit != sgroup->container.end(); ++cit) {
		pgroup->container.erase(*cit);
	}
	return 0;
}
int32 scriptlib::group_equal(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_GROUP, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	group* sgroup = *(group**) lua_touserdata(L, 2);
	if(pgroup->container.size() != sgroup->container.size()) {
		lua_pushboolean(L, 0);
		return 1;
	}
	auto pit = pgroup->container.begin();
	auto sit = sgroup->container.begin();
	for (; pit != pgroup->container.end(); ++pit, ++sit) {
		if((*pit) != (*sit)) {
			lua_pushboolean(L, 0);
			return 1;
		}
	}
	lua_pushboolean(L, 1);
	return 1;
}
int32 scriptlib::group_is_contains(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_CARD, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	card* pcard = *(card**) lua_touserdata(L, 2);
	if(pgroup->container.find(pcard) != pgroup->container.end())
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
	return 1;
}
int32 scriptlib::group_search_card(lua_State *L) {
	check_param_count(L, 2);
	check_param(L, PARAM_TYPE_GROUP, 1);
	check_param(L, PARAM_TYPE_FUNCTION, 2);
	group* pgroup = *(group**) lua_touserdata(L, 1);
	duel* pduel = pgroup->pduel;
	uint32 extraargs = lua_gettop(L) - 2;
	field::card_set::iterator cit;
	for(cit = pgroup->container.begin(); cit != pgroup->container.end(); ++cit)
		if(pduel->lua->check_matching(*cit, 2, extraargs)) {
			interpreter::card2value(L, *cit);
			return 1;
		}
	return 0;
}
