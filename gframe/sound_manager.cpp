#include "sound_manager.h"
#include "config.h"

namespace ygo {

bool SoundManager::Init(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled, void* payload) {
	soundsEnabled = sounds_enabled;
	musicEnabled = music_enabled;
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
    try {
        openal = std::make_unique<YGOpen::OpenALSingleton>();
        sfx = std::make_unique<YGOpen::OpenALSoundLayer>(openal);
        bgm = std::make_unique<YGOpen::OpenALSoundLayer>(openal);
        sfx->setVolume(sounds_volume);
        bgm->setVolume(music_volume);
        return true;
    }
    catch (std::runtime_error& e) {
        return soundsEnabled = musicEnabled = false;
    }
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
	RefreshBGMDir(TEXT(""), BGM::DUEL);
	RefreshBGMDir(TEXT("duel"), BGM::DUEL);
	RefreshBGMDir(TEXT("menu"), BGM::MENU);
	RefreshBGMDir(TEXT("deck"), BGM::DECK);
	RefreshBGMDir(TEXT("advantage"), BGM::ADVANTAGE);
	RefreshBGMDir(TEXT("disadvantage"), BGM::DISADVANTAGE);
	RefreshBGMDir(TEXT("win"), BGM::WIN);
	RefreshBGMDir(TEXT("lose"), BGM::LOSE);
}
void SoundManager::RefreshBGMDir(path_string path, BGM scene) {
	for(auto& file : Utils::FindfolderFiles(TEXT("./sound/BGM/") + path, { TEXT("mp3"), TEXT("ogg"), TEXT("wav") })) {
		auto conv = Utils::ToUTF8IfNeeded(path + TEXT("/") + file);
		BGMList[BGM::ALL].push_back(conv);
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
    if (sfx) sfx->play(fx.at(sound), false);
}
void SoundManager::PlayMusic(const std::string& song, bool loop) {
	if(!musicEnabled) return;
    StopBGM();
    if (bgm) bgmCurrent = bgm->play(song, loop);
}
void SoundManager::PlayBGM(BGM scene) {
	auto& list = BGMList[scene];
	int count = list.size();
	if (musicEnabled && (scene != bgm_scene || !bgm->exists(bgmCurrent)) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = "./sound/BGM/" + list[bgm];
		PlayMusic(BGMName, true);
	}
}
void SoundManager::StopBGM() {
    bgm->stopAll();
}
bool SoundManager::PlayChant(unsigned int code) {
	if(ChantsList.count(code)) {
        if (bgm) bgm->play("./sound/chants/" + ChantsList[code], false);
		return true;
	}
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
    if (sfx) sfx->setVolume(volume);
}
void SoundManager::SetMusicVolume(double volume) {
    if (bgm) bgm->setVolume(volume);
}
void SoundManager::EnableSounds(bool enable) {
	soundsEnabled = enable;
}
void SoundManager::EnableMusic(bool enable) {
	musicEnabled = enable;
	if(!musicEnabled) {
        StopBGM();
    }
}

} // namespace ygo
