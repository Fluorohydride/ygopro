#include "card_data.h"
#include "game_frame.h"
#include "../common/bufferutil.h"
#include "../sqlite3/sqlite3.h"

namespace ygopro
{

	DataMgr dataMgr;

	int DataMgr::LoadDatas(const wxString& file) {
		_datas.clear();
		sqlite3* pDB;
		if(sqlite3_open(file.c_str(), &pDB) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		sqlite3_stmt* pStmt;
		const char* sql = "select * from datas,texts where datas.id=texts.id";
		if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		CardData cd;
		wchar_t unicode_buffer[2048];
        int full_width_space = commonCfg["full_width_space"];
        wxString space = stringCfg["full_width_space"];
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
				len = BufferUtil::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), unicode_buffer);
				unicode_buffer[len] = 0;
				cd.name = unicode_buffer;
                if(full_width_space)
                    cd.name.Replace(wxT(" "), space);
				len = BufferUtil::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), unicode_buffer);
				unicode_buffer[len] = 0;
				cd.texts = unicode_buffer;
                if(full_width_space)
                    cd.texts.Replace(wxT(" "), space);
                for(unsigned int i = 0; i < 16; ++i) {
                    len = BufferUtil::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 14 + i), unicode_buffer);
                    unicode_buffer[len] = 0;
                    cd.desc[i] = unicode_buffer;
                }
                _datas.insert(std::make_pair(cd.code, cd));
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
    
    wxString DataMgr::GetAttributeString(unsigned int attr) {
        wxString attname;
        if(attr & 0x1)
            attname += static_cast<const std::string&>(stringCfg["attribtue_earth"]) + wxT("|");
        if(attr & 0x2)
            attname += static_cast<const std::string&>(stringCfg["attribtue_water"]) + wxT("|");
        if(attr & 0x4)
            attname += static_cast<const std::string&>(stringCfg["attribtue_fire"]) + wxT("|");
        if(attr & 0x8)
            attname += static_cast<const std::string&>(stringCfg["attribtue_wind"]) + wxT("|");
        if(attr & 0x10)
            attname += static_cast<const std::string&>(stringCfg["attribtue_light"]) + wxT("|");
        if(attr & 0x20)
            attname += static_cast<const std::string&>(stringCfg["attribtue_dark"]) + wxT("|");
        if(attr & 0x40)
            attname += static_cast<const std::string&>(stringCfg["attribtue_divine"]) + wxT("|");
        attname.resize(attname.length() - 1);
        return attname;
    }
    
    wxString DataMgr::GetRaceString(unsigned int race) {
        wxString racname;
        if(race & 0x1)
            racname += static_cast<const std::string&>(stringCfg["race_warrior"]) + wxT("|");
        if(race & 0x2)
            racname += static_cast<const std::string&>(stringCfg["race_spellcaster"]) + wxT("|");
        if(race & 0x4)
            racname += static_cast<const std::string&>(stringCfg["race_fairy"]) + wxT("|");
        if(race & 0x8)
            racname += static_cast<const std::string&>(stringCfg["race_fiend"]) + wxT("|");
        if(race & 0x10)
            racname += static_cast<const std::string&>(stringCfg["race_zombie"]) + wxT("|");
        if(race & 0x20)
            racname += static_cast<const std::string&>(stringCfg["race_machine"]) + wxT("|");
        if(race & 0x40)
            racname += static_cast<const std::string&>(stringCfg["race_aqua"]) + wxT("|");
        if(race & 0x80)
            racname += static_cast<const std::string&>(stringCfg["race_pyro"]) + wxT("|");
        if(race & 0x100)
            racname += static_cast<const std::string&>(stringCfg["race_rock"]) + wxT("|");
        if(race & 0x200)
            racname += static_cast<const std::string&>(stringCfg["race_windbeast"]) + wxT("|");
        if(race & 0x400)
            racname += static_cast<const std::string&>(stringCfg["race_plant"]) + wxT("|");
        if(race & 0x800)
            racname += static_cast<const std::string&>(stringCfg["race_insect"]) + wxT("|");
        if(race & 0x1000)
            racname += static_cast<const std::string&>(stringCfg["race_thunder"]) + wxT("|");
        if(race & 0x2000)
            racname += static_cast<const std::string&>(stringCfg["race_dragon"]) + wxT("|");
        if(race & 0x4000)
            racname += static_cast<const std::string&>(stringCfg["race_beast"]) + wxT("|");
        if(race & 0x8000)
            racname += static_cast<const std::string&>(stringCfg["race_beastwarrior"]) + wxT("|");
        if(race & 0x10000)
            racname += static_cast<const std::string&>(stringCfg["race_dinosaur"]) + wxT("|");
        if(race & 0x20000)
            racname += static_cast<const std::string&>(stringCfg["race_fish"]) + wxT("|");
        if(race & 0x40000)
            racname += static_cast<const std::string&>(stringCfg["race_seaserpent"]) + wxT("|");
        if(race & 0x80000)
            racname += static_cast<const std::string&>(stringCfg["race_reptile"]) + wxT("|");
        if(race & 0x100000)
            racname += static_cast<const std::string&>(stringCfg["race_psychic"]) + wxT("|");
        if(race & 0x200000)
            racname += static_cast<const std::string&>(stringCfg["race_divine"]) + wxT("|");
        if(race & 0x400000)
            racname += static_cast<const std::string&>(stringCfg["race_creatorgod"]) + wxT("|");
        racname.resize(racname.length() - 1);
        return racname;
    }
    
    wxString DataMgr::GetTypeString(unsigned int arctype) {
        wxString tpname;
        if(arctype & 0x1) {
            if(arctype & 0x8020c0) {
                if(arctype & 0x800000)
                    tpname = static_cast<const std::string&>(stringCfg["type_xyz"]);
                else if(arctype & 0x2000)
                    tpname = static_cast<const std::string&>(stringCfg["type_synchro"]);
                else if(arctype & 0x80)
                    tpname = static_cast<const std::string&>(stringCfg["type_ritual"]);
                else
                    tpname = static_cast<const std::string&>(stringCfg["type_fusion"]);
                tpname += static_cast<const std::string&>(stringCfg["type_monster"]);
                if(arctype & 0x20)
                    tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_effect"]);
            } else {
                if(arctype & 0x20)
                    tpname = static_cast<const std::string&>(stringCfg["type_effect"]);
                else
                    tpname = static_cast<const std::string&>(stringCfg["type_normal"]);
                tpname += static_cast<const std::string&>(stringCfg["type_monster"]);
            }
            if(arctype & 0x200)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_spirit"]);
            if(arctype & 0x400)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_union"]);
            if(arctype & 0x800)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_dual"]);
            if(arctype & 0x1000)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_tuner"]);
            if(arctype & 0x4000)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_token"]);
            if(arctype & 0x200000)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_flip"]);
            if(arctype & 0x400000)
                tpname += wxT("|") + static_cast<const std::string&>(stringCfg["type_toon"]);
        } else if(arctype & 0x2) {
            if(arctype == 0x2)
                tpname = static_cast<const std::string&>(stringCfg["type_normal"]);
            else if(arctype & 0x80)
                tpname = static_cast<const std::string&>(stringCfg["type_ritual"]);
            else if(arctype & 0x10000)
                tpname = static_cast<const std::string&>(stringCfg["type_quickplay"]);
            else if(arctype & 0x20000)
                tpname = static_cast<const std::string&>(stringCfg["type_continuous"]);
            else if(arctype & 0x40000)
                tpname = static_cast<const std::string&>(stringCfg["type_equip"]);
            else if(arctype & 0x80000)
                tpname = static_cast<const std::string&>(stringCfg["type_field"]);
            tpname += static_cast<const std::string&>(stringCfg["type_spell"]);
        } else {
            if(arctype == 0x4)
                tpname = static_cast<const std::string&>(stringCfg["type_normal"]);
            else if(arctype & 0x20000)
                tpname = static_cast<const std::string&>(stringCfg["type_continuous"]);
            else if(arctype & 0x100000)
                tpname = static_cast<const std::string&>(stringCfg["type_counter"]);
            tpname += static_cast<const std::string&>(stringCfg["type_trap"]);
        }
        return tpname;
    }
    
}
