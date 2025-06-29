#ifndef REPLAY_MODE_H
#define REPLAY_MODE_H

#include <cstdint>
#include <vector>
#include "replay.h"

namespace ygo {

class ReplayMode {
private:
	static intptr_t pduel;
	static bool is_continuing;
	static bool is_closing;
	static bool is_pausing;
	static bool is_paused;
	static bool is_swaping;
	static bool is_restarting;
	static bool exit_pending;
	static int skip_turn;
	static int current_step;
	static int skip_step;
	static void ReloadLocation(int player, int location, int flag, std::vector<unsigned char>& queryBuffer);

public:
	static Replay cur_replay;
	
	static void LoadReplay(const wchar_t* replay_path, int start_turn = 0);
	static bool StartReplay(int skipturn);
	static void StopReplay(bool is_exiting = false);
	static void SwapField();
	static void Pause(bool is_pause, bool is_step);
	static bool ReadReplayResponse();
	static int ReplayThread();
	static bool StartDuel();
	static void EndDuel();
	static void Restart(bool refresh);
	static void Undo();
	static bool ReplayAnalyze(unsigned char* msg, unsigned int len);
	
	static void ReplayRefresh(int flag = 0xf81fff);
	static void ReplayRefreshLocation(int player, int location, int flag);
	static void ReplayRefreshHand(int player, int flag = 0x781fff);
	static void ReplayRefreshGrave(int player, int flag = 0x181fff);
	static void ReplayRefreshDeck(int player, int flag = 0x181fff);
	static void ReplayRefreshExtra(int player, int flag = 0x181fff);
	static void ReplayRefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);
	static void ReplayReload();

	static uint32_t MessageHandler(intptr_t fduel, uint32_t type);
};

}

#endif //REPLAY_MODE_H
