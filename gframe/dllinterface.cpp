#ifdef YGOPRO_BUILD_DLL
#include <string>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#ifdef __ANDROID__
#include "Android/porting_android.h"
#endif
#include <dlfcn.h>
#endif
#include "config.h"
#include "dllinterface.h"
#include "utils.h"

#if defined(_WIN32)
#define CORENAME EPRO_TEXT("ocgcore.dll")
#elif defined(EDOPRO_MACOS)
#define CORENAME EPRO_TEXT("libocgcore.dylib")
#elif defined(__ANDROID__)
#include <fcntl.h> //open()
#include <unistd.h> //close()
struct AndroidCore {
	void* library;
	int fd;
};
#if defined(__arm__)
#define CORENAME EPRO_TEXT("libocgcorev7.so")
#elif defined(__i386__)
#define CORENAME EPRO_TEXT("libocgcorex86.so")
#elif defined(__aarch64__)
#define CORENAME EPRO_TEXT("libocgcorev8.so")
#elif defined(__x86_64__)
#define CORENAME EPRO_TEXT("libocgcorex64.so")
#endif //__arm__
#elif defined(__linux__)
#define CORENAME EPRO_TEXT("libocgcore.so")
#endif //_WIN32

#define X(type,name,...) type(*name)(__VA_ARGS__) = nullptr;
#include "ocgcore_functions.inl"
#undef X

#ifdef _WIN32
static inline void* OpenLibrary(epro::path_stringview path) {
	return LoadLibrary(fmt::format("{}" CORENAME, path).data());
}
#define CloseLibrary(core) FreeLibrary((HMODULE)core)

#define GetFunction(core, x) (decltype(x))GetProcAddress((HMODULE)core, #x)

#elif defined(__ANDROID__)

static void* OpenLibrary(epro::path_stringview path) {
	void* lib = nullptr;
	auto dest_path = porting::internal_storage + "/libocgcoreXXXXXX.so";
	auto output = mkstemps(&dest_path[0], 3);
	if(output == -1)
		return nullptr;
	auto input = open(fmt::format("{}" CORENAME, path).data(), O_RDONLY);
	if(input == -1) {
		unlink(dest_path.data());
		close(output);
		return nullptr;
	}
	ygo::Utils::FileCopyFD(input, output);
	lib = dlopen(dest_path.data(), RTLD_NOW);
	unlink(dest_path.data());
	if(!lib) {
		close(output);
		close(input);
		return nullptr;
	}
	close(input);
	auto core = new AndroidCore;
	core->library = lib;
	core->fd = output;
	return core;
}

static inline void CloseLibrary(void* core) {
	AndroidCore* acore = static_cast<AndroidCore*>(core);
	dlclose(acore->library);
	close(acore->fd);
	delete acore;
}

#define GetFunction(core, x) (decltype(x))dlsym(static_cast<AndroidCore*>(core)->library, #x)

#else

static inline void* OpenLibrary(epro::path_stringview path) {
	return dlopen(fmt::format("{}" CORENAME, path).data(), RTLD_NOW);
}

#define CloseLibrary(core) dlclose(core)

#define GetFunction(core, x) (decltype(x))dlsym(core, #x)

#endif

class Core {
#define X(type,name,...) type(*int_##name)(__VA_ARGS__);
#include "ocgcore_functions.inl"
#undef X
	void* library{ nullptr };
	bool valid{ false };
	bool enabled{ false };

	bool check_api_version() const {
		int max = 0, min = 0;
		int_OCG_GetVersion(&max, &min);
		return (max == OCG_VERSION_MAJOR) && (min == OCG_VERSION_MINOR);
	}
public:

	Core(epro::path_stringview path) {
		library = OpenLibrary(path);
		if(!library)
			return;
#define X(type,name,...) if((int_##name = GetFunction(library, name)) == nullptr) return;
#include "ocgcore_functions.inl"
#undef X
		valid = check_api_version();
	}
	~Core() {
		Disable();
		if(library) {
			CloseLibrary(library);
		}
	}
	void Enable() {
#define X(type,name,...) name = int_##name;
#include "ocgcore_functions.inl"
#undef X
		enabled = true;
	}
	void Disable() {
		if(enabled) {
#define X(type,name,...) name = nullptr;
#include "ocgcore_functions.inl"
#undef X
			enabled = false;
		}
	}
	bool IsValid() const {
		return valid;
	}
};

void* LoadOCGcore(epro::path_stringview path) {
	Core* core = new Core(path);
	if(!core->IsValid()) {
		delete core;
		return nullptr;
	}
	core->Enable();
	return core;
}

void UnloadCore(void* handle) {
	delete static_cast<Core*>(handle);
}

void* ChangeOCGcore(epro::path_stringview path, void* handle) {
	Core* newcore = new Core(path);
	if(!newcore->IsValid())
		return nullptr;
	delete static_cast<Core*>(handle);
	newcore->Enable();
	return newcore;
}

#endif //YGOPRO_BUILD_DLL
