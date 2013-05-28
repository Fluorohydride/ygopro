#ifndef _NETS_IMPL_H_
#define _NETS_IMPL_H_

#include "../common/tcpserver.h"

namespace ygopro {

	class Player;

	class PlayerConnection: public ClientConnectionSeed {
		friend class PlayerServer;

	protected:
		Player* ref_player;

	public:
		PlayerConnection(): ref_player(nullptr) {

		}

		virtual void Connected();
		virtual void Disconnected();

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
