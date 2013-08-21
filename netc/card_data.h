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
