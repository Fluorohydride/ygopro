#include "replay.h"
#include "myfilesystem.h"
#include "lzma/LzmaLib.h"

namespace ygo {

#ifdef YGOPRO_SERVER_MODE
extern unsigned short server_port;
extern unsigned short replay_mode;
#endif
Replay::Replay() {
	replay_data = new unsigned char[MAX_REPLAY_SIZE];
	comp_data = new unsigned char[MAX_COMP_SIZE];
}
Replay::~Replay() {
	delete[] replay_data;
	delete[] comp_data;
}
void Replay::BeginRecord() {
#ifdef YGOPRO_SERVER_MODE
	if(replay_mode & REPLAY_MODE_SAVE_IN_SERVER) {
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
	myswprintf(path, tmppath, server_port);
	recording_fp = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
#else
	recording_fp = CreateFileW(L"./replay/_LastReplay.yrp", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, nullptr);
#endif //YGOPRO_SERVER_MODE
	if(recording_fp == INVALID_HANDLE_VALUE)
		return;
#else
	if(is_recording)
		std::fclose(fp);
#ifdef YGOPRO_SERVER_MODE
	time_t nowtime = time(NULL);
	struct tm *localedtime = localtime(&nowtime);
	char tmppath[40];
	strftime(tmppath, 40, "./replay/%Y-%m-%d %H-%M-%S %%u.yrp", localedtime);
	char path[40];
	sprintf(path, tmppath, server_port);
	fp = myfopen(path, "wb");
#else
	fp = myfopen("./replay/_LastReplay.yrp", "wb");
#endif //YGOPRO_SERVER_MODE
	if(!fp)
		return;
#endif
#ifdef YGOPRO_SERVER_MODE
	}
#endif //YGOPRO_SERVER_MODE
	replay_size = 0;
	comp_size = 0;
	is_replaying = false;
	is_recording = true;
}
void Replay::WriteHeader(ReplayHeader& header) {
	pheader = header;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & REPLAY_MODE_SAVE_IN_SERVER)) return;
#endif
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, &header, sizeof(header), &size, nullptr);
#else
	std::fwrite(&header, sizeof(header), 1, fp);
	std::fflush(fp);
#endif
}
void Replay::WriteData(const void* data, size_t length, bool flush) {
	if(!is_recording)
		return;
	if (replay_size + length > MAX_REPLAY_SIZE)
		return;
	std::memcpy(replay_data + replay_size, data, length);
	replay_size += length;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & REPLAY_MODE_SAVE_IN_SERVER)) return;
#endif
#ifdef _WIN32
	DWORD size;
	WriteFile(recording_fp, data, length, &size, nullptr);
#else
	std::fwrite(data, length, 1, fp);
	if(flush)
		std::fflush(fp);
#endif
}
void Replay::WriteInt32(int32_t data, bool flush) {
	Write<int32_t>(data, flush);
}
void Replay::Flush() {
	if(!is_recording)
		return;
#ifdef YGOPRO_SERVER_MODE
	if(!(replay_mode & REPLAY_MODE_SAVE_IN_SERVER)) return;
#endif
#ifdef _WIN32
#else
	std::fflush(fp);
#endif
}
void Replay::EndRecord() {
	if(!is_recording)
		return;
#ifdef YGOPRO_SERVER_MODE
	if(replay_mode & REPLAY_MODE_SAVE_IN_SERVER) {
#endif
#ifdef _WIN32
	CloseHandle(recording_fp);
#else
	std::fclose(fp);
#endif
#ifdef YGOPRO_SERVER_MODE
	}
#endif
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
	FILE* rfp = mywfopen(fname, "wb");
	if(!rfp)
		return;
	std::fwrite(&pheader, sizeof pheader, 1, rfp);
	std::fwrite(comp_data, comp_size, 1, rfp);
	std::fclose(rfp);
}
bool Replay::OpenReplay(const wchar_t* name) {
	FILE* rfp = mywfopen(name, "rb");
	if(!rfp) {
		wchar_t fname[256];
		myswprintf(fname, L"./replay/%ls", name);
		rfp = mywfopen(fname, "rb");
	}
	if(!rfp)
		return false;

	data_position = 0;
	is_recording = false;
	is_replaying = false;
	replay_size = 0;
	comp_size = 0;
	if(std::fread(&pheader, sizeof pheader, 1, rfp) < 1) {
		std::fclose(rfp);
		return false;
	}
	if(pheader.flag & REPLAY_COMPRESSED) {
		comp_size = std::fread(comp_data, 1, MAX_COMP_SIZE, rfp);
		std::fclose(rfp);
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
		replay_size = std::fread(replay_data, 1, MAX_REPLAY_SIZE, rfp);
		std::fclose(rfp);
		comp_size = 0;
	}
	is_replaying = true;
	return true;
}
bool Replay::CheckReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
	FILE* rfp = mywfopen(fname, "rb");
	if(!rfp)
		return false;
	ReplayHeader rheader;
	size_t count = std::fread(&rheader, sizeof rheader, 1, rfp);
	std::fclose(rfp);
	return count == 1 && rheader.id == 0x31707279 && rheader.version >= 0x12d0u && (rheader.version < 0x1353u || (rheader.flag & REPLAY_UNIFORM));
}
bool Replay::DeleteReplay(const wchar_t* name) {
	wchar_t fname[256];
	myswprintf(fname, L"./replay/%ls", name);
	return FileSystem::RemoveFile(fname);
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
bool Replay::ReadName(wchar_t* data) {
	uint16_t buffer[20]{};
	if (!ReadData(buffer, sizeof buffer)) {
		return false;
	}
	BufferIO::CopyWStr(buffer, data, 20);
	return true;
}
void Replay::ReadHeader(ReplayHeader& header) {
	header = pheader;
}
bool Replay::ReadData(void* data, size_t length) {
	if(!is_replaying)
		return false;
	if (data_position + length > replay_size) {
		is_replaying = false;
		return false;
	}
	if (length)
		std::memcpy(data, &replay_data[data_position], length);
	data_position += length;
	return true;
}
int32_t Replay::ReadInt32() {
	return Read<int32_t>();
}
void Replay::Rewind() {
	data_position = 0;
}

}
