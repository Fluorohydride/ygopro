#include "replay_mode.h"
#include <fmt/format.h>
#include "RNG/mt19937.h"
#include "duelclient.h"
#include "game.h"
#include "single_mode.h"

namespace ygo {
	bool ReplayMode::ReadReplayResponse() {
		ReplayResponse res;
		bool result = cur_yrp->GetNextResponse(&res);
		if (result)
			OCG_DuelSetResponse(pduel, res.response.data(), res.length);
		return result;
	}
	int ReplayMode::OldReplayThread() {
		Utils::SetThreadName("OldReplay");
		mainGame->dInfo.isReplay = true;
		mainGame->dInfo.isOldReplay = true;
		if(!cur_yrp) {
			EndDuel();
			return 0;
		}
		const ReplayHeader& rh = cur_yrp->pheader;
		mainGame->dInfo.isFirst = true;
		mainGame->dInfo.isTeam1 = true;
		mainGame->dInfo.isRelay = !!(cur_yrp->params.duel_flags & DUEL_RELAY);
		mainGame->dInfo.isSingleMode = !!(rh.flag & REPLAY_SINGLE_MODE);
		mainGame->dInfo.isHandTest = !!(rh.flag & REPLAY_HAND_TEST);
		mainGame->dInfo.compat_mode = false;
		mainGame->dInfo.current_player[0] = 0;
		mainGame->dInfo.current_player[1] = 0;
		mainGame->dInfo.opponames.clear();
		mainGame->dInfo.selfnames.clear();
		mainGame->dInfo.team1 = ReplayMode::cur_yrp->GetPlayersCount(0);
		mainGame->dInfo.team2 = ReplayMode::cur_yrp->GetPlayersCount(1);
		if(!mainGame->dInfo.isRelay)
			mainGame->dInfo.current_player[1] = mainGame->dInfo.team2 - 1;
		if (!StartDuel()) {
			EndDuel();
			return 0;
		}
		mainGame->dInfo.isInDuel = true;
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isOldReplay = true;
		mainGame->dInfo.checkRematch = false;
		mainGame->SetMessageWindow();
		mainGame->dInfo.isCatchingUp = (skip_turn > 0);
		is_continuing = true;
		skip_step = 0;
		if (mainGame->dInfo.isSingleMode) {
			for(const auto& message : CoreUtils::ParseMessages(pduel))
				is_continuing = OldReplayAnalyze(message) && is_continuing;
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
			for(const auto& message : CoreUtils::ParseMessages(pduel)) {
				if(is_restarting || !is_continuing)
					break;
				is_continuing = OldReplayAnalyze(message) && is_continuing;
			}
			if(is_restarting) {
				mainGame->gMutex.lock();
				is_restarting = false;
				int step = current_step - 1;
				if(step < 0)
					step = 0;
				if(mainGame->dInfo.isSingleMode) {
					is_continuing = true;
					skip_step = 0;
					for(const auto& message : CoreUtils::ParseMessages(pduel)) {
						if(is_restarting || !is_continuing)
							break;
						is_continuing = OldReplayAnalyze(message) && is_continuing;
					}
				}
				if(step == 0) {
					Pause(true, false);
					mainGame->dInfo.isInDuel = true;
					mainGame->dInfo.isStarted = true;
					mainGame->dInfo.checkRematch = false;
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
		const ReplayHeader& rh = cur_yrp->pheader;
		uint32_t seed = rh.seed;
		if(!(rh.flag & REPLAY_DIRECT_SEED))
			seed = RNG::mt19937(seed)();
		const auto& names = ReplayMode::cur_yrp->GetPlayerNames();
		mainGame->dInfo.selfnames.clear();
		mainGame->dInfo.opponames.clear();
		mainGame->dInfo.selfnames.insert(mainGame->dInfo.selfnames.end(), names.begin(), names.begin() + ReplayMode::cur_yrp->GetPlayersCount(0));
		mainGame->dInfo.opponames.insert(mainGame->dInfo.opponames.end(), names.begin() + ReplayMode::cur_yrp->GetPlayersCount(0), names.end());
		uint32_t start_lp = cur_yrp->params.start_lp;
		uint32_t start_hand = cur_yrp->params.start_hand;
		uint32_t draw_count = cur_yrp->params.draw_count;
		OCG_Player team = { start_lp, start_hand, draw_count };
		pduel = mainGame->SetupDuel({ seed, cur_yrp->params.duel_flags, team, team });
		mainGame->dInfo.duel_params = cur_yrp->params.duel_flags;
		mainGame->dInfo.duel_field = mainGame->GetMasterRule(mainGame->dInfo.duel_params);
		mainGame->SetPhaseButtons();
		mainGame->dInfo.lp[0] = start_lp;
		mainGame->dInfo.lp[1] = start_lp;
		mainGame->dInfo.startlp = start_lp;
		mainGame->dInfo.strLP[0] = fmt::to_wstring(start_lp);
		mainGame->dInfo.strLP[1] = mainGame->dInfo.strLP[0];
		mainGame->dInfo.turn = 0;
		if (!mainGame->dInfo.isSingleMode || (rh.flag & REPLAY_HAND_TEST)) {
			auto rule_cards = cur_yrp->GetRuleCards();
			OCG_NewCardInfo card_info = { 0, 0, 0, 0, 0, 0, POS_FACEDOWN_DEFENSE };
			for(auto card : rule_cards) {
				card_info.code = card;
				OCG_DuelNewCard(pduel, card_info);
			}
			auto decks = cur_yrp->GetPlayerDecks();
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
			if(rh.flag & REPLAY_HAND_TEST) {
				const char cmd[] = "Debug.ReloadFieldEnd()";
				OCG_LoadScript(pduel, cmd, sizeof(cmd) - 1, " ");
			} else {
				mainGame->dField.Initial(0, decks[0].main_deck.size(), decks[0].extra_deck.size());
				mainGame->dField.Initial(1, decks[mainGame->dInfo.team1].main_deck.size(), decks[mainGame->dInfo.team1].extra_deck.size());
			}
		} else {
			if(!mainGame->LoadScript(pduel, cur_yrp->scriptname))
				return false;
		}
		OCG_StartDuel(pduel);
		return true;
	}

	bool ReplayMode::OldReplayAnalyze(const CoreUtils::Packet& packet) {
		if(packet.message == MSG_SELECT_BATTLECMD || packet.message == MSG_SELECT_IDLECMD || packet.message == MSG_SELECT_CHAIN)
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
		if(!ReplayAnalyze(packet))
			return false;
		const auto* pbuf = packet.data();
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
				if(!mainGame->dInfo.isCatchingUp)
					mainGame->gMutex.lock();
				mainGame->dField.RefreshAllCards();
				if(!mainGame->dInfo.isCatchingUp)
					mainGame->gMutex.unlock();
				break;
			}
		}
		return true;
	}
	void ReplayMode::ReplayRefresh(uint8_t player, uint8_t location, uint32_t flag) {
		uint32_t len = 0;
		auto buff = static_cast<uint8_t*>(OCG_DuelQueryLocation(pduel, &len, { flag, player, location }));
		if(len == 0)
			return;
		mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), location, buff);
	}
	void ReplayMode::ReplayRefresh(uint32_t flag) {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_HAND; loc != LOCATION_GRAVE; loc *= 2)
				ReplayRefresh(p, loc, flag);
	}
	void ReplayMode::ReplayRefreshSingle(uint8_t player, uint8_t location, uint32_t sequence, uint32_t flag) {
		uint32_t len = 0;
		auto buff = static_cast<uint8_t*>(OCG_DuelQuery(pduel, &len, { flag, player, location, sequence }));
		if(buff == nullptr)
			return;
		mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, buff);
	}
	void ReplayMode::ReplayReload() {
		for(int p = 0; p < 2; p++)
			for(int loc = LOCATION_DECK; loc != LOCATION_OVERLAY; loc <<= 1)
				ReplayRefresh(p, loc, 0x2ffdfff);
	}

}
