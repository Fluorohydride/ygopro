#include "card_data.h"
#include "game_frame.h"
#include "../common/bufferutil.h"
#include "../sqlite3/sqlite3.h"

namespace ygopro
{

	DataMgr dataMgr;

    bool CardData::CheckCondition(const FilterCondition& fc, const wxString& keyword, bool check_desc) {
        if(type & 0x4000)
            return false;
        if(fc.code != 0 && code != fc.code && alias != fc.code)
            return false;
        if(fc.type != 0 && (type & fc.type) != fc.type)
            return false;
        if(fc.subtype != 0 && (type & fc.subtype) == 0)
            return false;
        if(fc.pool != 0 && pool != fc.pool)
            return false;
        if(fc.type & 0x1) {
            if(fc.atkmin != -1 && attack < fc.atkmin)
                return false;
            if(fc.atkmax != -1 && attack > fc.atkmax)
                return false;
            if(fc.defmin != -1 && defence < fc.defmin)
                return false;
            if(fc.defmax != -1 && defence > fc.defmax)
                return false;
            if(fc.lvmin != 0 && level < (unsigned int)fc.lvmin)
                return false;
            if(fc.lvmax != 0 && level > (unsigned int)fc.lvmax)
                return false;
            if(fc.race != 0 && (race & fc.race) == 0)
                return false;
            if(fc.attribute != 0 && (attribute & fc.attribute) == 0)
                return false;
        }
        if(fc.category != 0 && (category & fc.category) == 0)
            return false;
        if(fc.setcode != 0) {
            unsigned long long sc = setcode;
            if(alias) {
                CardData* adata = dataMgr[alias];
                if(adata)
                    sc = adata->setcode;
            }
            unsigned int scode = fc.setcode & 0xfff;
            unsigned int subcode = fc.setcode >> 12;
            for(int i = 0; i < 64; i += 16) {
                unsigned int partc = (sc >> i) & 0xffff;
                if(partc && ((partc & 0xfff) == scode) && ((partc >> 12) & subcode) == subcode)
                    return true;
            }
            return false;
        } else {
            if(keyword.size()) {
                wxString kw = keyword.Upper();
                if(name.Upper().Find(kw) == -1 && (!check_desc || texts.Upper().Find(kw) == -1))
                    return false;
            }
        }
        return true;
    }
    
    bool CardData::card_sort(const CardData* p1, const CardData* p2) {
        if((p1->type & 0x7) != (p2->type & 0x7))
            return (p1->type & 0x7) < (p2->type & 0x7);
        if((p1->type & 0x7) == 1) {
            int type1 = (p1->type & 0x8020c0) ? (p1->type & 0x8020c1) : (p1->type & 0x31);
            int type2 = (p2->type & 0x8020c0) ? (p2->type & 0x8020c1) : (p2->type & 0x31);
            if(type1 != type2)
                return type1 < type2;
            if(p1->level != p2->level)
                return p1->level > p2->level;
            if(p1->attack != p2->attack)
                return p1->attack > p2->attack;
            if(p1->defence != p2->defence)
                return p1->defence > p2->defence;
            else return p1->code < p2->code;
        }
        if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
            return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
        return p1->code < p2->code;
    }
    
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
				cd.pool = sqlite3_column_int(pStmt, 1);
				cd.alias = sqlite3_column_int(pStmt, 2);
				cd.setcode = sqlite3_column_int64(pStmt, 3);
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
                if(cd.alias)
                    _aliases[cd.alias].push_back(cd.code);
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
    
    std::vector<unsigned int> DataMgr::AllAliases(unsigned int code) {
        std::vector<unsigned int> result;
        auto iter = _aliases.find(code);
        if(iter == _aliases.end())
            return std::move(result);
        else {
            result = iter->second;
            return std::move(result);
        }
    }

    std::vector<CardData*> DataMgr::FilterCard(const FilterCondition& fc, const wxString& fs, bool check_desc) {
        std::vector<CardData*> result;
        for(auto& iter : _datas) {
            CardData& cd = iter.second;
            if(cd.CheckCondition(fc, fs, check_desc))
                result.push_back(&cd);
        }
        if(result.size())
            std::sort(result.begin(), result.end(), CardData::card_sort);
        return std::move(result);
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
        return std::move(attname);
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
        return std::move(racname);
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
        return std::move(tpname);
    }
    
    wxString DataMgr::GetTypeString2(unsigned int arctype) {
        switch(arctype) {
            case 0x1: return static_cast<const std::string&>(stringCfg["type_monster"]);
            case 0x2: return static_cast<const std::string&>(stringCfg["type_spell"]);
            case 0x4: return static_cast<const std::string&>(stringCfg["type_trap"]);
            case 0x10: return static_cast<const std::string&>(stringCfg["type_normal"]);
            case 0x20: return static_cast<const std::string&>(stringCfg["type_effect"]);
            case 0x40: return static_cast<const std::string&>(stringCfg["type_fusion"]);
            case 0x80: return static_cast<const std::string&>(stringCfg["type_ritual"]);
            case 0x100: return static_cast<const std::string&>(stringCfg["type_trapmonster"]);
            case 0x200: return static_cast<const std::string&>(stringCfg["type_spirit"]);
            case 0x400: return static_cast<const std::string&>(stringCfg["type_union"]);
            case 0x800: return static_cast<const std::string&>(stringCfg["type_dual"]);
            case 0x1000: return static_cast<const std::string&>(stringCfg["type_tuner"]);
            case 0x2000: return static_cast<const std::string&>(stringCfg["type_synchro"]);
            case 0x4000: return static_cast<const std::string&>(stringCfg["type_token"]);
            case 0x10000: return static_cast<const std::string&>(stringCfg["type_quickplay"]);
            case 0x20000: return static_cast<const std::string&>(stringCfg["type_continuous"]);
            case 0x40000: return static_cast<const std::string&>(stringCfg["type_equip"]);
            case 0x80000: return static_cast<const std::string&>(stringCfg["type_field"]);
            case 0x100000: return static_cast<const std::string&>(stringCfg["type_counter"]);
            case 0x200000: return static_cast<const std::string&>(stringCfg["type_flip"]);
            case 0x400000: return static_cast<const std::string&>(stringCfg["type_toon"]);
            case 0x800000: return static_cast<const std::string&>(stringCfg["type_xyz"]);
        }
        return wxT("");
    }
    
}
