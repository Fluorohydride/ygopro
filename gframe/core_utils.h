#ifndef CORE_UTILS_H_
#define CORE_UTILS_H_
#include <vector>
#include <cstdint>
#include "bufferio.h"
namespace CoreUtils {
class Packet {
public:
	Packet() {}
	Packet(char* buf, int len) {
		uint8_t msg = BufferIO::Read<uint8_t>(buf);
		Set(msg, buf, len);
	};
	Packet(int msg, char* buf, int len) {
		Set(msg, buf, len);
	};
	void Set(int msg, char* buf, int len) {
		message = msg;
		data.resize(len);
		if(len)
			memcpy(data.data(), buf, data.size());
		data.insert(data.begin(), (uint8_t)message);
	};
	uint8_t message;
	std::vector<unsigned char> data;
};
class PacketStream {
public:
	std::vector<Packet> packets;
	PacketStream() {}
	PacketStream(char* buf, int len);
};
struct loc_info {
	uint8_t controler;
	uint8_t location;
	uint32_t sequence;
	uint32_t position;
};
loc_info ReadLocInfo(char*& p, bool compat = false);
class Query {
public:
	Query() {};
	Query(char*& buff) { Parse(buff); };
	void Parse(char*& buff);
	bool onfield_skipped = false;
	uint32_t flag;
	uint32_t code;
	uint32_t position;
	uint32_t alias;
	uint32_t type;
	uint32_t level;
	uint32_t rank;
	uint32_t link;
	uint32_t attribute;
	uint32_t race;
	int32_t attack;
	int32_t defense;
	int32_t base_attack;
	int32_t base_defense;
	uint32_t reason;
	uint32_t owner;
	uint32_t status;
	uint32_t is_public;
	uint32_t lscale;
	uint32_t rscale;
	uint32_t link_marker;
	loc_info reason_card;
	loc_info equip_card;
	std::vector<loc_info> target_cards;
	std::vector<uint32_t> overlay_cards;
	std::vector<uint32_t> counters;
	void GenerateBuffer(std::vector<uint8_t>& len, bool is_public);
	bool IsPublicQuery(uint32_t flag);
	uint32_t GetSize(uint32_t flag);
	uint32_t GetSize();
};
class QueryStream {
public:
	std::vector<Query> queries;
	QueryStream() {};
	QueryStream(char*& buff) { Parse(buff); };
	void Parse(char*& buff);
	void GenerateBuffer(std::vector<uint8_t>& buffer);
	void GeneratePublicBuffer(std::vector<uint8_t>& buffer);
};
using OCG_Duel = void*;
PacketStream ParseMessages(OCG_Duel duel);
bool MessageBeRecorded(uint32_t message);
};

#endif
