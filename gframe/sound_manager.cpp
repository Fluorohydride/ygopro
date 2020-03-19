#include "sound_manager.h"
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

#ifdef XDG_ENVIRONMENT
#define DATA(x) mainGame->FindDataFile(x).c_str()
#else
#define DATA(x) "./" x
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init() {
#ifdef YGOPRO_USE_IRRKLANG
	bgm_scene = -1;
	RefreshBGMList();
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
#ifdef XDG_ENVIRONMENT
	RefreshBGMDir("duel", BGM_DUEL);
	RefreshBGMDir("menu", BGM_MENU);
	RefreshBGMDir("deck", BGM_DECK);
	RefreshBGMDir("advantage", BGM_ADVANTAGE);
	RefreshBGMDir("disadvantage", BGM_DISADVANTAGE);
	RefreshBGMDir("win", BGM_WIN);
	RefreshBGMDir("lose", BGM_LOSE);
#else
	RefershBGMDir(L"", BGM_DUEL);
	RefershBGMDir(L"duel", BGM_DUEL);
	RefershBGMDir(L"menu", BGM_MENU);
	RefershBGMDir(L"deck", BGM_DECK);
	RefershBGMDir(L"advantage", BGM_ADVANTAGE);
	RefershBGMDir(L"disadvantage", BGM_DISADVANTAGE);
	RefershBGMDir(L"win", BGM_WIN);
	RefershBGMDir(L"lose", BGM_LOSE);
#endif
}
#ifdef XDG_ENVIRONMENT
void SoundManager::RefreshBGMDir1(std::string path, int scene) {
	FileSystem::TraversalDir(path.c_str(), [this, &path, scene](const char* name, bool isdir)
	{
		if (isdir || !strchr(name, '.'))
			return;
		const char* suffix = strchr(name, '.');
		if (!strncmp(suffix, ".mp3", 4) || !strncmp(suffix, ".ogg", 4)) {
			std::string full_path = path + "/" + name;
			BGMList[BGM_ALL].push_back(full_path);
			BGMList[scene].push_back(full_path);
		}

	});
}

void SoundManager::RefreshBGMDir(std::string path, int scene) {
	RefreshBGMDir1(mainGame->DATA_HOME + "/bgm/" + path, scene);
	RefreshBGMDir1(std::string(mainGame->sysdatadir) + "/bgm/" + path, scene);
}
#else
void SoundManager::RefershBGMDir(std::wstring path, int scene) {
	std::wstring search = L"./sound/BGM/" + path;
	FileSystem::TraversalDir(search.c_str(), [this, &path, scene](const wchar_t* name, bool isdir) {
		if(!isdir && wcsrchr(name, '.') && (!mywcsncasecmp(wcsrchr(name, '.'), L".mp3", 4) || !mywcsncasecmp(wcsrchr(name, '.'), L".ogg", 4))) {
			std::wstring filename = path + L"/" + name;
			BGMList[BGM_ALL].push_back(filename);
			BGMList[scene].push_back(filename);
		}
	});
}
#endif
void SoundManager::PlaySoundEffect(int sound) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableSound->isChecked())
		return;
	engineSound->setSoundVolume(mainGame->gameConf.sound_volume);
	switch(sound) {
	case SOUND_SUMMON: {
		engineSound->play2D(DATA("sound/summon.wav"));
		break;
	}
	case SOUND_SPECIAL_SUMMON: {
		engineSound->play2D(DATA("sound/specialsummon.wav"));
		break;
	}
	case SOUND_ACTIVATE: {
		engineSound->play2D(DATA("sound/activate.wav"));
		break;
	}
	case SOUND_SET: {
		engineSound->play2D(DATA("sound/set.wav"));
		break;
	}
	case SOUND_FILP: {
		engineSound->play2D(DATA("sound/flip.wav"));
		break;
	}
	case SOUND_REVEAL: {
		engineSound->play2D(DATA("sound/reveal.wav"));
		break;
	}
	case SOUND_EQUIP: {
		engineSound->play2D(DATA("sound/equip.wav"));
		break;
	}
	case SOUND_DESTROYED: {
		engineSound->play2D(DATA("sound/destroyed.wav"));
		break;
	}
	case SOUND_BANISHED: {
		engineSound->play2D(DATA("sound/banished.wav"));
		break;
	}
	case SOUND_TOKEN: {
		engineSound->play2D(DATA("sound/token.wav"));
		break;
	}
	case SOUND_ATTACK: {
		engineSound->play2D(DATA("sound/attack.wav"));
		break;
	}
	case SOUND_DIRECT_ATTACK: {
		engineSound->play2D(DATA("sound/directattack.wav"));
		break;
	}
	case SOUND_DRAW: {
		engineSound->play2D(DATA("sound/draw.wav"));
		break;
	}
	case SOUND_SHUFFLE: {
		engineSound->play2D(DATA("sound/shuffle.wav"));
		break;
	}
	case SOUND_DAMAGE: {
		engineSound->play2D(DATA("sound/damage.wav"));
		break;
	}
	case SOUND_RECOVER: {
		engineSound->play2D(DATA("sound/gainlp.wav"));
		break;
	}
	case SOUND_COUNTER_ADD: {
		engineSound->play2D(DATA("sound/addcounter.wav"));
		break;
	}
	case SOUND_COUNTER_REMOVE: {
		engineSound->play2D(DATA("sound/removecounter.wav"));
		break;
	}
	case SOUND_COIN: {
		engineSound->play2D(DATA("sound/coinflip.wav"));
		break;
	}
	case SOUND_DICE: {
		engineSound->play2D(DATA("sound/diceroll.wav"));
		break;
	}
	case SOUND_NEXT_TURN: {
		engineSound->play2D(DATA("sound/nextturn.wav"));
		break;
	}
	case SOUND_PHASE: {
		engineSound->play2D(DATA("sound/phase.wav"));
		break;
	}
	case SOUND_MENU: {
		engineSound->play2D(DATA("sound/menu.wav"));
		break;
	}
	case SOUND_BUTTON: {
		engineSound->play2D(DATA("sound/button.wav"));
		break;
	}
	case SOUND_INFO: {
		engineSound->play2D(DATA("sound/info.wav"));
		break;
	}
	case SOUND_QUESTION: {
		engineSound->play2D(DATA("sound/question.wav"));
		break;
	}
	case SOUND_CARD_PICK: {
		engineSound->play2D(DATA("sound/cardpick.wav"));
		break;
	}
	case SOUND_CARD_DROP: {
		engineSound->play2D(DATA("sound/carddrop.wav"));
		break;
	}
	case SOUND_PLAYER_ENTER: {
		engineSound->play2D(DATA("sound/playerenter.wav"));
		break;
	}
	case SOUND_CHAT: {
		engineSound->play2D(DATA("sound/chatmessage.wav"));
		break;
	}
	default:
		break;
	}
#endif
}
void SoundManager::PlayDialogSound(irr::gui::IGUIElement * element) {
	if(element == mainGame->wMessage) {
		PlaySoundEffect(SOUND_INFO);
	} else if(element == mainGame->wQuery) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wSurrender) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wOptions) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wANAttribute) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wANCard) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wANNumber) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wANRace) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wReplaySave) {
		PlaySoundEffect(SOUND_QUESTION);
	} else if(element == mainGame->wFTSelect) {
		PlaySoundEffect(SOUND_QUESTION);
	}
}
void SoundManager::PlayMusic(char* song, bool loop) {
#ifdef YGOPRO_USE_IRRKLANG
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!engineMusic->isCurrentlyPlaying(song)) {
		engineMusic->stopAllSounds();
		engineMusic->setSoundVolume(mainGame->gameConf.music_volume);
		soundBGM = engineMusic->play2D(song, loop, false, true);
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_IRRKLANG
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
#ifdef XDG_ENVIRONMENT
		PlayMusic(name.c_str(), false);
#else
		wchar_t fname[1024];
		myswprintf(fname, L"./sound/BGM/%ls", name);
		BufferIO::EncodeUTF8(fname, BGMName);
		PlayMusic(BGMName, false);
#endif
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_IRRKLANG
	engineMusic->stopAllSounds();
#endif
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
