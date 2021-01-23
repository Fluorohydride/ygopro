#ifdef YGOPRO_USE_IRRKLANG
#include "irrklang_dynamic_loader.h"
#include <irrKlang.h>
#include <stdexcept>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

#define CREATE_DEVICE_MSVC "?createIrrKlangDevice@irrklang@@YAPAVISoundEngine@1@W4E_SOUND_OUTPUT_DRIVER@1@HPBD1@Z"
#define CREATE_DEVICE_GCC "_ZN8irrklang20createIrrKlangDeviceENS_21E_SOUND_OUTPUT_DRIVEREiPKcS2_"

KlangLoader::KlangLoader() {
#ifndef IRRKLANG_STATIC
#ifdef _WIN32
	library = LoadLibrary(TEXT("./irrKlang.dll"));
#elif defined(__APPLE__)
	library = dlopen("./libIrrKlang.dylib", RTLD_NOW);
#elif defined(__linux__)
	library = dlopen("./libIrrKlang.so", RTLD_NOW);
#endif
	if(library == nullptr)
		throw std::runtime_error("Failed to load irrklang library");
#ifdef _WIN32
	if(!(createdevice = (CreateDevice)GetProcAddress((HMODULE)library, CREATE_DEVICE_MSVC)))
		createdevice = (CreateDevice)GetProcAddress((HMODULE)library, CREATE_DEVICE_GCC);
#else
	createdevice = (CreateDevice)dlsym(library, CREATE_DEVICE_GCC);
#endif
	if(createdevice == nullptr)
		throw std::runtime_error("Failed to load createIrrKlangDevice function");
#else
	createdevice = irrklang::createIrrKlangDevice;
#endif
}

KlangLoader::~KlangLoader() {
	if(!library)
		return;
#ifdef _WIN32
	FreeLibrary((HMODULE)library);
#else
	dlclose(library);
#endif
}

irrklang::ISoundEngine* KlangLoader::createIrrKlangDevice() {
	auto engine = createdevice(irrklang::ESOD_AUTO_DETECT, irrklang::ESEO_DEFAULT_OPTIONS, 0, IRR_KLANG_VERSION);
	if(!engine)
		return nullptr;
#ifdef IRRKLANG_STATIC
	ikpMP3Init(engine);
#endif
	return engine;
}

#endif //YGOPRO_USE_IRRKLANG