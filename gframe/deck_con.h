#ifndef DECK_CON_H
#define DECK_CON_H

#include "config.h"
#include "deck_manager.h"
#include <unordered_map>
#include <vector>
#include "client_card.h"

namespace ygo {

class DeckBuilder: public irr::IEventReceiver {
public:
	enum search_filters {
		FILTER_NONE,
		FILTER_BANNED,
		FILTER_LIMITED,
		FILTER_SEMI_LIMITED,
		FILTER_UNLIMITED,
		FILTER_OCG,
		FILTER_TCG,
		FILTER_TCG_OCG,
		FILTER_ANIME,
		FILTER_ILLEGAL,
		FILTER_VIDEOGAME,
		FILTER_CUSTOM
	};
	virtual bool OnEvent(const irr::SEvent& event);
	void Initialize();
	void Terminate();
	void GetHoveredCard();
	void FilterCards();
	bool CheckCard(const CardDataC& data, const CardString& text, const wchar_t* pstr, std::vector<unsigned int>& setcode);
	void StartFilter();
	void ClearFilter();
	void ClearSearch();
	void SortList();

	bool push_main(code_pointer pointer, int seq = -1);
	bool push_extra(code_pointer pointer, int seq = -1);
	bool push_side(code_pointer pointer, int seq = -1);
	void pop_main(int seq);
	void pop_extra(int seq);
	void pop_side(int seq);
	bool check_limit(code_pointer pointer);

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
	unsigned int filter_marks;
	search_filters filter_lm;
	position2di mouse_pos;
	int hovered_code;
	int hovered_pos;
	int hovered_seq;
	int is_lastcard;
	int click_pos;
	bool is_draging;
	int dragx;
	int dragy;
	code_pointer draging_pointer;
	int prev_deck;
	s32 prev_operation;

	LFList* filterList;
	std::map<std::wstring, std::vector<code_pointer>> searched_terms;
	std::vector<code_pointer> results;
	wchar_t result_string[8];
};

}

#endif //DECK_CON
