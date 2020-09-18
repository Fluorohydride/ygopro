#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <direct.h> //_getcwd
#include <Tchar.h> //_tgetcwd
#else
#define _tmain main
#include <unistd.h>
#endif
#include <event2/thread.h>
#include <IrrlichtDevice.h>
#include <IGUIButton.h>
#include <IGUICheckBox.h>
#include <IGUIEditBox.h>
#include <IGUIWindow.h>
#include <IGUIEnvironment.h>
#include <ISceneManager.h>
#include "client_updater.h"
#include "config.h"
#include "data_handler.h"
#include "logging.h"
#include "game.h"
#include "log.h"
#include "joystick_wrapper.h"
#ifdef __APPLE__
#import <CoreFoundation/CoreFoundation.h>
#include "osx_menu.h"
#endif

bool exit_on_return = false;
bool is_from_discord = false;
bool open_file = false;
path_string open_file_name = EPRO_TEXT("");
bool show_changelog = false;
ygo::Game* ygo::mainGame = nullptr;
ygo::ImageDownloader* ygo::gImageDownloader = nullptr;
ygo::DataManager* ygo::gDataManager = nullptr;
ygo::SoundManager* ygo::gSoundManager = nullptr;
ygo::GameConfig* ygo::gGameConfig = nullptr;
ygo::RepoManager* ygo::gRepoManager = nullptr;
ygo::DeckManager* ygo::gdeckManager = nullptr;
ygo::ClientUpdater* ygo::gClientUpdater = nullptr;
JWrapper* gJWrapper = nullptr;

inline void TriggerEvent(irr::gui::IGUIElement* target, irr::gui::EGUI_EVENT_TYPE type) {
	irr::SEvent event;
	event.EventType = irr::EET_GUI_EVENT;
	event.GUIEvent.EventType = type;
	event.GUIEvent.Caller = target;
	ygo::mainGame->device->postEventFromUser(event);
}

inline void ClickButton(irr::gui::IGUIElement* btn) {
	TriggerEvent(btn, irr::gui::EGET_BUTTON_CLICKED);
}

inline void SetCheckbox(irr::gui::IGUICheckBox* chk, bool state) {
	chk->setChecked(state);
	TriggerEvent(chk, irr::gui::EGET_CHECKBOX_CHANGED);
}


void CheckArguments(int argc, path_char* argv[]) {
	bool keep_on_return = false;
	for(int i = 1; i < argc; ++i) {
		path_string parameter(argv[i]);
#define PARAM_CHECK(x) if(parameter == EPRO_TEXT(x))
#define RUN_IF(x,y) PARAM_CHECK(x) {i++; if(i < argc) {y;} continue;}
#define SET_TXT(elem) ygo::mainGame->elem->setText(ygo::Utils::ToUnicodeIfNeeded(argv[i]).c_str())
		// Extra database
		RUN_IF("-e", ygo::gDataManager->LoadDB(argv[i]))
		// Nickname
		else RUN_IF("-n", SET_TXT(ebNickName))
		// Host address
		else RUN_IF("-h", SET_TXT(ebJoinHost))
		// Host Port
		else RUN_IF("-p", SET_TXT(ebJoinPort))
		// Host password
		else RUN_IF("-w", SET_TXT(ebJoinPass))
#undef RUN_IF
#undef SET_TXT
		else PARAM_CHECK("-k") { // Keep on return
			exit_on_return = false;
			keep_on_return = true;
		} else PARAM_CHECK("-m") { // Mute
			SetCheckbox(ygo::mainGame->tabSettings.chkEnableSound, false);
			SetCheckbox(ygo::mainGame->tabSettings.chkEnableMusic, false);
		} else PARAM_CHECK("-d") { // Deck
			++i;
			if(i + 1 < argc) { // select deck
				ygo::gGameConfig->lastdeck = ygo::Utils::ToUnicodeIfNeeded(argv[i]);
				continue;
			} else { // open deck
				exit_on_return = !keep_on_return;
				if(i < argc) {
					open_file = true;
					open_file_name = argv[i];
				}
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
		} else PARAM_CHECK("-c") { // Create host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnHostConfirm);
			break;
		} else PARAM_CHECK("-j") { // Join host
			exit_on_return = !keep_on_return;
			ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
			ClickButton(ygo::mainGame->btnJoinHost);
			break;
		} else PARAM_CHECK("-r") { // Replay
			exit_on_return = !keep_on_return;
			++i;
			if(i < argc) {
				open_file = true;
				open_file_name = argv[i];
			}
			ClickButton(ygo::mainGame->btnReplayMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadReplay);
			break;
		} else PARAM_CHECK("-s") { // Single
			exit_on_return = !keep_on_return;
			++i;
			if(i < argc) {
				open_file = true;
				open_file_name = argv[i];
			}
			ClickButton(ygo::mainGame->btnSingleMode);
			if(open_file)
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
			break;
		} else if(argc == 2 && path_string(argv[1]).size() >= 4) {
			parameter = argv[1];
			auto extension = ygo::Utils::GetFileExtension(parameter);
			if(extension == EPRO_TEXT("ydk")) {
				open_file = true;
				open_file_name = std::move(parameter);
				keep_on_return = true;
				exit_on_return = false;
				ClickButton(ygo::mainGame->btnDeckEdit);
				break;
			}
			if(extension == EPRO_TEXT("yrp") || extension == EPRO_TEXT("yrpx")) {
				open_file = true;
				open_file_name = std::move(parameter);
				keep_on_return = true;
				exit_on_return = false;
				ClickButton(ygo::mainGame->btnReplayMode);
				ClickButton(ygo::mainGame->btnLoadReplay);
				break;
			}
			if(extension == EPRO_TEXT("lua")) {
				open_file = true;
				open_file_name = std::move(parameter);
				keep_on_return = true;
				exit_on_return = false;
				ClickButton(ygo::mainGame->btnSingleMode);
				ClickButton(ygo::mainGame->btnLoadSinglePlay);
				break;
			}
		}
#undef PARAM_CHECK
	}
}


#ifdef _WIN32
#define Cleanup() WSACleanup()
#else
#define Cleanup() ((void)0)
#endif //_WIN32


int _tmain(int argc, path_char* argv[]) {
#ifdef __APPLE__
	CFURLRef bundle_url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef bundle_path = CFURLCopyFileSystemPath(bundle_url, kCFURLPOSIXPathStyle);
	CFURLRef bundle_base_url = CFURLCreateCopyDeletingLastPathComponent(NULL, bundle_url);
	CFRelease(bundle_url);
	CFStringRef path = CFURLCopyFileSystemPath(bundle_base_url, kCFURLPOSIXPathStyle);
	CFRelease(bundle_base_url);
#ifdef MAC_OS_DISCORD_LAUNCHER
	system(fmt::format("open {}/Contents/MacOS/discord-launcher.app --args random", CFStringGetCStringPtr(bundle_path, kCFStringEncodingUTF8)).c_str());
#endif
	chdir(CFStringGetCStringPtr(path, kCFStringEncodingUTF8));
	CFRelease(path);
	CFRelease(bundle_path);
#endif //__APPLE__
	bool is_in_sys32 = false;
#ifdef _WIN32
	path_char* buffer;
	if((buffer = _tgetcwd(NULL, 0))) {
		auto workdir = ygo::Utils::ToUpperNoAccents(ygo::Utils::GetFileName<path_string>(buffer));
		is_in_sys32 = workdir == EPRO_TEXT("SYSTEM32");
		free(buffer);
	}
#endif
	if(argc >= 2 && argv[1] == path_string(EPRO_TEXT("show_changelog"))) {
		show_changelog = true;
	}
	if(argc >= 2 || is_in_sys32) {
		if(!is_in_sys32 && argv[1] == path_string(EPRO_TEXT("from_discord"))) {
			is_from_discord = true;
#if defined(_WIN32)
			SetCurrentDirectory(argv[2]);
#if !defined(_DEBUG)
		} else {
			if(!is_in_sys32) {
				auto extension = ygo::Utils::GetFileExtension<path_string>(argv[1]);
				is_in_sys32 = (extension == EPRO_TEXT("ydk") || extension == EPRO_TEXT("yrp") || extension == EPRO_TEXT("yrpx") || extension == EPRO_TEXT("lua"));
			}
			if(is_in_sys32) {
				SetCurrentDirectory(ygo::Utils::GetExeFolder().data());
			}
#endif //_DEBUG
#endif //_WIN32
		}
	}
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	setlocale(LC_CTYPE, "UTF-8");
	evthread_use_pthreads();
#endif //_WIN32
	ygo::ClientUpdater updater;
	ygo::gClientUpdater = &updater;
	std::shared_ptr<ygo::DataHandler> data = nullptr;
	try {
		data = std::make_shared<ygo::DataHandler>();
		ygo::gImageDownloader = data->imageDownloader.get();
		ygo::gDataManager = data->dataManager.get();
		ygo::gSoundManager = data->sounds.get();
		ygo::gGameConfig = data->configs.get();
		ygo::gRepoManager = data->gitManager.get();
		ygo::gdeckManager = data->deckManager.get();
	}
	catch(std::exception e) {
		ygo::ErrorLog(e.what());
		Cleanup();
		return EXIT_FAILURE;
	}
	if (!data->configs->noClientUpdates)
		updater.CheckUpdates();
#ifdef _WIN32
	if(!data->configs->showConsole)
		FreeConsole();
#endif
#ifdef __APPLE__
	EDOPRO_SetupMenuBar([]() {
		ygo::gGameConfig->fullscreen = !ygo::gGameConfig->fullscreen;
		ygo::mainGame->gSettings.chkFullscreen->setChecked(ygo::gGameConfig->fullscreen);
	});
#endif
	srand(time(0));
	std::unique_ptr<JWrapper> joystick;
	bool reset = false;
	bool firstlaunch = true;
	do {
		ygo::Game _game{};
		ygo::mainGame = &_game;
		if(data->tmp_device) {
			ygo::mainGame->device = data->tmp_device;
			data->tmp_device = nullptr;
		}
		if(!ygo::mainGame->Initialize()) {
			Cleanup();
			return EXIT_FAILURE;
		}
		if(firstlaunch) {
			joystick = std::unique_ptr<JWrapper>(new JWrapper(ygo::mainGame->device));
			gJWrapper = joystick.get();
			firstlaunch = false;
			CheckArguments(argc, argv);
		}
		reset = ygo::mainGame->MainLoop();
		if(reset) {
			data->tmp_device = ygo::mainGame->device;
			data->tmp_device->setEventReceiver(nullptr);
			/*the gles drivers have an additional cache, that isn't cleared when the textures are removed,
			since it's not a big deal clearing them, as they'll be reused, they aren't cleared*/
			/*data->tmp_device->getVideoDriver()->removeAllTextures();*/
			data->tmp_device->getVideoDriver()->removeAllHardwareBuffers();
			data->tmp_device->getVideoDriver()->removeAllOcclusionQueries();
			data->tmp_device->getSceneManager()->clear();
			data->tmp_device->getGUIEnvironment()->clear();
		}
	} while(reset);
	Cleanup();
	return EXIT_SUCCESS;
}
