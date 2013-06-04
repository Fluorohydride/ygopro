/*
 * duel.h
 *
 *  Created on: 2010-4-8
 *      Author: Argon
 */

#ifndef DUEL_H_
#define DUEL_H_

#include "common.h"
#include "mtrandom.h"
#include <set>

class card;
class group;
class effect;
class field;
class interpreter;
class ygoAdapter;

struct duel_arg {
	int16 start_lp;
	int8 start_hand;
	int8 draw_count;
};

class duel {
public:
	char strbuffer[256];
	byte buffer[0x1000];
	uint32 bufferlen;
	byte* bufferp;
	interpreter* lua;
	field* game_field;
	mtrandom random;
	std::set<card*> cards;
	std::set<group*> groups;
	std::set<group*> sgroups;
	std::set<effect*> effects;
	std::set<effect*> uncopy;
	ygoAdapter* adapter;

	duel();
	~duel();
	void clear();
	
	card* new_card(uint32 code);
	group* new_group(card* pcard = 0);
	effect* new_effect();
	void delete_card(card* pcard);
	void delete_group(group* pgroup);
	void delete_effect(effect* peffect);
	void release_script_group();
	int32 read_buffer(byte* buf);
	void write_buffer32(uint32 value);
	void write_buffer16(uint16 value);
	void write_buffer8(uint8 value);
	void rollback_buffer(uint32 len);
	void clear_buffer();
	void set_responsei(uint32 resp);
	void set_responseb(byte* resp);
	int32 get_next_integer(int32 l, int32 h);
};

#endif /* DUEL_H_ */
