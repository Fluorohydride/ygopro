#ifndef MATCH_DUEL_H
#define MATCH_DUEL_H

#include "config.h"
#include "network.h"

namespace ygo {

class MatchDuel: public DuelMode {
public:
	virtual void JoinGame(DuelPlayer* dp, void* pdata, bool is_creater);
	virtual void LeaveGame(DuelPlayer* dp);
	virtual void ToDuelist(DuelPlayer* dp);
	virtual void ToObserver(DuelPlayer* dp);
	virtual void PlayerReady(DuelPlayer* dp);
	virtual void PlayerKick(DuelPlayer* dp, unsigned char pos);
	virtual void UpdateDeck(DuelPlayer* dp, void* pdata);
	virtual void StartDuel(DuelPlayer* dp);
	virtual void Process();
	virtual void EndDuel();
};

}

#endif //MATCH_DUEL_H

