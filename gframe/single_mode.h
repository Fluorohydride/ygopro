#ifndef SINGLE_MODE_H
#define SINGLE_MODE_H

#include "replay.h"

namespace ygo {

class SingleMode {
private:
	static long pduel;
	static bool is_closing;
	static bool is_continuing;

public:
	static bool StartPlay();
	static void StopPlay(bool is_exiting = false);
	static void SetResponse(unsigned char* resp, unsigned int len);
	static int SinglePlayThread(void* param);
	static bool SinglePlayAnalyze(char* msg, unsigned int len);
	
	static void SinglePlayRefresh(int player, int location, int flag = 0xf81fff);
	static void SinglePlayRefresh(int flag = 0xf81fff);
	static void SinglePlayRefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);
	static void SinglePlayReload();

	static byte* ScriptReader(const char* script_name, int* slen);
	static int MessageHandler(long fduel, int type);

protected:
	static Replay last_replay;
	static Replay new_replay;
	static std::vector<ReplayPacket> replay_stream;
};

}

#endif //SINGLE_MODE_H
