#include "replay_mode.h"
#include "duelclient.h"
#include "game.h"
#include "single_mode.h"
#include "../ocgcore/duel.h"
#include "../ocgcore/field.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

long ReplayMode::pduel = 0;
bool ReplayMode::yrp = false;
Replay ReplayMode::cur_replay;
std::vector<ReplayPacket> ReplayMode::current_stream;
bool ReplayMode::is_continuing = true;
bool ReplayMode::is_closing = false;
bool ReplayMode::is_pausing = false;
bool ReplayMode::is_paused = false;
bool ReplayMode::is_swapping = false;
bool ReplayMode::is_restarting = false;
bool ReplayMode::exit_pending = false;
int ReplayMode::skip_turn = 0;
int ReplayMode::current_step = 0;
int ReplayMode::skip_step = 0;

bool ReplayMode::StartReplay(int skipturn) {
	skip_turn = skipturn;
	if(skip_turn < 0)
		skip_turn = 0; 
	yrp = cur_replay.pheader.id == 0x31707279;
	if(yrp)
		Thread::NewThread(OldReplayThread, 0);
	else
		Thread::NewThread(ReplayThread, 0);
	return true;
}
void ReplayMode::StopReplay(bool is_exiting) {
	is_pausing = false;
	is_continuing = false;
	is_closing = is_exiting;
	exit_pending = true;
	mainGame->actionSignal.Set();
}
void ReplayMode::SwapField() {
	if(is_paused)
		mainGame->dField.ReplaySwap();
	else
		is_swapping = true;
}
void ReplayMode::Pause(bool is_pause, bool is_step) {
	if(is_pause)
		is_pausing = true;
	else {
		if(!is_step)
			is_pausing = false;
		mainGame->actionSignal.Set();
	}
}
int ReplayMode::ReplayThread(void* param) {
	const ReplayHeader& rh = cur_replay.pheader;
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isTag = !!(rh.flag & REPLAY_TAG);
	mainGame->dInfo.isRelay = !!(rh.flag & REPLAY_RELAY);
	mainGame->dInfo.isSingleMode = !!(rh.flag & REPLAY_SINGLE_MODE);
	mainGame->dInfo.lua64 = !!(rh.flag & REPLAY_LUA64);
	mainGame->dInfo.current_player[0] = 0;
	mainGame->dInfo.current_player[1] = 0;
	if (mainGame->dInfo.isRelay) {
		cur_replay.ReadName(mainGame->dInfo.hostname[0]);
		cur_replay.ReadName(mainGame->dInfo.hostname[1]);
		cur_replay.ReadName(mainGame->dInfo.hostname[2]);
		cur_replay.ReadName(mainGame->dInfo.clientname[0]);
		cur_replay.ReadName(mainGame->dInfo.clientname[1]);
		cur_replay.ReadName(mainGame->dInfo.clientname[2]);
	} else if (mainGame->dInfo.isTag) {
		cur_replay.ReadName(mainGame->dInfo.hostname[0]);
		cur_replay.ReadName(mainGame->dInfo.hostname[1]);
		cur_replay.ReadName(mainGame->dInfo.clientname[1]);
		cur_replay.ReadName(mainGame->dInfo.clientname[0]);
	} else {
		cur_replay.ReadName(mainGame->dInfo.hostname[0]);
		cur_replay.ReadName(mainGame->dInfo.clientname[0]);
	}
	int opt = cur_replay.ReadInt32();
	mainGame->dInfo.duel_field = opt & 0xff;
	mainGame->dInfo.extraval = ((opt >> 8) & SPEED_DUEL) ? 1 : 0;
	mainGame->SetPhaseButtons();
	if(!cur_replay.ReadStream(&current_stream)) {
		EndDuel();
		return 0;
	}
	mainGame->dInfo.isStarted = true;
	mainGame->dInfo.isReplay = true;
	mainGame->dInfo.turn = 0;
	mainGame->dInfo.isReplaySkiping = (skip_turn > 0);
	is_continuing = true;
	skip_step = 0;
	exit_pending = false;
	current_step = 0;
	if(mainGame->dInfo.isReplaySkiping)
		mainGame->gMutex.Lock();
	for(auto it = current_stream.begin(); is_continuing && !exit_pending && it != current_stream.end();) {
		is_continuing = ReplayAnalyze((*it));
		if(is_restarting) {
			it = current_stream.begin();
			is_restarting = false;
			int step = current_step - 1;
			if (step < 0)
				step = 0;
			if (step == 0) {
				Pause(true, false);
				mainGame->dInfo.isStarted = true;
				mainGame->dInfo.isReplaySkiping = false;
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.Unlock();
			}
			skip_step = step;
			current_step = 0;
		} else
			it++;
	}
	if(mainGame->dInfo.isReplaySkiping) {
		mainGame->dInfo.isReplaySkiping = false;
		mainGame->dField.RefreshAllCards();
		mainGame->gMutex.Unlock();
	}
	EndDuel();
	return 0;
}
void ReplayMode::EndDuel() {
	if(yrp)
		end_duel(pduel);
	if(!is_closing) {
		mainGame->actionSignal.Reset();
		mainGame->gMutex.Lock();
		mainGame->stMessage->setText(dataManager.GetSysString(1501));
		if(mainGame->wCardSelect->isVisible())
			mainGame->HideElement(mainGame->wCardSelect);
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.Unlock();
		mainGame->actionSignal.Wait();
		mainGame->gMutex.Lock();
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isReplay = false;
		mainGame->dInfo.isOldReplay = false;
		mainGame->gMutex.Unlock();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.Set();
		mainGame->closeDoneSignal.Wait();
		mainGame->gMutex.Lock();
		mainGame->ShowElement(mainGame->wReplay);
		mainGame->stTip->setVisible(false);
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->gMutex.Unlock();
		if(exit_on_return)
			mainGame->device->closeDevice();
	}
}
void ReplayMode::Restart(bool refresh) {
	if(yrp) {
		end_duel(pduel);
		cur_replay.Rewind();
	}
	mainGame->dInfo.isStarted = false;
	mainGame->dInfo.turn = 0;
	mainGame->dField.Clear();
	mainGame->dInfo.current_player[0] = 0;
	mainGame->dInfo.current_player[1] = 0;
	if (yrp && !StartDuel()) {
		EndDuel();
	}
	if(refresh) {
		mainGame->dField.RefreshAllCards();
		mainGame->dInfo.isStarted = true;
	}
	skip_turn = 0;
	is_restarting = true;
}
void ReplayMode::Undo() {
	if(skip_step > 0 || current_step == 0)
		return;
	mainGame->dInfo.isReplaySkiping = true;
	Restart(false);
	mainGame->gMutex.Lock();
	Pause(false, false);
}
bool ReplayMode::ReplayAnalyze(ReplayPacket p) {
	is_restarting = false;
	while(true) {
		if(is_closing)
			return false;
		if(is_restarting)
			break;
		if(is_swapping) {
			mainGame->gMutex.Lock();
			mainGame->dField.ReplaySwap();
			mainGame->gMutex.Unlock();
			is_swapping = false;
		}
		bool pauseable = true;
		mainGame->dInfo.curMsg = p.message;
		switch (mainGame->dInfo.curMsg) {
		case MSG_RETRY: {
			if(mainGame->dInfo.isReplaySkiping) {
				mainGame->dInfo.isReplaySkiping = false;
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.Unlock();
			}
			mainGame->gMutex.Lock();
			mainGame->stMessage->setText(L"Error occurs.");
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.Unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			return false;
		}
		case MSG_WIN: {
			if (mainGame->dInfo.isReplaySkiping) {
				mainGame->dInfo.isReplaySkiping = false;
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.Unlock();
			}
			DuelClient::ClientAnalyze((char*)p.data, p.length);
			return false;
		}
		case MSG_START:
		case MSG_UPDATE_DATA:
		case MSG_UPDATE_CARD:
		case MSG_SET:
		case MSG_SWAP:
		case MSG_FIELD_DISABLED:
		case MSG_SUMMONING:
		case MSG_SPSUMMONING:
		case MSG_FLIPSUMMONING:
		case MSG_CHAIN_SOLVING:
		case MSG_CHAIN_SOLVED:
		case MSG_CHAIN_END:
		case MSG_CARD_SELECTED:
		case MSG_RANDOM_SELECTED:
		case MSG_EQUIP:
		case MSG_UNEQUIP:
		case MSG_CARD_TARGET:
		case MSG_CANCEL_TARGET:
		case MSG_BATTLE:
		case MSG_ATTACK_DISABLED:
		case MSG_DAMAGE_STEP_START:
		case MSG_DAMAGE_STEP_END:
		case MSG_TAG_SWAP:
		case MSG_RELOAD_FIELD: {
			pauseable = false;
			break;
		}
		case MSG_NEW_TURN: {
			if(skip_turn) {
				skip_turn--;
				if(skip_turn == 0) {
					mainGame->dInfo.isReplaySkiping = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.Unlock();
				}
			}
			break;
		}
		case MSG_AI_NAME: {
			char* pbuf =(char*) p.data;
			char namebuf[128];
			wchar_t wname[128];
			int len = BufferIO::ReadInt16(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			memcpy(namebuf, begin, len + 1);
			BufferIO::DecodeUTF8(namebuf, wname);
			BufferIO::CopyWStr(wname, mainGame->dInfo.clientname[0], 20);
			return true;
		}
		case OLD_REPLAY_MODE:
			return true;
		}
		DuelClient::ClientAnalyze((char*)p.data, p.length);
		if(pauseable) {
			current_step++;
			if(skip_step) {
				skip_step--;
				if(skip_step == 0) {
					Pause(true, false);
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.isReplaySkiping = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.Unlock();
				}
			}
			if(is_pausing) {
				is_paused = true;
				mainGame->actionSignal.Reset();
				mainGame->actionSignal.Wait();
				is_paused = false;
			}
		}
		break;
	}
	return true;
}

}
