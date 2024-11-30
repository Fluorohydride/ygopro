#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include <unordered_map>
#include <sqlite3.h>
#include "client_card.h"

namespace ygo {
	constexpr int MAX_STRING_ID = 0x7ff;
	constexpr unsigned int MIN_CARD_ID = (unsigned int)(MAX_STRING_ID + 1) >> 4;

class DataManager {
public:
	DataManager();
	bool ReadDB(sqlite3* pDB);
	bool LoadDB(const wchar_t* wfile);
	bool LoadStrings(const char* file);
	bool LoadStrings(IReadFile* reader);
	void ReadStringConfLine(const char* linebuf);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = nullptr);

	code_pointer GetCodePointer(unsigned int code) const;
	string_pointer GetStringPointer(unsigned int code) const;
	code_pointer datas_begin();
	code_pointer datas_end();
	string_pointer strings_begin();
	string_pointer strings_end();
	bool GetData(unsigned int code, CardData* pData) const;
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
	char errmsg[512]{};

	static unsigned char scriptBuffer[0x20000];
	static const wchar_t* unknown_string;
	static uint32 CardReader(uint32, card_data*);
	static unsigned char* ScriptReaderEx(const char* script_name, int* slen);
	
	//read by IFileSystem
	static unsigned char* ScriptReader(const char* script_name, int* slen);
	//read by fread
	static unsigned char* DefaultScriptReader(const char* script_name, int* slen);
	
	static IFileSystem* FileSystem;

private:
	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, std::vector<uint16_t>> extra_setcode;
};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
