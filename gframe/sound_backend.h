#ifndef SOUND_BACKEND_H
#define SOUND_BACKEND_H
#include <string>
#include <vector>
#include "text_types.h"

class SoundBackend {
public:
	virtual ~SoundBackend() = default;
	virtual void SetSoundVolume(double volume) = 0;
	virtual void SetMusicVolume(double volume) = 0;
	virtual bool PlayMusic(const std::string& name, bool loop) = 0;
	virtual bool PlaySound(const std::string& name) = 0;
	virtual void StopSounds() = 0;
	virtual void StopMusic() = 0;
	virtual bool MusicPlaying() = 0;
	virtual void PauseMusic(bool pause) = 0;
	virtual void Tick() {};
	virtual std::vector<epro::path_stringview> GetSupportedSoundExtensions() {
		return { EPRO_TEXT("wav"), EPRO_TEXT("mp3"), EPRO_TEXT("ogg"), EPRO_TEXT("flac") };
	};
	virtual std::vector<epro::path_stringview> GetSupportedMusicExtensions() {
		return { EPRO_TEXT("mp3"), EPRO_TEXT("ogg"), EPRO_TEXT("wav"), EPRO_TEXT("flac") };
	};
};

#endif //SOUND_BACKEND_H