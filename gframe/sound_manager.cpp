#include "sound_manager.h"
#include "myfilesystem.h"

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init() {
#ifdef YGOPRO_USE_AUDIO
	bgm_scene = -1;
	RefreshBGMList();
	rnd.reset((unsigned int)std::time(nullptr));
	if(ma_engine_init(nullptr, &engineSound) || ma_engine_init(nullptr, &engineMusic)) {
		return false;
	} else {
		return true;
	}
#endif // YGOPRO_USE_AUDIO
	return false;
}
void SoundManager::RefreshBGMList() {
#ifdef YGOPRO_USE_AUDIO
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
void SoundManager::RefershBGMDir(std::wstring path, int scene) {
	std::wstring search = L"./sound/BGM/" + path;
	FileSystem::TraversalDir(search.c_str(), [this, &path, scene](const wchar_t* name, bool isdir) {
		if(!isdir && (IsExtension(name, L".mp3") || IsExtension(name, L".ogg"))) {
			std::wstring filename = path + L"/" + name;
			BGMList[BGM_ALL].push_back(filename);
			BGMList[scene].push_back(filename);
		}
	});
}
void SoundManager::PlaySoundEffect(int sound) {
	if(!mainGame->chkEnableSound->isChecked())
		return;
	char soundName[32];
	switch(sound) {
	case SOUND_SUMMON: {
		strcpy(soundName, "summon");
		break;
	}
	case SOUND_SPECIAL_SUMMON: {
		strcpy(soundName, "specialsummon");
		break;
	}
	case SOUND_ACTIVATE: {
		strcpy(soundName, "activate");
		break;
	}
	case SOUND_SET: {
		strcpy(soundName, "set");
		break;
	}
	case SOUND_FILP: {
		strcpy(soundName, "flip");
		break;
	}
	case SOUND_REVEAL: {
		strcpy(soundName, "reveal");
		break;
	}
	case SOUND_EQUIP: {
		strcpy(soundName, "equip");
		break;
	}
	case SOUND_DESTROYED: {
		strcpy(soundName, "destroyed");
		break;
	}
	case SOUND_BANISHED: {
		strcpy(soundName, "banished");
		break;
	}
	case SOUND_TOKEN: {
		strcpy(soundName, "token");
		break;
	}
	case SOUND_ATTACK: {
		strcpy(soundName, "attack");
		break;
	}
	case SOUND_DIRECT_ATTACK: {
		strcpy(soundName, "directattack");
		break;
	}
	case SOUND_DRAW: {
		strcpy(soundName, "draw");
		break;
	}
	case SOUND_SHUFFLE: {
		strcpy(soundName, "shuffle");
		break;
	}
	case SOUND_DAMAGE: {
		strcpy(soundName, "damage");
		break;
	}
	case SOUND_RECOVER: {
		strcpy(soundName, "recover");
		break;
	}
	case SOUND_COUNTER_ADD: {
		strcpy(soundName, "addcounter");
		break;
	}
	case SOUND_COUNTER_REMOVE: {
		strcpy(soundName, "removecounter");
		break;
	}
	case SOUND_COIN: {
		strcpy(soundName, "coin");
		break;
	}
	case SOUND_DICE: {
		strcpy(soundName, "dice");
		break;
	}
	case SOUND_NEXT_TURN: {
		strcpy(soundName, "nextturn");
		break;
	}
	case SOUND_PHASE: {
		strcpy(soundName, "phase");
		break;
	}
	case SOUND_MENU: {
		strcpy(soundName, "menu");
		break;
	}
	case SOUND_BUTTON: {
		strcpy(soundName, "button");
		break;
	}
	case SOUND_INFO: {
		strcpy(soundName, "info");
		break;
	}
	case SOUND_QUESTION: {
		strcpy(soundName, "question");
		break;
	}
	case SOUND_CARD_PICK: {
		strcpy(soundName, "cardpick");
		break;
	}
	case SOUND_CARD_DROP: {
		strcpy(soundName, "carddrop");
		break;
	}
	case SOUND_PLAYER_ENTER: {
		strcpy(soundName, "playerenter");
		break;
	}
	case SOUND_CHAT: {
		strcpy(soundName, "chat");
		break;
	}
	default:
		break;
	}
	char soundPath[40];
	std::snprintf(soundPath, 40, "./sound/%s.wav", soundName);
#ifdef YGOPRO_USE_AUDIO
	ma_engine_set_volume(&engineSound, mainGame->gameConf.sound_volume);
	auto res = ma_engine_play_sound(&engineSound, soundPath, nullptr);
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
bool SoundManager::IsCurrentlyPlaying(char* song) {
#ifdef YGOPRO_USE_AUDIO
	return currentPlayingMusic[0] && strcmp(currentPlayingMusic, song) == 0 && ma_sound_is_playing(&soundBGM);
#endif
	return false;
}
void SoundManager::PlayMusic(char* song, bool loop) {
#ifdef YGOPRO_USE_AUDIO
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!IsCurrentlyPlaying(song)) {
		StopBGM();
		strcpy(currentPlayingMusic, song);
#ifdef _WIN32
		wchar_t song_w[1024];
		BufferIO::DecodeUTF8(song, song_w);
		ma_sound_init_from_file_w(&engineMusic, song_w, MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_STREAM, nullptr, nullptr, &soundBGM);
#else
		ma_sound_init_from_file(&engineMusic, song, MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_STREAM, nullptr, nullptr, &soundBGM);
#endif
		ma_sound_set_looping(&soundBGM, loop);
		ma_sound_start(&soundBGM);
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_AUDIO
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!mainGame->chkMusicMode->isChecked())
		scene = BGM_ALL;
	char BGMName[1024];
	if(scene != bgm_scene || !IsCurrentlyPlaying(currentPlayingMusic)) {
		int count = BGMList[scene].size();
		if(count <= 0)
			return;
		bgm_scene = scene;
		int bgm = rnd.get_random_integer(0, count -1);
		auto name = BGMList[scene][bgm].c_str();
		wchar_t fname[1024];
		myswprintf(fname, L"./sound/BGM/%ls", name);
		BufferIO::EncodeUTF8(fname, BGMName);
		PlayMusic(BGMName, false);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_AUDIO
	if(!currentPlayingMusic[0])
		return;
	memset(currentPlayingMusic, 0, sizeof(currentPlayingMusic));
	ma_sound_uninit(&soundBGM);
#endif
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef YGOPRO_USE_AUDIO
	ma_engine_set_volume(&engineSound, volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef YGOPRO_USE_AUDIO
	ma_engine_set_volume(&engineMusic, volume);
#endif
}
}
