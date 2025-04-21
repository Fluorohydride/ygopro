#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"

namespace ygo {

// replay flag
#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_UNIFORM		0x10

// max size
constexpr int MAX_REPLAY_SIZE = 0x20000;
constexpr int MAX_COMP_SIZE = UINT16_MAX + 1;

struct ReplayHeader {
	uint32_t id{};
	uint32_t version{};
	uint32_t flag{};
	uint32_t seed{};
	uint32_t datasize{};
	uint32_t start_time{};
	uint8_t props[8]{};
};

class Replay {
public:
	Replay();
	~Replay();

	// record
	void BeginRecord();
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, size_t length, bool flush = true);
	template<typename T>
	void Write(T data, bool flush = true) {
		WriteData(&data, sizeof(T), flush);
	}
	void WriteInt32(int32_t data, bool flush = true);
	void Flush();
	void EndRecord();
	void SaveReplay(const wchar_t* name);

	// play
	bool OpenReplay(const wchar_t* name);
	static bool CheckReplay(const wchar_t* name);
	static bool DeleteReplay(const wchar_t* name);
	static bool RenameReplay(const wchar_t* oldname, const wchar_t* newname);
	bool ReadNextResponse(unsigned char resp[]);
	bool ReadName(wchar_t* data);
	void ReadHeader(ReplayHeader& header);
	bool ReadData(void* data, size_t length);
	template<typename T>
	T Read() {
		T ret{};
		ReadData(&ret, sizeof(T));
		return ret;
	}
	int32_t ReadInt32();
	void Rewind();

	FILE* fp{ nullptr };
#ifdef _WIN32
	HANDLE recording_fp{ nullptr };
#endif

	ReplayHeader pheader;
	unsigned char* comp_data;
	size_t comp_size{};

private:
	unsigned char* replay_data;
	size_t replay_size{};
	size_t data_position{};
	bool is_recording{};
	bool is_replaying{};
};

}

#endif
