#include "replay.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"
#include <algorithm>
#include "lzma/LzmaLib.h"

namespace ygo {

Replay::Replay() {
	is_recording = false;
	is_replaying = false;
	replay_data = new unsigned char[0x20000];
	comp_data = new unsigned char[0x2000];
}
Replay::~Replay() {
	delete replay_data;
	delete comp_data;
}
void Replay::BeginRecord() {
	if(is_recording)
		fclose(fp);
	fp = fopen("./replay/_LastReplay.yrp", "wb");
	if(!fp)
		return;
	pdata = replay_data;
	is_recording = true;
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
	fwrite(&header, sizeof(header), 1, fp);
	fflush(fp);
}
void Replay::WriteData(const void* data, unsigned int length, bool flush) {
	if(!is_recording)
		return;
	fwrite(data, length, 1, fp);
	memcpy(pdata, data, length);
	pdata += length;
	if(flush)
		fflush(fp);
}
void Replay::WriteInt32(int data, bool flush) {
	if(!is_recording)
		return;
	fwrite(&data, sizeof(int), 1, fp);
	*((int*)(pdata)) = data;
	pdata += 4;
	if(flush)
		fflush(fp);
}
void Replay::WriteInt16(short data, bool flush) {
	if(!is_recording)
		return;
	fwrite(&data, sizeof(short), 1, fp);
	*((short*)(pdata)) = data;
	pdata += 2;
	if(flush)
		fflush(fp);
}
void Replay::WriteInt8(char data, bool flush) {
	if(!is_recording)
		return;
	fwrite(&data, sizeof(char), 1, fp);
	*pdata = data;
	pdata++;
	if(flush)
		fflush(fp);
}
void Replay::Flush() {
	if(!is_recording)
		return;
	fflush(fp);
}
void Replay::EndRecord() {
	if(!is_recording)
		return;
	fclose(fp);
	pheader.datasize = pdata - replay_data;
	pheader.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = 0x1000;
	LzmaCompress(comp_data, &comp_size, replay_data, pdata - replay_data, pheader.props, &propsize, 5, 1 << 24, 3, 0, 2, 32, 1);
	is_recording = false;
}
void Replay::SaveReplay(const wchar_t* name) {
	wchar_t fname[64];
	myswprintf(fname, L"./replay/%ls.yrp", name);
#ifdef WIN32
	fp = _wfopen(fname, L"wb");
#else
	char fname2[256];
	DataManager::EncodeUTF8(fname, fname2);
	fp = fopen(fname2, "wb");
#endif
	if(!fp)
		return;
	fwrite(&pheader, sizeof(pheader), 1, fp);
	fwrite(comp_data, comp_size, 1, fp);
	fclose(fp);
}
bool Replay::OpenReplay(const wchar_t* name) {
	wchar_t fname[64];
	myswprintf(fname, L"./replay/%ls", name);
#ifdef WIN32
	fp = _wfopen(fname, L"rb");
#else
	char fname2[256];
	DataManager::EncodeUTF8(fname, fname2);
	fp = fopen(fname2, "rb");
#endif
	if(!fp)
		return false;
	fseek(fp, 0, SEEK_END);
	comp_size = ftell(fp) - sizeof(pheader);
	fseek(fp, 0, SEEK_SET);
	fread(&pheader, sizeof(pheader), 1, fp);
	if(pheader.flag & REPLAY_COMPRESSED) {
		fread(comp_data, 0x1000, 1, fp);
		fclose(fp);
		replay_size = pheader.datasize;
		if(LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.props, 5) != SZ_OK)
			return false;
	} else {
		fread(replay_data, 0x20000, 1, fp);
		fclose(fp);
		replay_size = comp_size;
	}
	pdata = replay_data;
	is_replaying = true;
	return true;
}
bool Replay::CheckReplay(const wchar_t* name) {
	wchar_t fname[64];
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
	return rheader.id == 0x31707279 && rheader.version >= 0x1020;
}
bool Replay::ReadNextResponse(unsigned char resp[64]) {
	char resType = *pdata++;
	if(pdata - replay_data >= replay_size)
		return false;
	if(resType == 1) {
		*((int*)resp) = *((int*)pdata);
		pdata += 4;
	} else if(resType = 2) {
		int len = *pdata++;
		for(int i = 0; i < len; ++i)
			resp[i] = *pdata++;
	} else
		return false;
	return true;
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
	pdata += 4;
	return ret;
}
char Replay::ReadInt8() {
	if(!is_replaying)
		return -1;
	return *pdata++;
}

}
