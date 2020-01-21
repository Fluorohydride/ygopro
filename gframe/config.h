#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef _WIN32

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>

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
#endif

#ifndef TEXT
#ifdef UNICODE
#define TEXT(x) L##x
#else
#define TEXT(x) x
#endif // UNICODE
#endif

#include <irrlicht.h>
#ifdef __ANDROID__
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES/glplatform.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
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

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

extern unsigned short PRO_VERSION;
extern int enable_log;
extern bool exit_on_return;
extern bool is_from_discord;
extern bool open_file;
extern path_string open_file_name;

#endif