#include "core_utils.h"
#include "bufferio.h"
#include "dllinterface.h"
#include "common.h"

namespace CoreUtils {

#define PARSE(value) do {value = BufferIO::Read<decltype(value)>(current);} while(0); break

void Query::Parse(const uint8_t*& current) {
	flag = 0;
	for(;;) {
		auto size = BufferIO::Read<uint16_t>(current);
		if(size == 0) {
			onfield_skipped = true;
			return;
		}
		auto cur_flag = BufferIO::Read<uint32_t>(current);
		flag |= cur_flag;
		switch(cur_flag) {
			case QUERY_CODE: PARSE(code);
			case QUERY_POSITION: PARSE(position);
			case QUERY_ALIAS: PARSE(alias);
			case QUERY_TYPE: PARSE(type);
			case QUERY_LEVEL: PARSE(level);
			case QUERY_RANK: PARSE(rank);
			case QUERY_ATTRIBUTE: PARSE(attribute);
			case QUERY_RACE: PARSE(race);
			case QUERY_ATTACK: PARSE(attack);
			case QUERY_DEFENSE: PARSE(defense);
			case QUERY_BASE_ATTACK: PARSE(base_attack);
			case QUERY_BASE_DEFENSE: PARSE(base_defense);
			case QUERY_REASON: PARSE(reason);
			case QUERY_OWNER: PARSE(owner);
			case QUERY_STATUS: PARSE(status);
			case QUERY_IS_PUBLIC: PARSE(is_public);
			case QUERY_LSCALE: PARSE(lscale);
			case QUERY_RSCALE: PARSE(rscale);
			case QUERY_IS_HIDDEN: PARSE(is_hidden);
			case QUERY_COVER: PARSE(cover);
			case QUERY_REASON_CARD: {
				reason_card = ReadLocInfo(current, false);
				break;
			}
			case QUERY_EQUIP_CARD: {
				equip_card = ReadLocInfo(current, false);
				break;
			}
			case QUERY_TARGET_CARD: {
				auto count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					target_cards.push_back(ReadLocInfo(current, false));
				break;
			}
			case QUERY_OVERLAY_CARD: {
				auto count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					overlay_cards.push_back(BufferIO::Read<uint32_t>(current));
				break;
			}
			case QUERY_COUNTERS: {
				auto count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					counters.push_back(BufferIO::Read<uint32_t>(current));
				break;
			}
			case QUERY_LINK: {
				link = BufferIO::Read<uint32_t>(current);
				link_marker = BufferIO::Read<uint32_t>(current);
				break;
			}
			case QUERY_END:
				return;
			default: {
				current += size - sizeof(uint32_t);
				break;
			}
		}
	}
}
#undef PARSE
#define PARSE_SINGLE(query,value) do{if(flag & query) {\
value = BufferIO::Read<uint32_t>(current);\
}} while(0)

void Query::ParseCompat(const uint8_t* current, uint32_t len) {
	if(len <= 8) {
		onfield_skipped = true;
		return;
	}
	flag = BufferIO::Read<int32_t>(current);
	PARSE_SINGLE(QUERY_CODE, code);
	if(flag & QUERY_POSITION)
		position = (BufferIO::Read<uint32_t>(current) >> 24) & 0xff;
	PARSE_SINGLE(QUERY_ALIAS, alias);
	PARSE_SINGLE(QUERY_TYPE, type);
	PARSE_SINGLE(QUERY_LEVEL, level);
	PARSE_SINGLE(QUERY_RANK, rank);
	PARSE_SINGLE(QUERY_ATTRIBUTE, attribute);
	PARSE_SINGLE(QUERY_RACE, race);
	PARSE_SINGLE(QUERY_ATTACK, attack);
	PARSE_SINGLE(QUERY_DEFENSE, defense);
	PARSE_SINGLE(QUERY_BASE_ATTACK, base_attack);
	PARSE_SINGLE(QUERY_BASE_DEFENSE, base_defense);
	PARSE_SINGLE(QUERY_REASON, reason);
	if(flag & QUERY_REASON_CARD)
		reason_card = ReadLocInfo(current, true);
	if(flag &  QUERY_EQUIP_CARD)
		equip_card = ReadLocInfo(current, true);
	if(flag & QUERY_TARGET_CARD) {
		auto count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			target_cards.push_back(ReadLocInfo(current, true));
	}
	if(flag & QUERY_OVERLAY_CARD) {
		auto count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			overlay_cards.push_back(BufferIO::Read<uint32_t>(current));
	}
	if(flag & QUERY_COUNTERS) {
		auto count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			counters.push_back(BufferIO::Read<uint32_t>(current));
	}
	PARSE_SINGLE(QUERY_OWNER, owner);
	PARSE_SINGLE(QUERY_STATUS, status);
	PARSE_SINGLE(QUERY_LSCALE, lscale);
	PARSE_SINGLE(QUERY_RSCALE, rscale);
	if(flag & QUERY_LINK) {
		link = BufferIO::Read<uint32_t>(current);
		link_marker = BufferIO::Read<uint32_t>(current);
	}
}
#undef PARSE_SINGLE

#define SET(val) do{BufferIO::insert_value<decltype(val)>(buffer, val);} while(0); break
void Query::GenerateBuffer(std::vector<uint8_t>& buffer, bool is_for_public_buffer, bool check_hidden) const {
	if(onfield_skipped) {
		BufferIO::insert_value<uint16_t>(buffer, 0);
		return;
	}
	const bool query_must_be_pubilc = is_for_public_buffer || (check_hidden && ((flag & QUERY_IS_HIDDEN) && is_hidden));
	for(uint64_t cur_flag = 1; cur_flag <= QUERY_END; cur_flag <<= 1) {
		if((flag & cur_flag) == 0)
			continue;
		if(query_must_be_pubilc && !IsPublicQuery(cur_flag)) {
			//Sends query that "removes" the knowledge for cards
			/*const auto vec_size = buffer.size();
			const auto flag_size = GetFlagSize(cur_flag);
			buffer.resize(vec_size + flag_size);
			std::memset(&buffer[vec_size], 0, flag_size);*/
			continue;
		}
		if((cur_flag == QUERY_REASON_CARD && reason_card.location == 0) ||
			(cur_flag == QUERY_EQUIP_CARD && equip_card.location == 0))
			continue;
		BufferIO::insert_value<uint16_t>(buffer, GetFlagSize(cur_flag) + sizeof(uint32_t));
		BufferIO::insert_value<uint32_t>(buffer, cur_flag);
		switch(cur_flag) {
			case QUERY_CODE: SET(code);
			case QUERY_POSITION: SET(position);
			case QUERY_ALIAS: SET(alias);
			case QUERY_TYPE: SET(type);
			case QUERY_LEVEL: SET(level);
			case QUERY_RANK: SET(rank);
			case QUERY_ATTRIBUTE: SET(attribute);
			case QUERY_RACE: SET(race);
			case QUERY_ATTACK: SET(attack);
			case QUERY_DEFENSE: SET(defense);
			case QUERY_BASE_ATTACK: SET(base_attack);
			case QUERY_BASE_DEFENSE: SET(base_defense);
			case QUERY_REASON: SET(reason);
			case QUERY_OWNER: SET(owner);
			case QUERY_STATUS: SET(status);
			case QUERY_IS_PUBLIC: SET(is_public);
			case QUERY_LSCALE: SET(lscale);
			case QUERY_RSCALE: SET(rscale);
			case QUERY_IS_HIDDEN: SET(is_hidden);
			case QUERY_COVER: SET(cover);
			case QUERY_REASON_CARD:
			case QUERY_EQUIP_CARD: {
				auto& info = (cur_flag == QUERY_REASON_CARD) ? reason_card : equip_card;
				BufferIO::insert_value<uint8_t>(buffer, info.controler);
				BufferIO::insert_value<uint8_t>(buffer, info.location);
				BufferIO::insert_value<uint32_t>(buffer, info.sequence);
				BufferIO::insert_value<uint32_t>(buffer, info.position);
				break;
			}
			case QUERY_TARGET_CARD: {
				BufferIO::insert_value<uint32_t>(buffer, target_cards.size());
				for(auto& info : target_cards) {
					BufferIO::insert_value<uint8_t>(buffer, info.controler);
					BufferIO::insert_value<uint8_t>(buffer, info.location);
					BufferIO::insert_value<uint32_t>(buffer, info.sequence);
					BufferIO::insert_value<uint32_t>(buffer, info.position);
				}
				break;
			}
			case QUERY_OVERLAY_CARD:
			case QUERY_COUNTERS: {
				auto& vec = (cur_flag == QUERY_OVERLAY_CARD) ? overlay_cards : counters;
				BufferIO::insert_value<uint32_t>(buffer, vec.size());
				for(auto& val : vec)
					BufferIO::insert_value<uint32_t>(buffer, val);
				break;
			}
			case QUERY_LINK: {
				BufferIO::insert_value<uint32_t>(buffer, link);
				BufferIO::insert_value<uint32_t>(buffer, link_marker);
				break;
			}
		}
	}
}
#undef SET

bool Query::IsPublicQuery(uint32_t to_check_flag) const {
	static constexpr auto private_queries = QUERY_CODE | QUERY_ALIAS | QUERY_TYPE | QUERY_LEVEL
		| QUERY_RANK | QUERY_ATTRIBUTE | QUERY_RACE | QUERY_ATTACK | QUERY_DEFENSE | QUERY_BASE_ATTACK
		| QUERY_BASE_DEFENSE | QUERY_STATUS | QUERY_LSCALE | QUERY_RSCALE | QUERY_LINK;

	if(((flag & QUERY_IS_PUBLIC) && is_public) || ((flag & QUERY_POSITION) && (position & POS_FACEUP)))
		return true;
	return (to_check_flag & private_queries) == 0;
}

uint32_t Query::GetFlagSize(uint32_t to_check_flag) const {
	static constexpr auto uint8_queries = QUERY_OWNER | QUERY_IS_PUBLIC | QUERY_IS_HIDDEN;
	static constexpr auto uint32_queries = QUERY_CODE | QUERY_POSITION | QUERY_ALIAS | QUERY_TYPE
		| QUERY_LEVEL | QUERY_RANK | QUERY_ATTRIBUTE | QUERY_RACE | QUERY_ATTACK
		| QUERY_DEFENSE | QUERY_BASE_ATTACK | QUERY_BASE_DEFENSE | QUERY_REASON
		| QUERY_STATUS | QUERY_LSCALE | QUERY_RSCALE | QUERY_COVER;

	if((to_check_flag & uint8_queries) != 0)
		return sizeof(uint8_t);

	if((to_check_flag & uint32_queries) != 0)
		return sizeof(uint32_t);

	if((to_check_flag & (QUERY_REASON_CARD | QUERY_EQUIP_CARD)) != 0)
		return sizeof(uint16_t) + sizeof(uint64_t);

	switch(to_check_flag) {
	case QUERY_TARGET_CARD:
		return sizeof(uint32_t) + (target_cards.size() * (sizeof(uint16_t) + sizeof(uint64_t)));
	case QUERY_OVERLAY_CARD:
		return sizeof(uint32_t) + (overlay_cards.size() * sizeof(uint32_t));
	case QUERY_COUNTERS:
		return sizeof(uint32_t) + (counters.size() * sizeof(uint32_t));
	case QUERY_LINK:
		return sizeof(uint32_t) + sizeof(uint32_t);
	case QUERY_END:
	default:
		return 0;
	}
}

uint32_t Query::GetSize() const {
	uint32_t size = 0;
	if(onfield_skipped)
		return 0;
	for(uint64_t cur_flag = 1; cur_flag <= QUERY_END; cur_flag <<= 1) {
		if((flag & cur_flag) == 0)
			continue;
		size += GetFlagSize(cur_flag) + sizeof(uint32_t);
	}
	return size;
}

loc_info ReadLocInfo(const uint8_t*& p, bool compat) {
	loc_info info;
	info.controler = BufferIO::Read<uint8_t>(p);
	info.location = BufferIO::Read<uint8_t>(p);
	if(compat) {
		info.sequence = BufferIO::Read<uint8_t>(p);
		info.position = BufferIO::Read<uint8_t>(p);
	} else {
		info.sequence = BufferIO::Read<uint32_t>(p);
		info.position = BufferIO::Read<uint32_t>(p);
	}
	return info;
}

loc_info ReadLocInfo(uint8_t*& p, bool compat) {
	loc_info info;
	info.controler = BufferIO::Read<uint8_t>(p);
	info.location = BufferIO::Read<uint8_t>(p);
	if(compat) {
		info.sequence = BufferIO::Read<uint8_t>(p);
		info.position = BufferIO::Read<uint8_t>(p);
	} else {
		info.sequence = BufferIO::Read<uint32_t>(p);
		info.position = BufferIO::Read<uint32_t>(p);
	}
	return info;
}

PacketStream ParseMessages(OCG_Duel duel) {
	uint32_t message_len;
	auto msg = static_cast<uint8_t*>(OCG_DuelGetMessage(duel, &message_len));
	if(message_len)
		return PacketStream{ msg, message_len };
	return PacketStream{};
}

void QueryStream::Parse(const uint8_t* buff) {
	auto size = BufferIO::Read<uint32_t>(buff);
	const auto* current = buff;
	while(static_cast<uint32_t>(current - buff) < size)
		queries.emplace_back(Query::Token{}, current);
}

void QueryStream::ParseCompat(const uint8_t* buff, uint32_t len) {
	const auto* start = buff;
	while(static_cast<uint32_t>(buff - start) < len) {
		int size = BufferIO::Read<int32_t>(buff);
		queries.emplace_back(buff, true, size);
		buff += size - 4;
	}
}

uint32_t QueryStream::GetSize() const {
	uint32_t size = 0;
	for(const auto& query : queries)
		size += query.GetSize();
	return size;
}

void QueryStream::GenerateBuffer(std::vector<uint8_t>& buffer, bool check_hidden) const {
	auto prev_size = buffer.size();
	buffer.reserve(prev_size + sizeof(uint32_t) + GetSize());
	buffer.resize(prev_size + sizeof(uint32_t));
	for(const auto& query : queries)
		query.GenerateBuffer(buffer, false, check_hidden);
	uint32_t written_size = (buffer.size() - prev_size) - sizeof(uint32_t);
	memcpy(&buffer[prev_size], &written_size, sizeof(uint32_t));
}

void QueryStream::GeneratePublicBuffer(std::vector<uint8_t>& buffer) const {
	auto prev_size = buffer.size();
	buffer.reserve(prev_size + sizeof(uint32_t) + GetSize());
	buffer.resize(prev_size + sizeof(uint32_t));
	for(const auto& query : queries)
		query.GenerateBuffer(buffer, true, true);
	uint32_t written_size = (buffer.size() - prev_size) - sizeof(uint32_t);
	memcpy(&buffer[prev_size], &written_size, sizeof(uint32_t));
}

PacketStream::PacketStream(uint8_t* buff, uint32_t len) {
	auto* current = buff;
	while(static_cast<uint32_t>(current - buff) < len) {
		uint32_t size = BufferIO::Read<uint32_t>(current);
		packets.emplace_back(current, size - sizeof(uint8_t));
		current += size;
	}
}

}
