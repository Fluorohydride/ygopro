#ifdef YGOPRO_USE_IRRKLANG
#define CREATE_DEVICE_MSVC "?createIrrKlangDevice@irrklang@@YAPAVISoundEngine@1@W4E_SOUND_OUTPUT_DRIVER@1@HPBD1@Z"
#define CREATE_DEVICE_GCC "_ZN8irrklang20createIrrKlangDeviceENS_21E_SOUND_OUTPUT_DRIVEREiPKcS2_"
#include "irrklang_dynamic_loader.h"
#include <irrKlang.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

void* library = nullptr;
irrklang::ISoundEngine* (*createdevice)(irrklang::E_SOUND_OUTPUT_DRIVER, int, const char*, const char*) = nullptr;

namespace irrdyn {

void init() {
#ifdef _WIN32
	if(!(library = LoadLibrary(TEXT("./irrKlang.dll"))))
		return;
	if(!(createdevice = (decltype(createdevice))GetProcAddress((HMODULE)library, CREATE_DEVICE_MSVC)))
		createdevice = (decltype(createdevice))GetProcAddress((HMODULE)library, CREATE_DEVICE_GCC);
#else
#ifdef __APPLE__
	if(!(library = dlopen("./libIrrKlang.dll", RTLD_NOW)))
#else
	if(!(library = dlopen("./libIrrKlang.dylib", RTLD_NOW)))
#endif
	   return;
	createdevice = (decltype(createdevice))dlsym(library, CREATE_DEVICE_GCC);
#endif
}

void close() {
#ifdef _WIN32
	FreeLibrary((HMODULE)library);
#else
	dlclose(library);
#endif
}

irrklang::ISoundEngine* irrdyn::createIrrKlangDevice() {
	if(!::createdevice)
		return nullptr;
	return ::createdevice(irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_DEFAULT_OPTIONS, 0, IRR_KLANG_VERSION);
}
}
#endif //YGOPRO_USE_IRRKLANG