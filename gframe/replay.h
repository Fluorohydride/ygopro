#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"
#include <time.h>

namespace ygo {

// replay flag
#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_UNIFORM		0x10

// max size
#define MAX_REPLAY_SIZE	0x20000
#define MAX_COMP_SIZE	0x2000

struct ReplayHeader {
	unsigned int id;
	unsigned int version;
	unsigned int flag;
	unsigned int seed;
	unsigned int datasize;
	unsigned int start_time;
	unsigned char props[8];

	ReplayHeader()
		: id(0), version(0), flag(0), seed(0), datasize(0), start_time(0), props{ 0 } {}
};

class Replay {
public:
	Replay();
	~Replay();

	// record
	void BeginRecord();
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, int length, bool flush = true);
	void WriteInt32(int data, bool flush = true);
	void WriteInt16(short data, bool flush = true);
	void WriteInt8(char data, bool flush = true);
	void Flush();
	void EndRecord();
	void SaveReplay(const wchar_t* name);

	// play
	bool OpenReplay(const wchar_t* name);
	static bool CheckReplay(const wchar_t* name);
	static bool DeleteReplay(const wchar_t* name);
	static bool RenameReplay(const wchar_t* oldname, const wchar_t* newname);
	bool ReadNextResponse(unsigned char resp[64]);
	void ReadName(wchar_t* data);
	void ReadHeader(ReplayHeader& header);
	void ReadData(void* data, int length);
	int ReadInt32();
	short ReadInt16();
	char ReadInt8();
	void Rewind();

	FILE* fp;
#ifdef _WIN32
	HANDLE recording_fp;
#endif

	ReplayHeader pheader;
	unsigned char* replay_data;
	unsigned char* comp_data;
	size_t replay_size;
	size_t comp_size;

private:
	unsigned char* pdata;
	bool is_recording;
	bool is_replaying;
};

}

#endif
