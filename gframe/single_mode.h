#ifndef SINGLE_MODE_H
#define SINGLE_MODE_H

#include "config.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

class SingleMode {
private:
	static long pduel;
	static bool is_closing;
	static bool is_continuing;

public:
	static bool StartPlay();
	static void StopPlay(bool is_exiting = false);
	static void SetResponse(unsigned char* resp);
	static int SinglePlayThread(void* param);
	static bool SinglePlayAnalyze(char* msg, unsigned int len);
	
	static void SinglePlayRefresh(int flag = 0x781fff);
	static void SinglePlayRefreshHand(int player, int flag = 0x781fff);
	static void SinglePlayRefreshGrave(int player, int flag = 0x181fff);
	static void SinglePlayRefreshDeck(int player, int flag = 0x181fff);
	static void SinglePlayRefreshExtra(int player, int flag = 0x181fff);
	static void SinglePlayRefreshSingle(int player, int location, int sequence, int flag = 0x781fff);
	static void SinglePlayReload();

	static int MessageHandler(long fduel, int type);
};

}

#endif //SINGLE_MODE_H
