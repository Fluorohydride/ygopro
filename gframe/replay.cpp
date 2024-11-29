#include "replay.h"
#include "myfilesystem.h"
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
#ifdef _WIN32
	if(is_recording)
		CloseHandle(recording_fp);
	recording_fp = CreateFileW(L"./replay/_LastReplay.yrp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if(recording_fp == INVALID_HANDLE_VALUE)
		return;
#else
	if(is_recording)
		fclose(fp);
	fp = fopen("./replay/_LastReplay.yrp", "wb");
	if(!fp)
		return;
#endif
	pwrite = replay_data;
	replay_size = 0;
	comp_size = 0;
	is_replaying = false;
	is_recording = true;
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
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
	if (length < 0 || (int)(pwrite - replay_data) + length > MAX_REPLAY_SIZE)
		return;
	std::memcpy(pwrite, data, length);
	pwrite += length;
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
	WriteData(&data, sizeof data, flush);
}
void Replay::WriteInt16(short data, bool flush) {
	WriteData(&data, sizeof data, flush);
}
void Replay::WriteInt8(char data, bool flush) {
	WriteData(&data, sizeof data, flush);
}
void Replay::Flush() {
	if(!is_recording)
		return;
#ifdef _WIN32
#else
	fflush(fp);
#endif
}
void Replay::EndRecord() {
	if(!is_recording)
		return;
#ifdef _WIN32
	CloseHandle(recording_fp);
#else
	fclose(fp);
#endif
	replay_size = pwrite - replay_data;
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
	char fullname[256]{};
	BufferIO::EncodeUTF8(fname, fullname);
	FILE* rfp = myfopen(fullname, "wb");
	if(!rfp)
		return;
	fwrite(&pheader, sizeof pheader, 1, rfp);
	fwrite(comp_data, comp_size, 1, rfp);
	fclose(rfp);
}
bool Replay::OpenReplay(const wchar_t* name) {
	char fullname[256]{};
	BufferIO::EncodeUTF8(name, fullname);
	FILE* rfp = myfopen(fullname, "rb");
	if(!rfp) {
		wchar_t fname[256];
		myswprintf(fname, L"./replay/%ls", name);
		BufferIO::EncodeUTF8(fname, fullname);
		rfp = myfopen(fullname, "rb");
	}
	if(!rfp)
		return false;

	pdata = replay_data;
	is_recording = false;
	is_replaying = false;
	replay_size = 0;
	comp_size = 0;
	if(fread(&pheader, sizeof pheader, 1, rfp) < 1) {
		fclose(rfp);
		return false;
	}
	if(pheader.flag & REPLAY_COMPRESSED) {
		comp_size = fread(comp_data, 1, MAX_COMP_SIZE, rfp);
		fclose(rfp);
		if (pheader.datasize > MAX_REPLAY_SIZE)
			return false;
		replay_size = pheader.datasize;
		if (LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.props, 5) != SZ_OK)
			return false;
		if (replay_size != pheader.datasize) {
			replay_size = 0;
			return false;
		}
	} else {
		replay_size = fread(replay_data, 1, MAX_REPLAY_SIZE, rfp);
		fclose(rfp);
		comp_size = 0;
	}
	is_replaying = true;
	return true;
}
bool Replay::CheckReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
	char fullname[256]{};
	BufferIO::EncodeUTF8(fname, fullname);
	FILE* rfp = myfopen(fullname, "rb");
	if(!rfp)
		return false;
	ReplayHeader rheader;
	size_t count = fread(&rheader, sizeof rheader, 1, rfp);
	fclose(rfp);
	return count == 1 && rheader.id == 0x31707279 && rheader.version >= 0x12d0u && (rheader.version < 0x1353u || (rheader.flag & REPLAY_UNIFORM));
}
bool Replay::DeleteReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
#ifdef _WIN32
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
#ifdef _WIN32
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
	unsigned char len{};
	if (!ReadData(&len, sizeof len))
		return false;
	if (len > SIZE_RETURN_VALUE) {
		is_replaying = false;
		return false;
	}
	if (!ReadData(resp, len))
		return false;
	return true;
}
void Replay::ReadName(wchar_t* data) {
	uint16_t buffer[20]{};
	if (!ReadData(buffer, sizeof buffer)) {
		data[0] = 0;
		return;
	}
	BufferIO::CopyWStr(buffer, data, 20);
}
bool Replay::ReadData(void* data, int length) {
	if(!is_replaying)
		return false;
	if (length < 0)
		return false;
	if ((int)(pdata - replay_data) + length > (int)replay_size) {
		is_replaying = false;
		return false;
	}
	std::memcpy(data, pdata, length);
	pdata += length;
	return true;
}
template<typename T>
T Replay::ReadValue() {
	T ret{};
	if (!ReadData(&ret, sizeof ret))
		return -1;
	return ret;
}
int Replay::ReadInt32() {
	return ReadValue<int32_t>();
}
short Replay::ReadInt16() {
	return ReadValue<int16_t>();
}
char Replay::ReadInt8() {
	return ReadValue<char>();
}
void Replay::Rewind() {
	pdata = replay_data;
}

}
