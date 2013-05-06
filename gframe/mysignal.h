#ifndef SIGNAL_H
#define SIGNAL_H

#ifdef _WIN32

#include <windows.h>

class Signal {
public:
	Signal() {
		_event = CreateEvent(0, FALSE, FALSE, 0);
		_nowait = false;
	}
	~Signal() {
		CloseHandle(_event);
	}
	void Set() {
		SetEvent(_event);
	}
	void Reset() {
		ResetEvent(_event);
	}
	void Wait() {
		if(_nowait)
			return;
		WaitForSingleObject(_event, INFINITE);
	}
	bool Wait(long milli) {
		if(_nowait)
			return false;
		return WaitForSingleObject(_event, milli + 1) != WAIT_TIMEOUT;
	}
	void SetNoWait(bool nowait) {
		_nowait = nowait;
	}
private:
	HANDLE _event;
	bool _nowait;
};

#else // _WIN32

#include <sys/time.h>
#include <pthread.h>

class Signal {
public:
	Signal() {
		_state = false;
		_nowait = false;
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_cond, NULL);
	}
	~Signal() {
		pthread_cond_destroy(&_cond);
		pthread_mutex_destroy(&_mutex);
	}
	void Set() {
		if(pthread_mutex_lock(&_mutex))
			return;
		_state = true;
		if(pthread_cond_broadcast(&_cond))
		{
			pthread_mutex_unlock(&_mutex);
			// ERROR Broadcasting event status!
			return;
		}
		pthread_mutex_unlock(&_mutex);
	}
	void Reset() {
		if(pthread_mutex_lock(&_mutex))
			return;
		_state = false;
		pthread_mutex_unlock(&_mutex);
	}
	void Wait() {
		if(_nowait || pthread_mutex_lock(&_mutex))
			return;
		while(!_state)
		{
			if(pthread_cond_wait(&_cond, &_mutex))
			{
				pthread_mutex_unlock(&_mutex);
				// ERROR Waiting events;
				return;
			}
		}
		_state = false;
		pthread_mutex_unlock(&_mutex);
	}

	bool Wait(long milliseconds)
	{
		if (_nowait || pthread_mutex_lock(&_mutex) != 0)
			return false;

		int rc = 0;
		struct timespec abstime;

		struct timeval tv;
		gettimeofday(&tv, NULL);
		abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
		abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000)*1000000;
		if (abstime.tv_nsec >= 1000000000)
		{
			abstime.tv_nsec -= 1000000000;
			abstime.tv_sec++;
		}

		while (!_state) 
		{
			if ((rc = pthread_cond_timedwait(&_cond, &_mutex, &abstime)))
			{
				if (rc == ETIMEDOUT) break;
				pthread_mutex_unlock(&_mutex);
				return false;
			}
		}
		_state = false;
		pthread_mutex_unlock(&_mutex);
		return rc == 0;
	}
	void SetNoWait(bool nowait) {
		_nowait = nowait;
	}
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
	bool _state;
	bool _nowait;
};

#endif // _WIN32

#endif // SIGNAL_H
