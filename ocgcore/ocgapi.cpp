/*
 * interface.cpp
 *
 *  Created on: 2010-5-2
 *      Author: Argon
 */
#include <stdio.h>
#include <string.h>
#include "ocgapi.h"
#include "duel.h"
#include "card.h"
#include "group.h"
#include "effect.h"
#include "field.h"
#include "interpreter.h"
#include <set>

script_reader sreader = default_script_reader;
card_reader creader = default_card_reader;
message_handler mhandler = default_message_handler;
byte buffer[0x10000];
std::set<duel*> duel_set;

extern "C" DECL_DLLEXPORT void set_script_reader(script_reader f) {
	sreader = f;
}
extern "C" DECL_DLLEXPORT void set_card_reader(card_reader f) {
	creader = f;
}
extern "C" DECL_DLLEXPORT void set_message_handler(message_handler f) {
	mhandler = f;
}
byte* read_script(const char* script_name, int* len) {
	return sreader(script_name, len);
}
uint32 read_card(uint32 code, card_data* data) {
	return creader(code, data);
}
uint32 handle_message(void* pduel, uint32 msg_type) {
	return mhandler(pduel, msg_type);
}
byte* default_script_reader(const char* script_name, int* slen) {
	FILE *fp;
	fp = fopen(script_name, "rb");
	if (!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	uint32 len = ftell(fp);
	if (len > 0x10000) {
		fclose(fp);
		return 0;
	}
	fseek(fp, 0, SEEK_SET);
	fread(buffer, len, 1, fp);
	fclose(fp);
	*slen = len;
	return buffer;
}
uint32 default_card_reader(uint32 code, card_data* data) {
	return 0;
}
uint32 default_message_handler(void* pduel, uint32 message_type) {
	return 0;
}
extern "C" DECL_DLLEXPORT ptr create_duel(uint32 seed) {
	duel* pduel = new duel();
	duel_set.insert(pduel);
	pduel->random.reset(seed);
	return (ptr)pduel;
}
extern "C" DECL_DLLEXPORT void start_duel(ptr pduel, int options) {
	duel* pd = (duel*)pduel;
	pd->game_field->core.duel_options |= options;
	pd->game_field->core.shuffle_hand_check[0] = FALSE;
	pd->game_field->core.shuffle_hand_check[1] = FALSE;
	pd->game_field->core.shuffle_deck_check[0] = FALSE;
	pd->game_field->core.shuffle_deck_check[1] = FALSE;
	if(pd->game_field->player[0].start_count > 0)
		pd->game_field->draw(0, REASON_RULE, PLAYER_NONE, 0, pd->game_field->player[0].start_count);
	if(pd->game_field->player[1].start_count > 0)
		pd->game_field->draw(0, REASON_RULE, PLAYER_NONE, 1, pd->game_field->player[1].start_count);
	if(options & DUEL_TAG_MODE) {
		for(int i = 0; i < pd->game_field->player[0].start_count && pd->game_field->player[0].tag_list_main.size(); ++i) {
			card* pcard = *pd->game_field->player[0].tag_list_main.rbegin();
			pd->game_field->player[0].tag_list_main.pop_back();
			pd->game_field->player[0].tag_list_hand.push_back(pcard);
			pcard->current.controler = 0;
			pcard->current.location = LOCATION_HAND;
			pcard->current.sequence = pd->game_field->player[0].tag_list_hand.size() - 1;
		}
		for(int i = 0; i < pd->game_field->player[1].start_count && pd->game_field->player[1].tag_list_main.size(); ++i) {
			card* pcard = *pd->game_field->player[1].tag_list_main.rbegin();
			pd->game_field->player[1].tag_list_main.pop_back();
			pd->game_field->player[1].tag_list_hand.push_back(pcard);
			pcard->current.controler = 1;
			pcard->current.location = LOCATION_HAND;
			pcard->current.sequence = pd->game_field->player[1].tag_list_hand.size() - 1;
		}
	}
	pd->game_field->add_process(PROCESSOR_TURN, 0, 0, 0, 0, 0);
}
extern "C" DECL_DLLEXPORT void end_duel(ptr pduel) {
	duel* pd = (duel*)pduel;
	if(duel_set.count(pd)) {
		duel_set.erase(pd);
		delete pd;
	}
}
extern "C" DECL_DLLEXPORT void set_player_info(ptr pduel, int32 playerid, int32 lp, int32 startcount, int32 drawcount) {
	duel* pd = (duel*)pduel;
	if(lp > 0)
		pd->game_field->player[playerid].lp = lp;
	if(startcount >= 0)
		pd->game_field->player[playerid].start_count = startcount;
	if(drawcount >= 0)
		pd->game_field->player[playerid].draw_count = drawcount;
}
extern "C" DECL_DLLEXPORT void get_log_message(ptr pduel, byte* buf) {
	strcpy((char*)buf, ((duel*)pduel)->strbuffer);
}
extern "C" DECL_DLLEXPORT int32 get_message(ptr pduel, byte* buf) {
	int32 len = ((duel*)pduel)->read_buffer(buf);
	((duel*)pduel)->clear_buffer();
	return len;
}
extern "C" DECL_DLLEXPORT int32 process(ptr pduel) {
	duel* pd = (duel*)pduel;
	int result = pd->game_field->process();
	while((result & 0xffff) == 0 && (result & 0xf0000) == 0)
		result = pd->game_field->process();
	return result;
}
extern "C" DECL_DLLEXPORT void new_card(ptr pduel, uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position) {
	duel* ptduel = (duel*)pduel;
	if(ptduel->game_field->is_location_useable(playerid, location, sequence)) {
		card* pcard = ptduel->new_card(code);
		pcard->owner = owner;
		ptduel->game_field->add_card(playerid, pcard, location, sequence);
		pcard->current.position = position;
		if(!(location & LOCATION_ONFIELD) || (position & POS_FACEUP)) {
			pcard->enable_field_effect(TRUE);
			ptduel->game_field->adjust_instant();
		}
		if(location & LOCATION_ONFIELD) {
			if(location == LOCATION_MZONE)
				pcard->set_status(STATUS_PROC_COMPLETE, TRUE);
		}
	}
}
extern "C" DECL_DLLEXPORT void new_tag_card(ptr pduel, uint32 code, uint8 owner, uint8 location) {
	duel* ptduel = (duel*)pduel;
	if(owner > 1 || !(location & 0x41))
		return;
	card* pcard = ptduel->new_card(code);
	switch(location) {
	case LOCATION_DECK:
		ptduel->game_field->player[owner].tag_list_main.push_back(pcard);
		pcard->owner = owner;
		pcard->current.controler = owner;
		pcard->current.location = LOCATION_DECK;
		pcard->current.sequence = ptduel->game_field->player[owner].tag_list_main.size() - 1;
		break;
	case LOCATION_EXTRA:
		ptduel->game_field->player[owner].tag_list_extra.push_back(pcard);
		pcard->owner = owner;
		pcard->current.controler = owner;
		pcard->current.location = LOCATION_EXTRA;
		pcard->current.sequence = ptduel->game_field->player[owner].tag_list_extra.size() - 1;
		break;
	}
}
extern "C" DECL_DLLEXPORT int32 query_card(ptr pduel, uint8 playerid, uint8 location, uint8 sequence, int32 query_flag, byte* buf, int32 use_cache) {
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* ptduel = (duel*)pduel;
	card* pcard = 0;
	location &= 0x7f;
	if(location & LOCATION_ONFIELD)
		pcard = ptduel->game_field->get_field_card(playerid, location, sequence);
	else {
		field::card_vector* lst = 0;
		if(location == LOCATION_HAND )
			lst = &ptduel->game_field->player[playerid].list_hand;
		else if(location == LOCATION_GRAVE )
			lst = &ptduel->game_field->player[playerid].list_grave;
		else if(location == LOCATION_REMOVED )
			lst = &ptduel->game_field->player[playerid].list_remove;
		else if(location == LOCATION_EXTRA )
			lst = &ptduel->game_field->player[playerid].list_extra;
		else if(location == LOCATION_DECK )
			lst = &ptduel->game_field->player[playerid].list_main;
		if(!lst || sequence > lst->size())
			pcard = 0;
		else {
			auto cit = lst->begin();
			for(uint32 i = 0; i < sequence; ++i, ++cit);
			pcard = *cit;
		}
	}
	if(pcard)
		return pcard->get_infos(buf, query_flag, use_cache);
	else {
		*((int32*)buf) = 4;
		return 4;
	}
}
extern "C" DECL_DLLEXPORT int32 query_field_count(ptr pduel, uint8 playerid, uint8 location) {
	duel* ptduel = (duel*)pduel;
	if(playerid != 0 && playerid != 1)
		return 0;
	if(location == LOCATION_HAND)
		return ptduel->game_field->player[playerid].list_hand.size();
	if(location == LOCATION_GRAVE)
		return ptduel->game_field->player[playerid].list_grave.size();
	if(location == LOCATION_REMOVED)
		return ptduel->game_field->player[playerid].list_remove.size();
	if(location == LOCATION_EXTRA)
		return ptduel->game_field->player[playerid].list_extra.size();
	if(location == LOCATION_DECK)
		return ptduel->game_field->player[playerid].list_main.size();
	if(location == LOCATION_MZONE) {
		uint32 count = 0;
		for(uint32 i = 0; i < 5; ++i)
			if(ptduel->game_field->player[playerid].list_mzone[i]) count++;
		return count;
	}
	if(location == LOCATION_SZONE) {
		uint32 count = 0;
		for(uint32 i = 0; i < 8; ++i)
			if(ptduel->game_field->player[playerid].list_szone[i]) count++;
		return count;
	}
	return 0;
}
extern "C" DECL_DLLEXPORT int32 query_field_card(ptr pduel, uint8 playerid, uint8 location, int32 query_flag, byte* buf, int32 use_cache) {
	if(playerid != 0 && playerid != 1)
		return 0;
	duel* ptduel = (duel*)pduel;
	card* pcard;
	uint32 ct = 0, clen;
	byte* p = buf;
	if(location == LOCATION_MZONE) {
		for(uint32 i = 0; i < 5; ++i) {
			pcard = ptduel->game_field->player[playerid].list_mzone[i];
			if(pcard) {
				ct += clen = pcard->get_infos(p, query_flag, use_cache);
				p += clen;
			} else {
				*((int32*)p) = 4;
				ct += 4;
				p += 4;
			}
		}
	} else if(location == LOCATION_SZONE) {
		for(uint32 i = 0; i < 8; ++i) {
			pcard = ptduel->game_field->player[playerid].list_szone[i];
			if(pcard) {
				ct += clen = pcard->get_infos(p, query_flag, use_cache);
				p += clen;
			} else {
				*((int32*)p) = 4;
				ct += 4;
				p += 4;
			}

		}
	} else {
		field::card_vector* lst;
		field::card_vector::iterator cit;
		if(location == LOCATION_HAND )
			lst = &ptduel->game_field->player[playerid].list_hand;
		else if(location == LOCATION_GRAVE )
			lst = &ptduel->game_field->player[playerid].list_grave;
		else if(location == LOCATION_REMOVED )
			lst = &ptduel->game_field->player[playerid].list_remove;
		else if(location == LOCATION_EXTRA )
			lst = &ptduel->game_field->player[playerid].list_extra;
		else if(location == LOCATION_DECK )
			lst = &ptduel->game_field->player[playerid].list_main;
		for(cit = lst->begin(); cit != lst->end(); ++cit) {
			ct += clen = (*cit)->get_infos(p, query_flag, use_cache);
			p += clen;
		}
	}
	return ct;
}
extern "C" DECL_DLLEXPORT int32 query_field_info(ptr pduel, byte* buf) {
	duel* ptduel = (duel*)pduel;
	*buf++ = MSG_RELOAD_FIELD;
	card* pcard;
	for(int playerid = 0; playerid < 2; ++playerid) {
		*((int*)(buf)) = ptduel->game_field->player[playerid].lp;
		buf += 4;
		for(uint32 i = 0; i < 5; ++i) {
			pcard = ptduel->game_field->player[playerid].list_mzone[i];
			if(pcard) {
				*buf++ = 1;
				*buf++ = pcard->current.position;
				*buf++ = pcard->xyz_materials.size();
			} else {
				*buf++ = 0;
			}
		}
		for(uint32 i = 0; i < 8; ++i) {
			pcard = ptduel->game_field->player[playerid].list_szone[i];
			if(pcard) {
				*buf++ = 1;
				*buf++ = pcard->current.position;
			} else {
				*buf++ = 0;
			}
		}
		*buf++ = ptduel->game_field->player[playerid].list_main.size();
		*buf++ = ptduel->game_field->player[playerid].list_hand.size();
		*buf++ = ptduel->game_field->player[playerid].list_grave.size();
		*buf++ = ptduel->game_field->player[playerid].list_remove.size();
		*buf++ = ptduel->game_field->player[playerid].list_extra.size();
	}
	*buf++ = ptduel->game_field->core.current_chain.size();
	for(auto chit = ptduel->game_field->core.current_chain.begin(); chit != ptduel->game_field->core.current_chain.end(); ++chit) {
		effect* peffect = chit->triggering_effect;
		*((int*)(buf)) = peffect->handler->data.code;
		buf += 4;
		*((int*)(buf)) = peffect->handler->get_info_location();
		buf += 4;
		*buf++ = chit->triggering_controler;
		*buf++ = chit->triggering_location;
		*buf++ = chit->triggering_sequence;
		*((int*)(buf)) = peffect->description;
		buf += 4;
	}
	return 0;
}
extern "C" DECL_DLLEXPORT void set_responsei(ptr pduel, int32 value) {
	((duel*)pduel)->set_responsei(value);
}
extern "C" DECL_DLLEXPORT void set_responseb(ptr pduel, byte* buf) {
	((duel*)pduel)->set_responseb(buf);
}
extern "C" DECL_DLLEXPORT int32 preload_script(ptr pduel, char* script, int32 len) {
	return ((duel*)pduel)->lua->load_script(script);
}
