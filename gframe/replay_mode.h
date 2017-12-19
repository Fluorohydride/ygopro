#ifndef REPLAY_MODE_H
#define REPLAY_MODE_H

#include "config.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "replay.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

class ReplayMode {
private:
	static long pduel;
	static bool yrp;
	static bool is_continuing;
	static bool is_closing;
	static bool is_pausing;
	static bool is_paused;
	static bool is_swapping;
	static bool is_restarting;
	static bool undo;
	static bool exit_pending;
	static int skip_turn;
	static int current_step;
	static int skip_step;

public:
	static Replay cur_replay;
	static std::vector<ReplayPacket> ReplayMode::current_stream;
	
public:
	static bool StartReplay(int skipturn);
	static void StopReplay(bool is_exiting = false);
	static void SwapField();
	static void Pause(bool is_pause, bool is_step);
	static bool ReadReplayResponse();
	static int ReplayThread(void* param);
	static int OldReplayThread(void* param);
	static bool StartDuel();
	static void EndDuel();
	static void Restart(bool refresh);
	static void Undo();
	static bool ReplayAnalyze(ReplayPacket p);
	static bool ReplayAnalyze(char* msg, unsigned int len);

	static void ReplayRefresh(int player, int location, int flag = 0xf81fff);
	static void ReplayRefresh(int flag = 0xf81fff);
	static void ReplayRefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);
	static void ReplayReload();

	static int MessageHandler(long fduel, int type);
};

}

#endif //REPLAY_MODE_H
