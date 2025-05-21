#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>
#include <clocale>
#include <memory>
#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#endif

unsigned int enable_log = 0x3;
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
#if defined(FOPEN_WINDOWS_SUPPORT_UTF8)
	std::setlocale(LC_CTYPE, ".UTF-8");
#elif defined(__APPLE__)
	std::setlocale(LC_CTYPE, "UTF-8");
#elif !defined(_WIN32)
	std::setlocale(LC_CTYPE, "");
#endif
#if defined __APPLE__ && !defined YGOPRO_SERVER_MODE
	CFURLRef bundle_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFURLRef bundle_base_url = CFURLCreateCopyDeletingLastPathComponent(nullptr, bundle_url);
	CFStringRef bundle_ext = CFURLCopyPathExtension(bundle_url);
	if (bundle_ext) {
		char path[PATH_MAX];
		if (CFStringCompare(bundle_ext, CFSTR("app"), kCFCompareCaseInsensitive) == kCFCompareEqualTo
			&& CFURLGetFileSystemRepresentation(bundle_base_url, true, (UInt8*)path, PATH_MAX)) {
			chdir(path);
		}
		CFRelease(bundle_ext);
	}
	CFRelease(bundle_url);
	CFRelease(bundle_base_url);
#endif //__APPLE__
#ifdef _WIN32
	if (argc == 2 && (ygo::IsExtension(argv[1], ".ydk") || ygo::IsExtension(argv[1], ".yrp"))) { // open file from explorer
		wchar_t exepath[MAX_PATH];
		GetModuleFileNameW(nullptr, exepath, MAX_PATH);
		wchar_t* p = std::wcsrchr(exepath, L'\\');
		if (p) {
			*p = 0;
			SetCurrentDirectoryW(exepath);
		}
	}
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
	ygo::server_port = 7911;
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
		ygo::server_port = atoi(argv[1]);
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
	int wargc = 0;
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
		if (wargc == 2 && std::wcslen(wargv[1]) >= 4) {
			wchar_t* pstrext = wargv[1] + std::wcslen(wargv[1]) - 4;
			if (!mywcsncasecmp(pstrext, L".ydk", 4)) {
				open_file = true;
				BufferIO::CopyWideString(wargv[1], open_file_name);
				exit_on_return = true;
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
			if (!mywcsncasecmp(pstrext, L".yrp", 4)) {
				open_file = true;
				BufferIO::CopyWideString(wargv[1], open_file_name);
				exit_on_return = true;
				ClickButton(ygo::mainGame->btnReplayMode);
				ClickButton(ygo::mainGame->btnLoadReplay);
				break;
			}
		}
		if(wargv[i][0] == L'-' && wargv[i][1] == L'e' && wargv[i][2] != L'\0') {
			ygo::dataManager.LoadDB(&wargv[i][2]);
			continue;
		}
		if(!std::wcscmp(wargv[i], L"-e")) { // extra database
			++i;
			if(i < wargc) {
				ygo::dataManager.LoadDB(wargv[i]);
			}
			continue;
		} else if(!std::wcscmp(wargv[i], L"-n")) { // nickName
			++i;
			if(i < wargc)
				ygo::mainGame->ebNickName->setText(wargv[i]);
			continue;
		} else if(!std::wcscmp(wargv[i], L"-h")) { // Host address
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinHost->setText(wargv[i]);
			continue;
		} else if(!std::wcscmp(wargv[i], L"-p")) { // host Port
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinPort->setText(wargv[i]);
			continue;
		} else if(!std::wcscmp(wargv[i], L"-w")) { // host passWord
			++i;
			if(i < wargc)
				ygo::mainGame->ebJoinPass->setText(wargv[i]);
			continue;
		} else if(!std::wcscmp(wargv[i], L"-k")) { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else if(!std::wcscmp(wargv[i], L"--deck-category")) {
			++i;
			if(i < wargc) {
				deckCategorySpecified = true;
				BufferIO::CopyWideString(wargv[i], ygo::mainGame->gameConf.lastcategory);
			}
		} else if(!std::wcscmp(wargv[i], L"-d")) { // Deck
			++i;
			if(!deckCategorySpecified)
				ygo::mainGame->gameConf.lastcategory[0] = 0;
			if(i + 1 < wargc) { // select deck
				BufferIO::CopyWideString(wargv[i], ygo::mainGame->gameConf.lastdeck);
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < wargc) {
					open_file = true;
					if(deckCategorySpecified) {
#ifdef _WIN32
						myswprintf(open_file_name, L"%ls\\%ls", ygo::mainGame->gameConf.lastcategory, wargv[i]);
#else
						myswprintf(open_file_name, L"%ls/%ls", ygo::mainGame->gameConf.lastcategory, wargv[i]);
#endif
					} else {
						BufferIO::CopyWideString(wargv[i], open_file_name);
					}
				}
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
		} else if(!std::wcscmp(wargv[i], L"-c")) { // Create host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnHostConfirm);
			break;
		} else if(!std::wcscmp(wargv[i], L"-j")) { // Join host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else if(!std::wcscmp(wargv[i], L"-r")) { // Replay
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				BufferIO::CopyWideString(wargv[i], open_file_name);
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else if(!std::wcscmp(wargv[i], L"-s")) { // Single
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				BufferIO::CopyWideString(wargv[i], open_file_name);
			}
			ClickButton(ygo::mainGame->btnSingleMode);
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
#endif //YGOPRO_SERVER_MODE
	return EXIT_SUCCESS;
}
