#include "replay.h"
#include "lzma/LzmaLib.h"

namespace ygo {

Replay::Replay() {
	replay_data = new unsigned char[MAX_REPLAY_SIZE];
	comp_data = new unsigned char[MAX_COMP_SIZE];
}
Replay::~Replay() {
	delete[] replay_data;
	delete[] comp_data;
}
void Replay::BeginRecord() {
	if(!FileSystem::IsDirExists(L"./replay") && !FileSystem::MakeDir(L"./replay"))
		return;
	if(is_recording)
		fclose(recording_fp);
	recording_fp = fopen("./replay/_LastReplay.yrp", "wb");
	if(!recording_fp)
		return;
	pdata = replay_data;
	replay_size = 0;
	comp_size = 0;
	is_replaying = false;
	is_recording = true;
}
void Replay::WriteHeader(ReplayHeader& header) {
	if (!is_recording)
		return;
	pheader = header;
	fwrite(&header, sizeof(header), 1, recording_fp);
	fflush(recording_fp);
}
void Replay::WriteData(const void* data, int length, bool flush) {
	if(!is_recording)
		return;
	if (length < 0 || (pdata - replay_data) + length > MAX_REPLAY_SIZE)
		return;
	std::memcpy(pdata, data, length);
	pdata += length;
	fwrite(data, length, 1, recording_fp);
	if(flush)
		fflush(recording_fp);
}
void Replay::WriteInt32(int data, bool flush) {
	if(!is_recording)
		return;
	if ((pdata - replay_data) + 4 > MAX_REPLAY_SIZE)
		return;
	BufferIO::WriteInt32(pdata, data);
	fwrite(&data, sizeof(int), 1, recording_fp);
	if(flush)
		fflush(recording_fp);
}
void Replay::WriteInt16(short data, bool flush) {
	if(!is_recording)
		return;
	if ((pdata - replay_data) + 2 > MAX_REPLAY_SIZE)
		return;
	BufferIO::WriteInt16(pdata, data);
	fwrite(&data, sizeof(short), 1, recording_fp);
	if(flush)
		fflush(recording_fp);
}
void Replay::WriteInt8(char data, bool flush) {
	if(!is_recording)
		return;
	if ((pdata - replay_data) + 1 > MAX_REPLAY_SIZE)
		return;
	BufferIO::WriteInt8(pdata, data);
	fwrite(&data, sizeof(char), 1, recording_fp);
	if(flush)
		fflush(recording_fp);
}
void Replay::Flush() {
	if(!is_recording)
		return;
	fflush(recording_fp);
}
void Replay::EndRecord() {
	if(!is_recording)
		return;
	fclose(recording_fp);
	if(pdata - replay_data > 0 && pdata - replay_data <= MAX_REPLAY_SIZE)
		replay_size = pdata - replay_data;
	else
		replay_size = 0;
	pheader.datasize = replay_size;
	pheader.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = MAX_COMP_SIZE;
	int ret = LzmaCompress(comp_data, &comp_size, replay_data, replay_size, pheader.props, &propsize, 5, 1 << 24, 3, 0, 2, 32, 1);
	if (ret != SZ_OK) {
		std::memcpy(comp_data, &ret, sizeof ret);
		comp_size = sizeof ret;
	}
	is_recording = false;
}
void Replay::SaveReplay(const wchar_t* name) {
	if(!FileSystem::IsDirExists(L"./replay") && !FileSystem::MakeDir(L"./replay"))
		return;
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls.yrp", name);
	char fname2[256];
	BufferIO::EncodeUTF8(fname, fname2);
	FILE* fp = fopen(fname2, "wb");
	if(!fp)
		return;
	fwrite(&pheader, sizeof(pheader), 1, fp);
	fwrite(comp_data, comp_size, 1, fp);
	fclose(fp);
}
bool Replay::OpenReplay(const wchar_t* name) {
	char name2[256]{};
	BufferIO::EncodeUTF8(name, name2);
	FILE* fp = fopen(name2, "rb");
	if(!fp) {
		wchar_t fname[256]{};
		myswprintf(fname, L"./replay/%ls", name);
		char fname2[256]{};
		BufferIO::EncodeUTF8(fname, fname2);
		fp = fopen(fname2, "rb");
	}
	if(!fp)
		return false;

	pdata = replay_data;
	is_recording = false;
	is_replaying = false;
	replay_size = 0;
	comp_size = 0;
	if(fread(&pheader, sizeof(pheader), 1, fp) < 1) {
		fclose(fp);
		return false;
	}
	if(pheader.flag & REPLAY_COMPRESSED) {
		comp_size = fread(comp_data, 1, MAX_COMP_SIZE, fp);
		fclose(fp);
		if ((int)pheader.datasize < 0 && (int)pheader.datasize > MAX_REPLAY_SIZE)
			return false;
		replay_size = pheader.datasize;
		if (LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.props, 5) != SZ_OK)
			return false;
		if (replay_size != pheader.datasize) {
			replay_size = 0;
			return false;
		}
	} else {
		replay_size = fread(replay_data, 1, MAX_REPLAY_SIZE, fp);
		fclose(fp);
		comp_size = 0;
	}
	is_replaying = true;
	return true;
}
bool Replay::CheckReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
	char fname2[256];
	BufferIO::EncodeUTF8(fname, fname2);
	FILE* rfp = fopen(fname2, "rb");
	if(!rfp)
		return false;
	ReplayHeader rheader;
	size_t count = fread(&rheader, sizeof(ReplayHeader), 1, rfp);
	fclose(rfp);
	return count == 1 && rheader.id == 0x31707279 && rheader.version >= 0x12d0u && (rheader.version < 0x1353u || (rheader.flag & REPLAY_UNIFORM));
}
bool Replay::DeleteReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
#ifdef WIN32
	BOOL result = DeleteFileW(fname);
	return !!result;
#else
	char filefn[256];
	BufferIO::EncodeUTF8(fname, filefn);
	int result = unlink(filefn);
	return result == 0;
#endif
}
bool Replay::RenameReplay(const wchar_t* oldname, const wchar_t* newname) {
	wchar_t oldfname[256];
	wchar_t newfname[256];
	myswprintf(oldfname, L"./replay/%ls", oldname);
	myswprintf(newfname, L"./replay/%ls", newname);
#ifdef WIN32
	BOOL result = MoveFileW(oldfname, newfname);
	return !!result;
#else
	char oldfilefn[256];
	char newfilefn[256];
	BufferIO::EncodeUTF8(oldfname, oldfilefn);
	BufferIO::EncodeUTF8(newfname, newfilefn);
	int result = rename(oldfilefn, newfilefn);
	return result == 0;
#endif
}
bool Replay::ReadNextResponse(unsigned char resp[]) {
	if(pdata - replay_data >= (int)replay_size)
		return false;
	int len = *pdata++;
	if(len > SIZE_RETURN_VALUE)
		return false;
	std::memcpy(resp, pdata, len);
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
void Replay::ReadData(void* data, int length) {
	if(!is_replaying)
		return;
	std::memcpy(data, pdata, length);
	pdata += length;
}
int Replay::ReadInt32() {
	if(!is_replaying)
		return -1;
	int ret = BufferIO::ReadInt32(pdata);
	return ret;
}
short Replay::ReadInt16() {
	if(!is_replaying)
		return -1;
	short ret = BufferIO::ReadInt16(pdata);
	return ret;
}
char Replay::ReadInt8() {
	if(!is_replaying)
		return -1;
	char ret= BufferIO::ReadInt8(pdata);
	return ret;
}
void Replay::Rewind() {
	pdata = replay_data;
}

}
