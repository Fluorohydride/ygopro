#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>
#include <memory>
#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#endif

int enable_log = 0;
#ifndef YGOPRO_SERVER_MODE
bool exit_on_return = false;
bool open_file = false;
wchar_t open_file_name[256] = L"";
bool bot_mode = false;

void ClickButton(irr::gui::IGUIElement* btn) {
	irr::SEvent event;
	event.EventType = irr::EET_GUI_EVENT;
	event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
	event.GUIEvent.Caller = btn;
	ygo::mainGame->device->postEventFromUser(event);
}
#endif //YGOPRO_SERVER_MODE

int main(int argc, char* argv[]) {
#ifndef _WIN32
	setlocale(LC_CTYPE, "UTF-8");
#endif
#if defined __APPLE__ && !defined YGOPRO_SERVER_MODE
	CFURLRef bundle_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFURLRef bundle_base_url = CFURLCreateCopyDeletingLastPathComponent(NULL, bundle_url);
	CFRelease(bundle_url);
	CFStringRef path = CFURLCopyFileSystemPath(bundle_base_url, kCFURLPOSIXPathStyle);
	CFRelease(bundle_base_url);
	chdir(CFStringGetCStringPtr(path, kCFStringEncodingUTF8));
	CFRelease(path);
#endif //__APPLE__
#ifdef _WIN32
#ifndef _DEBUG
	char* pstrext;
	if(argc == 2 && (pstrext = strrchr(argv[1], '.'))
		&& (!mystrncasecmp(pstrext, ".ydk", 4) || !mystrncasecmp(pstrext, ".yrp", 4))) {
		wchar_t exepath[MAX_PATH];
		GetModuleFileNameW(NULL, exepath, MAX_PATH);
		wchar_t* p = wcsrchr(exepath, '\\');
		*p = '\0';
		SetCurrentDirectoryW(exepath);
	}
#endif //_DEBUG
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
#ifdef YGOPRO_SERVER_MODE
	enable_log = 1;
	ygo::aServerPort = 7911;
	ygo::replay_mode = 0;
	ygo::game_info.lflist = 0;
	ygo::game_info.rule = 0;
	ygo::game_info.mode = 0;
	ygo::game_info.start_hand = 5;
	ygo::game_info.start_lp = 8000;
	ygo::game_info.draw_count = 1;
	ygo::game_info.no_check_deck = false;
	ygo::game_info.no_shuffle_deck = false;
	ygo::game_info.duel_rule = DEFAULT_DUEL_RULE;
	ygo::game_info.time_limit = 180;
	for (int i = 0; i < 3; ++i)
		ygo::pre_seed[i] = (unsigned int)0;
	if (argc > 1) {
		ygo::aServerPort = atoi(argv[1]);
		int lflist = atoi(argv[2]);
		if(lflist < 0)
			lflist = 999;
		ygo::game_info.lflist = lflist;
		ygo::game_info.rule = atoi(argv[3]);
		int mode = atoi(argv[4]);
		if(mode > 2)
			mode = 0;
		ygo::game_info.mode = mode;
		if(argv[5][0] == 'T')
			ygo::game_info.duel_rule = DEFAULT_DUEL_RULE - 1;
		else if(argv[5][0] == 'F')
			ygo::game_info.duel_rule = DEFAULT_DUEL_RULE;
		else {
			int master_rule = atoi(argv[5]);
			if(master_rule)
				ygo::game_info.duel_rule = master_rule;
			else
				ygo::game_info.duel_rule = DEFAULT_DUEL_RULE;
		}
		if(argv[6][0] == 'T')
			ygo::game_info.no_check_deck = true;
		else
			ygo::game_info.no_check_deck = false;
		if(argv[7][0] == 'T')
			ygo::game_info.no_shuffle_deck = true;
		else
			ygo::game_info.no_shuffle_deck = false;
		ygo::game_info.start_lp = atoi(argv[8]);
		ygo::game_info.start_hand = atoi(argv[9]);
		ygo::game_info.draw_count = atoi(argv[10]);
		ygo::game_info.time_limit = atoi(argv[11]);
		ygo::replay_mode = atoi(argv[12]);
		for (int i = 13; (i < argc && i <= 15) ; ++i)
		{
			ygo::pre_seed[i - 13] = (unsigned int)atol(argv[i]);
		}
	}
	ygo::mainGame = &_game;
	ygo::mainGame->MainServerLoop();
	return 0;
#else //YGOPRO_SERVER_MODE
	ygo::mainGame = &_game;
	if(!ygo::mainGame->Initialize())
		return 0;

#ifdef _WIN32
	int wargc;
	std::unique_ptr<wchar_t*[], void(*)(wchar_t**)> wargv(CommandLineToArgvW(GetCommandLineW(), &wargc), [](wchar_t** wargv) {
		LocalFree(wargv);
	});
#else
	int wargc = argc;
	auto wargv = std::make_unique<wchar_t[][256]>(wargc);
	for(int i = 0; i < argc; ++i) {
		BufferIO::DecodeUTF8(argv[i], wargv[i]);
	}
#endif //_WIN32

	bool keep_on_return = false;
	bool deckCategorySpecified = false;
	for(int i = 1; i < wargc; ++i) {
		if(wargv[i][0] == L'-' && wargv[i][1] == L'e' && wargv[i][2] != L'\0') {
			ygo::dataManager.LoadDB(&wargv[i][2]);
			continue;
		}
		if(!wcscmp(wargv[i], L"-e")) { // extra database
			++i;
			if(i < wargc) {
				ygo::dataManager.LoadDB(wargv[i]);
			}
			continue;
		} else if(!wcscmp(wargv[i], L"-n")) { // nickName
			++i;
			if(i < wargc)
				ygo::mainGame->ebNickName->setText(wargv[i]);
			continue;
		} else if(!wcscmp(wargv[i], L"-h")) { // Host address
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinHost->setText(wargv[i]);
			continue;
		} else if(!wcscmp(wargv[i], L"-p")) { // host Port
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinPort->setText(wargv[i]);
			continue;
		} else if(!wcscmp(wargv[i], L"-w")) { // host passWord
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinPass->setText(wargv[i]);
			continue;
		} else if(!wcscmp(wargv[i], L"-k")) { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else if(!wcscmp(wargv[i], L"--deck-category")) {
			++i;
			if(i < wargc) {
				deckCategorySpecified = true;
				wcscpy(ygo::mainGame->gameConf.lastcategory, wargv[i]);
			}
		} else if(!wcscmp(wargv[i], L"-d")) { // Deck
			++i;
			if(!deckCategorySpecified)
				ygo::mainGame->gameConf.lastcategory[0] = 0;
			if(i + 1 < wargc) { // select deck
				wcscpy(ygo::mainGame->gameConf.lastdeck, wargv[i]);
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < wargc) {
					open_file = true;
					if(deckCategorySpecified) {
#ifdef WIN32
						myswprintf(open_file_name, L"%ls\\%ls", ygo::mainGame->gameConf.lastcategory, wargv[i]);
#else
						myswprintf(open_file_name, L"%ls/%ls", ygo::mainGame->gameConf.lastcategory, wargv[i]);
#endif
					} else {
						wcscpy(open_file_name, wargv[i]);
					}
				}
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
		} else if(!wcscmp(wargv[i], L"-c")) { // Create host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnHostConfirm);
			break;
		} else if(!wcscmp(wargv[i], L"-j")) { // Join host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else if(!wcscmp(wargv[i], L"-r")) { // Replay
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else if(!wcscmp(wargv[i], L"-s")) { // Single
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
			}
			ClickButton(ygo::mainGame->btnSingleMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
		} else if(wargc == 2 && wcslen(wargv[1]) >= 4) {
			wchar_t* pstrext = wargv[1] + wcslen(wargv[1]) - 4;
			if(!mywcsncasecmp(pstrext, L".ydk", 4)) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
			if(!mywcsncasecmp(pstrext, L".yrp", 4)) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnReplayMode);
				ClickButton(ygo::mainGame->btnLoadReplay);
				break;
			}
		}
	}
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32
#endif //YGOPRO_SERVER_MODE
	return EXIT_SUCCESS;
}
