#ifndef SINGLE_MODE_H
#define SINGLE_MODE_H

#include "dllinterface.h"
#include "replay.h"
#include "mysignal.h"
#include "core_utils.h"

namespace ygo {

class SingleMode {
private:
	static OCG_Duel pduel;
	static bool is_closing;
	static bool is_continuing;
	static bool is_restarting;

public:
	struct DuelOptions {
		int startingLP = 8000;
		int startingDrawCount = 5;
		int drawCountPerTurn = 1;
		int duelFlags = 0;
		bool handTestNoOpponent = true;
		bool handTestNoShuffle = false;
		DuelOptions() {};
	};

	static bool StartPlay(const DuelOptions& duelOptions = DuelOptions());
	static void StopPlay(bool is_exiting = false);
	static void Restart();
	static void SetResponse(unsigned char* resp, unsigned int len);
	static int SinglePlayThread(DuelOptions duelOptions);
	static bool SinglePlayAnalyze(CoreUtils::Packet packet);
	
	static void SinglePlayRefresh(int player, int location, int flag = 0x2f81fff);
	static void SinglePlayRefresh(int flag = 0x2f81fff);
	static void SinglePlayRefreshSingle(int player, int location, int sequence, int flag = 0x2f81fff);
	static void SinglePlayReload();
	static Signal singleSignal;

protected:
	static Replay last_replay;
	static Replay new_replay;
	static ReplayStream replay_stream;
};

}

#endif //SINGLE_MODE_H
