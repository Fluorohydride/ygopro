#include "config.h"
#include "game.h"

int main() {
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
#else
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
