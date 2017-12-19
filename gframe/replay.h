#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"
#include <time.h>

namespace ygo {

#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_LUA64		0x10
#define REPLAY_NEWREPLAY	0x20

struct ReplayHeader {
	unsigned int id;
	unsigned int version;
	unsigned int flag;
	unsigned int seed;
	unsigned int datasize;
	unsigned int hash;
	unsigned char props[8];
};

class ReplayPacket {
public:
	int message;
	int length;
	unsigned char data[0x2000];
	ReplayPacket() {}
	ReplayPacket(char * buf, int len);
	ReplayPacket(int msg, char * buf, int len);
	void Set(int msg, char * buf, int len);
};

class Replay {
public:
	Replay();
	~Replay();
	void BeginRecord(bool write = true);
	void WriteStream(std::vector<ReplayPacket> stream);
	void WritePacket(ReplayPacket p);
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, unsigned int length, bool flush = true);
	void WriteInt32(int data, bool flush = true);
	void WriteInt16(short data, bool flush = true);
	void WriteInt8(char data, bool flush = true);
	void Flush();
	void EndRecord(size_t size = 0x20000);
	void SaveReplay(const wchar_t* name);
	bool OpenReplay(const wchar_t* name);
	static bool CheckReplay(const wchar_t* name);
	bool ReadNextPacket(ReplayPacket* packet);
	bool ReadStream(std::vector<ReplayPacket>* stream);
	bool ReadNextResponse(unsigned char resp[64]);
	void ReadName(wchar_t* data);
	void ReadData(void* data, unsigned int length);
	int ReadInt32();
	short ReadInt16();
	char ReadInt8();
	void Rewind();
	bool LoadYrp();

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
	bool is_writing;
	bool is_replaying;
};

}

#endif
