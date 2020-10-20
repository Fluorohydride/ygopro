#ifdef YGOPRO_USE_IRRKLANG
#include "sound_irrklang.h"
#include "irrklang_dynamic_loader.h"
#include <stdexcept>
#include <irrKlang.h>
#ifdef IRRKLANG_STATIC
#include "../ikpmp3/ikpMP3.h"
#endif

SoundIrrklang::SoundIrrklang() :
	soundEngine(nullptr), soundBGM(nullptr), sfxVolume(0.0), bgmVolume(0.0) {
	irrdyn::init();
	if(!(soundEngine = irrdyn::createIrrKlangDevice()))
		throw std::runtime_error("Failed to init irrklang device!");
#ifdef IRRKLANG_STATIC
	irrklang::ikpMP3Init(soundEngine);
#endif
}
void SoundIrrklang::SetSoundVolume(double volume) {
	sfxVolume = volume;
}
void SoundIrrklang::SetMusicVolume(double volume) {
	if(soundBGM)
		soundBGM->setVolume((irrklang::ik_f32)volume);
	bgmVolume = volume;
}
bool SoundIrrklang::PlayMusic(const std::string& name, bool loop) {
	if(!soundBGM || soundBGM->getSoundSource()->getName() != name) {
		StopMusic();
		if(soundEngine) {
			soundBGM = soundEngine->play2D(name.data(), loop, true, true);
			if(!soundBGM)
				return false;
			soundBGM->setVolume((irrklang::ik_f32)bgmVolume);
			soundBGM->setIsPaused(false);
		}
	}
	return true;
}
bool SoundIrrklang::PlaySound(const std::string& name) {
	auto sfx = soundEngine->play2D(name.data(), false, true);
	if(!sfx)
		return false;
	sfx->setVolume((irrklang::ik_f32)sfxVolume);
	sfx->setIsPaused(false);
	sounds.push_back(sfx);
	return true;
}
void SoundIrrklang::StopSounds() {
	for(auto& sound : sounds)
		sound->stop();
}
void SoundIrrklang::StopMusic() {
	if(soundBGM) {
		soundBGM->stop();
		soundBGM->drop();
		soundBGM = nullptr;
	}
}
void SoundIrrklang::PauseMusic(bool pause) {
	if(soundBGM)
		soundBGM->setIsPaused(pause);
}
bool SoundIrrklang::MusicPlaying() {
	return soundBGM && !soundBGM->isFinished();
}
void SoundIrrklang::Tick() {
	for(auto itsound = sounds.begin(); itsound != sounds.end();) {
		auto sound = *itsound;
		if(sound->isFinished()) {
			sound->drop();
			itsound = sounds.erase(itsound);
		} else {
			itsound++;
		}
	}
}
SoundIrrklang::~SoundIrrklang() {
	if(soundBGM)
		soundBGM->drop();
	if(soundEngine)
		soundEngine->drop();
	irrdyn::close();
}

#endif //YGOPRO_USE_IRRKLANG