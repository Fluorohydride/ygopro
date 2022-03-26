#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Tchar.h> //_tmain
#else
#define _tmain main
#include <unistd.h>
#endif
#include <cstdio>
#include <curl/curl.h>
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
#include "utils_gui.h"
#ifdef EDOPRO_MACOS
#include "osx_menu.h"
#endif

bool is_from_discord = false;
bool open_file = false;
epro::path_string open_file_name = EPRO_TEXT("");
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

namespace {
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

enum LAUNCH_PARAM {
	WORK_DIR,
	MUTE,
	CHANGELOG,
	DISCORD,
	OVERRIDE_UPDATE_URL,
	COUNT,
};

LAUNCH_PARAM GetOption(epro::path_char option) {
	switch(static_cast<char>(option)) {
	case 'C': return LAUNCH_PARAM::WORK_DIR;
	case 'm': return LAUNCH_PARAM::MUTE;
	case 'l': return LAUNCH_PARAM::CHANGELOG;
	case 'D': return LAUNCH_PARAM::DISCORD;
	case 'u': return LAUNCH_PARAM::OVERRIDE_UPDATE_URL;
	default: return LAUNCH_PARAM::COUNT;
	}
}

struct Option {
	bool enabled{ false };
	epro::path_stringview argument;
};

using args_t = std::array<Option, LAUNCH_PARAM::COUNT>;

args_t ParseArguments(int argc, epro::path_char* argv[]) {
	args_t res;
	for(int i = 1; i < argc; ++i) {
		epro::path_stringview parameter = argv[i];
		if(parameter.size() < 2)
			break;
		if(parameter[0] == EPRO_TEXT('-')) {
			auto launch_param = GetOption(parameter[1]);
			if(launch_param == LAUNCH_PARAM::COUNT)
				continue;
			epro::path_stringview argument;
			if(i + 1 < argc) {
				const auto* next = argv[i + 1];
				if(next[0] != EPRO_TEXT('-')) {
					argument = next;
					i++;
				}
			}
			res[launch_param] = { true, argument };
			continue;
		} else if(parameter == EPRO_TEXT("show_changelog"))
			res[LAUNCH_PARAM::CHANGELOG] = { true };
	}
	return res;
}

void CheckArguments(const args_t& args) {
	if(args[LAUNCH_PARAM::MUTE].enabled) {
		SetCheckbox(ygo::mainGame->tabSettings.chkEnableSound, false);
		SetCheckbox(ygo::mainGame->tabSettings.chkEnableMusic, false);
	}
}

inline void ThreadsStartup() {
#ifdef _WIN32
	const WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif
	curl_global_init(CURL_GLOBAL_SSL);
}
inline void ThreadsCleanup() {
	curl_global_cleanup();
	libevent_global_shutdown();
#ifdef _WIN32
	WSACleanup();
#endif
}

//clang below version 11 (llvm version 8) has a bug with brace class initialization
//where it can't properly deduce the destructors of its members
//https://reviews.llvm.org/D45898
//https://bugs.llvm.org/show_bug.cgi?id=28280
//add a workaround to construct the game object indirectly
//to avoid constructing it with brace initialization
#if defined(__clang_major__) && __clang_major__ <= 10
class Game {
	ygo::Game game;
public:
	Game() :game() {};
	ygo::Game* operator&() { return &game; }
};
#else
using Game = ygo::Game;
#endif

}

int _tmain(int argc, epro::path_char* argv[]) {
	std::puts(EDOPRO_VERSION_STRING_DEBUG);
	const auto args = ParseArguments(argc, argv);
	{
		const auto& workdir = args[LAUNCH_PARAM::WORK_DIR];
		const epro::path_stringview dest = workdir.enabled ? workdir.argument : ygo::Utils::GetExeFolder();
		if(!ygo::Utils::SetWorkingDirectory(dest)) {
			const auto err = fmt::format("failed to change directory to: {}", ygo::Utils::ToUTF8IfNeeded(dest));
			ygo::ErrorLog(err);
			fmt::print("{}\n", err);
			ygo::GUIUtils::ShowErrorWindow("Initialization fail", err);
			return EXIT_FAILURE;
		}
	}
	show_changelog = args[LAUNCH_PARAM::CHANGELOG].enabled;
	ThreadsStartup();
#ifndef _WIN32
	setlocale(LC_CTYPE, "UTF-8");
#endif //_WIN32
	ygo::ClientUpdater updater(args[LAUNCH_PARAM::OVERRIDE_UPDATE_URL].argument);
	ygo::gClientUpdater = &updater;
	std::shared_ptr<ygo::DataHandler> data{ nullptr };
	try {
		data = std::make_shared<ygo::DataHandler>();
		ygo::gImageDownloader = data->imageDownloader.get();
		ygo::gDataManager = data->dataManager.get();
		ygo::gSoundManager = data->sounds.get();
		ygo::gGameConfig = data->configs.get();
		ygo::gRepoManager = data->gitManager.get();
		ygo::gdeckManager = data->deckManager.get();
	}
	catch(const std::exception& e) {
		epro::stringview text(e.what());
		ygo::ErrorLog(text);
		fmt::print("{}\n", text);
		ygo::GUIUtils::ShowErrorWindow("Initialization fail", text);
		ThreadsCleanup();
		return EXIT_FAILURE;
	}
	if (!data->configs->noClientUpdates)
		updater.CheckUpdates();
#ifdef _WIN32
	if(!data->configs->showConsole)
		FreeConsole();
#endif
#ifdef EDOPRO_MACOS
	EDOPRO_SetupMenuBar([]() {
		ygo::gGameConfig->fullscreen = !ygo::gGameConfig->fullscreen;
		ygo::mainGame->gSettings.chkFullscreen->setChecked(ygo::gGameConfig->fullscreen);
	});
#endif
	srand(static_cast<uint32_t>(time(nullptr)));
	std::unique_ptr<JWrapper> joystick{ nullptr };
	bool firstlaunch = true;
	bool reset = false;
	do {
		Game _game{};
		ygo::mainGame = &_game;
		if(data->tmp_device) {
			ygo::mainGame->device = data->tmp_device;
			data->tmp_device = nullptr;
		}
		try {
			ygo::mainGame->Initialize();
		}
		catch(const std::exception& e) {
			epro::stringview text(e.what());
			ygo::ErrorLog(text);
			fmt::print("{}\n", text);
			ygo::GUIUtils::ShowErrorWindow("Assets load fail", text);
			ThreadsCleanup();
			return EXIT_FAILURE;
		}
		if(firstlaunch) {
			joystick = std::unique_ptr<JWrapper>(new JWrapper(ygo::mainGame->device));
			gJWrapper = joystick.get();
			firstlaunch = false;
			CheckArguments(args);
		}
		reset = ygo::mainGame->MainLoop();
		data->tmp_device = ygo::mainGame->device;
		if(reset) {
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
	data->tmp_device->drop();
	ThreadsCleanup();
	return EXIT_SUCCESS;
}
