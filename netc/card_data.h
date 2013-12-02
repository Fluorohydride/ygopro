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
        
        static wxString GetInfoString(unsigned int arctype, unsigned int attr, unsigned int race);
        static wxString GetAttributeString(unsigned int attr);
        static wxString GetRaceString(unsigned int race);
        static wxString GetTypeString(unsigned int arctype);
        
	private:
		std::unordered_map<unsigned int, CardData> _datas;
        
	};

	extern DataMgr dataMgr;

}

#endif
