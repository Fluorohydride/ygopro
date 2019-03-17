#include "replay.h"
#include <algorithm>
#include "lzma/LzmaLib.h"

namespace ygo {

ReplayPacket::ReplayPacket(char* buf, int len) {
	Set(BufferIO::ReadInt8(buf), buf, len);
}
ReplayPacket::ReplayPacket(int msg, char* buf, int len) {
	Set(msg, buf, len);
}
void ReplayPacket::Set(int msg, char* buf, int len) {
	message = msg;
	data.resize(len);
	memcpy(data.data(), buf, data.size());
}

Replay::Replay() {
	is_recording = false;
	is_replaying = false;
	can_read = false;
}
Replay::~Replay() {
}
void Replay::BeginRecord(bool write) {
#ifdef _WIN32
	if(is_recording && is_writing)
		CloseHandle(fp);
	is_writing = write;
	if(is_writing) {
		fp = CreateFileW(L"./replay/_LastReplay.yrpX", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
		if(fp == INVALID_HANDLE_VALUE)
			return;
	}
#else
	if(is_recording && is_writing)
		fclose(fp);
	is_writing = write;
	if(is_writing) {
		fp = fopen("./replay/_LastReplay.yrpX", "wb");
		if(!fp)
			return;
	}
#endif
	is_recording = true;
}
void Replay::WritePacket(ReplayPacket p) {
	WriteInt8(p.message, false);
	WriteInt32(p.data.size(), false);
	WriteData((char*)p.data.data(), p.data.size());
}
void Replay::WriteStream(std::vector<ReplayPacket> stream) {
	for(auto packet : stream)
		WritePacket(packet);
}
void Replay::Write(const void* data, size_t size, bool flush){
	if(!is_writing) return;
#ifdef _WIN32
	DWORD sizea;
	WriteFile(fp, data, size, &sizea, NULL);
#else
	fwrite(data, size, 1, fp);
	if(flush)
		fflush(fp);
#endif

}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
	Write<ReplayHeader>(&header, true);
}
template<typename T>
void Replay::Write(const void * data, bool flush) {
	WriteData(data, sizeof(T), flush);
}
void Replay::WriteData(const void* data, unsigned int length, bool flush) {
	if(!is_recording)
		return;
	const auto vec_size = replay_data.size();
	replay_data.resize(vec_size + length);
	std::memcpy(&replay_data[vec_size], data, length);
	Write(data, length, flush);
}
void Replay::WriteInt32(int32_t data, bool flush) {
	Write<int32_t>(&data, flush);
}
void Replay::WriteInt16(int16_t data, bool flush) {
	Write<int16_t>(&data, flush);
}
void Replay::WriteInt8(int8_t data, bool flush) {
	Write<int8_t>(&data, flush);
}
void Replay::Flush() {
	if(!is_recording)
		return;
	if(!is_writing) return;
#ifdef _WIN32
#else
	fflush(fp);
#endif
}
void Replay::EndRecord(size_t size) {
	if(!is_recording)
		return;
	if(is_writing)
#ifdef _WIN32
	CloseHandle(fp);
#else
	fclose(fp);
#endif
	pheader.datasize = replay_data.size() - sizeof(ReplayHeader);
	pheader.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = size;
	comp_data.resize(replay_data.size() * 2);
	LzmaCompress(comp_data.data(), &comp_size, replay_data.data() + sizeof(ReplayHeader), replay_data.size() - sizeof(ReplayHeader), pheader.props, &propsize, 5, 1 << 24, 3, 0, 2, 32, 1);
	comp_data.resize(comp_size);
	is_recording = false;
}
void Replay::SaveReplay(const std::wstring& name) {
#ifdef _WIN32
	std::ofstream replay_file(L"./replay/" + name + L".yrpX", std::ofstream::binary);
#else
	std::ofstream replay_file("./replay/" + BufferIO::EncodeUTF8s(name) + ".yrpX", std::ofstream::binary);
#endif
	if(!replay_file.is_open())
		return;
	replay_file.write((char*)&pheader, sizeof(pheader));
	replay_file.write((char*)comp_data.data(), comp_size);
	replay_file.close();
}
bool Replay::OpenReplay(const std::wstring& name) {
#ifdef _WIN32
	std::ifstream replay_file(name, std::ifstream::binary);
	if(!replay_file.is_open()) {
		replay_file.open(L"./replay/" + name, std::ifstream::binary);
#else
	std::ifstream replay_file(BufferIO::EncodeUTF8s(name), std::ifstream::binary);
	if(!replay_file.is_open()) {
		replay_file.open("./replay/" + BufferIO::EncodeUTF8s(name), std::ifstream::binary);
#endif
		if(!replay_file.is_open())
			return false;
	}
	std::vector<uint8_t> contents((std::istreambuf_iterator<char>(replay_file)), std::istreambuf_iterator<char>());
	memcpy(&pheader, contents.data(), sizeof(pheader));
	contents.erase(contents.begin(), contents.begin() + sizeof(pheader));
	comp_size = contents.size();
	if(pheader.flag & REPLAY_COMPRESSED) {
		replay_data.resize(0x200000);
		replay_file.close();
		replay_size = pheader.datasize;
		if(LzmaUncompress(replay_data.data(), &replay_size, contents.data(), &comp_size, pheader.props, 5) != SZ_OK)
			return false;
		replay_data.resize(replay_size);
	} else {
		replay_data = contents;
		replay_file.close();
		replay_size = comp_size;
	}
	data_position = 0;
	is_replaying = true;
	can_read = true;
	return true;
}
bool Replay::CheckReplay(const std::wstring& name) {
#ifdef _WIN32
	std::ifstream replay_file(name, std::ifstream::binary);
	if(!replay_file.is_open()) {
		replay_file.open(L"./replay/" + name, std::ifstream::binary);
#else
	std::ifstream replay_file(BufferIO::EncodeUTF8s(name), std::ifstream::binary);
	if(!replay_file.is_open()) {
		replay_file.open("./replay/" + BufferIO::EncodeUTF8s(name), std::ifstream::binary);
#endif
		if(!replay_file.is_open())
			return false;
	}
	ReplayHeader rheader;
	auto count = replay_file.read((char*)&rheader, sizeof(pheader)).gcount();
	replay_file.close();
	return count == 1 && (rheader.id == 0x31707279 || rheader.id == 0x58707279) && rheader.version >= 0x12d0;
}
bool Replay::ReadNextPacket(ReplayPacket* packet) {
	if (!can_read)
		return false;
	unsigned char message = (unsigned char)ReadInt8();
	if(!can_read)
		return false;
	packet->message = message;
	packet->data.resize(ReadInt32());
	ReadData((char*)packet->data.data(), packet->data.size());
	return true;
}
bool Replay::ReadStream(std::vector<ReplayPacket>* stream) {
	stream->clear();
	ReplayPacket p;
	while (ReadNextPacket(&p)) {
		stream->push_back(p);
	}
	return !stream->empty();
}
bool Replay::DeleteReplay(const std::wstring& name) {
	return Utils::Deletefile(L"./replay/" + name + L".ydk");
}
bool Replay::RenameReplay(const std::wstring& oldname, const std::wstring& newname) {
	return Utils::Movefile(L"./replay/" + oldname, L"./replay/" + newname);
}
bool Replay::GetNextResponse(ReplayResponse* res) {
	if(responses_iterator == responses.end())
		return false;
	*res = *responses_iterator;
	responses_iterator++;
	return true;
}
bool Replay::ReadNextResponse(ReplayResponse* res) {
	if(!can_read | !res)
		return false;
	res->length = ReadInt8();
	if(res->length < 1)
		return false;
	res->response.resize(res->length);
	if(!ReadData(res->response.data(), res->length))
		return false;
	return true;
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
bool Replay::ReadData(void* data, unsigned int length) {
	if(!is_replaying || !can_read)
		return false;
	if((replay_data.size() - data_position) < length) {
		can_read = false;
		return false;
	}
	memcpy(data, &replay_data[data_position], length);
	data_position += length;
	return true;
}
template<typename T>
T Replay::Read() {
	T ret = 0;
	if(!ReadData(&ret, sizeof(T)))
		return -1;
	return ret;
}
int32_t Replay::ReadInt32() {
	return Read<int32_t>();
}
int16_t Replay::ReadInt16() {
	return Read<int16_t>();
}
int8_t Replay::ReadInt8() {
	return Read<int8_t>();
}
void Replay::Rewind() {
	data_position = 0;
	responses_iterator = responses.begin();
}

bool Replay::LoadYrp() {
	if (pheader.flag & REPLAY_NEWREPLAY) {
		data_position += (4 + ((pheader.flag & REPLAY_RELAY) ? 240 : (pheader.flag & REPLAY_TAG) ? 160 : 80));
		ReplayPacket p;
		while (ReadNextPacket(&p))
			if (p.message == OLD_REPLAY_MODE) {
				char* prep = (char*)p.data.data();
				memcpy(&pheader, prep, sizeof(ReplayHeader));
				prep += sizeof(ReplayHeader);
				if(pheader.flag & REPLAY_COMPRESSED) {
					comp_size = (size_t)(p.data.size() - sizeof(ReplayHeader));
					replay_size = pheader.datasize;
					if (LzmaUncompress(replay_data.data(), &replay_size, (unsigned char*)prep, &comp_size, pheader.props, 5) != SZ_OK)
						return false;
				} else {
					replay_data.insert(replay_data.begin(), prep, prep + (size_t)(p.data.size() - sizeof(ReplayHeader)));
					replay_size = comp_size;
				}
				replay_data.shrink_to_fit();
				data_position = 0;
				is_replaying = true;
				return true;
			}
	}
	return !(pheader.flag & REPLAY_NEWREPLAY);
}
bool Replay::ParseResponses() {
	responses.clear();
	ReplayResponse r;
	while(ReadNextResponse(&r)) {
		responses.push_back(r);
	}
	responses_iterator = responses.begin();
	return !responses.empty();
}

}
