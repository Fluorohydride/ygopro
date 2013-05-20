#ifndef _CLAN_H_
#define _CLAN_H_

#include "../common/common.h"
#include "../common/packet_handler.h"
#include "dbobject.h"
#include <set>

namespace ygopro
{

	class Player;

	class Clan: public DBObject, public PacketStream {

	public:
		Clan();
		~Clan();

		virtual void SendPacket(unsigned char proto, void* buffer, size_t len);
		virtual void SendPacket(PacketWriter& pkt);
		virtual const char* GetCollection() { return "ygo.clan"; }

	public:
		int creator;
		int create_time;
		int clan_img;
		std::string name;
		std::set<Player*> members;

	};

}

#endif
