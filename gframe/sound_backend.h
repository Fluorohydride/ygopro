#ifndef SOUND_BACKEND_H
#define SOUND_BACKEND_H
#include <string>

class SoundBackend {
public:
	virtual ~SoundBackend() = default;
	virtual void SetSoundVolume(double volume) = 0;
	virtual void SetMusicVolume(double volume) = 0;
	virtual bool PlayMusic(const std::string& name, bool loop) = 0;
	virtual bool PlaySound(const std::string& name) = 0;
	virtual void StopMusic() = 0;
	virtual bool MusicPlaying() = 0;
	virtual void Tick() {};
};

#endif //SOUND_BACKEND_H