#include "replay_mode.h"
#include "duelclient.h"
#include "game.h"
#include "single_mode.h"
#include "../ocgcore/common.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

long ReplayMode::pduel = 0;
Replay ReplayMode::cur_replay;
bool ReplayMode::is_continuing = true;
bool ReplayMode::is_closing = false;
bool ReplayMode::is_pausing = false;
bool ReplayMode::is_paused = false;
bool ReplayMode::is_swaping = false;
bool ReplayMode::is_restarting = false;
bool ReplayMode::exit_pending = false;
int ReplayMode::skip_turn = 0;
int ReplayMode::current_step = 0;
int ReplayMode::skip_step = 0;

bool ReplayMode::StartReplay(int skipturn) {
	skip_turn = skipturn;
	if(skip_turn < 0)
		skip_turn = 0;
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
		is_swaping = true;
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
bool ReplayMode::ReadReplayResponse() {
	unsigned char resp[64];
	bool result = cur_replay.ReadNextResponse(resp);
	if(result)
		set_responseb(pduel, resp);
	return result;
}
int ReplayMode::ReplayThread(void* param) {
	const ReplayHeader& rh = cur_replay.pheader;
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isTag = !!(rh.flag & REPLAY_TAG);
	mainGame->dInfo.isSingleMode = !!(rh.flag & REPLAY_SINGLE_MODE);
	mainGame->dInfo.tag_player[0] = false;
	mainGame->dInfo.tag_player[1] = false;
	set_script_reader((script_reader)DataManager::ScriptReaderEx);
	set_card_reader((card_reader)DataManager::CardReader);
	set_message_handler((message_handler)MessageHandler);
	if(!StartDuel()) {
		EndDuel();
		return 0;
	}
	mainGame->dInfo.isStarted = true;
	mainGame->dInfo.isFinished = false;
	mainGame->dInfo.isReplay = true;
	mainGame->dInfo.isReplaySkiping = (skip_turn > 0);
	char engineBuffer[0x1000];
	is_continuing = true;
	skip_step = 0;
	if(mainGame->dInfo.isSingleMode) {
		int len = get_message(pduel, (byte*)engineBuffer);
		if (len > 0)
			is_continuing = ReplayAnalyze(engineBuffer, len);
	} else {
		ReplayRefreshDeck(0);
		ReplayRefreshDeck(1);
		ReplayRefreshExtra(0);
		ReplayRefreshExtra(1);
	}
	exit_pending = false;
	current_step = 0;
	if(mainGame->dInfo.isReplaySkiping)
		mainGame->gMutex.Lock();
	while (is_continuing && !exit_pending) {
		int result = process(pduel);
		int len = result & 0xffff;
		/*int flag = result >> 16;*/
		if (len > 0) {
			get_message(pduel, (byte*)engineBuffer);
			is_continuing = ReplayAnalyze(engineBuffer, len);
			if(is_restarting) {
				mainGame->gMutex.Lock();
				is_restarting = false;
				int step = current_step - 1;
				if(step < 0)
					step = 0;
				if(mainGame->dInfo.isSingleMode) {
					is_continuing = true;
					skip_step = 0;
					int len = get_message(pduel, (byte*)engineBuffer);
					if (len > 0) {
						mainGame->gMutex.Unlock();
						is_continuing = ReplayAnalyze(engineBuffer, len);
						mainGame->gMutex.Lock();
					}
				}
				if(step == 0) {
					Pause(true, false);
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.isFinished = false;
					mainGame->dInfo.isReplaySkiping = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.Unlock();
				}
				skip_step = step;
				current_step = 0;
			}
		}
	}
	if(mainGame->dInfo.isReplaySkiping) {
		mainGame->dInfo.isReplaySkiping = false;
		mainGame->dField.RefreshAllCards();
		mainGame->gMutex.Unlock();
	}
	EndDuel();
	return 0;
}
bool ReplayMode::StartDuel() {
	const ReplayHeader& rh = cur_replay.pheader;
	mtrandom rnd;
	int seed = rh.seed;
	rnd.reset(seed);
	if(mainGame->dInfo.isTag) {
		cur_replay.ReadName(mainGame->dInfo.hostname);
		cur_replay.ReadName(mainGame->dInfo.hostname_tag);
		cur_replay.ReadName(mainGame->dInfo.clientname_tag);
		cur_replay.ReadName(mainGame->dInfo.clientname);
	} else {
		cur_replay.ReadName(mainGame->dInfo.hostname);
		cur_replay.ReadName(mainGame->dInfo.clientname);
	}
	pduel = create_duel(rnd.rand());
	int start_lp = cur_replay.ReadInt32();
	int start_hand = cur_replay.ReadInt32();
	int draw_count = cur_replay.ReadInt32();
	int opt = cur_replay.ReadInt32();
	int duel_rule = opt >> 16;
	mainGame->dInfo.duel_rule = duel_rule;
	set_player_info(pduel, 0, start_lp, start_hand, draw_count);
	set_player_info(pduel, 1, start_lp, start_hand, draw_count);
	mainGame->dInfo.lp[0] = start_lp;
	mainGame->dInfo.lp[1] = start_lp;
	myswprintf(mainGame->dInfo.strLP[0], L"%d", mainGame->dInfo.lp[0]);
	myswprintf(mainGame->dInfo.strLP[1], L"%d", mainGame->dInfo.lp[1]);
	mainGame->dInfo.turn = 0;
	if(!mainGame->dInfo.isSingleMode) {
		if(!(opt & DUEL_TAG_MODE)) {
			int main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 0, 0, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
			int extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
			mainGame->dField.Initial(0, main, extra);
			main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
			extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
			mainGame->dField.Initial(1, main, extra);
		} else {
			int main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 0, 0, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
			int extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
			mainGame->dField.Initial(0, main, extra);
			main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_tag_card(pduel, cur_replay.ReadInt32(), 0, LOCATION_DECK);
			extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_tag_card(pduel, cur_replay.ReadInt32(), 0, LOCATION_EXTRA);
			main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE);
			extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_card(pduel, cur_replay.ReadInt32(), 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE);
			mainGame->dField.Initial(1, main, extra);
			main = cur_replay.ReadInt32();
			for(int i = 0; i < main; ++i)
				new_tag_card(pduel, cur_replay.ReadInt32(), 1, LOCATION_DECK);
			extra = cur_replay.ReadInt32();
			for(int i = 0; i < extra; ++i)
				new_tag_card(pduel, cur_replay.ReadInt32(), 1, LOCATION_EXTRA);
		}
	} else {
		char filename[256];
		size_t slen = cur_replay.ReadInt16();
		cur_replay.ReadData(filename, slen);
		filename[slen] = 0;
		if(!preload_script(pduel, filename, 0)) {
			return false;
		}
	}
	start_duel(pduel, opt);
	return true;
}
void ReplayMode::EndDuel() {
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
		mainGame->dInfo.isFinished = true;
		mainGame->dInfo.isReplay = false;
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
	end_duel(pduel);
	mainGame->dInfo.isStarted = false;
	mainGame->dInfo.isFinished = true;
	mainGame->dField.Clear();
	//mainGame->device->setEventReceiver(&mainGame->dField);
	cur_replay.Rewind();
	//mainGame->dInfo.isFirst = true;
	mainGame->dInfo.tag_player[0] = false;
	mainGame->dInfo.tag_player[1] = false;
	if(!StartDuel()) {
		EndDuel();
	}
	if(refresh) {
		mainGame->dField.RefreshAllCards();
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isFinished = false;
		//mainGame->dInfo.isReplay = true;
	}
	skip_turn = 0;
	is_restarting = true;
}
void ReplayMode::Undo() {
	if(skip_step > 0 || current_step == 0)
		return;
	mainGame->dInfo.isReplaySkiping = true;
	Restart(false);
	Pause(false, false);
}
bool ReplayMode::ReplayAnalyze(char* msg, unsigned int len) {
	char* pbuf = msg;
	int player, count;
	is_restarting = false;
	while (pbuf - msg < (int)len) {
		if(is_closing)
			return false;
		if(is_restarting) {
			//is_restarting = false;
			return true;
		}
		if(is_swaping) {
			mainGame->gMutex.Lock();
			mainGame->dField.ReplaySwap();
			mainGame->gMutex.Unlock();
			is_swaping = false;
		}
		char* offset = pbuf;
		bool pauseable = true;
		mainGame->dInfo.curMsg = BufferIO::ReadUInt8(pbuf);
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
		case MSG_HINT: {
			pbuf += 6;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_WIN: {
			if(mainGame->dInfo.isReplaySkiping) {
				mainGame->dInfo.isReplaySkiping = false;
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.Unlock();
			}
			pbuf += 2;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			return false;
		}
		case MSG_SELECT_BATTLECMD: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8 + 2;
			ReplayRefresh();
			return ReadReplayResponse();
		}
		case MSG_SELECT_IDLECMD: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11 + 3;
			ReplayRefresh();
			return ReadReplayResponse();
		}
		case MSG_SELECT_EFFECTYN: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 12;
			return ReadReplayResponse();
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			return ReadReplayResponse();
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			return ReadReplayResponse();
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 3;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			return ReadReplayResponse();
		}
		case MSG_SELECT_UNSELECT_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			return ReadReplayResponse();
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 10 + count * 13;
			return ReadReplayResponse();
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			return ReadReplayResponse();
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			return ReadReplayResponse();
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 9;
			return ReadReplayResponse();
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 6;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			return ReadReplayResponse();
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			return ReadReplayResponse();
		}
		case MSG_CONFIRM_DECKTOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_EXTRATOP: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefreshDeck(player);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			/*int oplayer = */BufferIO::ReadInt8(pbuf);
			int count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_EXTRA: {
			/*int oplayer = */BufferIO::ReadInt8(pbuf);
			int count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::ReadInt8(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefreshDeck(0);
			ReplayRefreshDeck(1);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 6;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			pbuf++;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
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
			player = BufferIO::ReadInt8(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			break;
		}
		case MSG_MOVE: {
			int pc = pbuf[4];
			int pl = pbuf[5];
			/*int ps = pbuf[6];*/
			/*int pp = pbuf[7];*/
			int cc = pbuf[8];
			int cl = pbuf[9];
			int cs = pbuf[10];
			/*int cp = pbuf[11];*/
			pbuf += 16;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			if(cl && !(cl & 0x80) && (pl != cl || pc != cc))
				ReplayRefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_POS_CHANGE: {
			pbuf += 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SET: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SWAP: {
			pbuf += 16;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			break;
		}
		case MSG_FLIPSUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_FLIPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			break;
		}
		case MSG_CHAINING: {
			pbuf += 16;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_CHAIN_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_CHAIN_NEGATED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CARD_SELECTED:
		case MSG_RANDOM_SELECTED: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_BECOME_TARGET: {
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DRAW: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 7;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 26;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_ATTACK_DISABLED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_MISSED_EFFECT: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_DICE: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ROCK_PAPER_SCISSORS: {
			player = BufferIO::ReadInt8(pbuf);
			return ReadReplayResponse();
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			return ReadReplayResponse();
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			return ReadReplayResponse();
		}
		case MSG_ANNOUNCE_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			return ReadReplayResponse();
		}
		case MSG_ANNOUNCE_NUMBER:
		case MSG_ANNOUNCE_CARD_FILTER: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadUInt8(pbuf);
			pbuf += 4 * count;
			return ReadReplayResponse();
		}
		case MSG_CARD_HINT: {
			pbuf += 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_PLAYER_HINT: {
			pbuf += 6;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_MATCH_KILL: {
			pbuf += 4;
			break;
		}
		case MSG_TAG_SWAP: {
			player = pbuf[0];
			pbuf += pbuf[2] * 4 + pbuf[4] * 4 + 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayRefreshDeck(player);
			ReplayRefreshExtra(player);
			break;
		}
		case MSG_RELOAD_FIELD: {
			pbuf++;
			for(int p = 0; p < 2; ++p) {
				pbuf += 4;
				for(int seq = 0; seq < 7; ++seq) {
					int val = BufferIO::ReadInt8(pbuf);
					if(val)
						pbuf += 2;
				}
				for(int seq = 0; seq < 8; ++seq) {
					int val = BufferIO::ReadInt8(pbuf);
					if(val)
						pbuf++;
				}
				pbuf += 6;
			}
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			ReplayReload();
			mainGame->dField.RefreshAllCards();
			break;
		}
		case MSG_AI_NAME: {
			int len = BufferIO::ReadInt16(pbuf);
			pbuf += len + 1;
			break;
		}
		case MSG_SHOW_HINT: {
			int len = BufferIO::ReadInt16(pbuf);
			pbuf += len + 1;
			break;
		}
		}
		if(pauseable) {
			current_step++;
			if(skip_step) {
				skip_step--;
				if(skip_step == 0) {
					Pause(true, false);
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.isFinished = false;
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
	}
	return true;
}
void ReplayMode::ReplayRefresh(int flag) {
	unsigned char queryBuffer[0x4000];
	/*int len = */query_field_card(pduel, 0, LOCATION_MZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_MZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_MZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_MZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_SZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_SZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_SZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_SZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_HAND, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_HAND, (char*)queryBuffer);
}
void ReplayMode::ReplayRefreshHand(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_HAND, (char*)queryBuffer);
}
void ReplayMode::ReplayRefreshGrave(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_GRAVE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_GRAVE, (char*)queryBuffer);
}
void ReplayMode::ReplayRefreshDeck(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_DECK, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_DECK, (char*)queryBuffer);
}
void ReplayMode::ReplayRefreshExtra(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_EXTRA, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_EXTRA, (char*)queryBuffer);
}
void ReplayMode::ReplayRefreshSingle(int player, int location, int sequence, int flag) {
	unsigned char queryBuffer[0x4000];
	/*int len = */query_card(pduel, player, location, sequence, flag, queryBuffer, 0);
	mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)queryBuffer);
}
void ReplayMode::ReplayReload() {
	unsigned char queryBuffer[0x4000];
	unsigned int flag = 0xffdfff;
	/*int len = */query_field_card(pduel, 0, LOCATION_MZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_MZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_MZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_MZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_SZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_SZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_SZONE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_SZONE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_HAND, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_HAND, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_DECK, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_DECK, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_DECK, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_DECK, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_EXTRA, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_EXTRA, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_EXTRA, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_EXTRA, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_GRAVE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_GRAVE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_GRAVE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_GRAVE, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 0, LOCATION_REMOVED, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_REMOVED, (char*)queryBuffer);
	/*len = */query_field_card(pduel, 1, LOCATION_REMOVED, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_REMOVED, (char*)queryBuffer);
}
int ReplayMode::MessageHandler(long fduel, int type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, (byte*)msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}

}
