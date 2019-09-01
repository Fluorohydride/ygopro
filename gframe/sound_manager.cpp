#include "sound_manager.h"
#include "utils.h"
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
	}
	soundEngine->setSoundVolume(sounds_volume);
#endif // YGOPRO_USE_IRRKLANG
	// TODO: Implement other sound engines
	return false;
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
	RefershBGMDir(TEXT(""), BGM_DUEL);
	RefershBGMDir(TEXT("duel"), BGM_DUEL);
	RefershBGMDir(TEXT("menu"), BGM_MENU);
	RefershBGMDir(TEXT("deck"), BGM_DECK);
	RefershBGMDir(TEXT("advantage"), BGM_ADVANTAGE);
	RefershBGMDir(TEXT("disadvantage"), BGM_DISADVANTAGE);
	RefershBGMDir(TEXT("win"), BGM_WIN);
	RefershBGMDir(TEXT("lose"), BGM_LOSE);
}
void SoundManager::RefershBGMDir(path_string path, int scene) {
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
void SoundManager::PlaySoundEffect(Sounds sound) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!soundsEnabled)
		return;
	switch(sound) {
		case SUMMON:
		{
			soundEngine->play2D("./sound/summon.wav");
			break;
		}
		case SPECIAL_SUMMON:
		{
			soundEngine->play2D("./sound/specialsummon.wav");
			break;
		}
		case ACTIVATE:
		{
			soundEngine->play2D("./sound/activate.wav");
			break;
		}
		case SET:
		{
			soundEngine->play2D("./sound/set.wav");
			break;
		}
		case FLIP:
		{
			soundEngine->play2D("./sound/flip.wav");
			break;
		}
		case REVEAL:
		{
			soundEngine->play2D("./sound/reveal.wav");
			break;
		}
		case EQUIP:
		{
			soundEngine->play2D("./sound/equip.wav");
			break;
		}
		case DESTROYED:
		{
			soundEngine->play2D("./sound/destroyed.wav");
			break;
		}
		case BANISHED:
		{
			soundEngine->play2D("./sound/banished.wav");
			break;
		}
		case TOKEN:
		{
			soundEngine->play2D("./sound/token.wav");
			break;
		}
		case ATTACK:
		{
			soundEngine->play2D("./sound/attack.wav");
			break;
		}
		case DIRECT_ATTACK:
		{
			soundEngine->play2D("./sound/directattack.wav");
			break;
		}
		case DRAW:
		{
			soundEngine->play2D("./sound/draw.wav");
			break;
		}
		case SHUFFLE:
		{
			soundEngine->play2D("./sound/shuffle.wav");
			break;
		}
		case DAMAGE:
		{
			soundEngine->play2D("./sound/damage.wav");
			break;
		}
		case RECOVER:
		{
			soundEngine->play2D("./sound/gainlp.wav");
			break;
		}
		case COUNTER_ADD:
		{
			soundEngine->play2D("./sound/addcounter.wav");
			break;
		}
		case COUNTER_REMOVE:
		{
			soundEngine->play2D("./sound/removecounter.wav");
			break;
		}
		case COIN:
		{
			soundEngine->play2D("./sound/coinflip.wav");
			break;
		}
		case DICE:
		{
			soundEngine->play2D("./sound/diceroll.wav");
			break;
		}
		case NEXT_TURN:
		{
			soundEngine->play2D("./sound/nextturn.wav");
			break;
		}
		case PHASE:
		{
			soundEngine->play2D("./sound/phase.wav");
			break;
		}
		case PLAYER_ENTER:
		{
			soundEngine->play2D("./sound/playerenter.wav");
			break;
		}
		case CHAT:
		{
			soundEngine->play2D("./sound/chatmessage.wav");
			break;
		}
		default:
			break;
	}
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
	if(ChantsList.count(code)) {
		if(!soundEngine->isCurrentlyPlaying(("./sound/chants/" + ChantsList[code]).c_str()))
			soundEngine->play2D(("./sound/chants/" + ChantsList[code]).c_str());
		return true;
	}
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
	if(!musicEnabled) {
		if(soundBGM){
			if(!soundBGM->isFinished())
				soundBGM->stop();
			soundBGM->drop();
			soundBGM = nullptr;
		}
	}
}
}
