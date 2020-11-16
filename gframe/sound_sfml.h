#include "sound_backend.h"
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <sfAudio/Music.hpp>

namespace sf {
	class Sound;
	class SoundBuffer;
}

class SoundSFML : public SoundBackend {
public:
	SoundSFML();
	~SoundSFML();
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
	sf::Music music;
	std::vector<std::unique_ptr<sf::Sound>> sounds;
	float music_volume, sound_volume;
	std::map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers;
	const sf::SoundBuffer& LookupSound(const std::string& name);
};