#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "config.h"
#include "game.h"
#ifdef YGOPRO_USE_IRRKLANG
#include <irrKlang.h>
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif
#endif

#define wcsicmp _wcsicmp

namespace ygo {

class SoundManager {
private:
	std::vector<std::wstring> BGMList[8];
	int bgm_scene;
#ifdef YGOPRO_USE_IRRKLANG
	irrklang::ISoundEngine* engineSound;
	irrklang::ISoundEngine* engineMusic;
	irrklang::ISound* soundBGM;
#endif
	void RefershBGMDir(std::wstring path, int scene);

public:
	bool Init();
	void RefreshBGMList();
	void PlaySoundEffect(int sound);
	void PlayDialogSound(irr::gui::IGUIElement * element);
	void PlayMusic(char* song, bool loop);
	void PlayBGM(int scene);
	void StopBGM();
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
};

extern SoundManager soundManager;

}

#endif //SOUNDMANAGER_H
