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
	enum limitation_search_filters {
		LIMITATION_FILTER_NONE,
		LIMITATION_FILTER_BANNED,
		LIMITATION_FILTER_LIMITED,
		LIMITATION_FILTER_SEMI_LIMITED,
		LIMITATION_FILTER_UNLIMITED,
		LIMITATION_FILTER_OCG,
		LIMITATION_FILTER_TCG,
		LIMITATION_FILTER_TCG_OCG,
		LIMITATION_FILTER_ANIME,
		LIMITATION_FILTER_ILLEGAL,
		LIMITATION_FILTER_VIDEOGAME,
		LIMITATION_FILTER_CUSTOM
	};
	virtual bool OnEvent(const irr::SEvent& event);
	void Initialize();
	void Terminate();
	void GetHoveredCard();
	bool FiltersChanged();
	void FilterCards(bool force_refresh = false);
	bool CheckCard(const CardDataC& data, const CardString& text, const wchar_t& checkchar, std::vector<std::wstring>& tokens, std::vector<unsigned int>& setcode);
	void StartFilter(bool force_refresh = false);
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
	limitation_search_filters filter_lm;
	long long prev_filter_effect;
	unsigned int prev_filter_type;
	unsigned int prev_filter_type2;
	unsigned int prev_filter_attrib;
	unsigned int prev_filter_race;
	unsigned int prev_filter_atktype;
	int prev_filter_atk;
	unsigned int prev_filter_deftype;
	int prev_filter_def;
	unsigned int prev_filter_lvtype;
	unsigned int prev_filter_lv;
	unsigned int prev_filter_scltype;
	unsigned int prev_filter_scl;
	unsigned int prev_filter_marks;
	limitation_search_filters prev_filter_lm;
	position2di mouse_pos;
	int hovered_code;
	int hovered_pos;
	int hovered_seq;
	int is_lastcard;
	int click_pos;
	bool is_draging;
	int scroll_pos;
	int dragx;
	int dragy;
	code_pointer draging_pointer;
	int prev_deck;
	s32 prev_operation;

	LFList* filterList;
	std::map<std::wstring, std::vector<code_pointer>> searched_terms;
	std::vector<code_pointer> results;
	std::wstring result_string;
};

}

#endif //DECK_CON
