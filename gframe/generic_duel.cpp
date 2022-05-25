#include "generic_duel.h"
#include "netserver.h"
#include "game.h"
#include "core_utils.h"

namespace ygo {

ReplayStream GenericDuel::replay_stream;

GenericDuel::GenericDuel(int team1, int team2, bool relay, int best_of) :relay(relay), best_of(best_of), match_kill(0), swapped(false), last_response(2){
	players.home.resize(team1);
	players.opposing.resize(team2);
	players.home_size = team1;
	players.opposing_size = team2;
	seeking_rematch = false;
}
GenericDuel::~GenericDuel() {
}

void GenericDuel::ResendToAll() {
	IteratePlayersAndObs(NetServer::ReSendToPlayer);
}

void GenericDuel::ResendToAll(DuelPlayer* except) {
	IteratePlayersAndObs([&except](DuelPlayer* dueler) {
		if(except != dueler)
			NetServer::ReSendToPlayer(dueler);
	});
}

void GenericDuel::Chat(DuelPlayer* dp, void* pdata, int len) {
	STOC_Chat2 scc;
	memcpy(scc.client_name, dp->name, 40);
	uint16_t* msg = (uint16_t*)pdata;
	int msglen = BufferIO::CopyStr(msg, scc.msg, 256);
	if(dp->type >= NETPLAYER_TYPE_OBSERVER) {
		scc.type = STOC_Chat2::PTYPE_OBS;
		NetServer::SendBufferToPlayer(nullptr, STOC_CHAT_2, &scc, 4 + 40 + (msglen * 2));
		ResendToAll();
		return;
	}
	scc.is_team = ((uint32_t)GetPos(dp)) < players.home_size;
	scc.type = STOC_Chat2::PTYPE_DUELIST;
	NetServer::SendBufferToPlayer(nullptr, STOC_CHAT_2, &scc, 4 + 40 + (msglen * 2));
	Iter(players.home, NetServer::ReSendToPlayer);
	scc.is_team = !scc.is_team;
	NetServer::SendBufferToPlayer(nullptr, STOC_CHAT_2, &scc, 4 + 40 + (msglen * 2));
	Iter(players.opposing, NetServer::ReSendToPlayer);
	Iter(observers, NetServer::ReSendToPlayer);
}
bool GenericDuel::CheckReady() {
	bool ready1 = true, ready2 = true;
	bool ready_atleast11 = false, ready_atleast12 = false;
	for(auto& dueler : players.home) {
		if(dueler) {
			ready1 = dueler.ready && ready1;
			ready_atleast11 = ready_atleast11 || dueler.ready;
		} else {
			ready1 = false;
		}
	}
	for(auto& dueler : players.opposing) {
		if(dueler) {
			ready2 = dueler.ready && ready2;
			ready_atleast12 = ready_atleast12 || dueler.ready;
		} else {
			ready2 = false;
		}
	}
	if(relay && match_result.empty()) {
		return ready_atleast11 && ready_atleast12;
	}
	return ready1 && ready2;
}
uint32_t GenericDuel::GetCount(const std::vector<duelist>& players) {
	uint32_t res = 0;
	for(auto& dueler : players) {
		if(dueler)
			res++;
	}
	return res;
}
bool GenericDuel::CheckFree(const std::vector<duelist>& players) {
	for(auto& dueler : players) {
		if(!dueler)
			return true;
	}
	return false;
}
int GenericDuel::GetFirstFree(int start) {
	size_t tot_size = players.home.size() + players.opposing.size();
	for(size_t i = start, j = 0; j < tot_size; i = (i+1) % tot_size, j++) {
		if(i < players.home.size()) {
			if(!players.home[i])
				return i;
		} else {
			if(!players.opposing[i - players.home.size()])
				return i;
		}
	}
	return -1;
}
int GenericDuel::GetPos(DuelPlayer* dp) {
	for(size_t i = 0; i < players.home.size(); i++) {
		if(players.home[i] == dp)
			return i;
	}
	for(size_t i = 0; i < players.opposing.size(); i++) {
		if(players.opposing[i] == dp)
			return i + players.home_size;
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
	for(size_t i = 0; i < duelists.size(); i++) {
		if(duelists[i].player->type != (i + offset)) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (uint8_t)((duelists[i].player->type << 4) | (i + offset));
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
			ResendToAll();
			STOC_TypeChange sctc;
			sctc.type = (uint8_t)((duelists[i] == host_player ? 0x10 : 0) | (i + offset));
			NetServer::SendPacketToPlayer(duelists[i], STOC_TYPE_CHANGE, sctc);
			duelists[i].player->type = (uint8_t)(i + offset);
		}
	}
}
void GenericDuel::SetAtPos(DuelPlayer* dp, size_t pos) {
	if(pos < players.home.size()) {
		players.home[pos] = dp;
	} else {
		players.opposing[pos - players.home.size()] = dp;
	}
}
GenericDuel::duelist& GenericDuel::GetAtPos(uint8_t pos) {
	if(pos < players.home_size) {
		return players.home[pos];
	} else {
		return players.opposing[pos - players.home_size];
	}
}
void GenericDuel::Catchup(DuelPlayer* dp) {
	if(!pduel) {
		observers.insert(dp);
		return;
	}
	char buf = 1;
	NetServer::SendPacketToPlayer(dp, STOC_CATCHUP, buf);
	for(size_t i = 0; i < packets_cache.size(); i++)
		NetServer::SendCoreUtilsPacketToPlayer(dp, STOC_GAME_MSG, packets_cache[i]);
	buf = 0;
	NetServer::SendPacketToPlayer(dp, STOC_CATCHUP, buf);
	observers.insert(dp);
}
void GenericDuel::JoinGame(DuelPlayer* dp, CTOS_JoinGame* pkt, bool is_creater) {
	static constexpr ClientVersion serverversion{ EXPAND_VERSION(CLIENT_VERSION) };
	if(!is_creater) {
		if(dp->game && dp->type != 0xff) {
			JoinError scem{ JoinError::JERR_UNABLE };
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
		if(pkt->version2 != serverversion) {
			VersionError scem{ serverversion };
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
		wchar_t jpass[20];
		BufferIO::DecodeUTF16(pkt->pass, jpass, 20);
		if(wcscmp(jpass, pass)) {
			JoinError scem{ JoinError::JERR_PASSWORD };
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
	}
	dp->game = this;
	if(duel_stage == DUEL_STAGE_FINGER || duel_stage == DUEL_STAGE_FIRSTGO || duel_stage == DUEL_STAGE_DUELING || duel_stage == DUEL_STAGE_SIDING || seeking_rematch) {
		STOC_JoinGame scjg;
		scjg.info = host_info;
		STOC_TypeChange sctc;
		sctc.type = NETPLAYER_TYPE_OBSERVER;
		NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		dp->state = CTOS_LEAVE_GAME;
		if(swapped)
			std::swap(players.home, players.opposing);
		IteratePlayers([this,&dp](DuelPlayer* dueler) {
			STOC_HS_PlayerEnter scpe;
			BufferIO::CopyStr(dueler->name, scpe.name, 20);
			scpe.pos = GetPos(dueler);
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		});
		if(swapped)
			std::swap(players.home, players.opposing);
		NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
		if(seeking_rematch || duel_stage == DUEL_STAGE_SIDING) {
			NetServer::SendPacketToPlayer(dp, STOC_WAITING_SIDE);
		}
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
		BufferIO::CopyStr(dp->name, scpe.name, 20);
		scpe.pos = GetFirstFree();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_ENTER, scpe);
		ResendToAll();
		SetAtPos(dp, scpe.pos);
		dp->type = scpe.pos;
		sctc.type |= scpe.pos;
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = (uint16_t)observers.size();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
		ResendToAll();
	}
	NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	IteratePlayers([this,&dp](duelist& dueler) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyStr(dueler.player->name, scpe.name, 20);
		scpe.pos = GetPos(dueler);
		NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		if(dueler.ready) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (scpe.pos << 4) | PLAYERCHANGE_READY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
		}
	});
	if(observers.size()) {
		STOC_HS_WatchChange scwc;
		scwc.watch_count = (uint16_t)observers.size();
		NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
	}
}
void GenericDuel::LeaveGame(DuelPlayer* dp) {
	auto HostLeft = [this, &dp]()->bool {
		if(dp == host_player) {
			EndDuel();
			if(duel_stage != DUEL_STAGE_BEGIN || seeking_rematch) {
				NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
				ResendToAll();
			}
			event_del(etimer);
			NetServer::StopServer();
			return true;
		}
		return false;
	};
	if(dp->type >= (players.home_size + players.opposing_size)) {
		if(HostLeft())
			return;
		NetServer::DisconnectPlayer(dp);
		if(observers.erase(dp)) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = (uint16_t)observers.size();
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
			ResendToAll();
		}
	} else {
		auto type = dp->type;
		NetServer::DisconnectPlayer(dp);
		IteratePlayers([dp](duelist& dueler) {
			if(dueler == dp)
				dueler.player = nullptr;
		});
		if(duel_stage == DUEL_STAGE_BEGIN && !seeking_rematch) {
			if(HostLeft())
				return;
			STOC_HS_PlayerChange scpc;
			GetAtPos(type).Clear();
			scpc.status = (type << 4) | PLAYERCHANGE_LEAVE;
			NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
			ResendToAll();
		} else {
			if(duel_stage == DUEL_STAGE_SIDING) {
				NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_START);
				IteratePlayers([](duelist& dueler) {
					if(!dueler.ready)
						NetServer::ReSendToPlayer(dueler);
				});
			}
			uint32_t player = type < players.home_size ? 0 : 1;
			if(duel_stage != DUEL_STAGE_END) {
				if(!seeking_rematch) {
					uint8_t wbuf[3];
					wbuf[0] = MSG_WIN;
					wbuf[1] = 1 - player;
					wbuf[2] = 0x4;
					NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
					ResendToAll();
					replay_stream.emplace_back(wbuf, 2);
					EndDuel();
				}
				NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
				ResendToAll();
				event_del(etimer);
			}
			NetServer::StopServer();
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
		BufferIO::CopyStr(dp->name, scpe.name, 20);
		dp->type = pos;
		scpe.pos = dp->type;
		SetAtPos(dp, scpe.pos);
		STOC_HS_WatchChange scwc;
		scwc.watch_count = (uint16_t)observers.size();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_ENTER, scpe);
		ResendToAll();
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_WATCH_CHANGE, scwc);
		ResendToAll();
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	} else {
		if(dp->type >= (players.home_size + players.opposing_size))
			return;
		if(GetAtPos(dp->type).ready)
			return;
		pos = GetFirstFree(dp->type);
		if(pos == -1)
			return;
		STOC_HS_PlayerChange scpc;
		scpc.status = (dp->type << 4) | pos;
		NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
		ResendToAll();
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | pos;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
		SetAtPos(dp, pos);
		GetAtPos(dp->type) = nullptr;
		dp->type = pos;
	}
}
void GenericDuel::ToObserver(DuelPlayer* dp) {
	if(dp->type >= (players.home_size + players.opposing_size))
		return;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | PLAYERCHANGE_OBSERVE;
	NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
	ResendToAll();
	GetAtPos(dp->type) = nullptr;
	dp->type = NETPLAYER_TYPE_OBSERVER;
	observers.insert(dp);
	STOC_TypeChange sctc;
	sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
}
void GenericDuel::PlayerReady(DuelPlayer* dp, bool is_ready) {
	if(dp->type >= (players.home_size + players.opposing_size))
		return;
	auto& dueler = GetAtPos(dp->type);
	if(dueler.ready == is_ready)
		return;
	if(is_ready) {
		DeckError scem{ DeckError::NONE };
		if(!host_info.no_check_deck) {
			if(dueler.deck_error) {
				scem.type = DeckError::UNKNOWNCARD;
				scem.code = dueler.deck_error;
			} else {
				scem = gdeckManager->CheckDeck(dueler.pdeck, host_info.lflist, static_cast<DuelAllowedCards>(host_info.rule), host_info.extra_rules & DOUBLE_DECK, host_info.forbiddentypes);
			}
		}
		if(scem.type) {
			STOC_HS_PlayerChange scpc;
			scpc.status = (dp->type << 4) | PLAYERCHANGE_NOTREADY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
	}
	dueler.ready = is_ready;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | (is_ready ? PLAYERCHANGE_READY : PLAYERCHANGE_NOTREADY);
	NetServer::SendPacketToPlayer(nullptr, STOC_HS_PLAYER_CHANGE, scpc);
	ResendToAll();
}
void GenericDuel::PlayerKick(DuelPlayer* dp, uint8_t pos) {
	if(pos >= (players.home_size + players.opposing_size))
		return;
	auto& dueler = GetAtPos(pos);
	if(dp != host_player || dueler == dp || !dueler)
		return;
	LeaveGame(dueler);
}
void GenericDuel::UpdateDeck(DuelPlayer* dp, void* pdata, uint32_t len) {
	if(dp->type >= (players.home_size + players.opposing_size))
		return;
	auto& dueler = GetAtPos(dp->type);
	if(dueler.ready)
		return;
	auto* deckbuf = static_cast<uint8_t*>(pdata);
	uint32_t mainc = BufferIO::Read<uint32_t>(deckbuf);
	uint32_t sidec = BufferIO::Read<uint32_t>(deckbuf);
	// verify data
	if(mainc + sidec > (len - 8) / 4) {
		DeckError scem{ DeckError::INVALIDSIZE };
		NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
		return;
	}
	if(match_result.empty()) {
		dueler.deck_error = gdeckManager->LoadDeck(dueler.pdeck, (uint32_t*)deckbuf, mainc, sidec);
		dueler.odeck = dueler.pdeck;
	} else {
		if(gdeckManager->LoadSide(dueler.pdeck, (uint32_t*)deckbuf, mainc, sidec)) {
			dueler.ready = true;
			NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
			if(CheckReady()) {
				DuelPlayer* player =  match_result[match_result.size() - 1] == 0 ? players.opposing.front() : players.home.front();
				NetServer::SendPacketToPlayer(player, STOC_SELECT_TP);
				IteratePlayers([](DuelPlayer* dueler) {
					dueler->state = 0xff;
				});
				player->state = CTOS_TP_RESULT;
				duel_stage = DUEL_STAGE_FIRSTGO;
			}
		} else {
			STOC_ErrorMsg scem{ ERROR_TYPE::SIDEERROR };
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
		}
	}
}
void GenericDuel::StartDuel(DuelPlayer* dp) {
	if(dp != host_player)
		return;
	if(!IteratePlayers([](duelist& dueler) { return dueler.ready; }))
		return;
	if(!CheckReady())
		return;
	OrderPlayers(players.home);
	OrderPlayers(players.opposing, players.home_size);
	players.home_iterator = players.home.begin();
	if(relay)
		players.opposing_iterator = players.opposing.begin();
	else
		players.opposing_iterator = players.opposing.end() - 1;
	//NetServer::StopListen();
	//NetServer::StopBroadcast();
	NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_START);
	IteratePlayers(NetServer::ReSendToPlayer);
	for(auto& obs : observers) {
		obs->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(obs);
	}
	NetServer::SendPacketToPlayer(players.home.front(), STOC_SELECT_HAND);
	NetServer::ReSendToPlayer(players.opposing.front());
	hand_result[0] = 0;
	hand_result[1] = 0;
	players.home.front().player->state = CTOS_HAND_RESULT;
	players.opposing.front().player->state = CTOS_HAND_RESULT;
	duel_stage = DUEL_STAGE_FINGER;
}
void GenericDuel::HandResult(DuelPlayer* dp, uint8_t res) {
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
		NetServer::SendPacketToPlayer(nullptr, STOC_HAND_RESULT, schr);
		for (auto& dueler : players.home)
			NetServer::ReSendToPlayer(dueler);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		schr.res1 = hand_result[1];
		schr.res2 = hand_result[0];
		NetServer::SendPacketToPlayer(nullptr, STOC_HAND_RESULT, schr);
		for(auto& dueler : players.opposing)
			NetServer::ReSendToPlayer(dueler);
		if(hand_result[0] == hand_result[1]) {
			NetServer::SendPacketToPlayer(players.home.front(), STOC_SELECT_HAND);
			NetServer::ReSendToPlayer(players.opposing.front());
			hand_result[0] = 0;
			hand_result[1] = 0;
			players.home.front().player->state = CTOS_HAND_RESULT;
			players.opposing.front().player->state = CTOS_HAND_RESULT;
		} else if((hand_result[0] == 1 && hand_result[1] == 2)
				  || (hand_result[0] == 2 && hand_result[1] == 3)
				  || (hand_result[0] == 3 && hand_result[1] == 1)) {
			NetServer::SendPacketToPlayer(players.opposing.front(), STOC_SELECT_TP);
			players.home.front().player->state = 0xff;
			players.opposing.front().player->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		} else {
			NetServer::SendPacketToPlayer(players.home.front(), STOC_SELECT_TP);
			players.opposing.front().player->state = 0xff;
			players.home.front().player->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		}
	}
}
void GenericDuel::RematchResult(DuelPlayer* dp, uint8_t rematch) {
	if(seeking_rematch) {
		if(dp->type >= (players.home_size + players.opposing_size))
			return;
		if(!rematch) {
			dp->type = NETPLAYER_TYPE_OBSERVER;
			NetServer::SendPacketToPlayer(nullptr, STOC_DUEL_END);
			ResendToAll();
			duel_stage = DUEL_STAGE_END;
			event_del(etimer);
			NetServer::StopServer();
			return;
		}
		dp->state = 0xff;
		auto& dueler = GetAtPos(dp->type);
		dueler.ready = true;
		NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
		if(CheckReady()) {
			seeking_rematch = false;
			DuelPlayer* player = match_result[match_result.size() - 1] == 0 ? players.opposing.front() : players.home.front();
			match_result.clear();
			NetServer::SendPacketToPlayer(player, STOC_SELECT_TP);
			IteratePlayers([](DuelPlayer* dueler) {
				dueler->state = 0xff;
			});
			player->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		}
	}
}
void GenericDuel::TPResult(DuelPlayer* dp, uint8_t tp) {
	if(dp->state != CTOS_TP_RESULT)
		return;
	duel_stage = DUEL_STAGE_DUELING;
	if(swapped) tp = 1 - tp;
	if((!swapped && ((tp == 1 && players.opposing.front() == dp) || (tp == 0 && players.home.front() == dp))) ||
		(swapped && ((tp == 0 && players.opposing.front() == dp) || (tp == 1 && players.home.front() == dp)))) {
		std::swap(players.opposing, players.home);
		std::swap(players.home_size, players.opposing_size);
		for(auto& val : match_result) {
			if(val < 2)
				val = 1 - val;
		}
		for(uint8_t i = 0; i < (uint8_t)players.home.size(); i++)
			players.home[i].player->type = i;
		for(uint8_t i = 0; i < (uint8_t)players.opposing.size(); i++)
			players.opposing[i].player->type = i + players.home_size;
		swapped = !swapped;
	}
	players.home_iterator = players.home.begin();
	if(relay)
		players.opposing_iterator = players.opposing.begin();
	else
		players.opposing_iterator = players.opposing.end() - 1;
	turn_count = 0;
	cur_player[0] = players.home_iterator->player;
	cur_player[1] = players.opposing_iterator->player;
	dp->state = CTOS_RESPONSE;
	auto rnd = Utils::GetRandomNumberGenerator();
	const uint32_t seed = static_cast<uint32_t>(rnd());
	ReplayHeader rh;
	rh.id = REPLAY_YRP1;
	rh.version = CLIENT_VERSION;
	rh.flag = REPLAY_LUA64 | REPLAY_NEWREPLAY | REPLAY_64BIT_DUELFLAG | REPLAY_DIRECT_SEED;
	rh.seed = seed;
	last_replay.BeginRecord(true, EPRO_TEXT("./replay/_LastReplay.yrp"));
	last_replay.WriteHeader(rh);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.seed = static_cast<uint32_t>(time(nullptr));
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
	time_limit[0] = time_limit[1] = host_info.time_limit ? (host_info.time_limit + 5) : 0;
	uint64_t opt = (((uint64_t)host_info.duel_flag_low) | ((uint64_t)host_info.duel_flag_high) << 32);
	if(host_info.no_shuffle_deck)
		opt |= ((uint64_t)DUEL_PSEUDO_SHUFFLE);
	OCG_Player team = { host_info.start_lp, host_info.start_hand, host_info.draw_count };
	pduel = mainGame->SetupDuel({ seed, opt, team, team });
	if(!host_info.no_shuffle_deck) {
		IteratePlayers([&rnd](duelist& dueler) {
			std::shuffle(dueler.pdeck.main.begin(), dueler.pdeck.main.end(), rnd);
		});
	}
	new_replay.Write<uint64_t>(opt);
	last_replay.Write<uint32_t>(host_info.start_lp, false);
	last_replay.Write<uint32_t>(host_info.start_hand, false);
	last_replay.Write<uint32_t>(host_info.draw_count, false);
	last_replay.Write<uint64_t>(opt, false);
	last_replay.Flush();
	//
	std::vector<uint32_t> extracards;
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
	if(host_info.extra_rules & DIMENSION_DUEL)
		extracards.push_back(511600002);
	if(host_info.extra_rules & TURBO_DUEL)
		extracards.push_back(110000000);
	if(host_info.extra_rules & COMMAND_DUEL)
		extracards.push_back(95200000);
	if(host_info.extra_rules & DECK_MASTER)
		extracards.push_back(300);
	if(host_info.extra_rules & DESTINY_DRAW)
		extracards.push_back(511004000);
	if(host_info.extra_rules & ACTION_DUEL)
		extracards.push_back(151999999);
	OCG_NewCardInfo card_info = { 0, 0, 0, 0, 0, 0, POS_FACEDOWN_DEFENSE };
	for(int32_t i = (int32_t)extracards.size() - 1; i >= 0; --i) {
		card_info.code = extracards[i];
		OCG_DuelNewCard(pduel, card_info);
	}
	for(uint32_t j = 0; j < (int32_t)players.home.size(); j++) {
		auto& dueler = players.home[j];
		card_info.duelist = j;
		card_info.loc = LOCATION_DECK;
		last_replay.Write<uint32_t>(dueler.pdeck.main.size(), false);
		for(int32_t i = (int32_t)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.main[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.main[i]->code, false);
		}
		card_info.loc = LOCATION_EXTRA;
		last_replay.Write<uint32_t>(dueler.pdeck.extra.size(), false);
		for(int32_t i = (int32_t)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.extra[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	card_info.team = 1;
	card_info.con = 1;
	auto idxinc = [relay=relay, size=players.opposing.size()](int i)->int {
		if(relay)
			return i;
		return (i + size - 1) % size;
	};
	for(int32_t j = 0; j < (int32_t)players.opposing.size(); j++) {
		auto& dueler = players.opposing[idxinc(j)];
		card_info.duelist = j;
		card_info.loc = LOCATION_DECK;
		last_replay.Write<uint32_t>(dueler.pdeck.main.size(), false);
		for(int32_t i = (int32_t)dueler.pdeck.main.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.main[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.main[i]->code, false);
		}
		card_info.loc = LOCATION_EXTRA;
		last_replay.Write<uint32_t>(dueler.pdeck.extra.size(), false);
		for(int32_t i = (int32_t)dueler.pdeck.extra.size() - 1; i >= 0; --i) {
			card_info.code = dueler.pdeck.extra[i]->code;
			OCG_DuelNewCard(pduel, card_info);
			last_replay.Write<uint32_t>(dueler.pdeck.extra[i]->code, false);
		}
	}
	last_replay.Write<uint32_t>(extracards.size(), false);
	for(int32_t i = (int32_t)extracards.size() - 1; i >= 0; --i) {
		last_replay.Write<uint32_t>(extracards[i], false);
	}
	last_replay.Flush();
	uint8_t startbuf[32];
	auto* pbuf = startbuf;
	BufferIO::Write<uint8_t>(pbuf, MSG_START);
	BufferIO::Write<uint8_t>(pbuf, 0);
	BufferIO::Write<uint32_t>(pbuf, host_info.start_lp);
	BufferIO::Write<uint32_t>(pbuf, host_info.start_lp);
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 0, LOCATION_DECK));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 0, LOCATION_EXTRA));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 1, LOCATION_DECK));
	BufferIO::Write<uint16_t>(pbuf, OCG_DuelQueryCount(pduel, 1, LOCATION_EXTRA));
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, startbuf, 18);
	for(auto& dueler : players.home)
		NetServer::ReSendToPlayer(dueler);
	startbuf[1] = 1;
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, startbuf, 18);
	for(auto& dueler : players.opposing)
		NetServer::ReSendToPlayer(dueler);
	if(!swapped)
		startbuf[1] = 0x10;
	else startbuf[1] = 0x11;
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, startbuf, 18);
	for(auto& obs : observers)
		NetServer::ReSendToPlayer(obs);
	packets_cache.emplace_back(startbuf, 18);
	startbuf[1] = 0;
	replay_stream.emplace_back(startbuf, 17);
	PseudoRefreshDeck(0);
	PseudoRefreshDeck(1);
	RefreshExtra(0);
	RefreshExtra(1);
	new_replay.WriteStream(replay_stream);
	OCG_StartDuel(pduel);
	Process();
}
void GenericDuel::Process() {
	std::vector<uint8_t> duelBuffer;
	uint32_t engFlag = 0;
	int stop = 0;
	do {
		engFlag = OCG_DuelProcess(pduel);
		for(auto& message : CoreUtils::ParseMessages(pduel)) {
			stop = Analyze(std::move(message));
			if(stop)
				break;
		}
	} while(!stop);
	if(engFlag == OCG_DUEL_STATUS_END || stop == 2)
		DuelEndProc();
}
void GenericDuel::DuelEndProc() {
	EndDuel();
	packets_cache.clear();
	int winc[3] = { 0, 0, 0 };
	for(size_t i = 0; i < match_result.size(); ++i)
		winc[match_result[i]]++;
	int minvictories = (int)std::ceil(best_of / 2.0);
	if(match_kill || (winc[0] >= minvictories || winc[1] >= minvictories) || (int)match_result.size() >= best_of) {
		seeking_rematch = true;
		NetServer::SendPacketToPlayer(nullptr, STOC_WAITING_REMATCH);
		ResendToAll();
		NetServer::SendPacketToPlayer(nullptr, STOC_REMATCH);
		IteratePlayers([](duelist& dueler) {
			dueler.pdeck = dueler.odeck;
			dueler.player->state = CTOS_REMATCH_RESPONSE;
			dueler.ready = false;
			NetServer::ReSendToPlayer(dueler);
		});
		duel_stage = DUEL_STAGE_BEGIN;
	} else {
		NetServer::SendPacketToPlayer(nullptr, STOC_CHANGE_SIDE);
		IteratePlayers([](duelist& dueler) {
			dueler.ready = false;
			dueler.player->state = CTOS_UPDATE_DECK;
			NetServer::ReSendToPlayer(dueler);
		});
		NetServer::SendPacketToPlayer(nullptr, STOC_WAITING_SIDE);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		duel_stage = DUEL_STAGE_SIDING;
	}
}
void GenericDuel::Surrender(DuelPlayer* dp) {
	/*if((players.home.size() + players.opposing.size()) != 2 || (players.home.front() != dp && players.opposing.front() != dp) || !pduel)
		return;*/
	if(pduel) {
		uint8_t wbuf[3];
		uint32_t player = dp->type < players.home_size ? 0 : 1;
		wbuf[0] = MSG_WIN;
		wbuf[1] = 1 - player;
		wbuf[2] = 0;
		NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
		ResendToAll();
		replay_stream.emplace_back(wbuf, 2);
		match_result.push_back(1 - player);
		DuelEndProc();
		event_del(etimer);
	}
}
#define SEND(to) NetServer::SendCoreUtilsPacketToPlayer(to, STOC_GAME_MSG, packet)
void GenericDuel::BeforeParsing(const CoreUtils::Packet& packet, int& return_value, bool& record, bool& record_last) {
	const auto* pbuf = packet.data();
	switch(packet.message) {
	case MSG_SELECT_BATTLECMD:
	case MSG_SELECT_IDLECMD: {
		RefreshMzone(0);
		RefreshMzone(1);
		RefreshSzone(0);
		RefreshSzone(1);
		RefreshHand(0);
		RefreshHand(1);
		record_last = true;
		break;
	}
	case MSG_SELECT_CHAIN:
	case MSG_NEW_TURN: {
		RefreshMzone(0);
		RefreshMzone(1);
		RefreshSzone(0);
		RefreshSzone(1);
		record_last = true;
		break;
	}
	case MSG_FLIPSUMMONING: {
		pbuf += 4;
		CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
		RefreshSingle(info.controler, info.location, info.sequence);
		record_last = true;
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
	uint8_t* pbufw;
	auto* pbuf = packet.data();
	switch (message) {
	case MSG_RETRY: {
		SEND(nullptr);
		ResendToAll();
		match_result.push_back(2);
		return_value = 2;
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
		case 9:
		case 11: {
			SEND(nullptr);
			ResendToAll(cur_player[player]);
			break;
		}
		case 10:
		case 201:
		case 202:
		case 203: {
			SEND(nullptr);
			ResendToAll();
			break;
		}
		case 200: {
			SEND(nullptr);
			for(auto& dueler : (player == 0) ? players.home : players.opposing)
				NetServer::ReSendToPlayer(dueler);
		}
		}
		break;
	}
	case MSG_SELECT_BATTLECMD:
	case MSG_SELECT_IDLECMD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		WaitforResponse(player);
		SEND(cur_player[player]);
		record = false;
		return_value = 1;
		break;
	}
	case MSG_WIN: {
		player = BufferIO::Read<uint8_t>(pbuf);
		type = BufferIO::Read<uint8_t>(pbuf);
		SEND(nullptr);
		ResendToAll();
		if(player > 1)
			match_result.push_back(2);
		else
			match_result.push_back(player);
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
		record = false;
		return_value = 1;
		break;
	}
	case MSG_SELECT_CARD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 9;
		count = BufferIO::Read<uint32_t>(pbuf);
		for(uint32_t i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		record = false;
		return_value = 1;
		break;
	}
	case MSG_SELECT_TRIBUTE: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 9;
		count = BufferIO::Read<uint32_t>(pbuf);
		for(uint32_t i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			int controler = BufferIO::Read<uint8_t>(pbuf);
			pbuf += 6;
			if(controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		record = false;
		return_value = 1;
		break;
	}
	case MSG_SELECT_UNSELECT_CARD: {
		player = BufferIO::Read<uint8_t>(pbuf);
		pbuf += 10;
		count = BufferIO::Read<uint32_t>(pbuf);
		for(uint32_t i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		count = BufferIO::Read<uint32_t>(pbuf);
		for(uint32_t i = 0; i < count; ++i) {
			pbufw = pbuf;
			/*code = */BufferIO::Read<uint32_t>(pbuf);
			CoreUtils::loc_info info = CoreUtils::ReadLocInfo(pbuf, false);
			if(info.controler != player)
				BufferIO::Write<uint32_t>(pbufw, 0);
		}
		WaitforResponse(player);
		SEND(cur_player[player]);
		record = false;
		return_value = 1;
		break;
	}
	case MSG_CONFIRM_CARDS: {
		player = BufferIO::Read<uint8_t>(pbuf);
		uint32_t count = BufferIO::Read<uint32_t>(pbuf);
		if(count > 0) {
			/*uint32_t code = */BufferIO::Read<uint32_t>(pbuf);
			/*uint32_t controler = */BufferIO::Read<uint8_t>(pbuf);
			uint8_t location = BufferIO::Read<uint8_t>(pbuf);
			if(location != LOCATION_DECK) {
				SEND(nullptr);
				ResendToAll();
				packets_cache.push_back(packet);
			} else {
				SEND(cur_player[player]);
			}
		} else {
			SEND(nullptr);
			ResendToAll();
			packets_cache.push_back(packet);
		}
		break;
	}
	case MSG_SHUFFLE_HAND:
	case MSG_SHUFFLE_EXTRA: {
		player = BufferIO::Read<uint8_t>(pbuf);
		count = BufferIO::Read<uint32_t>(pbuf);
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler);
		for(uint32_t i = 0; i < count; ++i)
			BufferIO::Write<uint32_t>(pbuf, 0);
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.push_back(packet);
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
			NetServer::ReSendToPlayer(dueler);
		if (!(current.location & (LOCATION_GRAVE + LOCATION_OVERLAY)) && ((current.location & (LOCATION_DECK + LOCATION_HAND)) || (current.position & POS_FACEDOWN)))
			BufferIO::Write<uint32_t>(pbufw, 0);
		SEND(nullptr);
		for(auto& dueler : (player == 1) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.push_back(packet);
		break;
	}
	case MSG_SET: {
		BufferIO::Write<uint32_t>(pbuf, 0);
		SEND(nullptr);
		ResendToAll();
		packets_cache.push_back(packet);
		break;
	}
	case MSG_DRAW: {
		player = BufferIO::Read<uint8_t>(pbuf);
		count = BufferIO::Read<uint32_t>(pbuf);
		pbufw = pbuf;
		SEND(nullptr);
		for(auto& dueler : (player == 0) ? players.home : players.opposing)
			NetServer::ReSendToPlayer(dueler);
		for(uint32_t i = 0; i < count; ++i) {
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
			NetServer::ReSendToPlayer(dueler);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.push_back(packet);
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
			NetServer::ReSendToPlayer(dueler);
		for (uint32_t i = 0; i < (hcount + ecount); ++i) {
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
			NetServer::ReSendToPlayer(dueler);
		for(auto& obs : observers)
			NetServer::ReSendToPlayer(obs);
		packets_cache.push_back(packet);
		break;
	}
	case MSG_MATCH_KILL: {
		if(best_of > 1) {
			SEND(nullptr);
			ResendToAll();
		}
		break;
	}
	default: {
		SEND(nullptr);
		ResendToAll();
		packets_cache.push_back(packet);
		break;
	}
	}
}
#undef SEND

void GenericDuel::AfterParsing(const CoreUtils::Packet& packet, int& return_value, bool& record, bool& record_last) {
	const auto message = packet.message;
	int player;
	const auto* pbuf = packet.data();
	switch(message) {
	case MSG_SHUFFLE_HAND:
	case MSG_DRAW: {
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
		time_limit[0] = time_limit[1] = host_info.time_limit ? (host_info.time_limit + 5) : 0;
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
	case MSG_CHAIN_SOLVED:
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
	case MSG_RELOAD_FIELD: {
		RefreshExtra(0);
		RefreshExtra(1);
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
	auto packetcpy = packet;
	BeforeParsing(packet, return_value, record, record_last);
	Sending(packet, return_value, record, record_last);
	AfterParsing(packet, return_value, record, record_last);
	if(record)
		replay_stream.insert(record_last ? replay_stream.end() : replay_stream.begin(), std::move(packetcpy));
	new_replay.WriteStream(replay_stream);
	new_replay.Flush();
	return return_value;
}
void GenericDuel::GetResponse(DuelPlayer* dp, void* pdata, uint32_t len) {
	last_replay.Write<uint8_t>(len, false);
	last_replay.WriteData(pdata, len);
	OCG_DuelSetResponse(pduel, pdata, len);
	GetAtPos(dp->type).player->state = 0xff;
	/*if(host_info.time_limit) {
		int resp_type = dp->type < players.home_size ? 0 : 1;
		if(time_limit[resp_type] >= grace_period)
			time_limit[resp_type] -= grace_period;
		else time_limit[resp_type] = 0;
		event_del(etimer);
	}*/
	Process();
}
void GenericDuel::EndDuel() {
	if(!pduel)
		return;
	last_replay.EndRecord(0x1000);

	//in case of remaining packets, e.g. MSG_WIN
	auto oldbuffer = last_replay.GetSerializedBuffer();
	
	{
		replay_stream.emplace_back();
		auto& packet = replay_stream.back();
		packet.message = OLD_REPLAY_MODE;
		packet.buffer.swap(oldbuffer);
	}

	new_replay.WriteStream(replay_stream);
	new_replay.EndRecord();

	auto newbuffer = new_replay.GetSerializedBuffer();
	
	NetServer::SendBufferToPlayer(nullptr, STOC_NEW_REPLAY, newbuffer.data(), newbuffer.size());
	ResendToAll();

	NetServer::SendPacketToPlayer(nullptr, STOC_REPLAY);
	ResendToAll();
	OCG_DestroyDuel(pduel);
	pduel = nullptr;
}
void GenericDuel::WaitforResponse(uint8_t playerid) {
	last_response = playerid;
	static constexpr uint8_t msg = MSG_WAITING;
	NetServer::SendPacketToPlayer(nullptr, STOC_GAME_MSG, msg);
	IteratePlayers([&player=cur_player[playerid]](DuelPlayer* dueler) {
		if(dueler != player)
			NetServer::ReSendToPlayer(dueler);
	});
	if(host_info.time_limit) {
		STOC_TimeLimit sctl;
		sctl.player = playerid;
		sctl.left_time = std::max<int32_t>(time_limit[playerid] - 5, 0);
		NetServer::SendPacketToPlayer(nullptr, STOC_TIME_LIMIT, sctl);
		IteratePlayers(NetServer::ReSendToPlayer);
		grace_period = 0;
	}
	cur_player[playerid]->state = CTOS_RESPONSE;
}
void GenericDuel::TimeConfirm(DuelPlayer* dp) {
	return;
	/*if(host_info.time_limit == 0)
		return;
	if(dp != cur_player[last_response])
		return;
	cur_player[last_response]->state = CTOS_RESPONSE;
	grace_period = 0;
	timeval timeout = {1, 0};
	event_add(etimer, &timeout);*/
}
void GenericDuel::RefreshMzone(uint8_t player, uint32_t flag) {
	RefreshLocation(player, flag, LOCATION_MZONE);
}
void GenericDuel::RefreshSzone(uint8_t player, uint32_t flag) {
	RefreshLocation(player, flag, LOCATION_SZONE);
}
void GenericDuel::RefreshHand(uint8_t player, uint32_t flag) {
	RefreshLocation(player, flag, LOCATION_HAND);
}
void GenericDuel::RefreshGrave(uint8_t player, uint32_t flag) {
	RefreshLocation(player, flag, LOCATION_GRAVE);
}
void GenericDuel::RefreshExtra(uint8_t player, uint32_t flag) {
	RefreshLocation(player, flag, LOCATION_EXTRA);
}
void GenericDuel::RefreshLocation(uint8_t player, uint32_t flag, uint8_t location) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, location);
	uint32_t len = 0;
	auto* buff = static_cast<uint8_t*>(OCG_DuelQueryLocation(pduel, &len, { flag, player, location }));
	if(len == 0)
		return;
	CoreUtils::QueryStream query(buff);
	query.GenerateBuffer(buffer, false);
	replay_stream.emplace_back(buffer.data(), buffer.size() - 1);
	buffer.resize(3);
	query.GenerateBuffer(buffer, true);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer);
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler);
	buffer.resize(3);
	query.GeneratePublicBuffer(buffer);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer);
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back(buffer.data(), buffer.size());
}
void GenericDuel::RefreshSingle(uint8_t player, uint8_t location, uint8_t sequence, uint32_t flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_CARD);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, location);
	BufferIO::insert_value<uint8_t>(buffer, sequence);
	uint32_t len = 0;
	auto* buff = static_cast<uint8_t*>(OCG_DuelQuery(pduel, &len, { flag, player, location, sequence }));
	if(buff == nullptr)
		return;
	CoreUtils::Query query(buff);
	query.GenerateBuffer(buffer, false, false);
	replay_stream.emplace_back(buffer.data(), buffer.size() - 1);
	buffer.resize(4);
	query.GenerateBuffer(buffer, false, true);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer);
	for(auto& dueler : (player == 0) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler);
	buffer.resize(4);
	query.GenerateBuffer(buffer, true, true);
	NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, buffer);
	for(auto& dueler : (player == 1) ? players.home : players.opposing)
		NetServer::ReSendToPlayer(dueler);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	packets_cache.emplace_back(buffer.data(), buffer.size());
}
void GenericDuel::PseudoRefreshDeck(uint8_t player, uint32_t flag) {
	std::vector<uint8_t> buffer;
	BufferIO::insert_value<uint8_t>(buffer, MSG_UPDATE_DATA);
	BufferIO::insert_value<uint8_t>(buffer, player);
	BufferIO::insert_value<uint8_t>(buffer, LOCATION_DECK);
	uint32_t len = 0;
	auto buff = OCG_DuelQueryLocation(pduel, &len, { flag, player, LOCATION_DECK });
	if(len == 0)
		return;
	buffer.resize(buffer.size() + len);
	memcpy(&buffer[3], buff, len);
	replay_stream.emplace_back(buffer.data(), buffer.size() - 1);
}
void GenericDuel::GenericTimer(evutil_socket_t fd, short events, void* arg) {
	GenericDuel* sd = static_cast<GenericDuel*>(arg);
	if(sd->last_response < 2 && sd->cur_player[sd->last_response]->state == CTOS_RESPONSE) {
		if(sd->grace_period >= 0) {
			sd->grace_period--;
			return;
		}
		sd->time_limit[sd->last_response]--;
		if(sd->time_limit[sd->last_response] <= 0) {
			uint8_t wbuf[3];
			uint8_t player = sd->last_response;
			wbuf[0] = MSG_WIN;
			wbuf[1] = 1 - player;
			wbuf[2] = 0x3;
			NetServer::SendBufferToPlayer(nullptr, STOC_GAME_MSG, wbuf, 3);
			sd->IteratePlayers(NetServer::ReSendToPlayer);
			sd->replay_stream.emplace_back(wbuf, 2);
			sd->match_result.push_back(1 - player);
			sd->DuelEndProc();
		}
	}
}

}
