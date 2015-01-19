/*
 * interpreter.h
 *
 *  Created on: 2010-4-28
 *      Author: Argon
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

extern "C" {
#ifdef WIN32
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#else
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#endif
}
#include "common.h"
#include <unordered_map>
#include <list>

class card;
class effect;
class group;
class duel;

class interpreter {
public:
	typedef std::unordered_map<int32, lua_State*> coroutine_map;
	typedef std::list<std::pair<void*, uint32> > param_list;
	
	duel* pduel;
	char msgbuf[64];
	lua_State* lua_state;
	lua_State* current_state;
	param_list params;
	param_list resumes;
	coroutine_map coroutines;
	int32 no_action;
	int32 call_depth;

	explicit interpreter(duel* pd);
	~interpreter();

	int32 register_card(card *pcard);
	void register_effect(effect* peffect);
	void unregister_effect(effect* peffect);
	void register_group(group* pgroup);
	void unregister_group(group* pgroup);
	
	int32 load_script(char* buffer);
	int32 load_card_script(uint32 code);
	void add_param(void* param, int32 type, bool front = false);
	void add_param(ptr param, int32 type, bool front = false);
	void push_param(lua_State* L, bool is_coroutine = false);
	int32 call_function(int32 f, uint32 param_count, uint32 ret_count);
	int32 call_card_function(card *pcard, char *f, uint32 param_count, uint32 ret_count);
	int32 call_code_function(uint32 code, char *f, uint32 param_count, uint32 ret_count);
	int32 check_condition(int32 f, uint32 param_count);
	int32 check_matching(card* pcard, int32 findex, int32 extraargs);
	int32 get_operation_value(card* pcard, int32 findex, int32 extraargs);
	int32 get_function_value(int32 f, uint32 param_count);
	int32 call_coroutine(int32 f, uint32 param_count, uint32* yield_value, uint16 step);

	static void card2value(lua_State* L, card* pcard);
	static void group2value(lua_State* L, group* pgroup);
	static void effect2value(lua_State* L, effect* peffect);
	static void function2value(lua_State* L, int32 pointer);
	static int32 get_function_handle(lua_State* L, int32 index);
	static void set_duel_info(lua_State* L, duel* pduel);
	static duel* get_duel_info(lua_State* L);
};

#define	PARAM_TYPE_INT		0x01
#define	PARAM_TYPE_STRING	0x02
#define	PARAM_TYPE_CARD		0x04
#define	PARAM_TYPE_GROUP	0x08
#define PARAM_TYPE_EFFECT	0x10
#define	PARAM_TYPE_FUNCTION	0x20
#define PARAM_TYPE_BOOLEAN	0x40
#define PARAM_TYPE_INDEX	0x80

#define COROUTINE_FINISH	1
#define COROUTINE_YIELD		2
#define COROUTINE_ERROR		3

#endif /* INTERPRETER_H_ */
