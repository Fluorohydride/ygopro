#ifndef SOUND_SDL_MIXER_H
#define SOUND_SDL_MIXER_H
#include <map>
#include <string>
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;
struct Mix_Chunk;

class SoundMixer {
public:
	SoundMixer();
	~SoundMixer();
	void SetSoundVolume(double volume);
	void SetMusicVolume(double volume);
	bool PlayMusic(const std::string& name, bool loop);
	bool PlaySound(const std::string& name);
	void StopMusic();
	bool MusicPlaying();
	void Tick();
private:
	std::string cur_music;
	std::map<int, Mix_Chunk*> sounds;
	Mix_Music* music = nullptr;
	int sound_volume, music_volume;
};

#endif //SOUND_SDL_MIXER_H
