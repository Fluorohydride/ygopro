#ifndef DECK_CON_H
#define DECK_CON_H

#include "config.h"
#include <unordered_map>
#include <vector>
#include "client_card.h"

namespace ygo {

class DeckBuilder: public irr::IEventReceiver {
public:
	virtual bool OnEvent(const irr::SEvent& event);
	void FilterCards();
	
	long long filter_effect;
	int filter_type;
	int filter_type2;
	int filter_attrib;
	int filter_race;
	int filter_atktype;
	int filter_atk;
	int filter_deftype;
	int filter_def;
	int filter_lvtype;
	int filter_lv;
	int filter_lm;
	int hovered_code;
	int hovered_pos;
	int hovered_seq;
	int click_pos;
	bool is_draging;
	int dragx;
	int dragy;
	int pre_mainc;
	int pre_extrac;
	int pre_sidec;
	code_pointer draging_pointer;
	
	std::unordered_map<int, int>* filterList;
	std::vector<code_pointer> results;
	wchar_t result_string[8];
};

}

#endif //DECK_CON
