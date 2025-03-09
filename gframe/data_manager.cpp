#include "data_manager.h"
#include "game.h"
#include "spmemvfs/spmemvfs.h"

namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
unsigned char DataManager::scriptBuffer[0x100000] = {};
irr::io::IFileSystem* DataManager::FileSystem = nullptr;
DataManager dataManager;

DataManager::DataManager() : _datas(32768), _strings(32768) {
	extra_setcode = { {8512558u, {0x8f, 0x54, 0x59, 0x82, 0x13a}}, };
}
bool DataManager::ReadDB(sqlite3* pDB) {
	sqlite3_stmt* pStmt = nullptr;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if (sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB, pStmt);
	wchar_t strBuffer[4096];
	int step = 0;
	do {
		CardDataC cd;
		CardString cs;
		step = sqlite3_step(pStmt);
		if (step == SQLITE_ROW) {
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.ot = sqlite3_column_int(pStmt, 1);
			cd.alias = sqlite3_column_int(pStmt, 2);
			uint64_t setcode = static_cast<uint64_t>(sqlite3_column_int64(pStmt, 3));
			if (setcode) {
				auto it = extra_setcode.find(cd.code);
				if (it != extra_setcode.end()) {
					int len = it->second.size();
					if (len > SIZE_SETCODE)
						len = SIZE_SETCODE;
					if (len)
						std::memcpy(cd.setcode, it->second.data(), len * sizeof(uint16_t));
				}
				else
					cd.set_setcode(setcode);
			}
			cd.type = static_cast<decltype(cd.type)>(sqlite3_column_int64(pStmt, 4));
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defense = sqlite3_column_int(pStmt, 6);
			if (cd.type & TYPE_LINK) {
				cd.link_marker = cd.defense;
				cd.defense = 0;
			}
			else
				cd.link_marker = 0;
			uint32_t level = static_cast<uint32_t>(sqlite3_column_int(pStmt, 7));
			cd.level = level & 0xff;
			cd.lscale = (level >> 24) & 0xff;
			cd.rscale = (level >> 16) & 0xff;
			cd.race = static_cast<decltype(cd.race)>(sqlite3_column_int64(pStmt, 8));
			cd.attribute = static_cast<decltype(cd.attribute)>(sqlite3_column_int64(pStmt, 9));
			cd.category = static_cast<decltype(cd.category)>(sqlite3_column_int64(pStmt, 10));
			_datas[cd.code] = cd;
			if (const char* text = (const char*)sqlite3_column_text(pStmt, 12)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.name = strBuffer;
			}
			if (const char* text = (const char*)sqlite3_column_text(pStmt, 13)) {
				BufferIO::DecodeUTF8(text, strBuffer);
				cs.text = strBuffer;
			}
			constexpr int desc_count = sizeof cs.desc / sizeof cs.desc[0];
			for (int i = 0; i < desc_count; ++i) {
				if (const char* text = (const char*)sqlite3_column_text(pStmt, i + 14)) {
					BufferIO::DecodeUTF8(text, strBuffer);
					cs.desc[i] = strBuffer;
				}
			}
			_strings[cd.code] = cs;
		}
		else if (step != SQLITE_DONE)
			return Error(pDB, pStmt);
	} while (step == SQLITE_ROW);
	sqlite3_finalize(pStmt);
	return true;
}
bool DataManager::LoadDB(const wchar_t* wfile) {
	char file[256];
	BufferIO::EncodeUTF8(wfile, file);
#ifdef _WIN32
	auto reader = FileSystem->createAndOpenFile(wfile);
#else
	auto reader = FileSystem->createAndOpenFile(file);
#endif
	if(reader == nullptr)
		return false;
	spmemvfs_db_t db;
	spmembuffer_t* mem = (spmembuffer_t*)calloc(sizeof(spmembuffer_t), 1);
	spmemvfs_env_init();
	mem->total = mem->used = reader->getSize();
	mem->data = (char*)malloc(mem->total + 1);
	reader->read(mem->data, mem->total);
	reader->drop();
	(mem->data)[mem->total] = '\0';
	bool ret{};
	if (spmemvfs_open_db(&db, file, mem) != SQLITE_OK)
		ret = Error(db.handle);
	else
		ret = ReadDB(db.handle);
	spmemvfs_close_db(&db);
	spmemvfs_env_fini();
	return ret;
}
bool DataManager::LoadStrings(const char* file) {
	FILE* fp = std::fopen(file, "r");
	if(!fp)
		return false;
	char linebuf[TEXT_LINE_SIZE]{};
	while(std::fgets(linebuf, sizeof linebuf, fp)) {
		ReadStringConfLine(linebuf);
	}
	std::fclose(fp);
	return true;
}
bool DataManager::LoadStrings(irr::io::IReadFile* reader) {
	char ch{};
	std::string linebuf;
	while (reader->read(&ch, 1)) {
		if (ch == '\0')
			break;
		linebuf.push_back(ch);
		if (ch == '\n' || linebuf.size() >= TEXT_LINE_SIZE - 1) {
			ReadStringConfLine(linebuf.data());
			linebuf.clear();
		}
	}
	reader->drop();
	return true;
}
void DataManager::ReadStringConfLine(const char* linebuf) {
	if(linebuf[0] != '!')
		return;
	char strbuf[TEXT_LINE_SIZE]{};
	int value{};
	wchar_t strBuffer[4096]{};
	if (std::sscanf(linebuf, "!%63s", strbuf) != 1)
		return;
	if(!std::strcmp(strbuf, "system")) {
		if (std::sscanf(&linebuf[7], "%d %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_sysStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "victory")) {
		if (std::sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_victoryStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "counter")) {
		if (std::sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_counterStrings[value] = strBuffer;
	} else if(!std::strcmp(strbuf, "setname")) {
		//using tab for comment
		if (std::sscanf(&linebuf[8], "%x %240[^\t\n]", &value, strbuf) != 2)
			return;
		BufferIO::DecodeUTF8(strbuf, strBuffer);
		_setnameStrings[value] = strBuffer;
	}
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	errmsg[0] = '\0';
	std::strncat(errmsg, sqlite3_errmsg(pDB), sizeof errmsg - 1);
	if(pStmt)
		sqlite3_finalize(pStmt);
	return false;
}
code_pointer DataManager::GetCodePointer(unsigned int code) const {
	return _datas.find(code);
}
string_pointer DataManager::GetStringPointer(unsigned int code) const {
	return _strings.find(code);
}
code_pointer DataManager::datas_begin() const {
	return _datas.cbegin();
}
code_pointer DataManager::datas_end() const {
	return _datas.cend();
}
string_pointer DataManager::strings_begin() const {
	return _strings.cbegin();
}
string_pointer DataManager::strings_end() const {
	return _strings.cend();
}
bool DataManager::GetData(unsigned int code, CardData* pData) const {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if (pData) {
		*pData = cdit->second;
	}
	return true;
}
bool DataManager::GetString(unsigned int code, CardString* pStr) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = unknown_string;
		pStr->text = unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
const wchar_t* DataManager::GetName(unsigned int code) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.name.empty())
		return csit->second.name.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetText(unsigned int code) const {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.text.empty())
		return csit->second.text.c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(unsigned int strCode) const {
	if (strCode < (MIN_CARD_ID << 4))
		return GetSysString(strCode);
	unsigned int code = (strCode >> 4) & 0x0fffffff;
	unsigned int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(!csit->second.desc[offset].empty())
		return csit->second.desc[offset].c_str();
	return unknown_string;
}
const wchar_t* DataManager::GetSysString(int code) const {
	if (code < 0 || code > MAX_STRING_ID)
		return unknown_string;
	auto csit = _sysStrings.find(code);
	if(csit == _sysStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetVictoryString(int code) const {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetCounterName(int code) const {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
const wchar_t* DataManager::GetSetName(int code) const {
	auto csit = _setnameStrings.find(code);
	if(csit == _setnameStrings.end())
		return unknown_string;
	return csit->second.c_str();
}
std::vector<unsigned int> DataManager::GetSetCodes(std::wstring setname) const {
	std::vector<unsigned int> matchingCodes;
	for(auto csit = _setnameStrings.begin(); csit != _setnameStrings.end(); ++csit) {
		auto xpos = csit->second.find_first_of(L'|');//setname|another setname or extra info
		if(setname.size() < 2) {
			if(csit->second.compare(0, xpos, setname) == 0
				|| csit->second.compare(xpos + 1, csit->second.length(), setname) == 0)
				matchingCodes.push_back(csit->first);
		} else {
			if(csit->second.substr(0, xpos).find(setname) != std::wstring::npos
				|| csit->second.substr(xpos + 1).find(setname) != std::wstring::npos) {
				matchingCodes.push_back(csit->first);
			}
		}
	}
	return matchingCodes;
}
std::wstring DataManager::GetNumString(int num, bool bracket) const {
	if(!bracket)
		return std::to_wstring(num);
	std::wstring numBuffer{ L"(" };
	numBuffer.append(std::to_wstring(num));
	numBuffer.push_back(L')');
	return numBuffer;
}
const wchar_t* DataManager::FormatLocation(int location, int sequence) const {
	if(location == LOCATION_SZONE) {
		if(sequence < 5)
			return GetSysString(1003);
		else if(sequence == 5)
			return GetSysString(1008);
		else
			return GetSysString(1009);
	}
	int i = 1000;
	int string_id = 0;
	for (unsigned filter = LOCATION_DECK; filter <= LOCATION_PZONE; filter <<= 1, ++i) {
		if (filter == location) {
			string_id = i;
			break;
		}
	}
	if (string_id)
		return GetSysString(string_id);
	else
		return unknown_string;
}
std::wstring DataManager::FormatAttribute(unsigned int attribute) const {
	std::wstring buffer;
	for (int i = 0; i < ATTRIBUTES_COUNT; ++i) {
		if (attribute & (0x1U << i)) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(1010 + i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatRace(unsigned int race) const {
	std::wstring buffer;
	for(int i = 0; i < RACES_COUNT; ++i) {
		if(race & (0x1U << i)) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(1020 + i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatType(unsigned int type) const {
	std::wstring buffer;
	int i = 1050;
	for (unsigned filter = TYPE_MONSTER; filter <= TYPE_LINK; filter <<= 1, ++i) {
		if (type & filter) {
			if (!buffer.empty())
				buffer.push_back(L'|');
			buffer.append(GetSysString(i));
		}
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatSetName(const uint16_t setcode[]) const {
	std::wstring buffer;
	for(int i = 0; i < 10; ++i) {
		if (!setcode[i])
			break;
		const wchar_t* setname = GetSetName(setcode[i]);
		if (!buffer.empty())
			buffer.push_back(L'|');
		buffer.append(setname);
	}
	if (buffer.empty())
		return std::wstring(unknown_string);
	return buffer;
}
std::wstring DataManager::FormatLinkMarker(unsigned int link_marker) const {
	std::wstring buffer;
	if (link_marker & LINK_MARKER_TOP_LEFT)
		buffer.append(L"[\u2196]");
	if (link_marker & LINK_MARKER_TOP)
		buffer.append(L"[\u2191]");
	if (link_marker & LINK_MARKER_TOP_RIGHT)
		buffer.append(L"[\u2197]");
	if (link_marker & LINK_MARKER_LEFT)
		buffer.append(L"[\u2190]");
	if (link_marker & LINK_MARKER_RIGHT)
		buffer.append(L"[\u2192]");
	if (link_marker & LINK_MARKER_BOTTOM_LEFT)
		buffer.append(L"[\u2199]");
	if (link_marker & LINK_MARKER_BOTTOM)
		buffer.append(L"[\u2193]");
	if (link_marker & LINK_MARKER_BOTTOM_RIGHT)
		buffer.append(L"[\u2198]");
	return buffer;
}
uint32_t DataManager::CardReader(uint32_t code, card_data* pData) {
	if (!dataManager.GetData(code, pData))
		pData->clear();
	return 0;
}
unsigned char* DataManager::ScriptReaderEx(const char* script_path, int* slen) {
	// default script name: ./script/c%d.lua
	if (std::strncmp(script_path, "./script", 8) != 0) // not a card script file
		return ReadScriptFromFile(script_path, slen);
	const char* script_name = script_path + 2;
	char expansions_path[1024]{};
	std::snprintf(expansions_path, sizeof expansions_path, "./expansions/%s", script_name);
	if (mainGame->gameConf.prefer_expansion_script) { // debug script with raw file in expansions
		if (ReadScriptFromFile(expansions_path, slen))
			return scriptBuffer;
		if (ReadScriptFromIrrFS(script_name, slen))
			return scriptBuffer;
		if (ReadScriptFromFile(script_path, slen))
			return scriptBuffer;
	} else {
		if (ReadScriptFromIrrFS(script_name, slen))
			return scriptBuffer;
		if (ReadScriptFromFile(script_path, slen))
			return scriptBuffer;
		if (ReadScriptFromFile(expansions_path, slen))
			return scriptBuffer;
	}

	return nullptr;
}
unsigned char* DataManager::ReadScriptFromIrrFS(const char* script_name, int* slen) {
#ifdef _WIN32
	wchar_t fname[256]{};
	BufferIO::DecodeUTF8(script_name, fname);
	auto reader = FileSystem->createAndOpenFile(fname);
#else
	auto reader = FileSystem->createAndOpenFile(script_name);
#endif
	if (!reader)
		return nullptr;
	int size = reader->read(scriptBuffer, sizeof scriptBuffer);
	reader->drop();
	if (size >= (int)sizeof scriptBuffer)
		return nullptr;
	*slen = size;
	return scriptBuffer;
}
unsigned char* DataManager::ReadScriptFromFile(const char* script_name, int* slen) {
	wchar_t fname[256]{};
	BufferIO::DecodeUTF8(script_name, fname);
	FILE* fp = mywfopen(fname, "rb");
	if (!fp)
		return nullptr;
	size_t len = std::fread(scriptBuffer, 1, sizeof scriptBuffer, fp);
	std::fclose(fp);
	if (len >= sizeof scriptBuffer)
		return nullptr;
	*slen = (int)len;
	return scriptBuffer;
}
bool DataManager::deck_sort_lv(code_pointer p1, code_pointer p2) {
	if ((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if ((p1->second.type & 0x7) == 1) {
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if (type1 != type2)
			return type1 < type2;
		if (p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		if (p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if (p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		return p1->first < p2->first;
	}
	if ((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool DataManager::deck_sort_atk(code_pointer p1, code_pointer p2) {
	if ((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if ((p1->second.type & 0x7) == 1) {
		if (p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if (p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		if (p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if (type1 != type2)
			return type1 < type2;
		return p1->first < p2->first;
	}
	if ((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool DataManager::deck_sort_def(code_pointer p1, code_pointer p2) {
	if ((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if ((p1->second.type & 0x7) == 1) {
		if (p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		if (p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if (p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if (type1 != type2)
			return type1 < type2;
		return p1->first < p2->first;
	}
	if ((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool DataManager::deck_sort_name(code_pointer p1, code_pointer p2) {
	const wchar_t* name1 = dataManager.GetName(p1->first);
	const wchar_t* name2 = dataManager.GetName(p2->first);
	int res = std::wcscmp(name1, name2);
	if (res != 0)
		return res < 0;
	return p1->first < p2->first;
}

}
