#include "single_duel.h"
#include "netserver.h"
#include "game.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/common.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

#ifdef YGOPRO_SERVER_MODE
extern unsigned short replay_mode;
#endif
SingleDuel::SingleDuel(bool is_match) {
	match_mode = is_match;
#ifdef YGOPRO_SERVER_MODE
	cache_recorder = 0;
	replay_recorder = 0;
#endif
}
SingleDuel::~SingleDuel() {
}
void SingleDuel::Chat(DuelPlayer* dp, unsigned char* pdata, int len) {
	unsigned char scc[SIZE_STOC_CHAT];
	const auto scc_size = NetServer::CreateChatPacket(pdata, len, scc, dp->type);
	if (!scc_size)
		return;
	NetServer::SendBufferToPlayer(players[0], STOC_CHAT, scc, scc_size);
	NetServer::ReSendToPlayer(players[1]);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder)
		NetServer::ReSendToPlayer(cache_recorder);
	if(replay_recorder && replay_mode & REPLAY_MODE_INCLUDE_CHAT)
		NetServer::ReSendToPlayer(replay_recorder);
#endif
}
void SingleDuel::JoinGame(DuelPlayer* dp, unsigned char* pdata, bool is_creater) {
#ifdef YGOPRO_SERVER_MODE
	bool is_recorder = false;
#endif
	if(!is_creater) {
		if(dp->game && dp->type != 0xff) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 0;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			NetServer::DisconnectPlayer(dp);
			return;
		}
		CTOS_JoinGame packet;
		std::memcpy(&packet, pdata, sizeof packet);
		const auto* pkt = &packet;
		if(pkt->version != PRO_VERSION) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_VERERROR;
			scem.code = PRO_VERSION;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			NetServer::DisconnectPlayer(dp);
			return;
		}
		wchar_t jpass[20];
		BufferIO::CopyWStr(pkt->pass, jpass, 20);
#ifdef YGOPRO_SERVER_MODE
		if(!wcscmp(jpass, L"the Big Brother") && !cache_recorder) {
			is_recorder = true;
			cache_recorder = dp;
		}
#ifndef YGOPRO_SERVER_MODE_DISABLE_CLOUD_REPLAY
		if(!wcscmp(jpass, L"Marshtomp") && !replay_recorder) {
			is_recorder = true;
			replay_recorder = dp;
		}
#endif //YGOPRO_SERVER_MODE_DISABLE_CLOUD_REPLAY
#else
		if(wcscmp(jpass, pass)) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 1;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
#endif //YGOPRO_SERVER_MODE
	}
	dp->game = this;
	if(!players[0] && !players[1] && observers.size() == 0)
		host_player = dp;
	STOC_JoinGame scjg;
	scjg.info = host_info;
	STOC_TypeChange sctc;
	sctc.type = (host_player == dp) ? 0x10 : 0;
#ifdef YGOPRO_SERVER_MODE
	if(is_recorder) {
		dp->type = 9;
		sctc.type = NETPLAYER_TYPE_OBSERVER;
	}
	else
#endif
	if(!players[0] || !players[1]) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		if(!players[0])
			scpe.pos = 0;
		else
			scpe.pos = 1;
		if(players[0]) {
			NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_ENTER, scpe);
		}
		if(players[1]) {
			NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_ENTER, scpe);
		}
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
#ifdef YGOPRO_SERVER_MODE
		if(cache_recorder)
			NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_PLAYER_ENTER, scpe);
		if(replay_recorder)
			NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_PLAYER_ENTER, scpe);
#endif
		if(!players[0]) {
			players[0] = dp;
			dp->type = NETPLAYER_TYPE_PLAYER1;
			sctc.type |= NETPLAYER_TYPE_PLAYER1;
		} else {
			players[1] = dp;
			dp->type = NETPLAYER_TYPE_PLAYER2;
			sctc.type |= NETPLAYER_TYPE_PLAYER2;
		}
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = (unsigned short)observers.size();
		if(players[0])
			NetServer::SendPacketToPlayer(players[0], STOC_HS_WATCH_CHANGE, scwc);
		if(players[1])
			NetServer::SendPacketToPlayer(players[1], STOC_HS_WATCH_CHANGE, scwc);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
#ifdef YGOPRO_SERVER_MODE
		if(cache_recorder)
			NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_WATCH_CHANGE, scwc);
		if(replay_recorder)
			NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_WATCH_CHANGE, scwc);
#endif
	}
	NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	if(players[0]) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(players[0]->name, scpe.name, 20);
		scpe.pos = 0;
		NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		if(ready[0]) {
			STOC_HS_PlayerChange scpc;
			scpc.status = PLAYERCHANGE_READY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
		}
	}
	if(players[1]) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(players[1]->name, scpe.name, 20);
		scpe.pos = 1;
		NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
		if(ready[1]) {
			STOC_HS_PlayerChange scpc;
			scpc.status = 0x10 | PLAYERCHANGE_READY;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
		}
	}
	if(observers.size()) {
		STOC_HS_WatchChange scwc;
		scwc.watch_count = (unsigned short)observers.size();
		NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
	}
}
void SingleDuel::LeaveGame(DuelPlayer* dp) {
	if(dp == host_player) {
#ifdef YGOPRO_SERVER_MODE
		int host_pos;
		if(players[0] && dp->type != 0) {
			host_pos = 0;
			host_player = players[0];
		} else if(players[1] && dp->type != 1) {
			host_pos = 1;
			host_player = players[1];
		} else {
			EndDuel();
			NetServer::StopServer();
			return;
		}
		if(duel_stage == DUEL_STAGE_BEGIN) {
			ready[host_pos] = false;
			STOC_TypeChange sctc;
			sctc.type = 0x10 | host_pos;
			NetServer::SendPacketToPlayer(players[host_pos], STOC_TYPE_CHANGE, sctc);
		}
	}
	if(dp->type == NETPLAYER_TYPE_OBSERVER) {
#else
		EndDuel();
		NetServer::StopServer();
	} else if(dp->type == NETPLAYER_TYPE_OBSERVER) {
#endif //YGOPRO_SERVER_MODE
		observers.erase(dp);
		if(duel_stage == DUEL_STAGE_BEGIN) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = (unsigned short)observers.size();
			if(players[0])
				NetServer::SendPacketToPlayer(players[0], STOC_HS_WATCH_CHANGE, scwc);
			if(players[1])
				NetServer::SendPacketToPlayer(players[1], STOC_HS_WATCH_CHANGE, scwc);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
#ifdef YGOPRO_SERVER_MODE
			if(cache_recorder)
				NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_WATCH_CHANGE, scwc);
			if(replay_recorder)
				NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_WATCH_CHANGE, scwc);
#endif
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		if(duel_stage == DUEL_STAGE_BEGIN) {
			STOC_HS_PlayerChange scpc;
			players[dp->type] = 0;
			ready[dp->type] = false;
			scpc.status = (dp->type << 4) | PLAYERCHANGE_LEAVE;
			if(players[0] && dp->type != 0)
				NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_CHANGE, scpc);
			if(players[1] && dp->type != 1)
				NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_CHANGE, scpc);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
#ifdef YGOPRO_SERVER_MODE
			if(cache_recorder)
				NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_PLAYER_CHANGE, scpc);
			if(replay_recorder)
				NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_PLAYER_CHANGE, scpc);
#endif
			NetServer::DisconnectPlayer(dp);
		} else {
			if(duel_stage == DUEL_STAGE_SIDING) {
				if(!ready[0])
					NetServer::SendPacketToPlayer(players[0], STOC_DUEL_START);
				if(!ready[1])
					NetServer::SendPacketToPlayer(players[1], STOC_DUEL_START);
			}
			if(duel_stage != DUEL_STAGE_END) {
				unsigned char wbuf[3];
				wbuf[0] = MSG_WIN;
				wbuf[1] = 1 - dp->type;
				wbuf[2] = 0x4;
				NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, wbuf, 3);
				NetServer::ReSendToPlayer(players[1]);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
				EndDuel();
				NetServer::SendPacketToPlayer(players[0], STOC_DUEL_END);
				NetServer::ReSendToPlayer(players[1]);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
				NetServer::StopServer();
#endif
			}
#ifndef YGOPRO_SERVER_MODE
			NetServer::DisconnectPlayer(dp);
#endif
		}
	}
}
void SingleDuel::ToDuelist(DuelPlayer* dp) {
	if(dp->type != NETPLAYER_TYPE_OBSERVER)
		return;
	if(players[0] && players[1])
		return;
	observers.erase(dp);
	STOC_HS_PlayerEnter scpe;
	BufferIO::CopyWStr(dp->name, scpe.name, 20);
	if(!players[0]) {
		players[0] = dp;
		dp->type = NETPLAYER_TYPE_PLAYER1;
		scpe.pos = 0;
	} else {
		players[1] = dp;
		dp->type = NETPLAYER_TYPE_PLAYER2;
		scpe.pos = 1;
	}
	STOC_HS_WatchChange scwc;
	scwc.watch_count = (unsigned short)observers.size();
	NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_ENTER, scpe);
	NetServer::SendPacketToPlayer(players[0], STOC_HS_WATCH_CHANGE, scwc);
	if(players[1]) {
		NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_ENTER, scpe);
		NetServer::SendPacketToPlayer(players[1], STOC_HS_WATCH_CHANGE, scwc);
	}
	for(auto pit = observers.begin(); pit != observers.end(); ++pit) {
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
		NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
	}
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder) {
		NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_PLAYER_ENTER, scpe);
		NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_WATCH_CHANGE, scwc);
	}
	if(replay_recorder) {
		NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_PLAYER_ENTER, scpe);
		NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_WATCH_CHANGE, scwc);
	}
#endif
	STOC_TypeChange sctc;
	sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
}
void SingleDuel::ToObserver(DuelPlayer* dp) {
	if(dp->type > 1)
		return;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | PLAYERCHANGE_OBSERVE;
	if(players[0])
		NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_CHANGE, scpc);
	if(players[1])
		NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_CHANGE, scpc);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder)
		NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_PLAYER_CHANGE, scpc);
	if(replay_recorder)
		NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_PLAYER_CHANGE, scpc);
#endif
	players[dp->type] = 0;
	ready[dp->type] = false;
	dp->type = NETPLAYER_TYPE_OBSERVER;
	observers.insert(dp);
	STOC_TypeChange sctc;
	sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
}
void SingleDuel::PlayerReady(DuelPlayer* dp, bool is_ready) {
	if(dp->type > 1)
		return;
	if(ready[dp->type] == is_ready)
		return;
	if(is_ready) {
		unsigned int deckerror = 0;
		if(!host_info.no_check_deck) {
			if(deck_error[dp->type]) {
				deckerror = (DECKERROR_UNKNOWNCARD << 28) + deck_error[dp->type];
			} else {
				deckerror = deckManager.CheckDeck(pdeck[dp->type], host_info.lflist, host_info.rule);
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
	ready[dp->type] = is_ready;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | (is_ready ? PLAYERCHANGE_READY : PLAYERCHANGE_NOTREADY);
	NetServer::SendPacketToPlayer(players[dp->type], STOC_HS_PLAYER_CHANGE, scpc);
	if(players[1 - dp->type])
		NetServer::SendPacketToPlayer(players[1 - dp->type], STOC_HS_PLAYER_CHANGE, scpc);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder)
		NetServer::SendPacketToPlayer(cache_recorder, STOC_HS_PLAYER_CHANGE, scpc);
	if(replay_recorder)
		NetServer::SendPacketToPlayer(replay_recorder, STOC_HS_PLAYER_CHANGE, scpc);
#endif
}
void SingleDuel::PlayerKick(DuelPlayer* dp, unsigned char pos) {
	if(pos > 1 || dp != host_player || dp == players[pos] || !players[pos])
		return;
	LeaveGame(players[pos]);
}
void SingleDuel::UpdateDeck(DuelPlayer* dp, unsigned char* pdata, int len) {
	if(dp->type > 1 || ready[dp->type])
		return;
	if (len < 8)
		return;
	bool valid = true;
	auto deckbuf = pdata;
	int mainc = BufferIO::ReadInt32(deckbuf);
	int sidec = BufferIO::ReadInt32(deckbuf);
	const int deck_size = len - 2 * sizeof(int32_t);
	if (mainc < 0 || mainc > MAINC_MAX)
		valid = false;
	else if (sidec < 0 || sidec > SIDEC_MAX)
		valid = false;
	else if (deck_size != (mainc + sidec) * (int)sizeof(int32_t))
		valid = false;
	if (!valid) {
		STOC_ErrorMsg scem;
		scem.msg = ERRMSG_DECKERROR;
		scem.code = 0;
		NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
		return;
	}
	int deck_list[SIZE_NETWORK_BUFFER / sizeof(int32_t)];
	std::memcpy(deck_list, deckbuf, deck_size);
	if(duel_count == 0) {
		deck_error[dp->type] = deckManager.LoadDeck(pdeck[dp->type], deck_list, mainc, sidec);
	} else {
		if(deckManager.LoadSide(pdeck[dp->type], deck_list, mainc, sidec)) {
			ready[dp->type] = true;
			NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
			if(ready[0] && ready[1]) {
				NetServer::SendPacketToPlayer(players[tp_player], STOC_SELECT_TP);
				players[1 - tp_player]->state = 0xff;
				players[tp_player]->state = CTOS_TP_RESULT;
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
void SingleDuel::StartDuel(DuelPlayer* dp) {
	if(dp != host_player)
		return;
	if(!ready[0] || !ready[1])
		return;
	NetServer::StopListen();
	//NetServer::StopBroadcast();
	NetServer::SendPacketToPlayer(players[0], STOC_DUEL_START);
	NetServer::ReSendToPlayer(players[1]);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit) {
		(*oit)->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(*oit);
	}
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder)
		cache_recorder->state = CTOS_LEAVE_GAME;
	if(replay_recorder)
		replay_recorder->state = CTOS_LEAVE_GAME;
	NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
	unsigned char deckbuff[12];
	auto pbuf = deckbuff;
	BufferIO::WriteInt16(pbuf, (short)pdeck[0].main.size());
	BufferIO::WriteInt16(pbuf, (short)pdeck[0].extra.size());
	BufferIO::WriteInt16(pbuf, (short)pdeck[0].side.size());
	BufferIO::WriteInt16(pbuf, (short)pdeck[1].main.size());
	BufferIO::WriteInt16(pbuf, (short)pdeck[1].extra.size());
	BufferIO::WriteInt16(pbuf, (short)pdeck[1].side.size());
	NetServer::SendBufferToPlayer(players[0], STOC_DECK_COUNT, deckbuff, 12);
	char tempbuff[6];
	std::memcpy(tempbuff, deckbuff, 6);
	std::memcpy(deckbuff, deckbuff + 6, 6);
	std::memcpy(deckbuff + 6, tempbuff, 6);
	NetServer::SendBufferToPlayer(players[1], STOC_DECK_COUNT, deckbuff, 12);
	NetServer::SendPacketToPlayer(players[0], STOC_SELECT_HAND);
	NetServer::ReSendToPlayer(players[1]);
	hand_result[0] = 0;
	hand_result[1] = 0;
	players[0]->state = CTOS_HAND_RESULT;
	players[1]->state = CTOS_HAND_RESULT;
	duel_stage = DUEL_STAGE_FINGER;
}
void SingleDuel::HandResult(DuelPlayer* dp, unsigned char res) {
	if(res > 3)
		return;
	if(dp->state != CTOS_HAND_RESULT)
		return;
	hand_result[dp->type] = res;
	if(hand_result[0] && hand_result[1]) {
		STOC_HandResult schr;
		schr.res1 = hand_result[0];
		schr.res2 = hand_result[1];
		NetServer::SendPacketToPlayer(players[0], STOC_HAND_RESULT, schr);
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
		NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
		schr.res1 = hand_result[1];
		schr.res2 = hand_result[0];
		NetServer::SendPacketToPlayer(players[1], STOC_HAND_RESULT, schr);
		if(hand_result[0] == hand_result[1]) {
			NetServer::SendPacketToPlayer(players[0], STOC_SELECT_HAND);
			NetServer::ReSendToPlayer(players[1]);
			hand_result[0] = 0;
			hand_result[1] = 0;
			players[0]->state = CTOS_HAND_RESULT;
			players[1]->state = CTOS_HAND_RESULT;
		} else if((hand_result[0] == 1 && hand_result[1] == 2)
		          || (hand_result[0] == 2 && hand_result[1] == 3)
		          || (hand_result[0] == 3 && hand_result[1] == 1)) {
			NetServer::SendPacketToPlayer(players[1], STOC_SELECT_TP);
			tp_player = 1;
			players[0]->state = 0xff;
			players[1]->state = CTOS_TP_RESULT;
			duel_stage = DUEL_STAGE_FIRSTGO;
		} else {
			NetServer::SendPacketToPlayer(players[0], STOC_SELECT_TP);
			players[1]->state = 0xff;
			players[0]->state = CTOS_TP_RESULT;
			tp_player = 0;
			duel_stage = DUEL_STAGE_FIRSTGO;
		}
	}
}
void SingleDuel::TPResult(DuelPlayer* dp, unsigned char tp) {
	if(dp->state != CTOS_TP_RESULT)
		return;
	duel_stage = DUEL_STAGE_DUELING;
	bool swapped = false;
	pplayer[0] = players[0];
	pplayer[1] = players[1];
	if((tp && dp->type == 1) || (!tp && dp->type == 0)) {
		DuelPlayer* p = players[0];
		players[0] = players[1];
		players[1] = p;
		players[0]->type = 0;
		players[1]->type = 1;
		Deck d = pdeck[0];
		pdeck[0] = pdeck[1];
		pdeck[1] = d;
		swapped = true;
	}
	dp->state = CTOS_RESPONSE;
	std::random_device rd;
	unsigned int seed = rd();
#ifdef YGOPRO_SERVER_MODE
	if(pre_seed[duel_count] > 0) {
		seed = pre_seed[duel_count];
	}
#endif
	mt19937 rnd((uint_fast32_t)seed);
	auto duel_seed = rnd.rand();
	ReplayHeader rh;
	rh.id = 0x31707279;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_UNIFORM;
	rh.seed = seed;
	rh.start_time = (unsigned int)time(nullptr);
	last_replay.BeginRecord();
	last_replay.WriteHeader(rh);
	last_replay.WriteData(players[0]->name, 40, false);
	last_replay.WriteData(players[1]->name, 40, false);
	if(!host_info.no_shuffle_deck) {
		rnd.shuffle_vector(pdeck[0].main);
		rnd.shuffle_vector(pdeck[1].main);
	}
	time_limit[0] = host_info.time_limit;
	time_limit[1] = host_info.time_limit;
	set_script_reader(DataManager::ScriptReaderEx);
	set_card_reader(DataManager::CardReader);
	set_message_handler(SingleDuel::MessageHandler);
	pduel = create_duel(duel_seed);
	set_player_info(pduel, 0, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	set_player_info(pduel, 1, host_info.start_lp, host_info.start_hand, host_info.draw_count);
#ifdef YGOPRO_SERVER_MODE
	preload_script(pduel, "./script/special.lua", 0);
#endif
	int opt = (int)host_info.duel_rule << 16;
	if(host_info.no_shuffle_deck)
		opt |= DUEL_PSEUDO_SHUFFLE;
	last_replay.WriteInt32(host_info.start_lp, false);
	last_replay.WriteInt32(host_info.start_hand, false);
	last_replay.WriteInt32(host_info.draw_count, false);
	last_replay.WriteInt32(opt, false);
	last_replay.Flush();
	last_replay.WriteInt32(pdeck[0].main.size(), false);
	for(int32 i = (int32)pdeck[0].main.size() - 1; i >= 0; --i) {
		new_card(pduel, pdeck[0].main[i]->first, 0, 0, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(pdeck[0].main[i]->first, false);
	}
	last_replay.WriteInt32(pdeck[0].extra.size(), false);
	for(int32 i = (int32)pdeck[0].extra.size() - 1; i >= 0; --i) {
		new_card(pduel, pdeck[0].extra[i]->first, 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(pdeck[0].extra[i]->first, false);
	}
	last_replay.WriteInt32(pdeck[1].main.size(), false);
	for(int32 i = (int32)pdeck[1].main.size() - 1; i >= 0; --i) {
		new_card(pduel, pdeck[1].main[i]->first, 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(pdeck[1].main[i]->first, false);
	}
	last_replay.WriteInt32(pdeck[1].extra.size(), false);
	for(int32 i = (int32)pdeck[1].extra.size() - 1; i >= 0; --i) {
		new_card(pduel, pdeck[1].extra[i]->first, 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(pdeck[1].extra[i]->first, false);
	}
	last_replay.Flush();
	unsigned char startbuf[32];
	auto pbuf = startbuf;
	BufferIO::WriteInt8(pbuf, MSG_START);
	BufferIO::WriteInt8(pbuf, 0);
	BufferIO::WriteInt8(pbuf, host_info.duel_rule);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, LOCATION_DECK));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, LOCATION_EXTRA));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, LOCATION_DECK));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, LOCATION_EXTRA));
	NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, startbuf, 19);
	startbuf[1] = 1;
	NetServer::SendBufferToPlayer(players[1], STOC_GAME_MSG, startbuf, 19);
	if(!swapped)
		startbuf[1] = 0x10;
	else
		startbuf[1] = 0x11;
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::SendBufferToPlayer(*oit, STOC_GAME_MSG, startbuf, 19);
#ifdef YGOPRO_SERVER_MODE
	if(cache_recorder)
		NetServer::SendBufferToPlayer(cache_recorder, STOC_GAME_MSG, startbuf, 19);
	if(replay_recorder)
		NetServer::SendBufferToPlayer(replay_recorder, STOC_GAME_MSG, startbuf, 19);
	turn_player = 0;
	phase = 1;
#endif
	RefreshExtra(0);
	RefreshExtra(1);
	start_duel(pduel, opt);
	if(host_info.time_limit) {
		time_elapsed = 0;
#ifdef YGOPRO_SERVER_MODE
		time_compensator[0] = host_info.time_limit;
		time_compensator[1] = host_info.time_limit;
		time_backed[0] = host_info.time_limit;
		time_backed[1] = host_info.time_limit;
		last_game_msg = 0;
#endif
		timeval timeout = { 1, 0 };
		event_add(etimer, &timeout);
	}
	Process();
}
void SingleDuel::Process() {
	std::vector<unsigned char> engineBuffer;
	engineBuffer.reserve(SIZE_MESSAGE_BUFFER);
	unsigned int engFlag = 0;
	int engLen = 0;
	int stop = 0;
	while (!stop) {
		if (engFlag == PROCESSOR_END)
			break;
		unsigned int result = process(pduel);
		engLen = result & PROCESSOR_BUFFER_LEN;
		engFlag = result & PROCESSOR_FLAG;
		if (engLen > 0) {
			if (engLen > (int)engineBuffer.size())
				engineBuffer.resize(engLen);
			get_message(pduel, engineBuffer.data());
			stop = Analyze(engineBuffer.data(), engLen);
		}
	}
	if(stop == 2)
		DuelEndProc();
}
void SingleDuel::DuelEndProc() {
	if(!match_mode) {
		NetServer::SendPacketToPlayer(players[0], STOC_DUEL_END);
		NetServer::ReSendToPlayer(players[1]);
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
		NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
		NetServer::StopServer();
#else
		duel_stage = DUEL_STAGE_END;
#endif
	} else {
		int winc[3] = {0, 0, 0};
		for(int i = 0; i < duel_count; ++i)
			winc[match_result[i]]++;
		if(match_kill
		        || (winc[0] == 2 || (winc[0] == 1 && winc[2] == 2))
		        || (winc[1] == 2 || (winc[1] == 1 && winc[2] == 2))
		        || (winc[2] == 3 || (winc[0] == 1 && winc[1] == 1 && winc[2] == 1)) ) {
			NetServer::SendPacketToPlayer(players[0], STOC_DUEL_END);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
			NetServer::StopServer();
#else
			duel_stage = DUEL_STAGE_END;
#endif
		} else {
			if(players[0] != pplayer[0]) {
				players[0] = pplayer[0];
				players[1] = pplayer[1];
				players[0]->type = 0;
				players[1]->type = 1;
				Deck d = pdeck[0];
				pdeck[0] = pdeck[1];
				pdeck[1] = d;
			}
			ready[0] = false;
			ready[1] = false;
			players[0]->state = CTOS_UPDATE_DECK;
			players[1]->state = CTOS_UPDATE_DECK;
			NetServer::SendPacketToPlayer(players[0], STOC_CHANGE_SIDE);
			NetServer::SendPacketToPlayer(players[1], STOC_CHANGE_SIDE);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::SendPacketToPlayer(*oit, STOC_WAITING_SIDE);
#ifdef YGOPRO_SERVER_MODE
			if(cache_recorder)
				NetServer::SendPacketToPlayer(cache_recorder, STOC_WAITING_SIDE);
			if(replay_recorder)
				NetServer::SendPacketToPlayer(replay_recorder, STOC_WAITING_SIDE);
#endif
			duel_stage = DUEL_STAGE_SIDING;
		}
	}
}
void SingleDuel::Surrender(DuelPlayer* dp) {
	if(dp->type > 1 || !pduel)
		return;
	unsigned char wbuf[3];
	uint32 player = dp->type;
	wbuf[0] = MSG_WIN;
	wbuf[1] = 1 - player;
	wbuf[2] = 0;
	NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, wbuf, 3);
	NetServer::ReSendToPlayer(players[1]);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
	NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
	if(players[player] == pplayer[player]) {
		match_result[duel_count++] = 1 - player;
		tp_player = player;
	} else {
		match_result[duel_count++] = player;
		tp_player = 1 - player;
	}
	EndDuel();
	DuelEndProc();
	event_del(etimer);
}
// Analyze ocgcore message
int SingleDuel::Analyze(unsigned char* msgbuffer, unsigned int len) {
	unsigned char* offset, *pbufw, *pbuf = msgbuffer;
	int player, count, type;
	while (pbuf - msgbuffer < (int)len) {
		offset = pbuf;
		unsigned char engType = BufferIO::ReadUInt8(pbuf);
#ifdef YGOPRO_SERVER_MODE
		last_game_msg = engType;
#endif
		switch (engType) {
		case MSG_RETRY: {
			WaitforResponse(last_response);
			NetServer::SendBufferToPlayer(players[last_response], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_HINT: {
			type = BufferIO::ReadUInt8(pbuf);
			player = BufferIO::ReadUInt8(pbuf);
			BufferIO::ReadInt32(pbuf);
			switch (type) {
			case 1:
			case 2:
			case 3:
			case 5: {
				NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
				break;
			}
			case 4:
			case 6:
			case 7:
			case 8:
			case 9:
			case 11: {
				NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
				break;
			}
			case 10: {
				NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
				NetServer::SendBufferToPlayer(players[1], STOC_GAME_MSG, offset, pbuf - offset);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
				break;
			}
			}
			break;
		}
		case MSG_WIN: {
			player = BufferIO::ReadUInt8(pbuf);
			type = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			if(player > 1) {
				match_result[duel_count++] = 2;
				tp_player = 1 - tp_player;
			} else if(players[player] == pplayer[player]) {
				match_result[duel_count++] = player;
				tp_player = 1 - player;
			} else {
				match_result[duel_count++] = 1 - player;
				tp_player = player;
			}
			EndDuel();
			return 2;
		}
		case MSG_SELECT_BATTLECMD: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 8 + 2;
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_IDLECMD: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 11 + 3;
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_EFFECTYN: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 12;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 4;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 4;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 3;
			count = BufferIO::ReadUInt8(pbuf);
			int c/*, l, s, ss, code*/;
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadUInt8(pbuf);
				/*l = */BufferIO::ReadUInt8(pbuf);
				/*s = */BufferIO::ReadUInt8(pbuf);
				/*ss = */BufferIO::ReadUInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_UNSELECT_CARD: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadUInt8(pbuf);
			int c/*, l, s, ss, code*/;
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadUInt8(pbuf);
				/*l = */BufferIO::ReadUInt8(pbuf);
				/*s = */BufferIO::ReadUInt8(pbuf);
				/*ss = */BufferIO::ReadUInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			count = BufferIO::ReadUInt8(pbuf);
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadUInt8(pbuf);
				/*l = */BufferIO::ReadUInt8(pbuf);
				/*s = */BufferIO::ReadUInt8(pbuf);
				/*ss = */BufferIO::ReadUInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += 10 + count * 13;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 9;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 6;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 11;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SORT_CARD: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			if(pbuf[5] != LOCATION_DECK) {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
				NetServer::ReSendToPlayer(players[1 - player]);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			} else {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			}
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SHUFFLE_HAND: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(replay_recorder);
#endif
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(cache_recorder);
#endif
			RefreshHand(player, 0x781fff, 0);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(replay_recorder);
#endif
			for (int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(cache_recorder);
#endif
			RefreshExtra(player);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::ReadUInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 6;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			unsigned int loc = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
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
			RefreshHand(0);
			RefreshHand(1);
#ifdef YGOPRO_SERVER_MODE
			turn_player = BufferIO::ReadInt8(pbuf);
#else
			pbuf++;
#endif
			time_limit[0] = host_info.time_limit;
			time_limit[1] = host_info.time_limit;
#ifdef YGOPRO_SERVER_MODE
			time_compensator[0] = host_info.time_limit;
			time_compensator[1] = host_info.time_limit;
			time_backed[0] = host_info.time_limit;
			time_backed[1] = host_info.time_limit;
#endif
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_NEW_PHASE: {
#ifdef YGOPRO_SERVER_MODE
			phase = BufferIO::ReadInt16(pbuf);
#else
			pbuf += 2;
#endif
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
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
			int pc = pbuf[4];
			int pl = pbuf[5];
			/*int ps = pbuf[6];*/
			/*int pp = pbuf[7];*/
			int cc = pbuf[8];
			int cl = pbuf[9];
			int cs = pbuf[10];
			int cp = pbuf[11];
			pbuf += 16;
			NetServer::SendBufferToPlayer(players[cc], STOC_GAME_MSG, offset, pbuf - offset);
			if (!(cl & (LOCATION_GRAVE + LOCATION_OVERLAY)) && ((cl & (LOCATION_DECK + LOCATION_HAND)) || (cp & POS_FACEDOWN)))
				BufferIO::WriteInt32(pbufw, 0);
			NetServer::SendBufferToPlayer(players[1 - cc], STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			if (cl != 0 && (cl & LOCATION_OVERLAY) == 0 && (cl != pl || pc != cc))
				RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_POS_CHANGE: {
			int cc = pbuf[4];
			int cl = pbuf[5];
			int cs = pbuf[6];
			int pp = pbuf[7];
			int cp = pbuf[8];
			pbuf += 9;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			if((pp & POS_FACEDOWN) && (cp & POS_FACEUP))
				RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_SET: {
			BufferIO::WriteInt32(pbuf, 0);
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SWAP: {
			int c1 = pbuf[4];
			int l1 = pbuf[5];
			int s1 = pbuf[6];
			int c2 = pbuf[12];
			int l2 = pbuf[13];
			int s2 = pbuf[14];
			pbuf += 16;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshSingle(c1, l1, s1);
			RefreshSingle(c2, l2, s2);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_SPSUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_FLIPSUMMONING: {
			RefreshSingle(pbuf[4], pbuf[5], pbuf[6]);
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_FLIPSUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_CHAINING: {
			pbuf += 16;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
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
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			RefreshHand(0);
			RefreshHand(1);
			break;
		}
		case MSG_CHAIN_END: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
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
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CARD_SELECTED: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 4;
			break;
		}
		case MSG_RANDOM_SELECTED: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(replay_recorder);
#endif
			for (int i = 0; i < count; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayer(cache_recorder);
#endif
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_BATTLE: {
			pbuf += 26;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_ATTACK_DISABLED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_MISSED_EFFECT: {
			player = pbuf[0];
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::ReadUInt8(pbuf);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadUInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_CARD:
		case MSG_ANNOUNCE_NUMBER: {
			player = BufferIO::ReadUInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += 4 * count;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CARD_HINT: {
			pbuf += 9;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 6;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
			NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			break;
		}
		case MSG_MATCH_KILL: {
			int code = BufferIO::ReadInt32(pbuf);
			if(match_mode) {
				match_kill = code;
				NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
				NetServer::ReSendToPlayer(players[1]);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
				NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
			}
			break;
		}
		}
	}
	return 0;
}
void SingleDuel::GetResponse(DuelPlayer* dp, unsigned char* pdata, unsigned int len) {
	byte resb[SIZE_RETURN_VALUE];
	if (len > SIZE_RETURN_VALUE)
		len = SIZE_RETURN_VALUE;
	std::memcpy(resb, pdata, len);
	last_replay.WriteInt8(len);
	last_replay.WriteData(resb, len);
	set_responseb(pduel, resb);
	players[dp->type]->state = 0xff;
	if(host_info.time_limit) {
		if(time_limit[dp->type] >= time_elapsed)
			time_limit[dp->type] -= time_elapsed;
		else time_limit[dp->type] = 0;
		time_elapsed = 0;
#ifdef YGOPRO_SERVER_MODE
		if(time_backed[dp->type] > 0 && time_limit[dp->type] < host_info.time_limit && NetServer::IsCanIncreaseTime(last_game_msg, pdata, len)) {
			++time_limit[dp->type];
			++time_compensator[dp->type];
			--time_backed[dp->type];
		}
#endif
	}
	Process();
}
void SingleDuel::EndDuel() {
	if(!pduel)
		return;
	last_replay.EndRecord();
	char replaybuf[0x2000], *pbuf = replaybuf;
	std::memcpy(pbuf, &last_replay.pheader, sizeof(ReplayHeader));
	pbuf += sizeof(ReplayHeader);
	std::memcpy(pbuf, last_replay.comp_data, last_replay.comp_size);
	NetServer::SendBufferToPlayer(players[0], STOC_REPLAY, replaybuf, sizeof(ReplayHeader) + last_replay.comp_size);
	NetServer::ReSendToPlayer(players[1]);
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & REPLAY_MODE_WATCHER_NO_SEND)) {
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
		NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
	}
#else
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
#endif //YGOPRO_SERVER_MODE
	end_duel(pduel);
	event_del(etimer);
	pduel = 0;
}
void SingleDuel::WaitforResponse(int playerid) {
	last_response = playerid;
	unsigned char msg = MSG_WAITING;
	NetServer::SendPacketToPlayer(players[1 - playerid], STOC_GAME_MSG, msg);
	if(host_info.time_limit) {
		STOC_TimeLimit sctl;
		sctl.player = playerid;
		sctl.left_time = time_limit[playerid];
		NetServer::SendPacketToPlayer(players[0], STOC_TIME_LIMIT, sctl);
		NetServer::SendPacketToPlayer(players[1], STOC_TIME_LIMIT, sctl);
		players[playerid]->state = CTOS_TIME_CONFIRM;
	} else
		players[playerid]->state = CTOS_RESPONSE;
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RequestField(DuelPlayer* dp) {
	if(dp->type > 1)
		return;
	int player = dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);

	unsigned char startbuf[32], *pbuf = startbuf;
	BufferIO::WriteInt8(pbuf, MSG_START);
	BufferIO::WriteInt8(pbuf, player);
	BufferIO::WriteInt8(pbuf, host_info.duel_rule);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt16(pbuf, 0);
	BufferIO::WriteInt16(pbuf, 0);
	BufferIO::WriteInt16(pbuf, 0);
	BufferIO::WriteInt16(pbuf, 0);
	NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, startbuf, 19);

	int newturn_count = 1;
	if(turn_player == 1)
		newturn_count = 2;
	for(int i = 0; i < newturn_count; i++) {
		unsigned char turnbuf[2], *pbuf_t = turnbuf;
		BufferIO::WriteInt8(pbuf_t, MSG_NEW_TURN);
		BufferIO::WriteInt8(pbuf_t, i);
		NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, turnbuf, 2);		
	}

	unsigned char phasebuf[4], *pbuf_p = phasebuf;
	BufferIO::WriteInt8(pbuf_p, MSG_NEW_PHASE);
	BufferIO::WriteInt16(pbuf_p, phase);
	NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, phasebuf, 3);

	unsigned char query_buffer[1024];
	int length = query_field_info(pduel, (unsigned char*)query_buffer);
	NetServer::SendBufferToPlayer(dp, STOC_GAME_MSG, query_buffer, length);
	RefreshMzone(1 - player, 0xefffff, 0, dp);
	RefreshMzone(player, 0xefffff, 0, dp);
	RefreshSzone(1 - player, 0xefffff, 0, dp);
	RefreshSzone(player, 0xefffff, 0, dp);
	RefreshHand(1 - player, 0xefffff, 0, dp);
	RefreshHand(player, 0xefffff, 0, dp);
	RefreshGrave(1 - player, 0xefffff, 0, dp);
	RefreshGrave(player, 0xefffff, 0, dp);
	RefreshExtra(1 - player, 0xefffff, 0, dp);
	RefreshExtra(player, 0xefffff, 0, dp);
	RefreshRemoved(1 - player, 0xefffff, 0, dp);
	RefreshRemoved(player, 0xefffff, 0, dp);
	/*
	if(dp == players[last_response])
		WaitforResponse(last_response);
	*/
	STOC_TimeLimit sctl;
	sctl.player = 1 - last_response;
	sctl.left_time = time_limit[1 - last_response];
	NetServer::SendPacketToPlayer(dp, STOC_TIME_LIMIT, sctl);
	sctl.player = last_response;
	sctl.left_time = time_limit[last_response] - time_elapsed;
	NetServer::SendPacketToPlayer(dp, STOC_TIME_LIMIT, sctl);

	NetServer::SendPacketToPlayer(dp, STOC_FIELD_FINISH);
}
#endif //YGOPRO_SERVER_MODE
void SingleDuel::TimeConfirm(DuelPlayer* dp) {
	if(host_info.time_limit == 0)
		return;
	if(dp->type != last_response)
		return;
	players[last_response]->state = CTOS_RESPONSE;
#ifdef YGOPRO_SERVER_MODE
	if(time_elapsed < 10 && time_elapsed <= time_compensator[dp->type]){
		time_compensator[dp->type] -= time_elapsed;
		time_elapsed = 0;
	}
	else {
		time_limit[dp->type] -= time_elapsed;
		time_elapsed = 0;
	}
#else
	if(time_elapsed < 10)
		time_elapsed = 0;
#endif //YGOPRO_SERVER_MODE
}
inline int SingleDuel::WriteUpdateData(int& player, int location, int& flag, unsigned char*& qbuf, int& use_cache) {
	flag |= (QUERY_CODE | QUERY_POSITION);
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, location);
	int len = query_field_card(pduel, player, location, flag, qbuf, use_cache);
	return len;
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshMzone(int player, int flag, int use_cache, DuelPlayer* dp)
#else
void SingleDuel::RefreshMzone(int player, int flag, int use_cache)
#endif //YGOPRO_SERVER_MODE
{
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_MZONE, flag, qbuf, use_cache);
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[player])
#endif
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(replay_recorder);
#endif
	int qlen = 0;
	while(qlen < len) {
		const int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen <= LEN_HEADER)
			continue;
		auto position = GetPosition(qbuf, 8);
		if (position & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[1 - player])
#endif
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
if(!dp)
#endif
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(cache_recorder);
#endif
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshSzone(int player, int flag, int use_cache, DuelPlayer* dp)
#else
void SingleDuel::RefreshSzone(int player, int flag, int use_cache)
#endif //YGOPRO_SERVER_MODE
{
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_SZONE, flag, qbuf, use_cache);
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[player])
#endif
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(replay_recorder);
#endif
	int qlen = 0;
	while(qlen < len) {
		const int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen <= LEN_HEADER)
			continue;
		auto position = GetPosition(qbuf, 8);
		if (position & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[1 - player])
#endif
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
if(!dp)
#endif
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(cache_recorder);
#endif
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshHand(int player, int flag, int use_cache, DuelPlayer* dp)
#else
void SingleDuel::RefreshHand(int player, int flag, int use_cache)
#endif //YGOPRO_SERVER_MODE
{
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_HAND, flag, qbuf, use_cache);
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[player])
#endif
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(replay_recorder);
#endif
	int qlen = 0;
	while(qlen < len) {
		const int slen = BufferIO::ReadInt32(qbuf);
		qlen += slen;
		if (slen <= LEN_HEADER)
			continue;
		auto position = GetPosition(qbuf, 8);
		if(!(position & POS_FACEUP))
			memset(qbuf, 0, slen - 4);
		qbuf += slen - 4;
	}
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[1 - player])
#endif
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
if(!dp)
#endif
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(cache_recorder);
#endif
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshGrave(int player, int flag, int use_cache, DuelPlayer* dp)
#else
void SingleDuel::RefreshGrave(int player, int flag, int use_cache)
#endif //YGOPRO_SERVER_MODE
{
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_GRAVE, flag, qbuf, use_cache);
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[0])
#endif
	NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
	if(!dp || dp == players[1])
		NetServer::SendBufferToPlayer(players[1], STOC_GAME_MSG, query_buffer.data(), len + 3);
if(!dp)
#else
	NetServer::ReSendToPlayer(players[1]);
#endif
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshExtra(int player, int flag, int use_cache, DuelPlayer* dp)
#else
void SingleDuel::RefreshExtra(int player, int flag, int use_cache)
#endif //YGOPRO_SERVER_MODE
{
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_EXTRA, flag, qbuf, use_cache);
#ifdef YGOPRO_SERVER_MODE
if(!dp || dp == players[player])
#endif
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer.data(), len + 3);
#ifdef YGOPRO_SERVER_MODE
	if(!dp)
		NetServer::ReSendToPlayer(replay_recorder);
	int qlen = 0;
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen <= LEN_HEADER)
			continue;
		auto position = GetPosition(qbuf, 8);
		if (position & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	if(!dp || dp == players[1 - player])
		NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer.data(), len + 3);
	if(!dp)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::ReSendToPlayer(*pit);
	if(!dp)
		NetServer::ReSendToPlayer(cache_recorder);
#endif //YGOPRO_SERVER_MODE
}
#ifdef YGOPRO_SERVER_MODE
void SingleDuel::RefreshRemoved(int player, int flag, int use_cache, DuelPlayer* dp) {
	std::vector<unsigned char> query_buffer;
	query_buffer.resize(SIZE_QUERY_BUFFER);
	auto qbuf = query_buffer.data();
	auto len = WriteUpdateData(player, LOCATION_REMOVED, flag, qbuf, use_cache);
	if(!dp || dp == players[player])
		NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer.data(), len + 3);
	if(!dp)
		NetServer::ReSendToPlayer(replay_recorder);
	int qlen = 0;
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen <= LEN_HEADER)
			continue;
		auto position = GetPosition(qbuf, 8);
		if (position & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	if(!dp || dp == players[1 - player])
		NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer.data(), len + 3);
	if(!dp)
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::ReSendToPlayer(*pit);
	if(!dp)
		NetServer::ReSendToPlayer(cache_recorder);
}
#endif
void SingleDuel::RefreshSingle(int player, int location, int sequence, int flag) {
	flag |= (QUERY_CODE | QUERY_POSITION);
	unsigned char query_buffer[0x1000];
	auto qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_CARD);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, location);
	BufferIO::WriteInt8(qbuf, sequence);
	int len = query_card(pduel, player, location, sequence, flag, qbuf, 0);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 4);
	if (len <= LEN_HEADER)
		return;
	const int clen = BufferIO::ReadInt32(qbuf);
	auto position = GetPosition(qbuf, 8);
	if (position & POS_FACEDOWN) {
		BufferIO::WriteInt32(qbuf, QUERY_CODE);
		BufferIO::WriteInt32(qbuf, 0);
		memset(qbuf, 0, clen - 12);
	}
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer, len + 4);
	for (auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
#ifdef YGOPRO_SERVER_MODE
	NetServer::ReSendToPlayers(cache_recorder, replay_recorder);
#endif
}
uint32 SingleDuel::MessageHandler(intptr_t fduel, uint32 type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}
void SingleDuel::SingleTimer(evutil_socket_t fd, short events, void* arg) {
	SingleDuel* sd = static_cast<SingleDuel*>(arg);
	sd->time_elapsed++;
	if(sd->time_elapsed >= sd->time_limit[sd->last_response] || sd->time_limit[sd->last_response] <= 0) {
		unsigned char wbuf[3];
		uint32 player = sd->last_response;
		wbuf[0] = MSG_WIN;
		wbuf[1] = 1 - player;
		wbuf[2] = 0x3;
		NetServer::SendBufferToPlayer(sd->players[0], STOC_GAME_MSG, wbuf, 3);
		NetServer::ReSendToPlayer(sd->players[1]);
		for(auto oit = sd->observers.begin(); oit != sd->observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
#ifdef YGOPRO_SERVER_MODE
		NetServer::ReSendToPlayers(sd->cache_recorder, sd->replay_recorder);
#endif
		if(sd->players[player] == sd->pplayer[player]) {
			sd->match_result[sd->duel_count++] = 1 - player;
			sd->tp_player = player;
		} else {
			sd->match_result[sd->duel_count++] = player;
			sd->tp_player = 1 - player;
		}
		sd->EndDuel();
		sd->DuelEndProc();
		event_del(sd->etimer);
		return;
	}
	timeval timeout = { 1, 0 };
	event_add(sd->etimer, &timeout);
}

}
