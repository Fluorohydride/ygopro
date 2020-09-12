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
		uint32_t startingLP = 8000;
		uint32_t startingDrawCount = 5;
		uint32_t drawCountPerTurn = 1;
		uint32_t duelFlags = 0;
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
	
	static void SinglePlayRefresh(uint8_t player, uint8_t location, uint32_t flag = 0x2f81fff);
	static void SinglePlayRefresh(uint32_t flag = 0x2f81fff);
	static void SinglePlayRefreshSingle(uint8_t player, uint8_t location, uint8_t sequence, uint32_t flag = 0x2f81fff);
	static void SinglePlayReload();
	static Signal singleSignal;

protected:
	static Replay last_replay;
	static Replay new_replay;
	static ReplayStream replay_stream;
};

}

#endif //SINGLE_MODE_H
