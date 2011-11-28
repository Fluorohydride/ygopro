#include "config.h"
#include "game.h"

ygo::Game* mainGame;

int main() {
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	timeBeginPeriod(1);
#else
#endif //_WIN32
	ygo::Game _game;
	mainGame = &_game;
	if(!mainGame->Initialize())
		return 0;
	mainGame->MainLoop();
#ifdef _WIN32
	timeEndPeriod(1);
	WSACleanup();
#else
#endif //_WIN32
	return EXIT_SUCCESS;
}
