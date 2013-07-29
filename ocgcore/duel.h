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
	std::set<card*> assumes;
	std::set<group*> groups;
	std::set<group*> sgroups;
	std::set<effect*> effects;
	std::set<effect*> uncopy;
	
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
	void restore_assumes();
	int32 read_buffer(byte* buf);
	void write_buffer32(uint32 value);
	void write_buffer16(uint16 value);
	void write_buffer8(uint8 value);
	void clear_buffer();
	void set_responsei(uint32 resp);
	void set_responseb(byte* resp);
	int32 get_next_integer(int32 l, int32 h);
};

//Player
#define PLAYER_NONE		2	//
#define PLAYER_ALL		3	//
//Phase
#define PHASE_DRAW			0x01	//
#define PHASE_STANDBY		0x02	//
#define PHASE_MAIN1			0x04	//
#define PHASE_BATTLE		0x08	//
#define PHASE_DAMAGE		0x10	//
#define PHASE_DAMAGE_CAL	0x20	//
#define PHASE_MAIN2			0x40	//
#define PHASE_END			0x80	//
//Options
#define DUEL_TEST_MODE			0x01
#define DUEL_ATTACK_FIRST_TURN	0x02
#define DUEL_NO_CHAIN_HINT		0x04
#define DUEL_ENABLE_PRIORITY	0x08
#define DUEL_PSEUDO_SHUFFLE		0x10
#define DUEL_TAG_MODE			0x20
#define DUEL_SIMPLE_AI			0x40
#endif /* DUEL_H_ */
