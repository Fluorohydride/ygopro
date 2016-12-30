#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>

int enable_log = 0;
bool exit_on_return = false;
bool open_file = false;
wchar_t open_file_name[256] = L"";

const char* GetParameter(const char* arg) {
#ifdef _WIN32
	wchar_t arg1[260];
	MultiByteToWideChar(CP_ACP, 0, arg, -1, arg1, 260);
	char arg2[260];
	BufferIO::EncodeUTF8(arg1, arg2);
	return arg2;
#else
	return arg;
#endif
}

const wchar_t* GetParameterW(const char* arg) {
#ifdef _WIN32
	wchar_t arg1[260];
	MultiByteToWideChar(CP_ACP, 0, arg, -1, arg1, 260);
	return arg1;
#else
	wchar_t arg2[260];
	BufferIO::DecodeUTF8(arg, arg2);
	return arg2;
#endif
}

void ClickButton(irr::gui::IGUIElement* btn) {
	irr::SEvent event;
	event.EventType = irr::EET_GUI_EVENT;
	event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
	event.GUIEvent.Caller = btn;
	ygo::mainGame->device->postEventFromUser(event);
}

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
		if(!strcmp(argv[i], "-e")) { // extra database
			++i;
			ygo::dataManager.LoadDB(GetParameter(&argv[i][0]));
		} else if(!strcmp(argv[i], "-n")) { // nickName
			++i;
			ygo::mainGame->ebNickName->setText(GetParameterW(&argv[i][0]));
		} else if(!strcmp(argv[i], "-h")) { // Host address
			++i;
			ygo::mainGame->ebJoinIP->setText(GetParameterW(&argv[i][0]));
		} else if(!strcmp(argv[i], "-p")) { // host Port
			++i;
			ygo::mainGame->ebJoinPort->setText(GetParameterW(&argv[i][0]));
		} else if(!strcmp(argv[i], "-w")) { // host passWord
			++i;
			ygo::mainGame->ebJoinPass->setText(GetParameterW(&argv[i][0]));
		} else if(!strcmp(argv[i], "-x")) { // eXit on return
			exit_on_return = true;
		} else if(!strcmp(argv[i], "-j")) { // Join host
			ClickButton(ygo::mainGame->btnLanMode);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else if(!strcmp(argv[i], "-o")) { // Open file
			open_file = true;
			++i;
			wcscpy(open_file_name, GetParameterW(&argv[i][0]));
		} else if(!strcmp(argv[i], "-d")) { // Deck
			ClickButton(ygo::mainGame->btnDeckEdit);
			break;
		} else if(!strcmp(argv[i], "-r")) { // Replay
			ClickButton(ygo::mainGame->btnReplayMode);
			ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else if(!strcmp(argv[i], "-s")) { // Single
			ClickButton(ygo::mainGame->btnServerMode);
			ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
		}
	}
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32
	return EXIT_SUCCESS;
}
