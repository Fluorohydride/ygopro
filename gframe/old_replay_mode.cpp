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
			OCG_DuelSetResponse(pduel, res.response.data(), res.length);
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
		mainGame->dInfo.clientname.clear();
		mainGame->dInfo.hostname.clear();
		mainGame->dInfo.team1 = ReplayMode::cur_replay.GetPlayersCount(0);
		mainGame->dInfo.team2 = ReplayMode::cur_replay.GetPlayersCount(1);
		if(!mainGame->dInfo.isRelay)
			mainGame->dInfo.current_player[1] = mainGame->dInfo.team2 - 1;
		if (!StartDuel()) {
			EndDuel();
			return 0;
		}
		mainGame->dInfo.isInDuel = true;
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isOldReplay = true;
		mainGame->SetMesageWindow();
		mainGame->dInfo.isCatchingUp = (skip_turn > 0);
		is_continuing = true;
		skip_step = 0;
		if (mainGame->dInfo.isSingleMode) {
			auto msg = CoreUtils::ParseMessages(pduel);
			for(auto& message : msg.packets) {
				is_continuing = ReplayAnalyze(message) && is_continuing;
			}
		} else {
			ReplayRefresh(0, LOCATION_DECK, 0x2181fff);
			ReplayRefresh(1, LOCATION_DECK, 0x2181fff);
			ReplayRefresh(0, LOCATION_EXTRA, 0x2181fff);
			ReplayRefresh(1, LOCATION_EXTRA, 0x2181fff);
		}
		exit_pending = false;
		current_step = 0;
		if (mainGame->dInfo.isCatchingUp)
			mainGame->gMutex.lock();
		while(is_continuing && !exit_pending) {
			/*int engFlag = */OCG_DuelProcess(pduel);
			auto msg = CoreUtils::ParseMessages(pduel);
			for(auto& message : msg.packets) {
				is_continuing = ReplayAnalyze(message) && is_continuing;
			}
			if(is_restarting) {
				is_restarting = false;
				int step = current_step - 1;
				if(step < 0)
					step = 0;
				if(mainGame->dInfo.isSingleMode) {
					is_continuing = true;
					skip_step = 0;
					auto msg = CoreUtils::ParseMessages(pduel);
					for(auto& message : msg.packets) {
						is_continuing = ReplayAnalyze(message) && is_continuing;
					}
				}
				if(step == 0) {
					Pause(true, false);
					mainGame->dInfo.isInDuel = true;
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.isCatchingUp = false;
					mainGame->dField.RefreshAllCards();
					mainGame->gMutex.unlock();
				}
				skip_step = step;
				current_step = 0;
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
		int start_lp = cur_replay.yrp->params.start_lp;
		int start_hand = cur_replay.yrp->params.start_hand;
		int draw_count = cur_replay.yrp->params.draw_count;
		OCG_Player team = { start_lp, start_hand, draw_count };
		pduel = mainGame->SetupDuel({ rnd(), cur_replay.yrp->params.duel_flags, team, team });
		int opt = cur_replay.yrp->params.duel_flags;
		mainGame->dInfo.duel_field = 2;
		if ((opt & DUEL_PZONE) && (opt & DUEL_SEPARATE_PZONE) && (opt & DUEL_EMZONE))
			mainGame->dInfo.duel_field = 5;
		else if (opt & DUEL_EMZONE)
			mainGame->dInfo.duel_field = 4;
		else if (opt & DUEL_PZONE)
			mainGame->dInfo.duel_field = 3;
		mainGame->dInfo.extraval = (opt & DUEL_SPEED) ? 1 : 0;
		mainGame->SetPhaseButtons();
		mainGame->dInfo.lp[0] = start_lp;
		mainGame->dInfo.lp[1] = start_lp;
		mainGame->dInfo.startlp = start_lp;
		mainGame->dInfo.strLP[0] = fmt::to_wstring(mainGame->dInfo.lp[0]);
		mainGame->dInfo.strLP[1] = fmt::to_wstring(mainGame->dInfo.lp[1]);
		mainGame->dInfo.turn = 0;
		if (!mainGame->dInfo.isSingleMode) {
			auto rule_cards = cur_replay.yrp->GetRuleCards();
			OCG_NewCardInfo card_info = { 0, 0, 0, 0, 0, 0, POS_FACEDOWN_DEFENSE };
			for(auto card : rule_cards) {
				card_info.code = card;
				OCG_DuelNewCard(pduel, card_info);
			}
			auto decks = cur_replay.yrp->GetPlayerDecks();
			for(int i = 0; i < mainGame->dInfo.team1; i++) {
				card_info.duelist = i;
				card_info.loc = LOCATION_DECK;
				for(auto card : decks[i].main_deck) {
					card_info.code = card;
					OCG_DuelNewCard(pduel, card_info);
				}
				card_info.loc = LOCATION_EXTRA;
				for(auto card : decks[i].extra_deck) {
					card_info.code = card;
					OCG_DuelNewCard(pduel, card_info);
				}
			}
			card_info.team = 1;
			card_info.con = 1;
			for(int i = 0; i < mainGame->dInfo.team2; i++) {
				card_info.duelist = i;
				card_info.loc = LOCATION_DECK;
				for(auto card : decks[i + mainGame->dInfo.team1].main_deck) {
					card_info.code = card;
					OCG_DuelNewCard(pduel, card_info);
				}
				card_info.loc = LOCATION_EXTRA;
				for(auto card : decks[i + mainGame->dInfo.team1].extra_deck) {
					card_info.code = card;
					OCG_DuelNewCard(pduel, card_info);
				}
			}
			mainGame->dField.Initial(0, decks[0].main_deck.size(), decks[0].extra_deck.size());
			mainGame->dField.Initial(1, decks[mainGame->dInfo.team1].main_deck.size(), decks[mainGame->dInfo.team1].extra_deck.size());
		} else {
			if(!mainGame->LoadScript(pduel, cur_replay.yrp->scriptname))
				return false;
		}
		OCG_StartDuel(pduel);
		return true;
	}

#define DATA (char*)(packet.data.data() + sizeof(uint8_t))

	bool ReplayMode::ReplayAnalyze(CoreUtils::Packet packet) {
		if(packet.message == MSG_SELECT_BATTLECMD || packet.message == MSG_SELECT_IDLECMD)
			ReplayRefresh();
		switch(packet.message) {
			case MSG_SELECT_BATTLECMD:
			case MSG_SELECT_IDLECMD:
			case MSG_SELECT_EFFECTYN:
			case MSG_SELECT_YESNO:
			case MSG_SELECT_OPTION:
			case MSG_SELECT_CARD:
			case MSG_SELECT_TRIBUTE:
			case MSG_SELECT_UNSELECT_CARD:
			case MSG_SELECT_CHAIN:
			case MSG_SELECT_PLACE:
			case MSG_SELECT_DISFIELD:
			case MSG_SELECT_POSITION:
			case MSG_SELECT_COUNTER:
			case MSG_SELECT_SUM:
			case MSG_SORT_CARD:
			case MSG_SORT_CHAIN:
			case MSG_ROCK_PAPER_SCISSORS:
			case MSG_ANNOUNCE_RACE:
			case MSG_ANNOUNCE_ATTRIB:
			case MSG_ANNOUNCE_CARD:
			case MSG_ANNOUNCE_NUMBER: {
				return ReadReplayResponse();
			}
		}
		bool ret = ReplayAnalyze(ReplayPacket((char*)packet.data.data(), (int)(packet.data.size() - sizeof(uint8_t))));
		char* pbuf = DATA;
		int player;
		switch(mainGame->dInfo.curMsg) {
			case MSG_SHUFFLE_DECK: {
				player = BufferIO::Read<uint8_t>(pbuf);
				ReplayRefresh(player, LOCATION_DECK, 0x2181fff);
				break;
			}
			case MSG_SWAP_GRAVE_DECK: {
				player = BufferIO::Read<uint8_t>(pbuf);
				ReplayRefresh(player, LOCATION_GRAVE, 0x2181fff);
				break;
			}
			case MSG_REVERSE_DECK: {
				ReplayRefresh(0, LOCATION_DECK, 0x2181fff);
				ReplayRefresh(1, LOCATION_DECK, 0x2181fff);
				break;
			}
			case MSG_MOVE: {
				pbuf += 4;
				auto previous = CoreUtils::ReadLocInfo(pbuf, false);
				auto current = CoreUtils::ReadLocInfo(pbuf, false);
				if(previous.location && !(current.location & 0x80) && (previous.location != current.location || previous.controler != current.controler))
					ReplayRefreshSingle(current.controler, current.location, current.sequence);
				break;
			}
			case MSG_TAG_SWAP: {
				player = BufferIO::Read<uint8_t>(pbuf);
				ReplayRefresh(player, LOCATION_DECK, 0x2181fff);
				ReplayRefresh(player, LOCATION_EXTRA, 0x2181fff);
				break;
			}
			case MSG_NEW_PHASE:
			case MSG_SUMMONED:
			case MSG_SPSUMMONED:
			case MSG_FLIPSUMMONED:
			case MSG_CHAINED:
			case MSG_CHAIN_SOLVED:
			case MSG_DAMAGE_STEP_START:
			case MSG_DAMAGE_STEP_END: {
				ReplayRefresh();
				break;
			}
		case MSG_CHAIN_END:	{
			ReplayRefresh();
			ReplayRefresh(0, LOCATION_DECK);
			ReplayRefresh(1, LOCATION_DECK);
			break;
		}
			case MSG_RELOAD_FIELD: {
				ReplayReload();
				mainGame->gMutex.lock();
				mainGame->dField.RefreshAllCards();
				mainGame->gMutex.unlock();
				break;
			}
		}
		return ret;
	}
	void ReplayMode::ReplayRefresh(int player, int location, int flag) {
		uint32 len = 0;
		auto buff = OCG_DuelQueryLocation(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32_t)location });
		if(len == 0)
			return;
		mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), location, (char*)buff);
	}
	void ReplayMode::ReplayRefresh(int flag) {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_HAND; loc != LOCATION_GRAVE; loc *= 2)
				ReplayRefresh(p, loc, flag);
	}
	void ReplayMode::ReplayRefreshSingle(int player, int location, int sequence, int flag) {
		uint32 len = 0;
		auto buff = OCG_DuelQuery(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32_t)location, (uint32_t)sequence });
		if(buff == nullptr)
			return;
		mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)buff);
	}
	void ReplayMode::ReplayReload() {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_DECK; loc != LOCATION_OVERLAY; loc <<= 1)
				ReplayRefresh(p, loc, 0x2ffdfff);
	}

}
