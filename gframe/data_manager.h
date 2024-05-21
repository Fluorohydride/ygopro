#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "sqlite3.h"
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
#include "spmemvfs/spmemvfs.h"
#endif
#include "client_card.h"
#include <unordered_map>

namespace ygo {
	constexpr int MAX_STRING_ID = 0x7ff;
	constexpr unsigned int MIN_CARD_ID = (unsigned int)(MAX_STRING_ID + 1) >> 4;

class DataManager {
public:
	DataManager();
	bool LoadDB(const wchar_t* wfile);
	bool LoadStrings(const char* file);
#ifndef YGOPRO_SERVER_MODE
	bool LoadStrings(IReadFile* reader);
#endif
	void ReadStringConfLine(const char* linebuf);
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	bool Error(spmemvfs_db_t* pDB, sqlite3_stmt* pStmt = 0);
#else
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = 0);
#endif //YGOPRO_SERVER_MODE
	bool GetData(unsigned int code, CardData* pData);
	code_pointer GetCodePointer(unsigned int code) const;
	string_pointer GetStringPointer(unsigned int code) const;
	bool GetString(unsigned int code, CardString* pStr);
	const wchar_t* GetName(unsigned int code);
	const wchar_t* GetText(unsigned int code);
	const wchar_t* GetDesc(unsigned int strCode);
	const wchar_t* GetSysString(int code);
	const wchar_t* GetVictoryString(int code);
	const wchar_t* GetCounterName(int code);
	const wchar_t* GetSetName(int code);
	std::vector<unsigned int> GetSetCodes(std::wstring setname);
	const wchar_t* GetNumString(int num, bool bracket = false);
	const wchar_t* FormatLocation(int location, int sequence);
	const wchar_t* FormatAttribute(int attribute);
	const wchar_t* FormatRace(int race);
	const wchar_t* FormatType(int type);
	const wchar_t* FormatSetName(const uint16_t setcode[]);
	const wchar_t* FormatLinkMarker(int link_marker);

	std::unordered_map<unsigned int, std::wstring> _counterStrings;
	std::unordered_map<unsigned int, std::wstring> _victoryStrings;
	std::unordered_map<unsigned int, std::wstring> _setnameStrings;
	std::unordered_map<unsigned int, std::wstring> _sysStrings;
	code_pointer datas_begin;
	code_pointer datas_end;
	string_pointer strings_begin;
	string_pointer strings_end;

	wchar_t numStrings[301][4]{};
	wchar_t numBuffer[6]{};
	wchar_t attBuffer[128]{};
	wchar_t racBuffer[128]{};
	wchar_t tpBuffer[128]{};
	wchar_t scBuffer[128]{};
	wchar_t lmBuffer[32]{};

	static byte scriptBuffer[0x20000];
	static const wchar_t* unknown_string;
	static uint32 CardReader(uint32, card_data*);
	static byte* ScriptReaderEx(const char* script_name, int* slen);
	static byte* ScriptReader(const char* script_name, int* slen);
#if !defined(YGOPRO_SERVER_MODE) || defined(SERVER_ZIP_SUPPORT)
	static IFileSystem* FileSystem;
#endif

private:
	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, std::vector<uint16_t>> extra_setcode;
};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
