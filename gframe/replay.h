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

using ReplayDeck = std::vector<std::pair<std::vector<int>, std::vector<int>>>;
using ReplayStream = std::vector<ReplayPacket>;

struct ReplayResponse {
public:
	int length;
	std::vector<uint8_t> response;
};

class Replay {
public:
	Replay();
	~Replay();
	void BeginRecord(bool write = true);
	void WriteStream(ReplayStream stream);
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
	bool OpenReplayFromBuffer(std::vector<uint8_t> contents);
	static bool CheckReplay(const std::wstring& name);
	static bool DeleteReplay(const std::wstring& name);
	static bool RenameReplay(const std::wstring& oldname, const std::wstring& newname);
	bool GetNextResponse(ReplayResponse* res);
	std::vector<std::wstring> GetPlayerNames();
	ReplayDeck GetPlayerDecks();
	ReplayStream packets_stream;
	void Rewind();
	std::unique_ptr<Replay> yrp;
	ReplayHeader pheader;
	std::vector<uint8_t> replay_data;
	std::vector<uint8_t> comp_data;
	size_t replay_size;
	size_t comp_size;
	struct duel_parameters {
		int start_lp;
		int start_hand;
		int draw_count;
		int duel_flags;
	};
	duel_parameters params;
	std::string scriptname;
private:
	void Reset();
	bool ReadData(void* data, unsigned int length);
	template <typename  T>
	T Read();
	int32_t ReadInt32();
	int16_t ReadInt16();
	int8_t ReadInt8();
	bool ReadNextResponse(ReplayResponse* res);
	bool ReadName(wchar_t* data);
	bool ReadNextPacket(ReplayPacket* packet);
#ifdef _WIN32
	HANDLE fp;
#else
	FILE* fp;
#endif
	size_t data_position;
	void ParseNames();
	void ParseParams();
	void ParseDecks();
	void ParseStream();
	bool ParseResponses();
	bool is_recording;
	bool is_writing;
	bool is_replaying;
	bool can_read;
	std::vector<ReplayResponse> responses;
	std::vector<std::wstring> players;
	ReplayDeck decks;
	std::vector<ReplayResponse>::iterator responses_iterator;
};

}

#endif
