#ifndef _DUEL_MESSAGE_H_
#define _DUEL_MESSAGE_H_

#include <queue>
#include <mutex>

namespace ygopro
{

	struct DuelMessageData {
		unsigned int msg;
		unsigned int size;
		void* data;
	};

	class DuelMessage {

	private:
		std::mutex msg_mutex;
		std::queue<DuelMessageData> msg_queue;

	public:
		virtual void PushMessage(unsigned int msg, unsigned int size, void* data) {
			msg_mutex.lock();
			DuelMessageData dmd;
			dmd.msg = msg;
			dmd.size = size;
			dmd.data = new unsigned char[size];
			memcpy(dmd.data, data, size);
			msg_queue.push(dmd);
			msg_mutex.unlock();
		}

		virtual void CheckMessage() {
			msg_mutex.lock();
			if(msg_queue.empty())
				return;
			DuelMessageData dmd = msg_queue.front();
			msg_queue.pop();
			msg_mutex.unlock();
			HandleMessage(dmd.msg, dmd.size, dmd.data);
			delete[] dmd.data;
		}

		virtual void HandleMessage(unsigned int msg, unsigned int size, void* data) = 0;
		
	};

}

#endif
