#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <unordered_map>
#include <cstdint>
#include "utils.h"
#include "client_card.h"

struct sqlite3;
struct sqlite3_stmt;

#define SCOPE_OCG        0x1
#define SCOPE_TCG        0x2
#define SCOPE_ANIME      0x4
#define SCOPE_ILLEGAL    0x8
#define SCOPE_VIDEO_GAME 0x10
#define SCOPE_CUSTOM     0x20
#define SCOPE_SPEED      0x40
#define SCOPE_PRERELEASE 0x100
#define SCOPE_RUSH       0x200
#define SCOPE_LEGEND     0x400
#define SCOPE_HIDDEN     0x1000

#define SCOPE_OCG_TCG    (SCOPE_OCG | SCOPE_TCG)
#define SCOPE_OFFICIAL   (SCOPE_OCG | SCOPE_TCG | SCOPE_PRERELEASE)

#define TYPE_SKILL       0x8000000
#define TYPE_ACTION      0x10000000

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
	CardString _strings{};
	CardString* _locale_strings = nullptr;
};

class DataManager {
public:
	DataManager() {
		cards.reserve(10000);
		locales.reserve(10000);
	}
	~DataManager() {}
	void ClearLocaleTexts();
	bool LoadLocaleDB(const path_string& file, bool usebuffer = false);
	bool LoadDB(const path_string& file, bool usebuffer = false);
	bool LoadDBFromBuffer(const std::vector<char>& buffer, const std::string& filename = "");
	bool LoadStrings(const path_string& file);
	bool LoadLocaleStrings(const path_string& file);
	void ClearLocaleStrings();
	bool GetData(uint32_t code, CardData* pData);
	CardDataC* GetCardData(uint32_t code);
	bool GetString(uint32_t code, CardString* pStr);
	std::wstring GetName(uint32_t code);
	std::wstring GetText(uint32_t code);
	std::wstring GetDesc(uint64_t strCode, bool compat);
	std::wstring GetSysString(uint32_t code);
	std::wstring GetVictoryString(int code);
	std::wstring GetCounterName(int code);
	std::wstring GetSetName(int code);
	std::vector<unsigned int> GetSetCode(std::vector<std::wstring>& setname);
	std::wstring GetNumString(int num, bool bracket = false);
	std::wstring FormatLocation(int location, int sequence);
	std::wstring FormatAttribute(int attribute);
	std::wstring FormatRace(int race, bool isSkill = false);
	std::wstring FormatType(int type);
	std::wstring FormatScope(int scope, bool hideOCGTCG = false);
	std::wstring FormatSetName(uint64_t setcode);
	std::wstring FormatSetName(std::vector<uint16_t> setcodes);
	std::wstring FormatLinkMarker(int link_marker);

	std::unordered_map<unsigned int, CardDataM> cards;

	static const wchar_t* unknown_string;
	static void CardReader(void* payload, uint32_t code, CardData* data);
private:
	template<typename T1, typename T2 = T1>
	using indexed_map = std::map<unsigned int, std::pair<T1, T2>>;

	class LocaleStringHelper {
	public:
		indexed_map<std::wstring> map{};
		const wchar_t* GetLocale(unsigned int code) {
			auto search = map.find(code);
			if(search == map.end() || search->second.first.empty())
				return nullptr;
			return search->second.second.size() ? search->second.second.c_str() : search->second.first.c_str();
		}
		void ClearLocales() {
			for(auto& elem : map)
				elem.second.second.clear();
		}
		void SetMain(unsigned int code, const std::wstring& val) {
			map[code].first = val;
		}
		void SetLocale(unsigned int code, const std::wstring& val) {
			map[code].second = val;
		}
	};
	bool ParseDB(sqlite3* pDB);
	bool ParseLocaleDB(sqlite3* pDB);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = nullptr);
	std::unordered_map<uint32_t, CardString> locales;
	indexed_map<CardDataM*, CardString*> indexes;
	LocaleStringHelper _counterStrings;
	LocaleStringHelper _victoryStrings;
	LocaleStringHelper _setnameStrings;
	LocaleStringHelper _sysStrings;
};

extern DataManager* gDataManager;

}

#endif // DATAMANAGER_H
