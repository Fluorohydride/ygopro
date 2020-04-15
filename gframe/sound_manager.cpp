#define WIN32_LEAN_AND_MEAN
#include "sound_manager.h"
#include "utils.h"
#include "config.h"
#if defined(YGOPRO_USE_IRRKLANG)
#include "sound_irrklang.h"
#define BACKEND SoundIrrklang
#elif defined(YGOPRO_USE_SDL_MIXER)
#include "sound_sdlmixer.h"
#define BACKEND SoundMixer
#endif

namespace ygo {
SoundManager::SoundManager(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, const path_string& working_directory) {
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
		succesfully_initied = soundsEnabled = musicEnabled = false;
		return;
	}
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
	succesfully_initied = true;
#else
	succesfully_initied = soundsEnabled = musicEnabled = false;
	return;
#endif // BACKEND
}
bool SoundManager::IsUsable() {
	return succesfully_initied;
}
void SoundManager::RefreshBGMList() {
#ifdef BACKEND
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/duel"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/menu"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/deck"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/advantage"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/disadvantage"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/win"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/lose"));
	for (auto list : BGMList) {
		list.clear();
	}
	RefreshBGMDir(EPRO_TEXT(""), BGM::DUEL);
	RefreshBGMDir(EPRO_TEXT("duel"), BGM::DUEL);
	RefreshBGMDir(EPRO_TEXT("menu"), BGM::MENU);
	RefreshBGMDir(EPRO_TEXT("deck"), BGM::DECK);
	RefreshBGMDir(EPRO_TEXT("advantage"), BGM::ADVANTAGE);
	RefreshBGMDir(EPRO_TEXT("disadvantage"), BGM::DISADVANTAGE);
	RefreshBGMDir(EPRO_TEXT("win"), BGM::WIN);
	RefreshBGMDir(EPRO_TEXT("lose"), BGM::LOSE);
#endif
}
void SoundManager::RefreshBGMDir(path_string path, BGM scene) {
#ifdef BACKEND
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./sound/BGM/") + path, { EPRO_TEXT("mp3"), EPRO_TEXT("ogg"), EPRO_TEXT("wav"), EPRO_TEXT("flac") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + EPRO_TEXT("/") + file);
		BGMList[BGM::ALL].push_back(conv);
		BGMList[scene].push_back(conv);
	}
#endif
}
void SoundManager::RefreshChantsList() {
#ifdef BACKEND
	static const std::vector<std::pair<CHANT, path_string>> types = {
		{CHANT::SUMMON,    EPRO_TEXT("summon")},
		{CHANT::ATTACK,    EPRO_TEXT("attack")},
		{CHANT::ACTIVATE,  EPRO_TEXT("activate")}
	};
	ChantsList.clear();
	for (const auto& chantType : types) {
		const path_string searchPath = EPRO_TEXT("./sound/") + chantType.second;
		Utils::MakeDirectory(searchPath);
		for (auto& file : Utils::FindFiles(searchPath, { EPRO_TEXT("mp3"), EPRO_TEXT("ogg"), EPRO_TEXT("wav"), EPRO_TEXT("flac") })) {
			auto scode = Utils::GetFileName(searchPath + EPRO_TEXT("/") + file);
			try {
				unsigned int code = std::stoi(scode);
				auto key = std::make_pair(chantType.first, code);
				if (code && !ChantsList.count(key))
					ChantsList[key] = working_dir + "/" + Utils::ToUTF8IfNeeded(searchPath + EPRO_TEXT("/") + file);
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
void SoundManager::PlayBGM(BGM scene, bool loop) {
#ifdef BACKEND
	auto& list = BGMList[scene];
	int count = list.size();
	if(musicEnabled && (scene != bgm_scene || !mixer->MusicPlaying()) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = working_dir + "/./sound/BGM/" + list[bgm];
		mixer->PlayMusic(BGMName, loop);
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
