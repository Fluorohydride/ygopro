#include "data_manager.h"
#include "readonlymemvfs.h"
#include "game.h"
#include <stdio.h>
#include <fstream>

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
DataManager dataManager;

bool DataManager::LoadDB(const std::string& file, bool usebuffer) {
	if(usebuffer) {
		std::ifstream db(Utils::ParseFilename(file), std::ifstream::binary);
		return LoadDBFromBuffer({ std::istreambuf_iterator<char>(db), std::istreambuf_iterator<char>() });
	}
	sqlite3* pDB;
	if(sqlite3_open_v2(file.c_str(), &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
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
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	CardString cs;
	int step = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if(step == SQLITE_ROW) {
			CardDataC* cd = new CardDataC();
			cd->code = sqlite3_column_int(pStmt, 0);
			cd->ot = sqlite3_column_int(pStmt, 1);
			cd->alias = sqlite3_column_int(pStmt, 2);
			cd->setcode = sqlite3_column_int64(pStmt, 3);
			cd->type = sqlite3_column_int(pStmt, 4);
			cd->attack = sqlite3_column_int(pStmt, 5);
			cd->defense = sqlite3_column_int(pStmt, 6);
			if(cd->type & TYPE_LINK) {
				cd->link_marker = cd->defense;
				cd->defense = 0;
			} else
				cd->link_marker = 0;
			int level = sqlite3_column_int(pStmt, 7);
			if(level < 0) {
				cd->level = -(level & 0xff);
			} else
				cd->level = level & 0xff;
			cd->lscale = (level >> 24) & 0xff;
			cd->rscale = (level >> 16) & 0xff;
			cd->race = sqlite3_column_int(pStmt, 8);
			cd->attribute = sqlite3_column_int(pStmt, 9);
			cd->category = sqlite3_column_int(pStmt, 10);
			_datas[cd->code] = cd;
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
			_strings.emplace(cd->code, cs);
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return true;
}
bool DataManager::LoadStrings(const std::string& file) {
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
		if(type == "system")
			_sysStrings[std::stoi(value)] = BufferIO::DecodeUTF8s(str);
		else if(type == "victory")
			_victoryStrings[std::stoi(value, 0, 16)] = BufferIO::DecodeUTF8s(str);
		else if(type == "counter")
			_counterStrings[std::stoi(value, 0, 16)] = BufferIO::DecodeUTF8s(str);
		else if(type == "setname")
			_setnameStrings[std::stoi(value, 0, 16)] = BufferIO::DecodeUTF8s(str);
	}
	string_file.close();
	return true;
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	auto error = sqlite3_errmsg(pDB);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	mainGame->ErrorLog(error);
	return false;
}
bool DataManager::GetData(int code, CardData* pData) {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if(pData)
		*pData = *((CardData*)cdit->second);
	return true;
}
CardDataC* DataManager::GetCardData(int code) {
	auto it = _datas.find(code);
	if(it != _datas.end())
		return it->second;
	return nullptr;
}
bool DataManager::GetString(int code, CardString* pStr) {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = unknown_string;
		pStr->text = unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
std::wstring DataManager::GetName(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end() || csit->second.name.empty())
		return unknown_string;
	return csit->second.name;
}
std::wstring DataManager::GetText(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end() || csit->second.text.empty())
		return unknown_string;
	return csit->second.text;
}
std::wstring DataManager::GetDesc(u64 strCode) {
	if(strCode < 10000)
		return GetSysString(strCode);
	u64 code = strCode >> 4;
	int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end() || csit->second.desc[offset].empty())
		return unknown_string;
	return csit->second.desc[offset];
}
std::wstring DataManager::GetSysString(int code) {
	if(code < 0 || code >= 2048)
		return unknown_string;
	auto csit = _sysStrings.find(code);
	if(csit == _sysStrings.end() || csit->second.empty())
		return unknown_string;
	return csit->second;
}
std::wstring DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end() || csit->second.empty())
		return unknown_string;
	return csit->second;
}
std::wstring DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end() || csit->second.empty())
		return unknown_string;
	return csit->second;
}
std::wstring DataManager::GetSetName(int code) {
	auto csit = _setnameStrings.find(code);
	if(csit == _setnameStrings.end() || csit->second.empty())
		return L"";
	return csit->second;
}
std::vector<unsigned int> DataManager::GetSetCode(std::vector<std::wstring>& setname) {
	std::vector<unsigned int> res;
	for(auto& string : _setnameStrings) {
		auto xpos = string.second.find_first_of(L'|');//setname|extra info
		if(Game::CompareStrings(string.second.substr(0, xpos), setname, true))
			res.push_back(string.first);
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
std::wstring DataManager::FormatRace(int race) {
	std::wstring res;
	unsigned filter = 1;
	int i = 1020;
	for(; filter != 0x2000000; filter <<= 1, ++i) {
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
	unsigned filter = 1;
	int i = 1050;
	for(; filter != 0x8000000; filter <<= 1, ++i) {
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
std::wstring DataManager::FormatLinkMarker(int link_marker) {
	std::wstring res;
	if(link_marker & LINK_MARKER_TOP_LEFT)
		res+=L"[\u2196]";
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
int DataManager::CardReader(int code, void* pData) {
	if(!dataManager.GetData(code, (CardData*)pData))
		memset(pData, 0, sizeof(CardData));
	return 0;
}

}
