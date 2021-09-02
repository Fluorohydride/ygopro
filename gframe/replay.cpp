#include "replay.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/common.h"
#include "lzma/LzmaLib.h"

namespace ygo {

#ifdef YGOPRO_SERVER_MODE
extern unsigned short aServerPort;
extern unsigned short replay_mode;
#endif
Replay::Replay()
	: fp(nullptr), pheader(), pdata(nullptr), replay_size(0), comp_size(0), is_recording(false), is_replaying(false) {
	#ifdef _WIN32
		recording_fp = nullptr;
	#endif
	replay_data = new unsigned char[MAX_REPLAY_SIZE];
	comp_data = new unsigned char[MAX_COMP_SIZE];
}
Replay::~Replay() {
	delete[] replay_data;
	delete[] comp_data;
}
void Replay::BeginRecord() {
#ifdef YGOPRO_SERVER_MODE
	if(replay_mode & 0x1) {
#endif
	if(!FileSystem::IsDirExists(L"./replay") && !FileSystem::MakeDir(L"./replay"))
		return;
#ifdef _WIN32
	if(is_recording)
		CloseHandle(recording_fp);
#ifdef YGOPRO_SERVER_MODE
	time_t nowtime = time(NULL);
	struct tm *localedtime = localtime(&nowtime);
	wchar_t tmppath[80];
	wcsftime(tmppath, 80, L"./replay/%Y-%m-%d %H-%M-%S %%u.yrp", localedtime);
	wchar_t path[80];
	myswprintf(path, tmppath, aServerPort);
	recording_fp = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
#else
	recording_fp = CreateFileW(L"./replay/_LastReplay.yrp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
#endif //YGOPRO_SERVER_MODE
	if(recording_fp == INVALID_HANDLE_VALUE)
		return;
#else
	if(is_recording)
		fclose(fp);
#ifdef YGOPRO_SERVER_MODE
	time_t nowtime = time(NULL);
	struct tm *localedtime = localtime(&nowtime);
	char tmppath[40];
	strftime(tmppath, 40, "./replay/%Y-%m-%d %H-%M-%S %%u.yrp", localedtime);
	char path[40];
	sprintf(path, tmppath, aServerPort);
	fp = fopen(path, "wb");
#else
	fp = fopen("./replay/_LastReplay.yrp", "wb");
#endif //YGOPRO_SERVER_MODE
	if(!fp)
		return;
#endif
#ifdef YGOPRO_SERVER_MODE
	}
#endif //YGOPRO_SERVER_MODE
	pdata = replay_data;
	replay_size = 0;
	comp_size = 0;
	is_replaying = false;
	is_recording = true;
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &header, sizeof(header), &size, NULL);
#else
	fwrite(&header, sizeof(header), 1, fp);
	fflush(fp);
#endif
}
void Replay::WriteData(const void* data, int length, bool flush) {
	if(!is_recording)
		return;
	if (length < 0 || (pdata - replay_data) + length > MAX_REPLAY_SIZE)
		return;
	memcpy(pdata, data, length);
	pdata += length;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
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
	if ((pdata - replay_data) + 4 > MAX_REPLAY_SIZE)
		return;
	*((int*)(pdata)) = data;
	pdata += 4;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
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
	if ((pdata - replay_data) + 2 > MAX_REPLAY_SIZE)
		return;
	*((short*)(pdata)) = data;
	pdata += 2;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
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
	if ((pdata - replay_data) + 1 > MAX_REPLAY_SIZE)
		return;
	*pdata = data;
	pdata++;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
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
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & 0x1)) return;
#endif
#ifdef _WIN32
#else
	fflush(fp);
#endif
}
void Replay::EndRecord() {
	if(!is_recording)
		return;
#ifdef YGOPRO_SERVER_MODE
	if(replay_mode & 0x1) {
#endif
#ifdef _WIN32
	CloseHandle(recording_fp);
#else
	fclose(fp);
#endif
#ifdef YGOPRO_SERVER_MODE
	}
#endif
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
		*((int*)(comp_data)) = ret;
		comp_size = sizeof(ret);
	}
	is_recording = false;
}
void Replay::SaveReplay(const wchar_t* name) {
	if(!FileSystem::IsDirExists(L"./replay") && !FileSystem::MakeDir(L"./replay"))
		return;
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls.yrp", name);
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
	size_t count = fread(&rheader, sizeof(ReplayHeader), 1, rfp);
	fclose(rfp);
	return count == 1 && rheader.id == 0x31707279 && rheader.version >= 0x12d0u;
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
void Replay::ReadData(void* data, int length) {
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

}
