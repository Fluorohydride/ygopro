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

#if defined(_MSC_VER) || defined(__MINGW32__)
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
template<size_t N, typename... TR>
inline int mysnprintf(char(&buf)[N], const char* fmt, TR... args) {
	return std::snprintf(buf, N, fmt, args...);
}

inline FILE* mywfopen(const wchar_t* filename, const char* mode) {
	FILE* fp{};
	char fname[1024]{};
	std::mbstate_t state{};
	std::wcsrtombs(fname, &filename, sizeof fname, &state);
	fp = std::fopen(fname, mode);
	return fp;
}

#define myfopen std::fopen

#include <irrlicht.h>

constexpr uint16_t PRO_VERSION = 0x1362;
extern unsigned int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern wchar_t open_file_name[256];
extern bool bot_mode;

#endif
