#ifndef SOCKETS_H
#define SOCKETS_H
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>

#define socklen_t int

#else //_WIN32

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SD_BOTH 2
#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define SOCKET_ERRNO() (errno)
#endif

#endif //SOCKETS_H