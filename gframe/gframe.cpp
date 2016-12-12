#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>

int enable_log = 1;
bool exit_on_return = false;
bool runasserver = true;

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
	if (runasserver){
	    ygo::aServerPort=7911;
		ygo::aServerPort=atoi(argv[1]);
		ygo::lflist=atoi(argv[2]);
		ygo::start_hand=0;
		ygo::replay_mode=0;
		if (argc>2) {
			ygo::rule=atoi(argv[3]);
			ygo::mode=atoi(argv[4]);
			if (argv[5][0]=='T')
				ygo::enable_priority=true;
			else
				ygo::enable_priority=false;
			if (argv[6][0]=='T')
				ygo::no_check_deck=true;
			else
				ygo::no_check_deck=false;
			if (argv[7][0]=='T')
				ygo::no_shuffle_deck=true;
			else
				ygo::no_shuffle_deck=false;
			ygo::start_lp=atoi(argv[8]);
			ygo::start_hand=atoi(argv[9]);
			ygo::draw_count=atoi(argv[10]);
			ygo::time_limit=atoi(argv[11]);
			if (argc>12)
				ygo::replay_mode=atoi(argv[12]);
		}
		ygo::mainGame = &_game;
		ygo::mainGame->MainServerLoop(ygo::mode, ygo::lflist);
		
		return 0;
	}
    /*
	ygo::mainGame = &_game;
	if(!ygo::mainGame->Initialize())
		return 0;

	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-' && argv[i][1] == 'e') {
#ifdef _WIN32
			wchar_t fname[260];
			MultiByteToWideChar(CP_ACP, 0, &argv[i][2], -1, fname, 260);
			char fname2[260];
			BufferIO::EncodeUTF8(fname, fname2);
			ygo::dataManager.LoadDB(fname2);
#else
			ygo::dataManager.LoadDB(&argv[i][2]);
#endif
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
    */
	return EXIT_SUCCESS;
}
