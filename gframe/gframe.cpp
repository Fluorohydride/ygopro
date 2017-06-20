#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>

int enable_log = 1;
bool exit_on_return = false;
bool runasserver = true;
bool keep_on_return = false;
bool open_file = false;
wchar_t open_file_name[256] = L"";

void GetParameter(char* param, const char* arg) {
#ifdef _WIN32
	wchar_t arg1[260];
	MultiByteToWideChar(CP_ACP, 0, arg, -1, arg1, 260);
	BufferIO::EncodeUTF8(arg1, param);
#else
	strcpy(param, arg);
#endif
}
void GetParameterW(wchar_t* param, const char* arg) {
#ifdef _WIN32
	MultiByteToWideChar(CP_ACP, 0, arg, -1, param, 260);
#else
	BufferIO::DecodeUTF8(arg, param);
#endif
}
/*
void ClickButton(irr::gui::IGUIElement* btn) {
	irr::SEvent event;
	event.EventType = irr::EET_GUI_EVENT;
	event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
	event.GUIEvent.Caller = btn;
	ygo::mainGame->device->postEventFromUser(event);
}
*/
int main(int argc, char* argv[]) {
#ifdef _WIN32
	wchar_t exepath[MAX_PATH];
	GetModuleFileNameW(NULL, exepath, MAX_PATH);
	wchar_t* p = wcsrchr(exepath, '\\');
	*p = '\0';
	SetCurrentDirectoryW(exepath);
#endif //_WIN32
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
				ygo::duel_rule=DEFAULT_DUEL_RULE-1;
			else
				ygo::duel_rule=DEFAULT_DUEL_RULE;
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

	bool keep_on_return = false;
	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-' && argv[i][1] == 'e') {
			char param[128];
			GetParameter(param, &argv[i][2]);
			ygo::dataManager.LoadDB(param);
		}
		if(!strcmp(argv[i], "-e")) { // extra database
			++i;
			char param[128];
			GetParameter(param, &argv[i][0]);
			ygo::dataManager.LoadDB(param);
			continue;
		} else if(!strcmp(argv[i], "-n")) { // nickName
			++i;
			wchar_t param[128];
			GetParameterW(param, &argv[i][0]);
			ygo::mainGame->ebNickName->setText(param);
			continue;
		} else if(!strcmp(argv[i], "-h")) { // Host address
			++i;
			wchar_t param[128];
			GetParameterW(param, &argv[i][0]);
			ygo::mainGame->ebJoinHost->setText(param);
			continue;
		} else if(!strcmp(argv[i], "-p")) { // host Port
			++i;
			wchar_t param[128];
			GetParameterW(param, &argv[i][0]);
			ygo::mainGame->ebJoinPort->setText(param);
			continue;
		} else if(!strcmp(argv[i], "-w")) { // host passWord
			++i;
			wchar_t param[128];
			GetParameterW(param, &argv[i][0]);
			ygo::mainGame->ebJoinPass->setText(param);
			continue;
		} else if(!strcmp(argv[i], "-k")) { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else if(!strcmp(argv[i], "-d")) { // Deck
			if(i + 2 < argc) { // select deck
				++i;
				GetParameterW(ygo::mainGame->gameConf.lastdeck, &argv[i][0]);
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < argc) {
					open_file = true;
					GetParameterW(open_file_name, &argv[i + 1][0]);
				}
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
		} else if(!strcmp(argv[i], "-c")) { // Create host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnHostConfirm);
			break;
		} else if(!strcmp(argv[i], "-j")) { // Join host
			exit_on_return = !keep_on_return;
			ClickButton(ygo::mainGame->btnLanMode);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else if(!strcmp(argv[i], "-r")) { // Replay
			exit_on_return = !keep_on_return;
			if(i < argc) {
				open_file = true;
				GetParameterW(open_file_name, &argv[i + 1][0]);
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else if(!strcmp(argv[i], "-s")) { // Single
			exit_on_return = !keep_on_return;
			if(i < argc) {
				open_file = true;
				GetParameterW(open_file_name, &argv[i + 1][0]);
			}
			ClickButton(ygo::mainGame->btnServerMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
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
