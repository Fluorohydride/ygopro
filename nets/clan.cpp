#include "clan.h"
#include "player.h"
#include <algorithm>

namespace ygopro
{

	Clan::Clan() {
	}

	Clan::~Clan() {
	}

	void Clan::SendPacket(unsigned char proto, void* buffer, size_t len) {
		for(auto& iter : members)
			iter->SendPacket(proto, buffer, len);
	}

	void Clan::SendPacket(PacketWriter& pkt) {
		for(auto& iter : members)
			iter->SendPacket(pkt);
	}

}
