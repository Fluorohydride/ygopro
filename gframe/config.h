#ifndef YGOPRO_CONFIG_H
#define YGOPRO_CONFIG_H

#define _IRR_STATIC_LIB_
#define IRR_COMPILE_WITH_DX9_DEV_PACK

#include <cerrno>

#ifdef _WIN32

#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#if defined(_MSC_VER) or defined(__MINGW32__)
#define mywcsncasecmp _wcsnicmp
#define mystrncasecmp _strnicmp
#else
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
#endif

#define socklen_t int

#else //_WIN32

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SD_BOTH 2
#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define SOCKET_ERRNO() (errno)

#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include "bufferio.h"
#include "../ocgcore/ocgapi.h"

template<size_t N, typename... TR>
inline int myswprintf(wchar_t(&buf)[N], const wchar_t* fmt, TR... args) {
	return std::swprintf(buf, N, fmt, args...);
}

inline FILE* mywfopen(const wchar_t* filename, const char* mode) {
	FILE* fp{};
#ifdef _WIN32
	wchar_t wmode[20]{};
	BufferIO::CopyCharArray(mode, wmode);
	fp = _wfopen(filename, wmode);
#else
	char fname[1024]{};
	BufferIO::EncodeUTF8(filename, fname);
	fp = std::fopen(fname, mode);
#endif
	return fp;
}

#if !defined(_WIN32)
#define myfopen std::fopen
#elif defined(WDK_NTDDI_VERSION) && (WDK_NTDDI_VERSION >= 0x0A000005) // Redstone 4, Version 1803, Build 17134.
#define FOPEN_WINDOWS_SUPPORT_UTF8
#define myfopen std::fopen
#else
inline FILE* myfopen(const char* filename, const char* mode) {
	wchar_t wfilename[256]{};
	BufferIO::DecodeUTF8(filename, wfilename);
	wchar_t wmode[20]{};
	BufferIO::CopyCharArray(mode, wmode);
	return _wfopen(wfilename, wmode);
}
#endif

#include <irrlicht.h>

extern const unsigned short PRO_VERSION;
extern unsigned int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern wchar_t open_file_name[256];
extern bool bot_mode;

#endif
