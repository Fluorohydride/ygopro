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
        
        bool deck_sort(CardData* p1, CardData* p2) {
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
        
	};
    
	class DataMgr {
	public:
		int LoadDatas(const char* file);
		CardData* operator [] (unsigned int code);

	private:
		std::unordered_map<unsigned int, CardData> _datas;
	};

	extern DataMgr dataMgr;

}

#endif
