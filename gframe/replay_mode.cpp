#include "replay_mode.h"

namespace ygo {

Replay ReplayMode::cur_replay;
bool ReplayMode::is_closing;
wchar_t ReplayMode::event_string[256];
ReplayMode replayMode;

bool ReplayMode::StartReplay() {
	return false;
}
void ReplayMode::StopReplay(bool is_exiting) {
	
}
int ReplayMode::ReplayThread(void* param) {
	return 0;
}
int ReplayMode::ReplayAnalyze(char* msg, unsigned int len) {
	return 0;
}
void ReplayMode::SetResponseI(int respI) {
	
}
void ReplayMode::SetResponseB(unsigned char* respB, unsigned char len) {
	
}

}
