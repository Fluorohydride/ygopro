#ifndef SOUND_IRRKLANG_H
#define SOUND_IRRKLANG_H
#include "sound_backend.h"
#include <vector>
#include <string>
namespace irrklang {
class ISoundEngine;
class ISound;
}

class SoundIrrklang : public SoundBackend {
public:
	SoundIrrklang();
	~SoundIrrklang();
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
	bool PlayMusic(const std::string& name, bool loop);
	bool PlaySound(const std::string& name);
	void StopSounds();
	void StopMusic();
	void PauseMusic(bool pause);
	bool MusicPlaying();
	void Tick();
private:
	irrklang::ISoundEngine* soundEngine;
	irrklang::ISound* soundBGM;
	std::vector<irrklang::ISound*> sounds;
	double sfxVolume = 1.0;
	double bgmVolume = 1.0;
};

#endif //SOUND_IRRKLANG_H
