/*
 * scriptlib.cpp
 *
 *  Created on: 2010-7-29
 *      Author: Argon
 */
#include "scriptlib.h"
#include "duel.h"

int32 scriptlib::check_param(lua_State* L, int32 param_type, int32 index, int32 retfalse) {
	int32 result;
	switch (param_type) {
	case PARAM_TYPE_CARD:
		if (lua_isuserdata(L, index)) {
			result = **(int32**)lua_touserdata(L, index);
			if(result == 1)
				return TRUE;
		}
		if(retfalse)
			return FALSE;
		luaL_error(L, "Parameter %d should be \"Card\".", index);
		break;
	case PARAM_TYPE_GROUP:
		if (lua_isuserdata(L, index)) {
			result = **(int32**)lua_touserdata(L, index);
			if(result == 2)
				return TRUE;
		}
		if(retfalse)
			return FALSE;
		luaL_error(L, "Parameter %d should be \"Group\".", index);
		break;
	case PARAM_TYPE_EFFECT:
		if (lua_isuserdata(L, index)) {
			result = **(int32**)lua_touserdata(L, index);
			if(result == 3)
				return TRUE;
		}
		if(retfalse)
			return FALSE;
		luaL_error(L, "Parameter %d should be \"Effect\".", index);
		break;
	case PARAM_TYPE_FUNCTION:
		if (lua_isfunction(L, index))
			return TRUE;
		if(retfalse)
			return FALSE;
		luaL_error(L, "Parameter %d should be \"Function\".", index);
		break;
	case PARAM_TYPE_STRING:
		if (lua_isstring(L, index))
			return TRUE;
		if(retfalse)
			return FALSE;
		luaL_error(L, "Parameter %d should be \"String\".", index);
		break;
	}
	return FALSE;
}

int32 scriptlib::check_param_count(lua_State* L, int32 count) {
	if (lua_gettop(L) < count)
		luaL_error(L, "%d Parameters are needed.", count);
	return TRUE;
}
int32 scriptlib::check_action_permission(lua_State* L) {
	duel* pduel = interpreter::get_duel_info(L);
	if(pduel->lua->no_action)
		luaL_error(L, "Action is not allowed here.");
	return TRUE;
}
