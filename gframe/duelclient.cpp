#include "duelclient.h"
#include "client_card.h"
#include "materials.h"
#include "image_manager.h"
#include "sound_manager.h"
#include "single_mode.h"
#include "../ocgcore/common.h"
#include "game.h"
#include "replay.h"
#include "replay_mode.h"

namespace ygo {

unsigned DuelClient::connect_state = 0;
unsigned char DuelClient::response_buf[64];
unsigned char DuelClient::response_len = 0;
unsigned int DuelClient::watching = 0;
unsigned char DuelClient::selftype = 0;
bool DuelClient::is_host = false;
event_base* DuelClient::client_base = 0;
bufferevent* DuelClient::client_bev = 0;
char DuelClient::duel_client_read[0x2000];
char DuelClient::duel_client_write[0x2000];
bool DuelClient::is_closing = false;
bool DuelClient::is_swapping = false;
int DuelClient::select_hint = 0;
int DuelClient::select_unselect_hint = 0;
int DuelClient::last_select_hint = 0;
char DuelClient::last_successful_msg[0x2000];
unsigned int DuelClient::last_successful_msg_length = 0;
wchar_t DuelClient::event_string[256];
mtrandom DuelClient::rnd;

bool DuelClient::is_refreshing = false;
int DuelClient::match_kill = 0;
std::vector<HostPacket> DuelClient::hosts;
std::set<unsigned int> DuelClient::remotes;
event* DuelClient::resp_event = 0;

bool DuelClient::StartClient(unsigned int ip, unsigned short port, bool create_game) {
	if(connect_state)
		return false;
	sockaddr_in sin;
	client_base = event_base_new();
	if(!client_base)
		return false;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(ip);
	sin.sin_port = htons(port);
	client_bev = bufferevent_socket_new(client_base, -1, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(client_bev, ClientRead, NULL, ClientEvent, (void*)create_game);
	if (bufferevent_socket_connect(client_bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
		bufferevent_free(client_bev);
		event_base_free(client_base);
		client_bev = 0;
		client_base = 0;
		return false;
	}
	connect_state = 0x1;
	rnd.reset(time(0));
	if(!create_game) {
		timeval timeout = {5, 0};
		event* resp_event = event_new(client_base, 0, EV_TIMEOUT, ConnectTimeout, 0);
		event_add(resp_event, &timeout);
	}
	std::thread(ClientThread).detach();
	return true;
}
void DuelClient::ConnectTimeout(evutil_socket_t fd, short events, void* arg) {
	if(connect_state == 0x7)
		return;
	if(!is_closing) {
		mainGame->btnCreateHost->setEnabled(true);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		mainGame->btnStartBot->setEnabled(true);
		mainGame->btnBotCancel->setEnabled(true);
		mainGame->gMutex.lock();
		if(bot_mode && !mainGame->wSinglePlay->isVisible())
			mainGame->ShowElement(mainGame->wSinglePlay);
		else if(!bot_mode && !mainGame->wLanWindow->isVisible())
			mainGame->ShowElement(mainGame->wLanWindow);
		soundManager.PlaySoundEffect(SOUND_INFO);
		mainGame->env->addMessageBox(L"", dataManager.GetSysString(1400));
		mainGame->gMutex.unlock();
	}
	event_base_loopbreak(client_base);
}
void DuelClient::StopClient(bool is_exiting) {
	if(connect_state != 0x7)
		return;
	is_closing = is_exiting;
	if(!is_closing) {

	}
	event_base_loopbreak(client_base);
}
void DuelClient::ClientRead(bufferevent* bev, void* ctx) {
	evbuffer* input = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(input);
	unsigned short packet_len = 0;
	while(true) {
		if(len < 2)
			return;
		evbuffer_copyout(input, &packet_len, 2);
		if(len < (size_t)packet_len + 2)
			return;
		evbuffer_remove(input, duel_client_read, packet_len + 2);
		if(packet_len)
			HandleSTOCPacketLan(&duel_client_read[2], packet_len);
		len -= packet_len + 2;
	}
}
void DuelClient::ClientEvent(bufferevent *bev, short events, void *ctx) {
	if (events & BEV_EVENT_CONNECTED) {
		bool create_game = (size_t)ctx != 0;
		CTOS_PlayerInfo cspi;
		BufferIO::CopyWStr(mainGame->ebNickName->getText(), cspi.name, 20);
		SendPacketToServer(CTOS_PLAYER_INFO, cspi);
		if(create_game) {
			CTOS_CreateGame cscg;
			if(bot_mode) {
				BufferIO::CopyWStr(L"Bot Game", cscg.name, 20);
				BufferIO::CopyWStr(L"", cscg.pass, 20);
				cscg.info.rule = 2;
				cscg.info.mode = 0;
				cscg.info.start_hand = 5;
				cscg.info.start_lp = 8000;
				cscg.info.draw_count = 1;
				cscg.info.time_limit = 0;
				cscg.info.lflist = 0;
				cscg.info.duel_rule = mainGame->cbBotRule->getSelected() + 3;
				cscg.info.no_check_deck = mainGame->chkBotNoCheckDeck->isChecked();
				cscg.info.no_shuffle_deck = mainGame->chkBotNoShuffleDeck->isChecked();
			}
			else {
				BufferIO::CopyWStr(mainGame->ebServerName->getText(), cscg.name, 20);
				BufferIO::CopyWStr(mainGame->ebServerPass->getText(), cscg.pass, 20);
				cscg.info.rule = mainGame->cbRule->getSelected();
				cscg.info.mode = mainGame->cbMatchMode->getSelected();
				cscg.info.start_hand = _wtoi(mainGame->ebStartHand->getText());
				cscg.info.start_lp = _wtoi(mainGame->ebStartLP->getText());
				cscg.info.draw_count = _wtoi(mainGame->ebDrawCount->getText());
				cscg.info.time_limit = _wtoi(mainGame->ebTimeLimit->getText());
				cscg.info.lflist = mainGame->cbHostLFlist->getItemData(mainGame->cbHostLFlist->getSelected());
				cscg.info.duel_rule = mainGame->cbDuelRule->getSelected() + 1;
				cscg.info.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
				cscg.info.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
			}
			SendPacketToServer(CTOS_CREATE_GAME, cscg);
		} else {
			CTOS_JoinGame csjg;
			csjg.version = PRO_VERSION;
			csjg.gameid = 0;
			BufferIO::CopyWStr(mainGame->ebJoinPass->getText(), csjg.pass, 20);
			SendPacketToServer(CTOS_JOIN_GAME, csjg);
		}
		bufferevent_enable(bev, EV_READ);
		connect_state |= 0x2;
	} else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
		bufferevent_disable(bev, EV_READ);
		if(!is_closing) {
			if(connect_state == 0x1) {
				mainGame->btnCreateHost->setEnabled(true);
				mainGame->btnJoinHost->setEnabled(true);
				mainGame->btnJoinCancel->setEnabled(true);
				mainGame->btnStartBot->setEnabled(true);
				mainGame->btnBotCancel->setEnabled(true);
				mainGame->gMutex.lock();
				if(bot_mode && !mainGame->wSinglePlay->isVisible())
					mainGame->ShowElement(mainGame->wSinglePlay);
				else if(!bot_mode && !mainGame->wLanWindow->isVisible())
					mainGame->ShowElement(mainGame->wLanWindow);
				soundManager.PlaySoundEffect(SOUND_INFO);
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1400));
				mainGame->gMutex.unlock();
			} else if(connect_state == 0x7) {
				if(!mainGame->dInfo.isStarted && !mainGame->is_building) {
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->btnStartBot->setEnabled(true);
					mainGame->btnBotCancel->setEnabled(true);
					mainGame->gMutex.lock();
					mainGame->HideElement(mainGame->wHostPrepare);
					if(bot_mode)
						mainGame->ShowElement(mainGame->wSinglePlay);
					else
						mainGame->ShowElement(mainGame->wLanWindow);
					mainGame->wChat->setVisible(false);
					soundManager.PlaySoundEffect(SOUND_INFO);
					if(events & BEV_EVENT_EOF)
						mainGame->env->addMessageBox(L"", dataManager.GetSysString(1401));
					else mainGame->env->addMessageBox(L"", dataManager.GetSysString(1402));
					mainGame->gMutex.unlock();
				} else {
					mainGame->gMutex.lock();
					soundManager.PlaySoundEffect(SOUND_INFO);
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1502));
					mainGame->btnCreateHost->setEnabled(true);
					mainGame->btnJoinHost->setEnabled(true);
					mainGame->btnJoinCancel->setEnabled(true);
					mainGame->btnStartBot->setEnabled(true);
					mainGame->btnBotCancel->setEnabled(true);
					mainGame->stTip->setVisible(false);
					mainGame->gMutex.unlock();
					mainGame->closeDoneSignal.Reset();
					mainGame->closeSignal.Set();
					mainGame->closeDoneSignal.Wait();
					mainGame->gMutex.lock();
					mainGame->dInfo.isStarted = false;
					mainGame->dInfo.isFinished = false;
					mainGame->is_building = false;
					mainGame->device->setEventReceiver(&mainGame->menuHandler);
					if(bot_mode)
						mainGame->ShowElement(mainGame->wSinglePlay);
					else
						mainGame->ShowElement(mainGame->wLanWindow);
					mainGame->gMutex.unlock();
				}
			}
		}
		event_base_loopexit(client_base, 0);
	}
}
int DuelClient::ClientThread() {
	event_base_dispatch(client_base);
	bufferevent_free(client_bev);
	event_base_free(client_base);
	client_bev = 0;
	client_base = 0;
	connect_state = 0;
	return 0;
}
void DuelClient::HandleSTOCPacketLan(char* data, unsigned int len) {
	char* pdata = data;
	unsigned char pktType = BufferIO::ReadUInt8(pdata);
	switch(pktType) {
	case STOC_GAME_MSG: {
		ClientAnalyze(pdata, len - 1);
		break;
	}
	case STOC_ERROR_MSG: {
		STOC_ErrorMsg* pkt = (STOC_ErrorMsg*)pdata;
		switch(pkt->msg) {
		case ERRMSG_JOINERROR: {
			mainGame->btnCreateHost->setEnabled(true);
			mainGame->btnJoinHost->setEnabled(true);
			mainGame->btnJoinCancel->setEnabled(true);
			mainGame->btnStartBot->setEnabled(true);
			mainGame->btnBotCancel->setEnabled(true);
			mainGame->gMutex.lock();
			soundManager.PlaySoundEffect(SOUND_INFO);
			if(pkt->code == 0)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1403));
			else if(pkt->code == 1)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1404));
			else if(pkt->code == 2)
				mainGame->env->addMessageBox(L"", dataManager.GetSysString(1405));
			mainGame->gMutex.unlock();
			event_base_loopbreak(client_base);
			break;
		}
		case ERRMSG_DECKERROR: {
			mainGame->gMutex.lock();
			unsigned int code = pkt->code & 0xFFFFFFF;
			int flag = pkt->code >> 28;
			wchar_t msgbuf[256];
			switch(flag)
			{
			case DECKERROR_LFLIST: {
				myswprintf(msgbuf, dataManager.GetSysString(1407), dataManager.GetName(code));
				break;
			}
			case DECKERROR_OCGONLY: {
				myswprintf(msgbuf, dataManager.GetSysString(1413), dataManager.GetName(code));
				break;
			}
			case DECKERROR_TCGONLY: {
				myswprintf(msgbuf, dataManager.GetSysString(1414), dataManager.GetName(code));
				break;
			}
			case DECKERROR_UNKNOWNCARD: {
				myswprintf(msgbuf, dataManager.GetSysString(1415), dataManager.GetName(code), code);
				break;
			}
			case DECKERROR_CARDCOUNT: {
				myswprintf(msgbuf, dataManager.GetSysString(1416), dataManager.GetName(code));
				break;
			}
			case DECKERROR_MAINCOUNT: {
				myswprintf(msgbuf, dataManager.GetSysString(1417), code);
				break;
			}
			case DECKERROR_EXTRACOUNT: {
				if(code>0)
					myswprintf(msgbuf, dataManager.GetSysString(1418), code);
				else
					myswprintf(msgbuf, dataManager.GetSysString(1420));
				break;
			}
			case DECKERROR_SIDECOUNT: {
				myswprintf(msgbuf, dataManager.GetSysString(1419), code);
				break;
			}
			default: {
				myswprintf(msgbuf, dataManager.GetSysString(1406));
				break;
			}
			}
			soundManager.PlaySoundEffect(SOUND_INFO);
			mainGame->env->addMessageBox(L"", msgbuf);
			mainGame->cbCategorySelect->setEnabled(true);
			mainGame->cbDeckSelect->setEnabled(true);
			mainGame->gMutex.unlock();
			break;
		}
		case ERRMSG_SIDEERROR: {
			mainGame->gMutex.lock();
			soundManager.PlaySoundEffect(SOUND_INFO);
			mainGame->env->addMessageBox(L"", dataManager.GetSysString(1408));
			mainGame->gMutex.unlock();
			break;
		}
		case ERRMSG_VERERROR: {
			mainGame->btnCreateHost->setEnabled(true);
			mainGame->btnJoinHost->setEnabled(true);
			mainGame->btnJoinCancel->setEnabled(true);
			mainGame->btnStartBot->setEnabled(true);
			mainGame->btnBotCancel->setEnabled(true);
			mainGame->gMutex.lock();
			wchar_t msgbuf[256];
			myswprintf(msgbuf, dataManager.GetSysString(1411), pkt->code >> 12, (pkt->code >> 4) & 0xff, pkt->code & 0xf);
			soundManager.PlaySoundEffect(SOUND_INFO);
			mainGame->env->addMessageBox(L"", msgbuf);
			mainGame->gMutex.unlock();
			event_base_loopbreak(client_base);
			break;
		}
		}
		break;
	}
	case STOC_SELECT_HAND: {
		mainGame->wHand->setVisible(true);
		break;
	}
	case STOC_SELECT_TP: {
		mainGame->gMutex.lock();
		mainGame->PopupElement(mainGame->wFTSelect);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HAND_RESULT: {
		STOC_HandResult* pkt = (STOC_HandResult*)pdata;
		mainGame->stHintMsg->setVisible(false);
		mainGame->showcardcode = (pkt->res1 - 1) + ((pkt->res2 - 1) << 16);
		mainGame->showcarddif = 50;
		mainGame->showcardp = 0;
		mainGame->showcard = 100;
		mainGame->WaitFrameSignal(60);
		break;
	}
	case STOC_TP_RESULT: {
		break;
	}
	case STOC_CHANGE_SIDE: {
		mainGame->gMutex.lock();
		mainGame->dInfo.isStarted = false;
		mainGame->dField.Clear();
		mainGame->is_building = true;
		mainGame->is_siding = true;
		mainGame->CloseGameWindow();
		mainGame->wChat->setVisible(false);
		mainGame->wDeckEdit->setVisible(false);
		mainGame->wFilter->setVisible(false);
		mainGame->wSort->setVisible(false);
		if(mainGame->dInfo.player_type < 7)
			mainGame->btnLeaveGame->setVisible(false);
		mainGame->btnSideOK->setVisible(true);
		mainGame->btnSideShuffle->setVisible(true);
		mainGame->btnSideSort->setVisible(true);
		mainGame->btnSideReload->setVisible(true);
		mainGame->deckBuilder.result_string[0] = L'0';
		mainGame->deckBuilder.result_string[1] = 0;
		mainGame->deckBuilder.results.clear();
		mainGame->deckBuilder.hovered_code = 0;
		mainGame->deckBuilder.is_draging = false;
		mainGame->deckBuilder.is_starting_dragging = false;
		mainGame->deckBuilder.pre_mainc = deckManager.current_deck.main.size();
		mainGame->deckBuilder.pre_extrac = deckManager.current_deck.extra.size();
		mainGame->deckBuilder.pre_sidec = deckManager.current_deck.side.size();
		mainGame->device->setEventReceiver(&mainGame->deckBuilder);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_WAITING_SIDE: {
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->stHintMsg->setText(dataManager.GetSysString(1409));
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_DECK_COUNT: {
		mainGame->gMutex.lock();
		int deckc = BufferIO::ReadInt16(pdata);
		int extrac = BufferIO::ReadInt16(pdata);
		int sidec = BufferIO::ReadInt16(pdata);
		mainGame->dField.Initial(0, deckc, extrac);
		deckc = BufferIO::ReadInt16(pdata);
		extrac = BufferIO::ReadInt16(pdata);
		sidec = BufferIO::ReadInt16(pdata);
		mainGame->dField.Initial(1, deckc, extrac);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_JOIN_GAME: {
		STOC_JoinGame* pkt = (STOC_JoinGame*)pdata;
		std::wstring str;
		wchar_t msgbuf[256];
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1226), deckManager.GetLFListName(pkt->info.lflist));
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1225), dataManager.GetSysString(1240 + pkt->info.rule));
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%ls\n", dataManager.GetSysString(1227), dataManager.GetSysString(1244 + pkt->info.mode));
		str.append(msgbuf);
		if(pkt->info.time_limit) {
			myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1237), pkt->info.time_limit);
			str.append(msgbuf);
		}
		str.append(L"==========\n");
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1231), pkt->info.start_lp);
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1232), pkt->info.start_hand);
		str.append(msgbuf);
		myswprintf(msgbuf, L"%ls%d\n", dataManager.GetSysString(1233), pkt->info.draw_count);
		str.append(msgbuf);
		if(pkt->info.duel_rule != DEFAULT_DUEL_RULE) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1260 + pkt->info.duel_rule - 1));
			str.append(msgbuf);
		}
		if(pkt->info.no_check_deck) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1229));
			str.append(msgbuf);
		}
		if(pkt->info.no_shuffle_deck) {
			myswprintf(msgbuf, L"*%ls\n", dataManager.GetSysString(1230));
			str.append(msgbuf);
		}
		mainGame->gMutex.lock();
		if(pkt->info.mode == 2) {
			mainGame->dInfo.isTag = true;
			mainGame->chkHostPrepReady[2]->setVisible(true);
			mainGame->chkHostPrepReady[3]->setVisible(true);
			mainGame->stHostPrepDuelist[2]->setVisible(true);
			mainGame->stHostPrepDuelist[3]->setVisible(true);
		} else {
			mainGame->dInfo.isTag = false;
			mainGame->chkHostPrepReady[2]->setVisible(false);
			mainGame->chkHostPrepReady[3]->setVisible(false);
			mainGame->stHostPrepDuelist[2]->setVisible(false);
			mainGame->stHostPrepDuelist[3]->setVisible(false);
		}
		for(int i = 0; i < 4; ++i)
			mainGame->chkHostPrepReady[i]->setChecked(false);
		mainGame->btnHostPrepReady->setVisible(true);
		mainGame->btnHostPrepNotReady->setVisible(false);
		mainGame->dInfo.time_limit = pkt->info.time_limit;
		mainGame->dInfo.time_left[0] = 0;
		mainGame->dInfo.time_left[1] = 0;
		mainGame->deckBuilder.filterList = deckManager.GetLFListContent(pkt->info.lflist);
		if(mainGame->deckBuilder.filterList == nullptr)
			mainGame->deckBuilder.filterList = &deckManager._lfList[0].content;
		mainGame->stHostPrepDuelist[0]->setText(L"");
		mainGame->stHostPrepDuelist[1]->setText(L"");
		mainGame->stHostPrepDuelist[2]->setText(L"");
		mainGame->stHostPrepDuelist[3]->setText(L"");
		mainGame->stHostPrepOB->setText(L"");
		mainGame->SetStaticText(mainGame->stHostPrepRule, 180, mainGame->guiFont, str.c_str());
		mainGame->RefreshCategoryDeck(mainGame->cbCategorySelect, mainGame->cbDeckSelect);
		mainGame->cbCategorySelect->setEnabled(true);
		mainGame->cbDeckSelect->setEnabled(true);
		mainGame->HideElement(mainGame->wCreateHost);
		mainGame->HideElement(mainGame->wLanWindow);
		mainGame->HideElement(mainGame->wSinglePlay);
		mainGame->ShowElement(mainGame->wHostPrepare);
		if(!mainGame->chkIgnore1->isChecked())
			mainGame->wChat->setVisible(true);
		mainGame->gMutex.unlock();
		mainGame->dInfo.duel_rule = pkt->info.duel_rule;
		watching = 0;
		connect_state |= 0x4;
		break;
	}
	case STOC_TYPE_CHANGE: {
		STOC_TypeChange* pkt = (STOC_TypeChange*)pdata;
		if(!mainGame->dInfo.isTag) {
			selftype = pkt->type & 0xf;
			is_host = ((pkt->type >> 4) & 0xf) != 0;
			mainGame->btnHostPrepKick[2]->setVisible(false);
			mainGame->btnHostPrepKick[3]->setVisible(false);
			if(is_host) {
				mainGame->btnHostPrepStart->setVisible(true);
				mainGame->btnHostPrepKick[0]->setVisible(true);
				mainGame->btnHostPrepKick[1]->setVisible(true);
			} else {
				mainGame->btnHostPrepStart->setVisible(false);
				mainGame->btnHostPrepKick[0]->setVisible(false);
				mainGame->btnHostPrepKick[1]->setVisible(false);
			}
			mainGame->chkHostPrepReady[0]->setEnabled(false);
			mainGame->chkHostPrepReady[1]->setEnabled(false);
			if(selftype < 2) {
				mainGame->chkHostPrepReady[selftype]->setEnabled(true);
				mainGame->chkHostPrepReady[selftype]->setChecked(false);
				mainGame->btnHostPrepDuelist->setEnabled(false);
				mainGame->btnHostPrepOB->setEnabled(true);
				mainGame->btnHostPrepReady->setVisible(true);
				mainGame->btnHostPrepNotReady->setVisible(false);
			} else {
				mainGame->btnHostPrepDuelist->setEnabled(true);
				mainGame->btnHostPrepOB->setEnabled(false);
				mainGame->btnHostPrepReady->setVisible(false);
				mainGame->btnHostPrepNotReady->setVisible(false);
			}
			if(mainGame->chkHostPrepReady[0]->isChecked() && mainGame->chkHostPrepReady[1]->isChecked()) {
				mainGame->btnHostPrepStart->setEnabled(true);
			} else {
				mainGame->btnHostPrepStart->setEnabled(false);
			}
		} else {
			if(selftype < 4) {
				mainGame->chkHostPrepReady[selftype]->setEnabled(false);
				mainGame->chkHostPrepReady[selftype]->setChecked(false);
			}
			selftype = pkt->type & 0xf;
			is_host = ((pkt->type >> 4) & 0xf) != 0;
			mainGame->btnHostPrepDuelist->setEnabled(true);
			if(is_host) {
				mainGame->btnHostPrepStart->setVisible(true);
				for(int i = 0; i < 4; ++i)
					mainGame->btnHostPrepKick[i]->setVisible(true);
			} else {
				mainGame->btnHostPrepStart->setVisible(false);
				for(int i = 0; i < 4; ++i)
					mainGame->btnHostPrepKick[i]->setVisible(false);
			}
			if(selftype < 4) {
				mainGame->chkHostPrepReady[selftype]->setEnabled(true);
				mainGame->btnHostPrepOB->setEnabled(true);
				mainGame->btnHostPrepReady->setVisible(true);
				mainGame->btnHostPrepNotReady->setVisible(false);
			} else {
				mainGame->btnHostPrepOB->setEnabled(false);
				mainGame->btnHostPrepReady->setVisible(false);
				mainGame->btnHostPrepNotReady->setVisible(false);
			}
			if(mainGame->chkHostPrepReady[0]->isChecked() && mainGame->chkHostPrepReady[1]->isChecked()
				&& mainGame->chkHostPrepReady[2]->isChecked() && mainGame->chkHostPrepReady[3]->isChecked()) {
				mainGame->btnHostPrepStart->setEnabled(true);
			} else {
				mainGame->btnHostPrepStart->setEnabled(false);
			}
		}
		mainGame->dInfo.player_type = selftype;
		break;
	}
	case STOC_DUEL_START: {
		mainGame->HideElement(mainGame->wHostPrepare);
		mainGame->WaitFrameSignal(11);
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->dInfo.isStarted = true;
		mainGame->dInfo.isFinished = false;
		mainGame->dInfo.lp[0] = 0;
		mainGame->dInfo.lp[1] = 0;
		mainGame->dInfo.strLP[0][0] = 0;
		mainGame->dInfo.strLP[1][0] = 0;
		mainGame->dInfo.turn = 0;
		mainGame->dInfo.time_left[0] = 0;
		mainGame->dInfo.time_left[1] = 0;
		mainGame->dInfo.time_player = 2;
		mainGame->dInfo.isReplaySwapped = false;
		mainGame->is_building = false;
		mainGame->wCardImg->setVisible(true);
		mainGame->wInfos->setVisible(true);
		mainGame->wPhase->setVisible(true);
		mainGame->btnSideOK->setVisible(false);
		mainGame->btnSideShuffle->setVisible(false);
		mainGame->btnSideSort->setVisible(false);
		mainGame->btnSideReload->setVisible(false);
		mainGame->btnPhaseStatus->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		if(!mainGame->chkIgnore1->isChecked())
			mainGame->wChat->setVisible(true);
		mainGame->device->setEventReceiver(&mainGame->dField);
		if(!mainGame->dInfo.isTag) {
			if(selftype > 1) {
				mainGame->dInfo.player_type = 7;
				mainGame->btnLeaveGame->setText(dataManager.GetSysString(1350));
				mainGame->btnLeaveGame->setVisible(true);
				mainGame->btnSpectatorSwap->setVisible(true);
			}
			if(selftype != 1) {
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[0]->getText(), mainGame->dInfo.hostname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[1]->getText(), mainGame->dInfo.clientname, 20);
			} else {
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[1]->getText(), mainGame->dInfo.hostname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[0]->getText(), mainGame->dInfo.clientname, 20);
			}
		} else {
			if(selftype > 3) {
				mainGame->dInfo.player_type = 7;
				mainGame->btnLeaveGame->setText(dataManager.GetSysString(1350));
				mainGame->btnLeaveGame->setVisible(true);
				mainGame->btnSpectatorSwap->setVisible(true);
			}
			if(selftype > 1 && selftype < 4) {
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[2]->getText(), mainGame->dInfo.hostname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[3]->getText(), mainGame->dInfo.hostname_tag, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[0]->getText(), mainGame->dInfo.clientname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[1]->getText(), mainGame->dInfo.clientname_tag, 20);
			} else {
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[0]->getText(), mainGame->dInfo.hostname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[1]->getText(), mainGame->dInfo.hostname_tag, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[2]->getText(), mainGame->dInfo.clientname, 20);
				BufferIO::CopyWStr(mainGame->stHostPrepDuelist[3]->getText(), mainGame->dInfo.clientname_tag, 20);
			}
			mainGame->dInfo.tag_player[0] = false;
			mainGame->dInfo.tag_player[1] = false;
		}
		mainGame->gMutex.unlock();
		match_kill = 0;
		break;
	}
	case STOC_DUEL_END: {
		mainGame->gMutex.lock();
		if(mainGame->dInfo.player_type < 7)
			mainGame->btnLeaveGame->setVisible(false);
		mainGame->CloseGameButtons();
		mainGame->stMessage->setText(dataManager.GetSysString(1500));
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.unlock();
		mainGame->actionSignal.Reset();
		mainGame->actionSignal.Wait();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.Set();
		mainGame->closeDoneSignal.Wait();
		mainGame->gMutex.lock();
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isFinished = true;
		mainGame->is_building = false;
		mainGame->wDeckEdit->setVisible(false);
		mainGame->btnCreateHost->setEnabled(true);
		mainGame->btnJoinHost->setEnabled(true);
		mainGame->btnJoinCancel->setEnabled(true);
		mainGame->btnStartBot->setEnabled(true);
		mainGame->btnBotCancel->setEnabled(true);
		mainGame->stTip->setVisible(false);
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		if(bot_mode)
			mainGame->ShowElement(mainGame->wSinglePlay);
		else
			mainGame->ShowElement(mainGame->wLanWindow);
		mainGame->gMutex.unlock();
		event_base_loopbreak(client_base);
		if(exit_on_return)
			mainGame->device->closeDevice();
		break;
	}
	case STOC_REPLAY: {
		mainGame->gMutex.lock();
		mainGame->wPhase->setVisible(false);
		if(mainGame->dInfo.player_type < 7)
			mainGame->btnLeaveGame->setVisible(false);
		mainGame->CloseGameButtons();
		char* prep = pdata;
		Replay new_replay;
		memcpy(&new_replay.pheader, prep, sizeof(ReplayHeader));
		time_t starttime = new_replay.pheader.seed;
		tm* localedtime = localtime(&starttime);
		wchar_t timetext[40];
		wcsftime(timetext, 40, L"%Y-%m-%d %H-%M-%S", localedtime);
		mainGame->ebRSName->setText(timetext);
		if(!mainGame->chkAutoSaveReplay->isChecked()) {
			mainGame->wReplaySave->setText(dataManager.GetSysString(1340));
			mainGame->PopupElement(mainGame->wReplaySave);
			mainGame->gMutex.unlock();
			mainGame->replaySignal.Reset();
			mainGame->replaySignal.Wait();
		}
		else {
			mainGame->actionParam = 1;
			wchar_t msgbuf[256];
			myswprintf(msgbuf, dataManager.GetSysString(1367), timetext);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, msgbuf);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(30);
		}
		if(mainGame->actionParam || !is_host) {
			prep += sizeof(ReplayHeader);
			memcpy(new_replay.comp_data, prep, len - sizeof(ReplayHeader) - 1);
			new_replay.comp_size = len - sizeof(ReplayHeader) - 1;
			if(mainGame->actionParam)
				new_replay.SaveReplay(mainGame->ebRSName->getText());
			else new_replay.SaveReplay(L"_LastReplay");
		}
		break;
	}
	case STOC_TIME_LIMIT: {
		STOC_TimeLimit* pkt = (STOC_TimeLimit*)pdata;
		int lplayer = mainGame->LocalPlayer(pkt->player);
		if(lplayer == 0)
			DuelClient::SendPacketToServer(CTOS_TIME_CONFIRM);
		mainGame->dInfo.time_player = lplayer;
		mainGame->dInfo.time_left[lplayer] = pkt->left_time;
		break;
	}
	case STOC_CHAT: {
		STOC_Chat* pkt = (STOC_Chat*)pdata;
		int player = pkt->player;
		if(player < 4) {
			if(mainGame->chkIgnore1->isChecked())
				break;
			if(!mainGame->dInfo.isTag) {
				if(mainGame->dInfo.isStarted)
					player = mainGame->LocalPlayer(player);
			} else {
				if(mainGame->dInfo.isStarted && !mainGame->dInfo.isFirst)
					player ^= 2;
				if(player == 0)
					player = 0;
				else if(player == 1)
					player = 2;
				else if(player == 2)
					player = 1;
				else if(player == 3)
					player = 3;
				else
					player = 10;
			}
		} else {
			if(player == 8) { //system custom message.
				if(mainGame->chkIgnore1->isChecked())
					break;
			} else if(player < 11 || player > 19) {
				if(mainGame->chkIgnore2->isChecked())
					break;
				player = 10;
			}
		}
		wchar_t msg[256];
		BufferIO::CopyWStr(pkt->msg, msg, 256);
		mainGame->gMutex.lock();
		mainGame->AddChatMsg(msg, player);
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HS_PLAYER_ENTER: {
		soundManager.PlaySoundEffect(SOUND_PLAYER_ENTER);
		STOC_HS_PlayerEnter* pkt = (STOC_HS_PlayerEnter*)pdata;
		if(pkt->pos > 3)
			break;
		wchar_t name[20];
		BufferIO::CopyWStr(pkt->name, name, 20);
		if(mainGame->dInfo.isTag) {
			if(pkt->pos == 0)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.hostname, 20);
			else if(pkt->pos == 1)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.hostname_tag, 20);
			else if(pkt->pos == 2)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.clientname, 20);
			else if(pkt->pos == 3)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.clientname_tag, 20);
		} else {
			if(pkt->pos == 0)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.hostname, 20);
			else if(pkt->pos == 1)
				BufferIO::CopyWStr(pkt->name, mainGame->dInfo.clientname, 20);
		}
		mainGame->gMutex.lock();
		mainGame->stHostPrepDuelist[pkt->pos]->setText(name);
		mainGame->gMutex.unlock();
		mainGame->FlashWindow();
		break;
	}
	case STOC_HS_PLAYER_CHANGE: {
		STOC_HS_PlayerChange* pkt = (STOC_HS_PlayerChange*)pdata;
		unsigned char pos = (pkt->status >> 4) & 0xf;
		unsigned char state = pkt->status & 0xf;
		if(pos > 3)
			break;
		mainGame->gMutex.lock();
		if(state < 8) {
			soundManager.PlaySoundEffect(SOUND_PLAYER_ENTER);
			wchar_t* prename = (wchar_t*)mainGame->stHostPrepDuelist[pos]->getText();
			mainGame->stHostPrepDuelist[state]->setText(prename);
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			if(pos == 0)
				BufferIO::CopyWStr(prename, mainGame->dInfo.hostname, 20);
			else if(pos == 1)
				BufferIO::CopyWStr(prename, mainGame->dInfo.hostname_tag, 20);
			else if(pos == 2)
				BufferIO::CopyWStr(prename, mainGame->dInfo.clientname, 20);
			else if(pos == 3)
				BufferIO::CopyWStr(prename, mainGame->dInfo.clientname_tag, 20);
		} else if(state == PLAYERCHANGE_READY) {
			mainGame->chkHostPrepReady[pos]->setChecked(true);
			if(pos == selftype) {
				mainGame->btnHostPrepReady->setVisible(false);
				mainGame->btnHostPrepNotReady->setVisible(true);
			}
		} else if(state == PLAYERCHANGE_NOTREADY) {
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			if(pos == selftype) {
				mainGame->btnHostPrepReady->setVisible(true);
				mainGame->btnHostPrepNotReady->setVisible(false);
			}
		} else if(state == PLAYERCHANGE_LEAVE) {
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
		} else if(state == PLAYERCHANGE_OBSERVE) {
			watching++;
			wchar_t watchbuf[32];
			myswprintf(watchbuf, L"%ls%d", dataManager.GetSysString(1253), watching);
			mainGame->stHostPrepDuelist[pos]->setText(L"");
			mainGame->chkHostPrepReady[pos]->setChecked(false);
			mainGame->stHostPrepOB->setText(watchbuf);
		}
		if(mainGame->chkHostPrepReady[0]->isChecked() && mainGame->chkHostPrepReady[1]->isChecked()
			&& (!mainGame->dInfo.isTag || (mainGame->chkHostPrepReady[2]->isChecked() && mainGame->chkHostPrepReady[3]->isChecked()))) {
			mainGame->btnHostPrepStart->setEnabled(true);
		} else {
			mainGame->btnHostPrepStart->setEnabled(false);
		}
		mainGame->gMutex.unlock();
		break;
	}
	case STOC_HS_WATCH_CHANGE: {
		STOC_HS_WatchChange* pkt = (STOC_HS_WatchChange*)pdata;
		watching = pkt->watch_count;
		wchar_t watchbuf[32];
		myswprintf(watchbuf, L"%ls%d", dataManager.GetSysString(1253), watching);
		mainGame->gMutex.lock();
		mainGame->stHostPrepOB->setText(watchbuf);
		mainGame->gMutex.unlock();
		break;
	}
	}
}
int DuelClient::ClientAnalyze(char * msg, unsigned int len) {
	char* pbuf = msg;
	wchar_t textBuffer[256];
	mainGame->dInfo.curMsg = BufferIO::ReadUInt8(pbuf);
	if(mainGame->dInfo.curMsg != MSG_RETRY) {
		memcpy(last_successful_msg, msg, len);
		last_successful_msg_length = len;
	}
	mainGame->wCmdMenu->setVisible(false);
	if(!mainGame->dInfo.isReplay && mainGame->dInfo.curMsg != MSG_WAITING && mainGame->dInfo.curMsg != MSG_CARD_SELECTED) {
		mainGame->waitFrame = -1;
		mainGame->stHintMsg->setVisible(false);
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->gMutex.lock();
			mainGame->HideElement(mainGame->wCardSelect);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
		if(mainGame->wOptions->isVisible()) {
			mainGame->gMutex.lock();
			mainGame->HideElement(mainGame->wOptions);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
	}
	if(mainGame->dInfo.time_player == 1)
		mainGame->dInfo.time_player = 2;
	if(is_swapping) {
		mainGame->gMutex.lock();
		mainGame->dField.ReplaySwap();
		mainGame->gMutex.unlock();
		is_swapping = false;
	}
	switch(mainGame->dInfo.curMsg) {
	case MSG_RETRY: {
		if(last_successful_msg_length) {
			char* p = last_successful_msg;
			auto last_msg = BufferIO::ReadUInt8(p);
			int err_desc = 1421;
			switch(last_msg) {
			case MSG_ANNOUNCE_CARD:
				err_desc = 1422;
				break;
			case MSG_ANNOUNCE_ATTRIB:
				err_desc = 1423;
				break;
			case MSG_ANNOUNCE_RACE:
				err_desc = 1424;
				break;
			case MSG_ANNOUNCE_NUMBER:
				err_desc = 1425;
				break;
			case MSG_SELECT_EFFECTYN:
			case MSG_SELECT_YESNO:
			case MSG_SELECT_OPTION:
				err_desc = 1426;
				break;
			case MSG_SELECT_CARD:
			case MSG_SELECT_UNSELECT_CARD:
			case MSG_SELECT_TRIBUTE:
			case MSG_SELECT_SUM:
			case MSG_SORT_CARD:
				err_desc = 1427;
				break;
			case MSG_SELECT_CHAIN:
				err_desc = 1428;
				break;
			case MSG_SELECT_PLACE:
			case MSG_SELECT_DISFIELD:
				err_desc = 1429;
				break;
			case MSG_SELECT_POSITION:
				err_desc = 1430;
				break;
			case MSG_SELECT_COUNTER:
				err_desc = 1431;
				break;
			default:
				break;
			}
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(dataManager.GetDesc(err_desc));
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			select_hint = last_select_hint;
			return ClientAnalyze(last_successful_msg, last_successful_msg_length);
		}
		mainGame->gMutex.lock();
		mainGame->stMessage->setText(L"Error occurs.");
		mainGame->PopupElement(mainGame->wMessage);
		mainGame->gMutex.unlock();
		mainGame->actionSignal.Reset();
		mainGame->actionSignal.Wait();
		if(!mainGame->dInfo.isSingleMode) {
			mainGame->closeDoneSignal.Reset();
			mainGame->closeSignal.Set();
			mainGame->closeDoneSignal.Wait();
			mainGame->gMutex.lock();
			mainGame->dInfo.isStarted = false;
			mainGame->dInfo.isFinished = false;
			mainGame->btnCreateHost->setEnabled(true);
			mainGame->btnJoinHost->setEnabled(true);
			mainGame->btnJoinCancel->setEnabled(true);
			mainGame->btnStartBot->setEnabled(true);
			mainGame->btnBotCancel->setEnabled(true);
			mainGame->stTip->setVisible(false);
			mainGame->device->setEventReceiver(&mainGame->menuHandler);
			if(bot_mode)
				mainGame->ShowElement(mainGame->wSinglePlay);
			else
				mainGame->ShowElement(mainGame->wLanWindow);
			mainGame->gMutex.unlock();
			event_base_loopbreak(client_base);
			if(exit_on_return)
				mainGame->device->closeDevice();
		}
		return false;
	}
	case MSG_HINT: {
		int type = BufferIO::ReadInt8(pbuf);
		int player = BufferIO::ReadInt8(pbuf);
		int data = BufferIO::ReadInt32(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		switch (type) {
		case HINT_EVENT: {
			myswprintf(event_string, L"%ls", dataManager.GetDesc(data));
			break;
		}
		case HINT_MESSAGE: {
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(dataManager.GetDesc(data));
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			break;
		}
		case HINT_SELECTMSG: {
			select_hint = data;
			last_select_hint = data;
			break;
		}
		case HINT_OPSELECTED: {
			myswprintf(textBuffer, dataManager.GetSysString(1510), dataManager.GetDesc(data));
			mainGame->AddLog(textBuffer);
			mainGame->gMutex.lock();
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_EFFECT: {
			mainGame->showcardcode = data;
			mainGame->showcarddif = 0;
			mainGame->showcard = 1;
			mainGame->WaitFrameSignal(30);
			break;
		}
		case HINT_RACE: {
			myswprintf(textBuffer, dataManager.GetSysString(1511), dataManager.FormatRace(data));
			mainGame->AddLog(textBuffer);
			mainGame->gMutex.lock();
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_ATTRIB: {
			myswprintf(textBuffer, dataManager.GetSysString(1511), dataManager.FormatAttribute(data));
			mainGame->AddLog(textBuffer);
			mainGame->gMutex.lock();
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_CODE: {
			myswprintf(textBuffer, dataManager.GetSysString(1511), dataManager.GetName(data));
			mainGame->AddLog(textBuffer, data);
			mainGame->gMutex.lock();
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_NUMBER: {
			myswprintf(textBuffer, dataManager.GetSysString(1512), data);
			mainGame->AddLog(textBuffer);
			mainGame->gMutex.lock();
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->guiFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_CARD: {
			mainGame->showcardcode = data;
			mainGame->showcarddif = 0;
			mainGame->showcard = 1;
			mainGame->WaitFrameSignal(30);
			break;
		}
		case HINT_ZONE: {
			if(mainGame->LocalPlayer(player) == 1)
				data = (data >> 16) | (data << 16);
			for(unsigned filter = 0x1; filter != 0; filter <<= 1) {
				std::wstring str;
				if(unsigned s = filter & data) {
					if(s & 0x60) {
						str += dataManager.GetSysString(1081);
						data &= ~0x600000;
					} else if(s & 0xffff)
						str += dataManager.GetSysString(102);
					else if(s & 0xffff0000) {
						str += dataManager.GetSysString(103);
						s >>= 16;
					}
					if(s & 0x1f)
						str += dataManager.GetSysString(1002);
					else if(s & 0xff00) {
						s >>= 8;
						if(s & 0x1f)
							str += dataManager.GetSysString(1003);
						else if(s & 0x20)
							str += dataManager.GetSysString(1008);
						else if(s & 0xc0)
							str += dataManager.GetSysString(1009);
					}
					int seq = 1;
					for(int i = 0x1; i < 0x100; i <<= 1) {
						if(s & i)
							break;
						++seq;
					}
					str += L"(" + std::to_wstring(seq) + L")";
					myswprintf(textBuffer, dataManager.GetSysString(1510), str.c_str());
					mainGame->AddLog(textBuffer);
				}
			}
			mainGame->dField.selectable_field = data;
			mainGame->WaitFrameSignal(40);
			mainGame->dField.selectable_field = 0;
			break;
		}
		}
		break;
	}
	case MSG_WIN: {
		mainGame->dInfo.isFinished = true;
		int player = BufferIO::ReadInt8(pbuf);
		int type = BufferIO::ReadInt8(pbuf);
		mainGame->showcarddif = 110;
		mainGame->showcardp = 0;
		mainGame->dInfo.vic_string = 0;
		wchar_t vic_buf[256];
		if(player == 2)
			mainGame->showcardcode = 3;
		else if(mainGame->LocalPlayer(player) == 0) {
			mainGame->showcardcode = 1;
			if(match_kill)
				myswprintf(vic_buf, dataManager.GetVictoryString(0xffff), dataManager.GetName(match_kill));
			else if(type < 0x10)
				myswprintf(vic_buf, L"[%ls] %ls", mainGame->dInfo.clientname, dataManager.GetVictoryString(type));
			else
				myswprintf(vic_buf, L"%ls", dataManager.GetVictoryString(type));
			mainGame->dInfo.vic_string = vic_buf;
		} else {
			mainGame->showcardcode = 2;
			if(match_kill)
				myswprintf(vic_buf, dataManager.GetVictoryString(0xffff), dataManager.GetName(match_kill));
			else if(type < 0x10)
				myswprintf(vic_buf, L"[%ls] %ls", mainGame->dInfo.hostname, dataManager.GetVictoryString(type));
			else
				myswprintf(vic_buf, L"%ls", dataManager.GetVictoryString(type));
			mainGame->dInfo.vic_string = vic_buf;
		}
		mainGame->showcard = 101;
		mainGame->WaitFrameSignal(120);
		mainGame->dInfo.vic_string = 0;
		mainGame->showcard = 0;
		break;
	}
	case MSG_WAITING: {
		mainGame->waitFrame = 0;
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(dataManager.GetSysString(1390));
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_START: {
		mainGame->showcardcode = 11;
		mainGame->showcarddif = 30;
		mainGame->showcardp = 0;
		mainGame->showcard = 101;
		mainGame->WaitFrameSignal(40);
		mainGame->showcard = 0;
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		int playertype = BufferIO::ReadInt8(pbuf);
		mainGame->dInfo.isFirst =  (playertype & 0xf) ? false : true;
		if(playertype & 0xf0)
			mainGame->dInfo.player_type = 7;
		if(mainGame->dInfo.isTag) {
			if(mainGame->dInfo.isFirst)
				mainGame->dInfo.tag_player[1] = true;
			else
				mainGame->dInfo.tag_player[0] = true;
		}
		mainGame->dInfo.duel_rule = BufferIO::ReadInt8(pbuf);
		mainGame->dInfo.lp[mainGame->LocalPlayer(0)] = BufferIO::ReadInt32(pbuf);
		mainGame->dInfo.lp[mainGame->LocalPlayer(1)] = BufferIO::ReadInt32(pbuf);
		myswprintf(mainGame->dInfo.strLP[0], L"%d", mainGame->dInfo.lp[0]);
		myswprintf(mainGame->dInfo.strLP[1], L"%d", mainGame->dInfo.lp[1]);
		int deckc = BufferIO::ReadInt16(pbuf);
		int extrac = BufferIO::ReadInt16(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(0), deckc, extrac);
		deckc = BufferIO::ReadInt16(pbuf);
		extrac = BufferIO::ReadInt16(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(1), deckc, extrac);
		mainGame->dInfo.turn = 0;
		mainGame->dInfo.is_shuffling = false;
		select_hint = 0;
		select_unselect_hint = 0;
		last_select_hint = 0;
		last_successful_msg_length = 0;
		if(mainGame->dInfo.isReplaySwapped) {
			std::swap(mainGame->dInfo.hostname, mainGame->dInfo.clientname);
			std::swap(mainGame->dInfo.hostname_tag, mainGame->dInfo.clientname_tag);
			mainGame->dInfo.isReplaySwapped = false;
			mainGame->dField.ReplaySwap();
		}
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UPDATE_DATA: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int location = BufferIO::ReadInt8(pbuf);
		mainGame->gMutex.lock();
		mainGame->dField.UpdateFieldCard(player, location, pbuf);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UPDATE_CARD: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int loc = BufferIO::ReadInt8(pbuf);
		int seq = BufferIO::ReadInt8(pbuf);
		mainGame->gMutex.lock();
		mainGame->dField.UpdateCard(player, loc, seq, pbuf);
		mainGame->gMutex.unlock();
		break;
	}
	case MSG_SELECT_BATTLECMD: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		int code, desc, count, con, loc, seq/*, diratt*/;
		ClientCard* pcard;
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc =  BufferIO::ReadInt8(pbuf);
			seq =  BufferIO::ReadInt8(pbuf);
			desc =  BufferIO::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			int flag = 0;
			if(code & 0x80000000) {
				flag = EDESC_OPERATION;
				code &= 0x7fffffff;
			}
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
			} else {
				pcard->cmdFlag |= COMMAND_ACTIVATE;
				if (pcard->location == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if (pcard->location == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
			}
		}
		mainGame->dField.attackable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			/*code = */BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			/*diratt = */BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.attackable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_ATTACK;
		}
		mainGame->gMutex.lock();
		if(BufferIO::ReadInt8(pbuf)) {
			mainGame->btnM2->setVisible(true);
			mainGame->btnM2->setEnabled(true);
			mainGame->btnM2->setPressed(false);
		}
		if(BufferIO::ReadInt8(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_IDLECMD: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		int code, desc, count, con, loc, seq;
		ClientCard* pcard;
		mainGame->dField.summonable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.summonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SUMMON;
		}
		mainGame->dField.spsummonable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.spsummonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SPSUMMON;
			if (pcard->location == LOCATION_DECK) {
				pcard->SetCode(code);
				mainGame->dField.deck_act = true;
			} else if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			else if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
			else if (pcard->location == LOCATION_EXTRA)
				mainGame->dField.extra_act = true;
			else {
				int seq = mainGame->dInfo.duel_rule >= 4 ? 0 : 6;
				if (pcard->location == LOCATION_SZONE && pcard->sequence == seq && (pcard->type & TYPE_PENDULUM) && !pcard->equipTarget)
					mainGame->dField.pzone_act[pcard->controler] = true;
			}
		}
		mainGame->dField.reposable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.reposable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_REPOS;
		}
		mainGame->dField.msetable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.msetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_MSET;
		}
		mainGame->dField.ssetable_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.ssetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SSET;
		}
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			loc = BufferIO::ReadInt8(pbuf);
			seq = BufferIO::ReadInt8(pbuf);
			desc = BufferIO::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			int flag = 0;
			if(code & 0x80000000) {
				flag = EDESC_OPERATION;
				code &= 0x7fffffff;
			}
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
			} else {
				pcard->cmdFlag |= COMMAND_ACTIVATE;
				if (pcard->location == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if (pcard->location == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
			}
		}
		if(BufferIO::ReadInt8(pbuf)) {
			mainGame->btnBP->setVisible(true);
			mainGame->btnBP->setEnabled(true);
			mainGame->btnBP->setPressed(false);
		}
		if(BufferIO::ReadInt8(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		if (BufferIO::ReadInt8(pbuf)) {
			mainGame->btnShuffle->setVisible(true);
		} else {
			mainGame->btnShuffle->setVisible(false);
		}
		return false;
	}
	case MSG_SELECT_EFFECTYN: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l = BufferIO::ReadInt8(pbuf);
		int s = BufferIO::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
		if (pcard->code != code)
			pcard->SetCode(code);
		BufferIO::ReadInt8(pbuf);
		if(l != LOCATION_DECK) {
			pcard->is_highlighting = true;
			mainGame->dField.highlighting_card = pcard;
		}
		int desc = BufferIO::ReadInt32(pbuf);
		if(desc == 0) {
			wchar_t ynbuf[256];
			myswprintf(ynbuf, dataManager.GetSysString(200), dataManager.FormatLocation(l, s), dataManager.GetName(code));
			myswprintf(textBuffer, L"%ls\n%ls", event_string, ynbuf);
		} else if(desc == 221) {
			wchar_t ynbuf[256];
			myswprintf(ynbuf, dataManager.GetSysString(221), dataManager.FormatLocation(l, s), dataManager.GetName(code));
			myswprintf(textBuffer, L"%ls\n%ls\n%ls", event_string, ynbuf, dataManager.GetSysString(223));
		} else if(desc < 2048) {
			myswprintf(textBuffer, dataManager.GetSysString(desc), dataManager.GetName(code));
		} else {
			myswprintf(textBuffer, dataManager.GetDesc(desc), dataManager.GetName(code));
		}
		mainGame->gMutex.lock();
		mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->guiFont, textBuffer);
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_YESNO: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		int desc = BufferIO::ReadInt32(pbuf);
		mainGame->dField.highlighting_card = 0;
		mainGame->gMutex.lock();
		mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->guiFont, dataManager.GetDesc(desc));
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_OPTION: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_options.clear();
		for (int i = 0; i < count; ++i)
			mainGame->dField.select_options.push_back(BufferIO::ReadInt32(pbuf));
		mainGame->dField.ShowSelectOption(select_hint);
		select_hint = 0;
		return false;
	}
	case MSG_SELECT_CARD: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_cancelable = BufferIO::ReadInt8(pbuf) != 0;
		mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		int c, l, s, ss;
		unsigned int code;
		bool panelmode = false;
		int handcount = 0;
		bool select_ready = mainGame->dField.select_min == 0;
		mainGame->dField.select_ready = select_ready;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			ss = BufferIO::ReadInt8(pbuf);
			if ((l & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
			else
				pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = false;
			if (l & 0xf1)
				panelmode = true;
			if(l & LOCATION_HAND) {
				handcount++;
				if(handcount >= 10)
					panelmode = true;
			}
		}
		std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
		if(select_hint)
			myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetDesc(select_hint),
			           mainGame->dField.select_min, mainGame->dField.select_max);
		else myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetSysString(560), mainGame->dField.select_min, mainGame->dField.select_max);
		select_hint = 0;
		if (panelmode) {
			mainGame->gMutex.lock();
			mainGame->wCardSelect->setText(textBuffer);
			mainGame->dField.ShowSelectCard(select_ready);
			mainGame->gMutex.unlock();
		} else {
			mainGame->stHintMsg->setText(textBuffer);
			mainGame->stHintMsg->setVisible(true);
		}
		if (mainGame->dField.select_cancelable) {
			mainGame->dField.ShowCancelOrFinishButton(1);
		} else if (select_ready) {
			mainGame->dField.ShowCancelOrFinishButton(2);
		} else {
			mainGame->dField.ShowCancelOrFinishButton(0);
		}
		return false;
	}
	case MSG_SELECT_UNSELECT_CARD: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		bool finishable = BufferIO::ReadInt8(pbuf) != 0;
		bool cancelable = BufferIO::ReadInt8(pbuf) != 0;
		mainGame->dField.select_cancelable = finishable || cancelable;
		mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
		int count1 = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		int c, l, s, ss;
		unsigned int code;
		bool panelmode = false;
		mainGame->dField.select_ready = false;
		ClientCard* pcard;
		for (int i = 0; i < count1; ++i) {
			code = (unsigned int)BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			ss = BufferIO::ReadInt8(pbuf);
			if ((l & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
			else
				pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = false;
			if (l & 0xf1)
				panelmode = true;
		}
		int count2 = BufferIO::ReadInt8(pbuf);
		for (int i = count1; i < count1 + count2; ++i) {
			code = (unsigned int)BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			ss = BufferIO::ReadInt8(pbuf);
			if ((l & LOCATION_OVERLAY) > 0)
				pcard = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
			else
				pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->select_seq = i;
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->is_selectable = true;
			pcard->is_selected = true;
			if (l & 0xf1)
				panelmode = true;
		}
		std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
		if(select_hint)
			select_unselect_hint = select_hint;
		if(select_unselect_hint)
			myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetDesc(select_unselect_hint),
			           mainGame->dField.select_min, mainGame->dField.select_max);
		else myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetSysString(560), mainGame->dField.select_min, mainGame->dField.select_max);
		select_hint = 0;
		if (panelmode) {
			mainGame->gMutex.lock();
			mainGame->wCardSelect->setText(textBuffer);
			mainGame->dField.ShowSelectCard(mainGame->dField.select_cancelable);
			mainGame->gMutex.unlock();
		} else {
			mainGame->stHintMsg->setText(textBuffer);
			mainGame->stHintMsg->setVisible(true);
		}
		if (mainGame->dField.select_cancelable) {
			if(finishable) {
				mainGame->dField.select_ready = true;
				mainGame->dField.ShowCancelOrFinishButton(2);
			}
			else {
				mainGame->dField.ShowCancelOrFinishButton(1);
			}
		}
		else
			mainGame->dField.ShowCancelOrFinishButton(0);
		return false;
	}
	case MSG_SELECT_CHAIN: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		int specount = BufferIO::ReadInt8(pbuf);
		int forced = BufferIO::ReadInt8(pbuf);
		/*int hint0 = */BufferIO::ReadInt32(pbuf);
		/*int hint1 = */BufferIO::ReadInt32(pbuf);
		int code, c, l, s, ss, desc;
		ClientCard* pcard;
		bool panelmode = false;
		bool conti_exist = false;
		bool select_trigger = (specount == 0x7f);
		mainGame->dField.chain_forced = (forced != 0);
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		mainGame->dField.conti_cards.clear();
		for (int i = 0; i < count; ++i) {
			int flag = BufferIO::ReadInt8(pbuf);
			code = BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			ss = BufferIO::ReadInt8(pbuf);
			desc = BufferIO::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s, ss);
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(std::make_pair(desc, flag));
			pcard->is_selected = false;
			if(flag == EDESC_OPERATION) {
				pcard->chain_code = code;
				mainGame->dField.conti_cards.push_back(pcard);
				mainGame->dField.conti_act = true;
				conti_exist = true;
			} else {
				pcard->is_selectable = true;
				if(flag == EDESC_RESET)
					pcard->cmdFlag |= COMMAND_RESET;
				else
					pcard->cmdFlag |= COMMAND_ACTIVATE;
				if(pcard->location == LOCATION_DECK) {
					pcard->SetCode(code);
					mainGame->dField.deck_act = true;
				} else if(l == LOCATION_GRAVE)
					mainGame->dField.grave_act = true;
				else if(l == LOCATION_REMOVED)
					mainGame->dField.remove_act = true;
				else if(l == LOCATION_EXTRA)
					mainGame->dField.extra_act = true;
				else if(l == LOCATION_OVERLAY)
					panelmode = true;
			}
		}
		if(!select_trigger && !forced && (mainGame->ignore_chain || ((count == 0 || specount == 0) && !mainGame->always_chain)) && (count == 0 || !mainGame->chain_when_avail)) {
			SetResponseI(-1);
			mainGame->dField.ClearChainSelect();
			if(mainGame->chkWaitChain->isChecked() && !mainGame->ignore_chain) {
				mainGame->WaitFrameSignal(rnd.real() * 20 + 20);
			}
			DuelClient::SendResponse();
			return true;
		}
		if(mainGame->chkAutoChain->isChecked() && forced && !(mainGame->always_chain || mainGame->chain_when_avail)) {
			SetResponseI(0);
			mainGame->dField.ClearChainSelect();
			DuelClient::SendResponse();
			return true;
		}
		mainGame->gMutex.lock();
		if(!conti_exist)
			mainGame->stHintMsg->setText(dataManager.GetSysString(550));
		else
			mainGame->stHintMsg->setText(dataManager.GetSysString(556));
		mainGame->stHintMsg->setVisible(true);
		if(panelmode) {
			mainGame->dField.list_command = COMMAND_ACTIVATE;
			mainGame->dField.selectable_cards = mainGame->dField.activatable_cards;
			std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
			auto eit = std::unique(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end());
			mainGame->dField.selectable_cards.erase(eit, mainGame->dField.selectable_cards.end());
			mainGame->dField.ShowChainCard();
		} else {
			if(!forced) {
				if(count == 0)
					myswprintf(textBuffer, L"%ls\n%ls", dataManager.GetSysString(201), dataManager.GetSysString(202));
				else if(select_trigger)
					myswprintf(textBuffer, L"%ls\n%ls\n%ls", event_string, dataManager.GetSysString(222), dataManager.GetSysString(223));
				else
					myswprintf(textBuffer, L"%ls\n%ls", event_string, dataManager.GetSysString(203));
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->guiFont, textBuffer);
				mainGame->PopupElement(mainGame->wQuery);
			}
		}
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_PLACE:
	case MSG_SELECT_DISFIELD: {
		int selecting_player = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_min = count > 0 ? count : 1;
		mainGame->dField.select_ready = false;
		mainGame->dField.select_cancelable = count == 0;
		mainGame->dField.selectable_field = ~BufferIO::ReadInt32(pbuf);
		if(selecting_player == mainGame->LocalPlayer(1))
			mainGame->dField.selectable_field = (mainGame->dField.selectable_field >> 16) | (mainGame->dField.selectable_field << 16);
		mainGame->dField.selected_field = 0;
		unsigned char respbuf[64];
		int pzone = 0;
		if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE) {
			if (select_hint) {
				myswprintf(textBuffer, dataManager.GetSysString(569), dataManager.GetName(select_hint));
			} else
				myswprintf(textBuffer, dataManager.GetSysString(560));
		} else {
			if (select_hint) {
				myswprintf(textBuffer, dataManager.GetDesc(select_hint));
			} else
				myswprintf(textBuffer, dataManager.GetSysString(570));
		}
		select_hint = 0;
		mainGame->stHintMsg->setText(textBuffer);
		mainGame->stHintMsg->setVisible(true);
		if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE && (
			(mainGame->chkMAutoPos->isChecked() && mainGame->dField.selectable_field & 0x7f007f) ||
			(mainGame->chkSTAutoPos->isChecked() && !(mainGame->dField.selectable_field & 0x7f007f)))) {
			unsigned int filter;
			if (mainGame->dField.selectable_field & 0x7f) {
				respbuf[0] = mainGame->LocalPlayer(0);
				respbuf[1] = LOCATION_MZONE;
				filter = mainGame->dField.selectable_field & 0x7f;
			} else if (mainGame->dField.selectable_field & 0x1f00) {
				respbuf[0] = mainGame->LocalPlayer(0);
				respbuf[1] = LOCATION_SZONE;
				filter = (mainGame->dField.selectable_field >> 8) & 0x1f;
			} else if (mainGame->dField.selectable_field & 0xc000) {
				respbuf[0] = mainGame->LocalPlayer(0);
				respbuf[1] = LOCATION_SZONE;
				filter = (mainGame->dField.selectable_field >> 14) & 0x3;
				pzone = 1;
			} else if (mainGame->dField.selectable_field & 0x7f0000) {
				respbuf[0] = mainGame->LocalPlayer(1);
				respbuf[1] = LOCATION_MZONE;
				filter = (mainGame->dField.selectable_field >> 16) & 0x7f;
			} else if (mainGame->dField.selectable_field & 0x1f000000) {
				respbuf[0] = mainGame->LocalPlayer(1);
				respbuf[1] = LOCATION_SZONE;
				filter = (mainGame->dField.selectable_field >> 24) & 0x1f;
			} else {
				respbuf[0] = mainGame->LocalPlayer(1);
				respbuf[1] = LOCATION_SZONE;
				filter = (mainGame->dField.selectable_field >> 30) & 0x3;
				pzone = 1;
			}
			if(!pzone) {
				if(mainGame->chkRandomPos->isChecked()) {
					do {
						respbuf[2] = rnd.real() * 7;
					} while(!(filter & (1 << respbuf[2])));
				} else {
					if (filter & 0x40) respbuf[2] = 6;
					else if (filter & 0x20) respbuf[2] = 5;
					else if (filter & 0x4) respbuf[2] = 2;
					else if (filter & 0x2) respbuf[2] = 1;
					else if (filter & 0x8) respbuf[2] = 3;
					else if (filter & 0x1) respbuf[2] = 0;
					else if (filter & 0x10) respbuf[2] = 4;
				}
			} else {
				if (filter & 0x1) respbuf[2] = 6;
				else if (filter & 0x2) respbuf[2] = 7;
			}
			mainGame->dField.selectable_field = 0;
			SetResponseB(respbuf, 3);
			DuelClient::SendResponse();
			return true;
		}
		if(mainGame->dField.select_cancelable) {
			mainGame->dField.ShowCancelOrFinishButton(1);
		}
		return false;
	}
	case MSG_SELECT_POSITION: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int positions = BufferIO::ReadInt8(pbuf);
		if (positions == 0x1 || positions == 0x2 || positions == 0x4 || positions == 0x8) {
			SetResponseI(positions);
			return true;
		}
		int count = 0, filter = 0x1, startpos;
		while(filter != 0x10) {
			if(positions & filter) count++;
			filter <<= 1;
		}
		if(count == 4) startpos = 10;
		else if(count == 3) startpos = 82;
		else startpos = 155;
		if(positions & 0x1) {
			mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSAU, code));
			mainGame->btnPSAU->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSAU->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSAU->setVisible(false);
		if(positions & 0x2) {
			mainGame->btnPSAD->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSAD->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSAD->setVisible(false);
		if(positions & 0x4) {
			mainGame->imageLoading.insert(std::make_pair(mainGame->btnPSDU, code));
			mainGame->btnPSDU->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSDU->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSDU->setVisible(false);
		if(positions & 0x8) {
			mainGame->btnPSDD->setRelativePosition(rect<s32>(startpos, 45, startpos + 140, 185));
			mainGame->btnPSDD->setVisible(true);
			startpos += 145;
		} else mainGame->btnPSDD->setVisible(false);
		mainGame->gMutex.lock();
		mainGame->PopupElement(mainGame->wPosSelect);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_TRIBUTE: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_cancelable = BufferIO::ReadInt8(pbuf) != 0;
		mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.selectsum_all.clear();
		mainGame->dField.selectsum_cards.clear();
		mainGame->dField.select_panalmode = false;
		int c, l, s, t;
		unsigned int code;
		ClientCard* pcard;
		mainGame->dField.select_ready = false;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			t = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			mainGame->dField.selectsum_all.push_back(pcard);
			pcard->opParam = t << 16 | 1;
			pcard->select_seq = i;
			pcard->is_selectable = true;
		}
		mainGame->dField.CheckSelectTribute();
		if(select_hint)
			myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetDesc(select_hint),
			           mainGame->dField.select_min, mainGame->dField.select_max);
		else myswprintf(textBuffer, L"%ls(%d-%d)", dataManager.GetSysString(531), mainGame->dField.select_min, mainGame->dField.select_max);
		select_hint = 0;
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(textBuffer);
		mainGame->stHintMsg->setVisible(true);
		if (mainGame->dField.select_cancelable) {
			mainGame->dField.ShowCancelOrFinishButton(1);
		}
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_COUNTER: {
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_counter_type = BufferIO::ReadInt16(pbuf);
		mainGame->dField.select_counter_count = BufferIO::ReadInt16(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		int c, l, s, t/*, code*/;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			/*code = */BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			t = BufferIO::ReadInt16(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->opParam = (t << 16) | t;
			pcard->is_selectable = true;
		}
		myswprintf(textBuffer, dataManager.GetSysString(204), mainGame->dField.select_counter_count, dataManager.GetCounterName(mainGame->dField.select_counter_type));
		mainGame->gMutex.lock();
		mainGame->stHintMsg->setText(textBuffer);
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_SELECT_SUM: {
		mainGame->dField.select_mode = BufferIO::ReadInt8(pbuf);
		/*int selecting_player = */BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_sumval = BufferIO::ReadInt32(pbuf);
		mainGame->dField.select_min = BufferIO::ReadInt8(pbuf);
		mainGame->dField.select_max = BufferIO::ReadInt8(pbuf);
		mainGame->dField.must_select_count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectsum_all.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.selectsum_cards.clear();
		mainGame->dField.select_panalmode = false;
		for (int i = 0; i < mainGame->dField.must_select_count; ++i) {
			unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
			int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			int l = BufferIO::ReadInt8(pbuf);
			int s = BufferIO::ReadInt8(pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->opParam = BufferIO::ReadInt32(pbuf);
			pcard->select_seq = 0;
			mainGame->dField.selected_cards.push_back(pcard);
		}
		int count = BufferIO::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
			int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			int l = BufferIO::ReadInt8(pbuf);
			int s = BufferIO::ReadInt8(pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->opParam = BufferIO::ReadInt32(pbuf);
			pcard->select_seq = i;
			mainGame->dField.selectsum_all.push_back(pcard);
			if ((l & 0xe) == 0)
				mainGame->dField.select_panalmode = true;
		}
		std::sort(mainGame->dField.selectsum_all.begin(), mainGame->dField.selectsum_all.end(), ClientCard::client_card_sort);
		if(select_hint)
			myswprintf(textBuffer, L"%ls(%d)", dataManager.GetDesc(select_hint), mainGame->dField.select_sumval);
		else myswprintf(textBuffer, L"%ls(%d)", dataManager.GetSysString(560), mainGame->dField.select_sumval);
		select_hint = 0;
		if(mainGame->dField.select_panalmode) {
			mainGame->wCardSelect->setText(textBuffer);
		} else {
			mainGame->stHintMsg->setText(textBuffer);
			mainGame->stHintMsg->setVisible(true);
		}
		return mainGame->dField.ShowSelectSum(mainGame->dField.select_panalmode);
	}
	case MSG_SORT_CARD: {
		/*int player = */BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.sort_list.clear();
		int c, l, s;
		unsigned int code;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = (unsigned int)BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			mainGame->dField.sort_list.push_back(0);
		}
		mainGame->wCardSelect->setText(dataManager.GetSysString(205));
		mainGame->dField.select_min = 0;
		mainGame->dField.select_max = count;
		mainGame->dField.ShowSelectCard();
		return false;
	}
	case MSG_CONFIRM_DECKTOP: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		int code;
		ClientCard* pcard;
		mainGame->dField.selectable_cards.clear();
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			pbuf += 3;
			pcard = *(mainGame->dField.deck[player].rbegin() + i);
			if (code != 0)
				pcard->SetCode(code);
		}
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_REVEAL);
		myswprintf(textBuffer, dataManager.GetSysString(207), count);
		mainGame->AddLog(textBuffer);
		for (int i = 0; i < count; ++i) {
			pcard = *(mainGame->dField.deck[player].rbegin() + i);
			mainGame->gMutex.lock();
			myswprintf(textBuffer, L"*[%ls]", dataManager.GetName(pcard->code));
			mainGame->AddLog(textBuffer, pcard->code);
			mainGame->gMutex.unlock();
			float shift = -0.15f;
			if (player == 1) shift = 0.15f;
			pcard->dPos = irr::core::vector3df(shift, 0, 0);
			if(!mainGame->dField.deck_reversed)
				pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
			else pcard->dRot = irr::core::vector3df(0, 0, 0);
			pcard->is_moving = true;
			pcard->aniFrame = 5;
			mainGame->WaitFrameSignal(45);
			mainGame->dField.MoveCard(pcard, 5);
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_CONFIRM_EXTRATOP: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		int code;
		ClientCard* pcard;
		mainGame->dField.selectable_cards.clear();
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			pbuf += 3;
			pcard = *(mainGame->dField.extra[player].rbegin() + i + mainGame->dField.extra_p_count[player]);
			if (code != 0)
				pcard->SetCode(code);
		}
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_REVEAL);
		myswprintf(textBuffer, dataManager.GetSysString(207), count);
		mainGame->AddLog(textBuffer);
		for (int i = 0; i < count; ++i) {
			pcard = *(mainGame->dField.extra[player].rbegin() + i + mainGame->dField.extra_p_count[player]);
			mainGame->gMutex.lock();
			myswprintf(textBuffer, L"*[%ls]", dataManager.GetName(pcard->code));
			mainGame->AddLog(textBuffer, pcard->code);
			mainGame->gMutex.unlock();
			if (player == 0)
				pcard->dPos = irr::core::vector3df(0, -0.20f, 0);
			else
				pcard->dPos = irr::core::vector3df(0.15f, 0, 0);
			pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
			pcard->is_moving = true;
			pcard->aniFrame = 5;
			mainGame->WaitFrameSignal(45);
			mainGame->dField.MoveCard(pcard, 5);
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_CONFIRM_CARDS: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		int code, c, l, s;
		std::vector<ClientCard*> field_confirm;
		std::vector<ClientCard*> panel_confirm;
		ClientCard* pcard;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pbuf += count * 7;
			return true;
		}
		soundManager.PlaySoundEffect(SOUND_REVEAL);
		myswprintf(textBuffer, dataManager.GetSysString(208), count);
		mainGame->AddLog(textBuffer);
		for (int i = 0; i < count; ++i) {
			code = BufferIO::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0)
				pcard->SetCode(code);
			mainGame->gMutex.lock();
			myswprintf(textBuffer, L"*[%ls]", dataManager.GetName(code));
			mainGame->AddLog(textBuffer, code);
			mainGame->gMutex.unlock();
			if (l & 0x41) {
				if(count == 1) {
					float shift = -0.15f;
					if (c == 0 && l == 0x40) shift = 0.15f;
					pcard->dPos = irr::core::vector3df(shift, 0, 0);
					if((l == LOCATION_DECK) && mainGame->dField.deck_reversed)
						pcard->dRot = irr::core::vector3df(0, 0, 0);
					else pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
					pcard->is_moving = true;
					pcard->aniFrame = 5;
					mainGame->WaitFrameSignal(45);
					mainGame->dField.MoveCard(pcard, 5);
					mainGame->WaitFrameSignal(5);
				} else {
					if(!mainGame->dInfo.isReplay)
						panel_confirm.push_back(pcard);
				}
			} else {
				if(!mainGame->dInfo.isReplay || (l & LOCATION_ONFIELD))
					field_confirm.push_back(pcard);
			}
		}
		if (field_confirm.size() > 0) {
			mainGame->WaitFrameSignal(5);
			for(size_t i = 0; i < field_confirm.size(); ++i) {
				pcard = field_confirm[i];
				c = pcard->controler;
				l = pcard->location;
				if (l == LOCATION_HAND) {
					mainGame->dField.MoveCard(pcard, 5);
					pcard->is_highlighting = true;
				} else if (l == LOCATION_MZONE) {
					if (pcard->position & POS_FACEUP)
						continue;
					pcard->dPos = irr::core::vector3df(0, 0, 0);
					if (pcard->position == POS_FACEDOWN_ATTACK)
						pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
					else
						pcard->dRot = irr::core::vector3df(3.14159f / 5.0f, 0, 0);
					pcard->is_moving = true;
					pcard->aniFrame = 5;
				} else if (l == LOCATION_SZONE) {
					if (pcard->position & POS_FACEUP)
						continue;
					pcard->dPos = irr::core::vector3df(0, 0, 0);
					pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
					pcard->is_moving = true;
					pcard->aniFrame = 5;
				}
			}
			if (mainGame->dInfo.isReplay)
				mainGame->WaitFrameSignal(30);
			else
				mainGame->WaitFrameSignal(90);
			for(size_t i = 0; i < field_confirm.size(); ++i) {
				pcard = field_confirm[i];
				mainGame->dField.MoveCard(pcard, 5);
				pcard->is_highlighting = false;
			}
			mainGame->WaitFrameSignal(5);
		}
		if (panel_confirm.size()) {
			std::sort(panel_confirm.begin(), panel_confirm.end(), ClientCard::client_card_sort);
			mainGame->gMutex.lock();
			mainGame->dField.selectable_cards = panel_confirm;
			myswprintf(textBuffer, dataManager.GetSysString(208), panel_confirm.size());
			mainGame->wCardSelect->setText(textBuffer);
			mainGame->dField.ShowSelectCard(true);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
		}
		return true;
	}
	case MSG_SHUFFLE_DECK: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		if(mainGame->dField.deck[player].size() < 2)
			return true;
		bool rev = mainGame->dField.deck_reversed;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			mainGame->dField.deck_reversed = false;
			if(rev) {
				for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
				mainGame->WaitFrameSignal(10);
			}
		}
		for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i) {
			mainGame->dField.deck[player][i]->code = 0;
			mainGame->dField.deck[player][i]->is_reversed = false;
		}
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_SHUFFLE);
			for (int i = 0; i < 5; ++i) {
				for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
					(*cit)->dPos = irr::core::vector3df(rand() * 0.4f / RAND_MAX - 0.2f, 0, 0);
					(*cit)->dRot = irr::core::vector3df(0, 0, 0);
					(*cit)->is_moving = true;
					(*cit)->aniFrame = 3;
				}
				mainGame->WaitFrameSignal(3);
				for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit)
					mainGame->dField.MoveCard(*cit, 3);
				mainGame->WaitFrameSignal(3);
			}
			mainGame->dField.deck_reversed = rev;
			if(rev) {
				for (size_t i = 0; i < mainGame->dField.deck[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.deck[player][i], 10);
			}
		}
		return true;
	}
	case MSG_SHUFFLE_HAND: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			if(count > 1)
				soundManager.PlaySoundEffect(SOUND_SHUFFLE);
			mainGame->WaitFrameSignal(5);
			if(player == 1 && !mainGame->dInfo.isReplay && !mainGame->dInfo.isSingleMode) {
				bool flip = false;
				for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
					if((*cit)->code) {
						(*cit)->dPos = irr::core::vector3df(0, 0, 0);
						(*cit)->dRot = irr::core::vector3df(1.322f / 5, 3.1415926f / 5, 0);
						(*cit)->is_moving = true;
						(*cit)->is_hovered = false;
						(*cit)->aniFrame = 5;
						flip = true;
					}
				if(flip)
					mainGame->WaitFrameSignal(5);
			}
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / 5, 0, 0);
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->is_hovered = false;
				(*cit)->aniFrame = 5;
			}
			mainGame->WaitFrameSignal(11);
		}
		for(auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
			(*cit)->SetCode(BufferIO::ReadInt32(pbuf));
			(*cit)->desc_hints.clear();
		}
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->is_hovered = false;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_SHUFFLE_EXTRA: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		if((mainGame->dField.extra[player].size() - mainGame->dField.extra_p_count[player]) < 2)
			return true;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			if(count > 1)
				soundManager.PlaySoundEffect(SOUND_SHUFFLE);
			for (int i = 0; i < 5; ++i) {
				for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
					if(!((*cit)->position & POS_FACEUP)) {
						(*cit)->dPos = irr::core::vector3df(rand() * 0.4f / RAND_MAX - 0.2f, 0, 0);
						(*cit)->dRot = irr::core::vector3df(0, 0, 0);
						(*cit)->is_moving = true;
						(*cit)->aniFrame = 3;
					}
				}
				mainGame->WaitFrameSignal(3);
				for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit)
					if(!((*cit)->position & POS_FACEUP))
						mainGame->dField.MoveCard(*cit, 3);
				mainGame->WaitFrameSignal(3);
			}
		}
		for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit)
			if(!((*cit)->position & POS_FACEUP))
				(*cit)->SetCode(BufferIO::ReadInt32(pbuf));
		return true;
	}
	case MSG_REFRESH_DECK: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		return true;
	}
	case MSG_SWAP_GRAVE_DECK: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
			for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit)
				(*cit)->location = LOCATION_GRAVE;
			int m = 0;
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
				if ((*cit)->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) {
					(*cit)->position = POS_FACEDOWN;
					mainGame->dField.AddCard(*cit, player, LOCATION_EXTRA, 0);
					cit = mainGame->dField.deck[player].erase(cit);
				} else {
					(*cit)->location = LOCATION_DECK;
					(*cit)->sequence = m++;
					++cit;
				}
			}
		} else {
			mainGame->gMutex.lock();
			mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
			for (auto cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit) {
				(*cit)->location = LOCATION_GRAVE;
				mainGame->dField.MoveCard(*cit, 10);
			}
			int m = 0;
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ) {
				ClientCard* pcard = *cit;
				if (pcard->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) {
					pcard->position = POS_FACEDOWN;
					mainGame->dField.AddCard(pcard, player, LOCATION_EXTRA, 0);
					cit = mainGame->dField.deck[player].erase(cit);
				} else {
					pcard->location = LOCATION_DECK;
					pcard->sequence = m++;
					++cit;
				}
				mainGame->dField.MoveCard(pcard, 10);
			}
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_REVERSE_DECK: {
		mainGame->dField.deck_reversed = !mainGame->dField.deck_reversed;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			for(size_t i = 0; i < mainGame->dField.deck[0].size(); ++i)
				mainGame->dField.MoveCard(mainGame->dField.deck[0][i], 10);
			for(size_t i = 0; i < mainGame->dField.deck[1].size(); ++i)
				mainGame->dField.MoveCard(mainGame->dField.deck[1][i], 10);
		}
		return true;
	}
	case MSG_DECK_TOP: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int seq = BufferIO::ReadInt8(pbuf);
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1 - seq);
		pcard->SetCode(code & 0x7fffffff);
		bool rev = (code & 0x80000000) != 0;
		if(pcard->is_reversed != rev) {
			pcard->is_reversed = rev;
			mainGame->dField.MoveCard(pcard, 5);
		}
		return true;
	}
	case MSG_SHUFFLE_SET_CARD: {
		std::vector<ClientCard*>* lst = 0;
		int loc = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		if(loc == LOCATION_MZONE)
			lst = mainGame->dField.mzone;
		else
			lst = mainGame->dField.szone;
		ClientCard* mc[5];
		ClientCard* swp;
		int c, l, s, ps;
		for (int i = 0; i < count; ++i) {
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			BufferIO::ReadInt8(pbuf);
			mc[i] = lst[c][s];
			mc[i]->SetCode(0);
			if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
				mc[i]->dPos = irr::core::vector3df((3.95f - mc[i]->curPos.X) / 10, 0, 0.05f);
				mc[i]->dRot = irr::core::vector3df(0, 0, 0);
				mc[i]->is_moving = true;
				mc[i]->aniFrame = 10;
			}
		}
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
			mainGame->WaitFrameSignal(20);
		for (int i = 0; i < count; ++i) {
			c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			l = BufferIO::ReadInt8(pbuf);
			s = BufferIO::ReadInt8(pbuf);
			BufferIO::ReadInt8(pbuf);
			ps = mc[i]->sequence;
			if (l > 0) {
				swp = lst[c][s];
				lst[c][ps] = swp;
				lst[c][s] = mc[i];
				mc[i]->sequence = s;
				swp->sequence = ps;
			}
		}
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_SHUFFLE);
			for (int i = 0; i < count; ++i) {
				mainGame->dField.MoveCard(mc[i], 10);
				for (auto cit = mc[i]->overlayed.begin(); cit != mc[i]->overlayed.end(); ++cit)
					mainGame->dField.MoveCard(*cit, 10);
			}
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_NEW_TURN: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		mainGame->dInfo.turn++;
		if(!mainGame->dInfo.isTag && !mainGame->dInfo.isReplay && mainGame->dInfo.player_type < 7) {
			mainGame->btnLeaveGame->setText(dataManager.GetSysString(1351));
			mainGame->btnLeaveGame->setVisible(true);
		}
		mainGame->HideElement(mainGame->wSurrender);
		if(!mainGame->dInfo.isReplay && mainGame->dInfo.player_type < 7) {
			if(mainGame->gameConf.control_mode == 0) {
				mainGame->btnChainIgnore->setVisible(true);
				mainGame->btnChainAlways->setVisible(true);
				mainGame->btnChainWhenAvail->setVisible(true);
				mainGame->dField.UpdateChainButtons();
			} else {
				mainGame->btnChainIgnore->setVisible(false);
				mainGame->btnChainAlways->setVisible(false);
				mainGame->btnChainWhenAvail->setVisible(false);
				mainGame->btnCancelOrFinish->setVisible(false);
			}
		}
		if(mainGame->dInfo.isTag && mainGame->dInfo.turn != 1) {
			if(player == 0)
				mainGame->dInfo.tag_player[0] = !mainGame->dInfo.tag_player[0];
			else
				mainGame->dInfo.tag_player[1] = !mainGame->dInfo.tag_player[1];
		}
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_NEXT_TURN);
			mainGame->showcardcode = 10;
			mainGame->showcarddif = 30;
			mainGame->showcardp = 0;
			mainGame->showcard = 101;
			mainGame->WaitFrameSignal(40);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_NEW_PHASE: {
		unsigned short phase = BufferIO::ReadInt16(pbuf);
		mainGame->btnPhaseStatus->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		mainGame->showcarddif = 30;
		mainGame->showcardp = 0;
		switch (phase) {
		case PHASE_DRAW:
			mainGame->btnPhaseStatus->setText(L"\xff24\xff30");
			mainGame->showcardcode = 4;
			break;
		case PHASE_STANDBY:
			mainGame->btnPhaseStatus->setText(L"\xff33\xff30");
			mainGame->showcardcode = 5;
			break;
		case PHASE_MAIN1:
			mainGame->btnPhaseStatus->setText(L"\xff2d\xff11");
			mainGame->showcardcode = 6;
			break;
		case PHASE_BATTLE_START:
			mainGame->btnPhaseStatus->setText(L"\xff22\xff30");
			mainGame->showcardcode = 7;
			break;
		case PHASE_MAIN2:
			mainGame->btnPhaseStatus->setText(L"\xff2d\xff12");
			mainGame->showcardcode = 8;
			break;
		case PHASE_END:
			mainGame->btnPhaseStatus->setText(L"\xff25\xff30");
			mainGame->showcardcode = 9;
			break;
		}
		mainGame->btnPhaseStatus->setPressed(true);
		mainGame->btnPhaseStatus->setVisible(true);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_PHASE);
			mainGame->showcard = 101;
			mainGame->WaitFrameSignal(40);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_MOVE: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int pc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int pl = BufferIO::ReadUInt8(pbuf);
		int ps = BufferIO::ReadInt8(pbuf);
		int pp = BufferIO::ReadInt8(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int cl = BufferIO::ReadUInt8(pbuf);
		int cs = BufferIO::ReadInt8(pbuf);
		int cp = BufferIO::ReadInt8(pbuf);
		int reason = BufferIO::ReadInt32(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			if(cl & LOCATION_REMOVED && pl != cl)
				soundManager.PlaySoundEffect(SOUND_BANISHED);
			else if(reason & REASON_DESTROY && pl != cl)
				soundManager.PlaySoundEffect(SOUND_DESTROYED);
		}
		if (pl == 0) {
			ClientCard* pcard = new ClientCard();
			pcard->position = cp;
			pcard->SetCode(code);
			if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
				mainGame->gMutex.lock();
				mainGame->dField.AddCard(pcard, cc, cl, cs);
				mainGame->gMutex.unlock();
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot, true);
				pcard->curAlpha = 5;
				mainGame->dField.FadeCard(pcard, 255, 20);
				mainGame->WaitFrameSignal(20);
			} else
				mainGame->dField.AddCard(pcard, cc, cl, cs);
		} else if (cl == 0) {
			ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->ClearTarget();
			for(auto eqit = pcard->equipped.begin(); eqit != pcard->equipped.end(); ++eqit)
				(*eqit)->equipTarget = 0;
			if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
				mainGame->dField.FadeCard(pcard, 5, 20);
				mainGame->WaitFrameSignal(20);
				mainGame->gMutex.lock();
				mainGame->dField.RemoveCard(pc, pl, ps);
				mainGame->gMutex.unlock();
				if(pcard == mainGame->dField.hovered_card)
					mainGame->dField.hovered_card = 0;
			} else
				mainGame->dField.RemoveCard(pc, pl, ps);
			delete pcard;
		} else {
			if (!(pl & 0x80) && !(cl & 0x80)) {
				ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
				if (pcard->code != code && (code != 0 || cl == 0x40))
					pcard->SetCode(code);
				pcard->cHint = 0;
				pcard->chValue = 0;
				if((pl & LOCATION_ONFIELD) && (cl != pl))
					pcard->counters.clear();
				if(cl != pl) {
					pcard->ClearTarget();
					if(pcard->equipTarget) {
						pcard->equipTarget->is_showequip = false;
						pcard->equipTarget->equipped.erase(pcard);
						pcard->equipTarget = 0;
					}
				}
				pcard->is_hovered = false;
				pcard->is_showequip = false;
				pcard->is_showtarget = false;
				pcard->is_showchaintarget = false;
				if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
					mainGame->dField.RemoveCard(pc, pl, ps);
					pcard->position = cp;
					mainGame->dField.AddCard(pcard, cc, cl, cs);
				} else {
					mainGame->gMutex.lock();
					mainGame->dField.RemoveCard(pc, pl, ps);
					pcard->position = cp;
					mainGame->dField.AddCard(pcard, cc, cl, cs);
					mainGame->gMutex.unlock();
					if (pl == cl && pc == cc && (cl & 0x71)) {
						pcard->dPos = irr::core::vector3df(-0.3f, 0, 0);
						pcard->dRot = irr::core::vector3df(0, 0, 0);
						if (pc == 1) pcard->dPos.X = 0.3f;
						pcard->is_moving = true;
						pcard->aniFrame = 5;
						mainGame->WaitFrameSignal(5);
						mainGame->dField.MoveCard(pcard, 5);
						mainGame->WaitFrameSignal(5);
					} else {
						if (cl == 0x4 && pcard->overlayed.size() > 0) {
							mainGame->gMutex.lock();
							for (size_t i = 0; i < pcard->overlayed.size(); ++i)
								mainGame->dField.MoveCard(pcard->overlayed[i], 10);
							mainGame->gMutex.unlock();
							mainGame->WaitFrameSignal(10);
						}
						if (cl == 0x2) {
							mainGame->gMutex.lock();
							for (size_t i = 0; i < mainGame->dField.hand[cc].size(); ++i)
								mainGame->dField.MoveCard(mainGame->dField.hand[cc][i], 10);
							mainGame->gMutex.unlock();
						} else {
							mainGame->gMutex.lock();
							mainGame->dField.MoveCard(pcard, 10);
							if (pl == 0x2)
								for (size_t i = 0; i < mainGame->dField.hand[pc].size(); ++i)
									mainGame->dField.MoveCard(mainGame->dField.hand[pc][i], 10);
							mainGame->gMutex.unlock();
						}
						mainGame->WaitFrameSignal(5);
					}
				}
			} else if (!(pl & 0x80)) {
				ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
				if (code != 0 && pcard->code != code)
					pcard->SetCode(code);
				pcard->counters.clear();
				pcard->ClearTarget();
				pcard->is_showtarget = false;
				pcard->is_showchaintarget = false;
				ClientCard* olcard = mainGame->dField.GetCard(cc, cl & 0x7f, cs);
				if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
					mainGame->dField.RemoveCard(pc, pl, ps);
					olcard->overlayed.push_back(pcard);
					mainGame->dField.overlay_cards.insert(pcard);
					pcard->overlayTarget = olcard;
					pcard->location = 0x80;
					pcard->sequence = olcard->overlayed.size() - 1;
				} else {
					mainGame->gMutex.lock();
					mainGame->dField.RemoveCard(pc, pl, ps);
					olcard->overlayed.push_back(pcard);
					mainGame->dField.overlay_cards.insert(pcard);
					mainGame->gMutex.unlock();
					pcard->overlayTarget = olcard;
					pcard->location = 0x80;
					pcard->sequence = olcard->overlayed.size() - 1;
					if (olcard->location == 0x4) {
						mainGame->gMutex.lock();
						mainGame->dField.MoveCard(pcard, 10);
						if (pl == 0x2)
							for (size_t i = 0; i < mainGame->dField.hand[pc].size(); ++i)
								mainGame->dField.MoveCard(mainGame->dField.hand[pc][i], 10);
						mainGame->gMutex.unlock();
						mainGame->WaitFrameSignal(5);
					}
				}
			} else if (!(cl & 0x80)) {
				ClientCard* olcard = mainGame->dField.GetCard(pc, pl & 0x7f, ps);
				ClientCard* pcard = olcard->overlayed[pp];
				if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
					olcard->overlayed.erase(olcard->overlayed.begin() + pcard->sequence);
					pcard->overlayTarget = 0;
					pcard->position = cp;
					mainGame->dField.AddCard(pcard, cc, cl, cs);
					mainGame->dField.overlay_cards.erase(pcard);
					for (size_t i = 0; i < olcard->overlayed.size(); ++i)
						olcard->overlayed[i]->sequence = i;
				} else {
					mainGame->gMutex.lock();
					olcard->overlayed.erase(olcard->overlayed.begin() + pcard->sequence);
					pcard->overlayTarget = 0;
					pcard->position = cp;
					mainGame->dField.AddCard(pcard, cc, cl, cs);
					mainGame->dField.overlay_cards.erase(pcard);
					for (size_t i = 0; i < olcard->overlayed.size(); ++i) {
						olcard->overlayed[i]->sequence = i;
						mainGame->dField.MoveCard(olcard->overlayed[i], 2);
					}
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
					mainGame->gMutex.lock();
					mainGame->dField.MoveCard(pcard, 10);
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
				}
			} else {
				ClientCard* olcard1 = mainGame->dField.GetCard(pc, pl & 0x7f, ps);
				ClientCard* pcard = olcard1->overlayed[pp];
				ClientCard* olcard2 = mainGame->dField.GetCard(cc, cl & 0x7f, cs);
				if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
					olcard1->overlayed.erase(olcard1->overlayed.begin() + pcard->sequence);
					olcard2->overlayed.push_back(pcard);
					pcard->sequence = olcard2->overlayed.size() - 1;
					pcard->location = 0x80;
					pcard->overlayTarget = olcard2;
					for (size_t i = 0; i < olcard1->overlayed.size(); ++i) {
						olcard1->overlayed[i]->sequence = i;
					}
				} else {
					mainGame->gMutex.lock();
					olcard1->overlayed.erase(olcard1->overlayed.begin() + pcard->sequence);
					olcard2->overlayed.push_back(pcard);
					pcard->sequence = olcard2->overlayed.size() - 1;
					pcard->location = 0x80;
					pcard->overlayTarget = olcard2;
					for (size_t i = 0; i < olcard1->overlayed.size(); ++i) {
						olcard1->overlayed[i]->sequence = i;
						mainGame->dField.MoveCard(olcard1->overlayed[i], 2);
					}
					mainGame->dField.MoveCard(pcard, 10);
					mainGame->gMutex.unlock();
					mainGame->WaitFrameSignal(5);
				}
			}
		}
		return true;
	}
	case MSG_POS_CHANGE: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int cl = BufferIO::ReadInt8(pbuf);
		int cs = BufferIO::ReadInt8(pbuf);
		int pp = BufferIO::ReadInt8(pbuf);
		int cp = BufferIO::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
		if((pp & POS_FACEUP) && (cp & POS_FACEDOWN)) {
			pcard->counters.clear();
			pcard->ClearTarget();
		}
		if (code != 0 && pcard->code != code)
			pcard->SetCode(code);
		pcard->position = cp;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			myswprintf(event_string, dataManager.GetSysString(1600));
			mainGame->dField.MoveCard(pcard, 10);
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SET: {
		/*int code = */BufferIO::ReadInt32(pbuf);
		/*int cc = mainGame->LocalPlayer*/(BufferIO::ReadInt8(pbuf));
		/*int cl = */BufferIO::ReadInt8(pbuf);
		/*int cs = */BufferIO::ReadInt8(pbuf);
		/*int cp = */BufferIO::ReadInt8(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
			soundManager.PlaySoundEffect(SOUND_SET);
		myswprintf(event_string, dataManager.GetSysString(1601));
		return true;
	}
	case MSG_SWAP: {
		/*int code1 = */BufferIO::ReadInt32(pbuf);
		int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l1 = BufferIO::ReadInt8(pbuf);
		int s1 = BufferIO::ReadInt8(pbuf);
		/*int p1 = */BufferIO::ReadInt8(pbuf);
		/*int code2 = */BufferIO::ReadInt32(pbuf);
		int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l2 = BufferIO::ReadInt8(pbuf);
		int s2 = BufferIO::ReadInt8(pbuf);
		/*int p2 = */BufferIO::ReadInt8(pbuf);
		myswprintf(event_string, dataManager.GetSysString(1602));
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			mainGame->gMutex.lock();
			mainGame->dField.RemoveCard(c1, l1, s1);
			mainGame->dField.RemoveCard(c2, l2, s2);
			mainGame->dField.AddCard(pc1, c2, l2, s2);
			mainGame->dField.AddCard(pc2, c1, l1, s1);
			mainGame->dField.MoveCard(pc1, 10);
			mainGame->dField.MoveCard(pc2, 10);
			for (size_t i = 0; i < pc1->overlayed.size(); ++i)
				mainGame->dField.MoveCard(pc1->overlayed[i], 10);
			for (size_t i = 0; i < pc2->overlayed.size(); ++i)
				mainGame->dField.MoveCard(pc2->overlayed[i], 10);
			mainGame->gMutex.unlock();
			mainGame->WaitFrameSignal(11);
		} else {
			mainGame->dField.RemoveCard(c1, l1, s1);
			mainGame->dField.RemoveCard(c2, l2, s2);
			mainGame->dField.AddCard(pc1, c2, l2, s2);
			mainGame->dField.AddCard(pc2, c1, l1, s1);
		}
		return true;
	}
	case MSG_FIELD_DISABLED: {
		unsigned int disabled = BufferIO::ReadInt32(pbuf);
		if (!mainGame->dInfo.isFirst)
			disabled = (disabled >> 16) | (disabled << 16);
		mainGame->dField.disabled_field = disabled;
		return true;
	}
	case MSG_SUMMONING: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		/*int cc = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		/*int cl = */BufferIO::ReadInt8(pbuf);
		/*int cs = */BufferIO::ReadInt8(pbuf);
		/*int cp = */BufferIO::ReadInt8(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_SUMMON);
			myswprintf(event_string, dataManager.GetSysString(1603), dataManager.GetName(code));
			mainGame->showcardcode = code;
			mainGame->showcarddif = 0;
			mainGame->showcardp = 0;
			mainGame->showcard = 7;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SUMMONED: {
		myswprintf(event_string, dataManager.GetSysString(1604));
		return true;
	}
	case MSG_SPSUMMONING: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		/*int cc = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		/*int cl = */BufferIO::ReadInt8(pbuf);
		/*int cs = */BufferIO::ReadInt8(pbuf);
		/*int cp = */BufferIO::ReadInt8(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			CardData cd;
			if(dataManager.GetData(code, &cd) && (cd.type & TYPE_TOKEN))
				soundManager.PlaySoundEffect(SOUND_TOKEN);
			else
				soundManager.PlaySoundEffect(SOUND_SPECIAL_SUMMON);
			myswprintf(event_string, dataManager.GetSysString(1605), dataManager.GetName(code));
			mainGame->showcardcode = code;
			mainGame->showcarddif = 1;
			mainGame->showcard = 5;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_SPSUMMONED: {
		myswprintf(event_string, dataManager.GetSysString(1606));
		return true;
	}
	case MSG_FLIPSUMMONING: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int cl = BufferIO::ReadInt8(pbuf);
		int cs = BufferIO::ReadInt8(pbuf);
		int cp = BufferIO::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
		pcard->SetCode(code);
		pcard->position = cp;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			soundManager.PlaySoundEffect(SOUND_FILP);
			myswprintf(event_string, dataManager.GetSysString(1607), dataManager.GetName(code));
			mainGame->dField.MoveCard(pcard, 10);
			mainGame->WaitFrameSignal(11);
			mainGame->showcardcode = code;
			mainGame->showcarddif = 0;
			mainGame->showcardp = 0;
			mainGame->showcard = 7;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
			mainGame->WaitFrameSignal(11);
		}
		return true;
	}
	case MSG_FLIPSUMMONED: {
		myswprintf(event_string, dataManager.GetSysString(1608));
		return true;
	}
	case MSG_CHAINING: {
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		int pcc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int pcl = BufferIO::ReadInt8(pbuf);
		int pcs = BufferIO::ReadInt8(pbuf);
		int subs = BufferIO::ReadInt8(pbuf);
		int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int cl = BufferIO::ReadInt8(pbuf);
		int cs = BufferIO::ReadInt8(pbuf);
		int desc = BufferIO::ReadInt32(pbuf);
		/*int ct = */BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_ACTIVATE);
		ClientCard* pcard = mainGame->dField.GetCard(pcc, pcl, pcs, subs);
		if(pcard->code != code) {
			pcard->code = code;
			mainGame->dField.MoveCard(pcard, 10);
		}
		mainGame->showcardcode = code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 1;
		pcard->is_highlighting = true;
		if(pcard->location & 0x30) {
			float shift = -0.15f;
			if(cc == 1) shift = 0.15f;
			pcard->dPos = irr::core::vector3df(shift, 0, 0);
			pcard->dRot = irr::core::vector3df(0, 0, 0);
			pcard->is_moving = true;
			pcard->aniFrame = 5;
			mainGame->WaitFrameSignal(30);
			mainGame->dField.MoveCard(pcard, 5);
		} else
			mainGame->WaitFrameSignal(30);
		pcard->is_highlighting = false;
		mainGame->dField.current_chain.chain_card = pcard;
		mainGame->dField.current_chain.code = code;
		mainGame->dField.current_chain.desc = desc;
		mainGame->dField.current_chain.controler = cc;
		mainGame->dField.current_chain.location = cl;
		mainGame->dField.current_chain.sequence = cs;
		mainGame->dField.GetChainLocation(cc, cl, cs, &mainGame->dField.current_chain.chain_pos);
		mainGame->dField.current_chain.solved = false;
		mainGame->dField.current_chain.target.clear();
		int chc = 0;
		for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
			if (cl == 0x10 || cl == 0x20) {
				if (chit->controler == cc && chit->location == cl)
					chc++;
			} else {
				if (chit->controler == cc && chit->location == cl && chit->sequence == cs)
					chc++;
			}
		}
		if(cl == LOCATION_HAND)
			mainGame->dField.current_chain.chain_pos.X += 0.35f;
		else
			mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
		return true;
	}
	case MSG_CHAINED: {
		int ct = BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		myswprintf(event_string, dataManager.GetSysString(1609), dataManager.GetName(mainGame->dField.current_chain.code));
		mainGame->gMutex.lock();
		mainGame->dField.chains.push_back(mainGame->dField.current_chain);
		mainGame->gMutex.unlock();
		if (ct > 1)
			mainGame->WaitFrameSignal(20);
		mainGame->dField.last_chain = true;
		return true;
	}
	case MSG_CHAIN_SOLVING: {
		int ct = BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		if(mainGame->dField.chains.size() > 1 || mainGame->gameConf.draw_single_chain) {
			if (mainGame->dField.last_chain)
				mainGame->WaitFrameSignal(11);
			for(int i = 0; i < 5; ++i) {
				mainGame->dField.chains[ct - 1].solved = false;
				mainGame->WaitFrameSignal(3);
				mainGame->dField.chains[ct - 1].solved = true;
				mainGame->WaitFrameSignal(3);
			}
		}
		mainGame->dField.last_chain = false;
		return true;
	}
	case MSG_CHAIN_SOLVED: {
		/*int ct = */BufferIO::ReadInt8(pbuf);
		return true;
	}
	case MSG_CHAIN_END: {
		for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
			for(auto tgit = chit->target.begin(); tgit != chit->target.end(); ++tgit)
				(*tgit)->is_showchaintarget = false;
			chit->chain_card->is_showchaintarget = false;
		}
		mainGame->dField.chains.clear();
		return true;
	}
	case MSG_CHAIN_NEGATED:
	case MSG_CHAIN_DISABLED: {
		int ct = BufferIO::ReadInt8(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			mainGame->showcardcode = mainGame->dField.chains[ct - 1].code;
			mainGame->showcarddif = 0;
			mainGame->showcard = 3;
			mainGame->WaitFrameSignal(30);
			mainGame->showcard = 0;
		}
		return true;
	}
	case MSG_CARD_SELECTED: {
		return true;
	}
	case MSG_RANDOM_SELECTED: {
		/*int player = */BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pbuf += count * 4;
			return true;
		}
		soundManager.PlaySoundEffect(SOUND_DICE);
		ClientCard* pcards[10];
		for (int i = 0; i < count; ++i) {
			int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			int l = BufferIO::ReadInt8(pbuf);
			int s = BufferIO::ReadInt8(pbuf);
			int ss = BufferIO::ReadInt8(pbuf);
			if ((l & 0x80) > 0)
				pcards[i] = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
			else
				pcards[i] = mainGame->dField.GetCard(c, l, s);
			pcards[i]->is_highlighting = true;
		}
		mainGame->WaitFrameSignal(30);
		for(int i = 0; i < count; ++i)
			pcards[i]->is_highlighting = false;
		return true;
	}
	case MSG_BECOME_TARGET: {
		//soundManager.PlaySoundEffect(SOUND_TARGET);
		int count = BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pbuf += count * 4;
			return true;
		}
		for (int i = 0; i < count; ++i) {
			int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
			int l = BufferIO::ReadInt8(pbuf);
			int s = BufferIO::ReadInt8(pbuf);
			/*int ss = */BufferIO::ReadInt8(pbuf);
			ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
			pcard->is_highlighting = true;
			mainGame->dField.current_chain.target.insert(pcard);
			if(pcard->location & LOCATION_ONFIELD) {
				for (int j = 0; j < 3; ++j) {
					mainGame->dField.FadeCard(pcard, 5, 5);
					mainGame->WaitFrameSignal(5);
					mainGame->dField.FadeCard(pcard, 255, 5);
					mainGame->WaitFrameSignal(5);
				}
			} else if(pcard->location & 0x30) {
				float shift = -0.15f;
				if(c == 1) shift = 0.15f;
				pcard->dPos = irr::core::vector3df(shift, 0, 0);
				pcard->dRot = irr::core::vector3df(0, 0, 0);
				pcard->is_moving = true;
				pcard->aniFrame = 5;
				mainGame->WaitFrameSignal(30);
				mainGame->dField.MoveCard(pcard, 5);
			} else
				mainGame->WaitFrameSignal(30);
			myswprintf(textBuffer, dataManager.GetSysString(1610), dataManager.GetName(pcard->code), dataManager.FormatLocation(l, s), s + 1);
			mainGame->AddLog(textBuffer, pcard->code);
			pcard->is_highlighting = false;
		}
		return true;
	}
	case MSG_DRAW: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1 - i);
			if(!mainGame->dField.deck_reversed || code)
				pcard->SetCode(code & 0x7fffffff);
		}
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			for (int i = 0; i < count; ++i) {
				pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1);
				mainGame->dField.deck[player].erase(mainGame->dField.deck[player].end() - 1);
				mainGame->dField.AddCard(pcard, player, LOCATION_HAND, 0);
			}
		} else {
			for (int i = 0; i < count; ++i) {
				soundManager.PlaySoundEffect(SOUND_DRAW);
				mainGame->gMutex.lock();
				pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1);
				mainGame->dField.deck[player].erase(mainGame->dField.deck[player].end() - 1);
				mainGame->dField.AddCard(pcard, player, LOCATION_HAND, 0);
				for(size_t i = 0; i < mainGame->dField.hand[player].size(); ++i)
					mainGame->dField.MoveCard(mainGame->dField.hand[player][i], 10);
				mainGame->gMutex.unlock();
				mainGame->WaitFrameSignal(5);
			}
		}
		if (player == 0)
			myswprintf(event_string, dataManager.GetSysString(1611), count);
		else myswprintf(event_string, dataManager.GetSysString(1612), count);
		return true;
	}
	case MSG_DAMAGE: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int val = BufferIO::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] - val;
		if (final < 0)
			final = 0;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			mainGame->dInfo.lp[player] = final;
			myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
			return true;
		}
		soundManager.PlaySoundEffect(SOUND_DAMAGE);
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		if (player == 0)
			myswprintf(event_string, dataManager.GetSysString(1613), val);
		else
			myswprintf(event_string, dataManager.GetSysString(1614), val);
		mainGame->lpccolor = 0xffff0000;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"-%d", val);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_RECOVER: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int val = BufferIO::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] + val;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			mainGame->dInfo.lp[player] = final;
			myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
			return true;
		}
		soundManager.PlaySoundEffect(SOUND_RECOVER);
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		if (player == 0)
			myswprintf(event_string, dataManager.GetSysString(1615), val);
		else
			myswprintf(event_string, dataManager.GetSysString(1616), val);
		mainGame->lpccolor = 0xff00ff00;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"+%d", val);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_EQUIP: {
		int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l1 = BufferIO::ReadInt8(pbuf);
		int s1 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l2 = BufferIO::ReadInt8(pbuf);
		int s2 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			if(pc1->equipTarget)
				pc1->equipTarget->equipped.erase(pc1);
			pc1->equipTarget = pc2;
			pc2->equipped.insert(pc1);
		} else {
			soundManager.PlaySoundEffect(SOUND_EQUIP);
			mainGame->gMutex.lock();
			if(pc1->equipTarget) {
				pc1->is_showequip = false;
				pc1->equipTarget->is_showequip = false;
				pc1->equipTarget->equipped.erase(pc1);
			}
			pc1->equipTarget = pc2;
			pc2->equipped.insert(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showequip = true;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showequip = true;
			mainGame->gMutex.unlock();
		}
		return true;
	}
	case MSG_LPUPDATE: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int val = BufferIO::ReadInt32(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			mainGame->dInfo.lp[player] = val;
			myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
			return true;
		}
		mainGame->lpd = (mainGame->dInfo.lp[player] - val) / 10;
		mainGame->lpplayer = player;
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->dInfo.lp[player] = val;
		mainGame->gMutex.lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_UNEQUIP: {
		int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l1 = BufferIO::ReadInt8(pbuf);
		int s1 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c1, l1, s1);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pc->equipTarget->equipped.erase(pc);
			pc->equipTarget = 0;
		} else {
			mainGame->gMutex.lock();
			if (mainGame->dField.hovered_card == pc)
				pc->equipTarget->is_showequip = false;
			else if (mainGame->dField.hovered_card == pc->equipTarget)
				pc->is_showequip = false;
			pc->equipTarget->equipped.erase(pc);
			pc->equipTarget = 0;
			mainGame->gMutex.unlock();
		}
		return true;
	}
	case MSG_CARD_TARGET: {
		int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l1 = BufferIO::ReadInt8(pbuf);
		int s1 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l2 = BufferIO::ReadInt8(pbuf);
		int s2 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pc1->cardTarget.insert(pc2);
			pc2->ownerTarget.insert(pc1);
		} else {
			mainGame->gMutex.lock();
			pc1->cardTarget.insert(pc2);
			pc2->ownerTarget.insert(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showtarget = true;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showtarget = true;
			mainGame->gMutex.unlock();
		}
		break;
	}
	case MSG_CANCEL_TARGET: {
		int c1 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l1 = BufferIO::ReadInt8(pbuf);
		int s1 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l2 = BufferIO::ReadInt8(pbuf);
		int s2 = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			pc1->cardTarget.erase(pc2);
			pc2->ownerTarget.erase(pc1);
		} else {
			mainGame->gMutex.lock();
			pc1->cardTarget.erase(pc2);
			pc2->ownerTarget.erase(pc1);
			if (mainGame->dField.hovered_card == pc1)
				pc2->is_showtarget = false;
			else if (mainGame->dField.hovered_card == pc2)
				pc1->is_showtarget = false;
			mainGame->gMutex.unlock();
		}
		break;
	}
	case MSG_PAY_LPCOST: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int cost = BufferIO::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] - cost;
		if (final < 0)
			final = 0;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping) {
			mainGame->dInfo.lp[player] = final;
			myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
			return true;
		}
		soundManager.PlaySoundEffect(SOUND_DAMAGE);
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		mainGame->lpccolor = 0xff0000ff;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"-%d", cost);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(11);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_ADD_COUNTER: {
		int type = BufferIO::ReadInt16(pbuf);
		int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l = BufferIO::ReadInt8(pbuf);
		int s = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt16(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		if (pc->counters.count(type))
			pc->counters[type] += count;
		else pc->counters[type] = count;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_COUNTER_ADD);
		myswprintf(textBuffer, dataManager.GetSysString(1617), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type));
		pc->is_highlighting = true;
		mainGame->gMutex.lock();
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_REMOVE_COUNTER: {
		int type = BufferIO::ReadInt16(pbuf);
		int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l = BufferIO::ReadInt8(pbuf);
		int s = BufferIO::ReadInt8(pbuf);
		int count = BufferIO::ReadInt16(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		pc->counters[type] -= count;
		if (pc->counters[type] <= 0)
			pc->counters.erase(type);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_COUNTER_REMOVE);
		myswprintf(textBuffer, dataManager.GetSysString(1618), dataManager.GetName(pc->code), count, dataManager.GetCounterName(type));
		pc->is_highlighting = true;
		mainGame->gMutex.lock();
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_ATTACK: {
		int ca = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int la = BufferIO::ReadInt8(pbuf);
		int sa = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		mainGame->dField.attacker = mainGame->dField.GetCard(ca, la, sa);
		int cd = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int ld = BufferIO::ReadInt8(pbuf);
		int sd = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		float sy;
		if (ld != 0) {
			soundManager.PlaySoundEffect(SOUND_ATTACK);
			mainGame->dField.attack_target = mainGame->dField.GetCard(cd, ld, sd);
			myswprintf(event_string, dataManager.GetSysString(1619), dataManager.GetName(mainGame->dField.attacker->code),
			           dataManager.GetName(mainGame->dField.attack_target->code));
			float xa = mainGame->dField.attacker->curPos.X;
			float ya = mainGame->dField.attacker->curPos.Y;
			float xd = mainGame->dField.attack_target->curPos.X;
			float yd = mainGame->dField.attack_target->curPos.Y;
			sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
			mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
			if (ca == 0)
				mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
			else
				mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
		} else {
			soundManager.PlaySoundEffect(SOUND_DIRECT_ATTACK);
			myswprintf(event_string, dataManager.GetSysString(1620), dataManager.GetName(mainGame->dField.attacker->code));
			float xa = mainGame->dField.attacker->curPos.X;
			float ya = mainGame->dField.attacker->curPos.Y;
			float xd = 3.95f;
			float yd = 3.5f;
			if (ca == 0)
				yd = -3.5f;
			sy = (float)sqrt((xa - xd) * (xa - xd) + (ya - yd) * (ya - yd)) / 2;
			mainGame->atk_t = vector3df((xa + xd) / 2, (ya + yd) / 2, 0);
			if (ca == 0)
				mainGame->atk_r = vector3df(0, 0, -atan((xd - xa) / (yd - ya)));
			else
				mainGame->atk_r = vector3df(0, 0, 3.1415926 - atan((xd - xa) / (yd - ya)));
		}
		matManager.GenArrow(sy);
		mainGame->attack_sv = 0;
		mainGame->is_attacking = true;
		mainGame->WaitFrameSignal(40);
		mainGame->is_attacking = false;
		return true;
	}
	case MSG_BATTLE: {
		int ca = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int la = BufferIO::ReadInt8(pbuf);
		int sa = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int aatk = BufferIO::ReadInt32(pbuf);
		int adef = BufferIO::ReadInt32(pbuf);
		/*int da = */BufferIO::ReadInt8(pbuf);
		int cd = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int ld = BufferIO::ReadInt8(pbuf);
		int sd = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int datk = BufferIO::ReadInt32(pbuf);
		int ddef = BufferIO::ReadInt32(pbuf);
		/*int dd = */BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		mainGame->gMutex.lock();
		ClientCard* pcard = mainGame->dField.GetCard(ca, la, sa);
		if(aatk != pcard->attack) {
			pcard->attack = aatk;
			myswprintf(pcard->atkstring, L"%d", aatk);
		}
		if(adef != pcard->defense) {
			pcard->defense = adef;
			myswprintf(pcard->defstring, L"%d", adef);
		}
		if(ld) {
			pcard = mainGame->dField.GetCard(cd, ld, sd);
			if(datk != pcard->attack) {
				pcard->attack = datk;
				myswprintf(pcard->atkstring, L"%d", datk);
			}
			if(ddef != pcard->defense) {
				pcard->defense = ddef;
				myswprintf(pcard->defstring, L"%d", ddef);
			}
		}
		mainGame->gMutex.unlock();
		return true;
	}
	case MSG_ATTACK_DISABLED: {
		myswprintf(event_string, dataManager.GetSysString(1621), dataManager.GetName(mainGame->dField.attacker->code));
		return true;
	}
	case MSG_DAMAGE_STEP_START: {
		return true;
	}
	case MSG_DAMAGE_STEP_END: {
		return true;
	}
	case MSG_MISSED_EFFECT: {
		BufferIO::ReadInt32(pbuf);
		unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
		myswprintf(textBuffer, dataManager.GetSysString(1622), dataManager.GetName(code));
		mainGame->AddLog(textBuffer, code);
		return true;
	}
	case MSG_TOSS_COIN: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		wchar_t* pwbuf = textBuffer;
		BufferIO::CopyWStrRef(dataManager.GetSysString(1623), pwbuf, 256);
		for (int i = 0; i < count; ++i) {
			int res = BufferIO::ReadInt8(pbuf);
			*pwbuf++ = L'[';
			BufferIO::CopyWStrRef(dataManager.GetSysString(res ? 60 : 61), pwbuf, 256);
			*pwbuf++ = L']';
		}
		*pwbuf = 0;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_COIN);
		mainGame->gMutex.lock();
		mainGame->AddLog(textBuffer);
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_TOSS_DICE: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadInt8(pbuf);
		wchar_t* pwbuf = textBuffer;
		BufferIO::CopyWStrRef(dataManager.GetSysString(1624), pwbuf, 256);
		for (int i = 0; i < count; ++i) {
			int res = BufferIO::ReadInt8(pbuf);
			*pwbuf++ = L'[';
			*pwbuf++ = L'0' + res;
			*pwbuf++ = L']';
		}
		*pwbuf = 0;
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		soundManager.PlaySoundEffect(SOUND_DICE);
		mainGame->gMutex.lock();
		mainGame->AddLog(textBuffer);
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_ROCK_PAPER_SCISSORS: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		mainGame->gMutex.lock();
		mainGame->PopupElement(mainGame->wHand);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_HAND_RES: {
		int res = BufferIO::ReadInt8(pbuf);
		if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
			return true;
		mainGame->stHintMsg->setVisible(false);
		int res1 = (res & 0x3) - 1;
		int res2 = ((res >> 2) & 0x3) - 1;
		if(mainGame->dInfo.isFirst)
			mainGame->showcardcode = res1 + (res2 << 16);
		else
			mainGame->showcardcode = res2 + (res1 << 16);
		mainGame->showcarddif = 50;
		mainGame->showcardp = 0;
		mainGame->showcard = 100;
		mainGame->WaitFrameSignal(60);
		return false;
	}
	case MSG_ANNOUNCE_RACE: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		mainGame->dField.announce_count = BufferIO::ReadInt8(pbuf);
		int available = BufferIO::ReadInt32(pbuf);
		for(int i = 0, filter = 0x1; i < 25; ++i, filter <<= 1) {
			mainGame->chkRace[i]->setChecked(false);
			if(filter & available)
				mainGame->chkRace[i]->setVisible(true);
			else mainGame->chkRace[i]->setVisible(false);
		}
		if(select_hint)
			myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
		else myswprintf(textBuffer, dataManager.GetSysString(563));
		select_hint = 0;
		mainGame->gMutex.lock();
		mainGame->wANRace->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANRace);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_ANNOUNCE_ATTRIB: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		mainGame->dField.announce_count = BufferIO::ReadInt8(pbuf);
		int available = BufferIO::ReadInt32(pbuf);
		for(int i = 0, filter = 0x1; i < 7; ++i, filter <<= 1) {
			mainGame->chkAttribute[i]->setChecked(false);
			if(filter & available)
				mainGame->chkAttribute[i]->setVisible(true);
			else mainGame->chkAttribute[i]->setVisible(false);
		}
		if(select_hint)
			myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
		else myswprintf(textBuffer, dataManager.GetSysString(562));
		select_hint = 0;
		mainGame->gMutex.lock();
		mainGame->wANAttribute->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANAttribute);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_ANNOUNCE_CARD: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadUInt8(pbuf);
		mainGame->dField.declare_opcodes.clear();
		for (int i = 0; i < count; ++i)
			mainGame->dField.declare_opcodes.push_back(BufferIO::ReadInt32(pbuf));
		if(select_hint)
			myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
		else myswprintf(textBuffer, dataManager.GetSysString(564));
		select_hint = 0;
		mainGame->gMutex.lock();
		mainGame->ebANCard->setText(L"");
		mainGame->wANCard->setText(textBuffer);
		mainGame->dField.UpdateDeclarableList();
		mainGame->PopupElement(mainGame->wANCard);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_ANNOUNCE_NUMBER: {
		/*int player = */mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int count = BufferIO::ReadUInt8(pbuf);
		mainGame->gMutex.lock();
		mainGame->cbANNumber->clear();
		bool quickmode = count <= 12;
		if(quickmode) {
			for(int i = 0; i < 12; ++i) {
				mainGame->btnANNumber[i]->setEnabled(false);
				mainGame->btnANNumber[i]->setPressed(false);
				mainGame->btnANNumber[i]->setVisible(true);
			}
		}
		for (int i = 0; i < count; ++i) {
			int value = BufferIO::ReadInt32(pbuf);
			myswprintf(textBuffer, L" % d", value);
			mainGame->cbANNumber->addItem(textBuffer, value);
			if(quickmode) {
				if((value > 12 || value <= 0)) {
					quickmode = false;
				} else {
					mainGame->btnANNumber[value - 1]->setEnabled(true);
				}
			}
		}
		mainGame->cbANNumber->setSelected(0);
		if(quickmode) {
			mainGame->cbANNumber->setVisible(false);
			mainGame->btnANNumberOK->setRelativePosition(rect<s32>(20, 195, 210, 230));
			mainGame->btnANNumberOK->setEnabled(false);
			recti pos = mainGame->wANNumber->getRelativePosition();
			pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + 250;
			mainGame->wANNumber->setRelativePosition(pos);
		} else {
			for(int i = 0; i < 12; ++i) {
				mainGame->btnANNumber[i]->setVisible(false);
			}
			mainGame->cbANNumber->setVisible(true);
			mainGame->btnANNumberOK->setRelativePosition(rect<s32>(80, 60, 150, 85));
			recti pos = mainGame->wANNumber->getRelativePosition();
			pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + 95;
			mainGame->wANNumber->setRelativePosition(pos);
		}
		if(select_hint)
			myswprintf(textBuffer, L"%ls", dataManager.GetDesc(select_hint));
		else myswprintf(textBuffer, dataManager.GetSysString(565));
		select_hint = 0;
		mainGame->wANNumber->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANNumber);
		mainGame->gMutex.unlock();
		return false;
	}
	case MSG_CARD_HINT: {
		int c = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int l = BufferIO::ReadInt8(pbuf);
		int s = BufferIO::ReadInt8(pbuf);
		BufferIO::ReadInt8(pbuf);
		int chtype = BufferIO::ReadInt8(pbuf);
		int value = BufferIO::ReadInt32(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
		if(!pcard)
			return true;
		if(chtype == CHINT_DESC_ADD) {
			pcard->desc_hints[value]++;
		} else if(chtype == CHINT_DESC_REMOVE) {
			pcard->desc_hints[value]--;
			if(pcard->desc_hints[value] == 0)
				pcard->desc_hints.erase(value);
		} else {
			pcard->cHint = chtype;
			pcard->chValue = value;
			if(chtype == CHINT_TURN) {
				if(value == 0)
					return true;
				if(mainGame->dInfo.isReplay && mainGame->dInfo.isReplaySkiping)
					return true;
				if(pcard->location & LOCATION_ONFIELD)
					pcard->is_highlighting = true;
				mainGame->showcardcode = pcard->code;
				mainGame->showcarddif = 0;
				mainGame->showcardp = value - 1;
				mainGame->showcard = 6;
				mainGame->WaitFrameSignal(30);
				pcard->is_highlighting = false;
				mainGame->showcard = 0;
			}
		}
		return true;
	}
	case MSG_PLAYER_HINT: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		int chtype = BufferIO::ReadInt8(pbuf);
		int value = BufferIO::ReadInt32(pbuf);
		auto& player_desc_hints = mainGame->dField.player_desc_hints[player];
		if(value == CARD_QUESTION && player == 0) {
			if(chtype == PHINT_DESC_ADD) {
				mainGame->dField.cant_check_grave = true;
			} else if(chtype == PHINT_DESC_REMOVE) {
				mainGame->dField.cant_check_grave = false;
			}
		}
		else if(chtype == PHINT_DESC_ADD) {
			player_desc_hints[value]++;
		} else if(chtype == PHINT_DESC_REMOVE) {
			player_desc_hints[value]--;
			if(player_desc_hints[value] == 0)
				player_desc_hints.erase(value);
		}
		return true;
	}
	case MSG_MATCH_KILL: {
		match_kill = BufferIO::ReadInt32(pbuf);
		return true;
	}
	case MSG_TAG_SWAP: {
		int player = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
		size_t mcount = (size_t)BufferIO::ReadInt8(pbuf);
		size_t ecount = (size_t)BufferIO::ReadInt8(pbuf);
		size_t pcount = (size_t)BufferIO::ReadInt8(pbuf);
		size_t hcount = (size_t)BufferIO::ReadInt8(pbuf);
		int topcode = BufferIO::ReadInt32(pbuf);
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 0.4f;
				else (*cit)->dPos.Y = -0.6f;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = 5;
			}
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 0.4f;
				else (*cit)->dPos.Y = -0.6f;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = 5;
			}
			for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
				if(player == 0) (*cit)->dPos.Y = 0.4f;
				else (*cit)->dPos.Y = -0.6f;
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = 5;
			}
			mainGame->WaitFrameSignal(5);
		}
		//
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
			mainGame->gMutex.lock();
		if(mainGame->dField.deck[player].size() > mcount) {
			while(mainGame->dField.deck[player].size() > mcount) {
				ClientCard* ccard = *mainGame->dField.deck[player].rbegin();
				mainGame->dField.deck[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.deck[player].size() < mcount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_DECK;
				ccard->sequence = mainGame->dField.deck[player].size();
				mainGame->dField.deck[player].push_back(ccard);
			}
		}
		if(mainGame->dField.hand[player].size() > hcount) {
			while(mainGame->dField.hand[player].size() > hcount) {
				ClientCard* ccard = *mainGame->dField.hand[player].rbegin();
				mainGame->dField.hand[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.hand[player].size() < hcount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_HAND;
				ccard->sequence = mainGame->dField.hand[player].size();
				mainGame->dField.hand[player].push_back(ccard);
			}
		}
		if(mainGame->dField.extra[player].size() > ecount) {
			while(mainGame->dField.extra[player].size() > ecount) {
				ClientCard* ccard = *mainGame->dField.extra[player].rbegin();
				mainGame->dField.extra[player].pop_back();
				delete ccard;
			}
		} else {
			while(mainGame->dField.extra[player].size() < ecount) {
				ClientCard* ccard = new ClientCard();
				ccard->controler = player;
				ccard->location = LOCATION_EXTRA;
				ccard->sequence = mainGame->dField.extra[player].size();
				mainGame->dField.extra[player].push_back(ccard);
			}
		}
		mainGame->dField.extra_p_count[player] = pcount;
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping)
			mainGame->gMutex.unlock();
		//
		if(!mainGame->dInfo.isReplay || !mainGame->dInfo.isReplaySkiping) {
			for (auto cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			if(mainGame->dField.deck[player].size())
				(*mainGame->dField.deck[player].rbegin())->code = topcode;
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				pcard->code = BufferIO::ReadInt32(pbuf);
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			for (auto cit = mainGame->dField.extra[player].begin(); cit != mainGame->dField.extra[player].end(); ++cit) {
				ClientCard* pcard = *cit;
				pcard->code = BufferIO::ReadInt32(pbuf) & 0x7fffffff;
				mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
				if(player == 0) pcard->curPos.Y += 2.0f;
				else pcard->curPos.Y -= 3.0f;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		break;
	}
	case MSG_RELOAD_FIELD: {
		mainGame->gMutex.lock();
		mainGame->dField.Clear();
		mainGame->dInfo.duel_rule = BufferIO::ReadInt8(pbuf);
		int val = 0;
		for(int i = 0; i < 2; ++i) {
			int p = mainGame->LocalPlayer(i);
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
							xcard->owner = p;
							xcard->controler = p;
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
		mainGame->dField.RefreshAllCards();
		val = BufferIO::ReadInt8(pbuf); //chains, always 0 in single mode
		if(!mainGame->dInfo.isSingleMode) {
			for(int i = 0; i < val; ++i) {
				unsigned int code = (unsigned int)BufferIO::ReadInt32(pbuf);
				int pcc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
				int pcl = BufferIO::ReadInt8(pbuf);
				int pcs = BufferIO::ReadInt8(pbuf);
				int subs = BufferIO::ReadInt8(pbuf);
				int cc = mainGame->LocalPlayer(BufferIO::ReadInt8(pbuf));
				int cl = BufferIO::ReadInt8(pbuf);
				int cs = BufferIO::ReadInt8(pbuf);
				int desc = BufferIO::ReadInt32(pbuf);
				ClientCard* pcard = mainGame->dField.GetCard(pcc, pcl, pcs, subs);
				mainGame->dField.current_chain.chain_card = pcard;
				mainGame->dField.current_chain.code = code;
				mainGame->dField.current_chain.desc = desc;
				mainGame->dField.current_chain.controler = cc;
				mainGame->dField.current_chain.location = cl;
				mainGame->dField.current_chain.sequence = cs;
				mainGame->dField.GetChainLocation(cc, cl, cs, &mainGame->dField.current_chain.chain_pos);
				mainGame->dField.current_chain.solved = false;
				int chc = 0;
				for(auto chit = mainGame->dField.chains.begin(); chit != mainGame->dField.chains.end(); ++chit) {
					if(cl == 0x10 || cl == 0x20) {
						if(chit->controler == cc && chit->location == cl)
							chc++;
					} else {
						if(chit->controler == cc && chit->location == cl && chit->sequence == cs)
							chc++;
					}
				}
				if(cl == LOCATION_HAND)
					mainGame->dField.current_chain.chain_pos.X += 0.35f;
				else
					mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
				mainGame->dField.chains.push_back(mainGame->dField.current_chain);
			}
			if(val) {
				myswprintf(event_string, dataManager.GetSysString(1609), dataManager.GetName(mainGame->dField.current_chain.code));
				mainGame->dField.last_chain = true;
			}
		}
		mainGame->gMutex.unlock();
		break;
	}
	}
	return true;
}
void DuelClient::SwapField() {
	is_swapping = true;
}
void DuelClient::SetResponseI(int respI) {
	*((int*)response_buf) = respI;
	response_len = 4;
}
void DuelClient::SetResponseB(void* respB, unsigned char len) {
	memcpy(response_buf, respB, len);
	response_len = len;
}
void DuelClient::SendResponse() {
	switch(mainGame->dInfo.curMsg) {
	case MSG_SELECT_BATTLECMD: {
		mainGame->dField.ClearCommandFlag();
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		break;
	}
	case MSG_SELECT_IDLECMD: {
		mainGame->dField.ClearCommandFlag();
		mainGame->btnBP->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->btnShuffle->setVisible(false);
		break;
	}
	case MSG_SELECT_CARD:
	case MSG_SELECT_UNSELECT_CARD: {
		mainGame->dField.ClearSelect();
		break;
	}
	case MSG_SELECT_CHAIN: {
		mainGame->dField.ClearChainSelect();
		break;
	}
	case MSG_SELECT_TRIBUTE: {
		mainGame->dField.ClearSelect();
		break;
	}
	case MSG_SELECT_COUNTER: {
		mainGame->dField.ClearSelect();
		break;
	}
	case MSG_SELECT_SUM: {
		for(int i = 0; i < mainGame->dField.must_select_count; ++i) {
			mainGame->dField.selected_cards[i]->is_selected = false;
		}
		for(size_t i = 0; i < mainGame->dField.selectsum_all.size(); ++i) {
			mainGame->dField.selectsum_all[i]->is_selectable = false;
			mainGame->dField.selectsum_all[i]->is_selected = false;
		}
		break;
	}
	}
	if(mainGame->dInfo.isSingleMode) {
		SingleMode::SetResponse(response_buf, response_len);
		mainGame->singleSignal.Set();
	} else {
		mainGame->dInfo.time_player = 2;
		SendBufferToServer(CTOS_RESPONSE, response_buf, response_len);
	}
}
void DuelClient::BeginRefreshHost() {
	if(is_refreshing)
		return;
	is_refreshing = true;
	mainGame->btnLanRefresh->setEnabled(false);
	mainGame->lstHostList->clear();
	remotes.clear();
	hosts.clear();
	event_base* broadev = event_base_new();
	char hname[256];
	gethostname(hname, 256);
	hostent* host = gethostbyname(hname);
	if(!host)
		return;
	SOCKET reply = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in reply_addr;
	memset(&reply_addr, 0, sizeof(reply_addr));
	reply_addr.sin_family = AF_INET;
	reply_addr.sin_port = htons(7921);
	reply_addr.sin_addr.s_addr = 0;
	if(bind(reply, (sockaddr*)&reply_addr, sizeof(reply_addr)) == SOCKET_ERROR) {
		closesocket(reply);
		return;
	}
	timeval timeout = {3, 0};
	resp_event = event_new(broadev, reply, EV_TIMEOUT | EV_READ | EV_PERSIST, BroadcastReply, broadev);
	event_add(resp_event, &timeout);
	std::thread(RefreshThread, broadev).detach();
	//send request
	SOCKADDR_IN local;
	local.sin_family = AF_INET;
	local.sin_port = htons(7922);
	SOCKADDR_IN sockTo;
	sockTo.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	sockTo.sin_family = AF_INET;
	sockTo.sin_port = htons(7920);
	HostRequest hReq;
	hReq.identifier = NETWORK_CLIENT_ID;
	for(int i = 0; i < 8; ++i) {
		if(host->h_addr_list[i] == 0)
			break;
		unsigned int local_addr = *(unsigned int*)host->h_addr_list[i];
		local.sin_addr.s_addr = local_addr;
		SOCKET sSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(sSend == INVALID_SOCKET)
			break;
		BOOL opt = TRUE;
		setsockopt(sSend, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
		if(bind(sSend, (sockaddr*)&local, sizeof(sockaddr)) == SOCKET_ERROR) {
			closesocket(sSend);
			break;
		}
		sendto(sSend, (const char*)&hReq, sizeof(HostRequest), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
		closesocket(sSend);
	}
}
int DuelClient::RefreshThread(event_base* broadev) {
	event_base_dispatch(broadev);
	evutil_socket_t fd;
	event_get_assignment(resp_event, 0, &fd, 0, 0, 0);
	evutil_closesocket(fd);
	event_free(resp_event);
	event_base_free(broadev);
	is_refreshing = false;
	return 0;
}
void DuelClient::BroadcastReply(evutil_socket_t fd, short events, void * arg) {
	if(events & EV_TIMEOUT) {
		evutil_closesocket(fd);
		event_base_loopbreak((event_base*)arg);
		if(!is_closing)
			mainGame->btnLanRefresh->setEnabled(true);
	} else if(events & EV_READ) {
		sockaddr_in bc_addr;
		socklen_t sz = sizeof(sockaddr_in);
		char buf[256];
		/*int ret = */recvfrom(fd, buf, 256, 0, (sockaddr*)&bc_addr, &sz);
		unsigned int ipaddr = bc_addr.sin_addr.s_addr;
		HostPacket* pHP = (HostPacket*)buf;
		if(!is_closing && pHP->identifier == NETWORK_SERVER_ID && pHP->version == PRO_VERSION && remotes.find(ipaddr) == remotes.end() ) {
			mainGame->gMutex.lock();
			remotes.insert(ipaddr);
			pHP->ipaddr = ipaddr;
			hosts.push_back(*pHP);
			std::wstring hoststr;
			hoststr.append(L"[");
			hoststr.append(deckManager.GetLFListName(pHP->host.lflist));
			hoststr.append(L"][");
			hoststr.append(dataManager.GetSysString(pHP->host.rule + 1240));
			hoststr.append(L"][");
			hoststr.append(dataManager.GetSysString(pHP->host.mode + 1244));
			hoststr.append(L"][");
			if(pHP->host.draw_count == 1 && pHP->host.start_hand == 5 && pHP->host.start_lp == 8000
			        && !pHP->host.no_check_deck && !pHP->host.no_shuffle_deck
			        && pHP->host.duel_rule == DEFAULT_DUEL_RULE)
				hoststr.append(dataManager.GetSysString(1247));
			else hoststr.append(dataManager.GetSysString(1248));
			hoststr.append(L"]");
			wchar_t gamename[20];
			BufferIO::CopyWStr(pHP->name, gamename, 20);
			hoststr.append(gamename);
			mainGame->lstHostList->addItem(hoststr.c_str());
			mainGame->gMutex.unlock();
		}
	}
}
}
