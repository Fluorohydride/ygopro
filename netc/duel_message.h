#ifndef _DUEL_MESSAGE_H_
#define _DUEL_MESSAGE_H_

#include <list>
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
		std::list<DuelMessageData> msg_queue;

	public:
		virtual ~DuelMessage() {
			for(auto& iter : msg_queue)
				delete[] iter.data;
		}

		virtual void PushMessage(unsigned int msg, unsigned int size, void* data) {
			msg_mutex.lock();
			DuelMessageData dmd;
			dmd.msg = msg;
			dmd.size = size;
			dmd.data = new unsigned char[size];
			memcpy(dmd.data, data, size);
			msg_queue.push_back(dmd);
			msg_mutex.unlock();
		}

		virtual void CheckMessage() {
			if(msg_queue.empty())
				return;
			msg_mutex.lock();
			DuelMessageData dmd = msg_queue.front();
			msg_queue.pop_front();
			msg_mutex.unlock();
			HandleMessage(dmd.msg, dmd.size, dmd.data);
			delete[] dmd.data;
		}

		virtual void HandleMessage(unsigned int msg, unsigned int size, void* data) = 0;
		
	};

}

#endif
