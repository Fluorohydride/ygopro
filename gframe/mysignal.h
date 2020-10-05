#ifndef SIGNAL_H
#define SIGNAL_H

#include <condition_variable>
#include <atomic>

class Signal {
public:
	Signal():_nowait(false){}
	~Signal() {}
	void Set() {
		val.notify_all();
	}
	void Wait() {
		if(_nowait)
			return;
		std::unique_lock<std::mutex> lk(mut);
		val.wait(lk);
	}
	void Wait(std::unique_lock<std::mutex>& _Lck) {
		if(_nowait)
			return;
		val.wait(_Lck);
	}
	void SetNoWait(bool nowait) {
		if((_nowait = nowait) == true)
			Set();
	}
private:
	std::mutex mut;
	std::condition_variable val;
	bool _nowait;
};

#endif // SIGNAL_H
