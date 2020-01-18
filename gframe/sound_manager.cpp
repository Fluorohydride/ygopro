#define WIN32_LEAN_AND_MEAN
#include "sound_manager.h"
#include "config.h"
#if defined(YGOPRO_USE_IRRKLANG)
#include "sound_irrklang.h"
#define BACKEND SoundIrrklang
#elif defined(YGOPRO_USE_SDL_MIXER)
#include "sound_sdlmixer.h"
#define BACKEND SoundMixer
#endif

namespace ygo {

bool SoundManager::Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, const path_string& working_directory) {
#ifdef BACKEND
	working_dir = Utils::ToUTF8IfNeeded(working_directory);
	soundsEnabled = sounds_enabled;
	musicEnabled = music_enabled;
	try {
		mixer = std::unique_ptr<SoundBackend>(new BACKEND());
		mixer->SetMusicVolume(music_volume);
		mixer->SetSoundVolume(sounds_volume);
	}
	catch(...) {
		return soundsEnabled = musicEnabled = false;
	}
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
	return true;
#else
	return soundsEnabled = musicEnabled = false;
#endif // BACKEND
}
void SoundManager::RefreshBGMList() {
#ifdef BACKEND
	Utils::Makedirectory(TEXT("./sound/BGM/"));
	Utils::Makedirectory(TEXT("./sound/BGM/duel"));
	Utils::Makedirectory(TEXT("./sound/BGM/menu"));
	Utils::Makedirectory(TEXT("./sound/BGM/deck"));
	Utils::Makedirectory(TEXT("./sound/BGM/advantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/disadvantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/win"));
	Utils::Makedirectory(TEXT("./sound/BGM/lose"));
	Utils::Makedirectory(TEXT("./sound/chants"));
	RefreshBGMDir(TEXT(""), BGM::DUEL);
	RefreshBGMDir(TEXT("duel"), BGM::DUEL);
	RefreshBGMDir(TEXT("menu"), BGM::MENU);
	RefreshBGMDir(TEXT("deck"), BGM::DECK);
	RefreshBGMDir(TEXT("advantage"), BGM::ADVANTAGE);
	RefreshBGMDir(TEXT("disadvantage"), BGM::DISADVANTAGE);
	RefreshBGMDir(TEXT("win"), BGM::WIN);
	RefreshBGMDir(TEXT("lose"), BGM::LOSE);
#endif
}
void SoundManager::RefreshBGMDir(path_string path, BGM scene) {
#ifdef BACKEND
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/BGM/") + path, { TEXT("mp3"), TEXT("ogg"), TEXT("wav"), TEXT("flac") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + TEXT("/") + file);
		BGMList[BGM::ALL].push_back(conv);
		BGMList[scene].push_back(conv);
	}
#endif
}
void SoundManager::RefreshChantsList() {
#ifdef BACKEND
	static const std::vector<std::pair<CHANT, path_string>> types = {
		{CHANT::SUMMON, TEXT("summon") },
		{CHANT::ATTACK, TEXT("attack") },
		{CHANT::CHAIN,  TEXT("chain")  }
	};
	for (const auto& chantType : types) {
		const path_string searchPath = TEXT("./sound/") + chantType.second;
		for (auto& file : Utils::FindfolderFiles(searchPath, { TEXT("mp3"), TEXT("ogg"), TEXT("wav"), TEXT("flac") })) {
			auto scode = Utils::GetFileName(searchPath + TEXT("/") + file);
			try {
				unsigned int code = std::stoi(scode);
				auto key = std::make_pair(chantType.first, code);
				if (code && !ChantsList.count(key))
					ChantsList[key] = working_dir + "/" + Utils::ToUTF8IfNeeded(searchPath + TEXT("/") + file);
			}
			catch (...) {
				continue;
			}
		}
	}
#endif
}
void SoundManager::PlaySoundEffect(SFX sound) {
#ifdef BACKEND
	static const std::map<SFX, const char*> fx = {
		{SUMMON, "./sound/summon.wav"},
		{SPECIAL_SUMMON, "./sound/specialsummon.wav"},
		{ACTIVATE, "./sound/activate.wav"},
		{SET, "./sound/set.wav"},
		{FLIP, "./sound/flip.wav"},
		{REVEAL, "./sound/reveal.wav"},
		{EQUIP, "./sound/equip.wav"},
		{DESTROYED, "./sound/destroyed.wav"},
		{BANISHED, "./sound/banished.wav"},
		{TOKEN, "./sound/token.wav"},
		{ATTACK, "./sound/attack.wav"},
		{DIRECT_ATTACK, "./sound/directattack.wav"},
		{DRAW, "./sound/draw.wav"},
		{SHUFFLE, "./sound/shuffle.wav"},
		{DAMAGE, "./sound/damage.wav"},
		{RECOVER, "./sound/gainlp.wav"},
		{COUNTER_ADD, "./sound/addcounter.wav"},
		{COUNTER_REMOVE, "./sound/removecounter.wav"},
		{COIN, "./sound/coinflip.wav"},
		{DICE, "./sound/diceroll.wav"},
		{NEXT_TURN, "./sound/nextturn.wav"},
		{PHASE, "./sound/phase.wav"},
		{PLAYER_ENTER, "./sound/playerenter.wav"},
		{CHAT, "./sound/chatmessage.wav"}
	};
	if (!soundsEnabled) return;
	mixer->PlaySound(working_dir + "/" + fx.at(sound));
#endif
}
void SoundManager::PlayBGM(BGM scene) {
#ifdef BACKEND
	auto& list = BGMList[scene];
	int count = list.size();
	if(musicEnabled && (scene != bgm_scene || !mixer->MusicPlaying()) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = working_dir + "/./sound/BGM/" + list[bgm];
		mixer->PlayMusic(BGMName, true);
	}
#endif
}
bool SoundManager::PlayChant(CHANT chant, unsigned int code) {
#ifdef BACKEND
	if(!soundsEnabled) return false;
	auto key = std::make_pair(chant, code);
	if (ChantsList.count(key)) {
		mixer->PlaySound(ChantsList[key]);
		return true;
	}
#endif
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef BACKEND
	if(!mixer)
		return;
	mixer->SetSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef BACKEND
	if(!mixer)
		return;
	mixer->SetMusicVolume(volume);
#endif
}
void SoundManager::EnableSounds(bool enable) {
#ifdef BACKEND
	if(!mixer)
		return;
	soundsEnabled = enable;
	if(!musicEnabled) {
		mixer->StopSounds();
	}
#endif
}
void SoundManager::EnableMusic(bool enable) {
#ifdef BACKEND
	if(!mixer)
		return;
	musicEnabled = enable;
	if(!musicEnabled) {
		mixer->StopMusic();
	}
#endif
}
void SoundManager::StopSounds() {
#ifdef BACKEND
	if(mixer)
		mixer->StopSounds();
#endif
}
void SoundManager::StopMusic() {
#ifdef BACKEND
	if(mixer)
		mixer->StopMusic();
#endif
}
void SoundManager::PauseMusic(bool pause) {
#ifdef BACKEND
	if(mixer)
		mixer->PauseMusic(pause);
#endif
}

void SoundManager::Tick() {
#ifdef BACKEND
	if(mixer)
		mixer->Tick();
#endif
}

} // namespace ygo
