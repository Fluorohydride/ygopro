#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef _WIN32

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
#include <locale.h>

#define SD_BOTH 2
#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define SOCKET_ERRNO() (errno)

#include <cwchar>
#define mywcsncasecmp wcsncasecmp
#define mystrncasecmp strncasecmp
inline int _wtoi(const wchar_t * s) {
	wchar_t * endptr;
	return (int)wcstol(s, &endptr, 10);
}
#endif

#ifndef TEXT
#ifdef UNICODE
#define TEXT(x) L##x
#else
#define TEXT(x) x
#endif // UNICODE
#endif

template<size_t N, typename... TR>
inline int myswprintf(wchar_t(&buf)[N], const wchar_t* fmt, TR... args) {
	return swprintf(buf, N, fmt, args...);
}

#include <irrlicht.h>
#include <irrKlang.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <ctime>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include "bufferio.h"
#include <mutex>
#include "mysignal.h"
#include <thread>
#include <common.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include "utils.h"
#include "server_lobby.h"
#ifndef YGOPRO_BUILD_DLL
#include <ocgapi.h>
#else
#include "dllinterface.h"
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern unsigned short PRO_VERSION;
extern int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern path_string open_file_name;

#endif