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
	static bool is_closing;
	static wchar_t event_string[256];
public:
	static Replay cur_replay;
	
public:
	static bool StartReplay();
	static void StopReplay(bool is_exiting = false);
	static int ReplayThread(void* param);
	static int ReplayAnalyze(char* msg, unsigned int len);
	static void SetResponseI(int respI);
	static void SetResponseB(unsigned char* respB, unsigned char len);
	
};

extern ReplayMode replayMode;

}

#endif //REPLAY_MODE_H
