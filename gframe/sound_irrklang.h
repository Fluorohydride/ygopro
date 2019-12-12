#ifndef SOUND_IRRKLANG_H
#define SOUND_IRRKLANG_H
#include "sound_backend.h"
#include <map>
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
	void StopMusic();
	bool MusicPlaying();
private:
	irrklang::ISoundEngine* soundEngine;
	irrklang::ISound* soundBGM;
	double sfxVolume = 1.0;
	double bgmVolume = 1.0;
};

#endif //SOUND_IRRKLANG_H
