#ifndef SIGNAL_H
#define SIGNAL_H

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Signal {
public:
	Signal() {
		_state = false;
		_nowait = false;
	}
	~Signal() {

	}
	void Set() {
		std::unique_lock<std::mutex> lock(_mutex);
		_state = true;
		_cond.notify_all();
	}
	void Reset() {
		std::unique_lock<std::mutex> lock(_mutex);
		_state = false;
	}
	void Wait() {
		if(_nowait.load())
			return;
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this]() { return _state; });
		_state = false;
	}

	bool Wait(long milliseconds) {
		if(_nowait.load())
			return false;
		std::unique_lock<std::mutex> lock(_mutex);
		bool res = _cond.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]() { return _state; });
		_state = false;
		return res;
	}
	bool TryWait() {
		if(_nowait.load())
			return false;
		std::unique_lock<std::mutex> lock(_mutex);
		bool res = _state;
		_state = false;
		return res;
	}
	void SetNoWait(bool nowait) {
		_nowait.store(nowait);
	}
private:
	std::mutex _mutex;
	std::condition_variable _cond;
	bool _state;
	std::atomic<bool> _nowait;
};

#endif // SIGNAL_H
