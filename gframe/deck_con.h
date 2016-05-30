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
	void ClearFilter();
	void ClearSearch();
	void SortList();

	long long filter_effect;
	unsigned int filter_type;
	unsigned int filter_type2;
	unsigned int filter_attrib;
	unsigned int filter_race;
	unsigned int filter_atktype;
	int filter_atk;
	unsigned int filter_deftype;
	int filter_def;
	unsigned int filter_lvtype;
	unsigned int filter_lv;
	unsigned int filter_scltype;
	unsigned int filter_scl;
	int filter_lm;
	int hovered_code;
	int hovered_pos;
	int hovered_seq;
	int click_pos;
	bool is_draging;
	int dragx;
	int dragy;
	size_t pre_mainc;
	size_t pre_extrac;
	size_t pre_sidec;
	code_pointer draging_pointer;
	
	std::unordered_map<int, int>* filterList;
	std::vector<code_pointer> results;
	wchar_t result_string[8];
};

}

#endif //DECK_CON
