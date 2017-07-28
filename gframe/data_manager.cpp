#include "data_manager.h"
#include <stdio.h>

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
wchar_t DataManager::strBuffer[4096];
DataManager dataManager;

bool DataManager::LoadDB(const char* file) {
	sqlite3* pDB;
	if(sqlite3_open_v2(file, &pDB, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
		return Error(pDB);
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	CardDataC cd;
	CardString cs;
	for(int i = 0; i < 16; ++i) cs.desc[i] = 0;
	int step = 0, len = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
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
			_datas.insert(std::make_pair(cd.code, cd));
			len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), strBuffer);
			if(len) {
				cs.name = new wchar_t[len + 1];
				memcpy(cs.name, strBuffer, (len + 1)*sizeof(wchar_t));
			} else cs.name = 0;
			len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), strBuffer);
			if(len) {
				cs.text = new wchar_t[len + 1];
				memcpy(cs.text, strBuffer, (len + 1)*sizeof(wchar_t));
			} else {
				cs.text = new wchar_t[1];
				cs.text[0] = 0;
			}
			for(int i = 14; i < 30; ++i) {
				len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, i), strBuffer);
				if(len) {
					cs.desc[i - 14] = new wchar_t[len + 1];
					memcpy(cs.desc[i - 14], strBuffer, (len + 1)*sizeof(wchar_t));
				} else cs.desc[i - 14] = 0;
			}
			_strings.insert(std::make_pair(cd.code, cs));
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return true;
}
bool DataManager::LoadStrings(const char* file) {
	FILE* fp = fopen(file, "r");
	if(!fp)
		return false;
	char linebuf[256];
	char strbuf[256];
	int value;
	while(fgets(linebuf, 256, fp)) {
		if(linebuf[0] != '!')
			continue;
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
	fclose(fp);
	for(int i = 0; i < 255; ++i)
		myswprintf(numStrings[i], L"%d", i);
	return true;
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	BufferIO::DecodeUTF8(sqlite3_errmsg(pDB), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return false;
}
bool DataManager::GetData(int code, CardData* pData) {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if(pData)
		*pData = *((CardData*)&cdit->second);
	return true;
}
code_pointer DataManager::GetCodePointer(int code) {
	return _datas.find(code);
}
bool DataManager::GetString(int code, CardString* pStr) {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = (wchar_t*)unknown_string;
		pStr->text = (wchar_t*)unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
const wchar_t* DataManager::GetName(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.name)
		return csit->second.name;
	return unknown_string;
}
const wchar_t* DataManager::GetText(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.text)
		return csit->second.text;
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(int strCode) {
	if(strCode < 10000)
		return GetSysString(strCode);
	int code = strCode >> 4;
	int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.desc[offset])
		return csit->second.desc[offset];
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
		auto xpos = csit->second.find_first_of(L'|');//setname|extra info
		if(csit->second.compare(0, xpos, setname) == 0)
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
	for(; filter != 0x2000000; filter <<= 1, ++i) {
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
int DataManager::CardReader(int code, void* pData) {
	if(!dataManager.GetData(code, (CardData*)pData))
		memset(pData, 0, sizeof(CardData));
	return 0;
}

}
