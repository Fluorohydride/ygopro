#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include "config.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"

namespace ygo {

class DuelClient {
private:
	static unsigned int connect_state;
	static int responseI;
	static unsigned char responseB[64];
	static bool is_responseB;
	static unsigned char response_len;
public:
	static bool StartClient(unsigned int ip, unsigned short port);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ptr);
	static int ClientThread(void* param);
	static void HandleSTOCPacket(char* data, unsigned int len);
	static void SetResponseI(int respI);
	static void SetResponseB(unsigned char* respB, unsigned char len);
};

}

#endif //DUELCLIENT_H
