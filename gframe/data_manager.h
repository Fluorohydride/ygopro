#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <sqlite3.h>
#include "../ocgcore/card_data.h"

namespace irr {
	namespace io {
		class IReadFile;
		class IFileSystem;
	}
}

namespace ygo {
constexpr int MAX_STRING_ID = 0x7ff;
constexpr unsigned int MIN_CARD_ID = (unsigned int)(MAX_STRING_ID + 1) >> 4;
constexpr unsigned int MAX_CARD_ID = 0x0fffffffU;
	
using CardData = card_data;
struct CardDataC : card_data {
	uint32_t ot{};
	uint32_t category{};

	bool is_setcodes(const std::vector<unsigned int>& values) const {
		for (auto& value : values) {
			if (is_setcode(value))
				return true;
		}
		return false;
	}
};
struct CardString {
	std::wstring name;
	std::wstring text;
	std::wstring desc[16];
};
using code_pointer = std::unordered_map<unsigned int, CardDataC>::const_iterator;
using string_pointer = std::unordered_map<unsigned int, CardString>::const_iterator;

class DataManager {
public:
	DataManager();
	bool ReadDB(sqlite3* pDB);
	bool LoadDB(const wchar_t* wfile);
#ifndef YGOPRO_SERVER_MODE
	bool LoadStrings(const char* file);
	bool LoadStrings(irr::io::IReadFile* reader);
	void ReadStringConfLine(const char* linebuf);
#endif
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = nullptr);

	code_pointer GetCodePointer(unsigned int code) const;
#ifndef YGOPRO_SERVER_MODE
	string_pointer GetStringPointer(unsigned int code) const;
	code_pointer datas_begin() const;
	code_pointer datas_end() const;
	string_pointer strings_begin() const;
	string_pointer strings_end() const;
#endif
	bool GetData(unsigned int code, CardData* pData) const;
#ifndef YGOPRO_SERVER_MODE
	bool GetString(unsigned int code, CardString* pStr) const;
	const wchar_t* GetName(unsigned int code) const;
	const wchar_t* GetText(unsigned int code) const;
	const wchar_t* GetDesc(unsigned int strCode) const;
	const wchar_t* GetSysString(int code) const;
	const wchar_t* GetVictoryString(int code) const;
	const wchar_t* GetCounterName(int code) const;
	const wchar_t* GetSetName(int code) const;
	std::vector<unsigned int> GetSetCodes(std::wstring setname) const;
	std::wstring GetNumString(int num, bool bracket = false) const;
	const wchar_t* FormatLocation(int location, int sequence) const;
	std::wstring FormatAttribute(unsigned int attribute) const;
	std::wstring FormatRace(unsigned int race) const;
	std::wstring FormatType(unsigned int type) const;
	std::wstring FormatSetName(const uint16_t setcode[]) const;
	std::wstring FormatLinkMarker(unsigned int link_marker) const;

	std::unordered_map<unsigned int, std::wstring> _counterStrings;
	std::unordered_map<unsigned int, std::wstring> _victoryStrings;
	std::unordered_map<unsigned int, std::wstring> _setnameStrings;
	std::unordered_map<unsigned int, std::wstring> _sysStrings;
#endif
	char errmsg[512]{};

	static unsigned char scriptBuffer[0x100000];
	static const wchar_t* unknown_string;
	static uint32_t CardReader(uint32_t, card_data*);
	static unsigned char* ScriptReaderEx(const char* script_path, int* slen);
	
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	//read by IFileSystem
	static unsigned char* ReadScriptFromIrrFS(const char* script_name, int* slen);
#endif
	//read by fread
	static unsigned char* ReadScriptFromFile(const char* script_name, int* slen);
	
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	static irr::io::IFileSystem* FileSystem;
#endif

#ifndef YGOPRO_SERVER_MODE
	static bool deck_sort_lv(code_pointer l1, code_pointer l2);
	static bool deck_sort_atk(code_pointer l1, code_pointer l2);
	static bool deck_sort_def(code_pointer l1, code_pointer l2);
	static bool deck_sort_name(code_pointer l1, code_pointer l2);
#endif

private:
	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, std::vector<uint16_t>> extra_setcode;
};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
