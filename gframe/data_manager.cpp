#include "data_manager.h"
#include "sqlite3.h"
#include <fstream>
#include "readonlymemvfs.h"
#include "logging.h"

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";

void DataManager::ClearLocaleTexts() {
	for(auto& val : indexes) {
		val.second.second = nullptr;
		if(val.second.first)
			val.second.first->_locale_strings = nullptr;
	}
	locales.clear();
}

bool DataManager::LoadLocaleDB(const path_string & file, bool usebuffer) {
	sqlite3* pDB;
	if(sqlite3_open_v2(Utils::ToUTF8IfNeeded(file).c_str(), &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
	return ParseLocaleDB(pDB);
}

bool DataManager::LoadDB(const path_string& file, bool usebuffer) {
	if(usebuffer) {
		std::ifstream db(file, std::ifstream::binary);
		return LoadDBFromBuffer({ std::istreambuf_iterator<char>(db), std::istreambuf_iterator<char>() });
	}
	sqlite3* pDB;
	if(sqlite3_open_v2(Utils::ToUTF8IfNeeded(file).c_str(), &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
	return ParseDB(pDB);
}
bool DataManager::LoadDBFromBuffer(const std::vector<char>& buffer) {
	sqlite3* pDB;
	readonlymemvfs_init();
	set_mem_db((void*)buffer.data(), buffer.size());
	if(sqlite3_open_v2("0", &pDB, SQLITE_OPEN_READONLY, READONLY_MEM_VFS_NAME) != SQLITE_OK)
		return Error(pDB);
	return ParseDB(pDB);
}
bool DataManager::ParseDB(sqlite3 * pDB) {
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
bool DataManager::ParseLocaleDB(sqlite3 * pDB) {
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
			auto code = sqlite3_column_int(pStmt, 0);
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
bool DataManager::LoadLocaleStrings(const path_string & file) {
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
	auto error = sqlite3_errmsg(pDB);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	ErrorLog(error);
	return false;
}
bool DataManager::GetData(uint32 code, CardData* pData) {
	auto cdit = cards.find(code);
	if(cdit == cards.end())
		return false;
	if(pData)
		*pData = *((CardData*)&cdit->second._data);
	return true;
}
CardDataC* DataManager::GetCardData(uint32 code) {
	auto it = cards.find(code);
	if(it != cards.end())
		return &it->second._data;
	return nullptr;
}
bool DataManager::GetString(uint32 code, CardString* pStr) {
	auto csit = cards.find(code);
	if(csit == cards.end()) {
		pStr->name = unknown_string;
		pStr->text = unknown_string;
		return false;
	}
	*pStr = *csit->second.GetStrings();
	return true;
}
std::wstring DataManager::GetName(uint32 code) {
	auto csit = cards.find(code);
	if(csit == cards.end() || csit->second.GetStrings()->name.empty())
		return unknown_string;
	return csit->second.GetStrings()->name;
}
std::wstring DataManager::GetText(uint32 code) {
	auto csit = cards.find(code);
	if(csit == cards.end() || csit->second.GetStrings()->text.empty())
		return unknown_string;
	return csit->second.GetStrings()->text;
}
std::wstring DataManager::GetDesc(uint64 strCode, bool compat) {
	uint32 code = 0;
	uint32 stringid = 0;
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
std::wstring DataManager::GetSysString(uint32 code) {
	auto csit = _sysStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
std::wstring DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
std::wstring DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.GetLocale(code);
	if(!csit)
		return unknown_string;
	return csit;
}
std::wstring DataManager::GetSetName(int code) {
	auto csit = _setnameStrings.GetLocale(code);
	if(!csit)
		return L"";
	return csit;
}
std::vector<unsigned int> DataManager::GetSetCode(std::vector<std::wstring>& setname) {
	std::vector<unsigned int> res;
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
std::wstring DataManager::FormatLocation(int location, int sequence) {
	if(location == 0x8) {
		if(sequence < 5)
			return GetSysString(1003);
		else if(sequence == 5)
			return GetSysString(1008);
		else
			return GetSysString(1009);
	}
	unsigned filter = 1;
	int i = 1000;
	for(; filter != 0x100 && filter != location; filter <<= 1)
		++i;
	if(filter == location)
		return GetSysString(i);
	else
		return unknown_string;
}
std::wstring DataManager::FormatAttribute(int attribute) {
	std::wstring res;
	unsigned filter = 1;
	int i = 1010;
	for(; filter != 0x80; filter <<= 1, ++i) {
		if(attribute & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i);
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatRace(int race, bool isSkill) {
	std::wstring res;
	unsigned filter = 1;
	for(int i = isSkill ? 2100 : 1020; filter != 0x2000000; filter <<= 1, ++i) {
		if(race & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i);
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatType(int type) {
	std::wstring res;
	if(type & TYPE_SKILL)
		res += GetSysString(1077);
	if(type & TYPE_ACTION) {
		if (!res.empty())
			res += L"|";
		res += GetSysString(1078);
	}
	int i = 1050;
	for(unsigned filter = 1; filter != TYPE_SKILL; filter <<= 1, ++i) {
		if(type & filter) {
			if(!res.empty())
				res += L"|";
			res += GetSysString(i);
		}
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatScope(int scope, bool hideOCGTCG) {
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
				buffer += L"/";
			}
			buffer += GetSysString(tuple.second);
		}
	}
	return buffer;
}
std::wstring DataManager::FormatSetName(unsigned long long setcode) {
	std::wstring res;
	for(int i = 0; i < 4; ++i) {
		auto name = GetSetName((setcode >> i * 16) & 0xffff);
		if(!res.empty() && !name.empty())
			res += L"|";
		res += name;
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatSetName(std::vector<uint16> setcodes) {
	std::wstring res;
	for(auto& setcode : setcodes) {
		auto name = GetSetName(setcode);
		if(!res.empty() && !name.empty())
			res += L"|";
		res += name;
	}
	if(res.empty())
		return unknown_string;
	return res;
}
std::wstring DataManager::FormatLinkMarker(int link_marker) {
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
void DataManager::CardReader(void* payload, int code, CardData* data) {
	if(!static_cast<DataManager*>(payload)->GetData(code, (CardData*)data))
		memset(data, 0, sizeof(CardData));
}

}
