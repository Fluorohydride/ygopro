#ifndef _CARD_DATA_H_
#define _CARD_DATA_H_

namespace ygopro
{

    struct FilterCondition {
        unsigned int code = 0;
        unsigned int setcode = 0;
        unsigned int pool = 0;
        unsigned int type = 0;
        unsigned int subtype = 0;
        int atkmin = -1;
        int atkmax = -1;
        int defmin = -1;
        int defmax = -1;
        int lvmin = 0;
        int lvmax = 0;
        unsigned int race = 0;
        unsigned int attribute = 0;
        std::wstring keyword;
    };
    
	struct CardData {
        unsigned int dbsrc;
		unsigned int code;
		unsigned int pool;
		unsigned int alias;
		unsigned long long setcode;
		unsigned int type;
		int attack;
		int defence;
		unsigned int level;
		unsigned int race;
		unsigned int attribute;
		unsigned int category;
        std::wstring name;
		std::wstring texts;
        std::wstring desc[16];
        
        bool CheckCondition(const FilterCondition& fc);
        
        static bool card_sort(const CardData* c1, const CardData* c2);
        
	};
    
	class DataMgr : public Singleton<DataMgr> {
	public:
		int LoadDatas(const std::wstring& file);
		CardData* operator [] (unsigned int code);
        std::vector<unsigned int> AllAliases(unsigned int code);
        std::vector<CardData*> FilterCard(const FilterCondition& fc);
        
        static std::wstring GetAttributeString(unsigned int attr);
        static std::wstring GetRaceString(unsigned int race);
        static std::wstring GetTypeString(unsigned int arctype);
        static std::wstring GetTypeString2(unsigned int arctype);
        
        void RegisterSetCode(unsigned int code, const std::wstring& value) {
            _setcodes[code] = value;
        }
        
        const std::wstring& GetSetCode(unsigned int code) {
            static const std::wstring empt = L"";
            auto iter = _setcodes.find(code);
            if(iter == _setcodes.end())
               return empt;
            return iter->second;
        }
        
        unsigned int RegistrDB(const std::wstring& db) {
            _dbsrc.push_back(db);
            return (unsigned int)_dbsrc.size() - 1;
        }
        
        const std::wstring& DBName(unsigned int index) {
            static const std::wstring empt = L"";
            if(index < _dbsrc.size())
                return _dbsrc[index];
            return empt;
        }
	private:
		std::unordered_map<unsigned int, CardData> _datas;
        std::unordered_map<unsigned int, std::wstring> _setcodes;
        std::unordered_map<unsigned int, std::vector<unsigned int>> _aliases;
        std::vector<std::wstring> _dbsrc;
	};

}

#endif
