#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "game.h"
#include "../ocgcore/mtrandom.h"
#ifdef YGOPRO_USE_MINIAUDIO
#include <miniaudio.h>
#endif
#ifdef YGOPRO_USE_IRRKLANG
#include <irrKlang.h>
#endif

namespace ygo {

class SoundManager {
private:
	std::vector<std::wstring> BGMList[8];
	int bgm_scene;
	mt19937 rnd;
#ifdef YGOPRO_USE_MINIAUDIO
	ma_engine_config engineConfig;
#ifdef YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS
	ma_resource_manager_config resourceManagerConfig;
	ma_resource_manager resourceManager;
#endif
	ma_engine engineSound;
	ma_engine engineMusic;
	ma_sound soundBGM;
	wchar_t currentPlayingMusic[1024]{};
#endif
#ifdef YGOPRO_USE_IRRKLANG
	irrklang::ISoundEngine* engineSound;
	irrklang::ISoundEngine* engineMusic;
	irrklang::ISound* soundBGM;
#endif
	void RefershBGMDir(std::wstring path, int scene);

public:
	bool Init();
	void RefreshBGMList();
	void PlaySoundEffect(int sound);
	void PlayDialogSound(irr::gui::IGUIElement * element);
	bool IsCurrentlyPlaying(wchar_t* music);
	void PlayMusic(wchar_t* music, bool loop);
	void PlayBGM(int scene);
	void StopBGM();
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
};

extern SoundManager soundManager;

#define SOUND_SUMMON				101
#define SOUND_SPECIAL_SUMMON		102
#define SOUND_ACTIVATE				103
#define SOUND_SET					104
#define SOUND_FILP					105
#define SOUND_REVEAL				106
#define SOUND_EQUIP					107
#define SOUND_DESTROYED				108
#define SOUND_BANISHED				109
#define SOUND_TOKEN					110
#define SOUND_NEGATE				111

#define SOUND_ATTACK				201
#define SOUND_DIRECT_ATTACK			202
#define SOUND_DRAW					203
#define SOUND_SHUFFLE				204
#define SOUND_DAMAGE				205
#define SOUND_RECOVER				206
#define SOUND_COUNTER_ADD			207
#define SOUND_COUNTER_REMOVE		208
#define SOUND_COIN					209
#define SOUND_DICE					210
#define SOUND_NEXT_TURN				211
#define SOUND_PHASE					212

#define SOUND_MENU					301
#define SOUND_BUTTON				302
#define SOUND_INFO					303
#define SOUND_QUESTION				304
#define SOUND_CARD_PICK				305
#define SOUND_CARD_DROP				306
#define SOUND_PLAYER_ENTER			307
#define SOUND_CHAT					308

#define BGM_ALL						0
#define BGM_DUEL					1
#define BGM_MENU					2
#define BGM_DECK					3
#define BGM_ADVANTAGE				4
#define BGM_DISADVANTAGE			5
#define BGM_WIN						6
#define BGM_LOSE					7

}

#endif //SOUNDMANAGER_H
