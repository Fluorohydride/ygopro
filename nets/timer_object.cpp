#include "timer_object.h"
#include "nets_impl.h"

namespace ygopro
{

	TimerObject::TimerObject(): event_ptr(nullptr) {
		
	}

	TimerObject::~TimerObject() {
		if(event_ptr)
			event_free(event_ptr);
	}

	void TimerObject::TimerBegin(unsigned int msec) {
		if(event_ptr) {
			timeval timeout = {msec / 1000, msec % 1000};
			event_add(event_ptr, &timeout);
		}
	}

	void TimerObject::TimerEnd() {
		if(event_ptr)
			event_del(event_ptr);
	}

	void TimerObject::SetEventPtr(event* evt) {
		event_ptr = evt;
	}

	void TimerObject::TimerCallback(evutil_socket_t fd, short events, void* arg) {
		TimerObject* ptimer = static_cast<TimerObject*>(arg);
		ptimer->TimeoutCallback(events);
	}

}
