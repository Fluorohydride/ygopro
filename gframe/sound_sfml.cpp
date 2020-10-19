#include "sound_sfml.h"
#include <SFML/Audio.hpp>
#include <iostream>

SoundSFML::SoundSFML() = default;
SoundSFML::~SoundSFML() = default;

void SoundSFML::SetSoundVolume(double volume)
{
	sound_volume = volume * 100;
	for (auto& sound: sounds)
		sound->setVolume(sound_volume);
}

void SoundSFML::SetMusicVolume(double volume)
{
	music_volume = volume * 100;
	if (music)
		music->setVolume(music_volume);
}

bool SoundSFML::PlayMusic(const std::string& name, bool loop)
{
	std::unique_ptr<sf::Music> new_music(new sf::Music);
	if (!new_music)
		return false;
	/* supported formats are WAV(PCM), Vorbis and FLAC, but no MP3 */
	bool can_play = new_music->openFromFile(name);
	new_music->setVolume(music_volume);
	new_music->setLoop(loop);
	if (music)
		music->stop();
	music.swap(new_music);
	if (can_play) {
		music->play();
		return true;
	}
	else
		return false;
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
	auto sound = new sf::Sound(buf);
	if (!sound) return false;
	sound->setVolume(sound_volume);
	sound->play();
	sounds.emplace_back(sound);
	return true;
}

void SoundSFML::StopSounds()
{
	for (auto it = sounds.begin(); it != sounds.end(); it = sounds.erase(it))
		(*it)->stop();
	buffers.clear();
}

void SoundSFML::StopMusic()
{
	if (music)
		music->stop();
}

void SoundSFML::PauseMusic(bool pause)
{
	if (!music) return;
	if (pause) music->pause();
	else music->play();
}

bool SoundSFML::MusicPlaying()
{
	return !!music && music->getStatus() == sf::SoundSource::Status::Playing;
}

void SoundSFML::Tick()
{
	for (auto it = sounds.begin(); it != sounds.end();) {
		if (!(*it)->getStatus() == sf::SoundSource::Status::Playing)
			it = sounds.erase(it);
		else
			it++;
	}
}
