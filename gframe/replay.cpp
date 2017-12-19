#include "replay.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"
#include "../ocgcore/field.h"
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
	if(stream.size())
		for(auto it = stream.begin(); it != stream.end(); it++)
			WritePacket((*it));
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
	if(!is_writing) return;
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &header, sizeof(header), &size, NULL);
#else
	fwrite(&header, sizeof(header), 1, fp);
	fflush(fp);
#endif
}
void Replay::WriteData(const void* data, unsigned int length, bool flush) {
	if(!is_recording)
		return;
	memcpy(pdata, data, length);
	pdata += length;
	if(!is_writing) return;
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, data, length, &size, NULL);
#else
	fwrite(data, length, 1, fp);
	if(flush)
		fflush(fp);
#endif
}
void Replay::WriteInt32(int data, bool flush) {
	if(!is_recording)
		return;
	*((int*)(pdata)) = data;
	pdata += 4;
	if(!is_writing) return;
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &data, sizeof(int), &size, NULL);
#else
	fwrite(&data, sizeof(int), 1, fp);
	if(flush)
		fflush(fp);
#endif
}
void Replay::WriteInt16(short data, bool flush) {
	if(!is_recording)
		return;
	*((short*)(pdata)) = data;
	pdata += 2;
	if(!is_writing) return;
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &data, sizeof(short), &size, NULL);
#else
	fwrite(&data, sizeof(short), 1, fp);
	if(flush)
		fflush(fp);
#endif
}
void Replay::WriteInt8(char data, bool flush) {
	if(!is_recording)
		return;
	*pdata = data;
	pdata++;
	if(!is_writing) return;
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &data, sizeof(char), &size, NULL);
#else
	fwrite(&data, sizeof(char), 1, fp);
	if(flush)
		fflush(fp);
#endif
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
void Replay::SaveReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls.yrpX", name);
#ifdef WIN32
	fp = _wfopen(fname, L"wb");
#else
	char fname2[256];
	BufferIO::EncodeUTF8(fname, fname2);
	fp = fopen(fname2, "wb");
#endif
	if(!fp)
		return;
	fwrite(&pheader, sizeof(pheader), 1, fp);
	fwrite(comp_data, comp_size, 1, fp);
	fclose(fp);
}
bool Replay::OpenReplay(const wchar_t* name) {
#ifdef WIN32
	fp = _wfopen(name, L"rb");
#else
	char name2[256];
	BufferIO::EncodeUTF8(name, name2);
	fp = fopen(name2, "rb");
#endif
	if(!fp) {
		wchar_t fname[256];
		myswprintf(fname, L"./replay/%ls", name);
#ifdef WIN32
		fp = _wfopen(fname, L"rb");
#else
		char fname2[256];
		BufferIO::EncodeUTF8(fname, fname2);
		fp = fopen(fname2, "rb");
#endif
	}
	if(!fp)
		return false;
	fread(&pheader, sizeof(pheader), 1, fp);
	if(pheader.flag & REPLAY_COMPRESSED) {
		comp_size = fread(comp_data, 1, 0x20000, fp);
		fclose(fp);
		replay_size = pheader.datasize;
		if(LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.props, 5) != SZ_OK)
			return false;
	} else {
		comp_size = fread(replay_data, 1, 0x20000, fp);
		fclose(fp);
		replay_size = comp_size;
	}
	pdata = replay_data;
	is_replaying = true;
	return true;
}
bool Replay::CheckReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
#ifdef WIN32
	FILE* rfp = _wfopen(fname, L"rb");
#else
	char fname2[256];
	BufferIO::EncodeUTF8(fname, fname2);
	FILE* rfp = fopen(fname2, "rb");
#endif
	if(!rfp)
		return false;
	ReplayHeader rheader;
	fread(&rheader, sizeof(ReplayHeader), 1, rfp);
	fclose(rfp);
	return (rheader.id == 0x31707279 || rheader.id == 0x58707279) && rheader.version >= 0x12d0;
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
		pdata += (4 + ((pheader.flag & REPLAY_TAG) ? 160 : 80));
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
					comp_size = fread(replay_data, 1, 0x20000, fp);
					fclose(fp);
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
