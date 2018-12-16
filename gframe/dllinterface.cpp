#ifdef YGOPRO_BUILD_DLL
#include "dllinterface.h"
#include <cstdio>
#include <string>
#ifndef _WIN32
#include <dlfcn.h>
#endif

void(*set_script_reader)(script_reader f) = nullptr;
void(*set_card_reader)(card_reader f) = nullptr;
void(*set_message_handler)(message_handler f) = nullptr;

ptr(*create_duel)(uint32 seed) = nullptr;
void(*start_duel)(ptr pduel, int32 options) = nullptr;
void(*end_duel)(ptr pduel) = nullptr;
void(*set_player_info)(ptr pduel, int32 playerid, int32 lp, int32 startcount, int32 drawcount) = nullptr;
void(*get_log_message)(ptr pduel, byte* buf) = nullptr;
int32(*get_message)(ptr pduel, byte* buf) = nullptr;
int32(*process)(ptr pduel) = nullptr;
void(*new_card)(ptr pduel, uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position) = nullptr;
void(*new_tag_card)(ptr pduel, uint32 code, uint8 owner, uint8 location) = nullptr;
void(*new_relay_card)(ptr pduel, uint32 code, uint8 owner, uint8 location, uint8 playernum) = nullptr;
int32(*query_card)(ptr pduel, uint8 playerid, uint8 location, uint8 sequence, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache) = nullptr;
int32(*query_field_count)(ptr pduel, uint8 playerid, uint8 location) = nullptr;
int32(*query_field_card)(ptr pduel, uint8 playerid, uint8 location, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache) = nullptr;
int32(*query_field_info)(ptr pduel, byte* buf) = nullptr;
void(*set_responsei)(ptr pduel, int32 value) = nullptr;
void(*set_responseb)(ptr pduel, byte* buf) = nullptr;
int32(*preload_script)(ptr pduel, char* script, int32 len, int32 scriptlen, char* scriptbuff) = nullptr;

#ifdef _WIN32
void* LoadOCGcore(const std::string& path) {
	void* newcore = LoadLibrary(BufferIO::DecodeUTF8s(path + ".dll").c_str());
	if(!newcore)
		return nullptr;
#define LF(x) x = (decltype(x))GetProcAddress((HMODULE)newcore, #x);\
		if(!x) return nullptr;
#else
void* LoadOCGcore(const std::string& path) {
#ifdef __APPLE__
	std::string ext(".dylib");
#else
	std::string ext(".so");
#endif //__APPLE__
	void* newcore = dlopen((path + ext).c_str(), RTLD_LAZY);
	if(!newcore)
		return nullptr;
#define LF(x) x = (decltype(x))dlsym(newcore, #x);\
		if(!x) return nullptr;
#endif //_WIN32
	LF(set_script_reader);
	LF(set_card_reader);
	LF(set_message_handler);
	LF(create_duel);
	LF(start_duel);
	LF(end_duel);
	LF(set_player_info);
	LF(get_log_message);
	LF(get_message);
	LF(process);
	LF(new_card);
	LF(new_tag_card);
	LF(new_relay_card);
	LF(query_card);
	LF(query_field_count);
	LF(query_field_card);
	LF(query_field_info);
	LF(set_responsei);
	LF(set_responseb);
	LF(preload_script);
#undef LF
	return newcore;
}
bool ReloadCore(void *handle) {
	if(!handle)
		return false;
#ifdef _WIN32
#define LF(x) x = (decltype(x))GetProcAddress((HMODULE)handle, #x);\
		if(!x) return false;
#else
#define LF(x) x = (decltype(x))dlsym(handle, #x);\
		if(!x) return false;
#endif //_WIN32
	LF(set_script_reader);
	LF(set_card_reader);
	LF(set_message_handler);
	LF(create_duel);
	LF(start_duel);
	LF(end_duel);
	LF(set_player_info);
	LF(get_log_message);
	LF(get_message);
	LF(process);
	LF(new_card);
	LF(new_tag_card);
	LF(new_relay_card);
	LF(query_card);
	LF(query_field_count);
	LF(query_field_card);
	LF(query_field_info);
	LF(set_responsei);
	LF(set_responseb);
	LF(preload_script);
#undef LF
	return true;
}

void UnloadCore(void *handle) {
#ifdef _WIN32
	FreeLibrary((HMODULE)handle);
#else
	dlclose(handle);
#endif
	set_script_reader = nullptr;
	set_card_reader = nullptr;
	set_message_handler = nullptr;
	create_duel = nullptr;
	start_duel = nullptr;
	end_duel = nullptr;
	set_player_info = nullptr;
	get_log_message = nullptr;
	get_message = nullptr;
	process = nullptr;
	new_card = nullptr;
	new_tag_card = nullptr;
	new_relay_card = nullptr;
	query_card = nullptr;
	query_field_count = nullptr;
	query_field_card = nullptr;
	query_field_info = nullptr;
	set_responsei = nullptr;
	set_responseb = nullptr;
	preload_script = nullptr;
}

void* ChangeOCGcore(const std::string& path, void *handle) {
#ifdef _WIN32
	void* newcore = LoadLibrary(BufferIO::DecodeUTF8s(path + ".dll").c_str());
	if(!newcore)
		return nullptr;
#define LF(x) x = (decltype(x))GetProcAddress((HMODULE)newcore, #x);\
		if(!x) return nullptr;
#else
#ifdef __APPLE__
	std::string ext(".dylib");
#else
	std::string ext(".so");
#endif //__APPLE__
	void* newcore = dlopen((path + ext).c_str(), RTLD_LAZY);
	if(!newcore)
		return nullptr;
#define LF(x) x = (decltype(x))dlsym(newcore, #x);\
		if(!x) return nullptr;
#endif //_WIN32
	LF(set_script_reader);
	LF(set_card_reader);
	LF(set_message_handler);
	LF(create_duel);
	LF(start_duel);
	LF(end_duel);
	LF(set_player_info);
	LF(get_log_message);
	LF(get_message);
	LF(process);
	LF(new_card);
	LF(new_tag_card);
	LF(new_relay_card);
	LF(query_card);
	LF(query_field_count);
	LF(query_field_card);
	LF(query_field_info);
	LF(set_responsei);
	LF(set_responseb);
	LF(preload_script);
#undef LF
#ifdef _WIN32
	FreeLibrary((HMODULE)handle);
#else
	dlclose(handle);
#endif
	return newcore;
}
#endif //YGOPRO_BUILD_DLL
