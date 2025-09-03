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
constexpr uint32_t MIN_CARD_ID = (uint32_t)(MAX_STRING_ID + 1) >> 4;
constexpr uint32_t MAX_CARD_ID = 0x0fffffffU;
	
using CardData = card_data;
struct CardDataC {
	uint32_t code{};
	uint32_t alias{};
	uint16_t setcode[SIZE_SETCODE]{};
	uint32_t type{};
	uint32_t level{};
	uint32_t attribute{};
	uint32_t race{};
	int32_t attack{};
	int32_t defense{};
	uint32_t lscale{};
	uint32_t rscale{};
	uint32_t link_marker{};
	uint32_t ot{};
	uint32_t category{};

	bool is_setcodes(const std::vector<unsigned int>& values) const {
		for (auto& value : values) {
			for (const auto& x : setcode) {
				if(!x)
					break;
				if(check_setcode(x, value))
					return true;
			}
		}
		return false;
	}
};
struct CardString {
	std::wstring name;
	std::wstring text;
	std::wstring desc[16];
};
using code_pointer = std::unordered_map<uint32_t, CardDataC>::const_iterator;
using string_pointer = std::unordered_map<uint32_t, CardString>::const_iterator;

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

	code_pointer GetCodePointer(uint32_t code) const;
#ifndef YGOPRO_SERVER_MODE
	string_pointer GetStringPointer(uint32_t code) const;
#endif
	const std::unordered_map<uint32_t, CardDataC>& GetDataTable() const {
		return _datas;
	}
#ifndef YGOPRO_SERVER_MODE
	const std::unordered_map<uint32_t, CardString>& GetStringTable() const {
		return _strings;
	}
#endif
	bool GetData(uint32_t code, CardData* pData) const;
#ifndef YGOPRO_SERVER_MODE
	bool GetString(uint32_t code, CardString* pStr) const;
	const wchar_t* GetName(uint32_t code) const;
	const wchar_t* GetText(uint32_t code) const;
	const wchar_t* GetDesc(uint32_t strCode) const;
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
	const wchar_t* unknown_string{ L"???" };
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	irr::io::IFileSystem* FileSystem{};
#endif

	static unsigned char scriptBuffer[0x100000];
	static uint32_t CardReader(uint32_t, card_data*);
	static unsigned char* ScriptReaderEx(const char* script_path, int* slen);
	
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	//read by IFileSystem
	static unsigned char* ReadScriptFromIrrFS(const char* script_name, int* slen);
#endif
	//read by fread
	static unsigned char* ReadScriptFromFile(const char* script_name, int* slen);

#ifndef YGOPRO_SERVER_MODE
	static bool deck_sort_lv(code_pointer l1, code_pointer l2);
	static bool deck_sort_atk(code_pointer l1, code_pointer l2);
	static bool deck_sort_def(code_pointer l1, code_pointer l2);
	static bool deck_sort_name(code_pointer l1, code_pointer l2);
#endif

private:
	std::unordered_map<uint32_t, CardDataC> _datas;
	std::unordered_map<uint32_t, CardString> _strings;
	std::unordered_map<uint32_t, std::vector<uint16_t>> extra_setcode;
};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
