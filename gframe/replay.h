#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"
#include <time.h>

namespace ygo {

#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4

struct ReplayHeader {
	unsigned int id;
	unsigned int version;
	unsigned int flag;
	unsigned int seed;
	unsigned int datasize;
	unsigned int hash;
	unsigned char props[8];
};

class Replay {
public:
	Replay();
	~Replay();
	void BeginRecord();
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, unsigned int length, bool flush = true);
	void WriteInt32(int data, bool flush = true);
	void WriteInt16(short data, bool flush = true);
	void WriteInt8(char data, bool flush = true);
	void Flush();
	void EndRecord();
	void SaveReplay(const wchar_t* name);
	bool OpenReplay(const wchar_t* name);
	static bool CheckReplay(const wchar_t* name);
	bool ReadNextResponse(unsigned char resp[64]);
	void ReadHeader(ReplayHeader& header);
	void ReadData(void* data, unsigned int length);
	int ReadInt32();
	short ReadInt16();
	char ReadInt8();
	void Rewind();

	FILE* fp;
	ReplayHeader pheader;
#ifdef _WIN32
	HANDLE recording_fp;
#endif
	unsigned char* replay_data;
	unsigned char* comp_data;
	unsigned char* pdata;
	size_t replay_size;
	size_t comp_size;
	bool is_recording;
	bool is_replaying;
};

}

#endif
