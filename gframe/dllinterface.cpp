#ifdef YGOPRO_BUILD_DLL
#include "dllinterface.h"
#include <string>
#ifndef _WIN32
#include <dlfcn.h>
#endif

int(*get_api_version)(int* min) = nullptr;

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
void(*new_card)(ptr pduel, uint32 code, uint8 owner, uint8 playerid, uint8 location, uint8 sequence, uint8 position, uint32 duelist) = nullptr;
int32(*get_cached_query)(ptr pduel, byte* buf) = nullptr;
int32(*query_card)(ptr pduel, uint8 playerid, uint8 location, uint8 sequence, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache) = nullptr;
int32(*query_field_count)(ptr pduel, uint8 playerid, uint8 location) = nullptr;
int32(*query_field_card)(ptr pduel, uint8 playerid, uint8 location, int32 query_flag, byte* buf, int32 use_cache, int32 ignore_cache) = nullptr;
int32(*query_field_info)(ptr pduel, byte* buf) = nullptr;
void(*set_responsei)(ptr pduel, int32 value) = nullptr;
void(*set_responseb)(ptr pduel, byte* buf, size_t len) = nullptr;
int32(*preload_script)(ptr pduel, char* script, int32 len, int32 scriptlen, char* scriptbuff) = nullptr;

#define CREATE_CLONE(x) auto x##_copy = x;
#define STORE_CLONE(x) x##_copy = x;
#define CLEAR_CLONE(x) x##_copy = nullptr;

CREATE_CLONE(get_api_version)
CREATE_CLONE(set_script_reader)
CREATE_CLONE(set_card_reader)
CREATE_CLONE(set_message_handler)
CREATE_CLONE(create_duel)
CREATE_CLONE(start_duel)
CREATE_CLONE(end_duel)
CREATE_CLONE(set_player_info)
CREATE_CLONE(get_log_message)
CREATE_CLONE(get_message)
CREATE_CLONE(process)
CREATE_CLONE(new_card)
CREATE_CLONE(get_cached_query)
CREATE_CLONE(query_card)
CREATE_CLONE(query_field_count)
CREATE_CLONE(query_field_card)
CREATE_CLONE(query_field_info)
CREATE_CLONE(set_responsei)
CREATE_CLONE(set_responseb)
CREATE_CLONE(preload_script)
#undef CREATE_CLONE

#ifdef _WIN32
void* OpenLibrary(const std::wstring& path) {
	return LoadLibrary((path + L"ocgcore.dll").c_str());
}
void* OpenLibrary(const std::string& path) {
	return OpenLibrary(BufferIO::DecodeUTF8s(path));
}
void CloseLibrary(void *handle) {
	FreeLibrary((HMODULE)handle);
}

#define GetFunction(core, x) GetProcAddress((HMODULE)core, x)

#else

void* OpenLibrary(const std::string& path) {
#ifdef __APPLE__
	return dlopen((path + "libocgcore.dylib").c_str(), RTLD_LAZY);
#else
	return dlopen((path + "libocgcore.so").c_str(), RTLD_LAZY);
#endif
}
void* OpenLibrary(const std::wstring& path) {
	return OpenLibrary(BufferIO::EncodeUTF8s(path));
}

void CloseLibrary(void *handle) {
	dlclose(handle);
}

#define GetFunction(core, x) dlsym(core, x)

#endif

#define RESTORE_CLONE(x) if(x##_copy) { x = x##_copy; x##_copy = nullptr; }

void RestoreFromCopies() {
	RESTORE_CLONE(get_api_version);
	RESTORE_CLONE(set_script_reader);
	RESTORE_CLONE(set_card_reader);
	RESTORE_CLONE(set_message_handler);
	RESTORE_CLONE(create_duel);
	RESTORE_CLONE(start_duel);
	RESTORE_CLONE(end_duel);
	RESTORE_CLONE(set_player_info);
	RESTORE_CLONE(get_log_message);
	RESTORE_CLONE(process);
	RESTORE_CLONE(new_card);
	RESTORE_CLONE(get_cached_query);
	RESTORE_CLONE(query_field_count);
	RESTORE_CLONE(query_field_card);
	RESTORE_CLONE(query_field_info);
	RESTORE_CLONE(set_responsei);
	RESTORE_CLONE(set_responseb);
	RESTORE_CLONE(preload_script);
}

#undef RESTORE_CLONE

void ClearCopies() {
	CLEAR_CLONE(get_api_version);
	CLEAR_CLONE(set_script_reader);
	CLEAR_CLONE(set_card_reader);
	CLEAR_CLONE(set_message_handler);
	CLEAR_CLONE(create_duel);
	CLEAR_CLONE(start_duel);
	CLEAR_CLONE(end_duel);
	CLEAR_CLONE(set_player_info);
	CLEAR_CLONE(get_log_message);
	CLEAR_CLONE(process);
	CLEAR_CLONE(new_card);
	CLEAR_CLONE(get_cached_query);
	CLEAR_CLONE(query_field_count);
	CLEAR_CLONE(query_field_card);
	CLEAR_CLONE(query_field_info);
	CLEAR_CLONE(set_responsei);
	CLEAR_CLONE(set_responseb);
	CLEAR_CLONE(preload_script);
}

void* LoadOCGcore(const std::wstring& path) {
	return LoadOCGcore(BufferIO::EncodeUTF8s(path));
}

bool check_api_version() {
	int min = 0;
	int max = get_api_version(&min);
	return max == 3;
}

#define LOAD_FUNCTION(x) x = (decltype(x))GetFunction(newcore, #x);\
		if(!x){ UnloadCore(newcore); return nullptr; }

void* LoadOCGcore(const std::string& path) {
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	LOAD_FUNCTION(get_api_version);
	if(!check_api_version()) {
		UnloadCore(newcore);
		return nullptr;
	}
	LOAD_FUNCTION(set_script_reader);
	LOAD_FUNCTION(set_card_reader);
	LOAD_FUNCTION(set_message_handler);
	LOAD_FUNCTION(create_duel);
	LOAD_FUNCTION(start_duel);
	LOAD_FUNCTION(end_duel);
	LOAD_FUNCTION(set_player_info);
	LOAD_FUNCTION(get_log_message);
	LOAD_FUNCTION(get_message);
	LOAD_FUNCTION(process);
	LOAD_FUNCTION(new_card);
	LOAD_FUNCTION(get_cached_query);
	LOAD_FUNCTION(query_card);
	LOAD_FUNCTION(query_field_count);
	LOAD_FUNCTION(query_field_card);
	LOAD_FUNCTION(query_field_info);
	LOAD_FUNCTION(set_responsei);
	LOAD_FUNCTION(set_responseb);
	LOAD_FUNCTION(preload_script);
	ClearCopies();
	return newcore;
}
#undef LOAD_FUNCTION

#define LOAD_WITH_COPY_CHECK(x) STORE_CLONE(x)\
		x = (decltype(x))GetFunction(handle, #x);\
		if(!x) {\
			UnloadCore(handle);\
			RestoreFromCopies();\
			return false;\
		}

bool ReloadCore(void *handle) {
	if(!handle)
		return false;
	LOAD_WITH_COPY_CHECK(get_api_version);
	LOAD_WITH_COPY_CHECK(set_script_reader);
	LOAD_WITH_COPY_CHECK(set_card_reader);
	LOAD_WITH_COPY_CHECK(set_message_handler);
	LOAD_WITH_COPY_CHECK(create_duel);
	LOAD_WITH_COPY_CHECK(start_duel);
	LOAD_WITH_COPY_CHECK(end_duel);
	LOAD_WITH_COPY_CHECK(set_player_info);
	LOAD_WITH_COPY_CHECK(get_log_message);
	LOAD_WITH_COPY_CHECK(get_message);
	LOAD_WITH_COPY_CHECK(process);
	LOAD_WITH_COPY_CHECK(new_card);
	LOAD_WITH_COPY_CHECK(get_cached_query);
	LOAD_WITH_COPY_CHECK(query_card);
	LOAD_WITH_COPY_CHECK(query_field_count);
	LOAD_WITH_COPY_CHECK(query_field_card);
	LOAD_WITH_COPY_CHECK(query_field_info);
	LOAD_WITH_COPY_CHECK(set_responsei);
	LOAD_WITH_COPY_CHECK(set_responseb);
	LOAD_WITH_COPY_CHECK(preload_script);
	ClearCopies();
	return true;
}

#undef LOAD_WITH_COPY_CHECK

void UnloadCore(void *handle) {
	CloseLibrary(handle);
	get_api_version = nullptr;
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
	get_cached_query = nullptr;
	query_card = nullptr;
	query_field_count = nullptr;
	query_field_card = nullptr;
	query_field_info = nullptr;
	set_responsei = nullptr;
	set_responseb = nullptr;
	preload_script = nullptr;
}

void* ChangeOCGcore(const std::wstring & path, void * handle) {
	return ChangeOCGcore(BufferIO::EncodeUTF8s(path), handle);
}

#define CHANGE_WITH_COPY_CHECK(x) STORE_CLONE(x)\
		x = (decltype(x))GetFunction(newcore, #x);\
		if(!x) {\
			CloseLibrary(newcore);\
			RestoreFromCopies();\
			return nullptr;\
		}

void* ChangeOCGcore(const std::string& path, void *handle) {
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	CHANGE_WITH_COPY_CHECK(get_api_version);
	if(!check_api_version()) {
		CloseLibrary(newcore);
		RestoreFromCopies();
		return nullptr;
	}
	CHANGE_WITH_COPY_CHECK(set_script_reader);
	CHANGE_WITH_COPY_CHECK(set_card_reader);
	CHANGE_WITH_COPY_CHECK(set_message_handler);
	CHANGE_WITH_COPY_CHECK(create_duel);
	CHANGE_WITH_COPY_CHECK(start_duel);
	CHANGE_WITH_COPY_CHECK(end_duel);
	CHANGE_WITH_COPY_CHECK(set_player_info);
	CHANGE_WITH_COPY_CHECK(get_log_message);
	CHANGE_WITH_COPY_CHECK(get_message);
	CHANGE_WITH_COPY_CHECK(process);
	CHANGE_WITH_COPY_CHECK(new_card);
	CHANGE_WITH_COPY_CHECK(get_cached_query);
	CHANGE_WITH_COPY_CHECK(query_card);
	CHANGE_WITH_COPY_CHECK(query_field_count);
	CHANGE_WITH_COPY_CHECK(query_field_card);
	CHANGE_WITH_COPY_CHECK(query_field_info);
	CHANGE_WITH_COPY_CHECK(set_responsei);
	CHANGE_WITH_COPY_CHECK(set_responseb);
	CHANGE_WITH_COPY_CHECK(preload_script);
	ClearCopies();
	if(handle)
		CloseLibrary(handle);
	return newcore;
}
#undef CHANGE_WITH_COPY_CHECK
#undef STORE_CLONE
#undef CLEAR_CLONE
#endif //YGOPRO_BUILD_DLL
