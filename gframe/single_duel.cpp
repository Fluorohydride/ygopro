#include "single_duel.h"
#include "netserver.h"
#include "game.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/common.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

SingleDuel::SingleDuel(bool is_match) {
	game_started = false;
	match_mode = is_match;
	match_kill = 0;
	for(int i = 0; i < 2; ++i) {
		players[i] = 0;
		ready[i] = false;
	}
	duel_count = 0;
	memset(match_result, 0, 3);
}
SingleDuel::~SingleDuel() {
}
void SingleDuel::Chat(DuelPlayer* dp, void* pdata, int len) {
	STOC_Chat scc;
	scc.player = dp->type;
	unsigned short* msg = (unsigned short*)pdata;
	int msglen = BufferIO::CopyWStr(msg, scc.msg, 256);
	NetServer::SendBufferToPlayer(players[0], STOC_CHAT, &scc, 4 + msglen * 2);
	NetServer::ReSendToPlayer(players[1]);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void SingleDuel::JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {
	if(!is_creater) {
		if(dp->game && dp->type != 0xff) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 0;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			NetServer::DisconnectPlayer(dp);
			return;
		}
		CTOS_JoinGame* pkt = (CTOS_JoinGame*)pdata;
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
		if(wcscmp(jpass, pass)) {
			STOC_ErrorMsg scem;
			scem.msg = ERRMSG_JOINERROR;
			scem.code = 1;
			NetServer::SendPacketToPlayer(dp, STOC_ERROR_MSG, scem);
			return;
		}
	}
	dp->game = this;
	if(!players[0] && !players[1] && observers.size() == 0)
		host_player = dp;
	STOC_JoinGame scjg;
	scjg.info = host_info;
	STOC_TypeChange sctc;
	sctc.type = (host_player == dp) ? 0x10 : 0;
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
		scwc.watch_count = observers.size();
		if(players[0])
			NetServer::SendPacketToPlayer(players[0], STOC_HS_WATCH_CHANGE, scwc);
		if(players[1])
			NetServer::SendPacketToPlayer(players[1], STOC_HS_WATCH_CHANGE, scwc);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
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
		scwc.watch_count = observers.size();
		NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
	}
}
void SingleDuel::LeaveGame(DuelPlayer* dp) {
	if(dp == host_player) {
		EndDuel();
		NetServer::StopServer();
	} else if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		observers.erase(dp);
		if(!game_started) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = observers.size();
			if(players[0])
				NetServer::SendPacketToPlayer(players[0], STOC_HS_WATCH_CHANGE, scwc);
			if(players[1])
				NetServer::SendPacketToPlayer(players[1], STOC_HS_WATCH_CHANGE, scwc);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		if(!game_started && duel_count == 0) {
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
			NetServer::DisconnectPlayer(dp);
		} else {
			if(!game_started) {
				if(!ready[0])
					NetServer::SendPacketToPlayer(players[0], STOC_DUEL_START);
				if(!ready[1])
					NetServer::SendPacketToPlayer(players[1], STOC_DUEL_START);
			}
			unsigned char wbuf[3];
			wbuf[0] = MSG_WIN;
			wbuf[1] = 1 - dp->type;
			wbuf[2] = 0;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, wbuf, 3);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			EndDuel();
			NetServer::SendPacketToPlayer(players[0], STOC_DUEL_END);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			NetServer::StopServer();
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
	scwc.watch_count = observers.size();
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
				bool allow_ocg = host_info.rule == 0 || host_info.rule == 2;
				bool allow_tcg = host_info.rule == 1 || host_info.rule == 2;
				deckerror = deckManager.CheckDeck(pdeck[dp->type], host_info.lflist, allow_ocg, allow_tcg);
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
}
void SingleDuel::PlayerKick(DuelPlayer* dp, unsigned char pos) {
	if(pos > 1 || dp != host_player || dp == players[pos] || !players[pos])
		return;
	LeaveGame(players[pos]);
}
void SingleDuel::UpdateDeck(DuelPlayer* dp, void* pdata) {
	if(dp->type > 1 || ready[dp->type])
		return;
	char* deckbuf = (char*)pdata;
	int mainc = BufferIO::ReadInt32(deckbuf);
	int sidec = BufferIO::ReadInt32(deckbuf);
	if(duel_count == 0) {
		deck_error[dp->type] = deckManager.LoadDeck(pdeck[dp->type], (int*)deckbuf, mainc, sidec);
	} else {
		if(deckManager.LoadSide(pdeck[dp->type], (int*)deckbuf, mainc, sidec)) {
			ready[dp->type] = true;
			NetServer::SendPacketToPlayer(dp, STOC_DUEL_START);
			if(ready[0] && ready[1]) {
				NetServer::SendPacketToPlayer(players[tp_player], STOC_SELECT_TP);
				players[1 - tp_player]->state = 0xff;
				players[tp_player]->state = CTOS_TP_RESULT;
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
	game_started = true;
	NetServer::SendPacketToPlayer(players[0], STOC_DUEL_START);
	NetServer::ReSendToPlayer(players[1]);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit) {
		(*oit)->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(*oit);
	}
	NetServer::SendPacketToPlayer(players[0], STOC_SELECT_HAND);
	NetServer::ReSendToPlayer(players[1]);
	hand_result[0] = 0;
	hand_result[1] = 0;
	players[0]->state = CTOS_HAND_RESULT;
	players[1]->state = CTOS_HAND_RESULT;
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
			NetServer::SendPacketToPlayer(players[1], CTOS_TP_RESULT);
			tp_player = 1;
			players[0]->state = 0xff;
			players[1]->state = CTOS_TP_RESULT;
		} else {
			NetServer::SendPacketToPlayer(players[0], CTOS_TP_RESULT);
			players[1]->state = 0xff;
			players[0]->state = CTOS_TP_RESULT;
			tp_player = 0;
		}
	}
}
void SingleDuel::TPResult(DuelPlayer* dp, unsigned char tp) {
	if(dp->state != CTOS_TP_RESULT)
		return;
	bool swapped = false;
	mtrandom rnd;
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
	ReplayHeader rh;
	rh.id = 0x31707279;
	rh.version = PRO_VERSION;
	rh.flag = 0;
	time_t seed = time(0);
	rh.seed = seed;
	last_replay.BeginRecord();
	last_replay.WriteHeader(rh);
	rnd.reset(seed);
	last_replay.WriteData(players[0]->name, 40, false);
	last_replay.WriteData(players[1]->name, 40, false);
	if(!host_info.no_shuffle_deck) {
		for(size_t i = pdeck[0].main.size() - 1; i > 0; --i) {
			int swap = rnd.real() * (i + 1);
			std::swap(pdeck[0].main[i], pdeck[0].main[swap]);
		}
		for(size_t i = pdeck[1].main.size() - 1; i > 0; --i) {
			int swap = rnd.real() * (i + 1);
			std::swap(pdeck[1].main[i], pdeck[1].main[swap]);
		}
	}
	time_limit[0] = host_info.time_limit;
	time_limit[1] = host_info.time_limit;
	set_script_reader((script_reader)DataManager::ScriptReaderEx);
	set_card_reader((card_reader)DataManager::CardReader);
	set_message_handler((message_handler)SingleDuel::MessageHandler);
	rnd.reset(seed);
	pduel = create_duel(rnd.rand());
	set_player_info(pduel, 0, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	set_player_info(pduel, 1, host_info.start_lp, host_info.start_hand, host_info.draw_count);
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
	char startbuf[32], *pbuf = startbuf;
	BufferIO::WriteInt8(pbuf, MSG_START);
	BufferIO::WriteInt8(pbuf, 0);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt32(pbuf, host_info.start_lp);
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, 0x1));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 0, 0x40));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, 0x1));
	BufferIO::WriteInt16(pbuf, query_field_count(pduel, 1, 0x40));
	NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, startbuf, 18);
	startbuf[1] = 1;
	NetServer::SendBufferToPlayer(players[1], STOC_GAME_MSG, startbuf, 18);
	if(!swapped)
		startbuf[1] = 0x10;
	else startbuf[1] = 0x11;
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::SendBufferToPlayer(*oit, STOC_GAME_MSG, startbuf, 18);
	RefreshExtra(0);
	RefreshExtra(1);
	start_duel(pduel, opt);
	Process();
}
void SingleDuel::Process() {
	char engineBuffer[0x1000];
	unsigned int engFlag = 0, engLen = 0;
	int stop = 0;
	while (!stop) {
		if (engFlag == 2)
			break;
		int result = process(pduel);
		engLen = result & 0xffff;
		engFlag = result >> 16;
		if (engLen > 0) {
			get_message(pduel, (byte*)&engineBuffer);
			stop = Analyze(engineBuffer, engLen);
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
		NetServer::StopServer();
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
			NetServer::StopServer();
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
int SingleDuel::Analyze(char* msgbuffer, unsigned int len) {
	char* offset, *pbufw, *pbuf = msgbuffer;
	int player, count, type;
	while (pbuf - msgbuffer < (int)len) {
		offset = pbuf;
		unsigned char engType = BufferIO::ReadUInt8(pbuf);
		switch (engType) {
		case MSG_RETRY: {
			WaitforResponse(last_response);
			NetServer::SendBufferToPlayer(players[last_response], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_HINT: {
			type = BufferIO::ReadInt8(pbuf);
			player = BufferIO::ReadInt8(pbuf);
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
			case 9: {
				NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
				break;
			}
			case 10: {
				NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
				NetServer::SendBufferToPlayer(players[1], STOC_GAME_MSG, offset, pbuf - offset);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
				break;
			}
			}
			break;
		}
		case MSG_WIN: {
			player = BufferIO::ReadInt8(pbuf);
			type = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadInt8(pbuf);
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
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
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
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 12;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 3;
			count = BufferIO::ReadInt8(pbuf);
			int c/*, l, s, ss, code*/;
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadInt8(pbuf);
				/*l = */BufferIO::ReadInt8(pbuf);
				/*s = */BufferIO::ReadInt8(pbuf);
				/*ss = */BufferIO::ReadInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_UNSELECT_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			int c/*, l, s, ss, code*/;
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadInt8(pbuf);
				/*l = */BufferIO::ReadInt8(pbuf);
				/*s = */BufferIO::ReadInt8(pbuf);
				/*ss = */BufferIO::ReadInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			count = BufferIO::ReadInt8(pbuf);
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				/*code = */BufferIO::ReadInt32(pbuf);
				c = BufferIO::ReadInt8(pbuf);
				/*l = */BufferIO::ReadInt8(pbuf);
				/*s = */BufferIO::ReadInt8(pbuf);
				/*ss = */BufferIO::ReadInt8(pbuf);
				if (c != player) BufferIO::WriteInt32(pbufw, 0);
			}
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 10 + count * 13;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 9;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 6;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			if(pbuf[5] != LOCATION_DECK) {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
				NetServer::ReSendToPlayer(players[1 - player]);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
			} else {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			}
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshHand(player, 0x781fff, 0);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for (int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshExtra(player);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 6;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			int loc = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			pbuf++;
			time_limit[0] = host_info.time_limit;
			time_limit[1] = host_info.time_limit;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			if (cl != 0 && (cl & 0x80) == 0 && (cl != pl || pc != cc))
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
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			break;
		}
		case MSG_SPSUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			break;
		}
		case MSG_FLIPSUMMONED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CARD_SELECTED: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			break;
		}
		case MSG_RANDOM_SELECTED: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = 0; i < count; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 26;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ATTACK_DISABLED: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::ReadInt8(pbuf);
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
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			WaitforResponse(player);
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_NUMBER:
		case MSG_ANNOUNCE_CARD_FILTER: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
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
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 6;
			NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, offset, pbuf - offset);
			NetServer::ReSendToPlayer(players[1]);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
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
			}
			break;
		}
		}
	}
	return 0;
}
void SingleDuel::GetResponse(DuelPlayer* dp, void* pdata, unsigned int len) {
	byte resb[64];
	memcpy(resb, pdata, len);
	last_replay.WriteInt8(len);
	last_replay.WriteData(resb, len);
	set_responseb(pduel, resb);
	players[dp->type]->state = 0xff;
	if(host_info.time_limit) {
		if(time_limit[dp->type] >= time_elapsed)
			time_limit[dp->type] -= time_elapsed;
		else time_limit[dp->type] = 0;
		event_del(etimer);
	}
	Process();
}
void SingleDuel::EndDuel() {
	if(!pduel)
		return;
	last_replay.EndRecord();
	char replaybuf[0x2000], *pbuf = replaybuf;
	memcpy(pbuf, &last_replay.pheader, sizeof(ReplayHeader));
	pbuf += sizeof(ReplayHeader);
	memcpy(pbuf, last_replay.comp_data, last_replay.comp_size);
	NetServer::SendBufferToPlayer(players[0], STOC_REPLAY, replaybuf, sizeof(ReplayHeader) + last_replay.comp_size);
	NetServer::ReSendToPlayer(players[1]);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
	end_duel(pduel);
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
void SingleDuel::TimeConfirm(DuelPlayer* dp) {
	if(host_info.time_limit == 0)
		return;
	if(dp->type != last_response)
		return;
	players[last_response]->state = CTOS_RESPONSE;
	time_elapsed = 0;
	timeval timeout = {1, 0};
	event_add(etimer, &timeout);
}
void SingleDuel::RefreshMzone(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_MZONE);
	int len = query_field_card(pduel, player, LOCATION_MZONE, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 3);
	int qlen = 0;
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen == 4)
			continue;
		if (qbuf[11] & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer, len + 3);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void SingleDuel::RefreshSzone(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_SZONE);
	int len = query_field_card(pduel, player, LOCATION_SZONE, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 3);
	int qlen = 0;
	while(qlen < len) {
		int clen = BufferIO::ReadInt32(qbuf);
		qlen += clen;
		if (clen == 4)
			continue;
		if (qbuf[11] & POS_FACEDOWN)
			memset(qbuf, 0, clen - 4);
		qbuf += clen - 4;
	}
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer, len + 3);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void SingleDuel::RefreshHand(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_HAND);
	int len = query_field_card(pduel, player, LOCATION_HAND, flag | QUERY_POSITION, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 3);
	int qlen = 0;
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
	NetServer::SendBufferToPlayer(players[1 - player], STOC_GAME_MSG, query_buffer, len + 3);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void SingleDuel::RefreshGrave(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_GRAVE);
	int len = query_field_card(pduel, player, LOCATION_GRAVE, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[0], STOC_GAME_MSG, query_buffer, len + 3);
	NetServer::ReSendToPlayer(players[1]);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void SingleDuel::RefreshExtra(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_EXTRA);
	int len = query_field_card(pduel, player, LOCATION_EXTRA, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 3);
}
void SingleDuel::RefreshSingle(int player, int location, int sequence, int flag) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_CARD);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, location);
	BufferIO::WriteInt8(qbuf, sequence);
	int len = query_card(pduel, player, location, sequence, flag, (unsigned char*)qbuf, 0);
	NetServer::SendBufferToPlayer(players[player], STOC_GAME_MSG, query_buffer, len + 4);
	if(location == LOCATION_REMOVED && (qbuf[15] & POS_FACEDOWN))
		return;
	if ((location & 0x90) || ((location & 0x2c) && (qbuf[15] & POS_FACEUP))) {
		NetServer::ReSendToPlayer(players[1 - player]);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::ReSendToPlayer(*pit);
	}
}
int SingleDuel::MessageHandler(long fduel, int type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, (byte*)msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}
void SingleDuel::SingleTimer(evutil_socket_t fd, short events, void* arg) {
	SingleDuel* sd = static_cast<SingleDuel*>(arg);
	sd->time_elapsed++;
	if(sd->time_elapsed >= sd->time_limit[sd->last_response]) {
		unsigned char wbuf[3];
		uint32 player = sd->last_response;
		wbuf[0] = MSG_WIN;
		wbuf[1] = 1 - player;
		wbuf[2] = 0x3;
		NetServer::SendBufferToPlayer(sd->players[0], STOC_GAME_MSG, wbuf, 3);
		NetServer::ReSendToPlayer(sd->players[1]);
		for(auto oit = sd->observers.begin(); oit != sd->observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
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
	}
}

}
