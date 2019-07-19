#include "replay_mode.h"
#include "duelclient.h"
#include "game.h"
#include "single_mode.h"
#include <random>

namespace ygo {
	bool ReplayMode::ReadReplayResponse() {
		ReplayResponse res;
		bool result = cur_replay.yrp->GetNextResponse(&res);
		if (result)
			set_responseb(pduel, res.response.data(), res.length);
		return result;
	}
	int ReplayMode::OldReplayThread() {
		mainGame->dInfo.isReplay = true;
		mainGame->dInfo.isOldReplay = true;
		if(!cur_replay.yrp) {
			EndDuel();
			return 0;
		}
		const ReplayHeader& rh = cur_replay.yrp->pheader;
		mainGame->dInfo.isFirst = true;
		mainGame->dInfo.isRelay = !!(rh.flag & REPLAY_RELAY);
		mainGame->dInfo.isSingleMode = !!(rh.flag & REPLAY_SINGLE_MODE);
		mainGame->dInfo.compat_mode = false;
		mainGame->dInfo.current_player[0] = 0;
		mainGame->dInfo.current_player[1] = 0;
		if(!mainGame->dInfo.isRelay)
			mainGame->dInfo.current_player[1] = mainGame->dInfo.team2 - 1;
		if (!StartDuel()) {
			EndDuel();
			return 0;
		}
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isOldReplay = true;
		mainGame->SetMesageWindow();
		mainGame->dInfo.isCatchingUp = (skip_turn > 0);
		std::vector<uint8> duelBuffer;
		is_continuing = true;
		skip_step = 0;
		if (mainGame->dInfo.isSingleMode) {
			int len = get_message(pduel, nullptr);
			if(len > 0) {
				duelBuffer.resize(len);
				get_message(pduel, duelBuffer.data());
				is_continuing = ReplayAnalyze((char*)duelBuffer.data(), len);
			}
		}
		else {
			ReplayRefresh(0, LOCATION_DECK, 0x181fff);
			ReplayRefresh(1, LOCATION_DECK, 0x181fff);
			ReplayRefresh(0, LOCATION_EXTRA, 0x181fff);
			ReplayRefresh(1, LOCATION_EXTRA, 0x181fff);
		}
		exit_pending = false;
		current_step = 0;
		if (mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		while (is_continuing && !exit_pending) {
			/*int flag = */process(pduel);
			int len = get_message(pduel, nullptr);
			if (len > 0) {
				duelBuffer.resize(len);
				get_message(pduel, duelBuffer.data());
				is_continuing = ReplayAnalyze((char*)duelBuffer.data(), len);
				if (is_restarting) {
					is_restarting = false;
					int step = current_step - 1;
					if (step < 0)
						step = 0;
					if (mainGame->dInfo.isSingleMode) {
						is_continuing = true;
						skip_step = 0;
						len = get_message(pduel, nullptr);
						if (len > 0) {
							duelBuffer.resize(len);
							get_message(pduel, duelBuffer.data());
							mainGame->gMutex.unlock();
							is_continuing = ReplayAnalyze((char*)duelBuffer.data(), len);
							mainGame->gMutex.lock();
						}
					}
					if (step == 0) {
						Pause(true, false);
						mainGame->dInfo.isStarted = true;
						mainGame->dInfo.isCatchingUp = false;
						mainGame->dField.RefreshAllCards();
						mainGame->gMutex.unlock();
					}
					skip_step = step;
					current_step = 0;
				}
			}
		}
		if (mainGame->dInfo.isCatchingUp) {
			mainGame->dInfo.isCatchingUp = false;
			mainGame->dField.RefreshAllCards();
			mainGame->gMutex.unlock();
		}
		EndDuel();
		return 0;
	}
	bool ReplayMode::StartDuel() {
		const ReplayHeader& rh = cur_replay.yrp->pheader;
		int seed = rh.seed;
		auto names = ReplayMode::cur_replay.yrp->GetPlayerNames();
		mainGame->dInfo.hostname.insert(mainGame->dInfo.hostname.end(), names.begin(), names.begin() + ReplayMode::cur_replay.yrp->GetPlayersCount(0));
		mainGame->dInfo.clientname.insert(mainGame->dInfo.clientname.end(), names.begin() + ReplayMode::cur_replay.yrp->GetPlayersCount(0), names.end());
		std::mt19937 rnd(seed);
		pduel = mainGame->SetupDuel(rnd());
		int start_lp = cur_replay.yrp->params.start_lp;
		int start_hand = cur_replay.yrp->params.start_hand;
		int draw_count = cur_replay.yrp->params.draw_count;
		int opt = cur_replay.yrp->params.duel_flags;
		mainGame->dInfo.duel_field = 2;
		if ((opt & DUEL_PZONE) && (opt & DUEL_SEPARATE_PZONE) && (opt & DUEL_EMZONE))
			mainGame->dInfo.duel_field = 5;
		else if (opt & DUEL_EMZONE)
			mainGame->dInfo.duel_field = 4;
		else if (opt & DUEL_PZONE)
			mainGame->dInfo.duel_field = 3;
		mainGame->dInfo.extraval = (opt & SPEED_DUEL) ? 1 : 0;
		mainGame->SetPhaseButtons();
		set_player_info(pduel, 0, start_lp, start_hand, draw_count);
		set_player_info(pduel, 1, start_lp, start_hand, draw_count);
		mainGame->dInfo.lp[0] = start_lp;
		mainGame->dInfo.lp[1] = start_lp;
		mainGame->dInfo.startlp = start_lp;
		mainGame->dInfo.strLP[0] = fmt::to_wstring(mainGame->dInfo.lp[0]);
		mainGame->dInfo.strLP[1] = fmt::to_wstring(mainGame->dInfo.lp[1]);
		mainGame->dInfo.turn = 0;
		auto rule_cards = cur_replay.yrp->GetRuleCards();
		for(auto card : rule_cards)
			new_card(pduel, card, 0, 0, 0, 0, POS_FACEDOWN_DEFENSE, 0);
		auto decks = cur_replay.yrp->GetPlayerDecks();
		if (!mainGame->dInfo.isSingleMode) {
			for(int i = 0; i < mainGame->dInfo.team1; i++) {
				for(auto card : decks[i].main_deck)
					new_card(pduel, card, 0, 0, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE, i);
				for(auto card : decks[i].extra_deck)
					new_card(pduel, card, 0, 0, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE, i);
			}
			for(int i = 0; i < mainGame->dInfo.team2; i++) {
				for(auto card : decks[i + mainGame->dInfo.team1].main_deck)
					new_card(pduel, card, 1, 1, LOCATION_DECK, 0, POS_FACEDOWN_DEFENSE, i);
				for(auto card : decks[i + mainGame->dInfo.team1].extra_deck)
					new_card(pduel, card, 1, 1, LOCATION_EXTRA, 0, POS_FACEDOWN_DEFENSE, i);
			}
			mainGame->dField.Initial(0, decks[0].main_deck.size(), decks[0].extra_deck.size());
			mainGame->dField.Initial(1, decks[mainGame->dInfo.team1].main_deck.size(), decks[mainGame->dInfo.team1].extra_deck.size());
		} else {
			if(!preload_script(pduel, (char*)cur_replay.yrp->scriptname.c_str(), 0, 0, nullptr))
				return false;
		}
		start_duel(pduel, opt);
		return true;
	}
	bool ReplayMode::ReplayAnalyze(char* msg, unsigned int len) {
		char* pbuf = msg;
		int player, count;
		is_restarting = false;
		while (pbuf - msg < (int)len) {
			if (is_closing)
				return false;
			if (is_restarting) {
				//is_restarting = false;
				return true;
			}
			if (is_swapping) {
				mainGame->gMutex.lock();
				mainGame->dField.ReplaySwap();
				mainGame->gMutex.unlock();
				is_swapping = false;
			}
			char* offset = pbuf;
			bool pauseable = true;
			mainGame->dInfo.curMsg = BufferIO::ReadUInt8(pbuf);
			switch (mainGame->dInfo.curMsg) {
			case MSG_RETRY: {
				if (mainGame->dInfo.isCatchingUp) {
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
				mainGame->gMutex.lock();
				mainGame->stMessage->setText(L"Error occurs.");
				mainGame->PopupElement(mainGame->wMessage);
				mainGame->gMutex.unlock();
				mainGame->actionSignal.Reset();
				mainGame->actionSignal.Wait();
				return false;
			}
			case MSG_HINT: {
				pbuf += 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_WIN: {
				if (mainGame->dInfo.isCatchingUp) {
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
				pbuf += 2;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				return false;
			}
			case MSG_SELECT_BATTLECMD: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 21;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 8 + 2;
				ReplayRefresh();
				return ReadReplayResponse();
			}
			case MSG_SELECT_IDLECMD: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 7;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 18 + 3;
				ReplayRefresh();
				return ReadReplayResponse();
			}
			case MSG_SELECT_EFFECTYN: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 22;
				return ReadReplayResponse();
			}
			case MSG_SELECT_YESNO: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 8;
				return ReadReplayResponse();
			}
			case MSG_SELECT_OPTION: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadUInt8(pbuf);
				pbuf += count * 8;
				return ReadReplayResponse();
			}
			case MSG_SELECT_CARD: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 9;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 14;
				return ReadReplayResponse();
			}
			case MSG_SELECT_TRIBUTE: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 9;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 11;
				return ReadReplayResponse();
			}
			case MSG_SELECT_UNSELECT_CARD: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 10;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 14;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 14;
				return ReadReplayResponse();
			}
			case MSG_SELECT_CHAIN: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += 10 + count * 21;
				return ReadReplayResponse();
			}
			case MSG_SELECT_PLACE:
			case MSG_SELECT_DISFIELD: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 5;
				return ReadReplayResponse();
			}
			case MSG_SELECT_POSITION: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 5;
				return ReadReplayResponse();
			}
			case MSG_SELECT_COUNTER: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 4;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 9;
				return ReadReplayResponse();
			}
			case MSG_SELECT_SUM: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 13;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 17;
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 17;
				return ReadReplayResponse();
			}
			case MSG_SORT_CARD:
			case MSG_SORT_CHAIN: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 13;
				return ReadReplayResponse();
			}
			case MSG_CONFIRM_DECKTOP: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_CONFIRM_EXTRATOP: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_CONFIRM_CARDS: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_SHUFFLE_DECK: {
				player = BufferIO::ReadUInt8(pbuf);
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				ReplayRefresh(player, LOCATION_DECK, 0x181fff);
				break;
			}
			case MSG_SHUFFLE_HAND: {
				/*int oplayer = */BufferIO::ReadUInt8(pbuf);
				int count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 4;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_SHUFFLE_EXTRA: {
				/*int oplayer = */BufferIO::ReadUInt8(pbuf);
				int count = BufferIO::ReadInt32(pbuf);
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
				player = BufferIO::ReadUInt8(pbuf);
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				ReplayRefresh(player, LOCATION_GRAVE, 0x181fff);
				break;
			}
			case MSG_REVERSE_DECK: {
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				ReplayRefresh(0, LOCATION_DECK, 0x181fff);
				ReplayRefresh(1, LOCATION_DECK, 0x181fff);
				break;
			}
			case MSG_DECK_TOP: {
				pbuf += 9;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_SHUFFLE_SET_CARD: {
				pbuf++;
				count = BufferIO::ReadUInt8(pbuf);
				pbuf += count * 20;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_NEW_TURN: {
				if (skip_turn) {
					skip_turn--;
					if (skip_turn == 0) {
						mainGame->dInfo.isCatchingUp = false;
						mainGame->dField.RefreshAllCards();
						mainGame->gMutex.unlock();
					}
				}
				player = BufferIO::ReadUInt8(pbuf);
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
				pbuf += 4;
				loc_info previous = ClientCard::read_location_info(pbuf);
				loc_info current = ClientCard::read_location_info(pbuf);
				pbuf += 4;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				if(current.location != 0 && (current.location & 0x80) == 0 && (current.location != previous.location || previous.controler != current.controler))
					ReplayRefreshSingle(current.controler, current.location, current.sequence);
				break;
			}
			case MSG_POS_CHANGE: {
				pbuf += 9;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_SET: {
				pbuf += 14;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				pauseable = false;
				break;
			}
			case MSG_SWAP: {
				pbuf += 28;
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
				pbuf += 14;
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
				pbuf += 14;
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
				pbuf += 14;
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
				pbuf += 32;
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
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				pauseable = false;
				break;
			}
			case MSG_BECOME_TARGET: {
				count = BufferIO::ReadInt32(pbuf);
				pbuf += count * 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_DRAW: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadInt32(pbuf);
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
				pbuf += 20;
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
				pbuf += 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				pauseable = false;
				break;
			}
			case MSG_CARD_TARGET: {
				pbuf += 20;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				pauseable = false;
				break;
			}
			case MSG_CANCEL_TARGET: {
				pbuf += 20;
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
				pbuf += 20;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_BATTLE: {
				pbuf += 38;
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
				pbuf += 14;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_TOSS_COIN: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadUInt8(pbuf);
				pbuf += count;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_TOSS_DICE: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadUInt8(pbuf);
				pbuf += count;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_ROCK_PAPER_SCISSORS: {
				player = BufferIO::ReadUInt8(pbuf);
				return ReadReplayResponse();
			}
			case MSG_HAND_RES: {
				pbuf += 1;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_ANNOUNCE_RACE: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 5;
				return ReadReplayResponse();
			}
			case MSG_ANNOUNCE_ATTRIB: {
				player = BufferIO::ReadUInt8(pbuf);
				pbuf += 5;
				return ReadReplayResponse();
			}
			case MSG_ANNOUNCE_CARD:
			case MSG_ANNOUNCE_NUMBER: {
				player = BufferIO::ReadUInt8(pbuf);
				count = BufferIO::ReadUInt8(pbuf);
				pbuf += 8 * count;
				return ReadReplayResponse();
			}
			case MSG_CARD_HINT: {
				pbuf += 19;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_PLAYER_HINT: {
				pbuf += 10;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				break;
			}
			case MSG_MATCH_KILL: {
				pbuf += 4;
				break;
			}
			case MSG_TAG_SWAP: {
				player = BufferIO::ReadUInt8(pbuf);
				/*int mcount = */BufferIO::ReadInt32(pbuf);
				int ecount = BufferIO::ReadInt32(pbuf);
				/*int pcount = */BufferIO::ReadInt32(pbuf);
				int hcount = BufferIO::ReadInt32(pbuf);
				pbuf += ecount * 4 + hcount * 4 + 4;
				DuelClient::ClientAnalyze(offset, pbuf - offset);
				ReplayRefresh(player, LOCATION_DECK, 0x181fff);
				ReplayRefresh(player, LOCATION_EXTRA, 0x181fff);
				break;
			}
			case MSG_RELOAD_FIELD: {
				pbuf++;
				for (int p = 0; p < 2; ++p) {
					pbuf += 4;
					for (int seq = 0; seq < 7; ++seq) {
						int val = BufferIO::ReadUInt8(pbuf);
						if (val)
							pbuf += 5;
					}
					for (int seq = 0; seq < 8; ++seq) {
						int val = BufferIO::ReadUInt8(pbuf);
						if (val)
							pbuf++;
					}
					pbuf += 24;
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
			if (pauseable) {
				current_step++;
				if (skip_step) {
					skip_step--;
					if (skip_step == 0) {
						Pause(true, false);
						mainGame->dInfo.isStarted = true;
						mainGame->dInfo.isCatchingUp = false;
						mainGame->dField.RefreshAllCards();
						mainGame->gMutex.unlock();
					}
				}
				if (is_pausing) {
					is_paused = true;
					mainGame->actionSignal.Reset();
					mainGame->actionSignal.Wait();
					is_paused = false;
				}
			}
		}
		return true;
	}
	void ReplayMode::ReplayRefresh(int player, int location, int flag) {
		std::vector<uint8_t> buffer;
		int len = query_field_card(pduel, player, location, flag, nullptr, 0, FALSE);
		buffer.resize(len);
		get_cached_query(pduel, buffer.data());
		mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), location, (char*)buffer.data());
	}
	void ReplayMode::ReplayRefresh(int flag) {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_HAND; loc != LOCATION_GRAVE; loc *= 2)
				ReplayRefresh(p, loc, flag);
	}
	void ReplayMode::ReplayRefreshSingle(int player, int location, int sequence, int flag) {
		std::vector<uint8_t> buffer;
		int len = query_card(pduel, player, location, sequence, flag, nullptr, 0, FALSE);
		buffer.resize(len);
		get_cached_query(pduel, buffer.data());
		mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)buffer.data());
	}
	void ReplayMode::ReplayReload() {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_DECK; loc != LOCATION_OVERLAY; loc *= 2)
				ReplayRefresh(p, loc, 0xffdfff);
	}

}
