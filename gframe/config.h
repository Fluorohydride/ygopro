#ifndef __CONFIG_H
#define __CONFIG_H

#pragma once

#define _IRR_STATIC_LIB_

#include <irrlicht.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "CGUITTFont.h"
#include "CGUIImageButton.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "bufferio.h"
#include "mymutex.h"
#include "mysignal.h"
#include "mythread.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"

#ifdef _WIN32

#include <windows.h>
#define myswprintf swprintf

#else

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>

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

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

#endif
