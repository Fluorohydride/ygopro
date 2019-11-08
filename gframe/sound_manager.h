#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <random>
#ifdef YGOPRO_USE_IRRKLANG
#include <irrKlang.h>
#else
#include "sound_openal.h"
#endif
#include "utils.h"

namespace ygo {

class SoundManager {
public:
	enum SFX {
		SUMMON,
		SPECIAL_SUMMON,
		ACTIVATE,
		SET,
		FLIP,
		REVEAL,
		EQUIP,
		DESTROYED,
		BANISHED,
		TOKEN,
		ATTACK,
		DIRECT_ATTACK,
		DRAW,
		SHUFFLE,
		DAMAGE,
		RECOVER,
		COUNTER_ADD,
		COUNTER_REMOVE,
		COIN,
		DICE,
		NEXT_TURN,
		PHASE,
		PLAYER_ENTER,
		CHAT
	};
    enum BGM {
        ALL,
        DUEL,
        MENU,
        DECK,
        ADVANTAGE,
        DISADVANTAGE,
        WIN,
        LOSE
    };
#ifndef YGOPRO_USE_IRRKLANG
    SoundManager() : openal(nullptr), sfx(nullptr) {}
#endif
    ~SoundManager();
	bool Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, void* payload = nullptr);
	void RefreshBGMList();
	void PlaySoundEffect(SFX sound);
	void PlayMusic(const std::string& song, bool loop);
	void PlayBGM(BGM scene);
	void StopBGM();
	bool PlayChant(unsigned int code);
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
	void EnableSounds(bool enable);
	void EnableMusic(bool enable);

private:
    std::vector<std::string> BGMList[8];
    std::map<unsigned int, std::string> ChantsList;
    int bgm_scene = -1;
    std::mt19937 rnd;
#ifdef YGOPRO_USE_IRRKLANG
    irrklang::ISoundEngine* soundEngine;
    irrklang::ISound* soundBGM;
#else
    std::unique_ptr<YGOpen::OpenALSingleton> openal;
    std::unique_ptr<YGOpen::OpenALSoundLayer> sfx;
    std::unique_ptr<YGOpen::OpenALSoundLayer> bgm;
    int bgmCurrent = -1;
#endif
    void RefreshBGMDir(path_string path, BGM scene);
    void RefreshChantsList();
    bool soundsEnabled = false;
    bool musicEnabled = false;
};

}

#endif //SOUNDMANAGER_H