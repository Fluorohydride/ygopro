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

#define PARAM_CHECK(x) ((wchar_t)argv[i][1] == (wchar_t)x)
#define RUN_IF(x,expr) (PARAM_CHECK(x)) {i++; if(i < argc) {expr;} continue;}
#define SET_TXT(elem) ygo::mainGame->elem->setText(ygo::Utils::ToUnicodeIfNeeded(argv[i]).data())

void CheckArguments(int argc, path_char* argv[]) {
	bool keep_on_return = false;
	for(int i = 1; i < argc; ++i) {
		if((wchar_t)argv[i][0] == '-' && argv[i][1]) {
			// Extra database
			if RUN_IF('e', ygo::gDataManager->LoadDB(argv[i]))
				// Nickname
			else if RUN_IF('n', SET_TXT(ebNickName))
				// Host address
			else if RUN_IF('h', SET_TXT(ebJoinHost))
				// Host Port
			else if RUN_IF('p', SET_TXT(ebJoinPort))
				// Host password
			else if RUN_IF('w', SET_TXT(ebJoinPass))
			else if(PARAM_CHECK('k')) { // Keep on return
				exit_on_return = false;
				keep_on_return = true;
			} else if(PARAM_CHECK('m')) { // Mute
				SetCheckbox(ygo::mainGame->tabSettings.chkEnableSound, false);
				SetCheckbox(ygo::mainGame->tabSettings.chkEnableMusic, false);
			} else if(PARAM_CHECK('d')) { // Deck
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
			} else if(PARAM_CHECK('c')) { // Create host
				exit_on_return = !keep_on_return;
				ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
				ClickButton(ygo::mainGame->btnHostConfirm);
				break;
			} else if(PARAM_CHECK('j')) { // Join host
				exit_on_return = !keep_on_return;
				ygo::mainGame->HideElement(ygo::mainGame->wMainMenu);
				ClickButton(ygo::mainGame->btnJoinHost);
				break;
			} else if(PARAM_CHECK('r')) { // Replay
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
			} else if(PARAM_CHECK('s')) { // Single
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
			}
		} else if(argc == 2 && argv[1][0] && argv[1][1] && argv[1][2] && argv[1][3]) {
			path_string parameter = argv[1];
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
	}
}
#undef RUN_IF
#undef SET_TXT
#undef PARAM_CHECK


#ifdef _WIN32
inline void ThreadsCleanup() {
	WSACleanup();
	libevent_global_shutdown();
}
inline void ThreadsStartup() {
	const WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
}
#else
#define ThreadsCleanup() libevent_global_shutdown()
#define ThreadsStartup() evthread_use_pthreads()
#endif //_WIN32

int _tmain(int argc, path_char* argv[]) {
	path_stringview dest{};
	int skipped = 0;
	if(argc >= 2 && (argv[1] == EPRO_TEXT("from_discord"_sv) || argv[1] == EPRO_TEXT("-C"_sv))) {
		dest = argv[2];
		skipped = 2;
	} else
		dest = ygo::Utils::GetExeFolder();
	if(!ygo::Utils::ChangeDirectory(dest)) {
		ygo::ErrorLog("failed to change directory");
		fmt::print("failed to change directory\n");
	}
	if(argc >= 2 && argv[1] == EPRO_TEXT("show_changelog"_sv))
		show_changelog = true;
	ThreadsStartup();
#ifndef _WIN32
	setlocale(LC_CTYPE, "UTF-8");
#endif //_WIN32
	ygo::ClientUpdater updater;
	ygo::gClientUpdater = &updater;
	std::shared_ptr<ygo::DataHandler> data = nullptr;
	try {
		data = std::make_shared<ygo::DataHandler>(dest);
		ygo::gImageDownloader = data->imageDownloader.get();
		ygo::gDataManager = data->dataManager.get();
		ygo::gSoundManager = data->sounds.get();
		ygo::gGameConfig = data->configs.get();
		ygo::gRepoManager = data->gitManager.get();
		ygo::gdeckManager = data->deckManager.get();
	}
	catch(std::exception e) {
		ygo::ErrorLog(e.what());
		ThreadsCleanup();
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
			ThreadsCleanup();
			return EXIT_FAILURE;
		}
		if(firstlaunch) {
			joystick = std::unique_ptr<JWrapper>(new JWrapper(ygo::mainGame->device));
			gJWrapper = joystick.get();
			firstlaunch = false;
			CheckArguments(argc - skipped, argv + skipped);
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
	ThreadsCleanup();
	return EXIT_SUCCESS;
}
