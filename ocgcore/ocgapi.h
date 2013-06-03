/*
 * interface.h
 *
 *  Created on: 2010-4-28
 *      Author: Argon
 */

#ifndef OCGAPI_H_
#define OCGAPI_H_

#include "common.h"
#ifdef _WIN32
#include <windows.h>
#define DECL_DLLEXPORT __declspec(dllexport)
#else
#define DECL_DLLEXPORT
#endif

class duel;
struct card_data;

class DECL_DLLEXPORT ygoAdapter {
public:
	virtual byte* ReadScript(const char* script_name, int* len);
	virtual uint32 ReadCard(uint32 code, card_data* data);
	virtual uint32 HandleMessage(void* msg, uint32 message_type);
};

extern ygoAdapter defaultAdapter;

class DECL_DLLEXPORT duelAdapter {

protected:
	duel* pduel;

public:
	duelAdapter(uint32 seed);
	virtual ~duelAdapter();
	void start_duel(int32 options);
	void set_player_info(int32 playerid, int32 lp, int32 startcount, int32 drawcount);
	void get_log_message(byte* buf);
	int32 get_message(byte* buf);
	int32 process();
	void new_card(uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position);
	void new_tag_card(uint32 code, uint8 owner, uint8 location);
	int32 query_field_info(byte* buf);
	void set_responsei(int32 value);
	void set_responseb(byte* buf);
	int32 preload_script(char* script, int32 len);
	void set_adapter(ygoAdapter*);
};

#endif /* OCGAPI_H_ */
