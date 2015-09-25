#ifndef _DUEL_SERVER_H_
#define _DUEL_SERVER_H_

#include "../common/tcpserver.h"

namespace ygopro {

	class DuelConnection: public ClientConnectionSeed {
		friend class DuelServer;

	public:
		DuelConnection() {}

		virtual void Connected();
		virtual void Disconnected();

		virtual void HandleData(bufferevent* bev);
		virtual void HandlePacket(unsigned char proto, unsigned char data[], unsigned int size);
	};

	class DuelServer: public TcpServer {
	public:
		virtual ClientConnection* NewConnection() {
			return new DuelConnection();
		}

	};

}

#endif
