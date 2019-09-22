#include "single_mode.h"
#include "duelclient.h"
#include "game.h"
#include "core_utils.h"
#include <random>

namespace ygo {

OCG_Duel SingleMode::pduel = 0;
bool SingleMode::is_closing = false;
bool SingleMode::is_continuing = false;
Replay SingleMode::last_replay;
Replay SingleMode::new_replay;
ReplayStream SingleMode::replay_stream;
Signal SingleMode::singleSignal;

bool SingleMode::StartPlay() {
	if(mainGame->dInfo.isSingleMode)
		return false;
	std::thread(SinglePlayThread).detach();
	return true;
}
void SingleMode::StopPlay(bool is_exiting) {
	is_closing = is_exiting;
	is_continuing = false;
	mainGame->actionSignal.Set();
	if(is_closing)
		singleSignal.SetNoWait(true);
	else
		singleSignal.Set();
}
void SingleMode::SetResponse(unsigned char* resp, unsigned int len) {
	if(!pduel)
		return;
	last_replay.Write<int8_t>(len);
	last_replay.WriteData(resp, len);
	OCG_DuelSetResponse(pduel, resp, len);
}
int SingleMode::SinglePlayThread() {
	mainGame->dInfo.isSingleMode = true;
	const int start_lp = 8000;
	const int start_hand = 5;
	const int draw_count = 1;
	const int opt = 0;
	time_t seed = time(0);
	DuelClient::rnd.seed(seed);
	OCG_Player team = { start_lp, start_hand, draw_count };
	pduel = mainGame->SetupDuel({ DuelClient::rnd(), opt, team, team });
	mainGame->dInfo.compat_mode = false;
	mainGame->dInfo.lp[0] = start_lp;
	mainGame->dInfo.lp[1] = start_lp;
	mainGame->dInfo.startlp = start_lp;
	mainGame->dInfo.strLP[0] = fmt::to_wstring(mainGame->dInfo.lp[0]);
	mainGame->dInfo.strLP[1] = fmt::to_wstring(mainGame->dInfo.lp[1]);
	mainGame->dInfo.hostname.push_back(mainGame->ebNickName->getText());
	mainGame->dInfo.clientname.push_back(L"");
	mainGame->dInfo.player_type = 0;
	mainGame->dInfo.turn = 0;
	bool loaded = true;
	std::string script_name = "";
	if(open_file) {
		open_file = false;
		script_name = Utils::ToUTF8IfNeeded(open_file_name);
		if(!mainGame->LoadScript(pduel, script_name)) {
			script_name = Utils::ToUTF8IfNeeded(TEXT("./puzzles/") + open_file_name);
			if(!mainGame->LoadScript(pduel, script_name))
				loaded = false;
		}
	} else {
		script_name = BufferIO::EncodeUTF8s(mainGame->lstSinglePlayList->getListItem(mainGame->lstSinglePlayList->getSelected(), true));
		if(!mainGame->LoadScript(pduel, script_name))
			loaded = false;
	}
	if(!loaded) {
		OCG_DestroyDuel(pduel);
		mainGame->dInfo.isSingleMode = false;
		return 0;
	}
	ReplayHeader rh;
	rh.id = REPLAY_YRP1;
	rh.version = PRO_VERSION;
	rh.flag = REPLAY_SINGLE_MODE + REPLAY_LUA64;
	rh.seed = seed;
	mainGame->gMutex.lock();
	mainGame->HideElement(mainGame->wSinglePlay);
	mainGame->ClearCardInfo();
	mainGame->mTopMenu->setVisible(false);
	mainGame->wCardImg->setVisible(true);
	mainGame->wInfos->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(dataManager.GetSysString(1210).c_str());
	mainGame->wPhase->setVisible(true);
	mainGame->dField.Clear();
	mainGame->dInfo.isFirst = true;
	mainGame->dInfo.isInDuel = true;
	mainGame->dInfo.isStarted = true;
	mainGame->dInfo.isCatchingUp = false;
	mainGame->SetMesageWindow();
	mainGame->device->setEventReceiver(&mainGame->dField);
	mainGame->gMutex.unlock();
	std::vector<uint8> duelBuffer;
	is_closing = false;
	is_continuing = true;
	last_replay.BeginRecord(false);
	last_replay.WriteHeader(rh);
	//records the replay with the new system
	new_replay.BeginRecord();
	rh.id = REPLAY_YRPX;
	rh.flag |= REPLAY_NEWREPLAY;
	new_replay.WriteHeader(rh);
	replay_stream.clear();
	unsigned short buffer[20];
	BufferIO::CopyWStr(mainGame->dInfo.hostname[0].c_str(), buffer, 20);
	last_replay.WriteData(buffer, 40, false);
	new_replay.WriteData(buffer, 40, false);
	BufferIO::CopyWStr(mainGame->dInfo.clientname[0].c_str(), buffer, 20);
	last_replay.WriteData(buffer, 40, false);
	new_replay.WriteData(buffer, 40, false);
	last_replay.Write<int32_t>(start_lp, false);
	last_replay.Write<int32_t>(start_hand, false);
	last_replay.Write<int32_t>(draw_count, false);
	last_replay.Write<int32_t>(opt, false);
	last_replay.Write<int16_t>(script_name.size(), false);
	last_replay.WriteData(script_name.c_str(), script_name.size(), false);
	last_replay.Flush();
	new_replay.Write<int32_t>((mainGame->GetMasterRule(opt, 0)) | (opt & DUEL_SPEED) << 8);
	int engFlag = 0;
	auto msg = CoreUtils::ParseMessages(pduel);
	{
		for(auto& message : msg.packets) {
			is_continuing = SinglePlayAnalyze(message) && is_continuing;
		}
		if(!is_continuing)
			goto end;
	}
	OCG_StartDuel(pduel);
	do {
		engFlag = OCG_DuelProcess(pduel);
		msg = CoreUtils::ParseMessages(pduel);
		for(auto& message : msg.packets) {
			is_continuing = SinglePlayAnalyze(message) && is_continuing;
		}
	} while(is_continuing && engFlag && mainGame->dInfo.curMsg != MSG_WIN);
	end :
	OCG_DestroyDuel(pduel);
	last_replay.EndRecord(0x1000);
	std::vector<unsigned char> oldreplay;
	oldreplay.insert(oldreplay.end(), (unsigned char*)&last_replay.pheader, ((unsigned char*)&last_replay.pheader) + sizeof(ReplayHeader));
	oldreplay.insert(oldreplay.end(), last_replay.comp_data.begin(), last_replay.comp_data.end());
	new_replay.WritePacket(ReplayPacket(OLD_REPLAY_MODE, (char*)oldreplay.data(), oldreplay.size()));
	new_replay.EndRecord();
	if(is_closing)
		return 0;
	time_t nowtime = time(NULL);
	tm* localedtime = localtime(&nowtime);
	wchar_t timetext[40];
	wcsftime(timetext, 40, L"%Y-%m-%d %H-%M-%S", localedtime);
	mainGame->gMutex.lock();
	mainGame->ebRSName->setText(timetext);
	mainGame->wReplaySave->setText(dataManager.GetSysString(1340).c_str());
	mainGame->PopupElement(mainGame->wReplaySave);
	mainGame->gMutex.unlock();
	mainGame->replaySignal.Reset();
	mainGame->replaySignal.Wait();
	if(mainGame->saveReplay)
		new_replay.SaveReplay(Utils::ParseFilename(mainGame->ebRSName->getText()));
	new_replay.Reset();
	last_replay.Reset();
	mainGame->gMutex.lock();
	mainGame->dField.Clear();
	mainGame->gMutex.unlock();
	if(!is_closing) {
		mainGame->gMutex.lock();
		mainGame->dInfo.isInDuel = false;
		mainGame->dInfo.isStarted = false;
		mainGame->dInfo.isSingleMode = false;
		mainGame->gMutex.unlock();
		mainGame->closeDoneSignal.Reset();
		mainGame->closeSignal.lock();
		mainGame->closeDoneSignal.Wait();
		mainGame->closeSignal.unlock();
		mainGame->gMutex.lock();
		mainGame->ShowElement(mainGame->wSinglePlay);
		mainGame->stTip->setVisible(false);
		mainGame->SetMesageWindow();
		mainGame->device->setEventReceiver(&mainGame->menuHandler);
		mainGame->gMutex.unlock();
		if(exit_on_return)
			mainGame->device->closeDevice();
	}
	return 0;
}

#define ANALYZE DuelClient::ClientAnalyze((char*)packet.data.data(), packet.data.size())
#define DATA (char*)(packet.data.data() + sizeof(uint8_t))

bool SingleMode::SinglePlayAnalyze(CoreUtils::Packet packet) {
	int player, count;
	replay_stream.clear();
	if(is_closing || !is_continuing)
		return false;
	mainGame->dInfo.curMsg = packet.message;
	bool record = CoreUtils::MessageBeRecorded(packet.message);
	bool record_last = false;
	switch(mainGame->dInfo.curMsg) {
		case MSG_RETRY:	{
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(L"Error occurs.");
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			return false;
		}
		case MSG_HINT: {
			char* pbuf = DATA;
			int type = BufferIO::Read<uint8_t>(pbuf);
			int player = BufferIO::Read<uint8_t>(pbuf);
			/*int data = */BufferIO::Read<int64_t>(pbuf);
			if(player == 0)
				ANALYZE;
			if(type > 0 && type < 6 && type != 4)
				record = false;
			break;
		}
		case MSG_AI_NAME: {
			char* pbuf = DATA;
			int len = BufferIO::Read<uint16_t>(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			std::string namebuf;
			namebuf.resize(len);
			memcpy(&namebuf[0], begin, len + 1);
			mainGame->dInfo.clientname[0] = BufferIO::DecodeUTF8s(namebuf);
			break;
		}
		case MSG_SHOW_HINT: {
			char msgbuf[1024];
			wchar_t msg[1024];
			char* pbuf = DATA;
			int len = BufferIO::Read<uint16_t>(pbuf);
			char* begin = pbuf;
			pbuf += len + 1;
			memcpy(msgbuf, begin, len + 1);
			BufferIO::DecodeUTF8(msgbuf, msg);
			mainGame->gMutex.lock();
			mainGame->stMessage->setText(msg);
			mainGame->PopupElement(mainGame->wMessage);
			mainGame->gMutex.unlock();
			mainGame->actionSignal.Reset();
			mainGame->actionSignal.Wait();
			break;
		}
		case MSG_SELECT_BATTLECMD:
		case MSG_SELECT_IDLECMD: {
			record_last = true;
			SinglePlayRefresh();
			if(!ANALYZE) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			break;
		}
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
			if(!ANALYZE) {
				singleSignal.Reset();
				singleSignal.Wait();
			}
			break;
		}
		default: {
			ANALYZE;
			break;
		}
	}
	char* pbuf = DATA;
	switch(mainGame->dInfo.curMsg) {
		case MSG_SHUFFLE_DECK: {
			player = BufferIO::Read<uint8_t>(pbuf);
			SinglePlayRefresh(player, LOCATION_DECK, 0x181fff);
			break;
		}
		case MSG_SWAP_GRAVE_DECK: {
			player = BufferIO::Read<uint8_t>(pbuf);
			SinglePlayRefresh(player, LOCATION_GRAVE, 0x181fff);
			break;
		}
		case MSG_REVERSE_DECK: {
			SinglePlayRefresh(0, LOCATION_DECK, 0x181fff);
			SinglePlayRefresh(1, LOCATION_DECK, 0x181fff);
			break;
		}
		case MSG_MOVE: {
			pbuf += 4;
			auto previous = CoreUtils::ReadLocInfo(pbuf);
			auto current = CoreUtils::ReadLocInfo(pbuf);
			if(previous.location && !(current.location & 0x80) && (previous.location != current.location || previous.controler != current.controler))
				SinglePlayRefreshSingle(current.controler, current.location, current.sequence);
			break;
		}
		case MSG_TAG_SWAP: {
			player = BufferIO::Read<uint8_t>(pbuf);
			SinglePlayRefresh(player, LOCATION_DECK, 0x181fff);
			SinglePlayRefresh(player, LOCATION_EXTRA, 0x181fff);
			break;
		}
		case MSG_NEW_PHASE:
		case MSG_SUMMONED:
		case MSG_SPSUMMONED:
		case MSG_FLIPSUMMONED:
		case MSG_CHAINED:
		case MSG_CHAIN_SOLVED:
		case MSG_CHAIN_END:
		case MSG_DAMAGE_STEP_START:
		case MSG_DAMAGE_STEP_END: {
			SinglePlayRefresh();
			break;
		}
		case MSG_RELOAD_FIELD: {
			SinglePlayReload();
			mainGame->gMutex.lock();
			mainGame->dField.RefreshAllCards();
			mainGame->gMutex.unlock();
			break;
		}
	}
	if(record) {
		ReplayPacket replay_packet = { (char*)packet.data.data(), (int)(packet.data.size() - sizeof(uint8_t)) };
		if(!record_last) {
			new_replay.WritePacket(replay_packet);
			new_replay.WriteStream(replay_stream);
		} else {
			new_replay.WriteStream(replay_stream);
			new_replay.WritePacket(replay_packet);
		}
		new_replay.Flush();
	}
	return is_continuing;
}
void SingleMode::SinglePlayRefresh(int player, int location, int flag) {
	std::vector<uint8_t> buffer;
	uint32 len = 0;
	auto buff = OCG_DuelQueryLocation(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32_t)location });
	if(len == 0)
		return;
	buffer.resize(buffer.size() + len);
	memcpy(buffer.data(), buff, len);
	mainGame->gMutex.lock();
	mainGame->dField.UpdateFieldCard(mainGame->LocalPlayer(player), location, (char*)buffer.data());
	mainGame->gMutex.unlock();
	buffer.insert(buffer.begin(), location);
	buffer.insert(buffer.begin(), player);
	replay_stream.push_back(ReplayPacket(MSG_UPDATE_DATA, (char*)buffer.data(), buffer.size()));
}
void SingleMode::SinglePlayRefreshSingle(int player, int location, int sequence, int flag) {
	std::vector<uint8_t> buffer;
	uint32 len = 0;
	auto buff = OCG_DuelQuery(pduel, &len, { (uint32_t)flag, (uint8_t)player, (uint32_t)location, (uint32_t)sequence });
	if(buff == nullptr)
		return;
	buffer.resize(buffer.size() + len);
	memcpy(buffer.data(), buff, len);
	mainGame->gMutex.lock();
	mainGame->dField.UpdateCard(mainGame->LocalPlayer(player), location, sequence, (char*)buffer.data());
	mainGame->gMutex.unlock();
	buffer.insert(buffer.begin(), sequence);
	buffer.insert(buffer.begin(), location);
	buffer.insert(buffer.begin(), player);
	ReplayPacket p(MSG_UPDATE_CARD, (char*)buffer.data(), buffer.size());
	replay_stream.push_back(p);
}
void SingleMode::SinglePlayRefresh(int flag) {
	for(int p = 0; p < 2; p++)
		for(int loc = LOCATION_HAND; loc != LOCATION_GRAVE; loc *= 2)
			SinglePlayRefresh(p, loc, flag);
}
void SingleMode::SinglePlayReload() {
	for(int p = 0; p < 2; p++)
		for(int loc = LOCATION_DECK; loc != LOCATION_OVERLAY; loc *= 2)
			SinglePlayRefresh(p, loc, 0xffdfff);
}

}
