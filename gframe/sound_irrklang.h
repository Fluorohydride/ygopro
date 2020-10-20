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
	virtual void SetSoundVolume(double volume) override;
	virtual void SetMusicVolume(double volume) override;
	virtual bool PlayMusic(const std::string& name, bool loop) override;
	virtual bool PlaySound(const std::string& name) override;
	virtual void StopSounds() override;
	virtual void StopMusic() override;
	virtual void PauseMusic(bool pause) override;
	virtual bool MusicPlaying() override;
	virtual void Tick() override;
private:
	irrklang::ISoundEngine* soundEngine;
	irrklang::ISound* soundBGM;
	std::vector<irrklang::ISound*> sounds;
	double sfxVolume;
	double bgmVolume;
};

#endif //SOUND_IRRKLANG_H
