#define WIN32_LEAN_AND_MEAN
#include "sound_manager.h"
#include "utils.h"
#include "config.h"
#if defined(YGOPRO_USE_IRRKLANG)
#include "sound_irrklang.h"
#define BACKEND SoundIrrklang
#elif defined(YGOPRO_USE_SDL_MIXER)
#include "sound_sdlmixer.h"
#define BACKEND SoundMixer
#elif defined(YGOPRO_USE_SFML)
#include "sound_sfml.h"
#define BACKEND SoundSFML
#endif

namespace ygo {
SoundManager::SoundManager(double sounds_volume, double music_volume, bool sounds_enabled, bool music_enabled) {
#ifdef BACKEND
	fmt::print("Using: " STR(BACKEND)" for audio playback.\n");
	working_dir = Utils::ToUTF8IfNeeded(Utils::GetWorkingDirectory());
	soundsEnabled = sounds_enabled;
	musicEnabled = music_enabled;
	try {
		mixer = std::unique_ptr<SoundBackend>(new BACKEND());
		mixer->SetMusicVolume(music_volume);
		mixer->SetSoundVolume(sounds_volume);
	}
	catch(const std::runtime_error& e) {
		fmt::print("Failed to initialize audio backend:\n");
		fmt::print(e.what());
		succesfully_initied = soundsEnabled = musicEnabled = false;
		return;
	}
	catch(...) {
		fmt::print("Failed to initialize audio backend.\n");
		succesfully_initied = soundsEnabled = musicEnabled = false;
		return;
	}
	rnd.seed(static_cast<uint32_t>(time(0)));
	bgm_scene = -1;
	RefreshBGMList();
	RefreshSoundsList();
	RefreshChantsList();
	succesfully_initied = true;
#else
	fmt::print("No audio backend available.\nAudio will be disabled.\n");
	succesfully_initied = soundsEnabled = musicEnabled = false;
	return;
#endif // BACKEND
}
bool SoundManager::IsUsable() {
	return succesfully_initied;
}
void SoundManager::RefreshBGMList() {
#ifdef BACKEND
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/duel"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/menu"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/deck"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/advantage"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/disadvantage"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/win"));
	Utils::MakeDirectory(EPRO_TEXT("./sound/BGM/lose"));
	for (auto& list : BGMList)
		list.clear();
	RefreshBGMDir(EPRO_TEXT(""), BGM::DUEL);
	RefreshBGMDir(EPRO_TEXT("duel"), BGM::DUEL);
	RefreshBGMDir(EPRO_TEXT("menu"), BGM::MENU);
	RefreshBGMDir(EPRO_TEXT("deck"), BGM::DECK);
	RefreshBGMDir(EPRO_TEXT("advantage"), BGM::ADVANTAGE);
	RefreshBGMDir(EPRO_TEXT("disadvantage"), BGM::DISADVANTAGE);
	RefreshBGMDir(EPRO_TEXT("win"), BGM::WIN);
	RefreshBGMDir(EPRO_TEXT("lose"), BGM::LOSE);
#endif
}
void SoundManager::RefreshSoundsList() {
#ifdef BACKEND
	static constexpr std::pair<SFX, epro::path_stringview> fx[]{
		{SUMMON, EPRO_TEXT("./sound/summon.{}"_sv)},
		{SPECIAL_SUMMON, EPRO_TEXT("./sound/specialsummon.{}"_sv)},
		{ACTIVATE, EPRO_TEXT("./sound/activate.{}"_sv)},
		{SET, EPRO_TEXT("./sound/set.{}"_sv)},
		{FLIP, EPRO_TEXT("./sound/flip.{}"_sv)},
		{REVEAL, EPRO_TEXT("./sound/reveal.{}"_sv)},
		{EQUIP, EPRO_TEXT("./sound/equip.{}"_sv)},
		{DESTROYED, EPRO_TEXT("./sound/destroyed.{}"_sv)},
		{BANISHED, EPRO_TEXT("./sound/banished.{}"_sv)},
		{TOKEN, EPRO_TEXT("./sound/token.{}"_sv)},
		{ATTACK, EPRO_TEXT("./sound/attack.{}"_sv)},
		{DIRECT_ATTACK, EPRO_TEXT("./sound/directattack.{}"_sv)},
		{DRAW, EPRO_TEXT("./sound/draw.{}"_sv)},
		{SHUFFLE, EPRO_TEXT("./sound/shuffle.{}"_sv)},
		{DAMAGE, EPRO_TEXT("./sound/damage.{}"_sv)},
		{RECOVER, EPRO_TEXT("./sound/gainlp.{}"_sv)},
		{COUNTER_ADD, EPRO_TEXT("./sound/addcounter.{}"_sv)},
		{COUNTER_REMOVE, EPRO_TEXT("./sound/removecounter.{}"_sv)},
		{COIN, EPRO_TEXT("./sound/coinflip.{}"_sv)},
		{DICE, EPRO_TEXT("./sound/diceroll.{}"_sv)},
		{NEXT_TURN, EPRO_TEXT("./sound/nextturn.{}"_sv)},
		{PHASE, EPRO_TEXT("./sound/phase.{}"_sv)},
		{PLAYER_ENTER, EPRO_TEXT("./sound/playerenter.{}"_sv)},
		{CHAT, EPRO_TEXT("./sound/chatmessage.{}"_sv)}
	};
	const auto extensions = mixer->GetSupportedSoundExtensions();
	for(const auto& sound : fx) {
		for(const auto& ext : extensions) {
			const auto filename = fmt::format(epro::to_fmtstring_view(sound.second), ext);
			if(Utils::FileExists(filename)) {
				SFXList[sound.first] = Utils::ToUTF8IfNeeded(filename);
				break;
			}
		}
	}
#endif
}
void SoundManager::RefreshBGMDir(epro::path_stringview path, BGM scene) {
#ifdef BACKEND
	for(auto& file : Utils::FindFiles(fmt::format(EPRO_TEXT("./sound/BGM/{}"), path), mixer->GetSupportedMusicExtensions())) {
		auto conv = Utils::ToUTF8IfNeeded(fmt::format(EPRO_TEXT("{}/{}"), path, file));
		BGMList[BGM::ALL].push_back(conv);
		BGMList[scene].push_back(std::move(conv));
	}
#endif
}
void SoundManager::RefreshChantsList() {
#ifdef BACKEND
	static constexpr std::pair<CHANT, epro::path_stringview> types[]{
		{CHANT::SUMMON,    EPRO_TEXT("summon"_sv)},
		{CHANT::ATTACK,    EPRO_TEXT("attack"_sv)},
		{CHANT::ACTIVATE,  EPRO_TEXT("activate"_sv)}
	};
	ChantsList.clear();
	for (const auto& chantType : types) {
		const epro::path_string searchPath = fmt::format(EPRO_TEXT("./sound/{}"), chantType.second);
		Utils::MakeDirectory(searchPath);
		for (auto& file : Utils::FindFiles(searchPath, mixer->GetSupportedSoundExtensions())) {
			auto scode = Utils::GetFileName(file);
			try {
				uint32_t code = static_cast<uint32_t>(std::stoul(scode));
				auto key = std::make_pair(chantType.first, code);
				if (code && !ChantsList.count(key))
					ChantsList[key] = fmt::format("{}/{}", working_dir, Utils::ToUTF8IfNeeded(fmt::format(EPRO_TEXT("{}/{}"), searchPath, file)));
			}
			catch (...) {
				continue;
			}
		}
	}
#endif
}
void SoundManager::PlaySoundEffect(SFX sound) {
#ifdef BACKEND
	if(!soundsEnabled) return;
	if(sound >= SFX::SFX_TOTAL_SIZE) return;
	const auto& soundfile = SFXList[sound];
	if(soundfile.empty()) return;
	mixer->PlaySound(soundfile);
#endif
}
void SoundManager::PlayBGM(BGM scene, bool loop) {
#ifdef BACKEND
	if(!musicEnabled)
		return;
	const auto& list = BGMList[scene];
	int count = list.size();
	if(count == 0)
		return;
	if(scene != bgm_scene || !mixer->MusicPlaying()) {
		bgm_scene = scene;
		auto bgm = (std::uniform_int_distribution<>(0, count - 1))(rnd);
		const std::string BGMName = fmt::format("{}/./sound/BGM/{}", working_dir, list[bgm]);
		mixer->PlayMusic(BGMName, loop);
	}
#endif
}
bool SoundManager::PlayChant(CHANT chant, uint32_t code) {
#ifdef BACKEND
	if(!soundsEnabled) return false;
	auto key = std::make_pair(chant, code);
	auto chant_it = ChantsList.find(key);
	if(chant_it == ChantsList.end())
		return false;
	mixer->PlaySound(chant_it->second);
	return true;
#else
	return false;
#endif
}
void SoundManager::SetSoundVolume(double volume) {
#ifdef BACKEND
	if(mixer)
		mixer->SetSoundVolume(volume);
#endif
}
void SoundManager::SetMusicVolume(double volume) {
#ifdef BACKEND
	if(mixer)
		mixer->SetMusicVolume(volume);
#endif
}
void SoundManager::EnableSounds(bool enable) {
#ifdef BACKEND
	if(mixer && !(soundsEnabled = enable))
		mixer->StopSounds();
#endif
}
void SoundManager::EnableMusic(bool enable) {
#ifdef BACKEND
	if(mixer && !(musicEnabled = enable))
		mixer->StopMusic();
#endif
}
void SoundManager::StopSounds() {
#ifdef BACKEND
	if(mixer)
		mixer->StopSounds();
#endif
}
void SoundManager::StopMusic() {
#ifdef BACKEND
	if(mixer)
		mixer->StopMusic();
#endif
}
void SoundManager::PauseMusic(bool pause) {
#ifdef BACKEND
	if(mixer)
		mixer->PauseMusic(pause);
#endif
}

void SoundManager::Tick() {
#ifdef BACKEND
	if(mixer)
		mixer->Tick();
#endif
}

} // namespace ygo
