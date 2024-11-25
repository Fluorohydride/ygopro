#include "data_manager.h"
#include "game.h"
#include <stdio.h>
#include "spmemvfs/spmemvfs.h"

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
byte DataManager::scriptBuffer[0x20000];
IFileSystem* DataManager::FileSystem;
DataManager dataManager;

DataManager::DataManager() : _datas(32768), _strings(32768) {
	extra_setcode = { {8512558u, {0x8f, 0x54, 0x59, 0x82, 0x13a}}, };
}
bool DataManager::ReadDB(sqlite3* pDB, bool expansion) {
	sqlite3_stmt* pStmt{};
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if (sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	wchar_t strBuffer[4096];
	int step = 0;
	do {
		CardDataC cd;
		CardString cs;
		step = sqlite3_step(pStmt);
		if (step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if (step == SQLITE_ROW) {
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.ot = sqlite3_column_int(pStmt, 1);
			cd.alias = sqlite3_column_int(pStmt, 2);
			auto setcode = sqlite3_column_int64(pStmt, 3);
			if (setcode) {
				auto it = extra_setcode.find(cd.code);
				if (it != extra_setcode.end()) {
					int len = it->second.size();
					if (len > SIZE_SETCODE)
						len = SIZE_SETCODE;
					if (len)
						std::memcpy(cd.setcode, it->second.data(), len * sizeof(uint16_t));
				}
				else
					cd.set_setcode(setcode);
			}
			cd.type = sqlite3_column_int(pStmt, 4);
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defense = sqlite3_column_int(pStmt, 6);
			if (cd.type & TYPE_LINK) {
				cd.link_marker = cd.defense;
				cd.defense = 0;
			}
			else
				cd.link_marker = 0;
			unsigned int level = sqlite3_column_int(pStmt, 7);
			cd.level = level & 0xff;
			cd.lscale = (level >> 24) & 0xff;
			cd.rscale = (level >> 16) & 0xff;
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			_datas[cd.code] = cd;
			if (expansion) _expansionDatas.push_back(cd.code);
			if (const char* text = (const char*)sqlite3_column_text(pStmt, 12)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.name = strBuffer;
			}
			if (const char* text = (const char*)sqlite3_column_text(pStmt, 13)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.text = strBuffer;
			}
			for (int i = 0; i < 16; ++i) {
				if (const char* text = (const char*)sqlite3_column_text(pStmt, i + 14)) {
					BufferIO::DecodeUTF8(text, strBuffer);
					cs.desc[i] = strBuffer;
				}
			}
			_strings[cd.code] = cs;
		}
	} while (step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	return true;
}
bool DataManager::LoadDB(const wchar_t* wfile, bool expansion) {
	char file[256];
	BufferIO::EncodeUTF8(wfile, file);
#ifdef _WIN32
	IReadFile* reader = FileSystem->createAndOpenFile(wfile);
#else
	IReadFile* reader = FileSystem->createAndOpenFile(file);
#endif
	if(reader == NULL)
		return false;
	spmemvfs_db_t db;
	spmembuffer_t* mem = (spmembuffer_t*)calloc(sizeof(spmembuffer_t), 1);
	spmemvfs_env_init();
	mem->total = mem->used = reader->getSize();
	mem->data = (char*)malloc(mem->total + 1);
	reader->read(mem->data, mem->total);
	reader->drop();
	(mem->data)[mem->total] = '\0';
	bool ret{};
	if (spmemvfs_open_db(&db, file, mem) != SQLITE_OK)
		ret = Error(db.handle);
	else
		ret = ReadDB(db.handle, expansion);
	spmemvfs_close_db(&db);
	spmemvfs_env_fini();
	return ret;
}
bool DataManager::LoadStrings(const char* file, bool expansion) {
	FILE* fp = fopen(file, "r");
	if(!fp)
		return false;
	char linebuf[TEXT_LINE_SIZE]{};
	while(fgets(linebuf, sizeof linebuf, fp)) {
		ReadStringConfLine(linebuf, expansion);
	}
	fclose(fp);
	return true;
}
bool DataManager::LoadStrings(IReadFile* reader, bool expansion) {
	char ch{};
	std::string linebuf;
	while (reader->read(&ch, 1)) {
		if (ch == '\0')
			break;
		linebuf.push_back(ch);
		if (ch == '\n' || linebuf.size() >= TEXT_LINE_SIZE - 1) {
			ReadStringConfLine(linebuf.data(), expansion);
			linebuf.clear();
		}
	}
	reader->drop();
	return true;
}
void DataManager::ReadStringConfLine(const char* linebuf, bool expansion) {
	if(linebuf[0] != '!')
		return;
	char strbuf[TEXT_LINE_SIZE]{};
	int value{};
	wchar_t strBuffer[4096]{};
	if (sscanf(linebuf, "!%63s", strbuf) != 1)
		return;
	if(!std::strcmp(strbuf, "system")) {
		if (sscanf(&linebuf[7], "%d %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_sysStrings[value] = strBuffer;
		if (expansion) _expansionStrings.push_back(value);
	} else if(!std::strcmp(strbuf, "victory")) {
		if (sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_victoryStrings[value] = strBuffer;
		if (expansion) _expansionStrings.push_back(value);
	} else if(!std::strcmp(strbuf, "counter")) {
		if (sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_counterStrings[value] = strBuffer;
		if (expansion) _expansionStrings.push_back(value);
	} else if(!std::strcmp(strbuf, "setname")) {
		//using tab for comment
		if (sscanf(&linebuf[8], "%x %240[^\t\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_setnameStrings[value] = strBuffer;
		if (expansion) _expansionStrings.push_back(value);
	}
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	errmsg[0] = '\0';
	std::strncat(errmsg, sqlite3_errmsg(pDB), sizeof errmsg - 1);
	if(pStmt)
		sqlite3_finalize(pStmt);
	return false;
}
code_pointer DataManager::GetCodePointer(unsigned int code) const {
	return _datas.find(code);
}
string_pointer DataManager::GetStringPointer(unsigned int code) const {
	return _strings.find(code);
}
code_pointer DataManager::datas_begin() {
	return _datas.cbegin();
}
code_pointer DataManager::datas_end() {
	return _datas.cend();
}
string_pointer DataManager::strings_begin() {
	return _strings.cbegin();
}
string_pointer DataManager::strings_end() {
	return _strings.cend();
}
bool DataManager::GetData(unsigned int code, CardData* pData) const {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if (pData) {
		*pData = cdit->second;
	}
	return true;
}
bool DataManager::GetString(unsigned int code, CardString* pStr) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = unknown_string;
		pStr->text = unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
const wchar_t* DataManager::GetName(unsigned int code) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.name.empty())
		return csit->second.name.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetText(unsigned int code) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.text.empty())
		return csit->second.text.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(unsigned int strCode) const {
	if (strCode < (MIN_CARD_ID << 4))
		return GetSysString(strCode);
	unsigned int code = (strCode >> 4) & 0x0fffffff;
	unsigned int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.desc[offset].empty())
		return csit->second.desc[offset].c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetSysString(int code) const {
	if (code < 0 || code > MAX_STRING_ID)
		return unknown_string;
	auto csit = _sysStrings.find(code);
	if(csit == _sysStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetVictoryString(int code) const {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetCounterName(int code) const {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetSetName(int code) const {
	auto csit = _setnameStrings.find(code);
	if(csit == _setnameStrings.end())
		return nullptr;
	return csit->second.c_str();
}
std::vector<unsigned int> DataManager::GetSetCodes(std::wstring setname) const {
	std::vector<unsigned int> matchingCodes;
	for(auto csit = _setnameStrings.begin(); csit != _setnameStrings.end(); ++csit) {
		auto xpos = csit->second.find_first_of(L'|');//setname|another setname or extra info
		if(setname.size() < 2) {
			if(csit->second.compare(0, xpos, setname) == 0
				|| csit->second.compare(xpos + 1, csit->second.length(), setname) == 0)
				matchingCodes.push_back(csit->first);
		} else {
			if(csit->second.substr(0, xpos).find(setname) != std::wstring::npos
				|| csit->second.substr(xpos + 1).find(setname) != std::wstring::npos) {
				matchingCodes.push_back(csit->first);
			}
		}
	}
	return matchingCodes;
}
std::wstring DataManager::GetNumString(int num, bool bracket) const {
	if(!bracket)
		return std::to_wstring(num);
	std::wstring numBuffer{ L"(" };
	numBuffer.append(std::to_wstring(num));
	numBuffer.push_back(L')');
	return numBuffer;
}
const wchar_t* DataManager::FormatLocation(int location, int sequence) const {
	if(location == LOCATION_SZONE) {
		if(sequence < 5)
			return GetSysString(1003);
		else if(sequence == 5)
			return GetSysString(1008);
		else
			return GetSysString(1009);
	}
	int i = 1000;
	int string_id = 0;
	for (unsigned filter = LOCATION_DECK; filter <= LOCATION_PZONE; filter <<= 1, ++i) {
		if (filter == location) {
			string_id = i;
			break;
		}
	}
	if (string_id)
		return GetSysString(string_id);
	else
		return unknown_string;
}
std::wstring DataManager::FormatAttribute(unsigned int attribute) const {
	std::wstring buffer;
	for (int i = 0; i < ATTRIBUTES_COUNT; ++i) {
		if (attribute & (0x1U << i)) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(1010 + i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatRace(unsigned int race) const {
	std::wstring buffer;
	for(int i = 0; i < RACES_COUNT; ++i) {
		if(race & (0x1U << i)) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(1020 + i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatType(unsigned int type) const {
	std::wstring buffer;
	int i = 1050;
	for (unsigned filter = TYPE_MONSTER; filter <= TYPE_LINK; filter <<= 1, ++i) {
		if (type & filter) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatSetName(const uint16_t setcode[]) const {
	std::wstring buffer;
	for(int i = 0; i < 10; ++i) {
		if (!setcode[i])
			break;
		const wchar_t* setname = GetSetName(setcode[i]);
		if(setname) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(setname);
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatLinkMarker(unsigned int link_marker) const {
	std::wstring buffer;
	if (link_marker & LINK_MARKER_TOP_LEFT)
		buffer.append(L"[\u2196]");
	if (link_marker & LINK_MARKER_TOP)
		buffer.append(L"[\u2191]");
	if (link_marker & LINK_MARKER_TOP_RIGHT)
		buffer.append(L"[\u2197]");
	if (link_marker & LINK_MARKER_LEFT)
		buffer.append(L"[\u2190]");
	if (link_marker & LINK_MARKER_RIGHT)
		buffer.append(L"[\u2192]");
	if (link_marker & LINK_MARKER_BOTTOM_LEFT)
		buffer.append(L"[\u2199]");
	if (link_marker & LINK_MARKER_BOTTOM)
		buffer.append(L"[\u2193]");
	if (link_marker & LINK_MARKER_BOTTOM_RIGHT)
		buffer.append(L"[\u2198]");
	return buffer;
}
uint32 DataManager::CardReader(uint32 code, card_data* pData) {
	if (!dataManager.GetData(code, pData))
		pData->clear();
	return 0;
}
byte* DataManager::ScriptReaderEx(const char* script_name, int* slen) {
	// default script name: ./script/c%d.lua
	char first[256]{};
	char second[256]{};
	if(mainGame->gameConf.prefer_expansion_script) {
		snprintf(first, sizeof first, "expansions/%s", script_name + 2);
		snprintf(second, sizeof second, "%s", script_name + 2);
	} else {
		snprintf(first, sizeof first, "%s", script_name + 2);
		snprintf(second, sizeof second, "expansions/%s", script_name + 2);
	}
	if(ScriptReader(first, slen))
		return scriptBuffer;
	else
		return ScriptReader(second, slen);
}
byte* DataManager::ScriptReader(const char* script_name, int* slen) {
#ifdef _WIN32
	wchar_t fname[256]{};
	BufferIO::DecodeUTF8(script_name, fname);
	IReadFile* reader = FileSystem->createAndOpenFile(fname);
#else
	IReadFile* reader = FileSystem->createAndOpenFile(script_name);
#endif
	if (!reader)
		return nullptr;
	size_t size = reader->getSize();
	if (size > sizeof scriptBuffer) {
		reader->drop();
		return nullptr;
	}
	reader->read(scriptBuffer, size);
	reader->drop();
	*slen = (int)size;
	return scriptBuffer;
}

}
