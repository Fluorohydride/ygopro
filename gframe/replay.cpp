#include "replay.h"
#include "myfilesystem.h"
#include "network.h"
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
	Reset();
	is_recording = true;
}
void Replay::WriteHeader(ExtendedReplayHeader& header) {
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
	pheader.base.datasize = replay_size;
	pheader.base.flag |= REPLAY_COMPRESSED;
	size_t propsize = 5;
	comp_size = MAX_COMP_SIZE;
	int ret = LzmaCompress(comp_data, &comp_size, replay_data, replay_size, pheader.base.props, &propsize, 5, 0x1U << 24, 3, 0, 2, 32, 1);
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

	Reset();
	bool correct_header = true;
	if (std::fread(&pheader, sizeof pheader.base, 1, rfp) < 1)
		correct_header = false;
	else if (pheader.base.id != REPLAY_ID_YRP1 && pheader.base.id != REPLAY_ID_YRP2)
		correct_header = false;
	else if (pheader.base.version < 0x12d0u)
		correct_header = false;
	else if (pheader.base.version >= 0x1353u && !(pheader.base.flag & REPLAY_UNIFORM))
		correct_header = false;
	if (!correct_header) {
		std::fclose(rfp);
		return false;
	}
	if (pheader.base.id == REPLAY_ID_YRP2 && std::fread(&pheader.seed_sequence, sizeof pheader.seed_sequence, 1, rfp) < 1) {
		std::fclose(rfp);
		return false;
	}
	if(pheader.base.flag & REPLAY_COMPRESSED) {
		comp_size = std::fread(comp_data, 1, MAX_COMP_SIZE, rfp);
		std::fclose(rfp);
		if (pheader.base.datasize > MAX_REPLAY_SIZE)
			return false;
		replay_size = pheader.base.datasize;
		if (LzmaUncompress(replay_data, &replay_size, comp_data, &comp_size, pheader.base.props, 5) != SZ_OK)
			return false;
		if (replay_size != pheader.base.datasize) {
			replay_size = 0;
			return false;
		}
	} else {
		replay_size = std::fread(replay_data, 1, MAX_REPLAY_SIZE, rfp);
		std::fclose(rfp);
		comp_size = 0;
	}
	is_replaying = true;
	can_read = true;
	if (!ReadInfo()) {
		Reset();
		return false;
	}
	info_offset = data_position;
	data_position = 0;
	return true;
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
void Replay::ReadHeader(ExtendedReplayHeader& header) {
	header = pheader;
}
bool Replay::ReadData(void* data, size_t length) {
	if (!is_replaying || !can_read)
		return false;
	if (data_position + length > replay_size) {
		can_read = false;
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
	can_read = true;
}
void Replay::Reset() {
	is_recording = false;
	is_replaying = false;
	can_read = false;
	replay_size = 0;
	comp_size = 0;
	data_position = 0;
	info_offset = 0;
	players.clear();
	params = { 0 };
	decks.clear();
	script_name.clear();
}
void Replay::SkipInfo(){
	if (data_position == 0)
		data_position += info_offset;
}
bool Replay::IsReplaying() const {
	return is_replaying;
}
bool Replay::ReadInfo() {
	int player_count = (pheader.base.flag & REPLAY_TAG) ? 4 : 2;
	for (int i = 0; i < player_count; ++i) {
		wchar_t name[20]{};
		if (!ReadName(name))
			return false;
		players.push_back(name);
	}
	if (!ReadData(&params, sizeof params))
		return false;
	bool is_tag1 = pheader.base.flag & REPLAY_TAG;
	bool is_tag2 = params.duel_flag & DUEL_TAG_MODE;
	if (is_tag1 != is_tag2)
		return false;
	if (pheader.base.flag & REPLAY_SINGLE_MODE) {
		uint16_t slen = Read<uint16_t>();
		char filename[256]{};
		if (slen == 0 || slen > sizeof(filename) - 1)
			return false;
		if (!ReadData(filename, slen))
			return false;
		filename[slen] = 0;
		if (std::strncmp(filename, "./single/", 9))
			return false;
		script_name = filename + 9;
		if (script_name.find_first_of(R"(/\)") != std::string::npos)
			return false;
	}
	else {
		for (int p = 0; p < player_count; ++p) {
			DeckArray deck;
			uint32_t main = Read<uint32_t>();
			if (main > MAINC_MAX)
				return false;
			if (main)
				deck.main.resize(main);
			if (!ReadData(deck.main.data(), main * sizeof(uint32_t)))
				return false;
			uint32_t extra = Read<uint32_t>();
			if (extra > MAINC_MAX)
				return false;
			if (extra)
				deck.extra.resize(extra);
			if (!ReadData(deck.extra.data(), extra * sizeof(uint32_t)))
				return false;
			decks.push_back(deck);
		}
	}
	return true;
}

}
