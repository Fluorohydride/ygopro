#ifndef _CARD_DATA_H_
#define _CARD_DATA_H_

#include "utils/singleton.h"

namespace ygopro
{

    struct FilterCondition {
        uint32_t code = 0;
        uint32_t setcode = 0;
        uint32_t pool = 0;
        uint32_t type = 0;
        uint32_t subtype = 0;
        int32_t atkmin = -1;
        int32_t atkmax = -1;
        int32_t defmin = -1;
        int32_t defmax = -1;
        int32_t lvmin = 0;
        int32_t lvmax = 0;
        int32_t scalemin = 0;
        int32_t scalemax = 0;
        uint32_t race = 0;
        uint32_t attribute = 0;
        std::vector<std::wstring> keywords;
        std::vector<std::wstring> tags;
    };

	struct CardData {
		uint32_t code;
        uint32_t alias;
		uint64_t setcode;
		uint32_t type;
        uint32_t star;
        uint32_t lscale;
        uint32_t rscale;
        uint32_t attribute;
        uint32_t race;
		int32_t attack;
		int32_t defence;
        uint32_t dbsrc;
        uint32_t pool;
		uint32_t category;
        std::wstring name;
		std::wstring texts;
        std::wstring desc[16];
        
        bool CheckCondition(const FilterCondition& fc);
        
        static bool card_sort(const CardData* c1, const CardData* c2);
        
	};
    
	class DataMgr : public Singleton<DataMgr> {
	public:
		int32_t LoadDatas(const std::string& file);
		CardData* operator [] (uint32_t code);
        std::vector<uint32_t> AllAliases(uint32_t code);
        std::vector<CardData*> FilterCard(const FilterCondition& fc);
        
        static std::wstring GetAttributeString(uint32_t attr);
        static std::wstring GetRaceString(uint32_t race);
        static std::wstring GetTypeString(uint32_t arctype);
        
        void RegisterSetCode(uint32_t code, const std::wstring& value) {
            _setcodes[code] = value;
            _setnames[value] = code;
        }
        
        uint32_t GetSetCode(const std::wstring& name) {
            auto iter = _setnames.find(name);
            if(iter == _setcodes.end())
                return 0;
            return iter->second;
        }
        
        const std::wstring& GetSetName(uint32_t code) {
            static const std::wstring empt = L"";
            auto iter = _setcodes.find(code);
            if(iter == _setcodes.end())
               return empt;
            return iter->second;
        }
        
        uint32_t RegistrDB(const std::wstring& db) {
            _dbsrc.push_back(db);
            return (uint32_t)_dbsrc.size() - 1;
        }
        
        const std::wstring& DBName(uint32_t index) {
            static const std::wstring empt = L"";
            if(index < _dbsrc.size())
                return _dbsrc[index];
            return empt;
        }
	private:
		std::unordered_map<uint32_t, CardData> _datas;
        std::unordered_map<uint32_t, std::wstring> _setcodes;
        std::unordered_map<std::wstring, uint32_t> _setnames;
        std::unordered_map<uint32_t, std::vector<uint32_t>> _aliases;
        std::vector<std::wstring> _dbsrc;
	};

}

#endif
