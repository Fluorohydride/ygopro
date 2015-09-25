#include "../common/common.h"

Random globalRandom;
unsigned int proto_version = 0x2000;

int main(int argc, char* argv[])
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#else
	evthread_use_pthreads();
#endif

	ygopro::PlayerServer playerServer;
	playerServer.Run(8848);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
