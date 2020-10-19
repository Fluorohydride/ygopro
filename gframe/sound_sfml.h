#include "sound_backend.h"
#include <vector>
#include <memory>
#include <map>
#include <string>

namespace sf
{
	class Music;
	class Sound;
	class SoundBuffer;
}

class SoundSFML : public SoundBackend {
public:
	SoundSFML();
	~SoundSFML();
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
	std::unique_ptr<sf::Music> music;
	std::vector<std::unique_ptr<sf::Sound>> sounds;
	float music_volume, sound_volume;
	std::map<std::string,std::unique_ptr<sf::SoundBuffer>> buffers;
	const sf::SoundBuffer& LookupSound(const std::string& name);
};
