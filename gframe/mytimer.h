#ifndef TIMER_H
#define TIMER_H

#ifdef _WIN32

#include <windows.h>

class Timer {
public:
	Timer() {
		hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	}
	~Timer() {
		CloseHandle(hTimer);
	}
	void Reset() {
		QueryPerformanceCounter(&startTime);
	}
	float GetElapsedTime() {
		LARGE_INTEGER endTime;
		LARGE_INTEGER timerFreq;
		QueryPerformanceFrequency(&timerFreq);
		QueryPerformanceCounter(&endTime);
		return (endTime.QuadPart - startTime.QuadPart) * 1000000.0f / timerFreq.QuadPart;
	}
	void Wait(long long us) {
		LARGE_INTEGER liDueTime;
		liDueTime.QuadPart = us * -10LL;
		SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0);
		WaitForSingleObject(hTimer, INFINITE);
	}
private:
	LARGE_INTEGER startTime;
	HANDLE hTimer;
};

#else // _WIN32

#include <sys/time.h>
#include <unistd.h>

class Timer {
public:
	Timer() {
	}
	~Timer() {
	}
	void Reset() {
		gettimeofday(&startTime, 0);
	}
	float GetElapsedTime() {
		timeval endTime;
		gettimeofday(&endTime, 0);
		return (endTime.tv_sec - startTime.tv_sec) * 1000000.0f + endTime.tv_usec - startTime.tv_usec;
	}
	void Wait(long long us) {
		usleep(us);
	}
private:
	timeval startTime;
};

#endif // _WIN32

#endif // TIMER_H
