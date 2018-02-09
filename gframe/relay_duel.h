#ifndef RELAY_DUEL_H
#define RELAY_DUEL_H

#include "config.h"
#include "network.h"
#include "replay.h"

namespace ygo {

class RelayDuel: public DuelMode {
public:
	RelayDuel();
	virtual ~RelayDuel();
	virtual void Chat(DuelPlayer* dp, void* pdata, int len);
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater);
	virtual void LeaveGame(DuelPlayer* dp);
	virtual void ToDuelist(DuelPlayer* dp);
	virtual void ToObserver(DuelPlayer* dp);
	virtual void PlayerReady(DuelPlayer* dp, bool ready);
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos);
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata);
	virtual void StartDuel(DuelPlayer* dp);
	virtual void HandResult(DuelPlayer* dp, unsigned char res);
	virtual void TPResult(DuelPlayer* dp, unsigned char tp);
	virtual void Process();
	virtual void Surrender(DuelPlayer* dp);
	virtual int Analyze(char* msgbuffer, unsigned int len);
	virtual void GetResponse(DuelPlayer* dp, void* pdata, unsigned int len);
	virtual void TimeConfirm(DuelPlayer* dp);
	virtual void EndDuel();
	
	void DuelEndProc();
	void WaitforResponse(int playerid);
	void RefreshMzone(int player, int flag = 0x881fff, int use_cache = 1);
	void RefreshSzone(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshHand(int player, int flag = 0x781fff, int use_cache = 1);
	void RefreshGrave(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshExtra(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);
	
	static int MessageHandler(long fduel, int type);
	static void RelayTimer(evutil_socket_t fd, short events, void* arg);

	void PseudoRefreshDeck(int player, int flag = 0x181fff);
	static std::vector<ReplayPacket> replay_stream;
	
protected:
	class duelist {
		public:
		DuelPlayer* player;
		bool ready;
		Deck pdeck;
		int deck_error;
		duelist() : player(0), ready(false), deck_error(0) {}
		duelist(DuelPlayer* _player) : player(_player), ready(false), deck_error(0) {}
	};
	duelist players[6];
	unsigned char startp[2];
	unsigned char endp[2];
	DuelPlayer* cur_player[2];
	std::set<DuelPlayer*> observers;
	unsigned char hand_result[2];
	unsigned char last_response;
	Replay last_replay;
	Replay new_replay;
	bool game_started;
	unsigned char turn_count;
	unsigned short time_limit[2];
	unsigned short time_elapsed;
};

}

#endif //RELAY_DUEL_H

