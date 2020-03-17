#include "game_config.h"
#include <fstream>
#include "config.h"
#include "logging.h"

namespace ygo {

GameConfig::GameConfig() {
	Load("./config/system.conf");
	if(configs.empty()) {
		std::ifstream conf_file(EPRO_TEXT("./config/configs.json"), std::ifstream::in);
		try {
			conf_file >> configs;
		}
		catch(std::exception& e) {
			ErrorLog(std::string("Exception occurred: ") + e.what());
			throw(e);
		}
	}
}

bool GameConfig::Load(const char* filename)
{
	std::ifstream conf_file(filename, std::ifstream::in);
	if(!conf_file.is_open())
		return false;
	std::string str;
	while (std::getline(conf_file, str)) {
		auto pos = str.find_first_of("\n\r");
		if (str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if (str.empty() || str.at(0) == '#') {
			continue;
		}
		pos = str.find_first_of("=");
		if (pos == std::wstring::npos)
			continue;
		auto type = str.substr(0, pos - 1);
		str = str.substr(pos + 2);
		try {
			if (type == "antialias")
				antialias = std::stoi(str);
			else if (type == "use_d3d")
				use_d3d = !!std::stoi(str);
			else if (type == "use_vsync")
				use_vsync = !!std::stoi(str);
			else if (type == "max_fps") {
				auto val = std::stoi(str);
				if (val >= 0)
					max_fps = val;
			}
			else if (type == "fullscreen")
				fullscreen = !!std::stoi(str);
			else if (type == "show_console")
				showConsole = !!std::stoi(str);
			else if (type == "errorlog")
				enable_log = std::stoi(str);
			else if (type == "nickname")
				nickname = BufferIO::DecodeUTF8s(str);
			else if (type == "gamename")
				gamename = BufferIO::DecodeUTF8s(str);
			else if (type == "lastdeck")
				lastdeck = BufferIO::DecodeUTF8s(str);
			else if (type == "lastDuelParam")
				lastDuelParam = std::stoi(str);
			else if (type == "lastDuelForbidden")
				lastDuelForbidden = std::stoi(str);
#define DESERIALIZE_UNSIGNED(name) \
			else if (type == #name) { \
				auto val = std::stoi(str); \
				name = val >= 0 ? val : 0; \
			}
			DESERIALIZE_UNSIGNED(lastlflist)
			DESERIALIZE_UNSIGNED(lastallowedcards)
			DESERIALIZE_UNSIGNED(timeLimit)
			DESERIALIZE_UNSIGNED(team1count)
			DESERIALIZE_UNSIGNED(team2count)
			DESERIALIZE_UNSIGNED(bestOf)
			DESERIALIZE_UNSIGNED(startLP)
			DESERIALIZE_UNSIGNED(startHand)
			DESERIALIZE_UNSIGNED(drawCount)
#undef DESERIALIZE_UNSIGNED
#define DESERIALIZE_BOOL(name) else if (type == #name) name = !!std::stoi(str);
			DESERIALIZE_BOOL(relayDuel)
			DESERIALIZE_BOOL(noCheckDeck)
			DESERIALIZE_BOOL(noShuffleDeck)
#undef DESERIALIZE_BOOL
			else if (type == "botThrowRock")
				botThrowRock = !!std::stoi(str);
			else if (type == "botMute")
				botMute = !!std::stoi(str);
			else if (type == "lastBot")
				lastBot = std::stoi(str);
			else if (type == "lastServer")
				lastServer = BufferIO::DecodeUTF8s(str);
			else if (type == "textfont") {
				pos = str.find(L' ');
				if (pos == std::wstring::npos) {
					textfont = BufferIO::DecodeUTF8s(str);
					continue;
				}
				textfont = BufferIO::DecodeUTF8s(str.substr(0, pos));
				textfontsize = std::stoi(str.substr(pos));
			}
			else if (type == "numfont")
				numfont = BufferIO::DecodeUTF8s(str);
			else if (type == "serverport")
				serverport = BufferIO::DecodeUTF8s(str);
			else if (type == "lasthost")
				lasthost = BufferIO::DecodeUTF8s(str);
			else if (type == "lastport")
				lastport = BufferIO::DecodeUTF8s(str);
			else if (type == "roompass")
				roompass = BufferIO::DecodeUTF8s(str);
			else if (type == "game_version") {
				int version = std::stoi(str);
				if (version) {
					PRO_VERSION = std::stoi(str);
					game_version = PRO_VERSION;
				}
			}
			else if (type == "automonsterpos")
				chkMAutoPos = !!std::stoi(str);
			else if (type == "autospellpos")
				chkSTAutoPos = !!std::stoi(str);
			else if (type == "randompos")
				chkRandomPos = !!std::stoi(str);
			else if (type == "autochain")
				chkAutoChain = !!std::stoi(str);
			else if (type == "waitchain")
				chkWaitChain = !!std::stoi(str);
			else if (type == "mute_opponent")
				chkIgnore1 = !!std::stoi(str);
			else if (type == "mute_spectators")
				chkIgnore1 = !!std::stoi(str);
			else if (type == "hide_setname")
				chkHideSetname = !!std::stoi(str);
			else if (type == "hide_hint_button")
				chkHideHintButton = !!std::stoi(str);
			else if (type == "draw_field_spell")
				draw_field_spell = !!std::stoi(str);
			else if (type == "quick_animation")
				quick_animation = !!std::stoi(str);
			else if (type == "show_unofficial")
				chkAnime = !!std::stoi(str);
			else if (type == "showFPS")
				showFPS = !!std::stoi(str);
			else if (type == "hidePasscodeScope")
				hidePasscodeScope = !!std::stoi(str);
			else if (type == "filterBot")
				filterBot = !!std::stoi(str);
			else if (type == "dpi_scale")
				dpi_scale = std::stof(str);
			else if (type == "skin")
				skin = Utils::ToPathString(str);
			else if (type == "language")
				locale = Utils::ToPathString(str);
			else if (type == "scale_background")
				scale_background = !!std::stoi(str);
#ifndef __ANDROID__
			else if (type == "accurate_bg_resize")
				accurate_bg_resize = !!std::stoi(str);
#endif
			else if (type == "ctrlClickIsRMB")
				ctrlClickIsRMB = !!std::stoi(str);
			else if (type == "enable_music")
				enablemusic = !!std::stoi(str);
			else if (type == "enable_sound")
				enablesound = !!std::stoi(str);
			else if (type == "music_volume")
				musicVolume = std::min(std::max(std::stoi(str), 0), 100);
			else if (type == "sound_volume")
				soundVolume = std::min(std::max(std::stoi(str), 0), 100);
#ifdef __ANDROID__
			else if (type == "native_keyboard")
				native_keyboard = !!std::stoi(str);
			else if (type == "native_mouse")
				native_mouse = !!std::stoi(str);
#endif
		}
		catch (...) {}
	}
	return true;
}

template<typename T>
inline void Serialize(std::ofstream& conf_file, const char* name, T value) {
	conf_file << name << " = " << value << "\n";
}
template<>
inline void Serialize(std::ofstream& conf_file, const char* name, float value) {
	conf_file << name << " = " << std::to_string(value) << "\n"; // Forces float to show decimals
}
template<>
inline void Serialize(std::ofstream& conf_file, const char* name, std::wstring value) {
	conf_file << name << " = " << BufferIO::EncodeUTF8s(value) << "\n";
}

bool GameConfig::Save(const char* filename)
{
	std::ofstream conf_file(filename, std::ofstream::out);
	if (!conf_file.is_open())
		return false;
	conf_file << "# EDOPro by Project Ignis system.conf\n";
	conf_file << "# Overwritten on normal game exit\n";
#define SERIALIZE(name) Serialize(conf_file, #name, name)
	conf_file << "use_d3d = "            << use_d3d << "\n";
	conf_file << "use_vsync = "          << use_vsync << "\n";
	conf_file << "max_fps = "            << max_fps << "\n";
	conf_file << "fullscreen = "         << fullscreen << "\n";
	conf_file << "show_console = "       << showConsole << "\n";
	conf_file << "antialias = "          << antialias << "\n";
	conf_file << "errorlog = "           << enable_log << "\n";
	conf_file << "nickname = "           << BufferIO::EncodeUTF8s(nickname) << "\n";
	conf_file << "gamename = "           << BufferIO::EncodeUTF8s(gamename) << "\n";
	conf_file << "lastdeck = "           << BufferIO::EncodeUTF8s(lastdeck) << "\n";
	conf_file << "lastlflist = "         << lastlflist << "\n";
	conf_file << "lastallowedcards = "   << lastallowedcards << "\n";
	SERIALIZE(lastDuelParam);
	SERIALIZE(lastDuelForbidden);
	SERIALIZE(timeLimit);
	SERIALIZE(team1count);
	SERIALIZE(team2count);
	SERIALIZE(bestOf);
	SERIALIZE(startLP);
	SERIALIZE(startHand);
	SERIALIZE(drawCount);
	SERIALIZE(relayDuel);
	SERIALIZE(noShuffleDeck);
	SERIALIZE(noCheckDeck);
	conf_file << "textfont = "           << BufferIO::EncodeUTF8s(textfont) << " " << std::to_string(textfontsize) << "\n";
	conf_file << "numfont = "            << BufferIO::EncodeUTF8s(numfont) << "\n";
	conf_file << "serverport = "         << BufferIO::EncodeUTF8s(serverport) << "\n";
	conf_file << "lasthost = "           << BufferIO::EncodeUTF8s(lasthost) << "\n";
	conf_file << "lastport = "           << BufferIO::EncodeUTF8s(lastport) << "\n";
	conf_file << "botThrowRock = "       << botThrowRock << "\n";
	conf_file << "botMute = "            << botMute << "\n";
	conf_file << "lastBot = "            << lastBot << "\n";
	conf_file << "lastServer = "         << BufferIO::EncodeUTF8s(lastServer) << "\n";
	conf_file << "game_version = "       << game_version << "\n";
	conf_file << "automonsterpos = "     << chkMAutoPos << "\n";
	conf_file << "autospellpos = "       << chkSTAutoPos << "\n";
	conf_file << "randompos = "          << chkRandomPos << "\n";
	conf_file << "autochain = "          << chkAutoChain << "\n";
	conf_file << "waitchain = "          << chkWaitChain << "\n";
	conf_file << "mute_opponent = "      << chkIgnore1 << "\n";
	conf_file << "mute_spectators = "    << chkIgnore2 << "\n";
	conf_file << "hide_setname = "       << chkHideSetname << "\n";
	conf_file << "hide_hint_button = "   << chkHideHintButton << "\n";
	conf_file << "draw_field_spell = "   << draw_field_spell << "\n";
	conf_file << "quick_animation = "    << quick_animation << "\n";
	conf_file << "showFPS = "            << showFPS << "\n";
	conf_file << "hidePasscodeScope = "  << hidePasscodeScope << "\n";
	conf_file << "filterBot = "          << filterBot << "\n";
	conf_file << "show_unofficial = "    << chkAnime << "\n";
	conf_file << "ctrlClickIsRMB = "     << ctrlClickIsRMB << "\n";
	conf_file << "dpi_scale = "          << std::to_string(dpi_scale) << "\n"; // Forces float to show decimals
	conf_file << "skin = "            	 << Utils::ToUTF8IfNeeded(skin) << "\n";
	conf_file << "language = "           << Utils::ToUTF8IfNeeded(locale) << "\n";
	conf_file << "scale_background = "   << scale_background << "\n";
	conf_file << "accurate_bg_resize = " << accurate_bg_resize << "\n";
	conf_file << "enable_music = "       << enablemusic << "\n";
	conf_file << "enable_sound = "       << enablesound << "\n";
	conf_file << "music_volume = "       << musicVolume << "\n";
	conf_file << "sound_volume = "       << soundVolume << "\n";
#ifdef __ANDROID__
	conf_file << "native_keyboard = "    << native_keyboard << "\n";
	conf_file << "native_mouse = "       << native_mouse << "\n";
#endif
#undef SERIALIZE
	return true;
}

}
