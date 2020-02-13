#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "text_types.h"

namespace ygo {

struct GameConfig
{
	bool Load(const char* filename);
	bool Save(const char* filename);

	bool use_d3d = true;
	bool use_vsync = true;
	float dpi_scale = 1.0f;
	int max_fps = 60;
	int game_version = 0;
	bool fullscreen = false;
	unsigned short antialias = 0;
	std::wstring serverport = L"7911";
	unsigned char textfontsize = 13;
	std::wstring lasthost = L"127.0.0.1";
	std::wstring lastport = L"7911";
	std::wstring nickname = L"Player";
	std::wstring gamename = L"Game";
	std::wstring lastdeck;
	unsigned int lastlflist;
	unsigned int lastallowedcards = 3;
	std::wstring textfont = L"fonts/NotoSansJP-Regular.otf";
	std::wstring numfont = L"fonts/NotoSansJP-Regular.otf";
	std::wstring roompass; // NOT SERIALIZED
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
	bool showFPS = true;

	bool scale_background = true;
#ifdef __ANDROID__
	bool accurate_bg_resize = false;
#else
	bool accurate_bg_resize = false;
#endif
	bool chkAnime = false;
	bool enablemusic = false;
	bool enablesound = true;
	double musicVolume = 20;
	double soundVolume = 20;
	path_string skin = EPRO_TEXT("none");
};

}

#endif