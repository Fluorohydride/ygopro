#ifndef SINGLE_DUEL_H
#define SINGLE_DUEL_H

#include "config.h"
#include "network.h"
#include "replay.h"

namespace ygo {

class SingleDuel: public DuelMode {
public:
	SingleDuel(bool is_match);
	virtual ~SingleDuel();
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
	void RefreshMzone(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshSzone(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshHand(int player, int flag = 0x781fff, int use_cache = 1);
	void RefreshGrave(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshExtra(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshSingle(int player, int location, int sequence, int flag = 0x781fff);
	
	static int MessageHandler(long fduel, int type);
	static void SingleTimer(evutil_socket_t fd, short events, void* arg);
	
protected:
	DuelPlayer* players[2];
	DuelPlayer* pplayer[2];
	bool ready[2];
	Deck pdeck[2];
	unsigned char hand_result[2];
	unsigned char last_response;
	std::set<DuelPlayer*> observers;
	Replay last_replay;
	bool match_mode;
	int match_kill;
	unsigned char duel_count;
	unsigned char tp_player;
	unsigned char match_result[3];
	unsigned short time_limit[2];
	unsigned short time_elapsed;
};

}

#endif //SINGLE_DUEL_H

