#ifdef YGOPRO_BUILD_DLL
#ifndef DLL_INTERFACE_H_
#define DLL_INTERFACE_H_

#include "common.h"
#include "config.h"

struct card_data;

typedef byte* (*script_reader)(const char*, int*);
typedef uint32 (*card_reader)(uint32, card_data*);
typedef uint32 (*message_handler)(void*, uint32);
bool ReloadCore(void* handle);
void UnloadCore(void *handle);
void* LoadOCGcore(const std::wstring& path);
void* LoadOCGcore(const std::string& path);
void* ChangeOCGcore(const std::wstring& path, void* handle);
void* ChangeOCGcore(const std::string& path, void* handle);

extern int(*get_api_version)(int* min);

extern void(*set_script_reader)(script_reader f);
extern void(*set_card_reader)(card_reader f);
extern void(*set_message_handler)(message_handler f);

extern ptr(*create_duel)(uint32 seed);
extern void(*start_duel)(ptr pduel, int32 options);
extern void(*end_duel)(ptr pduel);
extern void(*set_player_info)(ptr pduel, int32 playerid, int32 lp, int32 startcount, int32 drawcount);
extern void(*get_log_message)(ptr pduel, byte* buf);
extern int32(*get_message)(ptr pduel, byte* buf);
extern int32(*process)(ptr pduel);
extern void(*new_card)(ptr pduel, uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position, uint32 duelist);
extern int32(*get_cached_query)(ptr pduel, byte* buf);
extern int32(*query_card)(ptr pduel, uint8 playerid, uint8 location, uint8 sequence, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache);
extern int32(*query_field_count)(ptr pduel, uint8 playerid, uint8 location);
extern int32(*query_field_card)(ptr pduel, uint8 playerid, uint8 location, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache);
extern int32(*query_field_info)(ptr pduel, byte* buf);
extern void(*set_responsei)(ptr pduel, int32 value);
extern void(*set_responseb)(ptr pduel, byte* buf, size_t len);
extern int32(*preload_script)(ptr pduel, char* script, int32 len, int32 scriptlen, char* scriptbuff);

byte* default_script_reader(const char* script_name, int* len);

#endif /* DLL_INTERFACE_H_ */
#endif //YGOPRO_BUILD_DLL