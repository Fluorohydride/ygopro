#ifndef SOUND_SDL_MIXER_H
#define SOUND_SDL_MIXER_H
#include "sound_backend.h"
#include <map>
#include <string>
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;
struct Mix_Chunk;

class SoundMixer : public SoundBackend {
public:
	SoundMixer();
	~SoundMixer();
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
	std::string cur_music;
	std::map<int, Mix_Chunk*> sounds;
	Mix_Music* music;
	int sound_volume, music_volume;
};

#endif //SOUND_SDL_MIXER_H
