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
	static int SinglePlayThread();
	static bool SinglePlayAnalyze(char* msg, unsigned int len);
	
	static void SinglePlayRefresh(int flag = 0xf81fff);
	static void SinglePlayRefreshHand(int player, int flag = 0x781fff);
	static void SinglePlayRefreshGrave(int player, int flag = 0x181fff);
	static void SinglePlayRefreshDeck(int player, int flag = 0x181fff);
	static void SinglePlayRefreshExtra(int player, int flag = 0x181fff);
	static void SinglePlayRefreshSingle(int player, int location, int sequence, int flag = 0xf81fff);
	static void SinglePlayReload();

	static int MessageHandler(long fduel, int type);

protected:
	static Replay last_replay;
};

}

#endif //SINGLE_MODE_H
