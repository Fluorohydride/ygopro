#ifndef _DUEL_THREAD_H_
#define _DUEL_THREAD_H_

#include "../common/common.h"
#include "../common/tcpserver.h"
#include <queue>

namespace ygopro {

	class DuelBase;

	class DuelClient : public ClientConnection {
	public:
		// connection event
		virtual void Connected() {}
		virtual void Disconnected() {}
		virtual void HandlePacket(unsigned char proto, unsigned char data[], unsigned int size) {}
	};

	class DuelThread : public TcpServer {

		friend class DuelManager;
		friend struct DuelThreadSort;

	protected:
		DuelThread(): duel_count(0) {
		}

	public:
		~DuelThread() {
		}

		virtual ClientConnection* NewConnection() {
			return new DuelClient();
		}

		void PendingDuel();

	public:
		static void ThreadFunc(DuelThread* pthreadInfo, unsigned int port) {
			pthreadInfo->Run(port);
		}

		static DuelThread* NewDuelThread(unsigned int port) {
			DuelThread* pthread = new DuelThread();
			std::thread new_thread(ThreadFunc, pthread, port);
			return pthread;
		}

	protected:
		unsigned int duel_count;
		bool is_quit;
	};

	struct DuelThreadSort {
		bool operator()(const DuelThread* t1, const DuelThread* t2) const
		{
			if(t1->duel_count != t2->duel_count)
				return t1->duel_count < t2->duel_count;
			return t1 < t2;
		}
	};

}

#endif
