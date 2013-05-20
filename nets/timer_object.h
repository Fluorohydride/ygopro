#ifndef _TIMER_OBJECT_H_
#define _TIMER_OBJECT_H_

#include "event2/event.h"
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

namespace ygopro
{

	class TimerObject {
	public:
		TimerObject();
		virtual ~TimerObject();

		virtual void TimeoutCallback(short events) = 0;
		virtual void TimerBegin(unsigned int msec);
		virtual void TimerEnd();
		void SetEventPtr(event* evt);
		static void TimerCallback(evutil_socket_t fd, short events, void* arg);

	private:
		event* event_ptr;

	};

}
#endif
