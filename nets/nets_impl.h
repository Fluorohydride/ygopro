#ifndef _NETS_IMPL_H_
#define _NETS_IMPL_H_

#include "../common/tcpserver.h"
#include "timer_object.h"

namespace ygopro {

	class Player;

	class PlayerConnection: public ClientConnection {
		friend class PlayerServer;

	protected:
		unsigned int response_seed;
		Player* ref_player;

	public:
		PlayerConnection() {
			response_seed = 0;
			ref_player = nullptr;
		}

		virtual void Connected();
		virtual void Disconnected();

		virtual void HandleData(bufferevent* bev);
		virtual void HandlePacket(unsigned char proto, unsigned char data[], unsigned int size);
	};

	class PlayerServer: public TcpServer {
	public:
		virtual ClientConnection* NewConnection() {
			return new PlayerConnection();
		}

		void RegisterTimerEvent(TimerObject* timer, short flag = EV_TIMEOUT) {
			timer->TimerEnd();
			event* event_ptr = event_new(loop_base, 0, flag, TimerObject::TimerCallback, timer);
			timer->SetEventPtr(event_ptr);
		}
		
	};

	extern PlayerServer playerServer;
}

#endif
