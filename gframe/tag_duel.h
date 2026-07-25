#ifndef TAG_DUEL_H
#define TAG_DUEL_H

#include <set>
#include "network.h"
#include "deck.h"
#include "replay.h"

namespace ygo {

class TagDuel: public DuelMode {
public:
	TagDuel();
	~TagDuel() override;
	void Chat(DuelPlayer* dp, unsigned char* pdata, int len) override;
	void JoinGame(DuelPlayer* dp, unsigned char* pdata, bool is_creater) override;
	void LeaveGame(DuelPlayer* dp) override;
	void ToDuelist(DuelPlayer* dp) override;
	void ToObserver(DuelPlayer* dp) override;
	void PlayerReady(DuelPlayer* dp, bool is_ready) override;
	void PlayerKick(DuelPlayer* dp, unsigned char pos) override;
	void UpdateDeck(DuelPlayer* dp, unsigned char* pdata, unsigned int len) override;
	void StartDuel(DuelPlayer* dp) override;
	void HandResult(DuelPlayer* dp, unsigned char res) override;
	void TPResult(DuelPlayer* dp, unsigned char tp) override;
	void Process() override;
	void Surrender(DuelPlayer* dp) override;
	int Analyze(unsigned char* msgbuffer, unsigned int len) override;
	void GetResponse(DuelPlayer* dp, unsigned char* pdata, unsigned int len) override;
	void TimeConfirm(DuelPlayer* dp) override;
	void TimerTick() override;
	void EndDuel() override;
	void OnPlayerDisconnected(DuelPlayer* dp) override;
	
	void DuelEndProc();
	void WaitforResponse(int playerid);
	void RefreshMzone(int player, int flag = 0x881fff, int use_cache = 1);
	void RefreshSzone(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshHand(int player, int flag = 0x681fff, int use_cache = 1);
	void RefreshGrave(int player, int flag = 0x81fff, int use_cache = 1);
	void RefreshExtra(int player, int flag = 0xe81fff, int use_cache = 1);
	void RefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);

	static uint32_t MessageHandler(intptr_t fduel, uint32_t type);
private:
	int WriteUpdateData(int player, int location, unsigned int flag, unsigned char*& qbuf, int use_cache);
	
protected:
	DuelPlayer* players[4]{};
	DuelPlayer* pplayer[4]{};
	DuelPlayer* cur_player[2]{};
	std::set<DuelPlayer*> observers;
	bool ready[4]{};
	bool surrender[4]{};
	Deck pdeck[4];
	int deck_error[4]{};
	unsigned char hand_result[2]{};
	unsigned char last_response{ 0 };
	Replay last_replay;
	size_t last_replay_response_size{ 0 };
	unsigned char turn_count{ 0 };
	uint16_t time_limit[2]{};
	uint16_t time_elapsed{ 0 };
};

}

#endif //TAG_DUEL_H
