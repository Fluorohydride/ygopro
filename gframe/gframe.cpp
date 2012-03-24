#include "config.h"
#include "game.h"
#include <event2/thread.h>

bool enable_log = false;
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
    if(argc >= 2)
        if(!strcmp(argv[1], "-debug"))
            enable_log = true;

        /*command line args:
         * -j: join host (host info from system.conf)
         * -d: deck edit
         * -r: replay */
        else if(!strcmp(argv[1], "-j") or !strcmp(argv[1], "-d") or !strcmp(argv[1], "-r")){
            exit_on_return = true;
            irr::SEvent event;
            event.EventType = irr::EET_GUI_EVENT;
            event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
            if(!strcmp(argv[1], "-j")){
                event.GUIEvent.Caller = ygo::mainGame->btnLanMode;
                ygo::mainGame->device->postEventFromUser(event);
                event.GUIEvent.Caller = ygo::mainGame->btnJoinHost;
                ygo::mainGame->device->postEventFromUser(event);
            }else if(!strcmp(argv[1], "-d")){
                event.GUIEvent.Caller = ygo::mainGame->btnDeckEdit;
                ygo::mainGame->device->postEventFromUser(event);
            }else if(!strcmp(argv[1], "-r")){
                event.GUIEvent.Caller = ygo::mainGame->btnReplayMode;
                ygo::mainGame->device->postEventFromUser(event);
                ygo::mainGame->lstReplayList->setSelected(0);
                event.GUIEvent.Caller = ygo::mainGame->btnLoadReplay;
            }
        }
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else
#endif //_WIN32
	return EXIT_SUCCESS;
}
