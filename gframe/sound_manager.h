#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "config.h"
#include "game.h"
#include <vector>

namespace ygo {

class SoundManager {
private:
	std::vector<std::wstring> BGMList[8];
	int bgm_scene;
	irrklang::ISoundEngine* engineSound;
	irrklang::ISoundEngine* engineMusic;
	irrklang::ISound* soundBGM;
	void RefershBGMDir(std::wstring path, int scene);

public:
	bool Init();
	void RefreshBGMList();
	void PlaySoundEffect(int sound);
	void PlayMusic(char* song, bool loop);
	void PlayBGM(int scene);
	void StopBGM();
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
};

extern SoundManager soundManager;

}

#endif //SOUNDMANAGER_H
