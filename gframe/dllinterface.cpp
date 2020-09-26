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
#include "dllinterface.h"
#include "utils.h"

#ifdef _WIN32
#define CORENAME EPRO_TEXT("ocgcore.dll")
#elif defined(__APPLE__)
#define CORENAME EPRO_TEXT("libocgcore.dylib")
#elif defined(__ANDROID__)
#if defined(__arm__)
#define CORENAME EPRO_TEXT("libocgcorev7.so")
#elif defined(__i386__)
#define CORENAME EPRO_TEXT("libocgcorex86.so")
#elif defined(__aarch64__)
#define CORENAME EPRO_TEXT("libocgcorev8.so")
#elif defined(__x86_64__)
#define CORENAME EPRO_TEXT("libocgcorex64.so")
#endif
#else
#define CORENAME EPRO_TEXT("libocgcore.so")
#endif

#define X(type,name,...) type(*name)(__VA_ARGS__) = nullptr;
#include "ocgcore_functions.inl"
#undef X

#define CREATE_CLONE(x) auto x##_copy = x;
#define STORE_CLONE(x) x##_copy = x;
#define CLEAR_CLONE(x) x##_copy = nullptr;

#define X(type,name,...) CREATE_CLONE(name)
#include "ocgcore_functions.inl"
#undef X
#undef CREATE_CLONE

#ifdef _WIN32
void* OpenLibrary(path_stringview path) {
	return LoadLibrary(fmt::format("{}" CORENAME, path).data());
}
#define CloseLibrary(core) FreeLibrary((HMODULE)core)

#define GetFunction(core, x) (decltype(x))GetProcAddress((HMODULE)core, #x)

#else

void* OpenLibrary(path_stringview path) {
#ifdef __ANDROID__
	void* lib = nullptr;
	const auto dest_dir = porting::internal_storage + "/libocgcore.so";
	ygo::Utils::FileCopy(fmt::format("{}" CORENAME, path), dest_dir);
	lib = dlopen(dest_dir.data(), RTLD_LAZY);
	ygo::Utils::FileDelete(dest_dir);
	return lib;
#else
	return dlopen(fmt::format("{}" CORENAME, path).data(), RTLD_LAZY);
#endif
}

#define CloseLibrary(core) dlclose(core)

#define GetFunction(core, x) (decltype(x))dlsym(core, #x)

#endif

#define RESTORE_CLONE(x) if(x##_copy) { x = x##_copy; x##_copy = nullptr; }

void RestoreFromCopies() {
#define X(type,name,...) RESTORE_CLONE(name)
#include "ocgcore_functions.inl"
#undef X
}

#undef RESTORE_CLONE

void ClearCopies() {
#define X(type,name,...) CLEAR_CLONE(name)
#include "ocgcore_functions.inl"
#undef X
}

bool check_api_version() {
	int min = 0;
	int max = 0;
	OCG_GetVersion(&max, &min);
	return (max == OCG_VERSION_MAJOR) && (min == OCG_VERSION_MINOR);
}

#define LOAD_FUNCTION(x) x = GetFunction(newcore, x);\
		if(!x){ UnloadCore(newcore); return nullptr; }

void* LoadOCGcore(path_stringview path) {
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
	ClearCopies();
#define X(type,name,...) LOAD_FUNCTION(name)
#include "ocgcore_functions.inl"
#undef X
	if(!check_api_version()) {
		UnloadCore(newcore);
		return nullptr;
	}
	return newcore;
}
#undef LOAD_FUNCTION

#define LOAD_WITH_COPY_CHECK(x) STORE_CLONE(x)\
		x = GetFunction(handle, x);\
		if(!x) {\
			RestoreFromCopies();\
			return false;\
		}

bool ReloadCore(void* handle) {
	if(!handle)
		return false;
#define X(type,name,...) LOAD_WITH_COPY_CHECK(name)
#include "ocgcore_functions.inl"
#undef X
	if(!check_api_version()) {
		RestoreFromCopies();
		return false;
	}
	ClearCopies();
	return true;
}

#undef LOAD_WITH_COPY_CHECK

#define CLEAR_FUNCTION(x) x = nullptr;

void UnloadCore(void* handle) {
	CloseLibrary(handle);
#define X(type,name,...) CLEAR_FUNCTION(name)
#include "ocgcore_functions.inl"
#undef X
}

#undef CLEAR_FUNCTION

#define CHANGE_WITH_COPY_CHECK(x) STORE_CLONE(x)\
		x = GetFunction(newcore, x);\
		if(!x) {\
			CloseLibrary(newcore);\
			RestoreFromCopies();\
			return nullptr;\
		}

void* ChangeOCGcore(path_stringview path, void* handle) {
	void* newcore = OpenLibrary(path);
	if(!newcore)
		return nullptr;
#define X(type,name,...) CHANGE_WITH_COPY_CHECK(name)
#include "ocgcore_functions.inl"
#undef X
	if(!check_api_version()) {
		CloseLibrary(newcore);
		RestoreFromCopies();
		return nullptr;
	}
	ClearCopies();
	if(handle)
		CloseLibrary(handle);
	return newcore;
}
#undef CHANGE_WITH_COPY_CHECK
#undef STORE_CLONE
#undef CLEAR_CLONE

#endif //YGOPRO_BUILD_DLL
