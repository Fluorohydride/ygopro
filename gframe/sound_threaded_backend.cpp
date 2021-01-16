#include "sound_threaded_backend.h"
#include "utils.h"

SoundThreadedBackend::SoundThreadedBackend(std::unique_ptr<SoundBackend>&& base) :
	m_BaseBackend(std::move(base)),
	m_BaseThread(std::thread(&SoundThreadedBackend::BaseLoop, this)) {
}

void SoundThreadedBackend::BaseLoop() {
	ygo::Utils::SetThreadName("SoundsThread");
	while(true) {
		std::unique_lock<std::mutex> lck(m_ActionMutex);
		while(m_Actions.empty()) {
			m_ActionCondVar.wait(lck);
		}
		auto action = std::move(m_Actions.front());
		m_Actions.pop();
		lck.unlock();
		switch(action.type) {
		case ActionType::SET_SOUND_VOLUME: {
			m_BaseBackend->SetSoundVolume(action.arg.volume);
			break;
		}
		case ActionType::SET_MUSIC_VOLUME: {
			m_BaseBackend->SetMusicVolume(action.arg.volume);
			break;
		}
		case ActionType::PLAY_MUSIC: {
			auto res = m_BaseBackend->PlayMusic(*action.arg.play_music.name, action.arg.play_music.loop);
			std::unique_lock<std::mutex> lckres(m_ResponseMutex);
			response = res;
			m_ResponseCondVar.notify_all();
			break;
		}
		case ActionType::PLAY_SOUND: {
			auto res = m_BaseBackend->PlaySound(*action.arg.play_sound);
			std::unique_lock<std::mutex> lckres(m_ResponseMutex);
			response = res;
			m_ResponseCondVar.notify_all();
			break;
		}
		case ActionType::STOP_SOUNDS: {
			m_BaseBackend->StopSounds();
			break;
		}
		case ActionType::STOP_MUSIC: {
			m_BaseBackend->StopMusic();
			break;
		}
		case ActionType::PAUSE_MUSIC: {
			m_BaseBackend->PauseMusic(action.arg.pause);
			break;
		}
		case ActionType::MUSIC_PLAYING: {
			auto res = m_BaseBackend->MusicPlaying();
			std::unique_lock<std::mutex> lckres(m_ResponseMutex);
			response = res;
			m_ResponseCondVar.notify_all();
			break;
		}
		case ActionType::TICK: {
			m_BaseBackend->Tick();
			break;
		}
		case ActionType::TERMINATE: {
			return;
		}
		}
	}
}

SoundThreadedBackend::~SoundThreadedBackend() {
	std::queue<Action> tmp;
	Action action{ ActionType::TERMINATE };
	tmp.emplace(std::move(action));
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.swap(tmp);
	m_ActionCondVar.notify_all();
	lck.unlock();
	if(m_BaseThread.joinable())
		m_BaseThread.join();
}

void SoundThreadedBackend::SetSoundVolume(double volume) {
	Action action{ ActionType::SET_SOUND_VOLUME };
	action.arg.volume = volume;
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}

void SoundThreadedBackend::SetMusicVolume(double volume) {
	Action action{ ActionType::SET_MUSIC_VOLUME };
	action.arg.volume = volume;
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}

bool SoundThreadedBackend::PlayMusic(const std::string& name, bool loop) {
	Action action{ ActionType::PLAY_MUSIC };
	auto& args = action.arg.play_music;
	args.name = &name;
	args.loop = loop;
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	std::unique_lock<std::mutex> lckres(m_ResponseMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
	lck.unlock();
	m_ResponseCondVar.wait(lckres);
	return response;
}

bool SoundThreadedBackend::PlaySound(const std::string& name) {
	Action action{ ActionType::PLAY_SOUND };
	action.arg.play_sound = &name;
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	std::unique_lock<std::mutex> lckres(m_ResponseMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
	lck.unlock();
	m_ResponseCondVar.wait(lckres);
	return response;
}

void SoundThreadedBackend::StopSounds() {
	Action action{ ActionType::STOP_SOUNDS };
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}

void SoundThreadedBackend::StopMusic() {
	Action action{ ActionType::STOP_MUSIC };
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}

void SoundThreadedBackend::PauseMusic(bool pause) {
	Action action{ ActionType::PAUSE_MUSIC };
	action.arg.pause = pause;
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}

bool SoundThreadedBackend::MusicPlaying() {
	Action action{ ActionType::MUSIC_PLAYING };
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	std::unique_lock<std::mutex> lckres(m_ResponseMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
	lck.unlock();
	m_ResponseCondVar.wait(lckres);
	return response;
}

void SoundThreadedBackend::Tick() {
	Action action{ ActionType::TICK };
	std::unique_lock<std::mutex> lck(m_ActionMutex);
	m_Actions.emplace(std::move(action));
	m_ActionCondVar.notify_all();
}
