#ifndef MUTEX_H
#define MUTEX_H

#ifdef _WIN32

#include <windows.h>

class Mutex {
public:
	Mutex() {
		InitializeCriticalSection(&_mutex);
	}
	~Mutex() {
		DeleteCriticalSection(&_mutex);
	}
	void Lock() {
		EnterCriticalSection(&_mutex);
	}
	void Unlock() {
		LeaveCriticalSection(&_mutex);
	}
	bool TryLock() {
		return TryEnterCriticalSection(&_mutex) == TRUE;
	}
private:
	CRITICAL_SECTION _mutex;
};

#else // _WIN32

#include <pthread.h>

class Mutex {
public:
	Mutex() {
		pthread_mutex_init(&mutex_t, NULL);
	}
	~Mutex() {
		pthread_mutex_destroy(&mutex_t);
	}
	void Lock() {
		pthread_mutex_lock(&mutex_t);
	}
	void Unlock() {
		pthread_mutex_unlock(&mutex_t);
	}
	bool TryLock() {
		return !pthread_mutex_trylock(&mutex_t);
	}
private:
	pthread_mutex_t mutex_t;
};

#endif // _WIN32

#endif // MUTEX_H
