#ifndef YGOPRO_SOCKET_H
#define YGOPRO_SOCKET_H

#ifdef _WIN32

#include <WinSock2.h>
#include <ws2tcpip.h>

#else

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif // _WIN32

namespace ygo {

#ifdef _WIN32

using Socket = SOCKET;
constexpr Socket INVALID_SOCKET_HANDLE = INVALID_SOCKET;
constexpr int SOCKET_RESULT_ERROR = SOCKET_ERROR;

inline int CloseSocket(Socket socket) {
	return closesocket(socket);
}

#else

using Socket = int;
constexpr Socket INVALID_SOCKET_HANDLE = -1;
constexpr int SOCKET_RESULT_ERROR = -1;

inline int CloseSocket(Socket socket) {
	return close(socket);
}

#endif // _WIN32

}

#endif // YGOPRO_SOCKET_H
