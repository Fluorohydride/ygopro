#ifndef SOUND_THREADED_BACKEND_H
#define SOUND_THREADED_BACKEND_H
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "sound_backend.h"

class SoundThreadedBackend : public SoundBackend {
public:
	~SoundThreadedBackend();
	virtual void SetSoundVolume(double volume) override;
	virtual void SetMusicVolume(double volume) override;
	virtual bool PlayMusic(const std::string& name, bool loop) override;
	virtual bool PlaySound(const std::string& name) override;
	virtual void StopSounds() override;
	virtual void StopMusic() override;
	virtual void PauseMusic(bool pause) override;
	virtual bool MusicPlaying() override;
	virtual void Tick() override;
	virtual std::vector<epro::path_stringview> GetSupportedSoundExtensions() const override {
		return m_BaseBackend->GetSupportedSoundExtensions();
	}
	virtual std::vector<epro::path_stringview> GetSupportedMusicExtensions() const override {
		return m_BaseBackend->GetSupportedMusicExtensions();
	}
protected:
	explicit SoundThreadedBackend(std::unique_ptr<SoundBackend>&&);
private:
	enum class ActionType {
		SET_SOUND_VOLUME,
		SET_MUSIC_VOLUME,
		PLAY_MUSIC,
		PLAY_SOUND,
		STOP_SOUNDS,
		STOP_MUSIC,
		PAUSE_MUSIC,
		MUSIC_PLAYING,
		TICK,
		TERMINATE
	};
	enum class ResponseType {
		PLAY_MUSIC,
		PLAY_SOUND,
		MUSIC_PLAYING
	};
	union Argument {
		struct {
			const std::string* name;
			bool loop;
		} play_music;
		const std::string* play_sound;
		double volume;
		bool pause;
	};
	struct Action {
		ActionType type;
		Argument arg;
	};
	void BaseLoop();
	std::unique_ptr<SoundBackend> m_BaseBackend;
	std::mutex m_ActionMutex;
	std::condition_variable m_ActionCondVar;
	std::mutex m_ResponseMutex;
	std::condition_variable m_ResponseCondVar;
	std::queue<Action> m_Actions;
	bool response;
	std::thread m_BaseThread;
};

template<typename T>
class SoundThreadedBackendHelper : public SoundThreadedBackend {
public:
	SoundThreadedBackendHelper() : SoundThreadedBackend(std::move(std::unique_ptr<SoundBackend>(new T()))) {}
	~SoundThreadedBackendHelper() = default;
};

#endif //SOUND_THREADED_BACKEND_H