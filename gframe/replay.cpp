#include "replay.h"
#include <algorithm>
#include <fstream>
#include "lzma/LzmaLib.h"

namespace ygo {
ReplayPacket::ReplayPacket(const CoreUtils::Packet& packet) {
	char* buf = (char*)packet.data.data();
	int msg = BufferIO::Read<uint8_t>(buf);
	Set(msg, buf, (int)(packet.data.size() - sizeof(uint8_t)));
}
ReplayPacket::ReplayPacket(char* buf, int len) {
	int msg = BufferIO::Read<uint8_t>(buf);
	Set(msg, buf, len);
}
ReplayPacket::ReplayPacket(int msg, char* buf, int len) {
	Set(msg, buf, len);
}
void ReplayPacket::Set(int msg, char* buf, int len) {
	message = msg;
	data.resize(len);
	if(len)
		memcpy(data.data(), buf, data.size());
}

Replay::Replay() {
	yrp = nullptr;
	is_recording = false;
	is_replaying = false;
	can_read = false;
	turn_count = 0;
}
Replay::~Replay() {
}
void Replay::BeginRecord(bool write, path_string name) {
	Reset();
	if(fp.is_open())
		fp.close();
	is_recording = false;
	if(write) {
		fp.open(name, std::ofstream::binary);
		if(!fp.is_open()) {
			return;
		}
	}
	is_recording = true;
}
void Replay::WritePacket(const ReplayPacket& p) {
	Write<int8_t>(p.message, false);
	Write<int32_t>(p.data.size(), false);
	WriteData((char*)p.data.data(), p.data.size());
}
void Replay::WriteStream(const ReplayStream& stream) {
	for(auto& packet : stream)
		WritePacket(packet);
}
void Replay::WritetoFile(const void* data, size_t size, bool flush){
	if(!fp.is_open()) return;
	fp.write((char*)data, size);
	if(flush)
		fp.flush();
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
	Write<ReplayHeader>(header, true);
}
void Replay::WriteData(const void* data, unsigned int length, bool flush) {
	if(!is_recording)
		return;
	const auto vec_size = replay_data.size();
	replay_data.resize(vec_size + length);
	if(length)
		std::memcpy(&replay_data[vec_size], data, length);
	WritetoFile(data, length, flush);
}
void Replay::Flush() {
	if(!is_recording)
		return;
	if(!fp.is_open()) return;
	fp.flush();
}
void Replay::EndRecord(size_t size) {
	if(!is_recording)
		return;
	if(fp.is_open())
		fp.close();
	pheader.datasize = replay_data.size() - sizeof(ReplayHeader);
	pheader.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = size;
	comp_data.resize(replay_data.size() * 2);
	LzmaCompress(comp_data.data(), &comp_size, replay_data.data() + sizeof(ReplayHeader), replay_data.size() - sizeof(ReplayHeader), pheader.props, &propsize, 5, 1 << 24, 3, 0, 2, 32, 1);
	comp_data.resize(comp_size);
	is_recording = false;
}
void Replay::SaveReplay(const path_string& name) {
	std::ofstream replay_file(fmt::format(EPRO_TEXT("./replay/{}.yrpX"), name.c_str()), std::ofstream::binary);
	if(!replay_file.is_open())
		return;
	replay_file.write((char*)&pheader, sizeof(pheader));
	replay_file.write((char*)comp_data.data(), comp_size);
	replay_file.close();
}
bool Replay::OpenReplayFromBuffer(std::vector<uint8_t> contents) {
	Reset();
	memcpy(&pheader, contents.data(), sizeof(pheader));
	if(pheader.id != REPLAY_YRP1 && pheader.id != REPLAY_YRPX) {
		Reset();
		return false;
	}
	contents.erase(contents.begin(), contents.begin() + sizeof(pheader));
	comp_size = contents.size();
	if(pheader.flag & REPLAY_COMPRESSED) {
		replay_size = pheader.datasize;
		replay_data.resize(replay_size);
		if(LzmaUncompress(replay_data.data(), &replay_size, contents.data(), &comp_size, pheader.props, 5) != SZ_OK)
			return false;
	} else {
		replay_data = contents;
		replay_size = comp_size;
	}
	data_position = 0;
	is_replaying = true;
	can_read = true;
	ParseNames();
	ParseParams();
	if(pheader.id == REPLAY_YRP1) {
		ParseDecks();
		ParseResponses();
	} else {
		ParseStream();
	}
	return true;
}
bool Replay::IsExportable() {
	auto& deck = (yrp != nullptr) ? yrp->GetPlayerDecks() : decks;
	if(players.empty() || deck.empty() || players.size() > deck.size())
		return false;
	return true;
}
bool Replay::OpenReplay(const path_string& name) {
	if(replay_name == name) {
		Rewind();
		return true;
	}
	Reset();
	std::ifstream replay_file(name, std::ifstream::binary);
	if(!replay_file.is_open()) {
		replay_file.open(EPRO_TEXT("./replay/") + name, std::ifstream::binary);
		if(!replay_file.is_open()) {
			replay_name.clear();
			return false;
		}
	}
	std::vector<uint8_t> contents((std::istreambuf_iterator<char>(replay_file)), std::istreambuf_iterator<char>());
	replay_file.close();
	if (OpenReplayFromBuffer(contents)){
		replay_name = name;
		return true;
	}
	replay_name.clear();
	return false;
}
bool Replay::DeleteReplay(const path_string& name) {
	return Utils::FileDelete(name);
}
bool Replay::RenameReplay(const path_string& oldname, const path_string& newname) {
	return Utils::FileMove(oldname, newname);
}
bool Replay::GetNextResponse(ReplayResponse* res) {
	if(responses_iterator == responses.end())
		return false;
	*res = *responses_iterator;
	responses_iterator++;
	return true;
}
const std::vector<std::wstring>& Replay::GetPlayerNames() {
	return players;
}
const ReplayDeckList& Replay::GetPlayerDecks() {
	if(pheader.id == REPLAY_YRPX && yrp)
		return yrp->decks;
	return decks;
}
const std::vector<int>& Replay::GetRuleCards() {
	return replay_custom_rule_cards;
}
bool Replay::ReadNextResponse(ReplayResponse* res) {
	if(!can_read || !res)
		return false;
	res->length = Read<int8_t>();
	if(res->length < 1)
		return false;
	return ReadData(res->response, res->length);
}
void Replay::ParseNames() {
	players.clear();
	if(pheader.flag & REPLAY_SINGLE_MODE) {
		wchar_t namebuf[20];
		ReadName(namebuf);
		players.push_back(namebuf);
		ReadName(namebuf);
		players.push_back(namebuf);
		home_count = 1;
		opposing_count = 1;
		return;
	}
	auto f = [this](size_t& count) {
		if(pheader.flag & REPLAY_NEWREPLAY)
			count = Read<uint32_t>();
		else if(pheader.flag & REPLAY_TAG)
			count = 2;
		else
			count = 1;
		for(int i = 0; i < count; i++) {
			wchar_t namebuf[20];
			ReadName(namebuf);
			players.push_back(namebuf);
		}
	};
	f(home_count);
	f(opposing_count);
}
void Replay::ParseParams() {
	params = { 0 };
	if(pheader.id == REPLAY_YRP1) {
		params.start_lp = Read<int32_t>();
		params.start_hand = Read<int32_t>();
		params.draw_count = Read<int32_t>();
	}
	params.duel_flags = Read<int32_t>();
	if(pheader.flag & REPLAY_SINGLE_MODE && pheader.id == REPLAY_YRP1) {
		size_t slen = Read<uint16_t>();
		scriptname.resize(slen);
		ReadData(&scriptname[0], slen);
	}
}
void Replay::ParseDecks() {
	decks.clear();
	if(pheader.id != REPLAY_YRP1 || (pheader.flag & REPLAY_SINGLE_MODE && !(pheader.flag & REPLAY_HAND_TEST)))
		return;
	for(int i = 0; i < home_count + opposing_count; i++) {
		ReplayDeck tmp;
		int main = Read<int32_t>();
		for(int i = 0; i < main; ++i)
			tmp.main_deck.push_back(Read<int32_t>());
		std::vector<int> extra_deck;
		int extra = Read<int32_t>();
		for(int i = 0; i < extra; ++i)
			tmp.extra_deck.push_back(Read<int32_t>());
		decks.push_back(tmp);
	}
	replay_custom_rule_cards.clear();
	if(pheader.flag & REPLAY_NEWREPLAY && !(pheader.flag & REPLAY_HAND_TEST)) {
		int rules = Read<int32_t>();
		for(int i = 0; i < rules; ++i)
			replay_custom_rule_cards.push_back(Read<int32_t>());
	}
}
bool Replay::ReadNextPacket(ReplayPacket* packet) {
	if(!can_read)
		return false;
	unsigned char message = (unsigned char)Read<int8_t>();
	if(!can_read)
		return false;
	packet->message = message;
	int len = Read<int32_t>();
	if(!can_read || len == -1)
		return false;
	return ReadData(packet->data, len);
}
void Replay::ParseStream() {
	packets_stream.clear();
	if(pheader.id != REPLAY_YRPX)
		return;
	ReplayPacket p;
	while(ReadNextPacket(&p)) {
		if(p.message == MSG_AI_NAME) {
			char* pbuf = (char*)p.data.data();
			int len = BufferIO::Read<uint16_t>(pbuf);
			std::string namebuf;
			namebuf.resize(len);
			memcpy(&namebuf[0], pbuf, len + 1);
			players[1] = BufferIO::DecodeUTF8s(namebuf);
			continue;
		}
		if(p.message == MSG_NEW_TURN) {
			turn_count++;
		}
		if(p.message == OLD_REPLAY_MODE) {
			if(!yrp) {
				yrp = std::unique_ptr<Replay>(new Replay());
				yrp->OpenReplayFromBuffer(p.data);
			}
			continue;
		}
		packets_stream.push_back(p);
	}
}
bool Replay::ReadName(wchar_t* data) {
	if(!is_replaying || !can_read)
		return false;
	unsigned short buffer[20];
	if(!ReadData(buffer, 40))
		return false;
	BufferIO::CopyWStr(buffer, data, 20);
	return true;
}
void Replay::Reset() {
	yrp = nullptr;
	scriptname.clear();
	responses.clear();
	responses.shrink_to_fit();
	players.clear();
	decks.clear();
	decks.shrink_to_fit();
	params = { 0 };
	packets_stream.clear();
	packets_stream.shrink_to_fit();
	data_position = 0;
	replay_data.clear();
	replay_data.shrink_to_fit();
	comp_data.clear();
	comp_data.shrink_to_fit();
	turn_count = 0;
}
int Replay::GetPlayersCount(int side) {
	if(side == 0)
		return home_count;
	return opposing_count;
}
int Replay::GetTurnsCount() {
	return turn_count;
}
path_string Replay::GetReplayName() {
	return replay_name;
}
bool Replay::ReadData(void* data, unsigned int length) {
	if(!is_replaying || !can_read)
		return false;
	if((replay_data.size() - data_position) < length) {
		can_read = false;
		return false;
	}
	if(length)
		memcpy(data, &replay_data[data_position], length);
	data_position += length;
	return true;
}
bool Replay::ReadData(std::vector<uint8_t>& data, unsigned int length) {
	if(!is_replaying || !can_read)
		return false;
	if((replay_data.size() - data_position) < length) {
		can_read = false;
		return false;
	}
	if(length) {
		data.resize(length);
		memcpy(data.data(), &replay_data[data_position], length);
		data_position += length;
	}
	return true;
}
template<typename T>
T Replay::Read() {
	T ret = 0;
	if(!ReadData(&ret, sizeof(T)))
		return -1;
	return ret;
}
void Replay::Rewind() {
	data_position = 0;
	responses_iterator = responses.begin();
	if(yrp)
		yrp->Rewind();
}
bool Replay::ParseResponses() {
	responses.clear();
	if(pheader.id != REPLAY_YRP1)
		return false;
	ReplayResponse r;
	while(ReadNextResponse(&r)) {
		responses.push_back(r);
	}
	responses_iterator = responses.begin();
	return !responses.empty();
}

}
