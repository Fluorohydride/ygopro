#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"
#include <ctime>
#include <vector>

namespace ygo {

#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_LUA64		0x10
#define REPLAY_NEWREPLAY	0x20
#define REPLAY_RELAY		0x40

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
	unsigned char data[0x20000];
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
	void Write(const void* data, size_t size, bool flush);
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, unsigned int length, bool flush = true);
	void WriteInt32(int data, bool flush = true);
	void WriteInt16(short data, bool flush = true);
	void WriteInt8(char data, bool flush = true);
	void Flush();
	void EndRecord(size_t size = 0x20000);
	void SaveReplay(const std::wstring& name);
	bool OpenReplay(const std::wstring& name);
	static bool CheckReplay(const std::wstring& name);
	bool ReadNextPacket(ReplayPacket* packet);
	bool ReadStream(std::vector<ReplayPacket>* stream);
	static bool DeleteReplay(const std::wstring& name);
	static bool RenameReplay(const std::wstring& oldname, const std::wstring& newname);
	bool ReadNextResponse(unsigned char resp[64]);
	bool ReadName(wchar_t* data);
	bool ReadData(void* data, unsigned int length);
	template <typename  T>
	T Read();
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
	std::vector<uint8_t> replay_data;
	std::vector<uint8_t> comp_data;
	std::vector<uint8_t>::iterator data_iterator;
	size_t replay_size;
	size_t comp_size;
	bool is_recording;
	bool is_writing;
	bool is_replaying;
	bool can_read;
};

}

#endif
