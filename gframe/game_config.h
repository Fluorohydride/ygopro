#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <nlohmann/json.hpp>
#include "text_types.h"

namespace ygo {

enum CoreLogOutput {
	CORE_LOG_NONE = 0x0,
	CORE_LOG_TO_CHAT = 0x1,
	CORE_LOG_TO_FILE = 0x2
};

struct GameConfig
{
	GameConfig();
	bool Load(const path_char* filename);
	bool Save(const path_char* filename);

	bool use_d3d = true;
	bool vsync = true;
	std::string windowStruct = "";
	float dpi_scale = 1.0f;
	int maxFPS = 60;
	uint16_t game_version = 0;
	bool fullscreen = false;
	bool showConsole = false;
	uint32_t coreLogOutput = CORE_LOG_TO_CHAT | CORE_LOG_TO_FILE;
	uint8_t antialias = 0;
	std::wstring serverport = L"7911";
	uint8_t textfontsize = 12;
	std::wstring lasthost = L"127.0.0.1";
	std::wstring lastport = L"7911";
	std::wstring nickname = L"Player";
	std::wstring gamename = L"Game";
	std::wstring lastdeck = L"";
	uint32_t lastlflist = 0;
	uint32_t lastallowedcards = 3;
	uint32_t lastDuelParam = 0x2E800; //#define DUEL_MODE_MR5
	uint32_t lastExtraRules = 0;
	uint32_t lastDuelForbidden = 0; //#define DUEL_MODE_MR5_FORB
	uint32_t timeLimit = 180;
	uint32_t team1count = 1;
	uint32_t team2count = 1;
	uint32_t bestOf = 1;
	uint32_t startLP = 8000;
	uint32_t startHand = 5;
	uint32_t drawCount = 1;
	bool relayDuel = false;
	bool noCheckDeck = false;
	bool noShuffleDeck = false;
	bool botThrowRock = false;
	bool botMute = false;
	int lastBot = 0;
	std::wstring lastServer = L"";
	std::wstring textfont = L"fonts/NotoSansJP-Regular.otf";
	std::wstring numfont = L"fonts/NotoSansJP-Regular.otf";
	std::wstring roompass = L""; // NOT SERIALIZED
	//settings
	bool chkMAutoPos = false;
	bool chkSTAutoPos = false;
	bool chkRandomPos = false;
	bool chkAutoChain = false;
	bool chkWaitChain = false;
	bool chkIgnore1 = false;
	bool chkIgnore2 = false;
	bool chkHideSetname = false;
	bool chkHideHintButton = true;
	bool draw_field_spell = true;
	bool quick_animation = false;
	bool alternative_phase_layout = false;
	bool showFPS = true;
	bool hidePasscodeScope = false;
	bool showScopeLabel = true;
	bool filterBot = true;
	bool scale_background = true;
	bool dotted_lines = false;
#ifdef __ANDROID__
	bool accurate_bg_resize = true;
	bool native_keyboard = false;
	bool native_mouse = false;
#else
	bool accurate_bg_resize = false;
#endif
	bool hideHandsInReplays = false;
	bool chkAnime = false;
#ifdef __APPLE__
	bool ctrlClickIsRMB = true;
#else
	bool ctrlClickIsRMB = false;
#endif
	bool enablemusic = false;
	bool discordIntegration = true;
	bool enablesound = true;
	bool saveHandTest = true;
	int musicVolume = 20;
	int soundVolume = 20;
	bool loopMusic = true;
	bool noClientUpdates = false;
	bool controller_input = false;
	path_string skin = EPRO_TEXT("none");
	path_string locale = EPRO_TEXT("English");
	path_string working_directory = EPRO_TEXT("./");
	std::string ssl_certificate_path = "";

	nlohmann::json configs;
	nlohmann::json user_configs;
};

extern GameConfig* gGameConfig;

}

#endif
