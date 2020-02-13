#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "client_card.h"
#include <unordered_map>

struct sqlite3;
struct sqlite3_stmt;

#define TYPE_SKILL 0x8000000

namespace ygo {

class CardDataM {
public:
	CardDataC _data{};
	const CardString* GetStrings() {
		if(_locale_strings)
			return _locale_strings;
		return &_strings;
	}
	CardDataM(){}
	CardDataM(CardDataC&& data, CardString&& strings, CardString* locale_strings = nullptr):
		_data(std::move(data)), _strings(std::move(strings)), _locale_strings(locale_strings){}
private:
	CardString _strings{};
	CardString* _locale_strings = nullptr;
};

class DataManager {
public:
	DataManager() {
		cards.reserve(10000);
	}
	~DataManager() {}
	bool LoadDB(const path_string& file, bool usebuffer = false);
	bool LoadDBFromBuffer(const std::vector<char>& buffer);
	bool ParseDB(sqlite3* pDB);
	bool LoadStrings(const path_string& file);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = 0);
	bool GetData(int code, CardData* pData);
	CardDataC* GetCardData(int code);
	bool GetString(int code, CardString* pStr);
	std::wstring GetName(int code);
	std::wstring GetText(int code);
	std::wstring GetDesc(uint64 strCode, bool compat);
	std::wstring GetSysString(uint64 code);
	std::wstring GetVictoryString(int code);
	std::wstring GetCounterName(int code);
	std::wstring GetSetName(int code);
	std::vector<unsigned int> GetSetCode(std::vector<std::wstring>& setname);
	std::wstring GetNumString(int num, bool bracket = false);
	std::wstring FormatLocation(int location, int sequence);
	std::wstring FormatAttribute(int attribute);
	std::wstring FormatRace(int race, bool isSkill = false);
	std::wstring FormatType(int type);
	std::wstring FormatSetName(unsigned long long setcode);
	std::wstring FormatSetName(std::vector<uint16> setcodes);
	std::wstring FormatLinkMarker(int link_marker);

	std::unordered_map<unsigned int, CardDataM> cards;
	std::unordered_map<unsigned int, std::wstring> _counterStrings;
	std::unordered_map<unsigned int, std::wstring> _victoryStrings;
	std::unordered_map<unsigned int, std::wstring> _setnameStrings;
	std::unordered_map<unsigned int, std::wstring> _sysStrings;

	static const wchar_t* unknown_string;
	static void CardReader(void* payload, int code, CardData* data);

};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
