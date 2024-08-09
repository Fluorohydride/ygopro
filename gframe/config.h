#ifndef YGOPRO_CONFIG_H
#define YGOPRO_CONFIG_H

#define _IRR_STATIC_LIB_
#define IRR_COMPILE_WITH_DX9_DEV_PACK
#ifdef _WIN32

#define NOMINMAX
#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#define mywcsncasecmp _wcsnicmp
#define mystrncasecmp _strnicmp
#else
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
#endif

#define socklen_t int

#else //_WIN32

#include <errno.h>
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

template<size_t N, typename... TR>
inline int myswprintf(wchar_t(&buf)[N], const wchar_t* fmt, TR... args) {
	return swprintf(buf, N, fmt, args...);
}

#include <irrlicht.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <iostream>
#include <algorithm>
#include <string>
#include "bufferio.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/common.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern const unsigned short PRO_VERSION;
extern unsigned int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern wchar_t open_file_name[256];
extern bool bot_mode;

#endif
