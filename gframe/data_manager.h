#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "config.h"
#include "sqlite3.h"
#ifndef YGOPRO_SERVER_MODE
#include "spmemvfs/spmemvfs.h"
#endif
#include "client_card.h"
#include <unordered_map>

namespace ygo {

class DataManager {
public:
	DataManager(): _datas(8192), _strings(8192) {}
	bool LoadDB(const wchar_t* wfile);
	bool LoadStrings(const char* file);
#ifndef YGOPRO_SERVER_MODE
	bool LoadStrings(IReadFile* reader);
	void ReadStringConfLine(const char* linebuf);
	bool Error(spmemvfs_db_t* pDB, sqlite3_stmt* pStmt = 0);
#else
	void ReadStringConfLine(const char* linebuf);
	bool Error(sqlite3* pDB, sqlite3_stmt* pStmt = 0);
#endif
	bool GetData(int code, CardData* pData);
	code_pointer GetCodePointer(int code);
	bool GetString(int code, CardString* pStr);
	const wchar_t* GetName(int code);
	const wchar_t* GetText(int code);
	const wchar_t* GetDesc(unsigned int strCode);
	const wchar_t* GetSysString(int code);
	const wchar_t* GetVictoryString(int code);
	const wchar_t* GetCounterName(int code);
	const wchar_t* GetSetName(int code);
	unsigned int GetSetCode(const wchar_t* setname);
	const wchar_t* GetNumString(int num, bool bracket = false);
	const wchar_t* FormatLocation(int location, int sequence);
	const wchar_t* FormatAttribute(int attribute);
	const wchar_t* FormatRace(int race);
	const wchar_t* FormatType(int type);
	const wchar_t* FormatSetName(unsigned long long setcode);
	const wchar_t* FormatLinkMarker(int link_marker);

	std::unordered_map<unsigned int, CardDataC> _datas;
	std::unordered_map<unsigned int, CardString> _strings;
	std::unordered_map<unsigned int, std::wstring> _counterStrings;
	std::unordered_map<unsigned int, std::wstring> _victoryStrings;
	std::unordered_map<unsigned int, std::wstring> _setnameStrings;
	std::unordered_map<unsigned int, std::wstring> _sysStrings;

	wchar_t numStrings[301][4];
	wchar_t numBuffer[6];
	wchar_t attBuffer[128];
	wchar_t racBuffer[128];
	wchar_t tpBuffer[128];
	wchar_t scBuffer[128];
	wchar_t lmBuffer[32];

	static byte scriptBuffer[0x20000];
	static const wchar_t* unknown_string;
	static int CardReader(int, void*);
	static byte* ScriptReaderEx(const char* script_name, int* slen);
	static byte* ScriptReader(const char* script_name, int* slen);
#ifndef YGOPRO_SERVER_MODE
	static IFileSystem* FileSystem;
#endif
};

extern DataManager dataManager;

}

#endif // DATAMANAGER_H
