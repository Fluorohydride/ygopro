#include "game.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"
#include <sys/time.h>
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
bool Game::SendByte(int player, char val) {
	netManager.send_buf[2] = val;
	return SendGameMessage(player, netManager.send_buffer_ptr, 1);
}
bool Game::SendGameMessage(int player, char* buf, int len) {
	*(short*)(buf - 2) = len;
	if(dInfo.is_host_player[player]) {
		gBuffer.Lock();
		memcpy(&msgBuffer[dInfo.msgLen], buf - 2, len + 2);
		dInfo.msgLen += len + 2;
		localMessage.Set();
		gBuffer.Unlock();
		return true;
	} else {
		return netManager.SendtoRemote(buf - 2, len + 2);
	}
}
bool Game::WaitforResponse(int player) {
	if(!SendByte(1 - player, MSG_WAITING))
		return false;
	dInfo.resPlayer = player;
	if(dInfo.is_host_player[player]) {
		localResponse.Wait();
		return true;
	} else {
		return netManager.WaitClientResponse();
	}
}
void Game::SetResponseI() {
	if(dInfo.is_responsed)
		return;
	dInfo.is_responsed = true;
	if(dInfo.is_local_host) {
		set_responsei(dInfo.pDuel, dInfo.responseI);
		lastReplay.WriteInt8(1, false);
		lastReplay.WriteInt32(dInfo.responseI);
	} else {
		char* pbuf = netManager.send_buf;
		NetManager::WriteInt8(pbuf, 1);
		NetManager::WriteInt32(pbuf, dInfo.responseI);
		netManager.SendtoRemote(netManager.send_buf, 5);
	}
}
void Game::SetResponseB(int len) {
	if(dInfo.is_responsed)
		return;
	dInfo.is_responsed = true;
	if(dInfo.is_local_host) {
		set_responseb(dInfo.pDuel, (byte*)dInfo.responseB);
		lastReplay.WriteInt8(2, false);
		lastReplay.WriteInt8(len, false);
		lastReplay.WriteData(dInfo.responseB, len);
	} else {
		char* pbuf = netManager.send_buf;
		NetManager::WriteInt8(pbuf, 2);
		NetManager::WriteInt8(pbuf, len);
		for(int i = 0; i < len; ++i)
			pbuf[i] = dInfo.responseB[i];
		netManager.SendtoRemote(netManager.send_buf, 2 + len);
	}
}
bool Game::RefreshMzone(int player, int flag, int use_cache) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_MZONE, flag, (unsigned char*)queryBuffer, use_cache);
	char* pbuf = netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_DATA);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, LOCATION_MZONE);
	memcpy(pbuf, queryBuffer, len);
	if(!SendGameMessage(player, netManager.send_buffer_ptr, len + 3))
		return false;
	pbuf = netManager.send_buffer_ptr + 3;
	for (int i = 0; i < 5; ++i) {
		int clen = NetManager::ReadInt32(pbuf);
		if (clen == 4)
			continue;
		if (pbuf[11] & POS_FACEDOWN)
			memset(pbuf, 0, clen - 4);
		pbuf += clen - 4;
	}
	return SendGameMessage(1 - player, netManager.send_buffer_ptr, len + 3);
}
bool Game::RefreshSzone(int player, int flag, int use_cache) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_SZONE, flag, (unsigned char*)queryBuffer, use_cache);
	char* pbuf = netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_DATA);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, LOCATION_SZONE);
	memcpy(pbuf, queryBuffer, len);
	if(!SendGameMessage(player, netManager.send_buffer_ptr, len + 3))
		return false;
	pbuf = netManager.send_buffer_ptr + 3;
	for (int i = 0; i < 6; ++i) {
		int clen = NetManager::ReadInt32(pbuf);
		if (clen == 4)
			continue;
		if (pbuf[11] & POS_FACEDOWN)
			memset(pbuf, 0, clen - 4);
		pbuf += clen - 4;
	}
	return SendGameMessage(1 - player, netManager.send_buffer_ptr, len + 3);
}
bool Game::RefreshHand(int player, int flag, int use_cache) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_HAND, flag, (unsigned char*)queryBuffer, use_cache);
	char* pbuf = netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_DATA);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, LOCATION_HAND);
	memcpy(pbuf, queryBuffer, len);
	return SendGameMessage(player, netManager.send_buffer_ptr, len + 3);
}
bool Game::RefreshGrave(int player, int flag, int use_cache) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_GRAVE, flag, (unsigned char*)queryBuffer, use_cache);
	char* pbuf = netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_DATA);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, LOCATION_GRAVE);
	memcpy(pbuf, queryBuffer, len);
	return SendGameMessage(0, netManager.send_buffer_ptr, len + 3)
	       && SendGameMessage(1, netManager.send_buffer_ptr, len + 3);
}
bool Game::RefreshExtra(int player, int flag, int use_cache) {
	int len = query_field_card(dInfo.pDuel, player, LOCATION_EXTRA, flag, (unsigned char*)queryBuffer, use_cache);
	char* pbuf = netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_DATA);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, LOCATION_EXTRA);
	memcpy(pbuf, queryBuffer, len);
	return SendGameMessage(player, netManager.send_buffer_ptr, len + 3);
}
bool Game::RefreshSingle(int player, int location, int sequence, int flag) {
	int len = query_card(dInfo.pDuel, player, location, sequence, flag, (unsigned char*)queryBuffer, 0);
	char* pbuf = netManager.send_buffer_ptr;
	if(location == LOCATION_REMOVED && (queryBuffer[15] & POS_FACEDOWN))
		return true;
	NetManager::WriteInt8(pbuf, MSG_UPDATE_CARD);
	NetManager::WriteInt8(pbuf, player);
	NetManager::WriteInt8(pbuf, location);
	NetManager::WriteInt8(pbuf, sequence);
	memcpy(pbuf, queryBuffer, len);
	if(!SendGameMessage(player, netManager.send_buffer_ptr, len + 4))
		return false;
	if ((location & 0x90) || ((location & 0x2c) && (queryBuffer[15] & POS_FACEUP)))
		return SendGameMessage(1 - player, netManager.send_buffer_ptr, len + 4);
	return true;
}
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
void Game::ReplayRefreshGrave(int player, int flag) {
	int len = query_field_card(dInfo.pDuel, 0, LOCATION_GRAVE, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateFieldCard(mainGame->LocalPlayer(player), LOCATION_GRAVE, queryBuffer);
}
void Game::ReplayRefreshSingle(int player, int location, int sequence, int flag) {
	int len = query_card(dInfo.pDuel, player, location, sequence, flag, (unsigned char*)queryBuffer, 0);
	dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, queryBuffer);
}
int Game::CardReader(int code, void* pData) {
	mainGame->dataManager.GetData(code, (CardData*)pData);
	return 0;
}
int Game::MessageHandler(long fduel, int type) {
	char msgbuf[256];
	get_log_message(fduel, (byte*)msgbuf);
	FILE* fp = fopen("error.log", "at+");
	fprintf(fp, "[Script error:] %s\n", msgbuf);
	fclose(fp);
	return 0;
}
int Game::EngineThread(void* pd) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	ReplayHeader rh;
	rh.id = 0x31707279;
	rh.version = PROTO_VERSION;
	pdInfo->is_local_host = true;
	time_t seed = time(0);
	rh.seed = seed;
	mainGame->lastReplay.BeginRecord();
	mainGame->lastReplay.WriteHeader(rh);
	mainGame->rnd.reset(seed);
	if(mainGame->netManager.hInfo.no_shuffle_player || mainGame->rnd.real() < 0.5) {
		pdInfo->is_host_player[0] = true;
		pdInfo->is_host_player[1] = false;
		pdInfo->is_first_turn = true;
		mainGame->lastReplay.WriteData(pdInfo->hostname, 40, false);
		mainGame->lastReplay.WriteData(pdInfo->clientname, 40, false);
	} else {
		pdInfo->is_host_player[0] = false;
		pdInfo->is_host_player[1] = true;
		pdInfo->is_first_turn = false;
		mainGame->lastReplay.WriteData(pdInfo->clientname, 40, false);
		mainGame->lastReplay.WriteData(pdInfo->hostname, 40, false);
	}
	if(!mainGame->netManager.hInfo.no_shuffle_deck) {
		for(int i = 0; i < mainGame->deckManager.deckhost.main.size(); ++i) {
			int swap = mainGame->rnd.real() * mainGame->deckManager.deckhost.main.size();
			auto tmp = mainGame->deckManager.deckhost.main[i];
			mainGame->deckManager.deckhost.main[i] = mainGame->deckManager.deckhost.main[swap];
			mainGame->deckManager.deckhost.main[swap] = tmp;
		}
	}
	if(!mainGame->netManager.hInfo.no_shuffle_deck) {
		for(int i = 0; i < mainGame->deckManager.deckclient.main.size(); ++i) {
			int swap = mainGame->rnd.real() * mainGame->deckManager.deckclient.main.size();
			auto tmp = mainGame->deckManager.deckclient.main[i];
			mainGame->deckManager.deckclient.main[i] = mainGame->deckManager.deckclient.main[swap];
			mainGame->deckManager.deckclient.main[swap] = tmp;
		}
	}
	mainGame->dField.panel = 0;
	mainGame->dField.hovered_card = 0;
	mainGame->dField.clicked_card = 0;
	set_card_reader((card_reader)Game::CardReader);
	set_message_handler((message_handler)Game::MessageHandler);
	mainGame->rnd.reset(seed);
	pdInfo->pDuel = create_duel(mainGame->rnd.rand());
	HostInfo& hi = mainGame->netManager.hInfo;
	set_player_info(pdInfo->pDuel, 0, hi.start_lp, hi.start_hand, hi.draw_count);
	set_player_info(pdInfo->pDuel, 1, hi.start_lp, hi.start_hand, hi.draw_count);
	int opt = 0;
	if(mainGame->netManager.hInfo.no_chain_hint)
		opt |= DUEL_NO_CHAIN_HINT;
	if(mainGame->netManager.hInfo.attack_ft)
		opt |= DUEL_ATTACK_FIRST_TURN;
	mainGame->lastReplay.WriteInt32(hi.start_lp, false);
	mainGame->lastReplay.WriteInt32(hi.start_hand, false);
	mainGame->lastReplay.WriteInt32(hi.draw_count, false);
	mainGame->lastReplay.WriteInt32(opt, false);
	mainGame->lastReplay.Flush();

	if(pdInfo->is_first_turn) {
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.main.size(), false);
		for(int i = mainGame->deckManager.deckhost.main.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckhost.main[i]->first, 0, 0, LOCATION_DECK, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.main[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.extra.size(), false);
		for(int i = mainGame->deckManager.deckhost.extra.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckhost.extra[i]->first, 0, 0, LOCATION_EXTRA, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.extra[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.main.size(), false);
		for(int i = mainGame->deckManager.deckclient.main.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckclient.main[i]->first, 1, 1, LOCATION_DECK, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.main[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.extra.size(), false);
		for(int i = mainGame->deckManager.deckclient.extra.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckclient.extra[i]->first, 1, 1, LOCATION_EXTRA, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.extra[i]->first, false);
		}
	} else {
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.main.size(), false);
		for(int i = mainGame->deckManager.deckclient.main.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckclient.main[i]->first, 0, 0, LOCATION_DECK, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.main[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.extra.size(), false);
		for(int i = mainGame->deckManager.deckclient.extra.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckclient.extra[i]->first, 0, 0, LOCATION_EXTRA, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckclient.extra[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.main.size(), false);
		for(int i = mainGame->deckManager.deckhost.main.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckhost.main[i]->first, 1, 1, LOCATION_DECK, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.main[i]->first, false);
		}
		mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.extra.size(), false);
		for(int i = mainGame->deckManager.deckhost.extra.size() - 1; i >= 0; --i) {
			new_card(pdInfo->pDuel, mainGame->deckManager.deckhost.extra[i]->first, 1, 1, LOCATION_EXTRA, 0, 0);
			mainGame->lastReplay.WriteInt32(mainGame->deckManager.deckhost.extra[i]->first, false);
		}
	}
	mainGame->lastReplay.Flush();
	char* pbuf = mainGame->netManager.send_buffer_ptr;
	NetManager::WriteInt8(pbuf, MSG_START);
	NetManager::WriteInt32(pbuf, 0);
	NetManager::WriteInt32(pbuf, hi.start_lp);
	NetManager::WriteInt32(pbuf, hi.start_lp);
	NetManager::WriteInt32(pbuf, query_field_count(pdInfo->pDuel, 0, 0x1));
	NetManager::WriteInt32(pbuf, query_field_count(pdInfo->pDuel, 0, 0x40));
	NetManager::WriteInt32(pbuf, query_field_count(pdInfo->pDuel, 1, 0x1));
	NetManager::WriteInt32(pbuf, query_field_count(pdInfo->pDuel, 1, 0x40));
	mainGame->SendGameMessage(0, mainGame->netManager.send_buffer_ptr, 29);
	mainGame->netManager.send_buffer_ptr[1] = 1;
	mainGame->SendGameMessage(1, mainGame->netManager.send_buffer_ptr, 29);
	mainGame->RefreshExtra(0);
	mainGame->RefreshExtra(1);
	start_duel(pdInfo->pDuel, opt);
	Proceed(pd);
	end_duel(pdInfo->pDuel);
	shutdown(mainGame->netManager.sListen, SD_BOTH);
	closesocket(mainGame->netManager.sListen);
	shutdown(mainGame->netManager.sRemote, SD_BOTH);
	closesocket(mainGame->netManager.sRemote);
	pdInfo->isStarted = false;
	mainGame->localMessage.Set();
	mainGame->localResponse.Set();
	if(!mainGame->is_closing) {
		mainGame->gMutex.Lock();
		mainGame->wCardImg->setVisible(false);
		mainGame->stHintMsg->setVisible(false);
		mainGame->stTip->setVisible(false);
		mainGame->wInfos->setVisible(false);
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
		mainGame->ShowElement(mainGame->wModeSelection);
		mainGame->imageManager.ClearTexture();
		mainGame->gMutex.Unlock();
	}
	return 0;
}
void Game::Proceed(void* pd) {
	char engineBuffer[0x1000];
	DuelInfo* pdInfo = (DuelInfo*)pd;
	pdInfo->netError = false;
	mainGame->localResponse.Reset();
	pdInfo->engFlag = 0;
	while (pdInfo->isStarted && !pdInfo->netError && !mainGame->is_closing) {
		if (pdInfo->engFlag == 2) {
			pdInfo->engFlag = 0;
			break;
		}
		int result = process(pdInfo->pDuel);
		pdInfo->engLen = result & 0xffff;
		pdInfo->engFlag = result >> 16;
		if (pdInfo->engLen > 0) {
			get_message(pdInfo->pDuel, (byte*)&engineBuffer[2]);
			Analyze(pd, engineBuffer);
		}
	}
	if(!mainGame->is_closing && pdInfo->netError) {
		mainGame->gMutex.Lock();
		mainGame->ShowElement(mainGame->wACMessage, 60);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(90);
	}
}
void Game::Analyze(void* pd, char* engbuf) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	char* offset, *pbufw, *pbuf = &engbuf[2];
	int player, count, type;
	while (pbuf - &engbuf[2] < pdInfo->engLen) {
		if(mainGame->is_closing)
			return;
		offset = pbuf;
		pdInfo->engType = NetManager::ReadUInt8(pbuf);
		switch (pdInfo->engType) {
		case MSG_RETRY: {
			mainGame->SendGameMessage(pdInfo->resPlayer, offset, pbuf - offset);
			mainGame->WaitforResponse(pdInfo->resPlayer);
			break;
		}
		case MSG_HINT: {
			type = NetManager::ReadInt8(pbuf);
			player = NetManager::ReadInt8(pbuf);
			NetManager::ReadInt32(pbuf);
			switch (type) {
			case 1:
			case 2:
			case 3:
			case 5: {
				mainGame->SendGameMessage(player, offset, pbuf - offset);
				break;
			}
			case 4:
			case 6:
			case 7:
			case 8:
			case 9: {
				mainGame->SendGameMessage(1 - player, offset, pbuf - offset);
				break;
			}
			}
			break;
		}
		case MSG_WIN: {
			player = NetManager::ReadInt8(pbuf);
			type = NetManager::ReadInt8(pbuf);
			mainGame->localResponse.Reset();
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			pdInfo->engFlag = 2;
			mainGame->lastReplay.EndRecord();
			char* pbuf = mainGame->netManager.send_buf;
			NetManager::WriteInt16(pbuf, sizeof(ReplayHeader) + mainGame->lastReplay.comp_size + 1);
			NetManager::WriteInt8(pbuf, MSG_REPLAY);
			memcpy(pbuf, &mainGame->lastReplay.pheader, sizeof(ReplayHeader));
			pbuf += sizeof(ReplayHeader);
			memcpy(pbuf, mainGame->lastReplay.comp_data, mainGame->lastReplay.comp_size);
			mainGame->netManager.SendtoRemote(mainGame->netManager.send_buf, 3 + sizeof(ReplayHeader) + mainGame->lastReplay.comp_size);
			mainGame->msgBuffer[2] = MSG_DUEL_END;
			if(mainGame->dInfo.is_host_player[0])
				mainGame->SendGameMessage(1, &mainGame->msgBuffer[2], 1);
			else mainGame->SendGameMessage(0, &mainGame->msgBuffer[2], 1);
			mainGame->localResponse.Wait();
			mainGame->ebRSName->setText(L"");
			mainGame->PopupElement(mainGame->wReplaySave);
			mainGame->localAction.Reset();
			mainGame->localAction.Wait();
			if(mainGame->dInfo.is_host_player[0])
				mainGame->SendGameMessage(0, &mainGame->msgBuffer[2], 1);
			else mainGame->SendGameMessage(1, &mainGame->msgBuffer[2], 1);
			break;
		}
		case MSG_SELECT_BATTLECMD: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 11;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8 + 2;
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			mainGame->RefreshHand(0);
			mainGame->RefreshHand(1);
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
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
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			mainGame->RefreshHand(0);
			mainGame->RefreshHand(1);
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_EFFECTYN: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 8;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_YESNO: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 4;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_OPTION: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_CARD:
		case MSG_SELECT_TRIBUTE: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 3;
			count = NetManager::ReadInt8(pbuf);
			int c, l, s, ss, code;
			for (int i = 0; i < count; ++i) {
				pbufw = pbuf;
				code = NetManager::ReadInt32(pbuf);
				c = NetManager::ReadInt8(pbuf);
				l = NetManager::ReadInt8(pbuf);
				s = NetManager::ReadInt8(pbuf);
				ss = NetManager::ReadInt8(pbuf);
				if (c != player) NetManager::WriteInt32(pbufw, 0);
			}
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_CHAIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 12;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_PLACE:
		case MSG_SELECT_DISFIELD: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_POSITION: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_COUNTER: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 3;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SELECT_SUM: {
			pbuf++;
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 11;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_SORT_CARD:
		case MSG_SORT_CHAIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_CONFIRM_DECKTOP: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CONFIRM_CARDS: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 7;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_DECK: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SHUFFLE_HAND: {
			player = NetManager::ReadInt8(pbuf);
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshHand(player, 0x181fff, 0);
			break;
		}
		case MSG_REFRESH_DECK: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = NetManager::ReadInt8(pbuf);
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshGrave(player);
			break;
		}
		case MSG_SHUFFLE_SET_CARD: {
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0, 0x181fff, 0);
			mainGame->RefreshMzone(1, 0x181fff, 0);
			break;
		}
		case MSG_NEW_TURN: {
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_NEW_PHASE: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_MOVE: {
			pbufw = pbuf;
			int pc = pbuf[4];
			int pl = pbuf[5];
			int ps = pbuf[6];
			int pp = pbuf[7];
			int cc = pbuf[8];
			int cl = pbuf[9];
			int cs = pbuf[10];
			int cp = pbuf[11];
			pbuf += 12;
			if(cl == LOCATION_REMOVED && (cp & POS_FACEDOWN))
				NetManager::WriteInt32(pbufw, 0);
			mainGame->SendGameMessage(cc, offset, pbuf - offset);
			if (!(cl & 0xb0) && !((cl & 0xc) && (cp & POS_FACEUP)))
				NetManager::WriteInt32(pbufw, 0);
			mainGame->SendGameMessage(1 - cc, offset, pbuf - offset);
			if (cl != 0 && (cl & 0x80) == 0 && (cl != pl || pc != cc))
				mainGame->RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_DESTROY: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_RELEASE: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_POS_CHANGE: {
			int cc = pbuf[4];
			int cl = pbuf[5];
			int cs = pbuf[6];
			int pp = pbuf[7];
			int cp = pbuf[8];
			pbuf += 9;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			if((pp & POS_FACEDOWN) && (cp & POS_FACEUP))
				mainGame->RefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_SET: {
			NetManager::WriteInt32(pbuf, 0);
			pbuf += 4;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SWAP: {
			pbuf += 16;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_FIELD_DISABLED: {
			pbuf += 4;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONING: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SUMMONED: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_SPSUMMONING: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_SPSUMMONED: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_FLIPSUMMONING: {
			mainGame->RefreshSingle(pbuf[4], pbuf[5], pbuf[6]);
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_FLIPSUMMONED: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_CHAINING: {
			pbuf += 16;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CHAINED: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_CHAIN_SOLVING: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CHAIN_SOLVED: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_CHAIN_END: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_CHAIN_INACTIVATED: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			mainGame->RefreshSzone(0);
			mainGame->RefreshSzone(1);
			break;
		}
		case MSG_CHAIN_DISABLED: {
			pbuf++;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CARD_SELECTED:
		case MSG_RANDOM_SELECTED: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			mainGame->SendGameMessage(1 - player, offset, pbuf - offset);
			break;
		}
		case MSG_BECOME_TARGET: {
			count = NetManager::ReadInt8(pbuf);
			pbuf += count * 4;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_DRAW: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbufw = pbuf;
			pbuf += count * 4;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			for (int i = 0; i < count; ++i)
				NetManager::WriteInt32(pbufw, 0);
			mainGame->SendGameMessage(1 - player, offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE: {
			pbuf += 5;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_RECOVER: {
			pbuf += 5;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_EQUIP: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_LPUPDATE: {
			pbuf += 5;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_UNEQUIP: {
			pbuf += 4;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CARD_TARGET: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_CANCEL_TARGET: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_PAY_LPCOST: {
			pbuf += 5;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_ADD_COUNTER: {
			pbuf += 6;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_REMOVE_COUNTER: {
			pbuf += 6;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK: {
			pbuf += 8;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_BATTLE: {
			pbuf += 18;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_ATTACK_DISABLED: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_DAMAGE_STEP_START: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			break;
		}
		case MSG_DAMAGE_STEP_END: {
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			mainGame->RefreshMzone(0);
			mainGame->RefreshMzone(1);
			break;
		}
		case MSG_MISSED_EFFECT: {
			player = pbuf[0];
			pbuf += 8;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_COIN: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_TOSS_DICE: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += count;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		case MSG_ANNOUNCE_RACE: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_ANNOUNCE_ATTRIB: {
			player = NetManager::ReadInt8(pbuf);
			pbuf += 5;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_ANNOUNCE_CARD: {
			player = NetManager::ReadInt8(pbuf);
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_ANNOUNCE_NUMBER: {
			player = NetManager::ReadInt8(pbuf);
			count = NetManager::ReadInt8(pbuf);
			pbuf += 4 * count;
			mainGame->SendGameMessage(player, offset, pbuf - offset);
			mainGame->WaitforResponse(player);
			break;
		}
		case MSG_COUNT_TURN: {
			pbuf += 6;
			mainGame->SendGameMessage(0, offset, pbuf - offset);
			mainGame->SendGameMessage(1, offset, pbuf - offset);
			break;
		}
		}
	}
}
int Game::RecvThread(void* pd) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	int roffset = 0;
	short packlen = 0;
	bool mwrite = false, mend = false;
	mainGame->dInfo.netError = false;
	int recvlen = recv(mainGame->netManager.sRemote, mainGame->netManager.recv_buf + roffset, 4096, 0);
	while (recvlen > 0 && !mainGame->is_closing) {
		char* rbuf = mainGame->netManager.recv_buf;
		int left = roffset + recvlen;
		mainGame->gBuffer.Lock();
		mwrite = false;
		packlen = NetManager::ReadInt16(rbuf);
		while(packlen + 2 <= left) {
			memcpy(mainGame->msgBuffer + pdInfo->msgLen, rbuf - 2, packlen + 2);
			pdInfo->msgLen += packlen + 2;
			if(NetManager::ReadUInt8(rbuf) == MSG_DUEL_END)
				mend = true;
			rbuf += packlen - 1;
			left -= packlen + 2;
			mwrite = true;
			if(left < 2) {
				rbuf += 2;
				break;
			}
			packlen = NetManager::ReadInt16(rbuf);
		}
		rbuf -= 2;
		roffset = left;
		if(left) {
			roffset = left;
			for(int i = 0; i < left; ++i)
				mainGame->netManager.recv_buf[i] = rbuf[i];
		}
		if(mwrite)
			mainGame->localMessage.Set();
		mainGame->gBuffer.Unlock();
		if(mend)
			break;
		recvlen = recv(mainGame->netManager.sRemote, mainGame->netManager.recv_buf + roffset, 4096, 0);
	}
	if((recvlen == 0 || recvlen == SOCKET_ERROR) && !mainGame->is_closing) {
		if(recvlen == 0) {
			mainGame->gMutex.Lock();
			mainGame->stACMessage->setText(L"与对方的连接中断");
			mainGame->dInfo.netError = true;
			mainGame->gMutex.Unlock();
		} else {
			mainGame->gMutex.Lock();
			mainGame->stACMessage->setText(L"网络连接发生错误");
			mainGame->dInfo.netError = true;
			mainGame->gMutex.Unlock();
		}
		mainGame->ShowElement(mainGame->wACMessage, 60);
		mainGame->WaitFrameSignal(90);
	}
	if(mend) {
		mainGame->localResponse.Reset();
		mainGame->localResponse.Wait();
	}
	shutdown(mainGame->netManager.sRemote, SD_BOTH);
	closesocket(mainGame->netManager.sRemote);
	pdInfo->isStarted = false;
	mainGame->localMessage.Set();
	if(!mainGame->is_closing) {
		mainGame->gMutex.Lock();
		mainGame->wCardImg->setVisible(false);
		mainGame->stHintMsg->setVisible(false);
		mainGame->stTip->setVisible(false);
		mainGame->wInfos->setVisible(false);
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
		mainGame->ShowElement(mainGame->wModeSelection);
		mainGame->imageManager.ClearTexture();
		mainGame->gMutex.Unlock();
	}
	return 0;
}
int Game::GameThread(void* pd) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	pdInfo->isStarted = true;
	short packlen;
	bool result = true;
	char* pbuf = mainGame->msgBuffer;
	mainGame->dInfo.netError = false;
	do {
		mainGame->gBuffer.Lock();
		if (pbuf - mainGame->msgBuffer >= pdInfo->msgLen) {
			pbuf = mainGame->msgBuffer;
			pdInfo->msgLen = 0;
			mainGame->localMessage.Reset();
			mainGame->gBuffer.Unlock();
			mainGame->localMessage.Wait();
			continue;
		}
		mainGame->gBuffer.Unlock();
		packlen = NetManager::ReadInt16(pbuf);
		result = SolveMessage(pd, pbuf, packlen);
		pbuf += packlen;
	} while (result && pdInfo->isStarted && !pdInfo->netError && !mainGame->is_closing);
	return 0;
}
bool Game::SolveMessage(void* pd, char* msg, int len) {
	DuelInfo* pdInfo = (DuelInfo*)pd;
	char* pbuf = msg;
	wchar_t textBuffer[256];
	pdInfo->is_responsed = false;
	pdInfo->preMsg = pdInfo->curMsg;
	pdInfo->curMsg = NetManager::ReadUInt8(pbuf);
	mainGame->wCmdMenu->setVisible(false);
	if(pdInfo->curMsg != MSG_WAITING && pdInfo->curMsg != MSG_CARD_SELECTED) {
		mainGame->waitFrame = -1;
		mainGame->stHintMsg->setVisible(false);
		if(mainGame->wCardSelect->isVisible()) {
			mainGame->HideElement(mainGame->wCardSelect);
			mainGame->WaitFrameSignal(10);
		}
	}
	switch(pdInfo->curMsg) {
	case MSG_RETRY: {
		mainGame->stACMessage->setText(L"Error occurs.");
		mainGame->PopupElement(mainGame->wACMessage, 100);
		mainGame->WaitFrameSignal(120);
		if(!mainGame->dField.is_replaying) {
			pdInfo->isStarted = false;
			mainGame->localResponse.Set();
		}
		return false;
	}
	case MSG_HINT: {
		int type = NetManager::ReadInt8(pbuf);
		int player = NetManager::ReadInt8(pbuf);
		int data = NetManager::ReadInt32(pbuf);
		switch (type) {
		case HINT_EVENT: {
			myswprintf(pdInfo->strEvent, L"%ls", mainGame->dataManager.GetDesc(data));
			break;
		}
		case HINT_MESSAGE: {
			mainGame->stMessage->setText(mainGame->dataManager.GetDesc(data));
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->localAction.Reset();
			mainGame->localAction.Wait();
			break;
		}
		case HINT_SELECTMSG: {
			mainGame->dInfo.selectHint = data;
			break;
		}
		case HINT_OPSELECTED: {
			myswprintf(textBuffer, L"对方选择了：[%ls]", mainGame->dataManager.GetDesc(data));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(0);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->textFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
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
			myswprintf(textBuffer, L"对方宣言了：[%ls]", DataManager::FormatRace(data));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(0);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->textFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_ATTRIB: {
			myswprintf(textBuffer, L"对方宣言了：[%ls]", DataManager::FormatAttribute(data));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(0);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->textFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_CODE: {
			myswprintf(textBuffer, L"对方宣言了：[%ls]", mainGame->dataManager.GetName(data));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(data);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->textFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
			mainGame->WaitFrameSignal(40);
			break;
		}
		case HINT_NUMBER: {
			myswprintf(textBuffer, L"对方选择了：[%d]", data);
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(0);
			mainGame->SetStaticText(mainGame->stACMessage, 310, mainGame->textFont, textBuffer);
			mainGame->PopupElement(mainGame->wACMessage, 20);
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
		}
		break;
	}
	case MSG_WIN: {
		int player = NetManager::ReadInt8(pbuf);
		int type = NetManager::ReadInt8(pbuf);
		if(player == 2)
			myswprintf(textBuffer, L"Draw Game.\n原因：%ls", mainGame->dataManager.GetVictoryString(type));
		else if(mainGame->LocalPlayer(player) == 0) {
			if(!mainGame->dField.is_replaying) {
				if(type == 1 || type == 2)
					myswprintf(textBuffer, L"You Win！\n原因：%ls %ls", mainGame->LocalName(1), mainGame->dataManager.GetVictoryString(type));
				else myswprintf(textBuffer, L"You Win！\n原因：%ls", mainGame->dataManager.GetVictoryString(type));
			} else {
				if(type == 1 || type == 2)
					myswprintf(textBuffer, L"%ls Win！\n原因：%ls %ls", mainGame->LocalName(0), mainGame->LocalName(1), mainGame->dataManager.GetVictoryString(type));
				else myswprintf(textBuffer, L"%ls Win！\n原因：%ls", mainGame->LocalName(0), mainGame->dataManager.GetVictoryString(type));
			}
		} else {
			if(!mainGame->dField.is_replaying) {
				if(type == 1 || type == 2)
					myswprintf(textBuffer, L"You Lose！\n原因：%ls %ls", mainGame->LocalName(0), mainGame->dataManager.GetVictoryString(type));
				else myswprintf(textBuffer, L"You Lose！\n原因：%ls", mainGame->dataManager.GetVictoryString(type));
			} else {
				if(type == 1 || type == 2)
					myswprintf(textBuffer, L"%ls Win！\n原因：%ls %ls", mainGame->LocalName(1), mainGame->LocalName(0), mainGame->dataManager.GetVictoryString(type));
				else myswprintf(textBuffer, L"%ls Win！\n原因：%ls", mainGame->LocalName(1), mainGame->dataManager.GetVictoryString(type));
			}
		}
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 100);
		mainGame->WaitFrameSignal(120);
		if(mainGame->dInfo.is_local_host)
			mainGame->localResponse.Set();
		break;
	}
	case MSG_DUEL_END: {
		pdInfo->isStarted = false;
		mainGame->localResponse.Set();
		return false;
	}
	case MSG_REPLAY: {
		pbuf -= 3;
		int size = NetManager::ReadInt16(pbuf);
		pbuf++;
		memcpy(&mainGame->lastReplay.pheader, pbuf, sizeof(ReplayHeader));
		pbuf += sizeof(ReplayHeader);
		memcpy(mainGame->lastReplay.comp_data, pbuf, size - sizeof(ReplayHeader) - 1);
		mainGame->lastReplay.comp_size = size - sizeof(ReplayHeader) - 1;
		mainGame->PopupElement(mainGame->wReplaySave);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		break;
	}
	case MSG_WAITING: {
		mainGame->waitFrame = 0;
		mainGame->gMutex.Lock();
		mainGame->stHintMsg->setText(L"等待对方行动中...");
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_START: {
		mainGame->gMutex.Lock();
		pdInfo->is_first_turn = NetManager::ReadInt32(pbuf) ? false : true;
		pdInfo->lp[mainGame->LocalPlayer(0)] = NetManager::ReadInt32(pbuf);
		pdInfo->lp[mainGame->LocalPlayer(1)] = NetManager::ReadInt32(pbuf);
		myswprintf(pdInfo->strLP[0], L"%d", pdInfo->lp[0]);
		myswprintf(pdInfo->strLP[1], L"%d", pdInfo->lp[1]);
		int deckc = NetManager::ReadInt32(pbuf);
		int extrac = NetManager::ReadInt32(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(0), deckc, extrac);
		deckc = NetManager::ReadInt32(pbuf);
		extrac = NetManager::ReadInt32(pbuf);
		mainGame->dField.Initial(mainGame->LocalPlayer(1), deckc, extrac);
		pdInfo->turn = 0;
		pdInfo->strTurn[0] = 0;
		pdInfo->is_shuffling = false;
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_UPDATE_DATA: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int location = NetManager::ReadInt8(pbuf);
		mainGame->gMutex.Lock();
		mainGame->dField.UpdateFieldCard(player, location, pbuf);
		mainGame->gMutex.Unlock();
		if (location == LOCATION_HAND && pdInfo->is_shuffling) {
			pdInfo->is_shuffling = false;
			for(int i = 0; i < mainGame->dField.hand[0].size(); ++i) {
				mainGame->dField.hand[0][i]->is_hovered = false;
				mainGame->dField.MoveCard(mainGame->dField.hand[0][i], 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_UPDATE_CARD: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int loc = NetManager::ReadInt8(pbuf);
		int seq = NetManager::ReadInt8(pbuf);
		mainGame->gMutex.Lock();
		mainGame->dField.UpdateCard(player, loc, seq, pbuf);
		mainGame->gMutex.Unlock();
		break;
	}
	case MSG_SELECT_BATTLECMD: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int code, desc, count, con, loc, seq, diratt;
		ClientCard* pcard;
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc =  NetManager::ReadInt8(pbuf);
			seq =  NetManager::ReadInt8(pbuf);
			desc =  NetManager::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(desc);
			pcard->cmdFlag |= COMMAND_ACTIVATE;
			if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
		}
		mainGame->dField.attackable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			diratt = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.attackable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_ATTACK;
		}
		mainGame->gMutex.Lock();
		if(NetManager::ReadInt8(pbuf)) {
			mainGame->btnM2->setVisible(true);
			mainGame->btnM2->setEnabled(true);
			mainGame->btnM2->setPressed(false);
		}
		if(NetManager::ReadInt8(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearCommandFlag();
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_IDLECMD: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int code, desc, count, con, loc, seq;
		ClientCard* pcard;
		mainGame->dField.summonable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.summonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SUMMON;
		}
		mainGame->dField.spsummonable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.spsummonable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SPSUMMON;
			if (pcard->location == LOCATION_DECK) {
				pcard->SetCode(code);
				mainGame->dField.deck_act = true;
			}
			if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
			if (pcard->location == LOCATION_EXTRA)
				mainGame->dField.extra_act = true;
		}
		mainGame->dField.reposable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.reposable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_REPOS;
		}
		mainGame->dField.msetable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.msetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_MSET;
		}
		mainGame->dField.ssetable_cards.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.ssetable_cards.push_back(pcard);
			pcard->cmdFlag |= COMMAND_SSET;
		}
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		count = NetManager::ReadInt8(pbuf);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			con = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			loc = NetManager::ReadInt8(pbuf);
			seq = NetManager::ReadInt8(pbuf);
			desc = NetManager::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(con, loc, seq);
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(desc);
			pcard->cmdFlag |= COMMAND_ACTIVATE;
			if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
		}
		if(NetManager::ReadInt8(pbuf)) {
			mainGame->btnBP->setVisible(true);
			mainGame->btnBP->setEnabled(true);
			mainGame->btnBP->setPressed(false);
		}
		if(NetManager::ReadInt8(pbuf)) {
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setEnabled(true);
			mainGame->btnEP->setPressed(false);
		}
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearCommandFlag();
		mainGame->btnBP->setVisible(false);
		mainGame->btnEP->setVisible(false);
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_EFFECTYN: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int code = NetManager::ReadInt32(pbuf);
		int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l = NetManager::ReadInt8(pbuf);
		int s = NetManager::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
		if (pcard->code != code)
			pcard->SetCode(code);
		NetManager::ReadInt8(pbuf);
		myswprintf(textBuffer, L"是否在[%ls]发动[%ls]的效果？", DataManager::FormatLocation(l), mainGame->dataManager.GetName(code));
		mainGame->gMutex.Lock();
		mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, textBuffer);
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_YESNO: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int desc = NetManager::ReadInt32(pbuf);
		mainGame->gMutex.Lock();
		mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetDesc(desc));
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_OPTION: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_options.clear();
		for (int i = 0; i < count; ++i)
			mainGame->dField.select_options.push_back(NetManager::ReadInt32(pbuf));
		mainGame->gMutex.Lock();
		mainGame->SetStaticText(mainGame->stOptions, 310, mainGame->textFont,
		                        (wchar_t*)mainGame->dataManager.GetDesc(mainGame->dField.select_options[0]));
		mainGame->btnOptionp->setVisible(false);
		if(count > 1)
			mainGame->btnOptionn->setVisible(true);
		else mainGame->btnOptionn->setVisible(false);
		mainGame->dField.selected_option = 0;
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint));
		else myswprintf(textBuffer, L"请选择一个选项：");
		mainGame->dInfo.selectHint = 0;
		mainGame->wOptions->setText(textBuffer);
		mainGame->PopupElement(mainGame->wOptions);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_CARD: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_cancelable = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_min = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_max = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		int c, l, s, ss, code;
		bool panelmode = false;
		mainGame->dField.select_ready = false;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			ss = NetManager::ReadInt8(pbuf);
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
		std::sort(mainGame->dField.selectable_cards.begin(), mainGame->dField.selectable_cards.end(), ClientCard::client_card_sort);
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls(%d-%d)", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint),
			           mainGame->dField.select_min, mainGame->dField.select_max);
		else myswprintf(textBuffer, L"请选择卡：(%d-%d)", mainGame->dField.select_min, mainGame->dField.select_max);
		mainGame->dInfo.selectHint = 0;
		if (panelmode) {
			mainGame->gMutex.Lock();
			mainGame->wCardSelect->setText(textBuffer);
			mainGame->dField.ShowSelectCard();
			mainGame->gMutex.Unlock();
		} else {
			mainGame->stHintMsg->setText(textBuffer);
			mainGame->stHintMsg->setVisible(true);
		}
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearSelect();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_CHAIN: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		int specount = NetManager::ReadInt8(pbuf);
		int c, l, s, code, desc;
		ClientCard* pcard;
		mainGame->dField.activatable_cards.clear();
		mainGame->dField.activatable_descs.clear();
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			desc = NetManager::ReadInt32(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			mainGame->dField.activatable_cards.push_back(pcard);
			mainGame->dField.activatable_descs.push_back(desc);
			pcard->is_selectable = true;
			pcard->is_selected = false;
			pcard->cmdFlag |= COMMAND_ACTIVATE;
			if (pcard->location == LOCATION_GRAVE)
				mainGame->dField.grave_act = true;
			if (pcard->location == LOCATION_REMOVED)
				mainGame->dField.remove_act = true;
		}
		if(mainGame->ignore_chain || ((count == 0 || specount == 0) && !mainGame->always_chain)) {
			mainGame->dInfo.responseI = -1;
			mainGame->SetResponseI();
			mainGame->dField.ClearChainSelect();
			if(mainGame->chkWaitChain->isChecked()) {
				mainGame->WaitFrameSignal(mainGame->rnd.real() * 20 + 20);
			}
			mainGame->localResponse.Set();
			return true;
		}
		mainGame->gMutex.Lock();
		if(count == 0)
			myswprintf(textBuffer, L"此时没有可以发动的效果\n是否要确认场上的情况？");
		else
			myswprintf(textBuffer, L"%ls\n是否进行连锁？", pdInfo->strEvent);
		mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)textBuffer);
		mainGame->PopupElement(mainGame->wQuery);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearChainSelect();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_PLACE:
	case MSG_SELECT_DISFIELD: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_min = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectable_field = ~NetManager::ReadInt32(pbuf);
		mainGame->dField.selected_field = 0;
		if (mainGame->dInfo.curMsg == MSG_SELECT_PLACE && mainGame->chkAutoPos->isChecked()) {
			int filter;
			if (mainGame->dField.selectable_field & 0x1f) {
				mainGame->dInfo.responseB[0] = mainGame->dInfo.is_first_turn ? 0 : 1;
				mainGame->dInfo.responseB[1] = 0x4;
				filter = mainGame->dField.selectable_field & 0x1f;
			} else if (mainGame->dField.selectable_field & 0x1f00) {
				mainGame->dInfo.responseB[0] = mainGame->dInfo.is_first_turn ? 0 : 1;
				mainGame->dInfo.responseB[1] = 0x8;
				filter = (mainGame->dField.selectable_field >> 8) & 0x1f;
			} else if (mainGame->dField.selectable_field & 0x1f0000) {
				mainGame->dInfo.responseB[0] = mainGame->dInfo.is_first_turn ? 1 : 0;
				mainGame->dInfo.responseB[1] = 0x4;
				filter = (mainGame->dField.selectable_field >> 16) & 0x1f;
			} else {
				mainGame->dInfo.responseB[0] = mainGame->dInfo.is_first_turn ? 1 : 0;
				mainGame->dInfo.responseB[1] = 0x8;
				filter = (mainGame->dField.selectable_field >> 24) & 0x1f;
			}
			if(mainGame->chkRandomPos->isChecked()) {
				mainGame->dInfo.responseB[2] = mainGame->rnd.real() * 5;
				while(!(filter & (1 << mainGame->dInfo.responseB[2])))
					mainGame->dInfo.responseB[2] = mainGame->rnd.real() * 5;
			} else {
				if (filter & 0x4) mainGame->dInfo.responseB[2] = 2;
				else if (filter & 0x2) mainGame->dInfo.responseB[2] = 1;
				else if (filter & 0x8) mainGame->dInfo.responseB[2] = 3;
				else if (filter & 0x1) mainGame->dInfo.responseB[2] = 0;
				else if (filter & 0x10) mainGame->dInfo.responseB[2] = 4;
			}
			mainGame->dField.selectable_field = 0;
			mainGame->SetResponseB(3);
			mainGame->localResponse.Set();
			return true;
		}
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_POSITION: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		int code = NetManager::ReadInt32(pbuf);
		int positions = NetManager::ReadInt8(pbuf);
		if (positions == 0x1 || positions == 0x2 || positions == 0x4 || positions == 0x8) {
			mainGame->dInfo.responseI = positions;
			mainGame->SetResponseI();
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
		mainGame->PopupElement(mainGame->wPosSelect);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_TRIBUTE: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_cancelable = NetManager::ReadInt8(pbuf) ? true : false;
		mainGame->dField.select_min = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_max = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		int c, l, s, t, code;
		ClientCard* pcard;
		mainGame->dField.select_ready = false;
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			t = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->opParam = t;
			pcard->select_seq = i;
			pcard->is_selectable = true;
		}
		mainGame->stHintMsg->setText(L"请选择上级召唤用需要解放的怪兽：");
		mainGame->stHintMsg->setVisible(false);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearSelect();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_COUNTER: {
		int selecting_player = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_counter_type = NetManager::ReadInt16(pbuf);
		mainGame->dField.select_counter_count = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		int c, l, s, t, code;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			t = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->opParam = t;
			pcard->select_seq = i;
			pcard->is_selectable = true;
			mainGame->dInfo.responseB[i] = 0;
		}
		myswprintf(textBuffer, L"请移除%d个[%ls]:", mainGame->dField.select_counter_count, mainGame->dataManager.GetCounterName(mainGame->dField.select_counter_type));
		mainGame->gMutex.Lock();
		mainGame->stHintMsg->setText(textBuffer);
		mainGame->stHintMsg->setVisible(true);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearSelect();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SELECT_SUM: {
		mainGame->dField.select_mode = NetManager::ReadInt8(pbuf);
		int selecting_player = NetManager::ReadInt8(pbuf);
		mainGame->dField.select_max = NetManager::ReadInt32(pbuf);
		mainGame->dField.select_min = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectsum_all.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.selectsum_cards.clear();
		int c, l, s, code;
		bool panelmode = false;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->opParam = NetManager::ReadInt32(pbuf);
			pcard->select_seq = i;
			mainGame->dField.selectsum_all.push_back(pcard);
			if ((l & 0xe) == 0)
				panelmode = true;
		}
		std::sort(mainGame->dField.selectsum_all.begin(), mainGame->dField.selectsum_all.end(), ClientCard::client_card_sort);
		mainGame->dField.CheckSelectSum();
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls(%d)", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint), mainGame->dField.select_max);
		else myswprintf(textBuffer, L"请选择卡：(%d)", mainGame->dField.select_max);
		mainGame->dInfo.selectHint = 0;
		if (panelmode) {
			mainGame->wCardSelect->setText(textBuffer);
			mainGame->dField.ShowSelectCard();
		} else {
			mainGame->stHintMsg->setText(textBuffer);
			mainGame->stHintMsg->setVisible(true);
		}
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->dField.ClearSelect();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_SORT_CARD:
	case MSG_SORT_CHAIN: {
		int player = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		mainGame->dField.selectable_cards.clear();
		mainGame->dField.selected_cards.clear();
		mainGame->dField.sort_list.clear();
		int c, l, s, code;
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			mainGame->dField.selectable_cards.push_back(pcard);
			pcard->select_seq = i;
			mainGame->dField.sort_list.push_back(0);
		}
		if (mainGame->chkAutoChain->isChecked() && pdInfo->curMsg == MSG_SORT_CHAIN) {
			mainGame->dInfo.responseI = -1;
			mainGame->SetResponseI();
			mainGame->localResponse.Set();
			return true;
		}
		if(pdInfo->curMsg == MSG_SORT_CHAIN)
			mainGame->wCardSelect->setText(L"请选择连锁顺序：");
		else
			mainGame->wCardSelect->setText(L"请选择排列顺序：");
		mainGame->dField.select_min = 0;
		mainGame->dField.select_max = count;
		mainGame->dField.ShowSelectCard();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_CONFIRM_DECKTOP: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		int code, c, l, s;
		ClientCard* pcard;
		myswprintf(textBuffer, L"翻开卡组上方%d张卡：", count);
		mainGame->lstLog->addItem(textBuffer);
		mainGame->logParam.push_back(0);
		mainGame->dField.selectable_cards.clear();
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0)
				pcard->SetCode(code);
			myswprintf(textBuffer, L"%d [%ls]", i, mainGame->dataManager.GetName(code));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(code);
			float shift = -0.15f;
			if (player == 1) shift = 0.15f;
			pcard->dPos = irr::core::vector3df(shift, 0, 0);
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
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		int code, c, l, s;
		std::vector<ClientCard*> field_confirm;
		ClientCard*  pcard;
		myswprintf(textBuffer, L"确认%d张卡：", count);
		mainGame->lstLog->addItem(textBuffer);
		mainGame->logParam.push_back(0);
		for (int i = 0; i < count; ++i) {
			code = NetManager::ReadInt32(pbuf);
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			if (code != 0)
				pcard->SetCode(code);
			myswprintf(textBuffer, L"%d [%ls]", i, mainGame->dataManager.GetName(code));
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(code);
			if (l & 0x41) {
				float shift = -0.15f;
				if ((c == 0 && l == 0x40) || (c == 1 && l == 0x1)) shift = 0.15f;
				pcard->dPos = irr::core::vector3df(shift, 0, 0);
				pcard->dRot = irr::core::vector3df(0, 3.14159f / 5.0f, 0);
				pcard->is_moving = true;
				pcard->aniFrame = 5;
				mainGame->WaitFrameSignal(45);
				mainGame->dField.MoveCard(pcard, 5);
				mainGame->WaitFrameSignal(5);
			} else
				field_confirm.push_back(pcard);
		}
		if (field_confirm.size() > 0) {
			for(int i = 0; i < field_confirm.size(); ++i) {
				pcard = field_confirm[i];
				c = pcard->controler;
				l = pcard->location;
				if (l == LOCATION_HAND) {
					mainGame->dField.MoveCard(pcard, 5);
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
			mainGame->WaitFrameSignal(45);
			for(int i = 0; i < field_confirm.size(); ++i) {
				if (pcard->location != LOCATION_HAND)
					mainGame->dField.MoveCard(pcard, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_SHUFFLE_DECK: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		std::vector<ClientCard*>::iterator cit;
		for (int i = 0; i < mainGame->dField.deck[player].size(); ++i)
			mainGame->dField.deck[player][i]->code = 0;
		for (int i = 0; i < 5; ++i) {
			for (cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
				(*cit)->dPos = irr::core::vector3df(rand() * 0.4f / RAND_MAX - 0.2f, 0, 0);
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->aniFrame = 3;
			}
			mainGame->WaitFrameSignal(3);
			for (cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit)
				mainGame->dField.MoveCard(*cit, 3);
			mainGame->WaitFrameSignal(3);
		}
		return true;
	}
	case MSG_SHUFFLE_HAND: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		std::vector<ClientCard*>::iterator cit;
		mainGame->WaitFrameSignal(5);
		if(player == 1) {
			bool flip = false;
			for (cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
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
		for (cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
			(*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / 5, 0, 0);
			(*cit)->dRot = irr::core::vector3df(0, 0, 0);
			(*cit)->is_moving = true;
			(*cit)->is_hovered = false;
			(*cit)->aniFrame = 5;
		}
		mainGame->WaitFrameSignal(20);
		for (cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit)
			(*cit)->SetCode(0);
		if (player == 0)
			mainGame->dInfo.is_shuffling = true;
		else {
			for (cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->is_hovered = false;
				mainGame->dField.MoveCard(*cit, 5);
			}
			mainGame->WaitFrameSignal(5);
		}
		return true;
	}
	case MSG_REFRESH_DECK: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		return true;
	}
	case MSG_SWAP_GRAVE_DECK: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		std::vector<ClientCard*>::iterator cit;
		mainGame->gMutex.Lock();
		mainGame->dField.grave[player].swap(mainGame->dField.deck[player]);
		for (cit = mainGame->dField.grave[player].begin(); cit != mainGame->dField.grave[player].end(); ++cit) {
			(*cit)->location = LOCATION_GRAVE;
			mainGame->dField.MoveCard(*cit, 10);
		}
		for (cit = mainGame->dField.deck[player].begin(); cit != mainGame->dField.deck[player].end(); ++cit) {
			(*cit)->location = LOCATION_DECK;
			mainGame->dField.MoveCard(*cit, 10);
		}
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_SHUFFLE_SET_CARD: {
		std::vector<ClientCard*>::iterator cit;
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* mc[5];
		ClientCard* swp;
		int c, l, s, ps;
		for (int i = 0; i < count; ++i) {
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			NetManager::ReadInt8(pbuf);
			mc[i] = mainGame->dField.mzone[c][s];
			mc[i]->dPos = irr::core::vector3df((3.95f - mc[i]->curPos.X) / 10, 0, 0.05f);
			mc[i]->dRot = irr::core::vector3df(0, 0, 0);
			mc[i]->is_moving = true;
			mc[i]->aniFrame = 10;
		}
		mainGame->WaitFrameSignal(20);
		for (int i = 0; i < count; ++i) {
			c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			l = NetManager::ReadInt8(pbuf);
			s = NetManager::ReadInt8(pbuf);
			NetManager::ReadInt8(pbuf);
			ps = mc[i]->sequence;
			if (l > 0) {
				swp = mainGame->dField.mzone[c][s];
				mainGame->dField.mzone[c][ps] = swp;
				mainGame->dField.mzone[c][s] = mc[i];
				mc[i]->sequence = s;
				swp->sequence = ps;
			}
		}
		for (int i = 0; i < count; ++i) {
			mainGame->dField.MoveCard(mc[i], 10);
			for (cit = mc[i]->overlayed.begin(); cit != mc[i]->overlayed.end(); ++cit)
				mainGame->dField.MoveCard(*cit, 10);
		}
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_NEW_TURN: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		mainGame->dInfo.turn++;
		myswprintf(mainGame->dInfo.strTurn, L"Turn:%d", mainGame->dInfo.turn);
		myswprintf(textBuffer, L"%ls的回合", mainGame->LocalName(player));
		mainGame->gMutex.Lock();
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_NEW_PHASE: {
		int phase = (unsigned char)NetManager::ReadInt8(pbuf);
		mainGame->gMutex.Lock();
		mainGame->btnDP->setVisible(false);
		mainGame->btnSP->setVisible(false);
		mainGame->btnM1->setVisible(false);
		mainGame->btnBP->setVisible(false);
		mainGame->btnM2->setVisible(false);
		mainGame->btnEP->setVisible(false);
		switch (phase) {
		case PHASE_DRAW:
			mainGame->btnDP->setVisible(true);
			myswprintf(textBuffer, L"进入【抽卡阶段】");
			break;
		case PHASE_STANDBY:
			mainGame->btnSP->setVisible(true);
			myswprintf(textBuffer, L"进入【准备阶段】");
			break;
		case PHASE_MAIN1:
			mainGame->btnM1->setVisible(true);
			myswprintf(textBuffer, L"进入【主要阶段１】");
			break;
		case PHASE_BATTLE:
			mainGame->btnBP->setVisible(true);
			mainGame->btnBP->setPressed(true);
			mainGame->btnBP->setEnabled(false);
			myswprintf(textBuffer, L"进入【战斗阶段】");
			break;
		case PHASE_MAIN2:
			mainGame->btnM2->setVisible(true);
			mainGame->btnM2->setPressed(true);
			mainGame->btnM2->setEnabled(false);
			myswprintf(textBuffer, L"进入【主要阶段２】");
			break;
		case PHASE_END:
			mainGame->btnEP->setVisible(true);
			mainGame->btnEP->setPressed(true);
			mainGame->btnEP->setEnabled(false);
			myswprintf(textBuffer, L"进入【结束阶段】");
			break;
		}
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_MOVE: {
		int code = NetManager::ReadInt32(pbuf);
		int pc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int pl = NetManager::ReadUInt8(pbuf);
		int ps = NetManager::ReadInt8(pbuf);
		int pp = NetManager::ReadInt8(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadUInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		if (pl == 0) {
			ClientCard* pcard = new ClientCard();
			pcard->position = cp;
			pcard->SetCode(code);
			mainGame->gMutex.Lock();
			mainGame->dField.AddCard(pcard, cc, cl, cs);
			mainGame->gMutex.Unlock();
			mainGame->dField.GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
			pcard->mTransform.setTranslation(pcard->curPos);
			pcard->mTransform.setRotationRadians(pcard->curRot);
			pcard->curAlpha = 5;
			mainGame->dField.FadeCard(pcard, 255, 20);
			mainGame->WaitFrameSignal(20);
		} else if (cl == 0) {
			ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
			if (code != 0 && pcard->code != code)
				pcard->SetCode(code);
			pcard->ClearTarget();
			mainGame->dField.FadeCard(pcard, 5, 20);
			mainGame->WaitFrameSignal(20);
			mainGame->gMutex.Lock();
			mainGame->dField.RemoveCard(pc, pl, ps);
			mainGame->gMutex.Unlock();
			if(pcard == mainGame->dField.hovered_card)
				mainGame->dField.hovered_card = 0;
			delete pcard;
		} else {
			if (!(pl & 0x80) && !(cl & 0x80)) {
				ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
				if (code != 0 && pcard->code != code)
					pcard->SetCode(code);
				if((pl & LOCATION_ONFIELD) && (cl != pl))
					pcard->counters.clear();
				if(cl != pl)
					pcard->ClearTarget();
				mainGame->gMutex.Lock();
				mainGame->dField.RemoveCard(pc, pl, ps);
				pcard->position = cp;
				mainGame->dField.AddCard(pcard, cc, cl, cs);
				mainGame->gMutex.Unlock();
				if (pl == cl && pc == cc && (cl & 0x71)) {
					pcard->dPos = irr::core::vector3df(-0.2f, 0, 0);
					pcard->dRot = irr::core::vector3df(0, 0, 0);
					if (pc == 1) pcard->dPos.X = 0.2f;
					pcard->is_moving = true;
					pcard->aniFrame = 5;
					mainGame->WaitFrameSignal(5);
					mainGame->dField.MoveCard(pcard, 5);
					mainGame->WaitFrameSignal(5);
				} else {
					if (cl == 0x4 && pcard->overlayed.size() > 0) {
						mainGame->gMutex.Lock();
						for (int i = 0; i < pcard->overlayed.size(); ++i)
							mainGame->dField.MoveCard(pcard->overlayed[i], 10);
						mainGame->gMutex.Unlock();
						mainGame->WaitFrameSignal(5);
					}
					if (cl == 0x2) {
						mainGame->gMutex.Lock();
						for (int i = 0; i < mainGame->dField.hand[cc].size(); ++i)
							mainGame->dField.MoveCard(mainGame->dField.hand[cc][i], 10);
						mainGame->gMutex.Unlock();
					} else {
						mainGame->gMutex.Lock();
						mainGame->dField.MoveCard(pcard, 10);
						if (pl == 0x2)
							for (int i = 0; i < mainGame->dField.hand[pc].size(); ++i)
								mainGame->dField.MoveCard(mainGame->dField.hand[pc][i], 10);
						mainGame->gMutex.Unlock();
					}
					mainGame->WaitFrameSignal(5);
				}
			} else if (!(pl & 0x80)) {
				ClientCard* pcard = mainGame->dField.GetCard(pc, pl, ps);
				if (code != 0 && pcard->code != code)
					pcard->SetCode(code);
				if((pl & LOCATION_ONFIELD) && (cl != pl))
					pcard->counters.clear();
				if(cl != pl)
					pcard->ClearTarget();
				ClientCard* olcard = mainGame->dField.GetCard(cc, cl & 0x7f, cs);
				mainGame->gMutex.Lock();
				mainGame->dField.RemoveCard(pc, pl, ps);
				olcard->overlayed.push_back(pcard);
				mainGame->dField.overlay_cards.insert(pcard);
				mainGame->gMutex.Unlock();
				pcard->overlayTarget = olcard;
				pcard->location = 0x80;
				pcard->sequence = olcard->overlayed.size() - 1;
				if (olcard->location == 0x4) {
					mainGame->gMutex.Lock();
					mainGame->dField.MoveCard(pcard, 10);
					if (pl == 0x2)
						for (int i = 0; i < mainGame->dField.hand[pc].size(); ++i)
							mainGame->dField.MoveCard(mainGame->dField.hand[pc][i], 10);
					mainGame->gMutex.Unlock();
					mainGame->WaitFrameSignal(5);
				}
			} else if (!(cl & 0x80)) {
				ClientCard* olcard = mainGame->dField.GetCard(pc, pl & 0x7f, ps);
				ClientCard* pcard = olcard->overlayed[pp];
				mainGame->gMutex.Lock();
				olcard->overlayed.erase(olcard->overlayed.begin() + pcard->sequence);
				pcard->overlayTarget = 0;
				pcard->position = cp;
				mainGame->dField.AddCard(pcard, cc, cl, cs);
				mainGame->dField.overlay_cards.erase(pcard);
				for (int i = 0; i < olcard->overlayed.size(); ++i) {
					olcard->overlayed[i]->sequence = i;
					mainGame->dField.MoveCard(olcard->overlayed[i], 2);
				}
				mainGame->gMutex.Unlock();
				mainGame->WaitFrameSignal(5);
				mainGame->gMutex.Lock();
				mainGame->dField.MoveCard(pcard, 10);
				mainGame->gMutex.Unlock();
				mainGame->WaitFrameSignal(5);
			} else {
				ClientCard* olcard1 = mainGame->dField.GetCard(pc, pl & 0x7f, ps);
				ClientCard* pcard = olcard1->overlayed[pp];
				ClientCard* olcard2 = mainGame->dField.GetCard(cc, cl & 0x7f, cs);
				mainGame->gMutex.Lock();
				olcard1->overlayed.erase(olcard1->overlayed.begin() + pcard->sequence);
				olcard2->overlayed.push_back(pcard);
				pcard->sequence = olcard2->overlayed.size() - 1;
				pcard->location = 0x80;
				pcard->overlayTarget = olcard2;
				for (int i = 0; i < olcard1->overlayed.size(); ++i) {
					olcard1->overlayed[i]->sequence = i;
					mainGame->dField.MoveCard(olcard1->overlayed[i], 2);
				}
				mainGame->dField.MoveCard(pcard, 10);
				mainGame->gMutex.Unlock();
				mainGame->WaitFrameSignal(5);
			}
		}
		return true;
	}
	case MSG_DESTROY: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"卡片被破坏了");
		return true;
	}
	case MSG_RELEASE: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"卡片被解放了");
		return true;
	}
	case MSG_POS_CHANGE: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int pp = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
		if((pp & POS_FACEUP) && (cp & POS_FACEDOWN)) {
			pcard->counters.clear();
			pcard->ClearTarget();
		}
		if (code != 0 && pcard->code != code)
			pcard->SetCode(code);
		pcard->position = cp;
		myswprintf(pdInfo->strEvent, L"卡片改变了表示形式");
		mainGame->dField.MoveCard(pcard, 10);
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_SET: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"放置了卡片");
		return true;
	}
	case MSG_SWAP: {
		int code1 = NetManager::ReadInt32(pbuf);
		int c1 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l1 = NetManager::ReadInt8(pbuf);
		int s1 = NetManager::ReadInt8(pbuf);
		int p1 = NetManager::ReadInt8(pbuf);
		int code2 = NetManager::ReadInt32(pbuf);
		int c2 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l2 = NetManager::ReadInt8(pbuf);
		int s2 = NetManager::ReadInt8(pbuf);
		int p2 = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"卡的控制权改变了");
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		mainGame->gMutex.Lock();
		mainGame->dField.RemoveCard(c1, l1, s1);
		mainGame->dField.RemoveCard(c2, l2, s2);
		mainGame->dField.AddCard(pc1, c2, l2, s2);
		mainGame->dField.AddCard(pc2, c1, l1, s1);
		mainGame->dField.MoveCard(pc1, 10);
		mainGame->dField.MoveCard(pc2, 10);
		for (int i = 0; i < pc1->overlayed.size(); ++i)
			mainGame->dField.MoveCard(pc1->overlayed[i], 10);
		for (int i = 0; i < pc2->overlayed.size(); ++i)
			mainGame->dField.MoveCard(pc2->overlayed[i], 10);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_FIELD_DISABLED: {
		int disabled = NetManager::ReadInt32(pbuf);
		if (!mainGame->dInfo.is_first_turn)
			disabled = (disabled >> 16) | (disabled << 16);
		mainGame->dField.disabled_field = disabled;
		return true;
	}
	case MSG_SUMMONING: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"[%ls]召唤中...", mainGame->dataManager.GetName(code));
		mainGame->showcardcode = code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 4;
		mainGame->WaitFrameSignal(30);
		mainGame->showcard = 0;
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_SUMMONED: {
		myswprintf(pdInfo->strEvent, L"怪兽召唤成功");
		return true;
	}
	case MSG_SPSUMMONING: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		myswprintf(pdInfo->strEvent, L"[%ls]特殊召唤中...", mainGame->dataManager.GetName(code));
		myswprintf(pdInfo->strEvent, L"[%ls]召唤中...", mainGame->dataManager.GetName(code));
		mainGame->showcardcode = code;
		mainGame->showcarddif = 1;
		mainGame->showcard = 5;
		mainGame->WaitFrameSignal(30);
		mainGame->showcard = 0;
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_SPSUMMONED: {
		myswprintf(pdInfo->strEvent, L"怪兽特殊召唤成功");
		return true;
	}
	case MSG_FLIPSUMMONING: {
		int code = NetManager::ReadInt32(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int cp = NetManager::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(cc, cl, cs);
		pcard->SetCode(code);
		pcard->position = cp;
		myswprintf(pdInfo->strEvent, L"[%ls]反转召唤中...", mainGame->dataManager.GetName(code));
		mainGame->dField.MoveCard(pcard, 10);
		mainGame->WaitFrameSignal(10);
		mainGame->showcardcode = code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 4;
		mainGame->WaitFrameSignal(30);
		mainGame->showcard = 0;
		mainGame->WaitFrameSignal(10);
		return true;
	}
	case MSG_FLIPSUMMONED: {
		myswprintf(pdInfo->strEvent, L"怪兽反转召唤成功");
		return true;
	}
	case MSG_CHAINING: {
		int code = NetManager::ReadInt32(pbuf);
		int pcc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int pcl = NetManager::ReadInt8(pbuf);
		int pcs = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int cc = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cl = NetManager::ReadInt8(pbuf);
		int cs = NetManager::ReadInt8(pbuf);
		int desc = NetManager::ReadInt32(pbuf);
		int ct = NetManager::ReadInt8(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(pcc, pcl, pcs);
		mainGame->showcardcode = code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 1;
		pcard->is_highlighting = true;
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
		int chc = 0;
		for(int i = 0; i < mainGame->dField.chains.size(); ++i) {
			if (cl == 0x10 || cl == 0x20) {
				if (mainGame->dField.chains[i].controler == cc && mainGame->dField.chains[i].location == cl)
					chc++;
			} else {
				if (mainGame->dField.chains[i].controler == cc && mainGame->dField.chains[i].location == cl && mainGame->dField.chains[i].sequence == cs)
					chc++;
			}
		}
		if(cl == LOCATION_HAND)
			mainGame->dField.current_chain.chain_pos.X += 0.35;
		else
			mainGame->dField.current_chain.chain_pos.Y += chc * 0.25f;
		return true;
	}
	case MSG_CHAINED: {
		int ct = NetManager::ReadInt8(pbuf);
		myswprintf(mainGame->dInfo.strEvent, L"[%ls]的效果发动", mainGame->dataManager.GetName(mainGame->dField.current_chain.code));
		mainGame->gMutex.Lock();
		mainGame->dField.chains.push_back(mainGame->dField.current_chain);
		mainGame->gMutex.Unlock();
		if (ct > 1)
			mainGame->WaitFrameSignal(20);
		mainGame->dField.last_chain = true;
		return true;
	}
	case MSG_CHAIN_SOLVING: {
		int ct = NetManager::ReadInt8(pbuf);
		if (mainGame->dField.chains.size() > 1) {
			if (mainGame->dField.last_chain)
				mainGame->WaitFrameSignal(30);
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
		int ct = NetManager::ReadInt8(pbuf);
		return true;
	}
	case MSG_CHAIN_END: {
		mainGame->dField.chains.clear();
		return true;
	}
	case MSG_CHAIN_INACTIVATED:
	case MSG_CHAIN_DISABLED: {
		int ct = NetManager::ReadInt8(pbuf);
		mainGame->showcardcode = mainGame->dField.chains[ct - 1].code;
		mainGame->showcarddif = 0;
		mainGame->showcard = 3;
		mainGame->WaitFrameSignal(30);
		mainGame->showcard = 0;
		return true;
	}
	case MSG_CARD_SELECTED:
	case MSG_RANDOM_SELECTED: {
		int player = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			int l = NetManager::ReadInt8(pbuf);
			int s = NetManager::ReadInt8(pbuf);
			int ss = NetManager::ReadInt8(pbuf);
			if ((l & 0x80) > 0)
				pcard = mainGame->dField.GetCard(c, l & 0x7f, s)->overlayed[ss];
			else
				pcard = mainGame->dField.GetCard(c, l, s);
		}
		return true;
	}
	case MSG_BECOME_TARGET: {
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			int l = NetManager::ReadInt8(pbuf);
			int s = NetManager::ReadInt8(pbuf);
			int ss = NetManager::ReadInt8(pbuf);
			pcard = mainGame->dField.GetCard(c, l, s);
			pcard->is_highlighting = true;
			if(pcard->location & LOCATION_ONFIELD) {
				for (int j = 0; j < 3; ++j) {
					mainGame->dField.FadeCard(pcard, 5, 5);
					mainGame->WaitFrameSignal(5);
					mainGame->dField.FadeCard(pcard, 255, 5);
					mainGame->WaitFrameSignal(5);
				}
			} else
				mainGame->WaitFrameSignal(30);
			myswprintf(textBuffer, L"[%ls](%ls,%d)成为对象", mainGame->dataManager.GetName(pcard->code), DataManager::FormatLocation(l), s);
			mainGame->lstLog->addItem(textBuffer);
			mainGame->logParam.push_back(pcard->code);
			pcard->is_highlighting = false;
		}
		return true;
	}
	case MSG_DRAW: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* pcard;
		for (int i = 0; i < count; ++i) {
			int code = NetManager::ReadInt32(pbuf);
			mainGame->gMutex.Lock();
			pcard = mainGame->dField.GetCard(player, LOCATION_DECK, mainGame->dField.deck[player].size() - 1);
			pcard->SetCode(code);
			mainGame->dField.deck[player].erase(mainGame->dField.deck[player].end() - 1);
			mainGame->dField.AddCard(pcard, player, LOCATION_HAND, 0);
			for(int i = 0; i < mainGame->dField.hand[player].size(); ++i)
				mainGame->dField.MoveCard(mainGame->dField.hand[player][i], 10);
			mainGame->gMutex.Unlock();
			mainGame->WaitFrameSignal(5);
		}
		if (player == 0)
			myswprintf(pdInfo->strEvent, L"我方抽了%d张卡", count);
		else myswprintf(pdInfo->strEvent, L"对手抽了%d张卡", count);
		return true;
	}
	case MSG_DAMAGE: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int val = NetManager::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] - val;
		if (final < 0)
			final = 0;
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		if (player == 0)
			myswprintf(pdInfo->strEvent, L"我方受到%d伤害", val);
		else
			myswprintf(pdInfo->strEvent, L"对方受到%d伤害", val);
		mainGame->lpccolor = 0xffff0000;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"-%d", val);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(10);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.Lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_RECOVER: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int val = NetManager::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] + val;
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		if (player == 0)
			myswprintf(pdInfo->strEvent, L"我方回复%dLP", val);
		else
			myswprintf(pdInfo->strEvent, L"对方回复%dLP", val);
		mainGame->lpccolor = 0xff00ff00;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"+%d", val);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(10);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.Lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_EQUIP: {
		int c1 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l1 = NetManager::ReadInt8(pbuf);
		int s1 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l2 = NetManager::ReadInt8(pbuf);
		int s2 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		mainGame->gMutex.Lock();
		pc1->equipTarget = pc2;
		pc2->equipped.insert(pc1);
		if (mainGame->dField.hovered_card == pc1)
			pc2->is_showequip = true;
		else if (mainGame->dField.hovered_card == pc2)
			pc1->is_showequip = true;
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_LPUPDATE: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int val = NetManager::ReadInt32(pbuf);
		mainGame->lpd = (val - mainGame->dInfo.lp[player]) / 10;
		mainGame->lpplayer = player;
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(10);
		mainGame->dInfo.lp[player] = val;
		mainGame->gMutex.Lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_UNEQUIP: {
		int c1 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l1 = NetManager::ReadInt8(pbuf);
		int s1 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c1, l1, s1);
		mainGame->gMutex.Lock();
		if (mainGame->dField.hovered_card == pc)
			pc->equipTarget->is_showequip = false;
		else if (mainGame->dField.hovered_card == pc->equipTarget)
			pc->is_showequip = false;
		pc->equipTarget->equipped.erase(pc);
		pc->equipTarget = 0;
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_CARD_TARGET: {
		int c1 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l1 = NetManager::ReadInt8(pbuf);
		int s1 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l2 = NetManager::ReadInt8(pbuf);
		int s2 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		mainGame->gMutex.Lock();
		pc1->cardTarget.insert(pc2);
		pc2->ownerTarget.insert(pc1);
		if (mainGame->dField.hovered_card == pc1)
			pc2->is_showtarget = true;
		else if (mainGame->dField.hovered_card == pc2)
			pc1->is_showtarget = true;
		mainGame->gMutex.Unlock();
		break;
	}
	case MSG_CANCEL_TARGET: {
		int c1 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l1 = NetManager::ReadInt8(pbuf);
		int s1 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int c2 = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l2 = NetManager::ReadInt8(pbuf);
		int s2 = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		ClientCard* pc1 = mainGame->dField.GetCard(c1, l1, s1);
		ClientCard* pc2 = mainGame->dField.GetCard(c2, l2, s2);
		mainGame->gMutex.Lock();
		pc1->cardTarget.erase(pc2);
		pc2->ownerTarget.erase(pc1);
		if (mainGame->dField.hovered_card == pc1)
			pc2->is_showtarget = false;
		else if (mainGame->dField.hovered_card == pc2)
			pc1->is_showtarget = false;
		mainGame->gMutex.Unlock();
		break;
	}
	case MSG_PAY_LPCOST: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int cost = NetManager::ReadInt32(pbuf);
		int final = mainGame->dInfo.lp[player] - cost;
		if (final < 0)
			final = 0;
		mainGame->lpd = (mainGame->dInfo.lp[player] - final) / 10;
		mainGame->lpccolor = 0xff0000ff;
		mainGame->lpplayer = player;
		myswprintf(textBuffer, L"-%d", cost);
		mainGame->lpcstring = textBuffer;
		mainGame->WaitFrameSignal(30);
		mainGame->lpframe = 10;
		mainGame->WaitFrameSignal(10);
		mainGame->lpcstring = 0;
		mainGame->dInfo.lp[player] = final;
		mainGame->gMutex.Lock();
		myswprintf(mainGame->dInfo.strLP[player], L"%d", mainGame->dInfo.lp[player]);
		mainGame->gMutex.Unlock();
		return true;
	}
	case MSG_ADD_COUNTER: {
		int type = NetManager::ReadInt16(pbuf);
		int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l = NetManager::ReadInt8(pbuf);
		int s = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		if (pc->counters.count(type))
			pc->counters[type] += count;
		else pc->counters[type] = count;
		myswprintf(textBuffer, L"[%ls]放置了%d个[%ls]", mainGame->dataManager.GetName(pc->code), count, mainGame->dataManager.GetCounterName(type));
		pc->is_highlighting = true;
		mainGame->gMutex.Lock();
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_REMOVE_COUNTER: {
		int type = NetManager::ReadInt16(pbuf);
		int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l = NetManager::ReadInt8(pbuf);
		int s = NetManager::ReadInt8(pbuf);
		int count = NetManager::ReadInt8(pbuf);
		ClientCard* pc = mainGame->dField.GetCard(c, l, s);
		pc->counters[type] -= count;
		if (pc->counters[type] <= 0)
			pc->counters.erase(type);
		myswprintf(textBuffer, L"[%ls]移除了%d个[%ls]", mainGame->dataManager.GetName(pc->code), count, mainGame->dataManager.GetCounterName(type));
		pc->is_highlighting = true;
		mainGame->gMutex.Lock();
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(40);
		pc->is_highlighting = false;
		return true;
	}
	case MSG_ATTACK: {
		int ca = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int la = NetManager::ReadInt8(pbuf);
		int sa = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		mainGame->dField.attacker = mainGame->dField.GetCard(ca, la, sa);
		int cd = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int ld = NetManager::ReadInt8(pbuf);
		int sd = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		float sy;
		if (ld != 0) {
			mainGame->dField.attack_target = mainGame->dField.GetCard(cd, ld, sd);
			myswprintf(pdInfo->strEvent, L"[%ls]攻击[%ls]", mainGame->dataManager.GetName(mainGame->dField.attacker->code),
			           mainGame->dataManager.GetName(mainGame->dField.attack_target->code));
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
			myswprintf(pdInfo->strEvent, L"[%ls]直接攻击", mainGame->dataManager.GetName(mainGame->dField.attacker->code));
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
		mainGame->matManager.GenArrow(sy);
		mainGame->attack_sv = 0;
		mainGame->is_attacking = true;
		mainGame->WaitFrameSignal(40);
		mainGame->is_attacking = false;
		return true;
	}
	case MSG_BATTLE: {
		int ca = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int la = NetManager::ReadInt8(pbuf);
		int sa = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int va = NetManager::ReadInt32(pbuf);
		int da = NetManager::ReadInt8(pbuf);
		int cd = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int ld = NetManager::ReadInt8(pbuf);
		int sd = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int vd = NetManager::ReadInt32(pbuf);
		int dd = NetManager::ReadInt8(pbuf);
		return true;
	}
	case MSG_ATTACK_DISABLED: {
		myswprintf(pdInfo->strEvent, L"攻击被无效", mainGame->dataManager.GetName(mainGame->dField.attacker->code));
		return true;
	}
	case MSG_DAMAGE_STEP_START: {
		return true;
	}
	case MSG_DAMAGE_STEP_END: {
		return true;
	}
	case MSG_MISSED_EFFECT: {
		NetManager::ReadInt32(pbuf);
		int code = NetManager::ReadInt32(pbuf);
		myswprintf(textBuffer, L"[%ls]错过时点", mainGame->dataManager.GetName(code));
		mainGame->lstLog->addItem(textBuffer);
		mainGame->logParam.push_back(code);
		return true;
	}
	case MSG_TOSS_COIN: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		textBuffer[0] = 0;
		wcscat(textBuffer, L"投掷硬币结果：");
		for (int i = 0; i < count; ++i) {
			int res = NetManager::ReadInt8(pbuf);
			wcscat(textBuffer, res ? L"[正面]" : L"[反面]");
		}
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_TOSS_DICE: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		textBuffer[0] = 0;
		wcscat(textBuffer, L"投掷骰子结果：");
		wchar_t* p = &textBuffer[7];
		for (int i = 0; i < count; ++i) {
			int res = NetManager::ReadInt8(pbuf);
			*p++ = L'[';
			*p++ = L'0' + res;
			*p++ = L']';
		}
		*p = 0;
		mainGame->stACMessage->setText(textBuffer);
		mainGame->PopupElement(mainGame->wACMessage, 20);
		mainGame->WaitFrameSignal(40);
		return true;
	}
	case MSG_ANNOUNCE_RACE: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		mainGame->dField.announce_count = NetManager::ReadInt8(pbuf);
		int available = NetManager::ReadInt32(pbuf);
		for(int i = 0, filter = 0x1; i < 22; ++i, filter <<= 1) {
			mainGame->chkRace[i]->setChecked(false);
			if(filter & available)
				mainGame->chkRace[i]->setVisible(true);
			else mainGame->chkRace[i]->setVisible(false);
		}
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint));
		else myswprintf(textBuffer, L"请宣言种族");
		mainGame->dInfo.selectHint = 0;
		mainGame->wANRace->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANRace);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_ANNOUNCE_ATTRIB: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		mainGame->dField.announce_count = NetManager::ReadInt8(pbuf);
		int available = NetManager::ReadInt32(pbuf);
		for(int i = 0, filter = 0x1; i < 7; ++i, filter <<= 1) {
			mainGame->chkAttribute[i]->setChecked(false);
			if(filter & available)
				mainGame->chkAttribute[i]->setVisible(true);
			else mainGame->chkAttribute[i]->setVisible(false);
		}
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint));
		else myswprintf(textBuffer, L"请宣言属性");
		mainGame->dInfo.selectHint = 0;
		mainGame->wANAttribute->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANAttribute);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_ANNOUNCE_CARD: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		mainGame->ebANCard->setText(L"");
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint));
		else myswprintf(textBuffer, L"请宣言卡名");
		mainGame->dInfo.selectHint = 0;
		mainGame->wANCard->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANCard);
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_ANNOUNCE_NUMBER: {
		int player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int count = NetManager::ReadInt8(pbuf);
		mainGame->gMutex.Lock();
		mainGame->cbANNumber->clear();
		for (int i = 0; i < count; ++i) {
			int value = NetManager::ReadInt32(pbuf);
			myswprintf(textBuffer, L"%d", value);
			mainGame->cbANNumber->addItem(textBuffer, value);
		}
		mainGame->cbANNumber->setSelected(0);
		if(mainGame->dInfo.selectHint)
			myswprintf(textBuffer, L"%ls", mainGame->dataManager.GetDesc(mainGame->dInfo.selectHint));
		else myswprintf(textBuffer, L"请宣言数字");
		mainGame->dInfo.selectHint = 0;
		mainGame->wANNumber->setText(textBuffer);
		mainGame->PopupElement(mainGame->wANNumber);
		mainGame->gMutex.Unlock();
		mainGame->localAction.Reset();
		mainGame->localAction.Wait();
		mainGame->localResponse.Set();
		return true;
	}
	case MSG_COUNT_TURN: {
		int c = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
		int l = NetManager::ReadInt8(pbuf);
		int s = NetManager::ReadInt8(pbuf);
		NetManager::ReadInt8(pbuf);
		int ct = NetManager::ReadInt16(pbuf);
		ClientCard* pcard = mainGame->dField.GetCard(c, l, s);
		pcard->turnCounter = ct;
		if(ct == 0)
			return true;
		if(pcard->location & LOCATION_ONFIELD)
			pcard->is_selectable = true;
		mainGame->showcardcode = pcard->code;
		mainGame->showcarddif = 0;
		mainGame->showcardp = ct - 1;
		mainGame->showcard = 6;
		mainGame->WaitFrameSignal(30);
		pcard->is_selectable = false;
		mainGame->showcard = 0;
		return true;
	}
	}
	return true;
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
	mainGame->lstServerList->clear();
	mainGame->stModeStatus->setText(L"");
	mainGame->btnReplayStart->setVisible(false);
	mainGame->btnReplayPause->setVisible(true);
	mainGame->btnReplayStep->setVisible(false);
	mainGame->dInfo.engLen = 0;
	mainGame->dInfo.msgLen = 0;
	mainGame->dInfo.is_local_host = false;
	mainGame->btnLanStartServer->setEnabled(true);
	mainGame->btnLanConnect->setEnabled(true);
	mainGame->btnRefreshList->setEnabled(true);
	mainGame->btnLoadReplay->setEnabled(true);
	mainGame->btnDeckEdit->setEnabled(true);
	mainGame->HideElement(mainGame->wModeSelection);
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
		mainGame->ShowElement(mainGame->wModeSelection);
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
			pbuf += count * 12;
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
			player = mainGame->LocalPlayer(NetManager::ReadInt8(pbuf));
			mainGame->WaitFrameSignal(5);
			for (auto cit = mainGame->dField.hand[player].begin(); cit != mainGame->dField.hand[player].end(); ++cit) {
				(*cit)->dPos = irr::core::vector3df((3.9f - (*cit)->curPos.X) / 5, 0, 0);
				(*cit)->dRot = irr::core::vector3df(0, 0, 0);
				(*cit)->is_moving = true;
				(*cit)->is_hovered = false;
				(*cit)->aniFrame = 5;
			}
			mainGame->WaitFrameSignal(20);
			mainGame->ReplayRefresh();
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
			int code = NetManager::ReadInt32(pbuf);
			int pc = NetManager::ReadInt8(pbuf);
			int pl = NetManager::ReadInt8(pbuf);
			int ps = NetManager::ReadInt8(pbuf);
			int pp = NetManager::ReadInt8(pbuf);
			int cc = NetManager::ReadInt8(pbuf);
			int cl = NetManager::ReadInt8(pbuf);
			int cs = NetManager::ReadInt8(pbuf);
			int cp = NetManager::ReadInt8(pbuf);
			SolveMessage(pd, offset, pbuf - offset);
			if(pl != cl || pc != cc)
				mainGame->ReplayRefreshSingle(cc, cl, cs);
			break;
		}
		case MSG_DESTROY: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
			break;
		}
		case MSG_RELEASE: {
			pbuf += 8;
			SolveMessage(pd, offset, pbuf - offset);
			pauseable = false;
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

}
