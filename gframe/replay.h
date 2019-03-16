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
	ReplayPacket() {}
	ReplayPacket(char * buf, int len);
	ReplayPacket(int msg, char * buf, int len);
	void Set(int msg, char * buf, int len);
	int message;
	std::vector<unsigned char> data;
};

struct ReplayResponse {
public:
	int length;
	char message[64];
};

class Replay {
public:
	Replay();
	~Replay();
	void BeginRecord(bool write = true);
	void WriteStream(std::vector<ReplayPacket> stream);
	void WritePacket(ReplayPacket p);
	template <typename  T>
	void Write(const void* data, bool flush);
	void Write(const void* data, size_t size, bool flush);
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, unsigned int length, bool flush = true);
	void WriteInt32(int32_t data, bool flush = true);
	void WriteInt16(int16_t data, bool flush = true);
	void WriteInt8(int8_t data, bool flush = true);
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
	int32_t ReadInt32();
	int16_t ReadInt16();
	int8_t ReadInt8();
	void Rewind();
	bool LoadYrp();
	bool ParseResponses();
	ReplayHeader pheader;
	std::vector<uint8_t> replay_data;
	std::vector<uint8_t> comp_data;
	size_t data_position;
	size_t replay_size;
	size_t comp_size;
private:
	bool ReadNextResponse(ReplayResponse* res);
#ifdef _WIN32
	HANDLE fp;
#else
	FILE* fp;
#endif
	bool is_recording;
	bool is_writing;
	bool is_replaying;
	bool can_read;
	std::vector<ReplayResponse> responses;
	std::vector<ReplayResponse>::iterator responses_iterator;
};

}

#endif
