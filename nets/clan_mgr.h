#ifndef _CLAN_MGR_H_
#define _CLAN_MGR_H_

#include "../common/common.h"
#include "../common/packet_handler.h"
#include "dbobject.h"
#include <unordered_map>

namespace ygopro
{
	class Clan;

	class ClanMgr {
	public:
		ClanMgr();
		~ClanMgr();

		void LoadClans();
		Clan* NewClan(int owner_id, const std::string& name);
		Clan* FindClan(int uid);

	protected:
		std::unordered_map<int, Clan*> clans;
		std::unordered_map<std::string, Clan*> clanKey;
	};

	extern ClanMgr clanMgr;

}

#endif
