#include "replay.h"
#include <algorithm>
#include "lzma/LzmaLib.h"

namespace ygo {

ReplayPacket::ReplayPacket(char * buf, int len) {
	message = BufferIO::ReadInt8(buf);
	length = len;
	memcpy(data, buf, length);
}
ReplayPacket::ReplayPacket(int msg, char * buf, int len) {
	message = msg;
	length = len;
	memcpy(data, buf, length);
}
void ReplayPacket::Set(int msg, char * buf, int len) {
	message = msg;
	length = len;
	memcpy(data, buf, length);
}

Replay::Replay() {
	is_recording = false;
	is_replaying = false;
	replay_data = new unsigned char[0x20000];
	comp_data = new unsigned char[0x2000];
}
Replay::~Replay() {
	delete[] replay_data;
	delete[] comp_data;
}
void Replay::BeginRecord(bool write) {
#ifdef _WIN32
	if(is_recording && is_writing)
		CloseHandle(recording_fp);
	is_writing = write;
	if(is_writing) {
		recording_fp = CreateFileW(L"./replay/_LastReplay.yrpX", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
		if(recording_fp == INVALID_HANDLE_VALUE)
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
	pdata = replay_data;
	is_recording = true;
}
void Replay::WritePacket(ReplayPacket p) {
	WriteInt8(p.message, false);
	WriteInt32(p.length, false);
	WriteData((char*)p.data, p.length);
}
void Replay::WriteStream(std::vector<ReplayPacket> stream) {
	for(auto packet : stream)
		WritePacket(packet);
}
void Replay::Write(const void* data, size_t size, bool flush){
	if(!is_writing) return;
#ifdef _WIN32
	DWORD sizea;
	WriteFile(recording_fp, data, size, &sizea, NULL);
#else
	fwrite(data, size, 1, fp);
	if(flush)
		fflush(fp);
#endif

}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
	Write(&header, sizeof(header), true);
}
void Replay::WriteData(const void* data, unsigned int length, bool flush) {
	if(!is_recording)
		return;
	memcpy(pdata, data, length);
	pdata += length;
	Write(data, length, flush);
}
void Replay::WriteInt32(int data, bool flush) {
	if(!is_recording)
		return;
	*((int*)(pdata)) = data;
	pdata += 4;
	Write(&data, sizeof(int), flush);
}
void Replay::WriteInt16(short data, bool flush) {
	if(!is_recording)
		return;
	*((short*)(pdata)) = data;
	pdata += 2;
	Write(&data, sizeof(short), flush);
}
void Replay::WriteInt8(char data, bool flush) {
	if(!is_recording)
		return;
	*pdata = data;
	pdata++;
	Write(&data, sizeof(char), flush);
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
	CloseHandle(recording_fp);
#else
	fclose(fp);
#endif
	pheader.datasize = pdata - replay_data;
	pheader.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = size;
	LzmaCompress(comp_data, &comp_size, replay_data, pdata - replay_data, pheader.props, &propsize, 5, 1 << 24, 3, 0, 2, 32, 1);
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
	replay_file.write((char*)comp_data, comp_size);
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
	if(replay_file.read((char*)&pheader, sizeof(pheader)).gcount() < 1) {
		replay_file.close();
		return false;
	}
	if(pheader.flag & REPLAY_COMPRESSED) {
		comp_size = replay_file.read((char*)comp_data, 0x20000).gcount();
		replay_file.close();
		replay_size = pheader.datasize;
		if(LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.props, 5) != SZ_OK)
			return false;
	} else {
		comp_size = replay_file.read((char*)replay_data, 0x20000).gcount();
		replay_file.close();
		replay_size = comp_size;
	}
	pdata = replay_data;
	is_replaying = true;
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
	if (pdata - replay_data >= (int)replay_size)
		return false;
	packet->message = *pdata++;
	packet->length = ReadInt32();
	ReadData((char*)&packet->data, packet->length);
	return true;
}
bool Replay::ReadStream(std::vector<ReplayPacket>* stream) {
	stream->clear();
	ReplayPacket p;
	while (ReadNextPacket(&p)) {
		stream->push_back(p);
	}
	return !!stream->size();
}
bool Replay::DeleteReplay(const std::wstring& name) {
	return Utils::Deletefile(L"./replay/" + name + L".ydk");
}
bool Replay::RenameReplay(const std::wstring& oldname, const std::wstring& newname) {
	return Utils::Movefile(L"./replay/" + oldname, L"./replay/" + newname);
}
bool Replay::ReadNextResponse(unsigned char resp[64]) {
	if(pdata - replay_data >= (int)replay_size)
		return false;
	int len = *pdata++;
	if(len > 64)
		return false;
	memcpy(resp, pdata, len);
	pdata += len;
	return true;
}
void Replay::ReadName(wchar_t* data) {
	if(!is_replaying)
		return;
	unsigned short buffer[20];
	ReadData(buffer, 40);
	BufferIO::CopyWStr(buffer, data, 20);
}
void Replay::ReadData(void* data, unsigned int length) {
	if(!is_replaying)
		return;
	memcpy(data, pdata, length);
	pdata += length;
}
int Replay::ReadInt32() {
	if(!is_replaying)
		return -1;
	int ret = *((int*)pdata);
	pdata += 4;
	return ret;
}
short Replay::ReadInt16() {
	if(!is_replaying)
		return -1;
	short ret = *((short*)pdata);
	pdata += 2;
	return ret;
}
char Replay::ReadInt8() {
	if(!is_replaying)
		return -1;
	return *pdata++;
}
void Replay::Rewind() {
	pdata = replay_data;
}

bool Replay::LoadYrp() {
	if (pheader.flag & REPLAY_NEWREPLAY) {
		pdata += (4 + ((pheader.flag & REPLAY_RELAY) ? 240 : (pheader.flag & REPLAY_TAG) ? 160 : 80));
		ReplayPacket p;
		while (ReadNextPacket(&p))
			if (p.message == OLD_REPLAY_MODE) {
				char* prep = (char*)p.data;
				memcpy(&pheader, prep, sizeof(ReplayHeader));
				prep += sizeof(ReplayHeader);
				if(pheader.flag & REPLAY_COMPRESSED) {
					comp_size = (size_t)(p.length - sizeof(ReplayHeader));
					replay_size = pheader.datasize;
					if (LzmaUncompress(replay_data, &replay_size, (unsigned char*)prep, &comp_size, pheader.props, 5) != SZ_OK)
						return false;
				} else {
					replay_data = comp_data;
					replay_size = comp_size;
				}
				pdata = replay_data;
				is_replaying = true;
				return true;
			}
	}
	return !(pheader.flag & REPLAY_NEWREPLAY);
}

}
