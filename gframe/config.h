#ifndef __CONFIG_H
#define __CONFIG_H

#pragma once

#define _IRR_STATIC_LIB_
#define IRR_COMPILE_WITH_DX9_DEV_PACK
#ifdef _WIN32

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#define myswprintf _swprintf
#else
#define myswprintf swprintf
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

#include <wchar.h>
#define myswprintf(buf, fmt, ...) swprintf(buf, 4096, fmt, ##__VA_ARGS__)
inline int _wtoi(const wchar_t * s) {
	wchar_t * endptr;
	return (int)wcstol(s, &endptr, 10);
}
#endif

#include <irrlicht.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "CGUITTFont.h"
#include "CGUIImageButton.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "bufferio.h"
#include "mymutex.h"
#include "mysignal.h"
#include "mythread.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern const unsigned short PRO_VERSION;
extern int enable_log;
extern bool exit_on_return;
extern bool open_file;
extern wchar_t open_file_name[256];

#endif
