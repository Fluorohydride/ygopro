#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "sqlite3.h"
#include "client_card.h"
#include <unordered_map>

namespace ygo {

class DataManager {
public:
	DataManager(): _datas(8192), _strings(8192) {}
	bool LoadDB(const char* file);
	bool LoadStrings(const char* file);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = 0);
	bool GetData(int code, CardData* pData);
	code_pointer GetCodePointer(int code);
	bool GetString(int code, CardString* pStr);
	std::wstring GetName(int code);
	std::wstring GetText(int code);
	std::wstring GetDesc(u64 strCode);
	std::wstring GetSysString(int code);
	std::wstring GetVictoryString(int code);
	std::wstring GetCounterName(int code);
	std::wstring GetSetName(int code);
	unsigned int GetSetCode(const std::wstring& setname);
	std::wstring GetNumString(int num, bool bracket = false);
	std::wstring FormatLocation(int location, int sequence);
	std::wstring FormatAttribute(int attribute);
	std::wstring FormatRace(int race);
	std::wstring FormatType(int type);
	std::wstring FormatSetName(unsigned long long setcode);
	std::wstring FormatLinkMarker(int link_marker);

	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, std::wstring> _counterStrings;
	std::unordered_map<unsigned int, std::wstring> _victoryStrings;
	std::unordered_map<unsigned int, std::wstring> _setnameStrings;
	std::unordered_map<unsigned int, std::wstring> _sysStrings;

	static const wchar_t* unknown_string;
	static int CardReader(int, void*);

};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
