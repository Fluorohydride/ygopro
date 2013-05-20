#include "clan.h"
#include "clan_mgr.h"
#include "dbadapter.h"
#include <algorithm>

namespace ygopro
{

	ClanMgr clanMgr;

	ClanMgr::ClanMgr() {
	}

	ClanMgr::~ClanMgr() {
	}

	void ClanMgr::LoadClans() {
		MongoQuery mq = MongoExecutor(dbAdapter)("$query")()("$orderby", "_id", 1) << "ygo.clan";
		while(mq++) {
			Clan* new_clan = new Clan;
			mq("_id", new_clan->uid)
				("creator", new_clan->creator)
				("name", new_clan->name)
				("create_time", new_clan->create_time)
				("clan_img", new_clan->clan_img);
			clans[new_clan->uid] = new_clan;
			std::string namekey = new_clan->name;
			std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
			clanKey[namekey] = new_clan;
		}
	}

	Clan* ClanMgr::NewClan(int owner_id, const std::string& name) {
		Clan* new_clan = new Clan;
		new_clan->creator = owner_id;
		new_clan->name = name;
		do {
			new_clan->uid = globalRandom.GetUInt32();
		} while(clans.find(new_clan->uid) != clans.end());
		new_clan->creator = owner_id;
		new_clan->create_time = time(0);
		new_clan->clan_img = 0;
		clans[new_clan->uid] = new_clan;
		std::string namekey = new_clan->name;
		std::transform(namekey.begin(), namekey.end(), namekey.begin(), toupper);
		clanKey[namekey] = new_clan;
		MongoExecutor(dbAdapter, true)("_id", new_clan->uid)("creator", new_clan->creator)("name", name)("create_time", new_clan->create_time) + "ygo.clan";
		return new_clan;
	}

	Clan* ClanMgr::FindClan(int uid) {
		auto iter = clans.find(uid);
		if(iter == clans.end())
			return nullptr;
		return iter->second;
	}

}
