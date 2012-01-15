#include "config.h"
#include "game.h"
#include <event2/thread.h>

int main() {
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif //_WIN32
	ygo::Game _game;
	ygo::mainGame = &_game;
	if(!ygo::mainGame->Initialize())
		return 0;
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else
#endif //_WIN32
	return EXIT_SUCCESS;
}
