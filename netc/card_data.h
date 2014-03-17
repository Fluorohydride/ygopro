#ifndef _CARD_DATA_H_
#define _CARD_DATA_H_

#include "wx/string.h"
#include <string>
#include <vector>
#include <unordered_map>

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
        unsigned int category = 0;
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
		wxString name;
		wxString texts;
        wxString desc[16];
        
        bool CheckCondition(const FilterCondition& fc, const wxString& keyword, bool check_desc);
        
        static bool card_sort(const CardData* c1, const CardData* c2);
        
	};
    
	class DataMgr {
	public:
		int LoadDatas(const wxString& file);
		CardData* operator [] (unsigned int code);
        std::vector<unsigned int> AllAliases(unsigned int code);
        std::vector<CardData*> FilterCard(const FilterCondition& fc, const wxString& fs, bool check_desc);
        
        static wxString GetAttributeString(unsigned int attr);
        static wxString GetRaceString(unsigned int race);
        static wxString GetTypeString(unsigned int arctype);
        static wxString GetTypeString2(unsigned int arctype);
        
        void RegisterSetCode(unsigned int code, const wxString& value) {
            _setcodes[code] = value;
        }
        
        const wxString& GetSetCode(unsigned int code) {
            static const wxString empt = wxT("");
            auto iter = _setcodes.find(code);
            if(iter == _setcodes.end())
               return empt;
            return iter->second;
        }
        
        unsigned int RegistrDB(const wxString& db) {
            _dbsrc.push_back(db);
            return (unsigned int)_dbsrc.size() - 1;
        }
        
        const wxString& DBName(unsigned int index) {
            static const wxString empt = wxT("");
            if(index < _dbsrc.size())
                return _dbsrc[index];
            return empt;
        }
	private:
		std::unordered_map<unsigned int, CardData> _datas;
        std::unordered_map<unsigned int, wxString> _setcodes;
        std::unordered_map<unsigned int, std::vector<unsigned int>> _aliases;
        std::vector<wxString> _dbsrc;
	};

	extern DataMgr dataMgr;

}

#endif
