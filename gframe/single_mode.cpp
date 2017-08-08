#include "single_mode.h"
#include "duelclient.h"
#include "game.h"
#include "../ocgcore/duel.h"
#include "../ocgcore/field.h"
#include "../ocgcore/mtrandom.h"

namespace ygo {

long SingleMode::pduel = 0;
bool SingleMode::is_closing = false;
bool SingleMode::is_continuing = false;

static byte buffer[0x20000];

bool SingleMode::StartPlay() {
	Thread::NewThread(SinglePlayThread, 0);
	return true;
}
void SingleMode::StopPlay(bool is_exiting) {
	is_closing = is_exiting;
	is_continuing = false;
	mainGame->actionSignal.Set();
	mainGame->singleSignal.Set();
}
void SingleMode::SetResponse(unsigned char* resp) {
	if(!pduel)
		return;
	set_responseb(pduel, resp);
}
int SingleMode::SinglePlayThread(void* param) {
	char fname2[256];
	size_t slen;
	if(open_file) {
		slen = BufferIO::EncodeUTF8(open_file_name, fname2);
		open_file = false;
	} else {
		const wchar_t* name = mainGame->lstSinglePlayList->getListItem(mainGame->lstSinglePlayList->getSelected());
		wchar_t fname[256];
		myswprintf(fname, L"./single/%ls", name);
		slen = BufferIO::EncodeUTF8(fname, fname2);
	}
	mtrandom rnd;
	time_t seed = time(0);
	rnd.reset(seed);
	set_script_reader((script_reader)ScriptReader);
	set_card_reader((card_reader)DataManager::CardReader);
	set_message_handler((message_handler)MessageHandler);
	pduel = create_duel(rnd.rand());
	set_player_info(pduel, 0, 8000, 5, 1);
	set_player_info(pduel, 1, 8000, 5, 1);
	mainGame->dInfo.lp[0] = 8000;
	mainGame->dInfo.lp[1] = 8000;
	myswprintf(mainGame->dInfo.strLP[0], L"%d", mainGame->dInfo.lp[0]);
	myswprintf(mainGame->dInfo.strLP[1], L"%d", mainGame->dInfo.lp[1]);
	BufferIO::CopyWStr(mainGame->ebNickName->getText(), mainGame->dInfo.hostname, 20);
	mainGame->dInfo.clientname[0] = 0;
	mainGame->dInfo.turn = 0;
	if(!preload_script(pduel, fname2, slen)) {
		wchar_t fname[256];
		myswprintf(fname, L"./single/%ls", open_file_name);
		slen = BufferIO::EncodeUTF8(fname, fname2);
		if(!preload_script(pduel, fname2, slen)) {
			end_duel(pduel);
			return 0;
		}
	}
	mainGame->gMutex.Lock();
	mainGame->HideElement(mainGame->wSinglePlay);
	mainGame->wCardImg->setVisible(true);
	mainGame->wInfos->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(dataManager.GetSysString(1210));
	mainGame->stName->setText(L"");
	mainGame->stInfo->setText(L"");
	mainGame->stDataInfo->setText(L"");
	mainGame->stSetName->setText(L"");
	mainGame->stText->setText(L"");
	mainGame->scrCardText->setVisible(false);
	mainGame->wPhase->setVisible(true);
	mainGame->dField.Clear();
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isStarted = true;
	mainGame->dInfo.isSingleMode = true;
	mainGame->device->setEventReceiver(&mainGame->dField);
	mainGame->gMutex.Unlock();
	start_duel(pduel, 0);
	char engineBuffer[0x1000];
	is_closing = false;
	is_continuing = true;
	int len = 0;
	while (is_continuing) {
		int result = process(pduel);
		len = result & 0xffff;
		/* int flag = result >> 16; */
		if (len > 0) {
			get_message(pduel, (byte*)engineBuffer);
			is_continuing = SinglePlayAnalyze(engineBuffer, len);
		}
	}
	end_duel(pduel);
	if(!is_closing) {
		mainGame->gMutex.Lock();
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isSingleMode = false;
		mainGame->gMutex.Unlock();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.Set();
		mainGame->closeDoneSignal.Wait();
		mainGame->gMutex.Lock();
		mainGame->ShowElement(mainGame->wSinglePlay);
		mainGame->stTip->setVisible(false);
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->gMutex.Unlock();
		if(exit_on_return)
			mainGame->device->closeDevice();
	}
	return 0;
}
bool SingleMode::SinglePlayAnalyze(char* msg, unsigned int len) {
	char* offset, *pbuf = msg;
	int player, count;
	while (pbuf - msg < (int)len) {
		if(is_closing || !is_continuing)
			return false;
		offset = pbuf;
		mainGame->dInfo.curMsg = BufferIO::ReadUInt8(pbuf);
		switch (mainGame->dInfo.curMsg) {
		case MSG_RETRY: {
			mainGame->gMutex.Lock();
			mainGame->stMessage->setText(L"Error occurs.");
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.Unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			return false;
		}
		case MSG_HINT: {
			/*int type = */BufferIO::ReadInt8(pbuf);
			int player = BufferIO::ReadInt8(pbuf);
			/*int data = */BufferIO::ReadInt32(pbuf);
			if(player == 0)
				DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_WIN: {
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
			SinglePlayRefresh();
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
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
			SinglePlayRefresh();
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_EFFECTYN: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 12;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_YESNO: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_OPTION: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 4;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 3;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 10 + count * 13;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_POSITION: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_COUNTER: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 9;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 6;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 11;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 7;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_CONFIRM_DECKTOP: {
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
			SinglePlayRefreshDeck(player);
			break;
		}
		case MSG_SHUFFLE_HAND: {
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
			SinglePlayRefreshGrave(player);
			break;
		}
		case MSG_REVERSE_DECK: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefreshDeck(0);
			SinglePlayRefreshDeck(1);
			break;
		}
		case MSG_DECK_TOP: {
			pbuf += 6;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			count = BufferIO::ReadInt8(pbuf);
			pbuf += count * 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_NEW_TURN: {
			player = BufferIO::ReadInt8(pbuf);
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf += 2;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
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
				SinglePlayRefreshSingle(cc, cl, cs);
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
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_SPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_FLIPSUMMONING: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_FLIPSUMMONED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
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
			SinglePlayRefresh();
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_CHAIN_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			SinglePlayRefreshDeck(0);
			SinglePlayRefreshDeck(1);
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
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
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
			break;
		}
		case MSG_ATTACK_DISABLED: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefresh();
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
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_HAND_RES: {
			pbuf += 1;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 5;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_ANNOUNCE_CARD: {
			player = BufferIO::ReadInt8(pbuf);
			pbuf += 4;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
		}
		case MSG_ANNOUNCE_NUMBER:
		case MSG_ANNOUNCE_CARD_FILTER: {
			player = BufferIO::ReadInt8(pbuf);
			count = BufferIO::ReadInt8(pbuf);
			pbuf += 4 * count;
			if(!DuelClient::ClientAnalyze(offset, pbuf - offset)) {
				mainGame->singleSignal.Reset();
				mainGame->singleSignal.Wait();
			}
			break;
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
		case MSG_TAG_SWAP: {
			player = pbuf[0];
			pbuf += pbuf[2] * 4 + pbuf[4] * 4 + 9;
			DuelClient::ClientAnalyze(offset, pbuf - offset);
			SinglePlayRefreshDeck(player);
			SinglePlayRefreshExtra(player);
			break;
		}
		case MSG_MATCH_KILL: {
			pbuf += 4;
			break;
		}
		case MSG_RELOAD_FIELD: {
			mainGame->gMutex.Lock();
			mainGame->dField.Clear();
			mainGame->dInfo.duel_rule = BufferIO::ReadInt8(pbuf);
			int val = 0;
			for(int p = 0; p < 2; ++p) {
				mainGame->dInfo.lp[p] = BufferIO::ReadInt32(pbuf);
				myswprintf(mainGame->dInfo.strLP[p], L"%d", mainGame->dInfo.lp[p]);
				for(int seq = 0; seq < 7; ++seq) {
					val = BufferIO::ReadInt8(pbuf);
					if(val) {
						ClientCard* ccard = new ClientCard;
						mainGame->dField.AddCard(ccard, p, LOCATION_MZONE, seq);
						ccard->position = BufferIO::ReadInt8(pbuf);
						val = BufferIO::ReadInt8(pbuf);
						if(val) {
							for(int xyz = 0; xyz < val; ++xyz) {
								ClientCard* xcard = new ClientCard;
								ccard->overlayed.push_back(xcard);
								mainGame->dField.overlay_cards.insert(xcard);
								xcard->overlayTarget = ccard;
								xcard->location = 0x80;
								xcard->sequence = ccard->overlayed.size() - 1;
							}
						}
					}
				}
				for(int seq = 0; seq < 8; ++seq) {
					val = BufferIO::ReadInt8(pbuf);
					if(val) {
						ClientCard* ccard = new ClientCard;
						mainGame->dField.AddCard(ccard, p, LOCATION_SZONE, seq);
						ccard->position = BufferIO::ReadInt8(pbuf);
					}
				}
				val = BufferIO::ReadInt8(pbuf);
				for(int seq = 0; seq < val; ++seq) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_DECK, seq);
				}
				val = BufferIO::ReadInt8(pbuf);
				for(int seq = 0; seq < val; ++seq) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_HAND, seq);
				}
				val = BufferIO::ReadInt8(pbuf);
				for(int seq = 0; seq < val; ++seq) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_GRAVE, seq);
				}
				val = BufferIO::ReadInt8(pbuf);
				for(int seq = 0; seq < val; ++seq) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_REMOVED, seq);
				}
				val = BufferIO::ReadInt8(pbuf);
				for(int seq = 0; seq < val; ++seq) {
					ClientCard* ccard = new ClientCard;
					mainGame->dField.AddCard(ccard, p, LOCATION_EXTRA, seq);
				}
				val = BufferIO::ReadInt8(pbuf);
				mainGame->dField.extra_p_count[p] = val;
			}
			BufferIO::ReadInt8(pbuf); //chain count, always 0
			SinglePlayReload();
			mainGame->dField.RefreshAllCards();
			mainGame->gMutex.Unlock();
			break;
		}
		case MSG_AI_NAME: {
			char namebuf[128];
			wchar_t wname[128];
			int len = BufferIO::ReadInt16(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			memcpy(namebuf, begin, len + 1);
			BufferIO::DecodeUTF8(namebuf, wname);
			BufferIO::CopyWStr(wname, mainGame->dInfo.clientname, 20);
			break;
		}
		case MSG_SHOW_HINT: {
			char msgbuf[1024];
			wchar_t msg[1024];
			int len = BufferIO::ReadInt16(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			memcpy(msgbuf, begin, len + 1);
			BufferIO::DecodeUTF8(msgbuf, msg);
			mainGame->gMutex.Lock();
			mainGame->SetStaticText(mainGame->stMessage, 310, mainGame->textFont, msg);
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.Unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			break;
		}
		}
	}
	return is_continuing;
}
void SingleMode::SinglePlayRefresh(int flag) {
	unsigned char queryBuffer[0x2000];
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
void SingleMode::SinglePlayRefreshHand(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_HAND, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_HAND, (char*)queryBuffer);
}
void SingleMode::SinglePlayRefreshGrave(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_GRAVE, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_GRAVE, (char*)queryBuffer);
}
void SingleMode::SinglePlayRefreshDeck(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_DECK, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_DECK, (char*)queryBuffer);
}
void SingleMode::SinglePlayRefreshExtra(int player, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_field_card(pduel, player, LOCATION_EXTRA, flag, queryBuffer, 0);
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_EXTRA, (char*)queryBuffer);
}
void SingleMode::SinglePlayRefreshSingle(int player, int location, int sequence, int flag) {
	unsigned char queryBuffer[0x2000];
	/*int len = */query_card(pduel, player, location, sequence, flag, queryBuffer, 0);
	mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)queryBuffer);
}
void SingleMode::SinglePlayReload() {
	unsigned char queryBuffer[0x2000];
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
byte* SingleMode::ScriptReader(const char* script_name, int* slen) {
	FILE *fp;
#ifdef _WIN32
	wchar_t fname[256];
	BufferIO::DecodeUTF8(script_name, fname);
	fp = _wfopen(fname, L"rb");
#else
	fp = fopen(script_name, "rb");
#endif
	if(!fp)
		return 0;
	int len = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	if(len >= sizeof(buffer))
		return 0;
	*slen = len;
	return buffer;
}
int SingleMode::MessageHandler(long fduel, int type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, (byte*)msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}

}
