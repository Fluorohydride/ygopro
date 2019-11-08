#ifdef YGOPRO_BUILD_DLL
#include "dllinterface.h"
#include <string>
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

void(*OCG_GetVersion)(int* major, int* minor) = nullptr;

int(*OCG_CreateDuel)(OCG_Duel* duel, OCG_DuelOptions options) = nullptr;
void(*OCG_DestroyDuel)(OCG_Duel duel) = nullptr;
void(*OCG_DuelNewCard)(OCG_Duel duel, OCG_NewCardInfo info) = nullptr;
int(*OCG_StartDuel)(OCG_Duel duel) = nullptr;

int(*OCG_DuelProcess)(OCG_Duel duel) = nullptr;
void*(*OCG_DuelGetMessage)(OCG_Duel duel, uint32_t* length) = nullptr;
void(*OCG_DuelSetResponse)(OCG_Duel duel, void* buffer, uint32_t length) = nullptr;
int(*OCG_LoadScript)(OCG_Duel duel, const char* buffer, uint32_t length, const char* name) = nullptr;

uint32_t(*OCG_DuelQueryCount)(OCG_Duel duel, uint8_t team, uint32_t loc) = nullptr;
void*(*OCG_DuelQuery)(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info) = nullptr;
void*(*OCG_DuelQueryLocation)(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info) = nullptr;
void*(*OCG_DuelQueryField)(OCG_Duel duel, uint32_t* length) = nullptr;

#define CREATE_CLONE(x) auto x##_copy = x;
#define STORE_CLONE(x) x##_copy = x;
#define CLEAR_CLONE(x) x##_copy = nullptr;

CREATE_CLONE(OCG_GetVersion)
CREATE_CLONE(OCG_CreateDuel)
CREATE_CLONE(OCG_DestroyDuel)
CREATE_CLONE(OCG_DuelNewCard)
CREATE_CLONE(OCG_StartDuel)
CREATE_CLONE(OCG_DuelProcess)
CREATE_CLONE(OCG_DuelGetMessage)
CREATE_CLONE(OCG_DuelSetResponse)
CREATE_CLONE(OCG_LoadScript)
CREATE_CLONE(OCG_DuelQueryCount)
CREATE_CLONE(OCG_DuelQuery)
CREATE_CLONE(OCG_DuelQueryLocation)
CREATE_CLONE(OCG_DuelQueryField)
#undef CREATE_CLONE

#ifdef _WIN32
void* OpenLibrary(const std::basic_string<TCHAR>& path) {
	return LoadLibrary((path + TEXT("ocgcore.dll")).c_str());
}
void CloseLibrary(void* handle) {
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

void CloseLibrary(void*handle) {
	dlclose(handle);
}

#define GetFunction(core, x) dlsym(core, x)

#endif

#define RESTORE_CLONE(x) if(x##_copy) { x = x##_copy; x##_copy = nullptr; }

void RestoreFromCopies() {
	RESTORE_CLONE(OCG_GetVersion);
	RESTORE_CLONE(OCG_CreateDuel);
	RESTORE_CLONE(OCG_DestroyDuel);
	RESTORE_CLONE(OCG_DuelNewCard);
	RESTORE_CLONE(OCG_StartDuel);
	RESTORE_CLONE(OCG_DuelProcess);
	RESTORE_CLONE(OCG_DuelGetMessage);
	RESTORE_CLONE(OCG_DuelSetResponse);
	RESTORE_CLONE(OCG_LoadScript);
	RESTORE_CLONE(OCG_DuelQueryCount);
	RESTORE_CLONE(OCG_DuelQuery);
	RESTORE_CLONE(OCG_DuelQueryLocation);
	RESTORE_CLONE(OCG_DuelQueryField);
}

#undef RESTORE_CLONE

void ClearCopies() {
	CLEAR_CLONE(OCG_GetVersion);
	CLEAR_CLONE(OCG_CreateDuel);
	CLEAR_CLONE(OCG_DestroyDuel);
	CLEAR_CLONE(OCG_DuelNewCard);
	CLEAR_CLONE(OCG_StartDuel);
	CLEAR_CLONE(OCG_DuelProcess);
	CLEAR_CLONE(OCG_DuelGetMessage);
	CLEAR_CLONE(OCG_DuelSetResponse);
	CLEAR_CLONE(OCG_LoadScript);
	CLEAR_CLONE(OCG_DuelQueryCount);
	CLEAR_CLONE(OCG_DuelQuery);
	CLEAR_CLONE(OCG_DuelQueryLocation);
	CLEAR_CLONE(OCG_DuelQueryField);
}

bool check_api_version() {
	int min = 0;
	int max = 0;
	OCG_GetVersion(&max, &min);
	return (max == OCG_VERSION_MAJOR) && (min == OCG_VERSION_MINOR);
}

#define LOAD_FUNCTION(x) x = (decltype(x))GetFunction(newcore, #x);\
		if(!x){ UnloadCore(newcore); return nullptr; }

#ifdef UNICODE
void* LoadOCGcore(const std::wstring& path) {
#else
void* LoadOCGcore(const std::string& path) {
#endif
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	LOAD_FUNCTION(OCG_GetVersion);
	if(!check_api_version()) {
		UnloadCore(newcore);
		return nullptr;
	}
	LOAD_FUNCTION(OCG_CreateDuel);
	LOAD_FUNCTION(OCG_DestroyDuel);
	LOAD_FUNCTION(OCG_DuelNewCard);
	LOAD_FUNCTION(OCG_StartDuel);
	LOAD_FUNCTION(OCG_DuelProcess);
	LOAD_FUNCTION(OCG_DuelGetMessage);
	LOAD_FUNCTION(OCG_DuelSetResponse);
	LOAD_FUNCTION(OCG_LoadScript);
	LOAD_FUNCTION(OCG_DuelQueryCount);
	LOAD_FUNCTION(OCG_DuelQuery);
	LOAD_FUNCTION(OCG_DuelQueryLocation);
	LOAD_FUNCTION(OCG_DuelQueryField);
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

bool ReloadCore(void*handle) {
	if(!handle)
		return false;
	LOAD_WITH_COPY_CHECK(OCG_GetVersion);
	LOAD_WITH_COPY_CHECK(OCG_CreateDuel);
	LOAD_WITH_COPY_CHECK(OCG_DestroyDuel);
	LOAD_WITH_COPY_CHECK(OCG_DuelNewCard);
	LOAD_WITH_COPY_CHECK(OCG_StartDuel);
	LOAD_WITH_COPY_CHECK(OCG_DuelProcess);
	LOAD_WITH_COPY_CHECK(OCG_DuelGetMessage);
	LOAD_WITH_COPY_CHECK(OCG_DuelSetResponse);
	LOAD_WITH_COPY_CHECK(OCG_LoadScript);
	LOAD_WITH_COPY_CHECK(OCG_DuelQueryCount);
	LOAD_WITH_COPY_CHECK(OCG_DuelQuery);
	LOAD_WITH_COPY_CHECK(OCG_DuelQueryLocation);
	LOAD_WITH_COPY_CHECK(OCG_DuelQueryField);
	ClearCopies();
	return true;
}

#undef LOAD_WITH_COPY_CHECK

#define CLEAR_FUNCTION(x) x = nullptr;

void UnloadCore(void*handle) {
	CloseLibrary(handle);
	CLEAR_FUNCTION(OCG_GetVersion);
	CLEAR_FUNCTION(OCG_CreateDuel);
	CLEAR_FUNCTION(OCG_DestroyDuel);
	CLEAR_FUNCTION(OCG_DuelNewCard);
	CLEAR_FUNCTION(OCG_StartDuel);
	CLEAR_FUNCTION(OCG_DuelProcess);
	CLEAR_FUNCTION(OCG_DuelGetMessage);
	CLEAR_FUNCTION(OCG_DuelSetResponse);
	CLEAR_FUNCTION(OCG_LoadScript);
	CLEAR_FUNCTION(OCG_DuelQueryCount);
	CLEAR_FUNCTION(OCG_DuelQuery);
	CLEAR_FUNCTION(OCG_DuelQueryLocation);
	CLEAR_FUNCTION(OCG_DuelQueryField);
}

#undef CLEAR_FUNCTION

#define CHANGE_WITH_COPY_CHECK(x) STORE_CLONE(x)\
		x = (decltype(x))GetFunction(newcore, #x);\
		if(!x) {\
			CloseLibrary(newcore);\
			RestoreFromCopies();\
			return nullptr;\
		}

#ifdef UNICODE
void* ChangeOCGcore(const std::wstring& path, void*handle) {
#else
void* ChangeOCGcore(const std::string& path, void*handle) {
#endif
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	CHANGE_WITH_COPY_CHECK(OCG_GetVersion);
	if(!check_api_version()) {
		CloseLibrary(newcore);
		RestoreFromCopies();
		return nullptr;
	}
	CHANGE_WITH_COPY_CHECK(OCG_CreateDuel);
	CHANGE_WITH_COPY_CHECK(OCG_DestroyDuel);
	CHANGE_WITH_COPY_CHECK(OCG_DuelNewCard);
	CHANGE_WITH_COPY_CHECK(OCG_StartDuel);
	CHANGE_WITH_COPY_CHECK(OCG_DuelProcess);
	CHANGE_WITH_COPY_CHECK(OCG_DuelGetMessage);
	CHANGE_WITH_COPY_CHECK(OCG_DuelSetResponse);
	CHANGE_WITH_COPY_CHECK(OCG_LoadScript);
	CHANGE_WITH_COPY_CHECK(OCG_DuelQueryCount);
	CHANGE_WITH_COPY_CHECK(OCG_DuelQuery);
	CHANGE_WITH_COPY_CHECK(OCG_DuelQueryLocation);
	CHANGE_WITH_COPY_CHECK(OCG_DuelQueryField);
	ClearCopies();
	if(handle)
		CloseLibrary(handle);
	return newcore;
}
#undef CHANGE_WITH_COPY_CHECK
#undef STORE_CLONE
#undef CLEAR_CLONE

#endif //YGOPRO_BUILD_DLL
