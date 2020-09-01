#include "core_utils.h"
#include "bufferio.h"
#include "dllinterface.h"
#include "common.h"

namespace CoreUtils {

#define PARSE_SINGLE(query,value) case query: {\
value = BufferIO::Read<decltype(value)>(current);\
break;\
}

void Query::Parse(char*& current) {
	//char* current = buff;
	flag = 0;
	for(;;) {
		uint16_t size = BufferIO::Read<uint16_t>(current);
		if(size == 0) {
			onfield_skipped = true;
			return;
		}
		uint32_t _flag = BufferIO::Read<uint32_t>(current);
		flag |= _flag;
		switch(_flag) {
			PARSE_SINGLE(QUERY_CODE,code)
			PARSE_SINGLE(QUERY_POSITION, position)
			PARSE_SINGLE(QUERY_ALIAS, alias)
			PARSE_SINGLE(QUERY_TYPE, type)
			PARSE_SINGLE(QUERY_LEVEL, level)
			PARSE_SINGLE(QUERY_RANK, rank)
			PARSE_SINGLE(QUERY_ATTRIBUTE, attribute)
			PARSE_SINGLE(QUERY_RACE, race)
			PARSE_SINGLE(QUERY_ATTACK, attack)
			PARSE_SINGLE(QUERY_DEFENSE, defense)
			PARSE_SINGLE(QUERY_BASE_ATTACK, base_attack)
			PARSE_SINGLE(QUERY_BASE_DEFENSE, base_defense)
			PARSE_SINGLE(QUERY_REASON, reason)
			PARSE_SINGLE(QUERY_OWNER, owner)
			PARSE_SINGLE(QUERY_STATUS, status)
			PARSE_SINGLE(QUERY_IS_PUBLIC, is_public)
			PARSE_SINGLE(QUERY_LSCALE, lscale)
			PARSE_SINGLE(QUERY_RSCALE, rscale)
			PARSE_SINGLE(QUERY_IS_HIDDEN, is_hidden)
			PARSE_SINGLE(QUERY_COVER, cover)
			case QUERY_REASON_CARD: {
				reason_card = ReadLocInfo(current, false);
				break;
			}
			case QUERY_EQUIP_CARD: {
				equip_card = ReadLocInfo(current, false);
				break;
			}
			case QUERY_TARGET_CARD: {
				uint32_t count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					target_cards.push_back(ReadLocInfo(current, false));
				break;
			}
			case QUERY_OVERLAY_CARD: {
				uint32_t count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					overlay_cards.push_back(BufferIO::Read<uint32_t>(current));
				break;
			}
			case QUERY_COUNTERS: {
				uint32_t count = BufferIO::Read<uint32_t>(current);
				for(uint32_t i = 0; i < count; i++)
					counters.push_back(BufferIO::Read<uint32_t>(current));
				break;
			}
			case QUERY_LINK: {
				link = BufferIO::Read<uint32_t>(current);
				link_marker = BufferIO::Read<uint32_t>(current);
				break;
			}
			case QUERY_END: {
				return;
			}
			default: {
				current += size - sizeof(uint32_t);
				break;
			}
		}
	}
}
#undef PARSE_SINGLE
#define PARSE_SINGLE(query,value) if(flag & query) {\
value = BufferIO::Read<uint32_t>(current);\
}

void Query::ParseCompat(char* current, int len) {
	if(len <= 8) {
		onfield_skipped = true;
		return;
	}
	flag = BufferIO::Read<int32_t>(current);
	PARSE_SINGLE(QUERY_CODE, code)
	if(flag & QUERY_POSITION)
		position = (BufferIO::Read<uint32_t>(current) >> 24) & 0xff;
	PARSE_SINGLE(QUERY_ALIAS, alias)
	PARSE_SINGLE(QUERY_TYPE, type)
	PARSE_SINGLE(QUERY_LEVEL, level)
	PARSE_SINGLE(QUERY_RANK, rank)
	PARSE_SINGLE(QUERY_ATTRIBUTE, attribute)
	PARSE_SINGLE(QUERY_RACE, race)
	PARSE_SINGLE(QUERY_ATTACK, attack)
	PARSE_SINGLE(QUERY_DEFENSE, defense)
	PARSE_SINGLE(QUERY_BASE_ATTACK, base_attack)
	PARSE_SINGLE(QUERY_BASE_DEFENSE, base_defense)
	PARSE_SINGLE(QUERY_REASON, reason)
	if(flag & QUERY_REASON_CARD)
		reason_card = ReadLocInfo(current, true);
	if(flag &  QUERY_EQUIP_CARD)
		equip_card = ReadLocInfo(current, true);
	if(flag & QUERY_TARGET_CARD) {
		uint32_t count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			target_cards.push_back(ReadLocInfo(current, true));
	}
	if(flag & QUERY_OVERLAY_CARD) {
		uint32_t count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			overlay_cards.push_back(BufferIO::Read<uint32_t>(current));
	}
	if(flag & QUERY_COUNTERS) {
		uint32_t count = BufferIO::Read<uint32_t>(current);
		for(uint32_t i = 0; i < count; i++)
			counters.push_back(BufferIO::Read<uint32_t>(current));
	}
	PARSE_SINGLE(QUERY_OWNER, owner)
	PARSE_SINGLE(QUERY_STATUS, status)
	PARSE_SINGLE(QUERY_LSCALE, lscale)
	PARSE_SINGLE(QUERY_RSCALE, rscale)
	if(flag & QUERY_LINK) {
		link = BufferIO::Read<uint32_t>(current);
		link_marker = BufferIO::Read<uint32_t>(current);
	}
}

template<typename T>
void insert_value(std::vector<uint8_t>& vec, const T& _val) {
	T val = _val;
	const auto vec_size = vec.size();
	const auto val_size = sizeof(T);
	vec.resize(vec_size + val_size);
	std::memcpy(&vec[vec_size], &val, val_size);
}
#define INSERT(query, val)if(_flag == query) insert_value<decltype(val)>(buffer, val);
void Query::GenerateBuffer(std::vector<uint8_t>& buffer, bool is_public, bool check_hidden) {
	if(onfield_skipped) {
		insert_value<uint16_t>(buffer, 0);
		return;
	}
	for(uint64_t _flag = 1; _flag <= QUERY_END; _flag <<= 1) {
		if((this->flag & _flag) != _flag)
			continue;
		if(((is_public || (check_hidden && ((this->flag & QUERY_IS_HIDDEN) && is_hidden))) && !IsPublicQuery(_flag))) {
			continue;
		}
		if((_flag == QUERY_REASON_CARD && reason_card.location == 0) ||
			(_flag == QUERY_EQUIP_CARD && equip_card.location == 0))
			continue;
		insert_value<uint16_t>(buffer, GetSize(_flag) + sizeof(uint32_t));
		insert_value<uint32_t>(buffer, _flag);
		/*if(((is_public || (check_hidden && ((this->flag & QUERY_IS_HIDDEN) && is_hidden))) && !IsPublicQuery(_flag))) {
			const auto vec_size = buffer.size();
			const auto flag_size = GetSize(_flag);
			buffer.resize(vec_size + flag_size);
			std::memset(&buffer[vec_size], 0, flag_size);
			continue;
		}*/
		INSERT(QUERY_CODE, code)
		INSERT(QUERY_POSITION, position)
		INSERT(QUERY_ALIAS, alias)
		INSERT(QUERY_TYPE, type)
		INSERT(QUERY_LEVEL, level)
		INSERT(QUERY_RANK, rank)
		INSERT(QUERY_ATTRIBUTE, attribute)
		INSERT(QUERY_RACE, race)
		INSERT(QUERY_ATTACK, attack)
		INSERT(QUERY_DEFENSE, defense)
		INSERT(QUERY_BASE_ATTACK, base_attack)
		INSERT(QUERY_BASE_DEFENSE, base_defense)
		INSERT(QUERY_REASON, reason)
		INSERT(QUERY_OWNER, owner)
		INSERT(QUERY_STATUS, status)
		INSERT(QUERY_IS_PUBLIC, this->is_public)
		INSERT(QUERY_LSCALE, lscale)
		INSERT(QUERY_RSCALE, rscale)
		INSERT(QUERY_IS_HIDDEN, is_hidden)
		INSERT(QUERY_COVER, cover)
		if(_flag == QUERY_REASON_CARD || _flag == QUERY_EQUIP_CARD) {
			auto& info = (_flag == QUERY_REASON_CARD) ? reason_card : equip_card;
			insert_value<uint8_t>(buffer, info.controler);
			insert_value<uint8_t>(buffer, info.location);
			insert_value<uint32_t>(buffer, info.sequence);
			insert_value<uint32_t>(buffer, info.position);
		}
		if(_flag == QUERY_TARGET_CARD) {
			insert_value<uint32_t>(buffer, target_cards.size());
			for(auto& info : target_cards) {
				insert_value<uint8_t>(buffer, info.controler);
				insert_value<uint8_t>(buffer, info.location);
				insert_value<uint32_t>(buffer, info.sequence);
				insert_value<uint32_t>(buffer, info.position);
			}
		}
		if(_flag == QUERY_OVERLAY_CARD || _flag == QUERY_COUNTERS) {
			auto& vec = (_flag == QUERY_OVERLAY_CARD) ? overlay_cards : counters;
			insert_value<uint32_t>(buffer, vec.size());
			for(auto& val : vec) {
				insert_value<uint32_t>(buffer, val);
			}
		}
		if(_flag == QUERY_LINK) {
			insert_value<uint32_t>(buffer, link);
			insert_value<uint32_t>(buffer, link_marker);
		}
	}
}

bool Query::IsPublicQuery(uint32_t flag) {
	if(((this->flag & QUERY_IS_PUBLIC) && is_public) || ((this->flag & QUERY_POSITION) && (position & POS_FACEUP)))
		return true;
	switch(flag) {
	case QUERY_CODE:
	case QUERY_ALIAS:
	case QUERY_TYPE:
	case QUERY_LEVEL:
	case QUERY_RANK:
	case QUERY_ATTRIBUTE:
	case QUERY_RACE:
	case QUERY_ATTACK:
	case QUERY_DEFENSE:
	case QUERY_BASE_ATTACK:
	case QUERY_BASE_DEFENSE:
	case QUERY_STATUS:
	case QUERY_LSCALE:
	case QUERY_RSCALE:
	case QUERY_LINK:
		return false;
	}
	return true;
}

uint32_t Query::GetSize(uint32_t flag) {
	switch(flag) {
	case QUERY_OWNER:
	case QUERY_IS_PUBLIC:
	case QUERY_IS_HIDDEN: {
		return sizeof(uint8_t);
	}
	case QUERY_CODE:
	case QUERY_POSITION:
	case QUERY_ALIAS:
	case QUERY_TYPE:
	case QUERY_LEVEL:
	case QUERY_RANK:
	case QUERY_ATTRIBUTE:
	case QUERY_RACE:
	case QUERY_ATTACK:
	case QUERY_DEFENSE:
	case QUERY_BASE_ATTACK:
	case QUERY_BASE_DEFENSE:
	case QUERY_REASON:
	case QUERY_STATUS:
	case QUERY_LSCALE:
	case QUERY_RSCALE:
	case QUERY_COVER: {
		return sizeof(uint32_t);
		break;
	}
	case QUERY_REASON_CARD:
	case QUERY_EQUIP_CARD: {
		return sizeof(uint16_t) + sizeof(uint64_t);
		break;
	}
	case QUERY_TARGET_CARD: {
		return sizeof(uint32_t) + (target_cards.size() * (sizeof(uint16_t) + sizeof(uint64_t)));
		break;
	}
	case QUERY_OVERLAY_CARD: {
		return sizeof(uint32_t) + (overlay_cards.size() * sizeof(uint32_t));
		break;
	}
	case QUERY_COUNTERS: {
		return sizeof(uint32_t) + (counters.size() * sizeof(uint32_t));
		break;
	}
	case QUERY_LINK: {
		return sizeof(uint32_t) + sizeof(uint32_t);
		break;
	}
	case QUERY_END:
		return 0;
		break;
	}
	return 0;
}

uint32_t Query::GetSize() {
	uint32_t size = 0;
	if(onfield_skipped)
		return 0;
	for(uint64_t _flag = 1; _flag <= QUERY_END; _flag <<= 1) {
		if((this->flag & _flag) != _flag)
			continue;
		size += GetSize(_flag) + sizeof(uint32_t);
	}
	return size;
}

loc_info ReadLocInfo(char*& p, bool compat) {
	loc_info info;
	info.controler = BufferIO::Read<uint8_t>(p);
	info.location = BufferIO::Read<uint8_t>(p);
	if(compat) {
		info.sequence = BufferIO::Read<uint8_t>(p);
		info.position = BufferIO::Read<uint8_t>(p);
	} else {
		info.sequence = BufferIO::Read<int32_t>(p);
		info.position = BufferIO::Read<int32_t>(p);
	}
	return info;
}

CoreUtils::PacketStream ParseMessages(OCG_Duel duel) {
	uint32_t message_len;
	auto msg = OCG_DuelGetMessage(duel, &message_len);
	if(message_len)
		return PacketStream((char*)msg, message_len);
	else
		return PacketStream();
}

void QueryStream::Parse(char*& buff) {
	uint32_t size = BufferIO::Read<uint32_t>(buff);
	char* current = buff;
	while((uint32_t)(current - buff) < size) {
		queries.emplace_back(current);
	}
}

void QueryStream::ParseCompat(char*& buff, int len) {
	char* start = buff;
	while((buff - start) < len) {
		int size = BufferIO::Read<int32_t>(buff);
		queries.emplace_back(buff, true, size);
		buff += size - 4;
	}
}

void QueryStream::GenerateBuffer(std::vector<uint8_t>& buffer, bool check_hidden) {
	std::vector<uint8_t> tmp_buffer;
	for(auto& query : queries) {
		query.GenerateBuffer(tmp_buffer, false, check_hidden);
	}
	insert_value<uint32_t>(buffer, tmp_buffer.size());
	buffer.insert(buffer.end(), tmp_buffer.begin(), tmp_buffer.end());
}

void QueryStream::GeneratePublicBuffer(std::vector<uint8_t>& buffer) {
	std::vector<uint8_t> tmp_buffer;
	for(auto& query : queries) {
		query.GenerateBuffer(tmp_buffer, true, true);
	}
	insert_value<uint32_t>(buffer, tmp_buffer.size());
	buffer.insert(buffer.end(), tmp_buffer.begin(), tmp_buffer.end());
}

PacketStream::PacketStream(char* buff, int len) {
	char* current = buff;
	while((current - buff) < len) {
		uint32_t size = BufferIO::Read<uint32_t>(current);
		packets.emplace_back(current, size - sizeof(uint8_t));
		current += size;
	}
}

}
