#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "bufferutil.h"
#include "random.h"
#include "protocols.h"

#ifdef _WIN32

#include <WinSock2.h>
#include <Windows.h>

#ifndef HAVE_SOCKLEN_T
#define HAVE_SOCKLEN_T
#define socklen_t int
#endif

#else

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

#include <wchar.h>

#endif //_WIN32

extern unsigned int proto_version;

#define PRIVILEGE_WATCH				0x1
#define PRIVILEGE_DUEL				0x2
#define PRIVILEGE_MATCHING			0x4
#define PRIVILEGE_AVATAR			0x8
#define PRIVILEGE_SLEEVE			0x10
#define PRIVILEGE_KICK_PLAYER		0x10000
#define PRIVILEGE_BAN_PLAYER		0x20000
#define PRIVILEGE_GIVE_PRIVILEGE	0x40000

#endif //_COMMON_H_
