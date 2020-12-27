#ifdef YGOPRO_USE_SFML
#include "sound_sfml.h"
#include <sfAudio/Music.hpp>
#include <sfAudio/Sound.hpp>
#include <sfAudio/SoundBuffer.hpp>

using Status = sf::SoundSource::Status;

SoundSFML::SoundSFML() :
	music(), music_volume(0.0f), sound_volume(0.0f) {
};
SoundSFML::~SoundSFML() = default;

void SoundSFML::SetSoundVolume(double volume)
{
	sound_volume = (float)(volume * 100);
	for (auto& sound: sounds)
		sound->setVolume(sound_volume);
}

void SoundSFML::SetMusicVolume(double volume)
{
	music_volume = (float)(volume * 100);
	music.setVolume(music_volume);
}

bool SoundSFML::PlayMusic(const std::string& name, bool loop)
{
	const auto status = music.getStatus();
	if(status != Status::Stopped && cur_music == name)
		return true;
	if(status == Status::Playing)
		music.stop();
	if(!music.openFromFile(name))
		return false;
	cur_music = name;
	music.setLoop(loop);
	music.play();
	return true;
}
const sf::SoundBuffer& SoundSFML::LookupSound(const std::string& name)
{
	auto& buf = buffers[name];
	if (!buf) {
		std::unique_ptr<sf::SoundBuffer> new_buf(new sf::SoundBuffer);
		new_buf->loadFromFile(name);
		buf.swap(new_buf);
	}
	return *buf;
}

bool SoundSFML::PlaySound(const std::string& name)
{
	auto& buf = LookupSound(name);
	if (buf.getSampleCount() == 0) return false;
	std::unique_ptr<sf::Sound> sound(new sf::Sound(buf));
	if (!sound) return false;
	sound->setVolume(sound_volume);
	sound->play();
	sounds.emplace_back(std::move(sound));
	return true;
}

void SoundSFML::StopSounds()
{
	sounds.clear();
	buffers.clear();
}

void SoundSFML::StopMusic()
{
	music.stop();
}

void SoundSFML::PauseMusic(bool pause)
{
	if (pause) music.pause();
	else music.play();
}

bool SoundSFML::MusicPlaying()
{
	return music.getStatus() == Status::Playing;
}

void SoundSFML::Tick()
{
	for (auto it = sounds.begin(); it != sounds.end();) {
		if ((*it)->getStatus() != Status::Playing)
			it = sounds.erase(it);
		else
			it++;
	}
}
#endif //YGOPRO_USE_SFML