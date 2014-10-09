#include "../common/common.h"

#include <sqlite3.h>

#include "card_data_s.h"


namespace ygopro
{
    
    DataMgrS dataMgrs;
    
    void DataMgrS::LockData() {
        _mlock.lock();
    }
    
    void DataMgrS::UnlockData() {
        _mlock.unlock();
    }
    
    int DataMgrS::LoadDatas(const std::string& file) {
		_datas.clear();
		sqlite3* pDB;
		if(sqlite3_open(file.c_str(), &pDB) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		sqlite3_stmt* pStmt;
		const char* sql = "select * from datas";
		if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
			return sqlite3_errcode(pDB);
        _dbfile = file;
		card_data cd;
		int step = 0;
		do {
			step = sqlite3_step(pStmt);
			if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
				return sqlite3_errcode(pDB);
			else if(step == SQLITE_ROW) {
				cd.code = sqlite3_column_int(pStmt, 0);
				cd.alias = sqlite3_column_int(pStmt, 2);
				cd.setcode = sqlite3_column_int64(pStmt, 3);
				cd.type = sqlite3_column_int(pStmt, 4);
				cd.attack = sqlite3_column_int(pStmt, 5);
				cd.defence = sqlite3_column_int(pStmt, 6);
				cd.level = sqlite3_column_int(pStmt, 7);
                cd.lscale = (cd.level >> 16) & 0xff;
                cd.rscale = (cd.level >> 24) & 0xff;
                cd.level = cd.level & 0xffff;
				cd.race = sqlite3_column_int(pStmt, 8);
				cd.attribute = sqlite3_column_int(pStmt, 9);
			}
		} while(step != SQLITE_DONE);
		sqlite3_finalize(pStmt);
		sqlite3_close(pDB);
		return SQLITE_OK;
	}
    
    void DataMgrS::ReloadDatas() {
        LockData();
        LoadDatas(_dbfile);
        UnlockData();
    }
    
	card_data* DataMgrS:: operator [] (uint32 code) {
		auto iter = _datas.find(code);
		if(iter == _datas.end())
			return nullptr;
		return &iter->second;
	}
    
}
