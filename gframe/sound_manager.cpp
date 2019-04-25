#include "sound_manager.h"
#include "utils.h"
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init() {
#ifdef YGOPRO_USE_IRRKLANG
	rnd.seed(time(0));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshChantsList();
	engineSound = irrklang::createIrrKlangDevice();
	engineMusic = irrklang::createIrrKlangDevice();
	if(!engineSound || !engineMusic) {
		return false;
	} else {
#ifdef IRRKLANG_STATIC
		irrklang::ikpMP3Init(engineMusic);
#endif
		return true;
	}
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
	RefershBGMDir(L"", BGM_DUEL);
	RefershBGMDir(L"duel", BGM_DUEL);
	RefershBGMDir(L"menu", BGM_MENU);
	RefershBGMDir(L"deck", BGM_DECK);
	RefershBGMDir(L"advantage", BGM_ADVANTAGE);
	RefershBGMDir(L"disadvantage", BGM_DISADVANTAGE);
	RefershBGMDir(L"win", BGM_WIN);
	RefershBGMDir(L"lose", BGM_LOSE);
}
void SoundManager::RefershBGMDir(std::wstring path, int scene) {
	auto files = Utils::FindfolderFiles(L"./sound/BGM/" + path, { L"mp3", L"ogg", L"wav" });
	for(auto& file : files) {
		BGMList[BGM_ALL].push_back(BufferIO::EncodeUTF8s(path + L"/" + file));
		BGMList[scene].push_back(BufferIO::EncodeUTF8s(path + L"/" + file));
	}
}
void SoundManager::RefreshChantsList() {
	Utils::Makedirectory(TEXT("./sound/chants"));
	auto files = Utils::FindfolderFiles(L"./sound/chants", { L"mp3", L"ogg", L"wav" });
	for(auto& file : files) {
		auto scode = Utils::GetFileName(L"./sound/chants/" + file);
		unsigned int code = std::stoi(scode);
		if(code && !ChantsList.count(code))
			ChantsList[code] = BufferIO::EncodeUTF8s(file);
	}
}
void SoundManager::PlaySoundEffect(Sounds sound) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableSound->isChecked())
		return;
	switch(sound) {
		case SUMMON:
		{
			engineSound->play2D("./sound/summon.wav");
			break;
		}
		case SPECIAL_SUMMON:
		{
			engineSound->play2D("./sound/specialsummon.wav");
			break;
		}
		case ACTIVATE:
		{
			engineSound->play2D("./sound/activate.wav");
			break;
		}
		case SET:
		{
			engineSound->play2D("./sound/set.wav");
			break;
		}
		case FLIP:
		{
			engineSound->play2D("./sound/flip.wav");
			break;
		}
		case REVEAL:
		{
			engineSound->play2D("./sound/reveal.wav");
			break;
		}
		case EQUIP:
		{
			engineSound->play2D("./sound/equip.wav");
			break;
		}
		case DESTROYED:
		{
			engineSound->play2D("./sound/destroyed.wav");
			break;
		}
		case BANISHED:
		{
			engineSound->play2D("./sound/banished.wav");
			break;
		}
		case TOKEN:
		{
			engineSound->play2D("./sound/token.wav");
			break;
		}
		case ATTACK:
		{
			engineSound->play2D("./sound/attack.wav");
			break;
		}
		case DIRECT_ATTACK:
		{
			engineSound->play2D("./sound/directattack.wav");
			break;
		}
		case DRAW:
		{
			engineSound->play2D("./sound/draw.wav");
			break;
		}
		case SHUFFLE:
		{
			engineSound->play2D("./sound/shuffle.wav");
			break;
		}
		case DAMAGE:
		{
			engineSound->play2D("./sound/damage.wav");
			break;
		}
		case RECOVER:
		{
			engineSound->play2D("./sound/gainlp.wav");
			break;
		}
		case COUNTER_ADD:
		{
			engineSound->play2D("./sound/addcounter.wav");
			break;
		}
		case COUNTER_REMOVE:
		{
			engineSound->play2D("./sound/removecounter.wav");
			break;
		}
		case COIN:
		{
			engineSound->play2D("./sound/coinflip.wav");
			break;
		}
		case DICE:
		{
			engineSound->play2D("./sound/diceroll.wav");
			break;
		}
		case NEXT_TURN:
		{
			engineSound->play2D("./sound/nextturn.wav");
			break;
		}
		case PHASE:
		{
			engineSound->play2D("./sound/phase.wav");
			break;
		}
		case PLAYER_ENTER:
		{
			engineSound->play2D("./sound/playerenter.wav");
			break;
		}
		case CHAT:
		{
			engineSound->play2D("./sound/chatmessage.wav");
			break;
		}
		default:
			break;
	}
	engineSound->setSoundVolume(mainGame->gameConf.volume);
#endif
}
void SoundManager::PlayMusic(const std::string& song, bool loop) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!engineMusic->isCurrentlyPlaying(song.c_str())) {
		engineMusic->stopAllSounds();
		if(soundBGM) {
			soundBGM->drop();
			soundBGM = nullptr;
		}
		soundBGM = engineMusic->play2D(song.c_str(), loop, false, true);
		engineMusic->setSoundVolume(mainGame->gameConf.volume);
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_IRRKLANG
	int count = BGMList[scene].size();
	if(mainGame->chkEnableMusic->isChecked() && (scene != bgm_scene || (soundBGM && soundBGM->isFinished()) || !soundBGM) && count > 0) {
		bgm_scene = scene;
		int bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		std::string BGMName = "./sound/BGM/" + BGMList[scene][bgm];
		PlayMusic(BGMName, true);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_IRRKLANG
	engineMusic->stopAllSounds();
#endif
}
bool SoundManager::PlayChant(unsigned int code) {
	if(ChantsList.count(code)) {
		if(!engineSound->isCurrentlyPlaying(("./sound/chants/" + ChantsList[code]).c_str()))
			engineSound->play2D(("./sound/chants/" + ChantsList[code]).c_str());
		return true;
	}
	return false;
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	engineSound->setSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef YGOPRO_USE_IRRKLANG
	engineMusic->setSoundVolume(volume);
#endif
}
}