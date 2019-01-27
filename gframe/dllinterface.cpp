#ifdef YGOPRO_BUILD_DLL
#include "dllinterface.h"
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
void* OpenLibrary(const std::string& path) {
	return LoadLibrary(BufferIO::DecodeUTF8s(path + "ocgcore.dll").c_str());
}
void CloseLibrary(void *handle) {
	FreeLibrary((HMODULE)handle);
}

#define LF(x,core) x = (decltype(x))GetProcAddress((HMODULE)core, #x);\
		if(!x){ CloseLibrary(core); return nullptr; }

#else

void* OpenLibrary(const std::string& path) {
#ifdef __APPLE__
	return dlopen((path + "libocgcore.dylib").c_str(), RTLD_LAZY);
#else
	return dlopen((path + "libocgcore.so").c_str(), RTLD_LAZY);
#endif
}

void CloseLibrary(void *handle) {
	dlclose(handle);
}

#define LF(x,core) x = (decltype(x))dlsym(core, #x);\
		if(!x){ CloseLibrary(core); return nullptr; }

#endif

void* LoadOCGcore(const std::string& path) {
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	LF(set_script_reader, newcore);
	LF(set_card_reader, newcore);
	LF(set_message_handler, newcore);
	LF(create_duel, newcore);
	LF(start_duel, newcore);
	LF(end_duel, newcore);
	LF(set_player_info, newcore);
	LF(get_log_message, newcore);
	LF(get_message, newcore);
	LF(process, newcore);
	LF(new_card, newcore);
	LF(new_tag_card, newcore);
	LF(new_relay_card, newcore);
	LF(query_card, newcore);
	LF(query_field_count, newcore);
	LF(query_field_card, newcore);
	LF(query_field_info, newcore);
	LF(set_responsei, newcore);
	LF(set_responseb, newcore);
	LF(preload_script, newcore);
	return newcore;
}
bool ReloadCore(void *handle) {
	if(!handle)
		return false;
	LF(set_script_reader, handle);
	LF(set_card_reader, handle);
	LF(set_message_handler, handle);
	LF(create_duel, handle);
	LF(start_duel, handle);
	LF(end_duel, handle);
	LF(set_player_info, handle);
	LF(get_log_message, handle);
	LF(get_message, handle);
	LF(process, handle);
	LF(new_card, handle);
	LF(new_tag_card, handle);
	LF(new_relay_card, handle);
	LF(query_card, handle);
	LF(query_field_count, handle);
	LF(query_field_card, handle);
	LF(query_field_info, handle);
	LF(set_responsei, handle);
	LF(set_responseb, handle);
	LF(preload_script, handle);
	return true;
}

void UnloadCore(void *handle) {
	CloseLibrary(handle);
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
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	LF(set_script_reader, newcore);
	LF(set_card_reader, newcore);
	LF(set_message_handler, newcore);
	LF(create_duel, newcore);
	LF(start_duel, newcore);
	LF(end_duel, newcore);
	LF(set_player_info, newcore);
	LF(get_log_message, newcore);
	LF(get_message, newcore);
	LF(process, newcore);
	LF(new_card, newcore);
	LF(new_tag_card, newcore);
	LF(new_relay_card, newcore);
	LF(query_card, newcore);
	LF(query_field_count, newcore);
	LF(query_field_card, newcore);
	LF(query_field_info, newcore);
	LF(set_responsei, newcore);
	LF(set_responseb, newcore);
	LF(preload_script, newcore);
	CloseLibrary(handle);
	return newcore;
}
#undef LF
#endif //YGOPRO_BUILD_DLL
