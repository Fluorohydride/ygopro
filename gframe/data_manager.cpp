#include "data_manager.h"
#include "game.h"
#include <stdio.h>

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
byte DataManager::scriptBuffer[0x20000];
IFileSystem* DataManager::FileSystem;
DataManager dataManager;

DataManager::DataManager() : _datas(16384), _strings(16384) {
	datas_begin = _datas.begin();
	datas_end = _datas.end();
	strings_begin = _strings.begin();
	strings_end = _strings.end();
	extra_setcode = { {8512558u, {0x8f, 0x54, 0x59, 0x82, 0x13a}}, };
}
bool DataManager::LoadDB(const wchar_t* wfile) {
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
	if(spmemvfs_open_db(&db, file, mem) != SQLITE_OK)
		return Error(&db);
	sqlite3* pDB = db.handle;
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(&db);
	wchar_t strBuffer[4096];
	int step = 0;
	do {
		CardDataC cd;
		CardString cs;
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(&db, pStmt);
		else if(step == SQLITE_ROW) {
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
			if(cd.type & TYPE_LINK) {
				cd.link_marker = cd.defense;
				cd.defense = 0;
			} else
				cd.link_marker = 0;
			unsigned int level = sqlite3_column_int(pStmt, 7);
			cd.level = level & 0xff;
			cd.lscale = (level >> 24) & 0xff;
			cd.rscale = (level >> 16) & 0xff;
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			_datas[cd.code] = cd;
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 12)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.name = strBuffer;
			}
			if(const char* text = (const char*)sqlite3_column_text(pStmt, 13)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.text = strBuffer;
			}
			for(int i = 0; i < 16; ++i) {
				if(const char* text = (const char*)sqlite3_column_text(pStmt, i + 14)) {
					BufferIO::DecodeUTF8(text, strBuffer);
					cs.desc[i] = strBuffer;
				}
			}
			_strings[cd.code] = cs;
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	spmemvfs_close_db(&db);
	spmemvfs_env_fini();
	datas_begin = _datas.begin();
	datas_end = _datas.end();
	strings_begin = _strings.begin();
	strings_end = _strings.end();
	return true;
}
bool DataManager::LoadStrings(const char* file) {
	FILE* fp = fopen(file, "r");
	if(!fp)
		return false;
	char linebuf[256];
	while(fgets(linebuf, 256, fp)) {
		ReadStringConfLine(linebuf);
	}
	fclose(fp);
	return true;
}
bool DataManager::LoadStrings(IReadFile* reader) {
	char ch[2] = " ";
	char linebuf[256] = "";
	while(reader->read(&ch[0], 1)) {
		if(ch[0] == '\0')
			break;
		std::strcat(linebuf, ch);
		if(ch[0] == '\n') {
			ReadStringConfLine(linebuf);
			linebuf[0] = '\0';
		}
	}
	reader->drop();
	return true;
}
void DataManager::ReadStringConfLine(const char* linebuf) {
	if(linebuf[0] != '!')
		return;
	char strbuf[256]{};
	int value{};
	wchar_t strBuffer[4096]{};
	if (sscanf(linebuf, "!%63s", strbuf) != 1)
		return;
	if(!std::strcmp(strbuf, "system")) {
		if (sscanf(&linebuf[7], "%d %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_sysStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "victory")) {
		if (sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_victoryStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "counter")) {
		if (sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_counterStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "setname")) {
		//using tab for comment
		if (sscanf(&linebuf[8], "%x %240[^\t\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_setnameStrings[value] = strBuffer;
	}
}
bool DataManager::Error(spmemvfs_db_t* pDB, sqlite3_stmt* pStmt) {
	wchar_t strBuffer[4096];
	BufferIO::DecodeUTF8(sqlite3_errmsg(pDB->handle), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	spmemvfs_close_db(pDB);
	spmemvfs_env_fini();
	return false;
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
code_pointer DataManager::GetCodePointer(unsigned int code) const {
	return _datas.find(code);
}
string_pointer DataManager::GetStringPointer(unsigned int code) const {
	return _strings.find(code);
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
const wchar_t* DataManager::FormatLocation(int location, int sequence) {
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
const wchar_t* DataManager::FormatAttribute(int attribute) {
	wchar_t* p = attBuffer;
	unsigned filter = 1;
	int i = 1010;
	for(; filter != 0x80; filter <<= 1, ++i) {
		if(attribute & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != attBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return attBuffer;
}
const wchar_t* DataManager::FormatRace(int race) {
	wchar_t* p = racBuffer;
	unsigned filter = 1;
	int i = 1020;
	for(; filter < (1 << RACES_COUNT); filter <<= 1, ++i) {
		if(race & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != racBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return racBuffer;
}
const wchar_t* DataManager::FormatType(int type) {
	wchar_t* p = tpBuffer;
	unsigned filter = 1;
	int i = 1050;
	for(; filter != 0x8000000; filter <<= 1, ++i) {
		if(type & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != tpBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return tpBuffer;
}
const wchar_t* DataManager::FormatSetName(const uint16_t setcode[]) {
	wchar_t* p = scBuffer;
	for(int i = 0; i < 10; ++i) {
		if (!setcode[i])
			break;
		const wchar_t* setname = GetSetName(setcode[i]);
		if(setname) {
			BufferIO::CopyWStrRef(setname, p, 32);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != scBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return scBuffer;
}
const wchar_t* DataManager::FormatLinkMarker(int link_marker) {
	wchar_t* p = lmBuffer;
	*p = 0;
	if(link_marker & LINK_MARKER_TOP_LEFT)
		BufferIO::CopyWStrRef(L"[\u2196]", p, 4);
	if(link_marker & LINK_MARKER_TOP)
		BufferIO::CopyWStrRef(L"[\u2191]", p, 4);
	if(link_marker & LINK_MARKER_TOP_RIGHT)
		BufferIO::CopyWStrRef(L"[\u2197]", p, 4);
	if(link_marker & LINK_MARKER_LEFT)
		BufferIO::CopyWStrRef(L"[\u2190]", p, 4);
	if(link_marker & LINK_MARKER_RIGHT)
		BufferIO::CopyWStrRef(L"[\u2192]", p, 4);
	if(link_marker & LINK_MARKER_BOTTOM_LEFT)
		BufferIO::CopyWStrRef(L"[\u2199]", p, 4);
	if(link_marker & LINK_MARKER_BOTTOM)
		BufferIO::CopyWStrRef(L"[\u2193]", p, 4);
	if(link_marker & LINK_MARKER_BOTTOM_RIGHT)
		BufferIO::CopyWStrRef(L"[\u2198]", p, 4);
	return lmBuffer;
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
	if(reader == NULL)
		return 0;
	size_t size = reader->getSize();
	if(size > sizeof(scriptBuffer)) {
		reader->drop();
		return 0;
	}
	reader->read(scriptBuffer, size);
	reader->drop();
	*slen = size;
	return scriptBuffer;
}

}
