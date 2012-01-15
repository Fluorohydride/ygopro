#ifndef SINGLE_DUEL_H
#define SINGLE_DUEL_H

#include "config.h"
#include "network.h"

namespace ygo {

class SingleDuel: public DuelMode {
public:
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater);
	virtual void LeaveGame(DuelPlayer* dp);
	virtual void ToDuelist(DuelPlayer* dp);
	virtual void ToObserver(DuelPlayer* dp);
	virtual void PlayerReady(DuelPlayer* dp, bool ready);
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos);
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata);
	virtual void StartDuel(DuelPlayer* dp);
	virtual void Process();
	virtual void EndDuel();
	
	void RefreshMzone(int player, int flag = 0x181fff, int use_cache = 1);
	void RefreshSzone(int player, int flag = 0x181fff, int use_cache = 1);
	void RefreshHand(int player, int flag = 0x181fff, int use_cache = 1);
	void RefreshGrave(int player, int flag = 0x181fff, int use_cache = 1);
	void RefreshExtra(int player, int flag = 0x181fff, int use_cache = 1);
	void RefreshSingle(int player, int location, int sequence, int flag = 0x181fff);
	
	static int MessageHandler(long fduel, int type);
protected:
	unsigned char player_mapping[2];
};

}

#endif //SINGLE_DUEL_H

