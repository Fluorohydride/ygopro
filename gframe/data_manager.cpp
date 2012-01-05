#include "data_manager.h"
#include <stdio.h>
namespace ygo {
const wchar_t* DataManager::unknown_string = L"???";

wchar_t DataManager::strBuffer[2048];

bool DataManager::LoadDates(const char* file) {
	sqlite3* pDB;
	if(sqlite3_open(file, &pDB) != SQLITE_OK)
		return Error(pDB);
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	for(int i = 0; i < 2048; ++i)
		_sysStrings[i] = 0;
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
			cd.alias = sqlite3_column_int(pStmt, 2);
			cd.setcode = sqlite3_column_int(pStmt, 3);
			cd.type = sqlite3_column_int(pStmt, 4);
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defence = sqlite3_column_int(pStmt, 6);
			cd.level = sqlite3_column_int(pStmt, 7);
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			_datas.insert(std::make_pair(cd.code, cd));
			len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), strBuffer);
			if(len) {
				cs.name = new wchar_t[len + 1];
				memcpy(cs.name, strBuffer, (len + 1)*sizeof(wchar_t));
			} else cs.name = 0;
			len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), strBuffer);
			if(len) {
				cs.text = new wchar_t[len + 1];
				memcpy(cs.text, strBuffer, (len + 1)*sizeof(wchar_t));
			} else {
				cs.text = new wchar_t[1];
				cs.text[0] = 0;
			}
			for(int i = 14; i < 30; ++i) {
				len = DecodeUTF8((const char*)sqlite3_column_text(pStmt, i), strBuffer);
				if(len) {
					cs.desc[i - 14] = new wchar_t[len + 1];
					memcpy(cs.desc[i - 14], strBuffer, (len + 1)*sizeof(wchar_t));
				} else break;
			}
			_strings.insert(std::make_pair(cd.code, cs));
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	FILE* fp = fopen("strings.conf", "r");
	if(!fp)
		return false;
	char linebuf[256];
	char strbuf[256];
	int value;
	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fgets(linebuf, 256, fp);
	while(ftell(fp) < fsize) {
		fgets(linebuf, 256, fp);
		if(linebuf[0] != '!')
			continue;
		sscanf(linebuf, "!%s", strbuf);
		if(!strcmp(strbuf, "system")) {
			sscanf(&linebuf[7], "%d %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_sysStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "victory")) {
			sscanf(&linebuf[8], "%x %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_victoryStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "counter")) {
			sscanf(&linebuf[8], "%x %s", &value, strbuf);
			int len = DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_counterStrings[value] = pbuf;
		}
	}
	fclose(fp);
	for(int i = 0; i < 255; ++i)
		myswprintf(numStrings[i], L"%d", i);
	return true;
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	DecodeUTF8(sqlite3_errmsg(pDB), strBuffer);
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
	if(code < 0 || code >= 2048 || _sysStrings[code] == 0)
		return unknown_string;
	return _sysStrings[code];
}
const wchar_t* DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetNumString(int num) {
	return numStrings[num];
}
int DataManager::EncodeUTF8(const wchar_t * wsrc, char * str) {
	char* pstr = str;
	while(*wsrc != 0) {
		if(*wsrc < 0x80) {
			*str = *wsrc;
			++str;
		} else if(*wsrc < 0x800) {
			str[0] = (*wsrc >> 6) & 0x1f | 0xc0;
			str[1] = (*wsrc) & 0x3f | 0x80;
			str += 2;
		} else {
			str[0] = (*wsrc >> 12) & 0xf | 0xe0;
			str[1] = (*wsrc >> 6) & 0x3f | 0x80;
			str[2] = (*wsrc) & 0x3f | 0x80;
			str += 3;
		}
		wsrc++;
	}
	*str = 0;
	return str - pstr;
}
int DataManager::DecodeUTF8(const char * src, wchar_t * wstr) {
	char* p = (char*)src;
	wchar_t* wp = wstr;
	while(*p != 0) {
		if((*p & 0x80) == 0) {
			*wp = *p;
			p++;
		} else if((*p & 0xe0) == 0xc0) {
			*wp = (((int)p[0] & 0x1f) << 6) | ((int)p[1] & 0x3f);
			p += 2;
		} else if((*p & 0xf0) == 0xe0) {
			*wp = (((int)p[0] & 0xf) << 12) | (((int)p[1] & 0x3f) << 6) | ((int)p[2] & 0x3f);
			p += 3;
		} else if((*p & 0xf8) == 0xf0) {
			*wp = (((int)p[0] & 0x7) << 18) | (((int)p[1] & 0x3f) << 12) | (((int)p[2] & 0x3f) << 6) | ((int)p[3] & 0x3f);
			p += 4;
		} else
			p++;
		wp++;
	}
	*wp = 0;
	return wp - wstr;
}
int DataManager::GetVal(const wchar_t* pstr) {
	int ret = 0;
	while(*pstr >= L'0' && *pstr <= L'9') {
		ret = ret * 10 + (*pstr - L'0');
		pstr++;
	}
	return ret;
}
int DataManager::CopyStr(const wchar_t* src, wchar_t*& pstr, int maxlen) {
	int l = 0;
	while(src[l] && l < maxlen) {
		pstr[l] = src[l];
		l++;
	}
	pstr += l;
	*pstr = 0;
	return l;
}
const wchar_t* DataManager::FormatLocation(int location) {
	int filter = 1, i = 1000;
	while(filter != location) {
		filter <<= 1;
		i++;
	}
	if(filter == location)
		return GetSysString(i);
	else
		return unknown_string;
}
const wchar_t* DataManager::FormatAttribute(int attribute) {
	wchar_t* p = attBuffer;
	int filter = 1, i = 1010;
	for(; filter != 0x80; filter <<= 1, ++i) {
		if(attribute & filter) {
			CopyStr(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != attBuffer)
		*(p - 1) = 0;
	return attBuffer;
}
const wchar_t* DataManager::FormatRace(int race) {
	wchar_t* p = racBuffer;
	int filter = 1, i = 1020;
	for(; filter != 0x800000; filter <<= 1, ++i) {
		if(race & filter) {
			CopyStr(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != racBuffer)
		*(p - 1) = 0;
	return racBuffer;
}
const wchar_t* DataManager::FormatType(int type) {
	wchar_t* p = tpBuffer;
	int filter = 1, i = 1050;
	for(; filter != 0x1000000; filter <<= 1, ++i) {
		if(type & filter) {
			CopyStr(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != tpBuffer)
		*(p - 1) = 0;
	return tpBuffer;
}
}
