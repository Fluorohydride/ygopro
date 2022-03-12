#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <memory>
#include "RNG/mt19937.h"
#include <map>
#include "text_types.h"
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
		CHAT,
		SFX_TOTAL_SIZE
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
	enum class CHANT {
		SUMMON,
		ATTACK,
		ACTIVATE
	};
	SoundManager(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled);
	bool IsUsable();
	void RefreshBGMList();
	void RefreshChantsList();
	void PlaySoundEffect(SFX sound);
	void PlayBGM(BGM scene, bool loop = true);
	bool PlayChant(CHANT chant, uint32_t code);
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
	std::string SFXList[SFX::SFX_TOTAL_SIZE];
	std::map<std::pair<CHANT, uint32_t>, std::string> ChantsList;
	int bgm_scene = -1;
	RNG::mt19937 rnd;
	std::unique_ptr<SoundBackend> mixer;
	void RefreshSoundsList();
	void RefreshBGMDir(epro::path_stringview path, BGM scene);
	bool soundsEnabled = false;
	bool musicEnabled = false;
	std::string working_dir;
	bool succesfully_initied = false;
};

extern SoundManager* gSoundManager;

}

#endif //SOUNDMANAGER_H
