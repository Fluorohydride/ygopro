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

bool SoundManager::Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, void* payload) {
#ifdef BACKEND
	soundsEnabled = sounds_enabled;
	musicEnabled = music_enabled;
	try {
		mixer = std::make_unique<BACKEND>();
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
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/BGM/") + path, { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + TEXT("/") + file);
		BGMList[BGM::ALL].push_back(conv);
		BGMList[scene].push_back(conv);
	}
#endif
}
void SoundManager::RefreshChantsList() {
#ifdef BACKEND
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/chants"), { TEXT("mp3"), TEXT("wav") })) {
		auto scode = Utils::GetFileName(TEXT("./sound/chants/") + file);
		unsigned int code = std::stoi(scode);
		if(code && !ChantsList.count(code))
			ChantsList[code] = Utils::ToUTF8IfNeeded(file);
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
	mixer->PlaySound(fx.at(sound));
#endif
}
void SoundManager::PlayBGM(BGM scene) {
#ifdef BACKEND
	auto& list = BGMList[scene];
	int count = list.size();
	if(musicEnabled && (scene != bgm_scene || !mixer->MusicPlaying()) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = "./sound/BGM/" + list[bgm];
		mixer->PlayMusic(BGMName, true);
	}
#endif
}
bool SoundManager::PlayChant(unsigned int code) {
#ifdef BACKEND
	if(!soundsEnabled) return false;
	if(ChantsList.count(code)) {
		mixer->PlaySound("./sound/chants/" + ChantsList[code]);
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

void SoundManager::Tick() {
#ifdef BACKEND
	if(mixer)
		mixer->Tick();
#endif
}

} // namespace ygo
