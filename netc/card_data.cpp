#include "card_data.h"
#include "../common/bufferutil.h"
#include "../sqlite3/sqlite3.h"

namespace ygopro
{

	DataMgr dataMgr;

	int DataMgr::LoadDatas(const char* file) {
		_datas.clear();
		sqlite3* pDB;
		if(sqlite3_open(file, &pDB) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		sqlite3_stmt* pStmt;
		const char* sql = "select * from datas,texts where datas.id=texts.id";
		if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		CardData cd;
		wchar_t unicode_buffer[2048];
		int step = 0, len = 0;
		do {
			step = sqlite3_step(pStmt);
			if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
				return sqlite3_errcode(pDB);
			else if(step == SQLITE_ROW) {
				cd.code = sqlite3_column_int(pStmt, 0);
				cd.ot = sqlite3_column_int(pStmt, 1);
				cd.alias = sqlite3_column_int(pStmt, 2);
				cd.setcode = sqlite3_column_int(pStmt, 3);
				cd.type = sqlite3_column_int(pStmt, 4);
				cd.attack = sqlite3_column_int(pStmt, 5);
				cd.defence = sqlite3_column_int(pStmt, 6);
				cd.level = sqlite3_column_int(pStmt, 7);
				cd.race = sqlite3_column_int(pStmt, 8);
				cd.attribute = sqlite3_column_int(pStmt, 9);
				cd.category = sqlite3_column_int(pStmt, 10);
				_datas.insert(std::make_pair(cd.code, cd));
				len = BufferUtil::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), unicode_buffer);
				unicode_buffer[len] = 0;
				cd.name = unicode_buffer;
				len = BufferUtil::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), unicode_buffer);
				unicode_buffer[len] = 0;
				cd.texts = unicode_buffer;
			}
		} while(step != SQLITE_DONE);
		sqlite3_finalize(pStmt);
		sqlite3_close(pDB);
		return SQLITE_OK;
	}

	CardData* DataMgr:: operator [] (unsigned int code) {
		auto iter = _datas.find(code);
		if(iter == _datas.end())
			return nullptr;
		return &iter->second;
	}

}
