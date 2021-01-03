#include <WinSock2.h>
#include <Windows.h>
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

//some implementations taken from https://sourceforge.net/projects/win2kxp/

/*
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
*/

extern "C" void __stdcall handledfreeaddrinfo(addrinfo* ai) {
	static const auto pfFreeAddrInfo = (WSPIAPI_PFREEADDRINFO)WspiapiLoad(2);
	pfFreeAddrInfo(ai);
}

extern "C" INT __stdcall handledgetaddrinfo(const char* nodename, const char* servname, const addrinfo* hints, addrinfo** res) {
	static const auto pfGetAddrInfo = (WSPIAPI_PGETADDRINFO)WspiapiLoad(0);
	auto iError = pfGetAddrInfo(nodename, servname, hints, res);
	WSASetLastError(iError);
	return iError;
}

extern "C" INT __stdcall handledgetnameinfo(const sockaddr* sa, socklen_t salen, char* host, size_t hostlen, char* serv, size_t servlen, int flags) {
	static const auto pfGetNameInfo = (WSPIAPI_PGETNAMEINFO)WspiapiLoad(1);
	const auto iError = pfGetNameInfo(sa, salen, host, hostlen, serv, servlen, flags);
	WSASetLastError(iError);
	return iError;
}

static inline bool IsUnderKernelex() {
	//ntdll.dll is loaded automatically in every windows nt process, but it seems it isn't in windows 9x
	static const bool kernelex = GetModuleHandle(__TEXT("ntdll.dll")) == nullptr;
	return kernelex;
}


/*
creates 2 functions, the stub function prefixed by handledxxx that is then exported via asm,
and internalimplxxx that is the fallback function called if the function isn't loaded at runtime
on first call GetProcAddress is called, and if the function is found, then that will be called onwards
otherwise fall back to the internal implementation
*/

#define GETFUNC(funcname) (decltype(&handled##funcname))GetProcAddress(GetModuleHandle(LIBNAME), #funcname)
#define MAKELOADER(funcname,ret,args,argnames)\
ret __stdcall internalimpl##funcname args ;\
extern "C" ret __stdcall handled##funcname args { \
	static auto basefunc = [] { \
		auto func = GETFUNC(funcname); \
		return func ? func : internalimpl##funcname; \
	}(); \
	return basefunc argnames ; \
} \
ret __stdcall internalimpl##funcname args

#define LIBNAME __TEXT("Advapi32.dll")

MAKELOADER(IsWellKnownSid, BOOL, (PSID pSid, WELL_KNOWN_SID_TYPE WellKnownSidType), (pSid, WellKnownSidType)) {
	return FALSE;
}
static CHAR* convert_from_wstring(const WCHAR* wstr) {
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
extern "C" BOOL __stdcall handledCryptAcquireContextW(HCRYPTPROV *phProv, LPCWSTR pszContainer, LPCWSTR pszProvider, DWORD dwProvType, DWORD dwFlags) {
	static auto basefunc = []()->decltype(&handledCryptAcquireContextW) {
		if(IsUnderKernelex()) {
			return [](HCRYPTPROV *phProv, LPCWSTR pszContainer, LPCWSTR pszProvider, DWORD dwProvType, DWORD dwFlags)->BOOL {
				static auto basefunc = (decltype(&CryptAcquireContextA))GetProcAddress(GetModuleHandle(LIBNAME), "CryptAcquireContextA");
				auto container = convert_from_wstring(pszContainer);
				auto provider = convert_from_wstring(pszProvider);
				auto res = basefunc(phProv, container, provider, dwProvType, dwFlags);
				if(res == FALSE && GetLastError() == NTE_BAD_FLAGS)
					res = basefunc(phProv, container, provider, dwProvType, dwFlags & ~CRYPT_SILENT);
				free(container);
				free(provider);
				return res;
			};
		} else {
			return GETFUNC(CryptAcquireContextW);
		}
	}();
	return basefunc(phProv, pszContainer, pszProvider, dwProvType, dwFlags);
}

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
static PLDR_MODULE __stdcall GetLdrModule(LPCVOID address) {
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

static void LoaderLock(BOOL lock) {
	if(lock)
		EnterCriticalSection(NtCurrentPeb()->LoaderLock);
	else
		LeaveCriticalSection(NtCurrentPeb()->LoaderLock);
}
static HMODULE GetModuleHandleFromPtr(LPCVOID p) {
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
static void SListLock(PSLIST_HEADER ListHead) {
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

static void SListUnlock(PSLIST_HEADER ListHead) {
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

void __stdcall internalimplInitializeSListHead(PSLIST_HEADER ListHead) {
	SListLock(ListHead);
	ListHead->Next.Next = nullptr;
	ListHead->Depth = 0;
	ListHead->Sequence = 0;
	SListUnlock(ListHead);
}

/*
on first load this will be called when setting up the crt, calling GetProcAddress
at that moment will make the program crash.
*/
extern "C" void __stdcall handledInitializeSListHead(PSLIST_HEADER ListHead) {
	static decltype(&handledInitializeSListHead) basefunc = nullptr;
	static int firstrun = 0;
	if(firstrun == 1) {
		firstrun++;
		basefunc = GETFUNC(InitializeSListHead);
	} else if(firstrun == 0)
		firstrun++;
	if(basefunc)
		return basefunc(ListHead);
	return internalimplInitializeSListHead(ListHead);
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

MAKELOADER(GetNumaHighestNodeNumber, BOOL, (PULONG HighestNodeNumber), (HighestNodeNumber)) {
	*HighestNodeNumber = 0;
	return TRUE;
}

static void IncLoadCount(HMODULE hMod) {
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
BOOL GetRealOSVersion(LPOSVERSIONINFOW lpVersionInfo) {
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
		return (func && func(lpVersionInfo) == 0x00000000) || (GETFUNC(GetVersionExW))(lpVersionInfo);
	};
	if(IsUnderKernelex())
		return GetWin9xProductInfo();
	return GetWindowsVersionNotCompatMode();
}
/*
first call will be from irrlicht, no overwrite, 2nd will be from the crt,
if not spoofed, the runtime will abort as windows 2k isn't supported
*/
extern "C" BOOL __stdcall handledGetVersionExW(LPOSVERSIONINFOW lpVersionInfo) {
	static int firstrun = 0;
	if(!lpVersionInfo
	   || (lpVersionInfo->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXW) && lpVersionInfo->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
	   || !GetRealOSVersion(lpVersionInfo)) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
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

MAKELOADER(EncodePointer, PVOID, (PVOID ptr), (ptr)) {
	return (PVOID)((UINT_PTR)ptr ^ 0xDEADBEEF);
}

MAKELOADER(DecodePointer, PVOID, (PVOID ptr), (ptr)) {
	return (PVOID)((UINT_PTR)ptr ^ 0xDEADBEEF);
}

unsigned long __stdcall if_nametoindex(const char* ifname) {
	return 0;
}