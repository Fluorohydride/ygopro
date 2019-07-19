#include "generic_duel.h"
#include "netserver.h"
#include "game.h"
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

void GenericDuel::Chat(DuelPlayer* dp, void* pdata, int len) {
	STOC_Chat scc;
	scc.player = dp->type;
	unsigned short* msg = (unsigned short*)pdata;
	int msglen = BufferIO::CopyWStr(msg, scc.msg, 256);
	ITERATE_PLAYERS(
		if(dueler.player != dp)
				NetServer::SendBufferToPlayer(dueler.player, STOC_CHAT, &scc, 4 + msglen * 2);
	)
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
			ITERATE_PLAYERS(
				NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_CHANGE, scpc);
			)
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
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
		ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_ENTER, scpe);
		)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
		SetAtPos(dp, scpe.pos);
		dp->type = scpe.pos;
		sctc.type |= scpe.pos;
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_ENTER, scwc);
		)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
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
			ITERATE_PLAYERS(
				NetServer::SendPacketToPlayer(dueler.player, STOC_HS_WATCH_CHANGE, scwc);
			)
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		if(duel_stage == DUEL_STAGE_BEGIN) {
			STOC_HS_PlayerChange scpc;
			GetAtPos(dp->type).Clear();
			scpc.status = (dp->type << 4) | PLAYERCHANGE_LEAVE;
			ITERATE_PLAYERS(
				NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_CHANGE, scpc);
			)
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
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
				ITERATE_PLAYERS(
					NetServer::ReSendToPlayer(dueler.player);
				)
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
				EndDuel();
				NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
				ITERATE_PLAYERS(
					NetServer::ReSendToPlayer(dueler.player);
				)
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
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
		ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_ENTER, scpe);
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_WATCH_CHANGE, scwc);
		)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit) {
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
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
		ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_CHANGE, scpc);
		)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
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
	ITERATE_PLAYERS(
		NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_CHANGE, scpc);
	)
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
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
				deckerror = deckManager.CheckDeck(dueler.pdeck, host_info.lflist, allow_ocg, allow_tcg, host_info.extra_rules & DOUBLE_DECK, host_info.forbiddentypes);
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
	ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_HS_PLAYER_CHANGE, scpc);
	)
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
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
	int mainc = BufferIO::ReadInt32(deckbuf);
	int sidec = BufferIO::ReadInt32(deckbuf);
	// verify data
	if((unsigned)mainc + (unsigned)sidec > (len - 8) / 4) {
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
	ITERATE_PLAYERS(
			NetServer::SendPacketToPlayer(dueler.player, STOC_DUEL_START);
	)
	observers_mutex.lock();
	for(auto oit = observers.begin(); oit != observers.end(); ++oit) {
		(*oit)->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(*oit);
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
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
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
			players.home[i].player->type = i + players.home_size;
		}
		swapped = true;
	}
	turn_count = 0;
	cur_player[0] = players.home.front().player;
	cur_player[1] = players.opposing.front().player;
	dp->state = CTOS_RESPONSE;
	ReplayHeader rh;
	rh.id = 0x31707279;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_LUA64 | REPLAY_NEWREPLAY;
	time_t seed = time(0);
	rh.seed = seed;
	last_replay.BeginRecord(false);
	last_replay.WriteHeader(rh);
	std::mt19937 rnd(seed);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.id = 0x58707279;
	new_replay.WriteHeader(rh);
	last_replay.Write<int32_t>(players.home.size(), false);
	new_replay.Write<int32_t>(players.home.size(), false);
	for(auto& dueler : players.home) {
		last_replay.WriteData(dueler.player->name, 40, false);
		new_replay.WriteData(dueler.player->name, 40, false);
	}
	last_replay.Write<int32_t>(players.opposing.size(), false);
	new_replay.Write<int32_t>(players.opposing.size(), false);
	for(auto& dueler : players.opposing) {
		last_replay.WriteData(dueler.player->name, 40, false);
		new_replay.WriteData(dueler.player->name, 40, false);
	}
	replay_stream.clear();
	time_limit[0] = host_info.time_limit;
	time_limit[1] = host_info.time_limit;
	pduel = mainGame->SetupDuel(rnd());
	if(!host_info.no_shuffle_deck) {
		ITERATE_PLAYERS(
				std::shuffle(dueler.pdeck.main.begin(), dueler.pdeck.main.end(), rnd);
		)
	}
	set_player_info(pduel, 0, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	set_player_info(pduel, 1, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	int opt = host_info.duel_flag;
	if(host_info.no_shuffle_deck)
		opt |= DUEL_PSEUDO_SHUFFLE;
	if(host_info.extra_rules & DUEL_SPEED)
		opt |= SPEED_DUEL;
	new_replay.Write<int32_t>((mainGame->GetMasterRule(opt, 0)) | (opt & SPEED_DUEL) << 8);
	last_replay.Write<int32_t>(host_info.start_lp, false);
	last_replay.Write<int32_t>(host_info.start_hand, false);
	last_replay.Write<int32_t>(host_info.draw_count, false);
	last_replay.Write<int32_t>(opt, false);
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
	for(int32 i = (int32)extracards.size() - 1; i >= 0; --i) {
		new_card(pduel, extracards[i], 0, 0, 0, 0, POS_FACEDOWN_DEFENSE, 0);
	}
	for(int32 j = 0; j < players.home.size(); j++) {
		auto& dueler = players.home[j];
		last_replay.Write<int32_t>(dueler.pdeck.main.size(), false);
		for(int32 i = (int32)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			new_card(pduel, dueler.pdeck.main[i]->code, 0, 0, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE, j);
			last_replay.Write<int32_t>(dueler.pdeck.main[i]->code, false);
		}
		last_replay.Write<int32_t>(dueler.pdeck.extra.size(), false);
		for(int32 i = (int32)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			new_card(pduel, dueler.pdeck.extra[i]->code, 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE, j);
			last_replay.Write<int32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	for(int32 j = 0; j < players.opposing.size(); j++) {
		auto& dueler = players.opposing[j];
		last_replay.Write<int32_t>(dueler.pdeck.main.size(), false);
		for(int32 i = (int32)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			new_card(pduel, dueler.pdeck.main[i]->code, 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE, j);
			last_replay.Write<int32_t>(dueler.pdeck.main[i]->code, false);
		}
		last_replay.Write<int32_t>(dueler.pdeck.extra.size(), false);
		for(int32 i = (int32)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			new_card(pduel, dueler.pdeck.extra[i]->code, 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE, j);
			last_replay.Write<int32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	last_replay.Write<int32_t>(extracards.size(), false);
	for(int32 i = (int32)extracards.size() - 1; i >= 0; --i) {
		last_replay.Write<int32_t>(extracards[i], false);
	}
	last_replay.Flush();
	char startbuf[32], *pbuf = startbuf;
	BufferIO::WriteInt8(pbuf, MSG_START);
	BufferIO::WriteInt8(pbuf, 0);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, 0x1));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, 0x40));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, 0x1));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, 0x40));
	for (auto& dueler : players.home)
		NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, startbuf, 18);
	startbuf[1] = 1;
	for(auto& dueler : players.opposing)
		NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, startbuf, 18);
	if(!swapped)
		startbuf[1] = 0x10;
	else startbuf[1] = 0x11;
	observers_mutex.lock();
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::SendBufferToPlayer(*oit, STOC_GAME_MSG, startbuf, 18);
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
	start_duel(pduel, opt);
	Process();
}
void GenericDuel::Process() {
	std::vector<uint8> duelBuffer;
	unsigned int engFlag = 0, engLen = 0;
	int stop = 0;
	while(!stop) {
		if(engFlag == 2)
			break;
		engFlag = process(pduel);
		engLen = get_message(pduel, nullptr);
		if(engLen > 0) {
			duelBuffer.resize(engLen);
			get_message(pduel, duelBuffer.data());
			observers_mutex.lock();
			stop = Analyze((char*)duelBuffer.data(), engLen);
			observers_mutex.unlock();
		}
	}
	if(stop == 2)
		DuelEndProc();
}
void GenericDuel::DuelEndProc() {
	if(!best_of || match_result.size() == best_of) {
		NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
		ITERATE_PLAYERS(
			NetServer::ReSendToPlayer(dueler.player);
		)
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
		duel_stage = DUEL_STAGE_END;
	} else {
		int winc[3] = { 0, 0, 0 };
		for(int i = 0; i < match_result.size(); ++i)
			winc[match_result[i]]++;
		int minvictories = std::ceil(best_of / 2);
		if(match_kill || (winc[0] == minvictories || (winc[1] == minvictories || winc[2] == minvictories))) {
			NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			duel_stage = DUEL_STAGE_END;
		} else {
			ITERATE_PLAYERS(
				dueler.ready = false;
				dueler.player->state = CTOS_UPDATE_DECK;
				NetServer::SendPacketToPlayer(dueler.player, STOC_CHANGE_SIDE);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::SendPacketToPlayer(*oit, STOC_WAITING_SIDE);
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
	ITERATE_PLAYERS(
	NetServer::ReSendToPlayer(dueler.player);
	)
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
	ReplayPacket p((char*)wbuf, 3);
	replay_stream.push_back(p);
	match_result.push_back(1 - player);
	EndDuel();
	DuelEndProc();
	event_del(etimer);
}
int GenericDuel::Analyze(char* msgbuffer, unsigned int len) {
	char* offset, *pbufw, *pbuf = msgbuffer;
	int player, count, type;
	while (pbuf - msgbuffer < (int)len) {
		replay_stream.clear();
		bool record = true;
		offset = pbuf;
		unsigned char message = BufferIO::ReadUInt8(pbuf);
		ReplayPacket pk(message, pbuf, len - 1);
		switch (message) {
		case MSG_RETRY: {
			WaitforResponse(last_response);
			NetServer::SendBufferToPlayer(cur_player[last_response], STOC_GAME_MSG, offset, pbuf - offset);
			replay_stream.push_back(pk);
			return 1;
		}
		case MSG_HINT: {
			type = BufferIO::ReadUInt8(pbuf);
			player = BufferIO::ReadUInt8(pbuf);
			BufferIO::ReadInt64(pbuf);
			switch (type) {
			case 1:
			case 2:
			case 3:
			case 5:
			case 10: {
				NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
				record = false;
				break;
			}
			case 4:
			case 6:
			case 7:
			case 8:
			case 9: {
				NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
				ITERATE_PLAYERS(
					if(dueler.player != cur_player[player])
						NetServer::ReSendToPlayer(dueler.player);
				)
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
				break;
			}
			}
			break;
		}
		case MSG_WIN: {
			player = BufferIO::ReadUInt8(pbuf);
			type = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			if(player > 1)
				match_result.push_back(2);
			else
				match_result.push_back(1 - player);
			replay_stream.push_back(pk);
			EndDuel();
			return 2;
		}
		case MSG_SELECT_BATTLECMD: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 18;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 8 + 2;
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_IDLECMD: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 18 + 3;
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_EFFECTYN: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 22;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 8;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 8;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CARD: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 9;
			count = BufferIO::ReadInt32(pbuf);
			for(int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				loc_info info = ClientCard::read_location_info(pbuf);
				if(info.controler != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 9;
			count = BufferIO::ReadInt32(pbuf);
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				int controler = BufferIO::ReadUInt8(pbuf);
				pbuf += 6;
				if(controler != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_UNSELECT_CARD: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 10;
			count = BufferIO::ReadInt32(pbuf);
			for(int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				loc_info info = ClientCard::read_location_info(pbuf);
				if(info.controler != player) BufferIO::WriteInt32(pbufw, 0);
			}
			count = BufferIO::ReadInt32(pbuf);
			for(int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				loc_info info = ClientCard::read_location_info(pbuf);
				if(info.controler != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += 10 + count * 23;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 9;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_SUM: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 13;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 14;
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 14;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 13;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
					NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			if(pbuf[5] != LOCATION_DECK) {
				pbuf += count * 10;
				NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
				ITERATE_PLAYERS(
					NetServer::ReSendToPlayer(dueler.player);
				)
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
				packets_cache.emplace_back(offset, pbuf - offset);
			} else {
				pbuf += count * 10;
				NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			}
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			PseudoRefreshDeck(player);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshHand(player, 0x781fff, 0);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshExtra(player);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			PseudoRefreshDeck(0);
			PseudoRefreshDeck(1);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 9;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			int loc = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 20;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			if(loc == LOCATION_MZONE) {
				RefreshMzone(0, 0x181fff, 0);
				RefreshMzone(1, 0x181fff, 0);
			}
			else {
				RefreshSzone(0, 0x181fff, 0);
				RefreshSzone(1, 0x181fff, 0);
			}
			break;
		}
		case MSG_NEW_TURN: {
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			pbuf++;
			time_limit[0] = host_info.time_limit;
			time_limit[1] = host_info.time_limit;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			turn_count++;
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			break;
		}
		case MSG_MOVE: {
			pbufw = pbuf;
			pbuf += 4;
			loc_info previous = ClientCard::read_location_info(pbuf);
			loc_info current = ClientCard::read_location_info(pbuf);
			pbuf += 4;
			player = current.controler;
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			if (!(current.location & (LOCATION_GRAVE + LOCATION_OVERLAY)) && ((current.location & (LOCATION_DECK + LOCATION_HAND)) || (current.position & POS_FACEDOWN)))
				BufferIO::WriteInt32(pbufw, 0);
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
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
			pbuf += 9;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			if((pp & POS_FACEDOWN) && (cp & POS_FACEUP))
				RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_SET: {
			BufferIO::WriteInt32(pbuf, 0);
			pbuf += 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SWAP: {
			pbuf += 4;
			loc_info previous = ClientCard::read_location_info(pbuf);
			pbuf += 4;
			loc_info current = ClientCard::read_location_info(pbuf);
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshSingle(previous.controler, previous.location, previous.sequence);
			RefreshSingle(current.controler, current.location, current.sequence);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 14;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONED: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 14;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_SPSUMMONED: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_FLIPSUMMONING: {
			BufferIO::ReadInt32(pbuf);
			loc_info info = ClientCard::read_location_info(pbuf);
			RefreshSingle(info.controler, info.location, info.sequence);
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_FLIPSUMMONED: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_CHAINING: {
			pbuf += 32;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			break;
		}
		case MSG_CHAIN_END: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			break;
		}
		case MSG_CHAIN_NEGATED: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CARD_SELECTED: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 4;
			break;
		}
		case MSG_RANDOM_SELECTED: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::ReadInt32(pbuf);
			pbuf += count * 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadInt32(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = 0; i < count; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 20;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 20;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 20;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 20;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 38;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK_DISABLED: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_MISSED_EFFECT: {
			player = pbuf[0];
			pbuf += 14;
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::ReadUInt8(pbuf);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_CARD:
		case MSG_ANNOUNCE_NUMBER: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += 8 * count;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CARD_HINT: {
			pbuf += 19;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 10;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
			ITERATE_PLAYERS(
				NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
			break;
		}
		case MSG_TAG_SWAP: {
			player = BufferIO::ReadUInt8(pbuf);
			/*int mcount = */BufferIO::ReadInt32(pbuf);
			int ecount = BufferIO::ReadInt32(pbuf);
			/*int pcount = */BufferIO::ReadInt32(pbuf);
			int hcount = BufferIO::ReadInt32(pbuf);
			pbufw = pbuf + 4;
			pbuf += hcount * 4 + ecount * 4 + 4;
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = 0; i < hcount; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			for (int i = 0; i < ecount; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			packets_cache.emplace_back(offset, pbuf - offset);
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
			RefreshMzone(0, 0x81fff, 0);
			RefreshMzone(1, 0x81fff, 0);
			RefreshSzone(0, 0x681fff, 0);
			RefreshSzone(1, 0x681fff, 0);
			RefreshHand(0, 0x781fff, 0);
			RefreshHand(1, 0x781fff, 0);
			break;
		}
		case MSG_MATCH_KILL: {
			int code = BufferIO::ReadInt32(pbuf);
			if(best_of) {
				match_kill = code;
				NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, offset, pbuf - offset);
				ITERATE_PLAYERS(
					NetServer::ReSendToPlayer(dueler.player);
				)
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
			}
		}
		}
		//setting the length again in case of multiple messages in a row,
		//when the packet will be written in the replay, the extra data registered previously will be discarded
		pk.data.resize((pbuf - offset) - 1);
		if (record)
			replay_stream.insert(replay_stream.begin(), pk);
		new_replay.WriteStream(replay_stream);
	}
	return 0;
}
void GenericDuel::GetResponse(DuelPlayer* dp, void* pdata, unsigned int len) {
	last_replay.Write<int8_t>(len);
	last_replay.WriteData(pdata, len);
	set_responseb(pduel, (byte*)pdata, len);
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
	ITERATE_PLAYERS(
		NetServer::ReSendToPlayer(dueler.player);
	)
	for (auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);


	NetServer::SendBufferToPlayer(nullptr, STOC_REPLAY, oldreplay.data(), oldreplay.size());
	ITERATE_PLAYERS(
		NetServer::ReSendToPlayer(dueler.player);
	)
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
	end_duel(pduel);
	pduel = 0;
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
		ITERATE_PLAYERS(
			NetServer::ReSendToPlayer(dueler.player);
		)
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
void GenericDuel::RefreshMzone(int player, int flag, int use_cache) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_MZONE);
	int len = query_field_card(pduel, player, LOCATION_MZONE, flag, nullptr, use_cache, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	int qlen = 0;
	char* qbuf = (char*)&buffer[3];
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen == 4)
			continue;
		if (qbuf[11] & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
}
void GenericDuel::RefreshSzone(int player, int flag, int use_cache) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_SZONE);
	int len = query_field_card(pduel, player, LOCATION_SZONE, flag, nullptr, use_cache, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	int qlen = 0;
	char* qbuf = (char*)&buffer[3];
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if(clen == 4)
			continue;
		if(qbuf[11] & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler.player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
}
void GenericDuel::RefreshHand(int player, int flag, int use_cache) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_HAND);
	int len = query_field_card(pduel, player, LOCATION_HAND, flag | QUERY_POSITION, nullptr, use_cache, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
	NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, buffer.data(), buffer.size());
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	int qlen = 0;
	char* qbuf = (char*)&buffer[3];
	while(qlen < len) {
		int slen = BufferIO::ReadInt32(qbuf);
		int qflag = *(int*)qbuf;
		int offset = 8;
		if(!(qflag & QUERY_CODE))
			offset -= 4;
		unsigned position = ((*(int*)(qbuf + offset)) >> 24) & 0xff;
		if(!(position & POS_FACEUP))
			memset(qbuf, 0, slen - 4);
		qbuf += slen - 4;
		qlen += slen;
	}
	ITERATE_PLAYERS(
		if(dueler.player != cur_player[player])
			NetServer::SendBufferToPlayer(dueler.player, STOC_GAME_MSG, buffer.data(), buffer.size());
	)
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
}
void GenericDuel::RefreshGrave(int player, int flag, int use_cache) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_GRAVE);
	int len = query_field_card(pduel, player, LOCATION_GRAVE, flag, nullptr, use_cache, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
	ITERATE_PLAYERS(
		NetServer::ReSendToPlayer(dueler.player);
	)
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back((char*)buffer.data(), buffer.size());
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
}
void GenericDuel::RefreshExtra(int player, int flag, int use_cache) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_EXTRA);
	int len = query_field_card(pduel, player, LOCATION_EXTRA, flag, nullptr, use_cache, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
	NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, buffer.data(), buffer.size());
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
}
void GenericDuel::RefreshSingle(int player, int location, int sequence, int flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_CARD);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, location);
	BufferIO::insert_value<uint8_t>(buffer, sequence);
	int len = query_card(pduel, player, location, sequence, flag, nullptr, 0, FALSE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[4]);
	replay_stream.push_back(ReplayPacket((char*)buffer.data(), buffer.size() - 1));
	char* qbuf = (char*)&buffer[4];
	if(location & LOCATION_ONFIELD) {
		NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		if(qbuf[15] & POS_FACEUP) {
			for(auto& dueler : (player == 1) ? players.home : players.opposing)
				NetServer::ReSendToPlayer(dueler.player);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::ReSendToPlayer(*pit);
			packets_cache.emplace_back((char*)buffer.data(), buffer.size());
		}
	} else {
		NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer.data(), buffer.size());
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler.player);
		if(location == LOCATION_REMOVED && (qbuf[15] & POS_FACEDOWN))
			return;
		if (location & 0x90) {
			ITERATE_PLAYERS(
				if(dueler.player != cur_player[player])
					NetServer::ReSendToPlayer(dueler.player);
			)
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::ReSendToPlayer(*pit);
			packets_cache.emplace_back((char*)buffer.data(), buffer.size());
		}
	}
}
void GenericDuel::PseudoRefreshDeck(int player, int flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_DECK);
	int len = query_field_card(pduel, player, LOCATION_DECK, flag, nullptr, 0, TRUE);
	buffer.resize(buffer.size() + len);
	get_cached_query(pduel, &buffer[3]);
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
		for(auto& dueler : sd->players.home) {
				if(dueler.player) {
					NetServer::ReSendToPlayer(dueler.player);
				}
		}
		for(auto& dueler : sd->players.opposing) {
				if(dueler.player) {
					NetServer::ReSendToPlayer(dueler.player);
				}
		}
		ReplayPacket p((char*)wbuf, 3);
		sd->replay_stream.push_back(p);
		sd->EndDuel();
		sd->DuelEndProc();
		event_del(sd->etimer);
	}
}

}
