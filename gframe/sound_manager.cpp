#include "sound_manager.h"
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init() {
	bgm_scene = -1;
	RefreshBGMList();
	engineSound = irrklang::createIrrKlangDevice();
	engineMusic = irrklang::createIrrKlangDevice();
	if(!engineSound || !engineMusic) {
		return false;
	} else {
#ifdef IRRKLANG_STATIC
		irrklang::ikpMP3Init(engineSound);
		irrklang::ikpMP3Init(engineMusic);
#endif
		return true;
	}
}
void SoundManager::RefreshBGMList() {
	RefershBGMDir(L"", BGM_DUEL);
	RefershBGMDir(L"duel/", BGM_DUEL);
	RefershBGMDir(L"menu/", BGM_MENU);
	RefershBGMDir(L"deck/", BGM_DECK);
	RefershBGMDir(L"advantage/", BGM_ADVANTAGE);
	RefershBGMDir(L"disadvantage/", BGM_DISADVANTAGE);
	RefershBGMDir(L"win/", BGM_WIN);
	RefershBGMDir(L"lose/", BGM_LOSE);
}
void SoundManager::RefershBGMDir(std::wstring path, int scene) {
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	std::wstring search = L"./sound/BGM/" + path + L"*.mp3";
	HANDLE fh = FindFirstFileW(search.c_str(), &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::wstring filename = path + (std::wstring)fdataw.cFileName;
			BGMList[BGM_ALL].push_back(filename);
			BGMList[scene].push_back(filename);
		}
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./sound/BGM/*.mp3")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".mp3") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		BGMList[BGM_ALL].push_back(wname);
	}
	closedir(dir);
#endif
}
void SoundManager::PlaySoundEffect(int sound) {
	if(!mainGame->chkEnableSound->isChecked())
		return;
	switch(sound) {
	case SOUND_SUMMON: {
		engineSound->play2D("./sound/summon.wav");
		break;
	}
	case SOUND_SPECIAL_SUMMON: {
		engineSound->play2D("./sound/specialsummon.wav");
		break;
	}
	case SOUND_ACTIVATE: {
		engineSound->play2D("./sound/activate.wav");
		break;
	}
	case SOUND_SET: {
		engineSound->play2D("./sound/set.wav");
		break;
	}
	case SOUND_FILP: {
		engineSound->play2D("./sound/flip.wav");
		break;
	}
	case SOUND_REVEAL: {
		engineSound->play2D("./sound/reveal.wav");
		break;
	}
	case SOUND_EQUIP: {
		engineSound->play2D("./sound/equip.wav");
		break;
	}
	case SOUND_DESTROYED: {
		engineSound->play2D("./sound/destroyed.wav");
		break;
	}
	case SOUND_BANISHED: {
		engineSound->play2D("./sound/banished.wav");
		break;
	}
	case SOUND_TOKEN: {
		engineSound->play2D("./sound/token.wav");
		break;
	}
	case SOUND_ATTACK: {
		engineSound->play2D("./sound/attack.wav");
		break;
	}
	case SOUND_DIRECT_ATTACK: {
		engineSound->play2D("./sound/directattack.wav");
		break;
	}
	case SOUND_DRAW: {
		engineSound->play2D("./sound/draw.wav");
		break;
	}
	case SOUND_SHUFFLE: {
		engineSound->play2D("./sound/shuffle.wav");
		break;
	}
	case SOUND_DAMAGE: {
		engineSound->play2D("./sound/damage.wav");
		break;
	}
	case SOUND_RECOVER: {
		engineSound->play2D("./sound/gainlp.wav");
		break;
	}
	case SOUND_COUNTER_ADD: {
		engineSound->play2D("./sound/addcounter.wav");
		break;
	}
	case SOUND_COUNTER_REMOVE: {
		engineSound->play2D("./sound/removecounter.wav");
		break;
	}
	case SOUND_COIN: {
		engineSound->play2D("./sound/coinflip.wav");
		break;
	}
	case SOUND_DICE: {
		engineSound->play2D("./sound/diceroll.wav");
		break;
	}
	case SOUND_NEXT_TURN: {
		engineSound->play2D("./sound/nextturn.wav");
		break;
	}
	case SOUND_PHASE: {
		engineSound->play2D("./sound/phase.wav");
		break;
	}
	case SOUND_MENU: {
		engineSound->play2D("./sound/menu.wav");
		break;
	}
	case SOUND_BUTTON: {
		engineSound->play2D("./sound/button.wav");
		break;
	}
	case SOUND_INFO: {
		engineSound->play2D("./sound/info.wav");
		break;
	}
	case SOUND_QUESTION: {
		engineSound->play2D("./sound/question.wav");
		break;
	}
	case SOUND_CARD_PICK: {
		engineSound->play2D("./sound/cardpick.wav");
		break;
	}
	case SOUND_CARD_DROP: {
		engineSound->play2D("./sound/carddrop.wav");
		break;
	}
	case SOUND_PLAYER_ENTER: {
		engineSound->play2D("./sound/playerenter.wav");
		break;
	}
	case SOUND_CHAT: {
		engineSound->play2D("./sound/chatmessage.wav");
		break;
	}
	default:
		break;
	}
	engineSound->setSoundVolume(mainGame->gameConf.sound_volume);
}
void SoundManager::PlayMusic(char* song, bool loop) {
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!engineMusic->isCurrentlyPlaying(song)) {
		engineMusic->stopAllSounds();
		soundBGM = engineMusic->play2D(song, loop, false, true);
		engineMusic->setSoundVolume(mainGame->gameConf.music_volume);
	}
}
void SoundManager::PlayBGM(int scene) {
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!mainGame->chkMusicMode->isChecked())
		scene = BGM_ALL;
	char BGMName[1024];
	if(scene != bgm_scene || (soundBGM && soundBGM->isFinished())) {
		int count = BGMList[scene].size();
		if(count <= 0)
			return;
		bgm_scene = scene;
		int bgm = rand() % count;
		auto name = BGMList[scene][bgm].c_str();
		wchar_t fname[1024];
		myswprintf(fname, L"./sound/BGM/%ls", name);
		BufferIO::EncodeUTF8(fname, BGMName);
		PlayMusic(BGMName, false);
	}
}
void SoundManager::StopBGM() {
	engineMusic->stopAllSounds();
}
void SoundManager::SetSoundVolume(double volume) {
	engineSound->setSoundVolume(volume);
}
void SoundManager::SetMusicVolume(double volume) {
	engineMusic->setSoundVolume(volume);
}
}
