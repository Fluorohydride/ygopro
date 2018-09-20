#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>
#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#endif

int enable_log = 0;
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

int main(int argc, char* argv[]) {
#ifndef _WIN32
	setlocale(LC_CTYPE, "UTF-8");
#endif
#ifdef __APPLE__
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
	wchar_t exepath[MAX_PATH];
	GetModuleFileNameW(NULL, exepath, MAX_PATH);
	wchar_t* p = wcsrchr(exepath, '\\');
	*p = '\0';
	SetCurrentDirectoryW(exepath);
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
	ygo::mainGame = &_game;
	if(!ygo::mainGame->Initialize())
		return 0;

#ifdef _WIN32
	wchar_t** wargv;
	int wargc;
	LPWSTR commandLine = GetCommandLineW();
	wargv = CommandLineToArgvW(commandLine, &wargc);
#else
	wchar_t wargv[argc][256];
	for(int i = 1; i < argc; ++i) {
		BufferIO::DecodeUTF8(argv[i], wargv[i]);
	}
#endif //_WIN32

	bool keep_on_return = false;
	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-' && argv[i][1] == 'e' && argv[i][2] != '\0') {
			char param[128];
			BufferIO::EncodeUTF8(&wargv[i][2], param);
			ygo::dataManager.LoadDB(param);
			continue;
		}
		if(!strcmp(argv[i], "-e")) { // extra database
			++i;
			if(i < argc) {
				char param[128];
				BufferIO::EncodeUTF8(wargv[i], param);
				ygo::dataManager.LoadDB(param);
			}
			continue;
		} else if(!strcmp(argv[i], "-n")) { // nickName
			++i;
			if(i < argc)
				ygo::mainGame->ebNickName->setText(wargv[i]);
			continue;
		} else if(!strcmp(argv[i], "-h")) { // Host address
			++i;
			if(i < argc)
				ygo::mainGame->ebJoinHost->setText(wargv[i]);
			continue;
		} else if(!strcmp(argv[i], "-p")) { // host Port
			++i;
			if(i < argc)
				ygo::mainGame->ebJoinPort->setText(wargv[i]);
			continue;
		} else if(!strcmp(argv[i], "-w")) { // host passWord
			++i;
			if(i < argc)
				ygo::mainGame->ebJoinPass->setText(wargv[i]);
			continue;
		} else if(!strcmp(argv[i], "-k")) { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else if(!strcmp(argv[i], "-d")) { // Deck
			++i;
			if(i + 1 < argc) { // select deck
				wcscpy(ygo::mainGame->gameConf.lastdeck, wargv[i]);
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < argc) {
					open_file = true;
					wcscpy(open_file_name, wargv[i]);
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
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else if(!strcmp(argv[i], "-r")) { // Replay
			exit_on_return = !keep_on_return;
			++i;
			if(i < argc) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else if(!strcmp(argv[i], "-s")) { // Single
			exit_on_return = !keep_on_return;
			++i;
			if(i < argc) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
			}
			ClickButton(ygo::mainGame->btnSingleMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
		} else if(argc == 2 && strlen(argv[1]) >= 4) {
			char* pstrext = argv[1] + strlen(argv[1]) - 4;
			if(!mystrncasecmp(pstrext, ".ydk", 4)) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
			if(!mystrncasecmp(pstrext, ".yrp", 4)) {
				open_file = true;
				wcscpy(open_file_name, wargv[i]);
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnReplayMode);
				ClickButton(ygo::mainGame->btnLoadReplay);
				break;
			}
		}
	}
#ifdef _WIN32
	LocalFree(wargv);
#endif
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32
	return EXIT_SUCCESS;
}
