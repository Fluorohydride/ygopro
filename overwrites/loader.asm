IFDEF RAX
.data

handledif_nametoindex PROTO :DWORD
__imp_if_nametoindex DQ handledif_nametoindex
public __imp_if_nametoindex

; handledGetTickCount64 PROTO
; __imp_GetTickCount64 DQ handledGetTickCount64
; public __imp_GetTickCount64
ELSE

.model flat
.data

;windows 9x under kernelex
; handledCryptAcquireContextW  PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
; __imp__CryptAcquireContextW@20 dd handledCryptAcquireContextW
; public __imp__CryptAcquireContextW@20

; handledCryptGenRandom  PROTO STDCALL :DWORD,:DWORD,:DWORD
; __imp__CryptGenRandom@12 dd handledCryptGenRandom
; public __imp__CryptGenRandom@12

; handledCryptReleaseContext  PROTO STDCALL :DWORD,:DWORD
; __imp__CryptReleaseContext@8 dd handledCryptReleaseContext
; public __imp__CryptReleaseContext@8

;windows 2000 no service pack
handledIsWellKnownSid PROTO STDCALL :DWORD,:DWORD
__imp__IsWellKnownSid@8 dd handledIsWellKnownSid
public __imp__IsWellKnownSid@8

;windows 2000 sp4
handledGetLogicalProcessorInformation PROTO STDCALL :DWORD,:DWORD
__imp__GetLogicalProcessorInformation@8 dd handledGetLogicalProcessorInformation
public __imp__GetLogicalProcessorInformation@8

handledInterlockedFlushSList PROTO STDCALL :DWORD
__imp__InterlockedFlushSList@4 dd handledInterlockedFlushSList
public __imp__InterlockedFlushSList@4

handledInterlockedPopEntrySList PROTO STDCALL :DWORD
__imp__InterlockedPopEntrySList@4 dd handledInterlockedPopEntrySList
public __imp__InterlockedPopEntrySList@4

handledInitializeSListHead PROTO STDCALL :DWORD
__imp__InitializeSListHead@4 dd handledInitializeSListHead
public __imp__InitializeSListHead@4

handledInterlockedPushEntrySList  PROTO STDCALL :DWORD,:DWORD
__imp__InterlockedPushEntrySList@8 dd handledInterlockedPushEntrySList 
public __imp__InterlockedPushEntrySList@8

handledQueryDepthSList  PROTO STDCALL :DWORD
__imp__QueryDepthSList@4 dd handledQueryDepthSList
public __imp__QueryDepthSList@4

handledGetNumaHighestNodeNumber  PROTO STDCALL :DWORD
__imp__GetNumaHighestNodeNumber@4 dd handledGetNumaHighestNodeNumber
public __imp__GetNumaHighestNodeNumber@4

handledConvertFiberToThread  PROTO STDCALL
__imp__ConvertFiberToThread@0 dd handledConvertFiberToThread
public __imp__ConvertFiberToThread@0

handledGetModuleHandleExW PROTO STDCALL :DWORD,:DWORD,:DWORD
__imp__GetModuleHandleExW@12 dd handledGetModuleHandleExW
public __imp__GetModuleHandleExW@12

handledGetModuleHandleExA PROTO STDCALL :DWORD,:DWORD,:DWORD
__imp__GetModuleHandleExA@12 dd handledGetModuleHandleExA
public __imp__GetModuleHandleExA@12

handledGetVersionExW  PROTO STDCALL :DWORD
__imp__GetVersionExW@4 dd handledGetVersionExW
public __imp__GetVersionExW@4

; handledGetSystemTimes  PROTO STDCALL :DWORD,:DWORD,:DWORD
; __imp__GetSystemTimes@12 dd handledGetSystemTimes
; public __imp__GetSystemTimes@12

;windows xp no service pack
handledEncodePointer PROTO STDCALL :DWORD
__imp__EncodePointer@4 dd handledEncodePointer
public __imp__EncodePointer@4

handledDecodePointer PROTO STDCALL :DWORD
__imp__DecodePointer@4 dd handledDecodePointer
public __imp__DecodePointer@4

;windows xp sp 3
handledfreeaddrinfo PROTO STDCALL :DWORD
__imp__freeaddrinfo@4 dd handledfreeaddrinfo
public __imp__freeaddrinfo@4

handledgetaddrinfo PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD
__imp__getaddrinfo@16 dd handledgetaddrinfo
public __imp__getaddrinfo@16

handledgetnameinfo PROTO STDCALL :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
__imp__getnameinfo@28 dd handledgetnameinfo
public __imp__getnameinfo@28

handledif_nametoindex PROTO STDCALL :DWORD
__imp__if_nametoindex@4 dd handledif_nametoindex
public __imp__if_nametoindex@4

; handledFlsAlloc PROTO STDCALL :DWORD
; __imp__FlsAlloc@4 dd handledFlsAlloc
; public __imp__FlsAlloc@4

; handledFlsSetValue PROTO STDCALL :DWORD,:DWORD
; __imp__FlsSetValue@8 dd handledFlsSetValue
; public __imp__FlsSetValue@8

; handledFlsGetValue PROTO STDCALL :DWORD
; __imp__FlsGetValue@4 dd handledFlsGetValue
; public __imp__FlsGetValue@4

; handledFlsFree PROTO STDCALL :DWORD
; __imp__FlsFree@4 dd handledFlsFree
; public __imp__FlsFree@4

; handledGetTickCount64 PROTO STDCALL
; __imp__GetTickCount64@0 dd handledGetTickCount64
; public __imp__GetTickCount64@0

ENDIF
end
