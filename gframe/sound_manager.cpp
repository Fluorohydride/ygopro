#include "sound_manager.h"
#include "config.h"
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, void* payload) {
	soundsEnabled = sounds_enabled;
	musicEnabled = music_enabled;
#ifdef YGOPRO_USE_IRRKLANG
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
	soundEngine = irrklang::createIrrKlangDevice();
	if(!soundEngine) {
		return false;
	} else {
#ifdef IRRKLANG_STATIC
		irrklang::ikpMP3Init(soundEngine);
#endif
        soundEngine->setSoundVolume(sounds_volume);
        return true;
	}
#endif // YGOPRO_USE_IRRKLANG
	// TODO: Implement other sound engines
	return false;
}
SoundManager::~SoundManager() {
#ifdef YGOPRO_USE_IRRKLANG
    if (soundBGM)
        soundBGM->drop();
    if (soundEngine)
        soundEngine->drop();
#endif
}
void SoundManager::RefreshBGMList() {
	Utils::Makedirectory(TEXT("./sound/BGM/"));
	Utils::Makedirectory(TEXT("./sound/BGM/duel"));
	Utils::Makedirectory(TEXT("./sound/BGM/menu"));
	Utils::Makedirectory(TEXT("./sound/BGM/deck"));
	Utils::Makedirectory(TEXT("./sound/BGM/advantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/disadvantage"));
	Utils::Makedirectory(TEXT("./sound/BGM/win"));
	Utils::Makedirectory(TEXT("./sound/BGM/lose"));
	Utils::Makedirectory(TEXT("./sound/chants"));
	RefreshBGMDir(TEXT(""), BGM_DUEL);
	RefreshBGMDir(TEXT("duel"), BGM_DUEL);
	RefreshBGMDir(TEXT("menu"), BGM_MENU);
	RefreshBGMDir(TEXT("deck"), BGM_DECK);
	RefreshBGMDir(TEXT("advantage"), BGM_ADVANTAGE);
	RefreshBGMDir(TEXT("disadvantage"), BGM_DISADVANTAGE);
	RefreshBGMDir(TEXT("win"), BGM_WIN);
	RefreshBGMDir(TEXT("lose"), BGM_LOSE);
}
void SoundManager::RefreshBGMDir(path_string path, int scene) {
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/BGM/") + path, { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + TEXT("/") + file);
		BGMList[BGM_ALL].push_back(conv);
		BGMList[scene].push_back(conv);
	}
}
void SoundManager::RefreshChantsList() {
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/chants"), { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto scode = Utils::GetFileName(TEXT("./sound/chants/") + file);
		unsigned int code = std::stoi(scode);
		if(code && !ChantsList.count(code))
			ChantsList[code] = Utils::ToUTF8IfNeeded(file);
	}
}
void SoundManager::PlaySoundEffect(SFX sound) {
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
#ifdef YGOPRO_USE_IRRKLANG
    soundEngine->play2D(fx.at(sound));
#endif
}
void SoundManager::PlayMusic(const std::string& song, bool loop) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!musicEnabled)
		return;
	if(!soundBGM || soundBGM->getSoundSource()->getName() != song) {
		if(soundBGM) {
			soundBGM->stop();
			soundBGM->drop();
			soundBGM = nullptr;
		}
		soundBGM = soundEngine->play2D(song.c_str(), loop, false, true);
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_IRRKLANG
	auto& list = BGMList[scene];
	int count = list.size();
	if(musicEnabled && (scene != bgm_scene || (soundBGM && soundBGM->isFinished()) || !soundBGM) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = "./sound/BGM/" + list[bgm];
		PlayMusic(BGMName, true);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_IRRKLANG
	if(soundBGM) {
		soundBGM->stop();
		soundBGM->drop();
		soundBGM = nullptr;
	}
#endif
}
bool SoundManager::PlayChant(unsigned int code) {
#ifdef YGOPRO_USE_IRRKLANG
	if(ChantsList.count(code)) {
		if(!soundEngine->isCurrentlyPlaying(("./sound/chants/" + ChantsList[code]).c_str()))
			soundEngine->play2D(("./sound/chants/" + ChantsList[code]).c_str());
		return true;
	}
#endif
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->setSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	soundEngine->setSoundVolume(volume);
#endif
}
void SoundManager::EnableSounds(bool enable) {
	soundsEnabled = enable;
}
void SoundManager::EnableMusic(bool enable) {
	musicEnabled = enable;
#ifdef YGOPRO_USE_IRRKLANG
	if(!musicEnabled) {
		if(soundBGM){
			if(!soundBGM->isFinished())
				soundBGM->stop();
			soundBGM->drop();
			soundBGM = nullptr;
		}
	}
#endif
}

} // namespace ygo
