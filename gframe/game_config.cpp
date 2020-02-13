#include "game_config.h"
#include <fstream>
#include "config.h"

namespace ygo {

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
			else if (type == "errorlog")
				enable_log = std::stoi(str);
			else if (type == "nickname")
				nickname = BufferIO::DecodeUTF8s(str);
			else if (type == "gamename")
				gamename = BufferIO::DecodeUTF8s(str);
			else if (type == "lastdeck")
				lastdeck = BufferIO::DecodeUTF8s(str);
			else if (type == "lastlflist") {
				auto val = std::stoi(str);
				lastlflist = val >= 0 ? val : 0;
			}
			else if (type == "lastallowedcards") {
				auto val = std::stoi(str);
				lastallowedcards = val >= 0 ? val : 0;
			}
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
			else if (type == "dpi_scale")
				dpi_scale = std::stof(str);
			else if (type == "skin")
				skin = Utils::ToPathString(str);
			else if (type == "scale_background")
				scale_background = !!std::stoi(str);
#ifndef __ANDROID__
			else if (type == "accurate_bg_resize")
				accurate_bg_resize = !!std::stoi(str);
#endif
			else if (type == "enable_music")
				enablemusic = !!std::stoi(str);
			else if (type == "enable_sound")
				enablesound = !!std::stoi(str);
			else if (type == "music_volume")
				musicVolume = std::stof(str) / 100.0f;
			else if (type == "sound_volume")
				soundVolume = std::stof(str) / 100.0f;
		}
		catch (...) {}
	}
	return true;
}

void GameConfig::Save(const char* filename)
{
}

}
