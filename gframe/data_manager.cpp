#include "data_manager.h"
#include "game.h"
#include <stdio.h>

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
byte DataManager::scriptBuffer[0x20000];
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
IFileSystem* DataManager::FileSystem;
#endif
DataManager dataManager;

bool DataManager::LoadDB(const wchar_t* wfile) {
	char file[256];
	BufferIO::EncodeUTF8(wfile, file);
#if defined(YGOPRO_SERVER_MODE) && !defined(SERVER_ZIP_SUPPORT)
	sqlite3* pDB;
	if(sqlite3_open_v2(file, &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
#else
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
#endif //YGOPRO_SERVER_MODE
	sqlite3_stmt* pStmt;
#ifdef YGOPRO_SERVER_MODE
	const char* sql = "select * from datas";
#else
	const char* sql = "select * from datas,texts where datas.id=texts.id";
#endif
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
#if defined(YGOPRO_SERVER_MODE) && !defined(SERVER_ZIP_SUPPORT)
		return Error(pDB);
#else
		return Error(&db);
#endif
	CardDataC cd;
	CardString cs;
#ifndef YGOPRO_SERVER_MODE
	wchar_t strBuffer[4096];
#endif
	int step = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
#if defined(YGOPRO_SERVER_MODE) && !defined(SERVER_ZIP_SUPPORT)
			return Error(pDB, pStmt);
#else
			return Error(&db, pStmt);
#endif
		else if(step == SQLITE_ROW) {
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.ot = sqlite3_column_int(pStmt, 1);
			cd.alias = sqlite3_column_int(pStmt, 2);
			cd.setcode = sqlite3_column_int64(pStmt, 3);
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
#ifndef YGOPRO_SERVER_MODE
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
#endif //YGOPRO_SERVER_MODE
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
#ifdef YGOPRO_SERVER_MODE
	sqlite3_close(pDB);
#else
	spmemvfs_close_db(&db);
	spmemvfs_env_fini();
#endif
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
	for(int i = 0; i < 301; ++i)
		myswprintf(numStrings[i], L"%d", i);
	return true;
}
#ifndef YGOPRO_SERVER_MODE
bool DataManager::LoadStrings(IReadFile* reader) {
	char ch[2] = " ";
	char linebuf[256] = "";
	while(reader->read(&ch[0], 1)) {
		if(ch[0] == '\0')
			break;
		strcat(linebuf, ch);
		if(ch[0] == '\n') {
			ReadStringConfLine(linebuf);
			linebuf[0] = '\0';
		}
	}
	reader->drop();
	return true;
}
#endif //YGOPRO_SERVER_MODE
void DataManager::ReadStringConfLine(const char* linebuf) {
	if(linebuf[0] != '!')
		return;
	char strbuf[256];
	int value;
	wchar_t strBuffer[4096];
	sscanf(linebuf, "!%s", strbuf);
	if(!strcmp(strbuf, "system")) {
		sscanf(&linebuf[7], "%d %240[^\n]", &value, strbuf);
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_sysStrings[value] = strBuffer;
	} else if(!strcmp(strbuf, "victory")) {
		sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf);
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_victoryStrings[value] = strBuffer;
	} else if(!strcmp(strbuf, "counter")) {
		sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf);
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_counterStrings[value] = strBuffer;
	} else if(!strcmp(strbuf, "setname")) {
		sscanf(&linebuf[8], "%x %240[^\t\n]", &value, strbuf);//using tab for comment
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_setnameStrings[value] = strBuffer;
	}
}
#if defined(YGOPRO_SERVER_MODE) && !defined(SERVER_ZIP_SUPPORT)
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	wchar_t strBuffer[4096];
	BufferIO::DecodeUTF8(sqlite3_errmsg(pDB), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return false;
}
#else
bool DataManager::Error(spmemvfs_db_t* pDB, sqlite3_stmt* pStmt) {
	wchar_t strBuffer[4096];
	BufferIO::DecodeUTF8(sqlite3_errmsg(pDB->handle), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	spmemvfs_close_db(pDB);
	spmemvfs_env_fini();
	return false;
}
#endif //YGOPRO_SERVER_MODE
bool DataManager::GetData(unsigned int code, CardData* pData) {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	auto data = cdit->second;
	if (pData) {
		pData->code = data.code;
		pData->alias = data.alias;
		pData->setcode = data.setcode;
		pData->type = data.type;
		pData->level = data.level;
		pData->attribute = data.attribute;
		pData->race = data.race;
		pData->attack = data.attack;
		pData->defense = data.defense;
		pData->lscale = data.lscale;
		pData->rscale = data.rscale;
		pData->link_marker = data.link_marker;
	}
	return true;
}
code_pointer DataManager::GetCodePointer(int code) {
	return _datas.find(code);
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
const wchar_t* DataManager::GetName(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.name.empty())
		return csit->second.name.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetText(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.text.empty())
		return csit->second.text.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(unsigned int strCode) {
	if(strCode < 10000u)
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
const wchar_t* DataManager::GetSysString(int code) {
	if(code < 0 || code >= 2048)
		return unknown_string;
	auto csit = _sysStrings.find(code);
	if(csit == _sysStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetSetName(int code) {
	auto csit = _setnameStrings.find(code);
	if(csit == _setnameStrings.end())
		return NULL;
	return csit->second.c_str();
}
unsigned int DataManager::GetSetCode(const wchar_t* setname) {
	for(auto csit = _setnameStrings.begin(); csit != _setnameStrings.end(); ++csit) {
		auto xpos = csit->second.find_first_of(L'|');//setname|another setname or extra info
		if(csit->second.compare(0, xpos, setname) == 0 || csit->second.compare(xpos + 1, csit->second.length(), setname) == 0)
			return csit->first;
	}
	return 0;
}
const wchar_t* DataManager::GetNumString(int num, bool bracket) {
	if(!bracket)
		return numStrings[num];
	wchar_t* p = numBuffer;
	*p++ = L'(';
	BufferIO::CopyWStrRef(numStrings[num], p, 4);
	*p = L')';
	*++p = 0;
	return numBuffer;
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
const wchar_t* DataManager::FormatSetName(unsigned long long setcode) {
	wchar_t* p = scBuffer;
	for(int i = 0; i < 4; ++i) {
		const wchar_t* setname = GetSetName((setcode >> i * 16) & 0xffff);
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
#ifdef YGOPRO_SERVER_MODE
	char first[256];
	char second[256];
	char third[256];
	sprintf(first, "specials/%s", script_name + 9);
	sprintf(second, "expansions/%s", script_name + 2);
	sprintf(third, "%s", script_name + 2);
	if(ScriptReader(first, slen))
		return scriptBuffer;
	else if(ScriptReader(second, slen))
		return scriptBuffer;
	else
		return ScriptReader(third, slen);
#else
	char first[256];
	char second[256];
	if(mainGame->gameConf.prefer_expansion_script) {
		sprintf(first, "expansions/%s", script_name + 2);
		sprintf(second, "%s", script_name + 2);
	} else {
		sprintf(first, "%s", script_name + 2);
		sprintf(second, "expansions/%s", script_name + 2);
	}
	if(ScriptReader(first, slen))
		return scriptBuffer;
	else
		return ScriptReader(second, slen);
#endif //YGOPRO_SERVER_MODE
}
byte* DataManager::ScriptReader(const char* script_name, int* slen) {
#if defined(YGOPRO_SERVER_MODE) && !defined(SERVER_ZIP_SUPPORT)
	FILE* fp = fopen(script_name, "rb");
	if(!fp)
		return 0;
	int len = fread(scriptBuffer, 1, sizeof(scriptBuffer), fp);
	fclose(fp);
	if(len >= sizeof(scriptBuffer))
		return 0;
	*slen = len;
#else
#ifdef _WIN32
	wchar_t fname[256];
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
#endif //YGOPRO_SERVER_MODE
	return scriptBuffer;
}

}
