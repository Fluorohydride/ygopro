#include <IrrlichtDevice.h>
#include <IGUIWindow.h>
#include <IGUIStaticText.h>
#include "replay_mode.h"
#include "duelclient.h"
#include "game.h"
#include "single_mode.h"
#include "sound_manager.h"

namespace ygo {

void* ReplayMode::pduel = 0;
bool ReplayMode::yrp = false;
Replay ReplayMode::cur_replay;
Replay* ReplayMode::cur_yrp = nullptr;
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

bool ReplayMode::StartReplay(int skipturn, bool is_yrp) {
	if(mainGame->dInfo.isReplay)
		return false;
	skip_turn = skipturn;
	if(skip_turn < 0)
		skip_turn = 0;
	yrp = is_yrp;
	if(is_yrp) {
		if(cur_replay.pheader.id == REPLAY_YRP1)
			cur_yrp = &cur_replay;
		else
			cur_yrp = cur_replay.yrp.get();
		if(!cur_yrp)
			return false;
		std::thread(OldReplayThread).detach();
	} else
		std::thread(ReplayThread).detach();
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
int ReplayMode::ReplayThread() {
	mainGame->dInfo.isReplay = true;
	const ReplayHeader& rh = cur_replay.pheader;
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isTeam1 = true;
	mainGame->dInfo.isRelay = !!(cur_replay.params.duel_flags & DUEL_RELAY);
	mainGame->dInfo.isSingleMode = !!(rh.flag & REPLAY_SINGLE_MODE);
	mainGame->dInfo.isHandTest = !!(rh.flag & REPLAY_HAND_TEST);
	mainGame->dInfo.compat_mode = !(rh.flag & REPLAY_LUA64);
	mainGame->dInfo.team1 = ReplayMode::cur_replay.GetPlayersCount(0);
	mainGame->dInfo.team2 = ReplayMode::cur_replay.GetPlayersCount(1);
	mainGame->dInfo.current_player[0] = 0;
	mainGame->dInfo.current_player[1] = 0;
	if(!mainGame->dInfo.isRelay)
		mainGame->dInfo.current_player[1] = mainGame->dInfo.team2 - 1;
	auto names = ReplayMode::cur_replay.GetPlayerNames();
	mainGame->dInfo.selfnames.clear();
	mainGame->dInfo.opponames.clear();
	mainGame->dInfo.selfnames.insert(mainGame->dInfo.selfnames.end(), names.begin(), names.begin() + mainGame->dInfo.team1);
	mainGame->dInfo.opponames.insert(mainGame->dInfo.opponames.end(), names.begin() + mainGame->dInfo.team1, names.end());
	mainGame->dInfo.duel_params = cur_replay.params.duel_flags;
	mainGame->dInfo.duel_field = mainGame->GetMasterRule(mainGame->dInfo.duel_params);
	mainGame->SetPhaseButtons();
	auto& current_stream = cur_replay.packets_stream;
	if(!current_stream.size()) {
		EndDuel();
		return 0;
	}
	mainGame->dInfo.isInDuel = true;
	mainGame->dInfo.isStarted = true;
	mainGame->SetMessageWindow();
	mainGame->dInfo.turn = 0;
	mainGame->dInfo.isCatchingUp = (skip_turn > 0);
	is_continuing = true;
	skip_step = 0;
	exit_pending = false;
	current_step = 0;
	if(mainGame->dInfo.isCatchingUp)
		mainGame->gMutex.lock();
	for(auto it = current_stream.begin(); is_continuing && !exit_pending && it != current_stream.end();) {
		is_continuing = ReplayAnalyze((*it));
		if(is_restarting) {
			mainGame->gMutex.lock();
			it = current_stream.begin();
			is_restarting = false;
			int step = current_step - 1;
			if (step < 0)
				step = 0;
			if (step == 0) {
				Pause(true, false);
				mainGame->dInfo.isInDuel = true;
				mainGame->dInfo.isStarted = true;
				mainGame->dInfo.isCatchingUp = false;
				mainGame->dField.RefreshAllCards();
				mainGame->SetMessageWindow();
				mainGame->gMutex.unlock();
			}
			skip_step = step;
			current_step = 0;
		} else
			it++;
	}
	if(mainGame->dInfo.isCatchingUp) {
		mainGame->dInfo.isCatchingUp = false;
		mainGame->dField.RefreshAllCards();
		mainGame->gMutex.unlock();
	}
	EndDuel();
	return 0;
}
void ReplayMode::EndDuel() {
	if(pduel) {
		OCG_DestroyDuel(pduel);
		pduel = nullptr;
	}
	if(!is_closing) {
		mainGame->actionSignal.Reset();
		mainGame->gMutex.lock();
		mainGame->stMessage->setText(gDataManager->GetSysString(1501).c_str());
		if(mainGame->wCardSelect->isVisible())
			mainGame->HideElement(mainGame->wCardSelect);
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.unlock();
		mainGame->actionSignal.Wait();
		mainGame->gMutex.lock();
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isReplay = false;
		mainGame->dInfo.isSingleMode = false;
		mainGame->dInfo.isHandTest = false;
		mainGame->dInfo.isOldReplay = false;
		mainGame->gMutex.unlock();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.lock();
		mainGame->closeDoneSignal.Wait();
		mainGame->closeSignal.unlock();
		mainGame->gMutex.lock();
		mainGame->ShowElement(mainGame->wReplay);
		mainGame->SetMessageWindow();
		mainGame->stTip->setVisible(false);
		gSoundManager->StopSounds();
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->gMutex.unlock();
		if(exit_on_return)
			mainGame->device->closeDevice();
	}
}
void ReplayMode::Restart(bool refresh) {
	if(pduel) {
		OCG_DestroyDuel(pduel);
		pduel = nullptr;
		//end_duel(pduel);
		cur_replay.Rewind();
	}
	mainGame->dInfo.isInDuel = false;
	mainGame->dInfo.isStarted = false;
	mainGame->dInfo.turn = 0;
	mainGame->dField.Clear();
	mainGame->dInfo.current_player[0] = 0;
	mainGame->dInfo.current_player[1] = 0;
	if(!mainGame->dInfo.isRelay) {
		if(mainGame->dInfo.isFirst)
			mainGame->dInfo.current_player[1] = mainGame->dInfo.team2 - 1;
		else
			mainGame->dInfo.current_player[0] = mainGame->dInfo.team1 - 1;
	}
	if (yrp && !StartDuel()) {
		EndDuel();
	}
	if(refresh) {
		mainGame->dField.RefreshAllCards();
		mainGame->dInfo.isInDuel = true;
		mainGame->dInfo.isStarted = true;
	}
	skip_turn = 0;
	is_restarting = true;
}
void ReplayMode::Undo() {
	if(skip_step > 0 || current_step == 0)
		return;
	mainGame->dInfo.isCatchingUp = true;
	Restart(false);
	Pause(false, false);
}
bool ReplayMode::ReplayAnalyze(ReplayPacket p) {
	is_restarting = false;
	{
		if(is_closing)
			return false;
		if(is_restarting)
			return true;
		if(is_swapping) {
			mainGame->gMutex.lock();
			mainGame->dField.ReplaySwap();
			mainGame->gMutex.unlock();
			is_swapping = false;
		}
		bool pauseable = true;
		mainGame->dInfo.curMsg = p.message;
		switch (mainGame->dInfo.curMsg) {
		case MSG_RETRY: {
			if(mainGame->dInfo.isCatchingUp) {
				mainGame->dInfo.isCatchingUp = false;
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.unlock();
			}
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(gDataManager->GetSysString(1434).c_str());
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			return false;
		}
		case MSG_WIN: {
			if(!yrp || !cur_yrp || !(cur_yrp->pheader.flag & REPLAY_HAND_TEST)) {
				if (mainGame->dInfo.isCatchingUp) {
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
				DuelClient::ClientAnalyze((char*)p.data.data(), p.data.size());
				return false;
			}
			return true;
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
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
			}
			break;
		}
		case MSG_AI_NAME: {
			char* pbuf = (char*)p.data.data();
			int len = BufferIO::Read<uint16_t>(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			std::string namebuf;
			namebuf.resize(len);
			memcpy(&namebuf[0], begin, len + 1);
			mainGame->dInfo.opponames[0] = BufferIO::DecodeUTF8s(namebuf);
			return true;
		}
		case OLD_REPLAY_MODE:
			return true;
		}
		DuelClient::ClientAnalyze((char*)p.data.data(), p.data.size());
		if(pauseable) {
			current_step++;
			if(skip_step) {
				skip_step--;
				if(skip_step == 0) {
					Pause(true, false);
					mainGame->dInfo.isInDuel = true;
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
			}
			if(is_pausing) {
				is_paused = true;
				mainGame->actionSignal.Reset();
				mainGame->actionSignal.Wait();
				is_paused = false;
			}
		}
	}
	return true;
}

}
