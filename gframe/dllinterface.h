#ifndef DLL_INTERFACE_H_
#define DLL_INTERFACE_H_
#ifndef YGOPRO_BUILD_DLL
#include <ocgapi.h>
#else
#include "ocgapi_types.h"
#include <string>
bool ReloadCore(void* handle);
void UnloadCore(void *handle);
#ifdef UNICODE
void* LoadOCGcore(const std::wstring& path);
void* ChangeOCGcore(const std::wstring& path, void* handle);
#else
void* LoadOCGcore(const std::string& path);
void* ChangeOCGcore(const std::string& path, void* handle);
#endif

extern void(*OCG_GetVersion)(int* major, int* minor);

extern int(*OCG_CreateDuel)(OCG_Duel* duel, OCG_DuelOptions options);
extern void(*OCG_DestroyDuel)(OCG_Duel duel);
extern void(*OCG_DuelNewCard)(OCG_Duel duel, OCG_NewCardInfo info);
extern int(*OCG_StartDuel)(OCG_Duel duel);

extern int(*OCG_DuelProcess)(OCG_Duel duel);
extern void*(*OCG_DuelGetMessage)(OCG_Duel duel, uint32_t* length);
extern void(*OCG_DuelSetResponse)(OCG_Duel duel, void* buffer, uint32_t length);
extern int(*OCG_LoadScript)(OCG_Duel duel, const char* buffer, uint32_t length, const char* name);

extern uint32_t(*OCG_DuelQueryCount)(OCG_Duel duel, uint8_t team, uint32_t loc);
extern void*(*OCG_DuelQuery)(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info);
extern void*(*OCG_DuelQueryLocation)(OCG_Duel duel, uint32_t* length, OCG_QueryInfo info);
extern void*(*OCG_DuelQueryField)(OCG_Duel duel, uint32_t* length);

#endif //YGOPRO_BUILD_DLL
#endif /* DLL_INTERFACE_H_ */