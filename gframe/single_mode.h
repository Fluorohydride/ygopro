#ifndef SINGLE_MODE_H
#define SINGLE_MODE_H

#include "dllinterface.h"
#include "replay.h"
#include "mysignal.h"
#include "core_utils.h"
#include "text_types.h"

namespace ygo {

class SingleMode {
private:
	static OCG_Duel pduel;
	static bool is_closing;
	static bool is_continuing;
	static bool is_restarting;

public:
	struct DuelOptions {
		uint32_t startingLP{ 8000 };
		uint32_t startingDrawCount{ 5 };
		uint32_t drawCountPerTurn{ 1 };
		uint64_t duelFlags{ 0 };
		bool handTestNoOpponent{ true };
		std::string scriptName;
		DuelOptions() {};
		explicit DuelOptions(epro::stringview filename) : scriptName(filename.data(), filename.size()) {};
	};

	static bool StartPlay(DuelOptions&& duelOptions);
	static void StopPlay(bool is_exiting = false);
	static void Restart();
	static void SetResponse(void* resp, uint32_t len);
	static int SinglePlayThread(DuelOptions&& duelOptions);
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
