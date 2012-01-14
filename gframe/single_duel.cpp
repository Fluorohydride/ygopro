#include "single_duel.h"
#include "netserver.h"

namespace ygo {

void SingleDuel::JoinGame(DuelPlayer* dp, void* pdata, bool is_creater) {
	if(!is_creater) {
		if(dp->game && dp->type != 0xff) {
			STOC_JoinFail scjf;
			scjf.reason = 0;
			NetServer::SendPacketToPlayer(dp, STOC_JOIN_FAIL, scjf);
			return;
		}
		CTOS_JoinGame* pkt = (CTOS_JoinGame*)pdata;
		wchar_t jpass[20];
		for(int i = 0; i < 20; ++i) jpass[i] = pkt->pass[i];
		jpass[20] = 0;
		if(wcscmp(jpass, pass)) {
			STOC_JoinFail scjf;
			scjf.reason = 1;
			NetServer::SendPacketToPlayer(dp, STOC_JOIN_FAIL, scjf);
			return;
		}
	}
	dp->game = this;
	if(!players[0] && !players[1] && observers.size() == 0)
		host_player = dp;
	STOC_JoinGame scjg;
	scjg.info = host_info;
	scjg.type = (host_player == dp) ? 0x10 : 0;
	if(!players[0] || !players[1]) {
		STOC_HS_PlayerEnter scpe;
		BufferIO::CopyWStr(dp->name, scpe.name, 20);
		if(players[0]) {
			scpe.pos = 1;
			NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_ENTER, scpe);
		}
		if(players[1]) {
			scpe.pos = 0;
			NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_ENTER, scpe);
		}
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_ENTER, scpe);
		if(!players[0]) {
			players[0] = dp;
			dp->type = NETPLAYER_TYPE_PLAYER1;
			scjg.type |= NETPLAYER_TYPE_PLAYER1;
		} else {
			players[1] = dp;
			dp->type = NETPLAYER_TYPE_PLAYER2;
			scjg.type |= NETPLAYER_TYPE_PLAYER2;
		}
	} else {
		observers.insert(dp);
		dp->type = NETPLAYER_TYPE_OBSERVER;
		scjg.type |= NETPLAYER_TYPE_OBSERVER;
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
	if(dp == host_player){
		NetServer::StopServer();
	} else if(dp->type == NETPLAYER_TYPE_OBSERVER) {
		if(!pduel) {
			STOC_HS_WatchChange scwc;
			scwc.watch_count = observers.size();
		}
		NetServer::DisconnectPlayer(dp);
	} else {
		STOC_HS_PlayerChange scpc;
		players[dp->type] = 0;
		scpc.status = (dp->type << 4) | PLAYERCHANGE_LEAVE;
		if(players[0] && dp->type != 0)
			NetServer::SendPacketToPlayer(players[0], STOC_HS_PLAYER_CHANGE, scpc);
		if(players[1] && dp->type != 1)
			NetServer::SendPacketToPlayer(players[1], STOC_HS_PLAYER_CHANGE, scpc);
		for(auto pit = observers.begin(); pit != observers.end(); ++pit)
			NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
		if(pduel)
			NetServer::StopServer();
		else
			NetServer::DisconnectPlayer(dp);
	}
}
void SingleDuel::ToDuelist(DuelPlayer* dp) {
	if(dp->type != NETPLAYER_TYPE_OBSERVER)
		return;
}
void SingleDuel::ToObserver(DuelPlayer* dp) {
	if(dp->type > 1)
		return;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | PLAYERCHANGE_OBSERVE;
}
void SingleDuel::PlayerReady(DuelPlayer* dp) {
	if(dp->type > 1)
		return;
	ready[dp->type] = !ready[dp->type] ;
	STOC_HS_PlayerChange scpc;
	scpc.status = (dp->type << 4) | (ready[dp->type] ? PLAYERCHANGE_READY : PLAYERCHANGE_NOTREADY);
	if(players[1 - dp->type])
		NetServer::SendPacketToPlayer(players[1 - dp->type], STOC_HS_PLAYER_CHANGE, scpc);
	for(auto pit = observers.begin(); pit != observers.end(); ++pit)
		NetServer::SendPacketToPlayer(*pit, STOC_HS_PLAYER_CHANGE, scpc);
}
void SingleDuel::PlayerKick(DuelPlayer* dp, unsigned char pos) {
	if(dp != host_player || dp == players[pos] || !players[pos])
		return;
	LeaveGame(players[pos]);
}
void SingleDuel::UpdateDeck(DuelPlayer* dp, void* pdata) {
}
void SingleDuel::StartDuel(DuelPlayer* dp) {
}
void SingleDuel::Process() {
}
void SingleDuel::EndDuel() {
}

}
