#ifndef SIGNAL_H
#define SIGNAL_H

#include <condition_variable>
#include <atomic>

class Signal {
public:
	Signal():_nowait(false), _signaled(false){}
	~Signal() {}
	void Set() {
		val.notify_all();
		_signaled = true;
	}
	void Reset() {
		_signaled = false;
	}
	void Wait() {
		if(_nowait)
			return;
		std::unique_lock<std::mutex> lk(mut);
		val.wait(lk);
		_signaled = false;
	}
	bool Wait(int milli) {
		if(_nowait)
			return false;
		std::unique_lock<std::mutex> lk(mut);
		auto res = val.wait_for(lk, std::chrono::milliseconds(milli),[this]{ return _signaled.load(); });
		_signaled = false;
		return res;
	}
	void SetNoWait(bool nowait) {
		_nowait = nowait;
		if (nowait){
			Set();
		}
	}
private:
	std::mutex mut;
	std::condition_variable val;
	bool _nowait;
	std::atomic<bool> _signaled;
};

#endif // SIGNAL_H
