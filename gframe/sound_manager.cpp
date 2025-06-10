#include "sound_manager.h"
#include "myfilesystem.h"
#if defined(YGOPRO_USE_MINIAUDIO) && defined(YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS)
#include <miniaudio_libopus.h>
#include <miniaudio_libvorbis.h>
#endif
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

namespace ygo {

SoundManager soundManager;

bool SoundManager::Init() {
#ifdef YGOPRO_USE_AUDIO
	bgm_scene = -1;
	RefreshBGMList();
	rnd.reset((unsigned int)std::time(nullptr));
#ifdef YGOPRO_USE_MINIAUDIO
	engineConfig = ma_engine_config_init();
#ifdef YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS
	ma_decoding_backend_vtable* pCustomBackendVTables[] =
	{
		ma_decoding_backend_libvorbis,
		ma_decoding_backend_libopus
	};
	resourceManagerConfig = ma_resource_manager_config_init();
	resourceManagerConfig.ppCustomDecodingBackendVTables = pCustomBackendVTables;
	resourceManagerConfig.customDecodingBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);
	resourceManagerConfig.pCustomDecodingBackendUserData = NULL;
	if(ma_resource_manager_init(&resourceManagerConfig, &resourceManager) != MA_SUCCESS) {
		return false;
	}
	engineConfig.pResourceManager = &resourceManager;
#endif
	if(ma_engine_init(&engineConfig, &engineSound) != MA_SUCCESS || ma_engine_init(&engineConfig, &engineMusic) != MA_SUCCESS) {
		return false;
	} else {
		return true;
	}
#endif // YGOPRO_USE_MINIAUDIO
#ifdef YGOPRO_USE_IRRKLANG
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
		if(!isdir && (
			IsExtension(name, L".mp3")
#if defined(YGOPRO_MINIAUDIO_SUPPORT_OPUS_VORBIS) || defined(YGOPRO_USE_IRRKLANG)
			|| IsExtension(name, L".ogg")
#endif
			)) {
			std::wstring filename = path + L"/" + name;
			BGMList[BGM_ALL].push_back(filename);
			BGMList[scene].push_back(filename);
		}
	});
}
void SoundManager::PlaySoundEffect(int sound) {
#ifdef YGOPRO_USE_AUDIO
	if(!mainGame->chkEnableSound->isChecked())
		return;
	char soundName[32];
	switch(sound) {
	case SOUND_SUMMON: {
		std::strncpy(soundName, "summon", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_SPECIAL_SUMMON: {
		std::strncpy(soundName, "specialsummon", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_ACTIVATE: {
		std::strncpy(soundName, "activate", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_SET: {
		std::strncpy(soundName, "set", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_FILP: {
		std::strncpy(soundName, "flip", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_REVEAL: {
		std::strncpy(soundName, "reveal", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_EQUIP: {
		std::strncpy(soundName, "equip", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_DESTROYED: {
		std::strncpy(soundName, "destroyed", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_BANISHED: {
		std::strncpy(soundName, "banished", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_TOKEN: {
		std::strncpy(soundName, "token", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_NEGATE: {
		std::strncpy(soundName, "negate", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_ATTACK: {
		std::strncpy(soundName, "attack", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_DIRECT_ATTACK: {
		std::strncpy(soundName, "directattack", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_DRAW: {
		std::strncpy(soundName, "draw", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_SHUFFLE: {
		std::strncpy(soundName, "shuffle", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_DAMAGE: {
		std::strncpy(soundName, "damage", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_RECOVER: {
		std::strncpy(soundName, "gainlp", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_COUNTER_ADD: {
		std::strncpy(soundName, "addcounter", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_COUNTER_REMOVE: {
		std::strncpy(soundName, "removecounter", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_COIN: {
		std::strncpy(soundName, "coinflip", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_DICE: {
		std::strncpy(soundName, "diceroll", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_NEXT_TURN: {
		std::strncpy(soundName, "nextturn", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_PHASE: {
		std::strncpy(soundName, "phase", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_MENU: {
		std::strncpy(soundName, "menu", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_BUTTON: {
		std::strncpy(soundName, "button", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_INFO: {
		std::strncpy(soundName, "info", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_QUESTION: {
		std::strncpy(soundName, "question", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_CARD_PICK: {
		std::strncpy(soundName, "cardpick", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_CARD_DROP: {
		std::strncpy(soundName, "carddrop", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_PLAYER_ENTER: {
		std::strncpy(soundName, "playerenter", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	case SOUND_CHAT: {
		std::strncpy(soundName, "chatmessage", sizeof soundName);
		soundName[sizeof soundName - 1] = 0;
		break;
	}
	default:
		break;
	}
	char soundPath[64];
	std::snprintf(soundPath, sizeof soundPath, "./sound/%s.wav", soundName);
	SetSoundVolume(mainGame->gameConf.sound_volume);
#ifdef YGOPRO_USE_MINIAUDIO
	ma_engine_play_sound(&engineSound, soundPath, nullptr);
#endif
#ifdef YGOPRO_USE_IRRKLANG
	engineSound->play2D(soundPath);
#endif
#endif // YGOPRO_USE_AUDIO
}
void SoundManager::PlayDialogSound(irr::gui::IGUIElement * element) {
#ifdef YGOPRO_USE_AUDIO
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
#endif // YGOPRO_USE_AUDIO
}
bool SoundManager::IsPlayingMusic(wchar_t* music) {
#ifdef YGOPRO_USE_MINIAUDIO
	if(music) {
		return !mywcsncasecmp(currentPlayingMusic, music, 1024) && ma_sound_is_playing(&soundBGM);
	} else {
		return ma_sound_is_playing(&soundBGM);
	}
#endif
#ifdef YGOPRO_USE_IRRKLANG
	if(music) {
		char cmusic[1024];
		BufferIO::EncodeUTF8(music, cmusic);
		return engineMusic->isCurrentlyPlaying(cmusic);
	} else {
		return soundBGM && !soundBGM->isFinished();
	}
#endif
	return false;
}
void SoundManager::PlayMusic(wchar_t* music, bool loop) {
#ifdef YGOPRO_USE_AUDIO
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!IsPlayingMusic(music)) {
		StopBGM();
	SetMusicVolume(mainGame->gameConf.music_volume);
#ifdef YGOPRO_USE_MINIAUDIO
		BufferIO::CopyWStr(music, currentPlayingMusic, 1024);
		ma_sound_init_from_file_w(&engineMusic, music, MA_SOUND_FLAG_ASYNC | MA_SOUND_FLAG_STREAM, nullptr, nullptr, &soundBGM);
		ma_sound_set_looping(&soundBGM, loop);
		ma_sound_start(&soundBGM);
#endif
#ifdef YGOPRO_USE_IRRKLANG
		char cmusic[1024];
		BufferIO::EncodeUTF8(music, cmusic);
		soundBGM = engineMusic->play2D(cmusic, loop, false, true);
#endif
	}
#endif
}
void SoundManager::PlayBGM(int scene) {
#ifdef YGOPRO_USE_AUDIO
	if(!mainGame->chkEnableMusic->isChecked())
		return;
	if(!mainGame->chkMusicMode->isChecked())
		scene = BGM_ALL;
	if(scene != bgm_scene || !IsPlayingMusic()) {
		int count = BGMList[scene].size();
		if(count <= 0)
			return;
		bgm_scene = scene;
		int bgm = rnd.get_random_integer(0, count -1);
		auto name = BGMList[scene][bgm].c_str();
		wchar_t BGMName[1024];
		myswprintf(BGMName, L"./sound/BGM/%ls", name);
		PlayMusic(BGMName, false);
	}
#endif
}
void SoundManager::StopBGM() {
#ifdef YGOPRO_USE_MINIAUDIO
	if(!currentPlayingMusic[0])
		return;
	memset(currentPlayingMusic, 0, sizeof(currentPlayingMusic));
	ma_sound_uninit(&soundBGM);
#endif
#ifdef YGOPRO_USE_IRRKLANG
	engineMusic->stopAllSounds();
#endif
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef YGOPRO_USE_MINIAUDIO
	ma_engine_set_volume(&engineSound, volume);
#endif
#ifdef YGOPRO_USE_IRRKLANG
	engineSound->setSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef YGOPRO_USE_MINIAUDIO
	ma_engine_set_volume(&engineMusic, volume);
#endif
#ifdef YGOPRO_USE_IRRKLANG
	engineMusic->setSoundVolume(volume);
#endif
}
}
