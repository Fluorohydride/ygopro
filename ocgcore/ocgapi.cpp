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

byte buffer[0x10000];
ygoAdapter defaultAdapter;

byte* ygoAdapter::ReadScript(const char* script_name, int* len) {
	FILE *fp;
	fp = fopen(script_name, "rb");
	if (!fp)
		return 0;
	fseek(fp, 0, SEEK_END);
	uint32 flen = ftell(fp);
	if (flen > 0x10000) {
		fclose(fp);
		return 0;
	}
	fseek(fp, 0, SEEK_SET);
	fread(buffer, flen, 1, fp);
	fclose(fp);
	*len = flen;
	return buffer;
}
uint32 ygoAdapter::ReadCard(uint32 code, card_data* data) {
	return 0;
}
uint32 ygoAdapter::HandleMessage(void* pduel, uint32 msg_type) {
	return 0;
}

duelAdapter::duelAdapter(uint32 seed) {
	pduel = new duel();
	pduel->random.reset(seed);
}
duelAdapter::~duelAdapter() {
	delete pduel;
}
void duelAdapter::start_duel(int options) {
	pduel->game_field->core.duel_options |= options;
	pduel->game_field->core.shuffle_hand_check[0] = FALSE;
	pduel->game_field->core.shuffle_hand_check[1] = FALSE;
	pduel->game_field->core.shuffle_deck_check[0] = FALSE;
	pduel->game_field->core.shuffle_deck_check[1] = FALSE;
	if(pduel->game_field->player[0].start_count > 0)
		pduel->game_field->draw(0, REASON_RULE, PLAYER_NONE, 0, pduel->game_field->player[0].start_count);
	if(pduel->game_field->player[1].start_count > 0)
		pduel->game_field->draw(0, REASON_RULE, PLAYER_NONE, 1, pduel->game_field->player[1].start_count);
	if(options & DUEL_TAG_MODE) {
		for(int i = 0; i < pduel->game_field->player[0].start_count && pduel->game_field->player[0].tag_list_main.size(); ++i) {
			card* pcard = *pduel->game_field->player[0].tag_list_main.rbegin();
			pduel->game_field->player[0].tag_list_main.pop_back();
			pduel->game_field->player[0].tag_list_hand.push_back(pcard);
			pcard->current.controler = 0;
			pcard->current.location = LOCATION_HAND;
			pcard->current.sequence = pduel->game_field->player[0].tag_list_hand.size() - 1;
		}
		for(int i = 0; i < pduel->game_field->player[1].start_count && pduel->game_field->player[1].tag_list_main.size(); ++i) {
			card* pcard = *pduel->game_field->player[1].tag_list_main.rbegin();
			pduel->game_field->player[1].tag_list_main.pop_back();
			pduel->game_field->player[1].tag_list_hand.push_back(pcard);
			pcard->current.controler = 1;
			pcard->current.location = LOCATION_HAND;
			pcard->current.sequence = pduel->game_field->player[1].tag_list_hand.size() - 1;
		}
	}
	pduel->game_field->add_process(PROCESSOR_TURN, 0, 0, 0, 0, 0);
}
void duelAdapter::set_player_info(int32 playerid, int32 lp, int32 startcount, int32 drawcount) {
	if(lp > 0)
		pduel->game_field->player[playerid].lp = lp;
	if(startcount >= 0)
		pduel->game_field->player[playerid].start_count = startcount;
	if(drawcount >= 0)
		pduel->game_field->player[playerid].draw_count = drawcount;
}
void duelAdapter::get_log_message(byte* buf) {
	strcpy((char*)buf, pduel->strbuffer);
}
int32 duelAdapter::get_message(byte* buf) {
	int32 len = pduel->read_buffer(buf);
	pduel->clear_buffer();
	return len;
}
int32 duelAdapter::process() {
	int result = pduel->game_field->process();
	while((result & 0xffff) == 0 && (result & 0xf0000) == 0)
		result = pduel->game_field->process();
	return result;
}
void duelAdapter::new_card(uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position) {
	if(pduel->game_field->is_location_useable(playerid, location, sequence)) {
		card* pcard = pduel->new_card(code);
		pcard->owner = owner;
		pduel->game_field->add_card(playerid, pcard, location, sequence);
		pcard->current.position = position;
		if(!(location & LOCATION_ONFIELD) || (position & POS_FACEUP)) {
			pcard->enable_field_effect(TRUE);
			pduel->game_field->adjust_instant();
		}
		if(location & LOCATION_ONFIELD) {
			if(location == LOCATION_MZONE)
				pcard->set_status(STATUS_PROC_COMPLETE, TRUE);
		}
	}
}
void duelAdapter::new_tag_card(uint32 code, uint8 owner, uint8 location) {
	if(owner > 1 || !(location & 0x41))
		return;
	card* pcard = pduel->new_card(code);
	switch(location) {
	case LOCATION_DECK:
		pduel->game_field->player[owner].tag_list_main.push_back(pcard);
		pcard->owner = owner;
		pcard->current.controler = owner;
		pcard->current.location = LOCATION_DECK;
		pcard->current.sequence = pduel->game_field->player[owner].tag_list_main.size() - 1;
		break;
	case LOCATION_EXTRA:
		pduel->game_field->player[owner].tag_list_extra.push_back(pcard);
		pcard->owner = owner;
		pcard->current.controler = owner;
		pcard->current.location = LOCATION_EXTRA;
		pcard->current.sequence = pduel->game_field->player[owner].tag_list_extra.size() - 1;
		break;
	}
}
int32 duelAdapter::query_field_info( byte* buf) {
	*buf++ = MSG_RELOAD_FIELD;
	card* pcard;
	for(int playerid = 0; playerid < 2; ++playerid) {
		*((int*)(buf)) = pduel->game_field->player[playerid].lp;
		buf += 4;
		for(uint32 i = 0; i < 5; ++i) {
			pcard = pduel->game_field->player[playerid].list_mzone[i];
			if(pcard) {
				*buf++ = 1;
				*buf++ = pcard->current.position;
				*buf++ = pcard->xyz_materials.size();
			} else {
				*buf++ = 0;
			}
		}
		for(uint32 i = 0; i < 8; ++i) {
			pcard = pduel->game_field->player[playerid].list_szone[i];
			if(pcard) {
				*buf++ = 1;
				*buf++ = pcard->current.position;
			} else {
				*buf++ = 0;
			}
		}
		*buf++ = pduel->game_field->player[playerid].list_main.size();
		*buf++ = pduel->game_field->player[playerid].list_hand.size();
		*buf++ = pduel->game_field->player[playerid].list_grave.size();
		*buf++ = pduel->game_field->player[playerid].list_remove.size();
		*buf++ = pduel->game_field->player[playerid].list_extra.size();
	}
	*buf++ = pduel->game_field->core.current_chain.size();
	for(auto chit = pduel->game_field->core.current_chain.begin(); chit != pduel->game_field->core.current_chain.end(); ++chit) {
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
    for(int playerid = 0; playerid < 2; ++playerid) {
        for(uint32 i = 0; i < 5; ++i) {
            pcard = pduel->game_field->player[playerid].list_mzone[i];
            if(pcard)
                pcard->update_infos_nocache(QUERY_INITM);
        }
        for(uint32 i = 0; i < 6; ++i) {
            pcard = pduel->game_field->player[playerid].list_szone[i];
            if(pcard)
                pcard->update_infos_nocache(QUERY_INITST);
        }
        for(uint32 i = 7; i < 8; ++i) {
            pcard = pduel->game_field->player[playerid].list_szone[i];
            if(pcard)
                pcard->update_infos_nocache(QUERY_INITP);
        }
    }
	return 0;
}

void duelAdapter::set_adapter(ygoAdapter* adapter) {
	pduel->adapter = adapter;
}
