#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "sqlite3.h"
#include "client_card.h"
#include <unordered_map>

namespace ygo {

class DataManager {
public:
	DataManager() {
	}
	~DataManager() {
	}
	bool LoadDates(const char* file);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = 0);
	bool GetData(int code, CardData* pData);
	code_pointer GetCodePointer(int code);
	bool GetString(int code, CardString* pStr);
	const wchar_t* GetName(int code);
	const wchar_t* GetText(int code);
	const wchar_t* GetDesc(int strCode);
	const wchar_t* GetSysString(int code);
	const wchar_t* GetVictoryString(int code);
	const wchar_t* GetCounterName(int code);
	const wchar_t* GetNumString(int num);
	const wchar_t* FormatLocation(int location);
	const wchar_t* FormatAttribute(int attribute);
	const wchar_t* FormatRace(int race);
	const wchar_t* FormatType(int type);
	static int EncodeUTF8(const wchar_t * wsrc, char * str);
	static int DecodeUTF8(const char * src, wchar_t * wstr);
	static int GetVal(const wchar_t* pstr);
	static int CopyStr(const wchar_t* src, wchar_t*& pstr, int maxlen);
	
	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, wchar_t*> _sysStrings;
	std::unordered_map<unsigned int, wchar_t*> _counterStrings;
	std::unordered_map<unsigned int, wchar_t*> _victoryStrings;
	wchar_t numStrings[256][4];
	wchar_t attBuffer[128];
	wchar_t racBuffer[128];
	wchar_t tpBuffer[128];
	
	static wchar_t strBuffer[2048];
	static const wchar_t* unknown_string;
	static const wchar_t* effect_strings[];
};

}

#endif // DATAMANAGER_H
