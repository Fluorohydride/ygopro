#ifndef REPLAY_H
#define REPLAY_H

#include "config.h"
#include "core_utils.h"
#include <ctime>
#include <vector>
#include <fstream>
#include "text_types.h"

namespace ygo {

#define REPLAY_COMPRESSED	0x1
#define REPLAY_TAG			0x2
#define REPLAY_DECODED		0x4
#define REPLAY_SINGLE_MODE	0x8
#define REPLAY_LUA64		0x10
#define REPLAY_NEWREPLAY	0x20
#define REPLAY_HAND_TEST	0x40

#define REPLAY_YRP1			0x31707279
#define REPLAY_YRPX			0x58707279

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
	ReplayPacket(const CoreUtils::Packet& packet);
	ReplayPacket(char* buf, int len);
	ReplayPacket(int msg, char* buf, int len);
	void Set(int msg, char* buf, int len);
	int message;
	std::vector<unsigned char> data;
};

struct ReplayDeck {
	std::vector<int> main_deck, extra_deck;
};

using ReplayDeckList = std::vector<ReplayDeck>;
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
	void BeginRecord(bool write = true, path_string name = EPRO_TEXT("./replay/_LastReplay.yrpX"));
	void WriteStream(const ReplayStream& stream);
	void WritePacket(const ReplayPacket& p);
	template <typename  T>
	void Write(T data, bool flush = true);
	void WritetoFile(const void* data, size_t size, bool flush);
	void WriteHeader(ReplayHeader& header);
	void WriteData(const void* data, unsigned int length, bool flush = true);
	void Flush();
	void EndRecord(size_t size = 0x20000);
	void SaveReplay(const path_string& name);
	bool OpenReplay(const path_string& name);
	bool OpenReplayFromBuffer(std::vector<uint8_t> contents);
	bool IsExportable();
	static bool DeleteReplay(const path_string& name);
	static bool RenameReplay(const path_string& oldname, const path_string& newname);
	bool GetNextResponse(ReplayResponse* res);
	const std::vector<std::wstring>& GetPlayerNames();
	const ReplayDeckList& GetPlayerDecks();
	const std::vector<int>& GetRuleCards();
	ReplayStream packets_stream;
	void Rewind();
	void Reset();
	int GetPlayersCount(int side);
	int GetTurnsCount();
	path_string GetReplayName();
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
	bool ReadData(void* data, unsigned int length);
	bool ReadData(std::vector<uint8_t>& data, unsigned int length);
	template <typename  T>
	T Read();
	bool ReadNextResponse(ReplayResponse* res);
	bool ReadName(wchar_t* data);
	bool ReadNextPacket(ReplayPacket* packet);
	std::ofstream fp;
	size_t data_position;
	void ParseNames();
	void ParseParams();
	void ParseDecks();
	void ParseStream();
	bool ParseResponses();
	bool is_recording;
	bool is_replaying;
	bool can_read;
	std::vector<ReplayResponse> responses;
	std::vector<std::wstring> players;
	size_t home_count;
	size_t opposing_count;
	path_string replay_name;
	ReplayDeckList decks;
	std::vector<int> replay_custom_rule_cards;
	std::vector<ReplayResponse>::iterator responses_iterator;
	int turn_count;
};
template<typename T>
inline void Replay::Write(T data, bool flush) {
	WriteData(&data, sizeof(T), flush);
}

}

#endif
