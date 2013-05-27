#ifndef _NETS_IMPL_H_
#define _NETS_IMPL_H_

#include "../common/tcpserver.h"

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
		
	};

}

#endif
