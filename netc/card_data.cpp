#include "utils/common.h"
#include "utils/convert.h"
#include <sqlite3.h>

#include "config.h"
#include "card_data.h"
#include "scene_mgr.h"

namespace ygopro
{

    bool CardData::CheckCondition(const FilterCondition& fc) {
        if(type & 0x4000)
            return false;
        if(fc.code != 0 && code != fc.code && alias != fc.code)
            return false;
        if(fc.type != 0 && (type & fc.type) != fc.type)
            return false;
        if(fc.subtype != 0 && (type & fc.subtype) == 0)
            return false;
        if(fc.pool != 0 && (pool & fc.pool) == 0)
            return false;
        if((fc.type == 0) || (fc.type & 0x1)) {
            if(fc.atkmin != -1 && attack < fc.atkmin)
                return false;
            if(fc.atkmax != -1 && fc.atkmax != 0 && attack > fc.atkmax)
                return false;
            if(fc.defmin != -1 && defence < fc.defmin)
                return false;
            if(fc.defmax != -1 && fc.defmax != 0 && defence > fc.defmax)
                return false;
            if(fc.lvmin != 0 && star < (uint32_t)fc.lvmin)
                return false;
            if(fc.lvmax != 0 && star > (uint32_t)fc.lvmax)
                return false;
            if(fc.race != 0 && (race & fc.race) == 0)
                return false;
            if(fc.attribute != 0 && (attribute & fc.attribute) == 0)
                return false;
            if(fc.scalemin != 0 && lscale < (uint32_t)fc.scalemin)
                return false;
            if(fc.scalemax != 0 && (lscale == 0 || lscale > (uint32_t)fc.scalemax))
                return false;
        }
        if(fc.setcode != 0) {
            uint64_t sc = setcode;
            if(alias) {
                CardData* adata = DataMgr::Get()[alias];
                if(adata)
                    sc = adata->setcode;
            }
            uint32_t scode = fc.setcode & 0xfff;
            uint32_t subcode = fc.setcode >> 12;
            for(int32_t i = 0; i < 64; i += 16) {
                uint32_t partc = (sc >> i) & 0xffff;
                if(partc && ((partc & 0xfff) == scode) && ((partc >> 12) & subcode) == subcode)
                    return true;
            }
            return false;
        } else {
            if(fc.keywords.size()) {
                static auto charcmp = [](wchar_t ch1, wchar_t ch2) -> bool { return toupper(ch1) == toupper(ch2); };
                for(auto& kw : fc.keywords) {
                    if((std::search(name.begin(), name.end(), kw.begin(), kw.end(), charcmp) != name.end())
                       || (std::search(texts.begin(), texts.end(), kw.begin(), kw.end(), charcmp) != texts.end())) {
                        return true;
                    }
                }
                return false;
            }
        }
        return true;
    }
    
    bool CardData::card_sort(const CardData* p1, const CardData* p2) {
        if((p1->type & 0x7) != (p2->type & 0x7))
            return (p1->type & 0x7) < (p2->type & 0x7);
        if((p1->type & 0x7) == 1) {
            int32_t type1 = (p1->type & 0x8020c0) ? (p1->type & 0x8020c1) : (p1->type & 0x31);
            int32_t type2 = (p2->type & 0x8020c0) ? (p2->type & 0x8020c1) : (p2->type & 0x31);
            if(type1 != type2)
                return type1 < type2;
            if(p1->star != p2->star)
                return p1->star > p2->star;
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
    
	int32_t DataMgr::LoadDatas(const std::string& file) {
		_datas.clear();
		sqlite3* pDB;
		if(sqlite3_open(file.c_str(), &pDB) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		sqlite3_stmt* pStmt;
		const char* sql = "select * from datas,texts where datas.id=texts.id";
		if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
			return sqlite3_errcode(pDB);
		CardData cd;
		int32_t step = 0;
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
				cd.star = sqlite3_column_int(pStmt, 7);
                cd.lscale = (cd.star >> 16) & 0xff;
                cd.rscale = (cd.star >> 24) & 0xff;
                cd.star &= 0xffff;
				cd.race = sqlite3_column_int(pStmt, 8);
				cd.attribute = sqlite3_column_int(pStmt, 9);
				cd.category = sqlite3_column_int(pStmt, 10);
                std::string str((const char*)sqlite3_column_text(pStmt, 12));
                cd.name = To<std::wstring>(str);
                cd.texts = To<std::wstring>(std::string((const char*)sqlite3_column_text(pStmt, 13)));
                for(uint32_t i = 0; i < 16; ++i)
                    cd.desc[i] = To<std::wstring>(std::string((const char*)sqlite3_column_text(pStmt, 14 + i)));
                _datas.insert(std::make_pair(cd.code, cd));
                if(cd.alias)
                    _aliases[cd.alias].push_back(cd.code);
			}
		} while(step != SQLITE_DONE);
		sqlite3_finalize(pStmt);
		sqlite3_close(pDB);
		return SQLITE_OK;
	}

	CardData* DataMgr:: operator [] (uint32_t code) {
		auto iter = _datas.find(code);
		if(iter == _datas.end())
			return nullptr;
		return &iter->second;
	}
    
    std::vector<uint32_t> DataMgr::AllAliases(uint32_t code) {
        std::vector<uint32_t> result;
        auto iter = _aliases.find(code);
        if(iter == _aliases.end())
            return result;
        else {
            result = iter->second;
            return result;
        }
    }

    std::vector<CardData*> DataMgr::FilterCard(const FilterCondition& fc) {
        std::vector<CardData*> result;
        for(auto& iter : _datas) {
            CardData& cd = iter.second;
            if(cd.CheckCondition(fc))
                result.push_back(&cd);
        }
        if(result.size())
            std::sort(result.begin(), result.end(), CardData::card_sort);
        return result;
    }
    
    std::wstring DataMgr::GetAttributeString(uint32_t attr) {
        std::string attname;
        if(attr & 0x1)
            attname.append(stringCfg["attribute_earth"].to_string()).append("|");
        if(attr & 0x2)
            attname.append(stringCfg["attribute_water"].to_string()).append("|");
        if(attr & 0x4)
            attname.append(stringCfg["attribute_fire"].to_string()).append("|");
        if(attr & 0x8)
            attname.append(stringCfg["attribute_wind"].to_string()).append("|");
        if(attr & 0x10)
            attname.append(stringCfg["attribute_light"].to_string()).append("|");
        if(attr & 0x20)
            attname.append(stringCfg["attribute_dark"].to_string()).append("|");
        if(attr & 0x40)
            attname.append(stringCfg["attribute_divine"].to_string()).append("|");
        if(attname.length() > 0)
            attname.resize(attname.length() - 1);
        return To<std::wstring>(attname);
    }
    
    std::wstring DataMgr::GetRaceString(uint32_t race) {
        std::string racname;
        if(race & 0x1)
            racname.append(stringCfg["race_warrior"].to_string()).append("|");
        if(race & 0x2)
            racname.append(stringCfg["race_spellcaster"].to_string()).append("|");
        if(race & 0x4)
            racname.append(stringCfg["race_fairy"].to_string()).append("|");
        if(race & 0x8)
            racname.append(stringCfg["race_fiend"].to_string()).append("|");
        if(race & 0x10)
            racname.append(stringCfg["race_zombie"].to_string()).append("|");
        if(race & 0x20)
            racname.append(stringCfg["race_machine"].to_string()).append("|");
        if(race & 0x40)
            racname.append(stringCfg["race_aqua"].to_string()).append("|");
        if(race & 0x80)
            racname.append(stringCfg["race_pyro"].to_string()).append("|");
        if(race & 0x100)
            racname.append(stringCfg["race_rock"].to_string()).append("|");
        if(race & 0x200)
            racname.append(stringCfg["race_windbeast"].to_string()).append("|");
        if(race & 0x400)
            racname.append(stringCfg["race_plant"].to_string()).append("|");
        if(race & 0x800)
            racname.append(stringCfg["race_insect"].to_string()).append("|");
        if(race & 0x1000)
            racname.append(stringCfg["race_thunder"].to_string()).append("|");
        if(race & 0x2000)
            racname.append(stringCfg["race_dragon"].to_string()).append("|");
        if(race & 0x4000)
            racname.append(stringCfg["race_beast"].to_string()).append("|");
        if(race & 0x8000)
            racname.append(stringCfg["race_beastwarrior"].to_string()).append("|");
        if(race & 0x10000)
            racname.append(stringCfg["race_dinosaur"].to_string()).append("|");
        if(race & 0x20000)
            racname.append(stringCfg["race_fish"].to_string()).append("|");
        if(race & 0x40000)
            racname.append(stringCfg["race_seaserpent"].to_string()).append("|");
        if(race & 0x80000)
            racname.append(stringCfg["race_reptile"].to_string()).append("|");
        if(race & 0x100000)
            racname.append(stringCfg["race_psychic"].to_string()).append("|");
        if(race & 0x200000)
            racname.append(stringCfg["race_divine"].to_string()).append("|");
        if(race & 0x400000)
            racname.append(stringCfg["race_creatorgod"].to_string()).append("|");
        if(race & 0x800000)
            racname.append(stringCfg["race_phantomdragon"].to_string()).append("|");
        if(racname.length() > 0)
            racname.resize(racname.length() - 1);
        return To<std::wstring>(racname);
    }
    
    std::wstring DataMgr::GetTypeString(uint32_t arctype) {
        std::string tpname;
        if(arctype & 0x1) {
            if(arctype & 0x8020c0) {
                if(arctype & 0x800000)
                    tpname.append(stringCfg["type_xyz"].to_string());
                else if(arctype & 0x2000)
                    tpname.append(stringCfg["type_synchro"].to_string());
                else if(arctype & 0x80)
                    tpname.append(stringCfg["type_ritual"].to_string());
                else
                    tpname.append(stringCfg["type_fusion"].to_string());
                tpname.append(stringCfg["type_monster"].to_string());
                if(arctype & 0x20)
                    tpname.append("|").append(stringCfg["type_effect"].to_string());
            } else {
                if(arctype & 0x20)
                    tpname.append(stringCfg["type_effect"].to_string());
                else
                    tpname .append(stringCfg["type_normal"].to_string());
                tpname.append(stringCfg["type_monster"].to_string());
            }
            if(arctype & 0x200)
                tpname.append("|").append(stringCfg["type_spirit"].to_string());
            if(arctype & 0x400)
                tpname.append("|").append(stringCfg["type_union"].to_string());
            if(arctype & 0x800)
                tpname.append("|").append(stringCfg["type_dual"].to_string());
            if(arctype & 0x1000)
                tpname.append("|").append(stringCfg["type_tuner"].to_string());
            if(arctype & 0x4000)
                tpname.append("|").append(stringCfg["type_token"].to_string());
            if(arctype & 0x200000)
                tpname.append("|").append(stringCfg["type_flip"].to_string());
            if(arctype & 0x400000)
                tpname.append("|").append(stringCfg["type_toon"].to_string());
            if(arctype & 0x1000000)
                tpname.append("|").append(stringCfg["type_pendulum"].to_string());
        } else if(arctype & 0x2) {
            if(arctype == 0x2)
                tpname.append(stringCfg["type_normal"].to_string());
            else if(arctype & 0x80)
                tpname.append(stringCfg["type_ritual"].to_string());
            else if(arctype & 0x10000)
                tpname.append(stringCfg["type_quickplay"].to_string());
            else if(arctype & 0x20000)
                tpname.append(stringCfg["type_continuous"].to_string());
            else if(arctype & 0x40000)
                tpname.append(stringCfg["type_equip"].to_string());
            else if(arctype & 0x80000)
                tpname.append(stringCfg["type_field"].to_string());
            tpname.append(stringCfg["type_spell"].to_string());
        } else {
            if(arctype == 0x4)
                tpname.append(stringCfg["type_normal"].to_string());
            else if(arctype & 0x20000)
                tpname.append(stringCfg["type_continuous"].to_string());
            else if(arctype & 0x100000)
                tpname.append(stringCfg["type_counter"].to_string());
            tpname.append(stringCfg["type_trap"].to_string());
        }
        return To<std::wstring>(tpname);
    }
    
}
