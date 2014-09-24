#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <list>
#include <array>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "singleton.h"
#include "bufferutil.h"
#include "convert.h"
#include "random.h"
#include "command.h"
#include "hash.h"
#include "timer.h"
#include "animator.h"
#include "textfile.h"

#ifdef _WIN32

#include <winsock2.h>
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

#endif //_COMMON_H_
