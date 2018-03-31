#include "relay_duel.h"
#include "netserver.h"
#include "game.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"
#include "../ocgcore/duel.h"
#include "../ocgcore/field.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

std::vector<ReplayPacket> RelayDuel::replay_stream;

RelayDuel::RelayDuel() {
	game_started = false;
	for (int i = 0; i < 6; i++)
		players[i] = duelist();
	startp[0] = 0;
	endp[0] = 3;
	startp[1] = 3;
	endp[1] = 6;
}
RelayDuel::~RelayDuel() {
}
void RelayDuel::Chat(DuelPlayer* dp, void* pdata, int len) {
	STOC_Chat scc;
	scc.player = dp->type;
	unsigned short* msg = (unsigned short*)pdata;
	int msglen = BufferIO::CopyWStr(msg, scc.msg, 256);
	for(int i = 0; i < 6; i++)
		if(players[i].player != dp)
			NetServer::SendBufferToPlayer(players[i].player, STOC_CHAT, &scc, 4 + msglen * 2);
}
void RelayDuel::JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {
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
	if(!players[0].player && !players[1].player && !players[2].player &&
		!players[3].player && !players[4].player && !players[5].player && observers.size() == 0)
		host_player = dp;
	STOC_JoinGame scjg;
	scjg.info = host_info;
	STOC_TypeChange sctc;
	sctc.type = (host_player == dp) ? 0x10 : 0;
	if(!players[0].player || !players[1].player || !players[2].player ||
		!players[3].player || !players[4].player || !players[5].player) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		for (int i = 0; i < 6; i++)
			if (!players[i].player) {
				scpe.pos = i;
				break;
			}
		for (int i = 0; i < 6; i++)
			if(players[i].player)
				NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_ENTER, scpe);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
		players[scpe.pos] = duelist(dp);
		dp->type = scpe.pos;
		sctc.type |= scpe.pos;
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		sctc.type |= NETPLAYER_TYPE_OBSERVER;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		for (int i = 0; i < 6; i++)
			if (players[i].player)
				NetServer::SendPacketToPlayer(players[i].player, STOC_HS_WATCH_CHANGE, scwc);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
	}
	NetServer::SendPacketToPlayer(dp, STOC_JOIN_GAME, scjg);
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	for (int i = 0; i < 6; i++)
		if (players[i].player) {
			STOC_HS_PlayerEnter scpe;
			BufferIO::CopyWStr(players[i].player->name, scpe.name, 20);
			scpe.pos = i;
			NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_ENTER, scpe);
			if(players[i].ready) {
				STOC_HS_PlayerChange scpc;
				scpc.status = (i << 4) | PLAYERCHANGE_READY;
				NetServer::SendPacketToPlayer(dp, STOC_HS_PLAYER_CHANGE, scpc);
			}
		}
	if(observers.size()) {
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		NetServer::SendPacketToPlayer(dp, STOC_HS_WATCH_CHANGE, scwc);
	}
}
void RelayDuel::LeaveGame(DuelPlayer* dp) {
	if(dp == host_player) {
		EndDuel();
		NetServer::StopServer();
	} else if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		observers.erase(dp);
		if(!game_started) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = observers.size();
			for (int i = 0; i < 6; i++)
				if (players[i].player)
					NetServer::SendPacketToPlayer(players[i].player, STOC_HS_WATCH_CHANGE, scwc);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		if(!game_started) {
			STOC_HS_PlayerChange scpc;
			players[dp->type] = duelist();
			scpc.status = (dp->type << 4) | PLAYERCHANGE_LEAVE;
			for (int i = 0; i < 6; i++)
				if (players[i].player)
					NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_CHANGE, scpc);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
			NetServer::DisconnectPlayer(dp);
		} else {
			EndDuel();
			DuelEndProc();
		}
	}
}
void RelayDuel::ToDuelist(DuelPlayer* dp) {
	if(players[0].player && players[1].player && players[2].player
		&& players[3].player && players[4].player && players[5].player)
		return;
	if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		observers.erase(dp);
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		for (int i = 0; i < 6; i++)
			if (!players[i].player)
				dp->type = i;
		players[dp->type] = duelist(dp);
		scpe.pos = dp->type;
		STOC_HS_WatchChange scwc;
		scwc.watch_count = observers.size();
		for (int i = 0; i < 6; i++)
			if (players[i].player) {
				NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_ENTER, scpe);
				NetServer::SendPacketToPlayer(players[i].player, STOC_HS_WATCH_CHANGE, scwc);
			}
		for(auto pit = observers.begin(); pit != observers.end(); ++pit) {
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
			NetServer::SendPacketToPlayer(*pit, STOC_HS_WATCH_CHANGE, scwc);
		}
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
	} else {
		if(players[dp->type].ready)
			return;
		uint8 dptype = (dp->type + 1) % 6;
		while(players[dptype].player)
			dptype = (dptype + 1) % 6;
		STOC_HS_PlayerChange scpc;
		scpc.status = (dp->type << 4) | dptype;
		for (int i = 0; i < 6; i++)
			if (players[i].player)
				NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_CHANGE, scpc);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
		STOC_TypeChange sctc;
		sctc.type = (dp == host_player ? 0x10 : 0) | dptype;
		NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
		players[dptype] = duelist(dp);
		players[dp->type] = duelist();
		dp->type = dptype;
	}
}
void RelayDuel::ToObserver(DuelPlayer* dp) {
	if(dp->type > 5)
		return;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | PLAYERCHANGE_OBSERVE;
	for (int i = 0; i < 6; i++)
		if (players[i].player)
			NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_CHANGE, scpc);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
	players[dp->type] = duelist();
	dp->type = NETPLAYER_TYPE_OBSERVER;
	observers.insert(dp);
	STOC_TypeChange sctc;
	sctc.type = (dp == host_player ? 0x10 : 0) | dp->type;
	NetServer::SendPacketToPlayer(dp, STOC_TYPE_CHANGE, sctc);
}
void RelayDuel::PlayerReady(DuelPlayer* dp, bool is_ready) {
	if(dp->type > 5 || players[dp->type].ready == is_ready)
		return;
	if(is_ready) {
		unsigned int deckerror = 0;
		if(!host_info.no_check_deck) {
			if(players[dp->type].deck_error) {
				deckerror = (DECKERROR_UNKNOWNCARD << 28) + players[dp->type].deck_error;
			} else {
				bool allow_ocg = host_info.rule == 0 || host_info.rule == 2;
				bool allow_tcg = host_info.rule == 1 || host_info.rule == 2;
				deckerror = deckManager.CheckDeck(players[dp->type].pdeck, host_info.lflist, allow_ocg, allow_tcg, host_info.extra_rules & DOUBLE_DECK, host_info.forbiddentypes);
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
	players[dp->type].ready = is_ready;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | (is_ready ? PLAYERCHANGE_READY : PLAYERCHANGE_NOTREADY);
	for (int i = 0; i < 6; i++)
		if (players[i].player)
			NetServer::SendPacketToPlayer(players[i].player, STOC_HS_PLAYER_CHANGE, scpc);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
}
void RelayDuel::PlayerKick(DuelPlayer* dp, unsigned char pos) {
	if(pos > 5 || dp != host_player || dp == players[pos].player || !players[pos].player)
		return;
	LeaveGame(players[pos].player);
}
void RelayDuel::UpdateDeck(DuelPlayer* dp, void* pdata) {
	if(dp->type > 5 || players[dp->type].ready)
		return;
	char* deckbuf = (char*)pdata;
	int mainc = BufferIO::ReadInt32(deckbuf);
	int sidec = BufferIO::ReadInt32(deckbuf);
	players[dp->type].deck_error = deckManager.LoadDeck(players[dp->type].pdeck, (int*)deckbuf, mainc, sidec, 0, 0, host_info.extra_rules & DOUBLE_DECK);
}
void RelayDuel::StartDuel(DuelPlayer* dp) {
	if(dp != host_player)
		return;
	for (int i = 0; i < 6; i++)
		if (players[i].player && !players[i].ready)
			return;
	if(!((players[0].ready || players[1].ready || players[2].ready) &&
		(players[3].ready || players[4].ready || players[5].ready)))
		return;
	for (int i = 0; i < 3; i++)
		if (!players[i].player) {
			endp[0]--;
			for (int j = i + 1; j < 3; j++)
				if (players[j].player) {
					STOC_HS_PlayerChange scpc;
					scpc.status = (j << 4) | i;
					for (int k = 0; k < 6; k++)
						if (players[k].player)
							NetServer::SendPacketToPlayer(players[k].player, STOC_HS_PLAYER_CHANGE, scpc);
					for (auto pit = observers.begin(); pit != observers.end(); ++pit)
						NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
					STOC_TypeChange sctc;
					sctc.type = (players[j].player == host_player ? 0x10 : 0) | i;
					NetServer::SendPacketToPlayer(players[j].player, STOC_TYPE_CHANGE, sctc);
					players[i] = players[j];
					players[j] = duelist();
					players[i].player->type = i;
				}
		}
	for (int i = 3; i < 6; i++)
		if (!players[i].player) {
			endp[1]--;
			for (int j = i + 1; j < 6; j++)
				if (players[j].player) {
					STOC_HS_PlayerChange scpc;
					scpc.status = (j << 4) | i;
					for (int k = 0; k < 6; k++)
						if (players[k].player)
							NetServer::SendPacketToPlayer(players[k].player, STOC_HS_PLAYER_CHANGE, scpc);
					for (auto pit = observers.begin(); pit != observers.end(); ++pit)
						NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
					STOC_TypeChange sctc;
					sctc.type = (players[j].player == host_player ? 0x10 : 0) | i;
					NetServer::SendPacketToPlayer(players[j].player, STOC_TYPE_CHANGE, sctc);
					players[i] = players[j];
					players[j] = duelist();
					players[i].player->type = i;
				}
		}
	NetServer::StopListen();
	game_started = true;
	//NetServer::StopBroadcast();
	for (int i = 0; i < 6; ++i)
		if (players[i].player)
			NetServer::SendPacketToPlayer(players[i].player, STOC_DUEL_START);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit) {
		(*oit)->state = CTOS_LEAVE_GAME;
		NetServer::ReSendToPlayer(*oit);
	}
	NetServer::SendPacketToPlayer(players[startp[0]].player, STOC_SELECT_HAND);
	NetServer::ReSendToPlayer(players[startp[1]].player);
	hand_result[0] = 0;
	hand_result[1] = 0;
	players[startp[0]].player->state = CTOS_HAND_RESULT;
	players[startp[1]].player->state = CTOS_HAND_RESULT;
}
void RelayDuel::HandResult(DuelPlayer* dp, unsigned char res) {
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
		for (int i = 0; i < 3; i++)
			if(players[i].player)
				NetServer::SendPacketToPlayer(players[i].player, STOC_HAND_RESULT, schr);
		for(auto oit = observers.begin(); oit != observers.end(); ++oit)
			NetServer::ReSendToPlayer(*oit);
		schr.res1 = hand_result[1];
		schr.res2 = hand_result[0];
		for (int i = 3; i < 6; i++)
			if (players[i].player)
				NetServer::SendPacketToPlayer(players[i].player, STOC_HAND_RESULT, schr);
		if(hand_result[0] == hand_result[1]) {
			NetServer::SendPacketToPlayer(players[startp[0]].player, STOC_SELECT_HAND);
			NetServer::ReSendToPlayer(players[startp[1]].player);
			hand_result[0] = 0;
			hand_result[1] = 0;
			players[startp[0]].player->state = CTOS_HAND_RESULT;
			players[startp[1]].player->state = CTOS_HAND_RESULT;
		} else if((hand_result[0] == 1 && hand_result[1] == 2)
		          || (hand_result[0] == 2 && hand_result[1] == 3)
		          || (hand_result[0] == 3 && hand_result[1] == 1)) {
			NetServer::SendPacketToPlayer(players[startp[1]].player, CTOS_TP_RESULT);
			players[startp[0]].player->state = 0xff;
			players[startp[1]].player->state = CTOS_TP_RESULT;
		} else {
			NetServer::SendPacketToPlayer(players[startp[0]].player, CTOS_TP_RESULT);
			players[startp[1]].player->state = 0xff;
			players[startp[0]].player->state = CTOS_TP_RESULT;
		}
	}
}
void RelayDuel::TPResult(DuelPlayer* dp, unsigned char tp) {
	if(dp->state != CTOS_TP_RESULT)
		return;
	bool swapped = false;
	mtrandom rnd;
	if((tp && dp->type == startp[1]) || (!tp && dp->type == startp[0])) {
		std::swap(players[0], players[3]);
		std::swap(players[1], players[4]);
		std::swap(players[2], players[5]);
		for (int i = 0; i < 6; i++)
			if(players[i].player)
				players[i].player->type = i;
		swapped = true;
	}
	turn_count = 0;
	cur_player[0] = players[startp[0]].player;
	cur_player[1] = players[startp[1]].player;
	dp->state = CTOS_RESPONSE;
	ReplayHeader rh;
	rh.id = 0x31707279;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_RELAY + REPLAY_LUA64;
	time_t seed = time(0);
	rh.seed = seed;
	last_replay.BeginRecord(false);
	last_replay.WriteHeader(rh);
	rnd.reset(seed);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.id = 0x58707279;
	rh.flag |= REPLAY_NEWREPLAY;
	new_replay.WriteHeader(rh);
	unsigned short tmp[20];
	BufferIO::CopyWStr(L"", tmp, 20);
	for (int i = 0; i < 6; i++)
		if (players[i].player) {
			last_replay.WriteData(players[i].player->name, 40, false);
			new_replay.WriteData(players[i].player->name, 40, false);
		} else {
			last_replay.WriteData(tmp, 40, false);
			new_replay.WriteData(tmp, 40, false);
		}
	replay_stream.clear();
	if(!host_info.no_shuffle_deck) {
		for(int p = 0; p < 6; p++)
			if (players[p].player)
				for(size_t i = players[p].pdeck.main.size() - 1; i > 0; --i) {
					int swap = rnd.real() * (i + 1);
					std::swap(players[p].pdeck.main[i], players[p].pdeck.main[swap]);
				}
	}
	time_limit[0] = host_info.time_limit;
	time_limit[1] = host_info.time_limit;
	set_script_reader(default_script_reader);
	set_card_reader((card_reader)DataManager::CardReader);
	set_message_handler((message_handler)RelayDuel::MessageHandler);
	rnd.reset(seed);
	pduel = create_duel(rnd.rand());
	set_player_info(pduel, 0, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	set_player_info(pduel, 1, host_info.start_lp, host_info.start_hand, host_info.draw_count);
	int opt = host_info.duel_flag;
	if(host_info.no_shuffle_deck)
		opt |= DUEL_PSEUDO_SHUFFLE;
	if(host_info.extra_rules & DUEL_SPEED)
		opt |= SPEED_DUEL;
	opt |= DUEL_RELAY_MODE;
	new_replay.WriteInt32((mainGame->GetMasterRule(opt, 0)) | (opt & SPEED_DUEL) << 8);
	last_replay.WriteInt32(host_info.start_lp, false);
	last_replay.WriteInt32(host_info.start_hand, false);
	last_replay.WriteInt32(host_info.draw_count, false);
	last_replay.WriteInt32(opt, false);
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
	last_replay.WriteInt32(players[startp[0]].pdeck.main.size(), false);
	for(int32 i = (int32)players[startp[0]].pdeck.main.size() - 1; i >= 0; --i) {
		new_card(pduel, players[startp[0]].pdeck.main[i]->first, 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(players[startp[0]].pdeck.main[i]->first, false);
	}
	last_replay.WriteInt32(players[startp[0]].pdeck.extra.size() + extracards.size(), false);
	for(int32 i = (int32)extracards.size() - 1; i >= 0; --i) {
		new_card(pduel, extracards[i], 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(extracards[i], false);
	}
	for(int32 i = (int32)players[startp[0]].pdeck.extra.size() - 1; i >= 0; --i) {
		new_card(pduel, players[startp[0]].pdeck.extra[i]->first, 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(players[startp[0]].pdeck.extra[i]->first, false);
	}
	//
	for(int p = startp[0] + 1, num = 1; p < 3; p++)
		if(players[p].player) {
			last_replay.WriteInt32(players[p].pdeck.main.size(), false);
			for (int32 i = (int32)players[p].pdeck.main.size() - 1; i >= 0; --i) {
				new_relay_card(pduel, players[p].pdeck.main[i]->first, 0, LOCATION_DECK, num);
				last_replay.WriteInt32(players[p].pdeck.main[i]->first, false);
			}
			last_replay.WriteInt32(players[p].pdeck.extra.size(), false);
			for (int32 i = (int32)players[p].pdeck.extra.size() - 1; i >= 0; --i) {
				new_relay_card(pduel, players[p].pdeck.extra[i]->first, 0, LOCATION_EXTRA, num);
				last_replay.WriteInt32(players[p].pdeck.extra[i]->first, false);
			}
			num++;
		}
	//
	last_replay.WriteInt32(players[startp[1]].pdeck.main.size(), false);
	for(int32 i = (int32)players[startp[1]].pdeck.main.size() - 1; i >= 0; --i) {
		new_card(pduel, players[startp[1]].pdeck.main[i]->first, 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(players[startp[1]].pdeck.main[i]->first, false);
	}
	last_replay.WriteInt32(players[startp[1]].pdeck.extra.size(), false);
	for(int32 i = (int32)players[startp[1]].pdeck.extra.size() - 1; i >= 0; --i) {
		new_card(pduel, players[startp[1]].pdeck.extra[i]->first, 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
		last_replay.WriteInt32(players[startp[1]].pdeck.extra[i]->first, false);
	}
	//
	for(int p = startp[1] + 1, num = 1; p < 6; p++)
		if(players[p].player) {
			last_replay.WriteInt32(players[p].pdeck.main.size(), false);
			for (int32 i = (int32)players[p].pdeck.main.size() - 1; i >= 0; --i) {
				new_relay_card(pduel, players[p].pdeck.main[i]->first, 1, LOCATION_DECK, num);
				last_replay.WriteInt32(players[p].pdeck.main[i]->first, false);
			}
			last_replay.WriteInt32(players[p].pdeck.extra.size(), false);
			for (int32 i = (int32)players[p].pdeck.extra.size() - 1; i >= 0; --i) {
				new_relay_card(pduel, players[p].pdeck.extra[i]->first, 1, LOCATION_EXTRA, num);
				last_replay.WriteInt32(players[p].pdeck.extra[i]->first, false);
			}
			num++;
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
	for (int i = 0; i < 3; i++)
		if (players[i].player)
			NetServer::SendBufferToPlayer(players[i].player, STOC_GAME_MSG, startbuf, 18);
	startbuf[1] = 1;
	for (int i = 3; i < 6; i++)
		if (players[i].player)
			NetServer::SendBufferToPlayer(players[i].player, STOC_GAME_MSG, startbuf, 18);
	if(!swapped)
		startbuf[1] = 0x10;
	else startbuf[1] = 0x11;
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::SendBufferToPlayer(*oit, STOC_GAME_MSG, startbuf, 18);
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
void RelayDuel::Process() {
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
void RelayDuel::DuelEndProc() {
	NetServer::SendPacketToPlayer(players[startp[0]].player, STOC_DUEL_END);
	for (int i = startp[0] + 1; i < 6; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
	NetServer::StopServer();
}
void RelayDuel::Surrender(DuelPlayer* dp) {
	return;
}
int RelayDuel::Analyze(char* msgbuffer, unsigned int len) {
	char* offset, *pbufw, *pbuf = msgbuffer;
	int player, count, type;
	while (pbuf - msgbuffer < (int)len) {
		replay_stream.clear();
		bool record = true;
		ReplayPacket pk;
		offset = pbuf;
		unsigned char engType = BufferIO::ReadUInt8(pbuf);
		pk.message = engType;
		pk.length = len - 1;
		memcpy(pk.data, pbuf, pk.length);
		switch (engType) {
		case MSG_RETRY: {
			WaitforResponse(last_response);
			NetServer::SendBufferToPlayer(cur_player[last_response], STOC_GAME_MSG, offset, pbuf - offset);
			replay_stream.push_back(pk);
			return 1;
		}
		case MSG_HINT: {
			type = BufferIO::ReadInt8(pbuf);
			player = BufferIO::ReadInt8(pbuf);
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
				for(int i = 0; i < 6; ++i)
					if(players[i].player != cur_player[player])
						NetServer::SendBufferToPlayer(players[i].player, STOC_GAME_MSG, offset, pbuf - offset);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			replay_stream.push_back(pk);
			EndDuel();
			return 2;
		}
		case MSG_SELECT_BATTLECMD: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 15;
			count = BufferIO::ReadInt8(pbuf);
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
			pbuf += count * 15 + 3;
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
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 16;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 8;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
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
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
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
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 10 + count * 17;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 9;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
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
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			if(pbuf[5] != LOCATION_DECK) {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
				for (int i = startp[0] + 1; i < 6; i++)
					if (players[i].player)
						NetServer::ReSendToPlayer(players[i].player);
				for(auto oit = observers.begin(); oit != observers.end(); ++oit)
					NetServer::ReSendToPlayer(*oit);
			} else {
				pbuf += count * 7;
				NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			}
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			PseudoRefreshDeck(player);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			for (int p = player * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			for (int p = (1 - player) * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshHand(player, 0x781fff, 0);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			for (int p = player * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, (pbuf - offset) + count * 4);
			for(int i = 0; i < count; ++i)
				BufferIO::WriteInt32(pbuf, 0);
			for (int p = (1 - player) * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshExtra(player);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			PseudoRefreshDeck(0);
			PseudoRefreshDeck(1);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 6;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshMzone(0, 0x181fff, 0);
			RefreshMzone(1, 0x181fff, 0);
			break;
		}
		case MSG_NEW_TURN: {
			pbuf++;
			time_limit[0] = host_info.time_limit;
			time_limit[1] = host_info.time_limit;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			turn_count++;
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			for (int p = cc * 3, i = 0; i < 3; i++, p++)
				if (players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			if (!(cl & (LOCATION_GRAVE + LOCATION_OVERLAY)) && ((cl & (LOCATION_DECK + LOCATION_HAND)) || (cp & POS_FACEDOWN)))
				BufferIO::WriteInt32(pbufw, 0);
			for (int p = (1 - cc) * 3, i = 0; i < 3; i++, p++)
				if (players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			if((pp & POS_FACEDOWN) && (cp & POS_FACEUP))
				RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_SET: {
			BufferIO::WriteInt32(pbuf, 0);
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshSingle(c1, l1, s1);
			RefreshSingle(c2, l2, s2);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SUMMONED: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_SPSUMMONED: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_FLIPSUMMONED: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshMzone(0);
			RefreshMzone(1);
			RefreshSzone(0);
			RefreshSzone(1);
			break;
		}
		case MSG_CHAINING: {
			pbuf += 20;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
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
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			for (int p = player * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = 0; i < count; ++i) {
				if(!(pbufw[3] & 0x80))
					BufferIO::WriteInt32(pbufw, 0);
				else
					pbufw += 4;
			}
			for (int p = (1 - player) * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 26;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ATTACK_DISABLED: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			RefreshMzone(0);
			RefreshMzone(1);
			break;
		}
		case MSG_MISSED_EFFECT: {
			player = pbuf[0];
			pbuf += 8;
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::ReadInt8(pbuf);
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for (auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_ANNOUNCE_NUMBER:
		case MSG_ANNOUNCE_CARD_FILTER: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 8 * count;
			WaitforResponse(player);
			NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, offset, pbuf - offset);
			return 1;
		}
		case MSG_CARD_HINT: {
			pbuf += 13;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 10;
			NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, offset, pbuf - offset);
			for (int i = startp[0] + 1; i < 6; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			break;
		}
		case MSG_TAG_SWAP: {
			player = BufferIO::ReadInt8(pbuf);
			/*int mcount = */BufferIO::ReadInt8(pbuf);
			int ecount = BufferIO::ReadInt8(pbuf);
			/*int pcount = */BufferIO::ReadInt8(pbuf);
			int hcount = BufferIO::ReadInt8(pbuf);
			pbufw = pbuf + 4;
			pbuf += hcount * 4 + ecount * 4 + 4;
			for (int p = player * 3, i = 0; i < 3; i++, p++)
				if (players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
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
			for (int p = (1 - player) * 3, i = 0; i < 3; i++, p++)
				if(players[p].player)
					NetServer::SendBufferToPlayer(players[p].player, STOC_GAME_MSG, offset, pbuf - offset);
			for(auto oit = observers.begin(); oit != observers.end(); ++oit)
				NetServer::ReSendToPlayer(*oit);
			for (int i = startp[player]; i < endp[player]; i++)
				if (players[i].player == cur_player[player]) {
					for (int j = i + 1; j < endp[player]; j++)
						if (players[j].player) {
							cur_player[player] = players[j].player;
							break;
						}
					break;
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
			pbuf += 4;
			break;
		}
		}
		//setting the length again in case of multiple messages in a row,
		//when the packet will be written in the replay, the extra data registered previously will be discarded
		pk.length = (pbuf - offset) - 1;
		if (record)
			replay_stream.insert(replay_stream.begin(), pk);
		new_replay.WriteStream(replay_stream);
	}
	return 0;
}
void RelayDuel::GetResponse(DuelPlayer* dp, void* pdata, unsigned int len) {
	byte resb[64];
	memcpy(resb, pdata, len);
	last_replay.WriteInt8(len);
	last_replay.WriteData(resb, len);
	set_responseb(pduel, resb);
	players[dp->type].player->state = 0xff;
	if(host_info.time_limit) {
		int resp_type = dp->type < 2 ? 0 : 1;
		if(time_limit[resp_type] >= time_elapsed)
			time_limit[resp_type] -= time_elapsed;
		else time_limit[resp_type] = 0;
		event_del(etimer);
	}
	Process();
}
void RelayDuel::EndDuel() {
	if(!pduel)
		return;
	last_replay.EndRecord(0x1000);
	char replaybuf[0x2000], *pbuf = replaybuf;
	memcpy(pbuf, &last_replay.pheader, sizeof(ReplayHeader));
	pbuf += sizeof(ReplayHeader);
	memcpy(pbuf, last_replay.comp_data, last_replay.comp_size);

	replay_stream.push_back(ReplayPacket(OLD_REPLAY_MODE, replaybuf, sizeof(ReplayHeader) + last_replay.comp_size));

	//in case of remaining packets, e.g. MSG_WIN
	new_replay.WriteStream(replay_stream);
	new_replay.EndRecord();
	char nreplaybuf[0x2000], *npbuf = nreplaybuf;
	memcpy(npbuf, &new_replay.pheader, sizeof(ReplayHeader));
	npbuf += sizeof(ReplayHeader);
	memcpy(npbuf, new_replay.comp_data, new_replay.comp_size);
	NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_NEW_REPLAY, nreplaybuf, sizeof(ReplayHeader) + new_replay.comp_size);
	for (int i = startp[0] + 1; i < 6; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for (auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);


	NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_REPLAY, replaybuf, sizeof(ReplayHeader) + last_replay.comp_size);
	for (int i = startp[0] + 1; i < 6; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for(auto oit = observers.begin(); oit != observers.end(); ++oit)
		NetServer::ReSendToPlayer(*oit);
	end_duel(pduel);
	pduel = 0;
}
void RelayDuel::WaitforResponse(int playerid) {
	last_response = playerid;
	unsigned char msg = MSG_WAITING;
	for(int i = 0; i < 6; ++i)
		if(players[i].player && players[i].player != cur_player[playerid])
			NetServer::SendPacketToPlayer(players[i].player, STOC_GAME_MSG, msg);
	if(host_info.time_limit) {
		STOC_TimeLimit sctl;
		sctl.player = playerid;
		sctl.left_time = time_limit[playerid];
		NetServer::SendPacketToPlayer(players[startp[0]].player, STOC_TIME_LIMIT, sctl);
		for (int i = startp[0] + 1; i < 6; i++)
			if (players[i].player)
				NetServer::ReSendToPlayer(players[i].player);
		cur_player[playerid]->state = CTOS_TIME_CONFIRM;
	} else
		cur_player[playerid]->state = CTOS_RESPONSE;
}
void RelayDuel::TimeConfirm(DuelPlayer* dp) {
	if(host_info.time_limit == 0)
		return;
	if(dp != cur_player[last_response])
		return;
	cur_player[last_response]->state = CTOS_RESPONSE;
	time_elapsed = 0;
	timeval timeout = {1, 0};
	event_add(etimer, &timeout);
}
void RelayDuel::RefreshMzone(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_MZONE);
	int len = query_field_card(pduel, player, LOCATION_MZONE, flag, (unsigned char*)qbuf, use_cache);
	int pid = startp[player];
	NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 3);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
	for (int i = pid + 1; i < endp[player]; i++)
		if(players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
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
	pid = startp[1 - player];
	NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 3);
	for (int i = pid + 1; i < endp[1 - player]; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void RelayDuel::RefreshSzone(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_SZONE);
	int len = query_field_card(pduel, player, LOCATION_SZONE, flag, (unsigned char*)qbuf, use_cache);
	int pid = startp[player];
	NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 3);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
	for (int i = pid + 1; i < endp[player]; i++)
		if(players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
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
	pid = startp[1 - player];
	NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 3);
	for (int i = pid + 1; i < endp[1 - player]; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void RelayDuel::RefreshHand(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_HAND);
	int len = query_field_card(pduel, player, LOCATION_HAND, flag | QUERY_IS_PUBLIC, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, query_buffer, len + 3);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
	int qlen = 0;
	while(qlen < len) {
		int slen = BufferIO::ReadInt32(qbuf);
		int qflag = *(int*)qbuf;
		int pos = slen - 8;
		if(qflag & QUERY_LSCALE)
			pos -= 4;
		if(qflag & QUERY_RSCALE)
			pos -= 4;
		if(!qbuf[pos])
			memset(qbuf, 0, slen - 4);
		qbuf += slen - 4;
		qlen += slen;
	}
	for(int i = 0; i < 6; ++i)
		if(players[i].player && players[i].player != cur_player[player])
			NetServer::SendBufferToPlayer(players[i].player, STOC_GAME_MSG, query_buffer, len + 3);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
}
void RelayDuel::RefreshGrave(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_GRAVE);
	int len = query_field_card(pduel, player, LOCATION_GRAVE, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(players[startp[0]].player, STOC_GAME_MSG, query_buffer, len + 3);
	for (int i = startp[0] + 1; i < 6; i++)
		if (players[i].player)
			NetServer::ReSendToPlayer(players[i].player);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::ReSendToPlayer(*pit);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
}
void RelayDuel::RefreshExtra(int player, int flag, int use_cache) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_EXTRA);
	int len = query_field_card(pduel, player, LOCATION_EXTRA, flag, (unsigned char*)qbuf, use_cache);
	NetServer::SendBufferToPlayer(cur_player[player], STOC_GAME_MSG, query_buffer, len + 3);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
}
void RelayDuel::RefreshSingle(int player, int location, int sequence, int flag) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_CARD);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, location);
	BufferIO::WriteInt8(qbuf, sequence);
	int len = query_card(pduel, player, location, sequence, flag, (unsigned char*)qbuf, 0);
	ReplayPacket p((char*)query_buffer, len + 3);
	replay_stream.push_back(p);
	if(location & LOCATION_ONFIELD) {
		int pid = startp[player];
		NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 4);
		for (int i = pid + 1; i < endp[player]; i++)
			if (players[i].player)
				NetServer::ReSendToPlayer(players[i].player);
		if(qbuf[15] & POS_FACEUP) {
			for (int i = startp[1 - player]; i < endp[1 - player]; i++)
				if (players[i].player)
					NetServer::ReSendToPlayer(players[i].player);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::ReSendToPlayer(*pit);
		}
	} else {
		int pid = startp[player];
		NetServer::SendBufferToPlayer(players[pid].player, STOC_GAME_MSG, query_buffer, len + 4);
		for (int i = pid + 1; i < endp[player]; i++)
			if (players[i].player)
				NetServer::ReSendToPlayer(players[i].player);
		if(location == LOCATION_REMOVED && (qbuf[15] & POS_FACEDOWN))
			return;
		if (location & 0x90) {
			for(int i = 0; i < 6; ++i)
				if(players[i].player && players[i].player != cur_player[player])
					NetServer::ReSendToPlayer(players[i].player);
			for(auto pit = observers.begin(); pit != observers.end(); ++pit)
				NetServer::ReSendToPlayer(*pit);
		}
	}
}
void RelayDuel::PseudoRefreshDeck(int player, int flag) {
	char query_buffer[0x2000];
	char* qbuf = query_buffer;
	BufferIO::WriteInt8(qbuf, MSG_UPDATE_DATA);
	BufferIO::WriteInt8(qbuf, player);
	BufferIO::WriteInt8(qbuf, LOCATION_DECK);
	int len = query_field_card(pduel, player, LOCATION_DECK, flag, (unsigned char*)qbuf, 0);
	ReplayPacket p((char*)query_buffer, len + 2);
	replay_stream.push_back(p);
}
int RelayDuel::MessageHandler(long fduel, int type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, (byte*)msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}
void RelayDuel::RelayTimer(evutil_socket_t fd, short events, void* arg) {
	RelayDuel* sd = static_cast<RelayDuel*>(arg);
	sd->time_elapsed++;
	if(sd->time_elapsed >= sd->time_limit[sd->last_response]) {
		unsigned char wbuf[3];
		uint32 player = sd->last_response;
		wbuf[0] = MSG_WIN;
		wbuf[1] = 1 - player;
		wbuf[2] = 0x3;
		NetServer::SendBufferToPlayer(sd->players[sd->startp[0]].player, STOC_GAME_MSG, wbuf, 3);
		for (int i = sd->startp[0] + 1; i < 6; i++)
			if (sd->players[i].player)
				NetServer::ReSendToPlayer(sd->players[i].player);
		ReplayPacket p((char*)wbuf, 3);
		sd->replay_stream.push_back(p);
		sd->EndDuel();
		sd->DuelEndProc();
		event_del(sd->etimer);
	}
}

}
