#ifndef _CARD_DATA_H_
#define _CARD_DATA_H_

#include "wx/string.h"
#include <string>
#include <unordered_map>

namespace ygopro
{

	struct CardData {
		unsigned int code;
		unsigned int ot;
		unsigned int alias;
		unsigned int setcode;
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
        
	};
    
	class DataMgr {
	public:
		int LoadDatas(const wxString& file);
		CardData* operator [] (unsigned int code);
        
        static wxString GetAttributeString(unsigned int attr);
        static wxString GetRaceString(unsigned int race);
        static wxString GetTypeString(unsigned int arctype);
        
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
        
	private:
		std::unordered_map<unsigned int, CardData> _datas;
        std::unordered_map<unsigned int, wxString> _setcodes;
	};

	extern DataMgr dataMgr;

}

#endif
