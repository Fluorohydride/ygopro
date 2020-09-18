#include "data_manager.h"
#include <fstream>
#include <fmt/format.h>
#include "bufferio.h"
#include "sqlite3.h"
#include "readonlymemvfs.h"
#include "logging.h"
#include "utils.h"
#include "common.h"

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";

std::string cur_database = "";

void DataManager::ClearLocaleTexts() {
	for(auto& val : indexes) {
		val.second.second = nullptr;
		if(val.second.first)
			val.second.first->_locale_strings = nullptr;
	}
	locales.clear();
}

bool DataManager::LoadLocaleDB(const path_string& _file, bool usebuffer) {
	sqlite3* pDB;
	cur_database = Utils::ToUTF8IfNeeded(_file);
	if(sqlite3_open_v2(cur_database.c_str(), &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
	return ParseLocaleDB(pDB);
}

bool DataManager::LoadDB(const path_string& _file, bool usebuffer) {
	cur_database = Utils::ToUTF8IfNeeded(_file);
	if(usebuffer) {
		std::ifstream db(_file, std::ifstream::binary);
		return LoadDBFromBuffer({ std::istreambuf_iterator<char>(db), std::istreambuf_iterator<char>() }, cur_database);
	}
	sqlite3* pDB;
	if(sqlite3_open_v2(cur_database.c_str(), &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
	return ParseDB(pDB);
}
bool DataManager::LoadDBFromBuffer(const std::vector<char>& buffer, const std::string& filename) {
	cur_database = filename;
	sqlite3* pDB;
	readonlymemvfs_init();
	set_mem_db((void*)buffer.data(), buffer.size());
	if(sqlite3_open_v2("0", &pDB, SQLITE_OPEN_READONLY, READONLY_MEM_VFS_NAME) != SQLITE_OK)
		return Error(pDB);
	return ParseDB(pDB);
}
bool DataManager::ParseDB(sqlite3* pDB) {
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id ORDER BY texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	auto indexesiterator = indexes.begin();
	int step = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if(step == SQLITE_ROW) {
			CardString cs{};
			CardDataC cd{};
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.ot = sqlite3_column_int(pStmt, 1);
			cd.alias = sqlite3_column_int(pStmt, 2);
			cd.setcodes_p = nullptr;
			auto setcodes = sqlite3_column_int64(pStmt, 3);
			for(int i = 0; i < 4; i++) {
				uint16_t setcode = (setcodes >> (i * 16)) & 0xffff;
				if(setcode)
					cd.setcodes.push_back(setcode);
			}
			if(cd.setcodes.size()) {
				cd.setcodes.push_back(0);
				cd.setcodes_p = cd.setcodes.data();
			}
			cd.type = sqlite3_column_int(pStmt, 4);
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defense = sqlite3_column_int(pStmt, 6);
			if(cd.type & TYPE_LINK) {
				cd.link_marker = cd.defense;
				cd.defense = 0;
			} else
				cd.link_marker = 0;
			int level = sqlite3_column_int(pStmt, 7);
			if(level < 0) {
				cd.level = -(level & 0xff);
			} else
				cd.level = level & 0xff;
			cd.lscale = (level >> 24) & 0xff;
			cd.rscale = (level >> 16) & 0xff;
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 12)) {
				cs.name = BufferIO::DecodeUTF8s(text);
			}
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 13)) {
				cs.text = BufferIO::DecodeUTF8s(text);
			}
			for(int i = 0; i < 16; ++i) {
				if(const char* text = (const char*)sqlite3_column_text(pStmt, i + 14)) {
					cs.desc[i] = BufferIO::DecodeUTF8s(text);
				}
			}
			CardString* localestring = nullptr;
			if(indexesiterator != indexes.end()) {
				while(indexesiterator != indexes.end() && indexesiterator->first < cd.code)
					indexesiterator++;
				if(indexesiterator != indexes.end() && indexesiterator->first == cd.code)
					localestring = indexesiterator->second.second;
			}
			auto ptr = &(cards[cd.code] = { std::move(cd), std::move(cs), localestring });
			indexes[cd.code] = { ptr, localestring };
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return true;
}
bool DataManager::ParseLocaleDB(sqlite3* pDB) {
	sqlite3_stmt* pStmt;
	const char* sql = "select * from texts ORDER BY texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	auto indexesiterator = indexes.begin();
	int step = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if(step == SQLITE_ROW) {
			CardString cs{};
			auto code = (uint32_t)sqlite3_column_int64(pStmt, 0);
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 1)) {
				cs.name = BufferIO::DecodeUTF8s(text);
			}
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 2)) {
				cs.text = BufferIO::DecodeUTF8s(text);
			}
			for(int i = 0; i < 16; ++i) {
				if(const char* text = (const char*)sqlite3_column_text(pStmt, i + 3)) {
					cs.desc[i] = BufferIO::DecodeUTF8s(text);
				}
			}
			CardDataM* card_data = nullptr;
			if(indexesiterator != indexes.end()) {
				while(indexesiterator != indexes.end() && indexesiterator->first < code)
					indexesiterator++;
				if(indexesiterator != indexes.end() && indexesiterator->first == code)
					card_data = indexesiterator->second.first;
			}
			auto ptr = &(locales[code] = std::move(cs));
			if(card_data) {
				card_data->_locale_strings = ptr;
			}
			indexes[code] = { card_data,ptr };
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return true;
}
bool DataManager::LoadStrings(const path_string& file) {
	std::ifstream string_file(file, std::ifstream::in);
	if(!string_file.is_open())
		return false;
	std::string str;
	while(std::getline(string_file, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if(str.empty() || str.at(0) != '!') {
			continue;
		}
		pos = str.find(' ');
		auto type = str.substr(1, pos - 1);
		str = str.substr(pos + 1);
		pos = str.find(' ');
		auto value = str.substr(0, pos);
		str = str.substr(pos + 1);
		try {
			if(type == "system") {
				_sysStrings.SetMain(std::stoi(value), BufferIO::DecodeUTF8s(str));
			} else {
				LocaleStringHelper* obj = nullptr;
				if(type == "victory")
					obj = &_victoryStrings;
				else if(type == "counter")
					obj = &_counterStrings;
				else if(type == "setname")
					obj = &_setnameStrings;
				obj->SetMain(std::stoi(value, 0, 16), BufferIO::DecodeUTF8s(str));
			}
		}
		catch(...) {}
	}
	string_file.close();
	return true;
}
bool DataManager::LoadLocaleStrings(const path_string& file) {
	std::ifstream string_file(file, std::ifstream::in);
	if(!string_file.is_open())
		return false;
	std::string str;
	while(std::getline(string_file, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if(str.empty() || str.at(0) != '!') {
			continue;
		}
		pos = str.find(' ');
		auto type = str.substr(1, pos - 1);
		str = str.substr(pos + 1);
		pos = str.find(' ');
		auto value = str.substr(0, pos);
		str = str.substr(pos + 1);
		try {
			if(type == "system") {
				_sysStrings.SetLocale(std::stoi(value), BufferIO::DecodeUTF8s(str));
			} else {
				LocaleStringHelper* obj = nullptr;
				if(type == "victory")
					obj = &_victoryStrings;
				else if(type == "counter")
					obj = &_counterStrings;
				else if(type == "setname")
					obj = &_setnameStrings;
				if(obj)
					obj->SetLocale(std::stoi(value, 0, 16), BufferIO::DecodeUTF8s(str));
			}
		}
		catch(...) {}
	}
	string_file.close();
	return true;
}
void DataManager::ClearLocaleStrings() {
	_sysStrings.ClearLocales();
	_victoryStrings.ClearLocales();
	_counterStrings.ClearLocales();
	_setnameStrings.ClearLocales();
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	ErrorLog(fmt::format("Error when loading database ({}): {}", cur_database, sqlite3_errmsg(pDB)));
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return false;
}
bool DataManager::GetData(uint32_t code, CardData* pData) {
	auto cdit = cards.find(code);
	if(cdit == cards.end())
		return false;
	if(pData)
		*pData = *((CardData*)&cdit->second._data);
	return true;
}
CardDataC* DataManager::GetCardData(uint32_t code) {
	auto it = cards.find(code);
	if(it != cards.end())
		return &it->second._data;
	return nullptr;
}
bool DataManager::GetString(uint32_t code, CardString* pStr) {
	auto csit = cards.find(code);
	if(csit == cards.end()) {
		pStr->name = unknown_string;
		pStr->text = unknown_string;
		return false;
	}
	*pStr = *csit->second.GetStrings();
	return true;
}
epro_wstringview DataManager::GetName(uint32_t code) {
	auto csit = cards.find(code);
	if(csit == cards.end() || csit->second.GetStrings()->name.empty())
		return unknown_string;
	return csit->second.GetStrings()->name;
}
epro_wstringview DataManager::GetText(uint32_t code) {
	auto csit = cards.find(code);
	if(csit == cards.end() || csit->second.GetStrings()->text.empty())
		return unknown_string;
	return csit->second.GetStrings()->text;
}
epro_wstringview DataManager::GetDesc(uint64_t strCode, bool compat) {
	uint32_t code = 0;
	uint32_t stringid = 0;
	if(compat) {
		if(strCode < 10000)
			return GetSysString(strCode);
		code = strCode >> 4;
		stringid = strCode & 0xf;
	} else {
		code = strCode >> 20;
		stringid = strCode & 0xfffff;
	}
	if(code == 0)
		return GetSysString(stringid);
	auto csit = cards.find(code);
	if(csit == cards.end() || csit->second.GetStrings()->desc[stringid].empty())
		return unknown_string;
	return csit->second.GetStrings()->desc[stringid];
}
epro_wstringview DataManager::GetSysString(uint32_t code) {
	auto csit = _sysStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
epro_wstringview DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
epro_wstringview DataManager::GetCounterName(uint32_t code) {
	auto csit = _counterStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
epro_wstringview DataManager::GetSetName(uint32_t code) {
	auto csit = _setnameStrings.GetLocale(code);
	if(!csit)
		return L"";
	return csit;
}
std::vector<uint32_t> DataManager::GetSetCode(std::vector<std::wstring>& setname) {
	std::vector<uint32_t> res;
	for(auto& string : _setnameStrings.map) {
		if(string.second.first.empty())
			continue;
		auto str = Utils::ToUpperNoAccents(string.second.second.size() ? string.second.second : string.second.first);
		if(str.find(L'|') != std::wstring::npos){
			for(auto& name : Utils::TokenizeString<std::wstring>(Utils::ToUpperNoAccents(str), L"|")) {
				if(Utils::ContainsSubstring(name, setname)) {
					res.push_back(string.first);
					break;
				}
			}
		} else {
			if(Utils::ContainsSubstring(str, setname))
				res.push_back(string.first);
		}
	}
	return res;
}
std::wstring DataManager::GetNumString(int num, bool bracket) {
	if(!bracket)
		return fmt::to_wstring(num);
	return fmt::format(L"({})", num);
}
epro_wstringview DataManager::FormatLocation(uint32_t location, int sequence) {
	if(location == 0x8) {
		if(sequence < 5)
			return GetSysString(1003);
		else if(sequence == 5)
			return GetSysString(1008);
		else
			return GetSysString(1009);
	}
	uint32_t filter = 1;
	int i = 1000;
	for(; filter != 0x100 && filter != location; filter <<= 1)
		++i;
	if(filter == location)
		return GetSysString(i);
	else
		return unknown_string;
}
std::wstring DataManager::FormatAttribute(uint32_t attribute) {
	std::wstring res;
	uint32_t filter = 1;
	int i = 1010;
	for(; filter != 0x80; filter <<= 1, ++i) {
		if(attribute & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i).data();
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatRace(uint32_t race, bool isSkill) {
	std::wstring res;
	uint32_t filter = 1;
	for(int i = isSkill ? 2100 : 1020; filter != 0x2000000; filter <<= 1, ++i) {
		if(race & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i).data();
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatType(uint32_t type) {
	std::wstring res;
	if(type & TYPE_SKILL)
		res += GetSysString(1077).data();
	if(type & TYPE_ACTION) {
		if (!res.empty())
			res += L"|";
		res += GetSysString(1078).data();
	}
	int i = 1050;
	for(uint32_t filter = 1; filter != TYPE_SKILL; filter <<= 1, ++i) {
		if(type & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i).data();
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatScope(uint32_t scope, bool hideOCGTCG) {
	static const std::map<int, int> SCOPES = {
		{SCOPE_OCG, 1900},
		{SCOPE_TCG, 1901},
		{SCOPE_ANIME, 1265},
		{SCOPE_ILLEGAL, 1266},
		{SCOPE_VIDEO_GAME, 1267},
		{SCOPE_CUSTOM, 1268},
		{SCOPE_SPEED, 1910},
		{SCOPE_PRERELEASE, 1903},
		{SCOPE_RUSH, 1911}
	};
	if (hideOCGTCG && scope == SCOPE_OCG_TCG) return L"";
	std::wstring buffer;
	for (const auto& tuple : SCOPES) {
		if (scope & tuple.first) {
			if (!buffer.empty()) {
				buffer += L'/';
			}
			buffer += GetSysString(tuple.second).data();
		}
	}
	return buffer;
}
std::wstring DataManager::FormatSetName(uint64_t setcode) {
	std::wstring res;
	for(int i = 0; i < 4; ++i) {
		auto name = GetSetName((setcode >> i * 16) & 0xffff);
		if(!res.empty() && !name.empty())
			res += L"|";
		res += name.data();
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatSetName(std::vector<uint16_t> setcodes) {
	std::wstring res;
	for(auto& setcode : setcodes) {
		auto name = GetSetName(setcode);
		if(!res.empty() && !name.empty())
			res += L"|";
		res += name.data();
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatLinkMarker(uint32_t link_marker) {
	std::wstring res;
	if(link_marker & LINK_MARKER_TOP_LEFT)
		res += L"[\u2196]";
	if(link_marker & LINK_MARKER_TOP)
		res += L"[\u2191]";
	if(link_marker & LINK_MARKER_TOP_RIGHT)
		res += L"[\u2197]";
	if(link_marker & LINK_MARKER_LEFT)
		res += L"[\u2190]";
	if(link_marker & LINK_MARKER_RIGHT)
		res += L"[\u2192]";
	if(link_marker & LINK_MARKER_BOTTOM_LEFT)
		res += L"[\u2199]";
	if(link_marker & LINK_MARKER_BOTTOM)
		res += L"[\u2193]";
	if(link_marker & LINK_MARKER_BOTTOM_RIGHT)
		res += L"[\u2198]";
	return res;
}
void DataManager::CardReader(void* payload, uint32_t code, CardData* data) {
	if(!static_cast<DataManager*>(payload)->GetData(code, (CardData*)data))
		memset(data, 0, sizeof(CardData));
}
bool is_skill(uint32_t type) {
	return (type & (TYPE_SKILL | TYPE_ACTION));
}
bool check_both_skills(uint32_t type1, uint32_t type2) {
	return is_skill(type1) && is_skill(type2);
}
bool check_either_skills(uint32_t type1, uint32_t type2) {
	return is_skill(type1) || is_skill(type2);
}
bool check_skills(CardDataC* p1, CardDataC* p2) {
	if(check_both_skills(p1->type, p2->type)) {
		if((p1->type & 0xfffffff8) == (p2->type & 0xfffffff8)) {
			return p1->code < p2->code;
		} else {
			return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
		}
	}
	return is_skill(p2->type);
}
bool card_sorter(CardDataC* p1, CardDataC* p2, bool(*sortoop)(CardDataC* p1, CardDataC* p2)) {
	if(check_either_skills(p1->type, p2->type))
		return check_skills(p1, p2);
	if((p1->type & 0x7) != (p2->type & 0x7))
		return (p1->type & 0x7) < (p2->type & 0x7);
	if((p1->type & 0x7) == 1) {
		return sortoop(p1, p2);
	}
	if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
		return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
	return p1->code < p2->code;
}
bool DataManager::deck_sort_lv(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2)->bool {
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		return p1->code < p2->code;
	});
}
bool DataManager::deck_sort_atk(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2)->bool {
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->code < p2->code;
	});
}
bool DataManager::deck_sort_def(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2)->bool {
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->code < p2->code;
	});
}
bool DataManager::deck_sort_name(CardDataC* p1, CardDataC* p2) {
	int res = gDataManager->GetName(p1->code).compare(gDataManager->GetName(p2->code));
	if(res != 0)
		return res < 0;
	return p1->code < p2->code;
}

}
