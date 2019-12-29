#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <memory>
#include <random>
#include "utils.h"
#include "sound_backend.h"

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
	bool Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, const std::string& working_directory);
	void RefreshBGMList();
	void PlaySoundEffect(SFX sound);
	void PlayBGM(BGM scene);
	bool PlayChant(unsigned int code);
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
	void EnableSounds(bool enable);
	void EnableMusic(bool enable);
	void StopSounds();
	void StopMusic();
	void PauseMusic(bool pause);
	void Tick();

private:
	std::vector<std::string> BGMList[8];
	std::map<unsigned int, std::string> ChantsList;
	int bgm_scene = -1;
	std::mt19937 rnd;
	std::unique_ptr<SoundBackend> mixer;
	void RefreshBGMDir(path_string path, BGM scene);
	void RefreshChantsList();
	bool soundsEnabled = false;
	bool musicEnabled = false;
	std::string working_dir = "./";
};

}

#endif //SOUNDMANAGER_H