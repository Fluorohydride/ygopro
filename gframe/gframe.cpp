#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>

int enable_log = 0;
bool exit_on_return = false;

int main(int argc, char* argv[]) {
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

	for(int i = 1; i < argc; ++i) {
		/*command line args:
		 * -j: join host (host info from system.conf)
		 * -d: deck edit
		 * -r: replay */
		if(argv[i][0] == '-' && argv[i][1] == 'e') {
			ygo::dataManager.LoadDB(&argv[i][2]);
		} else if(!strcmp(argv[i], "-j") || !strcmp(argv[i], "-d") || !strcmp(argv[i], "-r") || !strcmp(argv[i], "-s")) {
			exit_on_return = true;
			irr::SEvent event;
			event.EventType = irr::EET_GUI_EVENT;
			event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
			if(!strcmp(argv[i], "-j")) {
				ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
				event.GUIEvent.Caller = ygo::mainGame->btnJoinHost;
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-d")) {
				event.GUIEvent.Caller = ygo::mainGame->btnDeckEdit;
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-r")) {
				event.GUIEvent.Caller = ygo::mainGame->btnReplayMode;
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->lstReplayList->setSelected(0);
				event.GUIEvent.Caller = ygo::mainGame->btnLoadReplay;
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-s")) {
				event.GUIEvent.Caller = ygo::mainGame->btnServerMode;
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->lstSinglePlayList->setSelected(0);
				event.GUIEvent.Caller = ygo::mainGame->btnLoadSinglePlay;
				ygo::mainGame->device->postEventFromUser(event);
			}

		}
	}
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32
	return EXIT_SUCCESS;
}
