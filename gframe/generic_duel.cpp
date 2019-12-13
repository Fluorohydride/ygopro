#include "generic_duel.h"
#include "netserver.h"
#include "game.h"
#include "core_utils.h"
#include <random>

namespace ygo {

ReplayStream GenericDuel::replay_stream;

GenericDuel::GenericDuel(int team1, int team2, bool relay, int best_of) :relay(relay), best_of(best_of), match_kill(0) {
	players.home.resize(team1);
	players.opposing.resize(team2);
	players.home_size = team1;
	players.opposing_size = team2;
}
GenericDuel::~GenericDuel() {
}
#define ITERATE_PLAYERS(func)for(auto& dueler:players.home){\
		if(dueler.player){\
			func\
		}\
	}\
	for(auto& dueler:players.opposing){\
		if(dueler.player){\
			func\
		}\
	}
#define ITERATE_PLAYERS_AND_OBS(func)for(auto& dlr:players.home){\
		if(dlr.player){\
			auto dueler = dlr.player;\
			func\
		}\
	}\
	for(auto& dlr:players.opposing){\
		if(dlr.player){\
			auto dueler = dlr.player;\
			func\
		}\
	}\
	for(auto& dueler:observers){\
		func\
	}
void GenericDuel::Chat(DuelPlayer* dp, void* pdata, int len) {
	STOC_Chat scc;
	scc.player = dp->type;
	unsigned short* msg = (unsigned short*)pdata;
	int msglen = BufferIO::CopyWStr(msg, scc.msg, 256);
	ITERATE_PLAYERS_AND_OBS(NetServer::SendBufferToPlayer(dueler, STOC_CHAT, &scc, 4 + msglen * 2);)
}
bool GenericDuel::CheckReady() {
	bool ready1 = false, ready2 = false;
	for(auto& dueler : players.home) {
		if(dueler.player) {
			ready1 = dueler.ready;
		} else if(!relay) {
			return false;
		}
	}
	for(auto& dueler : players.opposing) {
		if(dueler.player) {
			ready2 = dueler.ready;
		} else if(!relay) {
			return false;
		}
	}
	return ready1 && ready2;
}
int GenericDuel::GetCount(const std::vector<duelist>& players) {
	int res = 0;
	for(auto& dueler : players) {
		if(dueler.player)
			res++;
	}
	return res;
}
bool GenericDuel::CheckFree(const std::vector<duelist>& players) {
	for(auto& dueler : players) {
		if(!dueler.player)
			return true;
	}
	return false;
}
int GenericDuel::GetFirstFree(int start) {
	int tot_size = players.home.size() + players.opposing.size();
	for(int i = start, j = 0; j < tot_size; i = (i+1) % tot_size, j++) {
		if(i < players.home.size()) {
			if(!players.home[i].player)
				return i;
		} else {
			if(!players.opposing[i - players.home.size()].player)
				return i;
		}
	}
	return -1;
}
int GenericDuel::GetPos(DuelPlayer* dp) {
	for(int i = 0; i < players.home.size(); i++) {
		if(players.home[i].player == dp)
			return i;
	}
	for(int i = 0; i < players.opposing.size(); i++) {
		if(players.opposing[i].player == dp)
			return i + players.home.size();
	}
	return -1;
}
void GenericDuel::OrderPlayers(std::vector<duelist>& duelists, int offset) {
	for(auto it = duelists.begin(); it != duelists.end();) {
		if(!it->player) {
			it = duelists.erase(it);
			continue;
		}
		it++;
	}
	for(int i = 0; i < duelists.size(); i++) {
		if(duelists[i].player->type != (i + offset)) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (duelists[i].player->type << 4) | (i + offset);
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			STOC_TypeChange sctc;
			sctc.type = (duelists[i].player == host_player ? 0x10 : 0) | i;
			NetServer::SendPacketToPlayer(duelists[i].player, STOC_TYPE_CHANGE, sctc);
			duelists[i].player->type = (i + offset);
		}
	}
}
void GenericDuel::SetAtPos(DuelPlayer* dp, int pos) {
	if(pos < players.home.size()) {
		players.home[pos] = duelist(dp);
	} else {
		players.opposing[pos - players.home.size()] = duelist(dp);
	}
}
GenericDuel::duelist& GenericDuel::GetAtPos(int pos) {
	if(pos < players.home_size) {
		return players.home[pos];
	} else {
		return players.opposing[pos - players.home_size];
	}
}
void GenericDuel::Catchup(DuelPlayer * dp) {
	observers_mutex.lock();
	if(!pduel) {
		observers.insert(dp);
		observers_mutex.unlock();
		return;
	}
	observers_mutex.unlock();
	char buf = 1;
	NetServer::SendPacketToPlayer(dp, STOC_CATCHUP, buf);
	int i = 0;
	for(i = 0; i < packets_cache.size(); i++) {
		NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, packets_cache[i].data.data(), packets_cache[i].data.size());
	}
	observers_mutex.lock();
	for(; i < packets_cache.size(); i++) {
		NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, packets_cache[i].data.data(), packets_cache[i].data.size());
	}
	buf = 0;
	NetServer::SendPacketToPlayer(dp, STOC_CATCHUP, buf);
	observers.insert(dp);
	observers_mutex.unlock();
}
void GenericDuel::JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {
	if(!is_creater) {
		if(dp->game && dp->type != 0xff) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 0;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
		CTOS_JoinGame* pkt = (CTOS_JoinGame*)pdata;
		if(pkt->version != PRO_VERSION) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_VERERROR;
			scem.code = PRO_VERSION;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
		wchar_t jpass[20];
		BufferIO::CopyWStr(pkt->pass, jpass, 20);
		if(wcscmp(jpass, pass)) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 1;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
	}
	dp->game = this;
	if(duel_stage == DUEL_STAGE_FINGER || duel_stage == DUEL_STAGE_FIRSTGO || duel_stage == DUEL_STAGE_DUELING) {
		STOC_JoinGame scjg;
		scjg.info = host_info;
		STOC_TypeChange sctc;
		sctc.type = 0;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		dp->state = CTOS_LEAVE_GAME;
		ITERATE_PLAYERS(
			STOC_HS_PlayerEnter scpe;
			BufferIO::CopyWStr(dueler.player->name, scpe.name, 20);
			scpe.pos = GetPos(dueler.player);
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		)
		NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
		Catchup(dp);
		return;
	}
	if(!GetCount(players.home) && !GetCount(players.opposing) && observers.empty())
		host_player = dp;
	STOC_JoinGame scjg;
	scjg.info = host_info;
	STOC_TypeChange sctc;
	sctc.type = (host_player == dp) ? 0x10 : 0;
	if(CheckFree(players.home) || CheckFree(players.opposing)) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		scpe.pos = GetFirstFree();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_ENTER, scpe);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		SetAtPos(dp, scpe.pos);
		dp->type = scpe.pos;
		sctc.type |= scpe.pos;
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
	}
	NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	ITERATE_PLAYERS(
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dueler.player->name, scpe.name, 20);
		scpe.pos = GetPos(dueler.player);
		NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		if(dueler.ready) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (scpe.pos << 4) | PLAYERCHANGE_READY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
		})
	if(observers.size()) {
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
	}
}
void GenericDuel::LeaveGame(DuelPlayer* dp) {
	if(dp == host_player) {
		EndDuel();
		NetServer::StopServer();
	} else if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		observers_mutex.lock();
		observers.erase(dp);
		observers_mutex.unlock();
		if(duel_stage == DUEL_STAGE_BEGIN) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = observers.size();
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		if(duel_stage == DUEL_STAGE_BEGIN) {
			STOC_HS_PlayerChange scpc;
			GetAtPos(dp->type).Clear();
			scpc.status = (dp->type << 4) | PLAYERCHANGE_LEAVE;
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			NetServer::DisconnectPlayer(dp);
		} else {
			if(duel_stage == DUEL_STAGE_BEGIN) {
				ITERATE_PLAYERS(
					if(!dueler.ready)
						NetServer::SendPacketToPlayer(dueler.player, STOC_DUEL_START);
				)
			}
			if(duel_stage != DUEL_STAGE_END) {
				unsigned char wbuf[3];
				uint32 player = dp->type < players.home_size ? 0 : 1;
				wbuf[0] = MSG_WIN;
				wbuf[1] = 1 - player;
				wbuf[2] = 0x4;
				NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
				ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
				EndDuel();
				NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
				ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			}
			NetServer::DisconnectPlayer(dp);
		}
	}
}
void GenericDuel::ToDuelist(DuelPlayer* dp) {
	int pos = GetFirstFree();
	if(pos == -1)
		return;
	if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		observers.erase(dp);
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		dp->type = pos;
		scpe.pos = dp->type;
		SetAtPos(dp, scpe.pos);
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_ENTER, scpe);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	} else {
		if(GetAtPos(dp->type).ready)
			return;
		pos = GetFirstFree(dp->type);
		if(pos == -1)
			return;
		STOC_HS_PlayerChange scpc;
		scpc.status = (dp->type << 4) | pos;
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | pos;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
		SetAtPos(dp, pos);
		GetAtPos(dp->type).Clear();
		dp->type = pos;
	}
}
void GenericDuel::ToObserver(DuelPlayer* dp) {
	if(dp->type >= (players.home_size + players.opposing_size))
		return;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | PLAYERCHANGE_OBSERVE;
	NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
	ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
	GetAtPos(dp->type).Clear();
	dp->type = NETPLAYER_TYPE_OBSERVER;
	observers.insert(dp);
	STOC_TypeChange sctc;
	sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
}
void GenericDuel::PlayerReady(DuelPlayer* dp, bool is_ready) {
	auto& dueler = GetAtPos(dp->type);
	if(dp->type >= (players.home_size + players.opposing_size) || dueler.ready == is_ready)
		return;
	if(is_ready) {
		unsigned int deckerror = 0;
		if(!host_info.no_check_deck) {
			if(dueler.deck_error) {
				deckerror = (DECKERROR_UNKNOWNCARD << 28) + dueler.deck_error;
			} else {
				bool allow_ocg = host_info.rule == 0 || host_info.rule == 2;
				bool allow_tcg = host_info.rule == 1 || host_info.rule == 2;
				bool allow_prerelease = true;
				deckerror = deckManager.CheckDeck(dueler.pdeck, host_info.lflist, allow_ocg, allow_tcg, allow_prerelease, host_info.extra_rules & DOUBLE_DECK, host_info.forbiddentypes);
			}
		}
		if(deckerror) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (dp->type << 4) | PLAYERCHANGE_NOTREADY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_DECKERROR;
			scem.code = deckerror;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
	}
	dueler.ready = is_ready;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | (is_ready ? PLAYERCHANGE_READY : PLAYERCHANGE_NOTREADY);
	NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
	ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
}
void GenericDuel::PlayerKick(DuelPlayer* dp, unsigned char pos) {
	auto& dueler = GetAtPos(pos);
	if(pos > 5 || dp != host_player || dp == dueler.player || !dueler.player)
		return;
	LeaveGame(dueler.player);
}
void GenericDuel::UpdateDeck(DuelPlayer* dp, void* pdata, unsigned int len) {
	auto& dueler = GetAtPos(dp->type);
	if(dp->type >= (players.home_size + players.opposing_size) || dueler.ready)
		return;
	char* deckbuf = (char*)pdata;
	uint32_t mainc = BufferIO::Read<uint32_t>(deckbuf);
	uint32_t sidec = BufferIO::Read<uint32_t>(deckbuf);
	// verify data
	if(mainc + sidec > (len - 8) / 4) {
		STOC_ErrorMsg scem;
		scem.msg = ERRMSG_DECKERROR;
		scem.code = 0;
		NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
		return;
	}
	if(match_result.empty()) {
		dueler.deck_error = deckManager.LoadDeck(dueler.pdeck, (int*)deckbuf, mainc, sidec);
	} else {
		if(deckManager.LoadSide(dueler.pdeck, (int*)deckbuf, mainc, sidec)) {
			dueler.ready = true;
			NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
			if(CheckReady()) {
				auto& player =  match_result[match_result.size() - 1] == 0 ? players.opposing.front().player : players.home.front().player;
				NetServer::SendPacketToPlayer(player, STOC_SELECT_TP);
				ITERATE_PLAYERS(
					if(dueler.player != player)
						dueler.player->state = 0xff;
				)
				player->state = CTOS_TP_RESULT;
				duel_stage = DUEL_STAGE_FIRSTGO;
			}
		} else {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_SIDEERROR;
			scem.code = 0;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
		}
	}
}
void GenericDuel::StartDuel(DuelPlayer* dp) {
	if(dp != host_player)
		return;
	ITERATE_PLAYERS(
		if (!dueler.ready)
			return;
	)
	if(!CheckReady())
		return;
	OrderPlayers(players.home);
	OrderPlayers(players.opposing, players.home_size);
	players.home_iterator = players.home.begin();
	players.opposing_iterator = players.opposing.begin();
	//NetServer::StopListen();
	//NetServer::StopBroadcast();
	ITERATE_PLAYERS(NetServer::SendPacketToPlayer(dueler.player, STOC_DUEL_START);)
	observers_mutex.lock();
	for(auto& obs : observers) {
		obs->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(obs);
	}
	observers_mutex.unlock();
	NetServer::SendPacketToPlayer(players.home.front().player, STOC_SELECT_HAND);
	NetServer::ReSendToPlayer(players.opposing.front().player);
	hand_result[0] = 0;
	hand_result[1] = 0;
	players.home.front().player->state = CTOS_HAND_RESULT;
	players.opposing.front().player->state = CTOS_HAND_RESULT;
	duel_stage = DUEL_STAGE_FINGER;
}
void GenericDuel::HandResult(DuelPlayer* dp, unsigned char res) {
	if(res > 3 || dp->state != CTOS_HAND_RESULT)
		return;
	if(dp->type == 0)
		hand_result[0] = res;
	else
		hand_result[1] = res;
	if(hand_result[0] && hand_result[1]) {
		STOC_HandResult schr;
		schr.res1 = hand_result[0];
		schr.res2 = hand_result[1];
		for (auto& dueler : players.home)
			NetServer::SendPacketToPlayer(dueler.player, STOC_HAND_RESULT, schr);
		observers_mutex.lock();
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		observers_mutex.unlock();
		schr.res1 = hand_result[1];
		schr.res2 = hand_result[0];
		for(auto& dueler : players.opposing)
			NetServer::SendPacketToPlayer(dueler.player, STOC_HAND_RESULT, schr);
		if(hand_result[0] == hand_result[1]) {
			NetServer::SendPacketToPlayer(players.home.front().player, STOC_SELECT_HAND);
			NetServer::ReSendToPlayer(players.opposing.front().player);
			hand_result[0] = 0;
			hand_result[1] = 0;
			players.home.front().player->state = CTOS_HAND_RESULT;
			players.opposing.front().player->state = CTOS_HAND_RESULT;
		} else if((hand_result[0] == 1 && hand_result[1] == 2)
		          || (hand_result[0] == 2 && hand_result[1] == 3)
		          || (hand_result[0] == 3 && hand_result[1] == 1)) {
			NetServer::SendPacketToPlayer(players.opposing.front().player, STOC_SELECT_TP);
			players.home.front().player->state = 0xff;
			players.opposing.front().player->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		} else {
			NetServer::SendPacketToPlayer(players.home.front().player, STOC_SELECT_TP);
			players.opposing.front().player->state = 0xff;
			players.home.front().player->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		}
	}
}
void GenericDuel::TPResult(DuelPlayer* dp, unsigned char tp) {
	if(dp->state != CTOS_TP_RESULT)
		return;
	duel_stage = DUEL_STAGE_DUELING;
	bool swapped = false;
	if((tp && dp == players.opposing.front().player) || (!tp && dp == players.home.front().player)) {
		std::swap(players.opposing, players.home);
		for(int i = 0; i < players.home.size(); i++) {
			players.home[i].player->type = i;
		}
		for(int i = 0; i < players.opposing.size(); i++) {
			players.opposing[i].player->type = i + players.home_size;
		}
		swapped = true;
	}
	turn_count = 0;
	cur_player[0] = players.home.front().player;
	cur_player[1] = players.opposing.front().player;
	dp->state = CTOS_RESPONSE;
	ReplayHeader rh;
	rh.id = REPLAY_YRP1;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_LUA64 | REPLAY_NEWREPLAY;
	time_t seed = time(0);
	rh.seed = seed;
	last_replay.BeginRecord(false);
	last_replay.WriteHeader(rh);
	std::mt19937 rnd(seed);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.id = REPLAY_YRPX;
	new_replay.WriteHeader(rh);
	last_replay.Write<uint32_t>(players.home.size(), false);
	new_replay.Write<uint32_t>(players.home.size(), false);
	for(auto& dueler : players.home) {
		last_replay.WriteData(dueler.player->name, 40, false);
		new_replay.WriteData(dueler.player->name, 40, false);
	}
	last_replay.Write<uint32_t>(players.opposing.size(), false);
	new_replay.Write<uint32_t>(players.opposing.size(), false);
	for(auto& dueler : players.opposing) {
		last_replay.WriteData(dueler.player->name, 40, false);
		new_replay.WriteData(dueler.player->name, 40, false);
	}
	replay_stream.clear();
	time_limit[0] = host_info.time_limit;
	time_limit[1] = host_info.time_limit;
	int opt = host_info.duel_flag;
	if(host_info.no_shuffle_deck)
		opt |= DUEL_PSEUDO_SHUFFLE;
	if(host_info.extra_rules & SPEED_DUEL)
		opt |= DUEL_SPEED;
	OCG_Player team = { host_info.start_lp, host_info.start_hand, host_info.draw_count };
	pduel = mainGame->SetupDuel({ rnd(), opt, team, team });
	if(!host_info.no_shuffle_deck) {
		ITERATE_PLAYERS(std::shuffle(dueler.pdeck.main.begin(), dueler.pdeck.main.end(), rnd);)
	}
	new_replay.Write<uint32_t>((mainGame->GetMasterRule(opt, 0)) | (opt & DUEL_SPEED) << 8);
	last_replay.Write<uint32_t>(host_info.start_lp, false);
	last_replay.Write<uint32_t>(host_info.start_hand, false);
	last_replay.Write<uint32_t>(host_info.draw_count, false);
	last_replay.Write<uint32_t>(opt, false);
	last_replay.Flush();
	//
	std::vector<unsigned int> extracards;
	if(host_info.extra_rules & SEALED_DUEL)
		extracards.push_back(511005092);
	if(host_info.extra_rules & BOOSTER_DUEL)
		extracards.push_back(511005093);
	if(host_info.extra_rules & CONCENTRATION_DUEL)
		extracards.push_back(511004322);
	if(host_info.extra_rules & BOSS_DUEL)
		extracards.push_back(95000000);
	if(host_info.extra_rules & BATTLE_CITY)
		extracards.push_back(511004014);
	if(host_info.extra_rules & DUELIST_KINGDOM)
		extracards.push_back(511002621);
	if(host_info.extra_rules & DIMENSTION_DUEL)
		extracards.push_back(511600002);
	if(host_info.extra_rules & TURBO_DUEL)
		extracards.push_back(302);
	if(host_info.extra_rules & COMMAND_DUEL)
		extracards.push_back(95200000);
	if(host_info.extra_rules & DECK_MASTER)
		extracards.push_back(300);
	if(host_info.extra_rules & DESTINY_DRAW)
		extracards.push_back(511004000);
	OCG_NewCardInfo card_info = { 0, 0, 0, 0, 0, 0, POS_FACEDOWN_DEFENSE };
	for(int32 i = (int32)extracards.size() - 1; i >= 0; --i) {
		card_info.code = extracards[i];
		OCG_DuelNewCard(pduel, card_info);
	}
	for(int32 j = 0; j < players.home.size(); j++) {
		auto& dueler = players.home[j];
		card_info.duelist = j;
		card_info.loc = LOCATION_DECK;
		last_replay.Write<uint32_t>(dueler.pdeck.main.size(), false);
		for(int32 i = (int32)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.main[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.main[i]->code, false);
		}
		card_info.loc = LOCATION_EXTRA;
		last_replay.Write<uint32_t>(dueler.pdeck.extra.size(), false);
		for(int32 i = (int32)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.extra[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	card_info.team = 1;
	card_info.con = 1;
	for(int32 j = 0; j < players.opposing.size(); j++) {
		auto& dueler = players.opposing[j];
		card_info.duelist = j;
		card_info.loc = LOCATION_DECK;
		last_replay.Write<uint32_t>(dueler.pdeck.main.size(), false);
		for(int32 i = (int32)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.main[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.main[i]->code, false);
		}
		card_info.loc = LOCATION_EXTRA;
		last_replay.Write<uint32_t>(dueler.pdeck.extra.size(), false);
		for(int32 i = (int32)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.extra[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	last_replay.Write<uint32_t>(extracards.size(), false);
	for(int32 i = (int32)extracards.size() - 1; i >= 0; --i) {
		last_replay.Write<uint32_t>(extracards[i], false);
	}
	last_replay.Flush();
	char startbuf[32], *pbuf = startbuf;
	BufferIO::Write<uint8_t>(pbuf, MSG_START);
	BufferIO::Write<uint8_t>(pbuf, 0);
	BufferIO::Write<uint32_t>(pbuf, host_info.start_lp);
	BufferIO::Write<uint32_t>(pbuf, host_info.start_lp);
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 0, LOCATION_DECK));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 0, LOCATION_EXTRA));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 1, LOCATION_DECK));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 1, LOCATION_EXTRA));
	for (auto& dueler : players.home)
		NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, startbuf, 18);
	startbuf[1] = 1;
	for(auto& dueler : players.opposing)
		NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, startbuf, 18);
	if(!swapped)
		startbuf[1] = 0x10;
	else startbuf[1] = 0x11;
	observers_mutex.lock();
	for(auto& obs : observers)
		NetServer::SendBufferToPlayer(obs, STOC_GAME_MSG, startbuf, 18);
	observers_mutex.unlock();
	packets_cache.emplace_back(startbuf, 18);
	startbuf[1] = 0;
	ReplayPacket p((char*)startbuf, 17);
	replay_stream.push_back(p);
	PseudoRefreshDeck(0);
	PseudoRefreshDeck(1);
	RefreshExtra(0);
	RefreshExtra(1);
	new_replay.WriteStream(replay_stream);
	OCG_StartDuel(pduel);
	Process();
}
void GenericDuel::Process() {
	std::vector<uint8> duelBuffer;
	unsigned int engFlag = 0, message_len = 0;
	int stop = 0;
	do {
		engFlag = OCG_DuelProcess(pduel);
		auto msg = CoreUtils::ParseMessages(pduel);
		for(auto& message : msg.packets) {
			observers_mutex.lock();
			stop = Analyze(message);
			observers_mutex.unlock();
			if(stop)
				break;
		}
	} while(!stop);
	if(engFlag == OCG_DUEL_STATUS_END || stop == 2)
		DuelEndProc();
}
void GenericDuel::DuelEndProc() {
	if(match_result.size() >= best_of) {
		NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		duel_stage = DUEL_STAGE_END;
	} else {
		int winc[3] = { 0, 0, 0 };
		for(int i = 0; i < match_result.size(); ++i)
			winc[match_result[i]]++;
		int minvictories = std::ceil(best_of / 2.0);
		if(match_kill || (winc[0] > minvictories || winc[1] > minvictories)) {
			NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			duel_stage = DUEL_STAGE_END;
		} else {
			ITERATE_PLAYERS(
				dueler.ready = false;
				dueler.player->state = CTOS_UPDATE_DECK;
				NetServer::SendPacketToPlayer(dueler.player, STOC_CHANGE_SIDE);
			)
			for(auto& obs : observers)
				NetServer::SendPacketToPlayer(obs, STOC_WAITING_SIDE);
			duel_stage = DUEL_STAGE_SIDING;
		}
	}
}
void GenericDuel::Surrender(DuelPlayer* dp) {
	if((players.home.size() + players.opposing.size()) != 2 || (players.home.front().player != dp && players.opposing.front().player != dp) || !pduel)
		return;
	unsigned char wbuf[3];
	uint32 player = dp->type < players.home_size ? 0 : 1;
	wbuf[0] = MSG_WIN;
	wbuf[1] = 1 - player;
	wbuf[2] = 0;
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
	ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
	ReplayPacket p((char*)wbuf, 3);
	replay_stream.push_back(p);
	match_result.push_back(1 - player);
	EndDuel();
	DuelEndProc();
	event_del(etimer);
}
#define DATA (char*)(packet.data.data() + sizeof(uint8_t))
#define TO_SEND_BUFFER (char*)packet.data.data(), packet.data.size()
#define SEND(to) NetServer::SendBufferToPlayer(to, STOC_GAME_MSG, TO_SEND_BUFFER);
void GenericDuel::BeforeParsing(CoreUtils::Packet& packet, int& return_value, bool& record, bool& record_last) {
	char* pbuf = DATA;
	switch(packet.message) {
	case MSG_NEW_TURN: {
		RefreshMzone(0);
		RefreshMzone(1);
		RefreshSzone(0);
		RefreshSzone(1);
		break;
	}
	case MSG_FLIPSUMMONING: {
		pbuf += 4;
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
		RefreshSingle(info.controler, info.location, info.sequence);
		break;
	}
	default:
		return;
	}
}
void GenericDuel::Sending(CoreUtils::Packet& packet, int& return_value, bool& record, bool& record_last) {
	uint8_t& message = packet.message;
	uint32_t type, count;
	uint8_t player;
	char* pbufw, *pbuf = DATA;
	switch (message) {
	case MSG_RETRY: {
		WaitforResponse(last_response);
		SEND(cur_player[last_response]);
		return_value = 1;
		break;
	}
	case MSG_HINT: {
		type = BufferIO::Read<uint8_t>(pbuf);
		player = BufferIO::Read<uint8_t>(pbuf);
		switch (type) {
		case 1:
		case 2:
		case 3:
		case 5: {
			SEND(cur_player[player]);
			record = false;
			break;
		}
		case 4:
		case 6:
		case 7:
		case 8:
		case 9: {
			SEND(nullptr);
			ITERATE_PLAYERS_AND_OBS(
				if(dueler != cur_player[player])
					NetServer::ReSendToPlayer(dueler);
			)
			break;
		}
		case 10: {
			SEND(nullptr);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			break;
		}
		}
		break;
	}
	case MSG_SELECT_BATTLECMD:
	case MSG_SELECT_IDLECMD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		RefreshMzone(0);
		RefreshMzone(1);
		RefreshSzone(0);
		RefreshSzone(1);
		RefreshHand(0);
		RefreshHand(1);
		WaitforResponse(player);
		SEND(cur_player[player]);
		return_value = 1;
		break;
	}
	case MSG_WIN: {
		player = BufferIO::Read<uint8_t>(pbuf);
		type = BufferIO::Read<uint8_t>(pbuf);
		SEND(nullptr);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		if(player > 1)
			match_result.push_back(2);
		else
			match_result.push_back(1 - player);
		EndDuel();
		return_value = 2;
		break;
	}
	case MSG_SELECT_EFFECTYN:
	case MSG_SELECT_YESNO:
	case MSG_SELECT_OPTION:
	case MSG_SELECT_CHAIN:
	case MSG_SELECT_PLACE:
	case MSG_SELECT_DISFIELD:
	case MSG_SELECT_POSITION:
	case MSG_SELECT_COUNTER:
	case MSG_SELECT_SUM:
	case MSG_SORT_CARD:
	case MSG_SORT_CHAIN:
	case MSG_ROCK_PAPER_SCISSORS:
	case MSG_ANNOUNCE_RACE:
	case MSG_ANNOUNCE_ATTRIB:
	case MSG_ANNOUNCE_CARD:
	case MSG_ANNOUNCE_NUMBER: {
		player = BufferIO::Read<uint8_t>(pbuf);
		WaitforResponse(player);
		SEND(cur_player[player]);
		return_value = 1;
		break;
	}
	case MSG_SELECT_CARD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 9;
		count = BufferIO::Read<uint32_t>(pbuf);
		for(int i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		return_value = 1;
		break;
	}
	case MSG_SELECT_TRIBUTE: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 9;
		count = BufferIO::Read<uint32_t>(pbuf);
		for (int i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			int controler = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 6;
			if(controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		return_value = 1;
		break;
	}
	case MSG_SELECT_UNSELECT_CARD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 10;
		count = BufferIO::Read<uint32_t>(pbuf);
		for(int i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		count = BufferIO::Read<uint32_t>(pbuf);
		for(int i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		return_value = 1;
		break;
	}
	case MSG_CONFIRM_CARDS: {
		player = BufferIO::Read<uint8_t>(pbuf);
		if(pbuf[5] != LOCATION_DECK) {
			SEND(nullptr);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
			packets_cache.emplace_back(TO_SEND_BUFFER);
		} else {
			SEND(cur_player[player]);
		}
		break;
	}
	case MSG_SHUFFLE_HAND:
	case MSG_SHUFFLE_EXTRA: {
		player = BufferIO::Read<uint8_t>(pbuf);
		count = BufferIO::Read<uint32_t>(pbuf);
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for(int i = 0; i < count; ++i)
			BufferIO::Write<uint32_t>(pbuf, 0);
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.emplace_back(TO_SEND_BUFFER);
		RefreshHand(player, 0x3781fff);
		break;
	}
	case MSG_MOVE: {
		pbufw = pbuf;
		pbuf += 4;
		/*CoreUtils::loc_info previous = */CoreUtils::ReadLocInfo(pbuf, false);
		CoreUtils::loc_info current = CoreUtils::ReadLocInfo(pbuf, false);
		player = current.controler;
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		if (!(current.location & (LOCATION_GRAVE + LOCATION_OVERLAY)) && ((current.location & (LOCATION_DECK + LOCATION_HAND)) || (current.position & POS_FACEDOWN)))
			BufferIO::Write<uint32_t>(pbufw, 0);
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.emplace_back(TO_SEND_BUFFER);
		break;
	}
	case MSG_SET: {
		BufferIO::Write<uint32_t>(pbuf, 0);
		SEND(nullptr);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		packets_cache.emplace_back(TO_SEND_BUFFER);
		break;
	}
	case MSG_CARD_SELECTED: {
		break;
	}
	case MSG_DRAW: {
		player = BufferIO::Read<uint8_t>(pbuf);
		count = BufferIO::Read<uint32_t>(pbuf);
		pbufw = pbuf;
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for (int i = 0; i < count; ++i) {
			/*uint32_t code = */BufferIO::Read<uint32_t>(pbufw);
			uint32_t pos = BufferIO::Read<uint32_t>(pbufw);
			if(!(pos & POS_FACEUP)) {
				pbufw -= 8;
				BufferIO::Write<uint32_t>(pbufw, 0);
				pbufw += 4;
			}
		}
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.emplace_back(TO_SEND_BUFFER);
		break;
	}
	case MSG_MISSED_EFFECT: {
		player = pbuf[0];
		SEND(cur_player[player]);
		break;
	}
	case MSG_TAG_SWAP: {
		player = BufferIO::Read<uint8_t>(pbuf);
		/*uint32_t mcount = */BufferIO::Read<uint32_t>(pbuf);
		uint32_t ecount = BufferIO::Read<uint32_t>(pbuf);
		/*uint32_t pcount = */BufferIO::Read<uint32_t>(pbuf);
		uint32_t hcount = BufferIO::Read<uint32_t>(pbuf);
		pbufw = pbuf + 4;
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for (int i = 0; i < (hcount + ecount); ++i) {
			/*int code = */BufferIO::Read<uint32_t>(pbufw);
			int pos = BufferIO::Read<uint32_t>(pbufw);
			if(!(pos & POS_FACEUP)) {
				pbufw -= 8;
				BufferIO::Write<uint32_t>(pbufw, 0);
				pbufw += 4;
			}
		}
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.emplace_back(TO_SEND_BUFFER);
		break;
	}
	case MSG_MATCH_KILL: {
		if(best_of > 1) {
			SEND(nullptr);
			ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		}
		break;
	}
	default: {
		SEND(nullptr);
		ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
		packets_cache.emplace_back(TO_SEND_BUFFER);
		break;
	}
	}
}
void GenericDuel::AfterParsing(CoreUtils::Packet& packet, int& return_value, bool& record, bool& record_last) {
	uint8_t& message = packet.message;
	int player;
	char* pbuf = DATA;
	switch(message) {
		case MSG_SHUFFLE_HAND: {
		player = BufferIO::Read<uint8_t>(pbuf);
		RefreshHand(player, 0x3781fff);
		break;
	}
	case MSG_SHUFFLE_EXTRA: {
		player = BufferIO::Read<uint8_t>(pbuf);
		RefreshExtra(player);
		break;
	}
	case MSG_SWAP_GRAVE_DECK: {
		player = BufferIO::Read<uint8_t>(pbuf);
		RefreshGrave(player);
		break;
	}
	case MSG_REVERSE_DECK: {
		PseudoRefreshDeck(0);
		PseudoRefreshDeck(1);
		break;
	}
	case MSG_SHUFFLE_SET_CARD: {
		int loc = BufferIO::Read<uint8_t>(pbuf);
		RefreshLocation(0, 0x3181fff, loc);
		RefreshLocation(1, 0x3181fff, loc);
		break;
	}
	case MSG_NEW_TURN: {
		time_limit[0] = host_info.time_limit;
		time_limit[1] = host_info.time_limit;
		turn_count++;
		break;
	}
	case MSG_DAMAGE_STEP_START:
	case MSG_DAMAGE_STEP_END:
	case MSG_SUMMONED:
	case MSG_SPSUMMONED:
	case MSG_FLIPSUMMONED:
	case MSG_NEW_PHASE:
	case MSG_CHAINED:
	case MSG_CHAIN_END: {
		if(message == MSG_CHAIN_END){
			PseudoRefreshDeck(0);
			PseudoRefreshDeck(1);
		}
		RefreshMzone(0);
		RefreshMzone(1);
		if(message != MSG_DAMAGE_STEP_START && message != MSG_DAMAGE_STEP_END) {
			RefreshSzone(0);
			RefreshSzone(1);
		}
		if(message == MSG_NEW_PHASE || message == MSG_CHAINED || message == MSG_CHAIN_END) {
			RefreshHand(0);
			RefreshHand(1);
		}
		break;
	}
	case MSG_MOVE: {
		pbuf += 4;
		CoreUtils::loc_info previous = CoreUtils::ReadLocInfo(pbuf, false);
		CoreUtils::loc_info current = CoreUtils::ReadLocInfo(pbuf, false);
		if (current.location != 0 && (current.location & 0x80) == 0 && (current.location != previous.location || previous.controler != current.controler))
			RefreshSingle(current.controler, current.location, current.sequence);
		break;
	}
	case MSG_POS_CHANGE: {
		int cc = pbuf[4];
		int cl = pbuf[5];
		int cs = pbuf[6];
		int pp = pbuf[7];
		int cp = pbuf[8];
		if((pp & POS_FACEDOWN) && (cp & POS_FACEUP))
			RefreshSingle(cc, cl, cs);
		break;
	}
	case MSG_SWAP: {
		pbuf += 4;
		CoreUtils::loc_info previous = CoreUtils::ReadLocInfo(pbuf, false);
		pbuf += 4;
		CoreUtils::loc_info current = CoreUtils::ReadLocInfo(pbuf, false);
		RefreshSingle(previous.controler, previous.location, previous.sequence);
		RefreshSingle(current.controler, current.location, current.sequence);
		break;
	}
	case MSG_TAG_SWAP: {
		player = BufferIO::Read<uint8_t>(pbuf);
		if(player == 0) {
			players.home_iterator++;
			if(players.home_iterator == players.home.end())
				players.home_iterator = players.home.begin();
			cur_player[player] = players.home_iterator->player;
		} else {
			players.opposing_iterator++;
			if(players.opposing_iterator == players.opposing.end())
				players.opposing_iterator = players.opposing.begin();
			cur_player[player] = players.opposing_iterator->player;
		}
		PseudoRefreshDeck(player);
		RefreshExtra(player);
		RefreshMzone(0, 0x3081fff);
		RefreshMzone(1, 0x3081fff);
		RefreshSzone(0, 0x30681fff);
		RefreshSzone(1, 0x30681fff);
		RefreshHand(0);
		RefreshHand(1);
		break;
	}
	case MSG_MATCH_KILL: {
		if(best_of > 1) {
			match_kill = BufferIO::Read<uint32_t>(pbuf);
		}
		break;
	}
	}
}
#undef DATA
int GenericDuel::Analyze(CoreUtils::Packet packet) {
	int return_value = 0;
	replay_stream.clear();
	bool record = true;
	bool record_last = false;
	unsigned char message = packet.message;
	ReplayPacket pk = { (char*)packet.data.data(), (int)(packet.data.size() - sizeof(uint8_t)) };
	BeforeParsing(packet, return_value, record, record_last);
	Sending(packet, return_value, record, record_last);
	AfterParsing(packet, return_value, record, record_last);
	if(record && (return_value != 1 && message != MSG_RETRY)) {
		if(!record_last) {
			new_replay.WritePacket(pk);
			new_replay.WriteStream(replay_stream);
		} else {
			new_replay.WriteStream(replay_stream);
			new_replay.WritePacket(pk);
		}
		new_replay.Flush();
	} else {
		new_replay.WriteStream(replay_stream);
		new_replay.Flush();
	}
	return return_value;
}
void GenericDuel::GetResponse(DuelPlayer* dp, void* pdata, unsigned int len) {
	last_replay.Write<uint8_t>(len);
	last_replay.WriteData(pdata, len);
	OCG_DuelSetResponse(pduel, pdata, len);
	GetAtPos(dp->type).player->state = 0xff;
	if(host_info.time_limit) {
		int resp_type = dp->type < players.home_size ? 0 : 1;
		if(time_limit[resp_type] >= time_elapsed)
			time_limit[resp_type] -= time_elapsed;
		else time_limit[resp_type] = 0;
		event_del(etimer);
	}
	Process();
}
void GenericDuel::EndDuel() {
	if(!pduel)
		return;
	last_replay.EndRecord(0x1000);
	std::vector<unsigned char> oldreplay;
	oldreplay.insert(oldreplay.end(),(unsigned char*)&last_replay.pheader, ((unsigned char*)&last_replay.pheader) + sizeof(ReplayHeader));
	oldreplay.insert(oldreplay.end(), last_replay.comp_data.begin(), last_replay.comp_data.end());

	replay_stream.push_back(ReplayPacket(OLD_REPLAY_MODE, (char*)oldreplay.data(), sizeof(ReplayHeader) + last_replay.comp_size));

	//in case of remaining packets, e.g. MSG_WIN
	new_replay.WriteStream(replay_stream);
	new_replay.EndRecord();

	std::vector<unsigned char> newreplay;
	newreplay.insert(newreplay.end(), (unsigned char*)&new_replay.pheader, ((unsigned char*)&new_replay.pheader) + sizeof(ReplayHeader));
	newreplay.insert(newreplay.end(), new_replay.comp_data.begin(), new_replay.comp_data.end());

	NetServer::SendBufferToPlayer(nullptr, STOC_NEW_REPLAY, newreplay.data(), newreplay.size());
	ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)

	NetServer::SendBufferToPlayer(nullptr, STOC_REPLAY, oldreplay.data(), oldreplay.size());
	ITERATE_PLAYERS_AND_OBS(NetServer::ReSendToPlayer(dueler);)
	OCG_DestroyDuel(pduel);
	pduel = nullptr;
}
void GenericDuel::WaitforResponse(int playerid) {
	last_response = playerid;
	unsigned char msg = MSG_WAITING;
	ITERATE_PLAYERS(
		if(dueler.player != cur_player[playerid])
			NetServer::SendPacketToPlayer(dueler.player, STOC_GAME_MSG, msg);
	)
	if(host_info.time_limit) {
		STOC_TimeLimit sctl;
		sctl.player = playerid;
		sctl.left_time = time_limit[playerid];
		NetServer::SendPacketToPlayer(nullptr, STOC_TIME_LIMIT, sctl);
		ITERATE_PLAYERS(NetServer::ReSendToPlayer(dueler.player);)
		cur_player[playerid]->state = CTOS_TIME_CONFIRM;
	} else
		cur_player[playerid]->state = CTOS_RESPONSE;
}
void GenericDuel::TimeConfirm(DuelPlayer* dp) {
	if(host_info.time_limit == 0)
		return;
	if(dp != cur_player[last_response])
		return;
	cur_player[last_response]->state = CTOS_RESPONSE;
	time_elapsed = 0;
	timeval timeout = {1, 0};
	event_add(etimer, &timeout);
}
void GenericDuel::RefreshMzone(int player, int flag) {
	RefreshLocation(player, flag, LOCATION_MZONE);
}
void GenericDuel::RefreshSzone(int player, int flag) {
	RefreshLocation(player, flag, LOCATION_SZONE);
}
void GenericDuel::RefreshHand(int player, int flag) {
	RefreshLocation(player, flag, LOCATION_HAND);
}
void GenericDuel::RefreshGrave(int player, int flag) {
	RefreshLocation(player, flag, LOCATION_GRAVE);
}
#undef TO_SEND_BUFFER
#define TO_SEND_BUFFER (char*)buffer.data(), buffer.size()
void GenericDuel::RefreshExtra(int player, int flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_EXTRA);
	uint32 len = 0;
	auto buff = OCG_DuelQueryLocation(pduel, &len, { (uint32_t)flag, (uint8_t)player, LOCATION_EXTRA });
	if(len == 0)
		return;
	buffer.resize(buffer.size() + len);
	memcpy(&buffer[3], buff, len);
	SEND(nullptr);
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
}
void GenericDuel::RefreshLocation(int player, int flag, int location) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, location);
	uint32 len = 0;
	auto buff = OCG_DuelQueryLocation(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32)location });
	if(len == 0)
		return;
	char* a = (char*)buff;
	CoreUtils::QueryStream query(a);
	query.GenerateBuffer(buffer, false);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	buffer.resize(3);
	query.GenerateBuffer(buffer, true);
	SEND(nullptr);
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	buffer.resize(3);
	query.GeneratePublicBuffer(buffer);
	SEND(nullptr);
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
}
void GenericDuel::RefreshSingle(int player, int location, int sequence, int flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_CARD);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, location);
	BufferIO::insert_value<uint8_t>(buffer, sequence);
	uint32 len = 0;
	auto buff = OCG_DuelQuery(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32_t)location, (uint32_t)sequence });
	if(buff == nullptr)
		return;
	char* a = (char*)buff;
	CoreUtils::Query query(a);
	query.GenerateBuffer(buffer, false, false);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	buffer.resize(4);
	query.GenerateBuffer(buffer, false, true);
	SEND(nullptr);
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	buffer.resize(4);
	query.GenerateBuffer(buffer, true, true);
	SEND(nullptr);
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
}
#undef SEND
#undef TO_SEND_BUFFER
void GenericDuel::PseudoRefreshDeck(int player, int flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_DECK);
	uint32 len = 0;
	auto buff = OCG_DuelQueryLocation(pduel, &len, { (uint32_t)flag, (uint8_t)player, LOCATION_DECK });
	if(len == 0)
		return;
	buffer.resize(buffer.size() + len);
	memcpy(&buffer[3], buff, len);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
}
void GenericDuel::GenericTimer(evutil_socket_t fd, short events, void* arg) {
	GenericDuel* sd = static_cast<GenericDuel*>(arg);
	sd->time_elapsed++;
	if(sd->time_elapsed >= sd->time_limit[sd->last_response]) {
		unsigned char wbuf[3];
		uint32 player = sd->last_response;
		wbuf[0] = MSG_WIN;
		wbuf[1] = 1 - player;
		wbuf[2] = 0x3;
		NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
		auto& players = sd->players;
		ITERATE_PLAYERS(NetServer::ReSendToPlayer(dueler.player);)
		ReplayPacket p((char*)wbuf, 3);
		sd->replay_stream.push_back(p);
		sd->match_result.push_back(1 - player);
		sd->EndDuel();
		sd->DuelEndProc();
		event_del(sd->etimer);
	}
}

}
