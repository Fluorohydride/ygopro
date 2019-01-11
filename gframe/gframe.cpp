#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>
#include <memory>
#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#endif

int enable_log = 0;
bool exit_on_return = false;
bool open_file = false;
std::wstring open_file_name = L"";

void ClickButton(irr::gui::IGUIElement* btn) {
	irr::SEvent event;
	event.EventType = irr::EET_GUI_EVENT;
	event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
	event.GUIEvent.Caller = btn;
	ygo::mainGame->device->postEventFromUser(event);
}
#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:wmainCRTStartup") 
int wmain(int wargc, wchar_t* wargv[]) {
#else
int main(int argc, char* argv[]) {
	int wargc = argc;
	auto wargv = argv;
	for(int i = 0; i < argc; ++i) {
		BufferIO::DecodeUTF8(argv[i], wargv[i]);
	}
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
#if defined(_WIN32) && !defined(_DEBUG)
	if(wargc == 2) {
		std::wstring name(wargv[1]);
		auto extension = name.substr(name.find_last_of(L"."));
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if(extension == L".ydk" || extension == L".yrp" || extension == L".yrpx") {
			wchar_t exepath[MAX_PATH];
			GetModuleFileNameW(NULL, exepath, MAX_PATH);
			wchar_t* p = wcsrchr(exepath, '\\');
			*p = '\0';
			SetCurrentDirectoryW(exepath);
		}
	}
#endif //_WIN32 && !_DEBUG
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
		return EXIT_FAILURE;
	bool keep_on_return = false;
	for(int i = 1; i < wargc; ++i) {
		std::wstring parameter(wargv[i]);
#define PARAM_CHECK(x) if(parameter == x)
#define RUN_IF(x,y) PARAM_CHECK(x) {i++; if(i < wargc) {y;} continue;}
		// Extra database
		RUN_IF(L"-e", ygo::dataManager.LoadDB(BufferIO::EncodeUTF8s(wargv[i]).c_str()))
		// Nickname
		else RUN_IF(L"-n", ygo::mainGame->ebNickName->setText(wargv[i]))
		// Host address
		else RUN_IF(L"-h", ygo::mainGame->ebJoinHost->setText(wargv[i]))
		// Host Port
		else RUN_IF(L"-p", ygo::mainGame->ebJoinPort->setText(wargv[i]))
		// Host password
		else RUN_IF(L"-w", ygo::mainGame->ebJoinPass->setText(wargv[i]))
#undef RUN_IF
		else PARAM_CHECK(L"-k") { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else PARAM_CHECK(L"-d") { // Deck
			++i;
			if(i + 1 < wargc) { // select deck
				ygo::mainGame->gameConf.lastdeck = wargv[i];
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < wargc) {
					open_file = true;
					open_file_name = wargv[i];
				}
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
		} else PARAM_CHECK(L"-c") { // Create host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnHostConfirm);
			break;
		} else PARAM_CHECK(L"-j") { // Join host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else PARAM_CHECK(L"-r") { // Replay
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				open_file_name = wargv[i];
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else PARAM_CHECK(L"-s") { // Single
			exit_on_return = !keep_on_return;
			++i;
			if(i < wargc) {
				open_file = true;
				open_file_name = wargv[i];
			}
			ClickButton(ygo::mainGame->btnSingleMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
		} else if(wargc == 2 && wcslen(wargv[1]) >= 4) {
			std::wstring name(wargv[i]);
			auto extension = name.substr(name.find_last_of(L"."));
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			if(extension == L".ydk") {
				open_file = true;
				open_file_name = name;
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
			if(extension == L".yrp" || extension == L".yrpx") {
				open_file = true;
				open_file_name = name;
				exit_on_return = !keep_on_return;
				ClickButton(ygo::mainGame->btnReplayMode);
				ClickButton(ygo::mainGame->btnLoadReplay);
				break;
			}
		}
#undef PARAM_CHECK
	}
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#endif //_WIN32
	return EXIT_SUCCESS;
}
