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
#if !defined(_WIN32)
	std::setlocale(LC_CTYPE, "UTF-8");
#elif defined(FOPEN_WINDOWS_SUPPORT_UTF8)
	setlocale(LC_CTYPE, ".UTF8");
#endif
#ifdef __APPLE__
	CFURLRef bundle_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFURLRef bundle_base_url = CFURLCreateCopyDeletingLastPathComponent(nullptr, bundle_url);
	CFRelease(bundle_url);
	CFStringRef path = CFURLCopyFileSystemPath(bundle_base_url, kCFURLPOSIXPathStyle);
	CFRelease(bundle_base_url);
	chdir(CFStringGetCStringPtr(path, kCFStringEncodingUTF8));
	CFRelease(path);
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
	return EXIT_SUCCESS;
}
