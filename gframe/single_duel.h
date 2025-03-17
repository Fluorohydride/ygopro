#ifndef SINGLE_DUEL_H
#define SINGLE_DUEL_H

#include <set>
#include "network.h"
#include "deck_manager.h"
#include "replay.h"

namespace ygo {

class SingleDuel: public DuelMode {
public:
	SingleDuel(bool is_match);
	~SingleDuel() override;
	void Chat(DuelPlayer* dp, unsigned char* pdata, int len) override;
	void JoinGame(DuelPlayer* dp, unsigned char* pdata, bool is_creater) override;
	void LeaveGame(DuelPlayer* dp) override;
	void ToDuelist(DuelPlayer* dp) override;
	void ToObserver(DuelPlayer* dp) override;
	void PlayerReady(DuelPlayer* dp, bool ready) override;
	void PlayerKick(DuelPlayer* dp, unsigned char pos) override;
	void UpdateDeck(DuelPlayer* dp, unsigned char* pdata, int len) override;
	void StartDuel(DuelPlayer* dp) override;
	void HandResult(DuelPlayer* dp, unsigned char res) override;
	void TPResult(DuelPlayer* dp, unsigned char tp) override;
	void Process() override;
	void Surrender(DuelPlayer* dp) override;
	int Analyze(unsigned char* msgbuffer, unsigned int len) override;
	void GetResponse(DuelPlayer* dp, unsigned char* pdata, unsigned int len) override;
	void TimeConfirm(DuelPlayer* dp) override;
	void EndDuel() override;
	
	void DuelEndProc();
	void WaitforResponse(int playerid);
	void RefreshMzone(int player, int flag = 0x881fff, int use_cache = 1);
	void RefreshSzone(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshHand(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshGrave(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshExtra(int player, int flag = 0xe81fff, int use_cache = 1);
	void RefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);

	static uint32_t MessageHandler(intptr_t fduel, uint32_t type);
	static void SingleTimer(evutil_socket_t fd, short events, void* arg);

private:
	int WriteUpdateData(int& player, int location, int& flag, unsigned char*& qbuf, int& use_cache);
	
protected:
	DuelPlayer* players[2]{};
	DuelPlayer* pplayer[2]{};
	bool ready[2]{};
	Deck pdeck[2];
	int deck_error[2]{};
	unsigned char hand_result[2]{};
	unsigned char last_response{ 0 };
	std::set<DuelPlayer*> observers;
	Replay last_replay;
	bool match_mode{ false };
	int match_kill{ 0 };
	unsigned char duel_count{ 0 };
	unsigned char tp_player{ 0 };
	unsigned char match_result[3]{};
	short time_limit[2]{};
	short time_elapsed{ 0 };
};

}

#endif //SINGLE_DUEL_H

