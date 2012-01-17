#include "game.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"
#include "../ocgcore/duel.h"
#include "../ocgcore/field.h"

#include <algorithm>

#define MSG_REPLAY 		0xf0
#define MSG_DUEL_END	0xf1

namespace ygo {

int Game::LocalPlayer(int player) {
	return dInfo.is_first_turn ? player : 1 - player;
}
const wchar_t* Game::LocalName(int local_player) {
	return local_player == 0 ? dInfo.hostname : dInfo.clientname;
}
/*
void Game::ReplayRefresh(int flag) {
	int len = query_field_card(dInfo.pDuel, 0, LOCATION_MZONE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_MZONE, queryBuffer);
	len = query_field_card(dInfo.pDuel, 1, LOCATION_MZONE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_MZONE, queryBuffer);
	len = query_field_card(dInfo.pDuel, 0, LOCATION_SZONE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_SZONE, queryBuffer);
	len = query_field_card(dInfo.pDuel, 1, LOCATION_SZONE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_SZONE, queryBuffer);
	len = query_field_card(dInfo.pDuel, 0, LOCATION_HAND, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(0), LOCATION_HAND, queryBuffer);
	len = query_field_card(dInfo.pDuel, 1, LOCATION_HAND, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(1), LOCATION_HAND, queryBuffer);
}
void  Game::ReplayRefreshHand(int player, int flag) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_HAND, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_HAND, queryBuffer);
}
void Game::ReplayRefreshGrave(int player, int flag) {
	int len = query_field_card(dInfo.pDuel, 0, LOCATION_GRAVE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_GRAVE, queryBuffer);
}
void Game::ReplayRefreshSingle(int player, int location, int sequence, int flag) {
	int len = query_card(dInfo.pDuel, player, location, sequence, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, queryBuffer);
}

int Game::ReplayThread(void* pd) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	Replay& rep = mainGame->lastReplay;
	ReplayHeader rh = mainGame->lastReplay.pheader;
	pdInfo->is_local_host = true;
	int seed = rh.seed;
	mainGame->rnd.reset(seed);
	pdInfo->is_host_player[0] = true;
	pdInfo->is_host_player[1] = false;
	pdInfo->is_first_turn = true;
	mainGame->lastReplay.ReadData(pdInfo->hostname, 40);
	mainGame->lastReplay.ReadData(pdInfo->clientname, 40);

	mainGame->gMutex.Lock();
	mainGame->imgCard->setImage(mainGame->imageManager.tCover);
	mainGame->wCardImg->setVisible(true);
	mainGame->wInfos->setVisible(true);
	mainGame->wReplay->setVisible(true);
	mainGame->stName->setText(L"");
	mainGame->stInfo->setText(L"");
	mainGame->stDataInfo->setText(L"");
	mainGame->stText->setText(L"");
	mainGame->btnReplayStart->setVisible(false);
	mainGame->btnReplayPause->setVisible(true);
	mainGame->btnReplayStep->setVisible(false);
	mainGame->dInfo.engLen = 0;
	mainGame->dInfo.msgLen = 0;
	mainGame->dInfo.is_local_host = false;
	mainGame->dField.panel = 0;
	mainGame->dField.hovered_card = 0;
	mainGame->dField.clicked_card = 0;
	mainGame->gMutex.Unlock();
	mainGame->WaitFrameSignal(10);

	set_card_reader((card_reader)Game::CardReader);
	set_message_handler((message_handler)Game::MessageHandler);
	pdInfo->pDuel = create_duel(mainGame->rnd.rand());
	int start_lp = mainGame->lastReplay.ReadInt32();
	int start_hand = mainGame->lastReplay.ReadInt32();
	int draw_count = mainGame->lastReplay.ReadInt32();
	int opt = mainGame->lastReplay.ReadInt32();
	set_player_info(pdInfo->pDuel, 0, start_lp, start_hand, draw_count);
	set_player_info(pdInfo->pDuel, 1, start_lp, start_hand, draw_count);
	pdInfo->lp[0] = start_lp;
	pdInfo->lp[1] = start_lp;
	myswprintf(pdInfo->strLP[0], L"%d", pdInfo->lp[0]);
	myswprintf(pdInfo->strLP[1], L"%d", pdInfo->lp[1]);
	pdInfo->turn = 0;
	pdInfo->strTurn[0] = 0;
	mainGame->dField.Clear();
	int main = rep.ReadInt32();
	for(int i = 0; i < main; ++i)
		new_card(pdInfo->pDuel, rep.ReadInt32(), 0, 0, LOCATION_DECK, 0, 0);
	int extra = rep.ReadInt32();
	for(int i = 0; i < extra; ++i)
		new_card(pdInfo->pDuel, rep.ReadInt32(), 0, 0, LOCATION_EXTRA, 0, 0);
	mainGame->dField.Initial(0, main, extra);
	main = rep.ReadInt32();
	for(int i = 0; i < main; ++i)
		new_card(pdInfo->pDuel, rep.ReadInt32(), 1, 1, LOCATION_DECK, 0, 0);
	extra = rep.ReadInt32();
	for(int i = 0; i < extra; ++i)
		new_card(pdInfo->pDuel, rep.ReadInt32(), 1, 1, LOCATION_EXTRA, 0, 0);
	mainGame->dField.Initial(1, main, extra);

	start_duel(pdInfo->pDuel, opt);
	mainGame->dField.is_replaying = true;
	mainGame->dField.is_pausing = false;
	mainGame->dField.is_paused = false;
	mainGame->dField.is_swaping = false;
	pdInfo->isStarted = true;
	char engineBuffer[0x1000];
	pdInfo->engFlag = 0;
	bool replaying = true;
	while (replaying && mainGame->dField.is_replaying && pdInfo->isStarted && !mainGame->is_closing) {
		if (pdInfo->engFlag == 2) {
			pdInfo->engFlag = 0;
			break;
		}
		int result = process(pdInfo->pDuel);
		pdInfo->engLen = result & 0xffff;
		pdInfo->engFlag = result >> 16;
		if (pdInfo->engLen > 0) {
			get_message(pdInfo->pDuel, (byte*)&engineBuffer[2]);
			replaying = AnalyzeReplay(pd, engineBuffer);
		}
	}
	end_duel(pdInfo->pDuel);
	if(!mainGame->is_closing) {
		mainGame->stMessage->setText(L"录像结束");
		mainGame->localAction.Reset();
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->localAction.Wait();
	}
	pdInfo->isStarted = false;
	mainGame->dField.is_replaying = false;
	if(!mainGame->is_closing) {
		mainGame->gMutex.Lock();
		mainGame->wCardImg->setVisible(false);
		mainGame->stHintMsg->setVisible(false);
		mainGame->stTip->setVisible(false);
		mainGame->wInfos->setVisible(false);
		mainGame->wReplay->setVisible(false);
		mainGame->btnDP->setVisible(false);
		mainGame->btnSP->setVisible(false);
		mainGame->btnM1->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->lstLog->clear();
		mainGame->logParam.clear();
		mainGame->imgCard->setImage(0);
		mainGame->stName->setText(L"");
		mainGame->stDataInfo->setText(L"");
		mainGame->stText->setText(L"");
		mainGame->imageManager.ClearTexture();
		mainGame->gMutex.Unlock();
	}
	return 0;
}
bool Game::AnalyzeReplay(void* pd, char* engbuf) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	Replay& rep = mainGame->lastReplay;
	char* offset, *pbufw, *pbuf = &engbuf[2];
	int player, count, type;
	bool pauseable;
	while (pbuf - &engbuf[2] < pdInfo->engLen) {
		if(mainGame->is_closing)
			return false;
		if(mainGame->dField.is_swaping) {
			mainGame->gMutex.Lock();
			mainGame->dField.ReplaySwap();
			mainGame->gMutex.Unlock();
			mainGame->dField.is_swaping = false;
		}
		offset = pbuf;
		pauseable = true;
		pdInfo->engType = NetManager::ReadUInt8(pbuf);
		switch (pdInfo->engType) {
		case MSG_RETRY: {
			SolveMessage(pd, offset, pbuf - offset);
			pdInfo->engFlag = 2;
			return false;
		}
		case MSG_HINT: {
			pbuf += 6;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_WIN: {
			pbuf += 2;
			SolveMessage(pd, offset, pbuf - offset);
			return false;
		}
		case MSG_SELECT_BATTLECMD: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 11;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8 + 2;
			mainGame->ReplayRefresh();
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_IDLECMD: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 11 + 2;
			mainGame->ReplayRefresh();
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_EFFECTYN: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 8;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_YESNO: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 4;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_OPTION: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 3;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_CHAIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += 9 + count * 11;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_POSITION: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_COUNTER: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 3;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8;
			return rep.ReadNextResponse();
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 11;
			return rep.ReadNextResponse();
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			return rep.ReadNextResponse();
		}
		case MSG_CONFIRM_DECKTOP: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			pauseable = false;
			break;
		}
		case MSG_SHUFFLE_DECK: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			int oplayer = NetManager::ReadInt8(pbuf);
			player = mainGame->LocalPlayer(oplayer);
			mainGame->WaitFrameSignal(5);
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / 5, 0, 0);
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->is_hovered = false;
				(*cit)->aniFrame = 5;
			}
			mainGame->WaitFrameSignal(10);
			mainGame->ReplayRefreshHand(oplayer);
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->is_hovered = false;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = NetManager::ReadInt8(pbuf);
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefreshGrave(player);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_NEW_TURN: {
			pbuf++;
			mainGame->ReplayRefresh();
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			break;
		}
		case MSG_MOVE: {
			int pc = pbuf[4];
			int pl = pbuf[5];
			int ps = pbuf[6];
			int pp = pbuf[7];
			int cc = pbuf[8];
			int cl = pbuf[9];
			int cs = pbuf[10];
			int cp = pbuf[11];
			pbuf += 16;
			SolveMessage(pd, offset, pbuf - offset);
			if(pl != cl || pc != cc)
				mainGame->ReplayRefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_POS_CHANGE: {
			pbuf += 9;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_SET: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SWAP: {
			pbuf += 16;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SUMMONED: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_SPSUMMONED: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			break;
		}
		case MSG_FLIPSUMMONING: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_FLIPSUMMONED: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			break;
		}
		case MSG_CHAINING: {
			pbuf += 16;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_CHAIN_END: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_CHAIN_INACTIVATED: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_CARD_SELECTED:
		case MSG_RANDOM_SELECTED: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_BECOME_TARGET: {
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_DRAW: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 6;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 6;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 18;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_ATTACK_DISABLED: {
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			SolveMessage(pd, offset, pbuf - offset);
			mainGame->ReplayRefresh();
			pauseable = false;
			break;
		}
		case MSG_MISSED_EFFECT: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_DICE: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			return rep.ReadNextResponse();
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			return rep.ReadNextResponse();
		}
		case MSG_ANNOUNCE_CARD: {
			player = NetManager::ReadInt8(pbuf);
			return rep.ReadNextResponse();
		}
		case MSG_ANNOUNCE_NUMBER: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += 4 * count;
			return rep.ReadNextResponse();
		}
		case MSG_COUNT_TURN: {
			pbuf += 6;
			SolveMessage(pd, offset, pbuf - offset);
			break;
		}
		}
		if(pauseable && mainGame->dField.is_pausing) {
			mainGame->dField.is_paused = true;
			mainGame->localAction.Reset();
			mainGame->localAction.Wait();
		}
	}
	return true;
}
*/
}
