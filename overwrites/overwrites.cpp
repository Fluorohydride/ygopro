#ifndef _WIN64
#include <WinSock2.h>
#endif
#include <Windows.h>

#define KERNELEX 0
#define LIBGIT2_1_4 0

/*
creates 2 functions, the stub function prefixed by handledxxx that is then exported via asm,
and internalimplxxx that is the fallback function called if the function isn't loaded at runtime
on first call GetProcAddress is called, and if the function is found, then that will be called onwards
otherwise fall back to the internal implementation
*/

#define GETFUNC(funcname) (decltype(&handled##funcname))GetProcAddress(GetModuleHandle(LIBNAME), #funcname)
#define MAKELOADER(funcname,ret,args,argnames) \
ret __stdcall internalimpl##funcname args ; \
extern "C" ret __stdcall handled##funcname args; \
const auto basefunc##funcname = [] { \
	auto func = GETFUNC(funcname); \
	return func ? func : internalimpl##funcname; \
}(); \
extern "C" ret __stdcall handled##funcname args { \
	return basefunc##funcname argnames ; \
} \
ret __stdcall internalimpl##funcname args

#define MAKELOADER_WITH_CHECK(funcname,ret,args,argnames) \
ret __stdcall internalimpl##funcname args ; \
extern "C" ret __stdcall handled##funcname args; \
const auto basefunc##funcname = [] { \
	auto func = GETFUNC(funcname); \
	return func ? func : internalimpl##funcname; \
}(); \
extern "C" ret __stdcall handled##funcname args { \
	if(!basefunc##funcname) { \
		auto func = GETFUNC(funcname); \
		return (func ? func : internalimpl##funcname) argnames ; \
	} \
	return basefunc##funcname argnames; \
} \
ret __stdcall internalimpl##funcname args

#ifndef _WIN64
#define socklen_t int
#define EAI_AGAIN           WSATRY_AGAIN
#define EAI_BADFLAGS        WSAEINVAL
#define EAI_FAIL            WSANO_RECOVERY
#define EAI_FAMILY          WSAEAFNOSUPPORT
#define EAI_MEMORY          WSA_NOT_ENOUGH_MEMORY
#define EAI_NOSECURENAME    WSA_SECURE_HOST_NOT_FOUND
#define EAI_NONAME          WSAHOST_NOT_FOUND
#define EAI_NODATA          EAI_NONAME
#define EAI_SERVICE         WSATYPE_NOT_FOUND
#define EAI_SOCKTYPE        WSAESOCKTNOSUPPORT
#define EAI_IPSECPOLICY     WSA_IPSEC_NAME_POLICY_ERROR
#include <WSPiApi.h>
#include <winternl.h>
#endif

namespace {
#ifndef _WIN64
//some implementations taken from https://sourceforge.net/projects/win2kxp/

#if 0
//can't use c runtime functions as the runtime might not have been loaded yet
void ___write(const char* ch) {
	DWORD dwCount;
	static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(hOut, ch, strlen(ch), &dwCount, nullptr);
}
void ___write(const wchar_t* ch) {
	DWORD dwCount;
	static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleW(hOut, ch, wcslen(ch), &dwCount, nullptr);
}
#endif


const auto pfFreeAddrInfo = (WSPIAPI_PFREEADDRINFO)WspiapiLoad(2);
extern "C" void __stdcall handledfreeaddrinfo(addrinfo * ai) {
	pfFreeAddrInfo(ai);
}

const auto pfGetAddrInfo = (WSPIAPI_PGETADDRINFO)WspiapiLoad(0);
extern "C" INT __stdcall handledgetaddrinfo(const char* nodename, const char* servname, const addrinfo* hints, addrinfo** res) {
	auto iError = pfGetAddrInfo(nodename, servname, hints, res);
	WSASetLastError(iError);
	return iError;
}

const auto pfGetNameInfo = (WSPIAPI_PGETNAMEINFO)WspiapiLoad(1);
extern "C" INT __stdcall handledgetnameinfo(const sockaddr* sa, socklen_t salen, char* host, size_t hostlen, char* serv, size_t servlen, int flags) {
	const auto iError = pfGetNameInfo(sa, salen, host, hostlen, serv, servlen, flags);
	WSASetLastError(iError);
	return iError;
}

const bool kernelex = GetModuleHandle(__TEXT("ntdll.dll")) == nullptr;
inline bool IsUnderKernelex() {
	//ntdll.dll is loaded automatically in every windows nt process, but it seems it isn't in windows 9x
	return kernelex;
}

#define LIBNAME __TEXT("Advapi32.dll")

MAKELOADER(IsWellKnownSid, BOOL, (PSID pSid, WELL_KNOWN_SID_TYPE WellKnownSidType), (pSid, WellKnownSidType)) {
	return FALSE;
}
#if KERNELEX
CHAR* convert_from_wstring(const WCHAR* wstr) {
	if(wstr == nullptr)
		return nullptr;
	const int wstr_len = (int)wcslen(wstr);
	const int num_chars = WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, nullptr, 0, nullptr, nullptr);
	CHAR* strTo = (CHAR*)malloc((num_chars + 1) * sizeof(CHAR));
	if(strTo) {
		WideCharToMultiByte(CP_UTF8, 0, wstr, wstr_len, strTo, num_chars, nullptr, nullptr);
		strTo[num_chars] = '\0';
	}
	return strTo;
}


#define MAKELOADER_KERNELEX(funcname,ret,args,argnames) \
ret __stdcall kernelex##funcname args ; \
extern "C" ret __stdcall handled##funcname args; \
const auto basefunc##funcname = [] { \
	if (IsUnderKernelex()) \
		return kernelex##funcname; \
	else \
		return GETFUNC(funcname); \
}(); \
extern "C" ret __stdcall handled##funcname args { \
	return basefunc##funcname argnames ; \
} \
ret __stdcall kernelex##funcname args

MAKELOADER_KERNELEX(CryptAcquireContextW, BOOL, (HCRYPTPROV* phProv, LPCWSTR pszContainer, LPCWSTR pszProvider, DWORD dwProvType, DWORD dwFlags),
					(phProv, pszContainer, pszProvider, dwProvType, dwFlags)) {
	return pszContainer == nullptr && pszProvider == nullptr;
}

MAKELOADER_KERNELEX(CryptGenRandom, BOOL, (HCRYPTPROV hProv, DWORD dwLen, BYTE* pbBuffer),
					(hProv, dwLen, pbBuffer)) {
	auto RtlGenRandom = (BOOLEAN(__stdcall*)(PVOID RandomBuffer, ULONG RandomBufferLength))GetProcAddress(GetModuleHandle(LIBNAME), "SystemFunction036");
	return RtlGenRandom && RtlGenRandom(pbBuffer, dwLen);
}

MAKELOADER_KERNELEX(CryptReleaseContext, BOOL, (HCRYPTPROV hProv, DWORD dwFlags), (hProv, dwFlags)) {
	return TRUE;
}
#endif

#undef LIBNAME
#define LIBNAME __TEXT("kernel32.dll")

typedef struct _LDR_MODULE {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

typedef struct _PEB_LDR_DATA_2K {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA_2K, *PPEB_LDR_DATA_2K;

typedef struct _RTL_USER_PROCESS_PARAMETERS_2K {
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
} RTL_USER_PROCESS_PARAMETERS_2K, *PRTL_USER_PROCESS_PARAMETERS_2K;
typedef struct _PEB_2K {
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	BOOLEAN Spare;
	HANDLE Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA_2K LoaderData;
	PRTL_USER_PROCESS_PARAMETERS_2K ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	//And more but we don't care...
	DWORD reserved[0x21];
	PRTL_CRITICAL_SECTION LoaderLock;
} PEB_2K, *PPEB_2K;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _TEB_2K {
	NT_TIB NtTib;
	PVOID EnvironmentPointer;
	CLIENT_ID Cid;
	PVOID ActiveRpcInfo;
	PVOID ThreadLocalStoragePointer;
	PPEB_2K Peb;
	DWORD reserved[0x3D0];
	BOOLEAN InDbgPrint; // 0xF74
	BOOLEAN FreeStackOnTermination;
	BOOLEAN HasFiberData;
	UCHAR IdealProcessor;
} TEB_2K, *PTEB_2K;
inline PTEB_2K NtCurrentTeb2k(void) {
	return (PTEB_2K)NtCurrentTeb();
}

inline PPEB_2K NtCurrentPeb(void) {
	return NtCurrentTeb2k()->Peb;
}
PLDR_MODULE __stdcall GetLdrModule(LPCVOID address) {
	PLDR_MODULE first_mod, mod;
	first_mod = mod = (PLDR_MODULE)NtCurrentPeb()->LoaderData->InLoadOrderModuleList.Flink;
	do {
		if((ULONG_PTR)mod->BaseAddress <= (ULONG_PTR)address &&
			(ULONG_PTR)address < (ULONG_PTR)mod->BaseAddress + mod->SizeOfImage)
			return mod;
		mod = (PLDR_MODULE)mod->InLoadOrderModuleList.Flink;
	} while(mod != first_mod);
	return nullptr;
}

void LoaderLock(BOOL lock) {
	if(lock)
		EnterCriticalSection(NtCurrentPeb()->LoaderLock);
	else
		LeaveCriticalSection(NtCurrentPeb()->LoaderLock);
}
HMODULE GetModuleHandleFromPtr(LPCVOID p) {
	PLDR_MODULE pLM;
	HMODULE ret;
	LoaderLock(TRUE);
	pLM = GetLdrModule(p);
	if(pLM)
		ret = (HMODULE)pLM->BaseAddress;
	else
		ret = nullptr;
	LoaderLock(FALSE);
	return ret;
}

BYTE slist_lock[0x100];
void SListLock(PSLIST_HEADER ListHead) {
	DWORD index = (((DWORD)ListHead) >> MEMORY_ALLOCATION_ALIGNMENT) & 0xFF;

	__asm {
		mov edx, index
		mov cl, 1
		spin:	mov al, 0
				lock cmpxchg byte ptr[slist_lock + edx], cl
				jnz spin
	};
	return;
}

void SListUnlock(PSLIST_HEADER ListHead) {
	DWORD index = (((DWORD)ListHead) >> MEMORY_ALLOCATION_ALIGNMENT) & 0xFF;
	slist_lock[index] = 0;
}

//Note: ListHead->Next.N== first node
MAKELOADER(InterlockedPopEntrySList, PSLIST_ENTRY, (PSLIST_HEADER ListHead), (ListHead)) {
	PSLIST_ENTRY ret;
	SListLock(ListHead);
	if(!ListHead->Next.Next) ret = nullptr;
	else {
		ret = ListHead->Next.Next;
		ListHead->Next.Next = ret->Next;
		ListHead->Depth--;
		ListHead->Sequence++;
	}
	SListUnlock(ListHead);
	return ret;
}

MAKELOADER(InterlockedPushEntrySList, PSLIST_ENTRY, (PSLIST_HEADER ListHead, PSLIST_ENTRY ListEntry), (ListHead, ListEntry)) {
	PSLIST_ENTRY ret;
	SListLock(ListHead);
	ret = ListHead->Next.Next;
	ListEntry->Next = ret;
	ListHead->Next.Next = ListEntry;
	ListHead->Depth++;
	ListHead->Sequence++;
	SListUnlock(ListHead);
	return ret;
}

MAKELOADER(InterlockedFlushSList, PSLIST_ENTRY, (PSLIST_HEADER ListHead), (ListHead)) {
	PSLIST_ENTRY ret;
	SListLock(ListHead);
	ret = ListHead->Next.Next;
	ListHead->Next.Next = nullptr;
	ListHead->Depth = 0;
	ListHead->Sequence++;
	SListUnlock(ListHead);
	return ret;
}


MAKELOADER_WITH_CHECK(InitializeSListHead, void, (PSLIST_HEADER ListHead), (ListHead)) {
	SListLock(ListHead);
	ListHead->Next.Next = nullptr;
	ListHead->Depth = 0;
	ListHead->Sequence = 0;
	SListUnlock(ListHead);
}

MAKELOADER(QueryDepthSList, USHORT, (PSLIST_HEADER ListHead), (ListHead)) {
	PSLIST_ENTRY n;
	USHORT depth = 0;
	SListLock(ListHead);
	n = ListHead->Next.Next;
	while(n) {
		depth++;
		n = n->Next;
	}
	SListUnlock(ListHead);
	return depth;
}

MAKELOADER(ConvertFiberToThread, BOOL, (), ()) {
	PVOID Fiber;
	if(NtCurrentTeb2k()->HasFiberData) {
		NtCurrentTeb2k()->HasFiberData = FALSE;
		Fiber = NtCurrentTeb2k()->NtTib.FiberData;
		if(Fiber) {
			NtCurrentTeb2k()->NtTib.FiberData = nullptr;
			HeapFree(GetProcessHeap(), 0, Fiber);
		}
		return TRUE;
	} else {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
}

#if LIBGIT2_1_4
// Fiber local storage callback function workaround
// When fiber local storage is used, it's possible to provide
// a callback function that would be called on thread termination
// emulate the behaviour by using a thread_local storage that will
// instead be handled by the c runtime rather than by the kernel
struct Callbacker {
	PFLS_CALLBACK_FUNCTION callback{};
	DWORD m_index;
	void CallCallback() {
		if(callback)
			callback(TlsGetValue(m_index));
		callback = nullptr;
	}
	~Callbacker() {
		CallCallback();
	}
};
thread_local Callbacker tl;

MAKELOADER(FlsAlloc, DWORD, (PFLS_CALLBACK_FUNCTION lpCallback), (lpCallback)) {
	auto index = TlsAlloc();
	tl.callback = lpCallback;
	tl.m_index = index;
	return index;
}

MAKELOADER(FlsSetValue, BOOL, (DWORD dwFlsIndex, PVOID lpFlsData), (dwFlsIndex, lpFlsData)) {
	return TlsSetValue(dwFlsIndex, lpFlsData);
}

MAKELOADER(FlsGetValue, PVOID, (DWORD dwFlsIndex), (dwFlsIndex)) {
	return TlsGetValue(dwFlsIndex);
}

MAKELOADER(FlsFree, BOOL, (DWORD dwFlsIndex), (dwFlsIndex)) {
	tl.CallCallback();
	return TlsFree(dwFlsIndex);
}
using fpRtlNtStatusToDosError = ULONG(WINAPI*)(DWORD Status);
using fpNtQuerySystemInformation = NTSTATUS(WINAPI*)(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

auto pRtlNtStatusToDosError = (fpRtlNtStatusToDosError)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlNtStatusToDosError");
auto pNtQuerySystemInformation = (fpNtQuerySystemInformation)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");

MAKELOADER(GetSystemTimes, BOOL, (LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime), (lpIdleTime, lpKernelTime, lpUserTime)) {
	if(!pRtlNtStatusToDosError || !pNtQuerySystemInformation)
		return FALSE;

	//Need atleast one out parameter
	if(!lpIdleTime && !lpKernelTime && !lpUserTime)
		return FALSE;

	//Get number of processors
	SYSTEM_BASIC_INFORMATION SysBasicInfo;
	NTSTATUS status = pNtQuerySystemInformation(SystemBasicInformation, &SysBasicInfo, sizeof(SysBasicInfo), nullptr);
	if(NT_SUCCESS(status)) {
		const auto total_size = sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * SysBasicInfo.NumberOfProcessors;
		auto hHeap = GetProcessHeap();
		auto* SysProcPerfInfo = static_cast<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*>(HeapAlloc(hHeap, 0, total_size));

		if(!SysProcPerfInfo) return FALSE;

		//Get counters
		status = pNtQuerySystemInformation(SystemProcessorPerformanceInformation, SysProcPerfInfo, total_size, nullptr);

		if(NT_SUCCESS(status)) {
			LARGE_INTEGER it = { 0, 0 }, kt = { 0, 0 }, ut = { 0, 0 };
			for(int i = 0; i < SysBasicInfo.NumberOfProcessors; ++i) {
				it.QuadPart += SysProcPerfInfo[i].IdleTime.QuadPart;
				kt.QuadPart += SysProcPerfInfo[i].KernelTime.QuadPart;
				ut.QuadPart += SysProcPerfInfo[i].UserTime.QuadPart;
			}

			if(lpIdleTime) {
				lpIdleTime->dwLowDateTime = it.LowPart;
				lpIdleTime->dwHighDateTime = it.HighPart;
			}

			if(lpKernelTime) {
				lpKernelTime->dwLowDateTime = kt.LowPart;
				lpKernelTime->dwHighDateTime = kt.HighPart;
			}

			if(lpUserTime) {
				lpUserTime->dwLowDateTime = ut.LowPart;
				lpUserTime->dwHighDateTime = ut.HighPart;
			}

			HeapFree(hHeap, 0, SysProcPerfInfo);
			return TRUE;
		} else {
			HeapFree(hHeap, 0, SysProcPerfInfo);
		}
	}

	SetLastError(pRtlNtStatusToDosError(status));
	return FALSE;
}
#endif

MAKELOADER(GetNumaHighestNodeNumber, BOOL, (PULONG HighestNodeNumber), (HighestNodeNumber)) {
	*HighestNodeNumber = 0;
	return TRUE;
}

void IncLoadCount(HMODULE hMod) {
	WCHAR path[MAX_PATH];
	if(GetModuleFileNameW(hMod, path, sizeof(path)) != sizeof(path))
		LoadLibraryW(path);
}

MAKELOADER(GetModuleHandleExW, BOOL, (DWORD dwFlags, LPCWSTR lpModuleName, HMODULE* phModule), (dwFlags, lpModuleName, phModule)) {
	LoaderLock(TRUE);
	if(dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
		*phModule = GetModuleHandleFromPtr(lpModuleName);
	else
		*phModule = GetModuleHandleW(lpModuleName);

	if(*phModule == nullptr) {
		LoaderLock(FALSE);
		return FALSE;
	}
	if(!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) ||
	   dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN) {
		//How to pin? We'll just inc the LoadCount and hope
		IncLoadCount(*phModule);
	}

	LoaderLock(FALSE);
	return TRUE;
}

extern "C" BOOL __stdcall handledGetVersionExW(LPOSVERSIONINFOW lpVersionInfo);
bool GetRealOSVersion(LPOSVERSIONINFOW lpVersionInfo) {
	auto GetWin9xProductInfo = [lpVersionInfo] {
		WCHAR data[80];
		HKEY hKey;
		auto GetRegEntry = [&data, &hKey](const WCHAR* name) {
			DWORD dwRetFlag, dwBufLen{ sizeof(data) };
			return (RegQueryValueExW(hKey, name, nullptr, &dwRetFlag, (LPBYTE)data, &dwBufLen) == ERROR_SUCCESS)
				&& (dwRetFlag & REG_SZ) != 0;
		};
		if(RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
			return false;
		if(GetRegEntry(L"VersionNumber") && data[0] == L'4') {
			lpVersionInfo->dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
			lpVersionInfo->dwMajorVersion = 4;
			if(data[2] == L'9') {
				lpVersionInfo->dwMinorVersion = 90;
			} else {
				lpVersionInfo->dwMinorVersion = data[2] == L'0' ? 0 : 10;
				if(GetRegEntry(L"SubVersionNumber"))
					lpVersionInfo->szCSDVersion[1] = data[1];
			}
			RegCloseKey(hKey);
			return true;
		}
		RegCloseKey(hKey);
		return false;
	};
	auto GetWindowsVersionNotCompatMode = [lpVersionInfo] {
		using RtlGetVersionPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
		const auto func = (RtlGetVersionPtr)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "RtlGetVersion");
		if(func && func(lpVersionInfo) == 0x00000000) {
			if(lpVersionInfo->dwMajorVersion != 5 || lpVersionInfo->dwMinorVersion != 0)
				return true;
		}
		return !!(GETFUNC(GetVersionExW))(lpVersionInfo);
	};
	if(IsUnderKernelex())
		return GetWin9xProductInfo();
	return GetWindowsVersionNotCompatMode();
}

const OSVERSIONINFOEXW system_version = []() {
	OSVERSIONINFOEXW ret{};
	ret.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
	if(!GetRealOSVersion(reinterpret_cast<OSVERSIONINFOW*>(&ret))) {
		ret.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetRealOSVersion(reinterpret_cast<OSVERSIONINFO*>(&ret))) {
			ret.dwOSVersionInfoSize = 0;
			return ret;
		}
	}
	return ret;
}();

/*
first call will be from irrlicht, no overwrite, 2nd will be from the crt,
if not spoofed, the runtime will abort as windows 2k isn't supported
*/
int firstrun = 0;
extern "C" BOOL __stdcall handledGetVersionExW(LPOSVERSIONINFOW lpVersionInfo) {
	if(!lpVersionInfo
	   || (lpVersionInfo->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXW) && lpVersionInfo->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
	   || (lpVersionInfo->dwOSVersionInfoSize > system_version.dwOSVersionInfoSize)) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	memcpy(lpVersionInfo, &system_version, lpVersionInfo->dwOSVersionInfoSize);
	//spoof win2k to the c runtime
	if(firstrun == 1 && lpVersionInfo->dwMajorVersion <= 5) {
		firstrun++;
		lpVersionInfo->dwMajorVersion = 5; //windows xp
		lpVersionInfo->dwMinorVersion = 1;
	} else if(firstrun == 0)
		firstrun++;
	return TRUE;
}

MAKELOADER(GetModuleHandleExA, BOOL, (DWORD dwFlags, LPCSTR lpModuleName, HMODULE* phModule), (dwFlags, lpModuleName, phModule)) {
	if(!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) && lpModuleName) {
		LoaderLock(TRUE);
		*phModule = GetModuleHandleA(lpModuleName);
		if(*phModule == nullptr) {
			LoaderLock(FALSE);
			return FALSE;
		}
		if(!(dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) ||
		   dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN) {
			//How to pin? We'll just inc the LoadCount and hope
			IncLoadCount(*phModule);
		}
		LoaderLock(FALSE);
		return TRUE;
	}
	return internalimplGetModuleHandleExW(dwFlags, (LPCWSTR)lpModuleName, phModule);
}

MAKELOADER(GetLogicalProcessorInformation, BOOL, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer, PDWORD ReturnedLength), (Buffer, ReturnedLength)) {
	*ReturnedLength = 0;
	SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
	return FALSE;
}

MAKELOADER_WITH_CHECK(EncodePointer, PVOID, (PVOID ptr), (ptr)) {
	return (PVOID)((UINT_PTR)ptr ^ 0xDEADBEEF);
}

MAKELOADER(DecodePointer, PVOID, (PVOID ptr), (ptr)) {
	return (PVOID)((UINT_PTR)ptr ^ 0xDEADBEEF);
}
#endif

#if LIBGIT2_1_4
#undef LIBNAME
#define LIBNAME __TEXT("kernel32.dll")
/* We need the initial tick count to detect if the tick
 * count has rolled over. */
DWORD initial_tick_count = GetTickCount();
MAKELOADER(GetTickCount64, ULONGLONG, (), ()) {
	/* GetTickCount returns the number of milliseconds that have
	 * elapsed since the system was started. */
	DWORD count = GetTickCount();
	if(count < initial_tick_count) {
		/* The tick count has rolled over - adjust for it. */
		count = (0xFFFFFFFFu - initial_tick_count) + count;
	}
	return static_cast<DWORD>(static_cast<double>(count) / 1000.0);
}
#endif

extern "C" ULONG __stdcall handledif_nametoindex(PCSTR* InterfaceName) {
	return 0;
}

extern "C" ULONG __stdcall if_nametoindex(PCSTR* InterfaceName) {
	return 0;
}
}
