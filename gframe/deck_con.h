#ifndef DECK_CON_H
#define DECK_CON_H

#include "config.h"
#include <position2d.h>
#include <IEventReceiver.h>
#include <unordered_map>
#include <vector>
#include "deck.h"

namespace ygo {

struct LFList;
struct CardDataC;
class CardDataM;
struct CardString;

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
		LIMITATION_FILTER_PRERELEASE,
		LIMITATION_FILTER_SPEED,
		LIMITATION_FILTER_RUSH,
		LIMITATION_FILTER_ANIME,
		LIMITATION_FILTER_ILLEGAL,
		LIMITATION_FILTER_VIDEOGAME,
		LIMITATION_FILTER_CUSTOM,
		LIMITATION_FILTER_ALL
	};
	enum SEARCH_MODIFIER {
		SEARCH_MODIFIER_NAME_ONLY = 0x1,
		SEARCH_MODIFIER_ARCHETYPE_ONLY = 0x2,
		SEARCH_MODIFIER_NEGATIVE_LOOKUP = 0x4
	};
	virtual bool OnEvent(const irr::SEvent& event);
	void Initialize(bool refresh = true);
	void Terminate(bool showmenu = true);
	const Deck& GetCurrentDeck() const {
		return current_deck;
	}
	void SetCurrentDeck(Deck new_deck) {
		current_deck = std::move(new_deck);
		RefreshLimitationStatus();
	}
	void StartFilter(bool force_refresh = false);
	void RefreshCurrentDeck();
private:
	void GetHoveredCard();
	bool FiltersChanged();
	void FilterCards(bool force_refresh = false);
	bool CheckCard(CardDataM* data, SEARCH_MODIFIER modifier, const std::vector<std::wstring>& tokens, const std::vector<uint16_t>& setcode);
	void ClearFilter();
	void ClearSearch();
	void SortList();

	void RefreshLimitationStatus();
	enum DeckType {
		MAIN,
		EXTRA,
		SIDE
	};
	void RefreshLimitationStatusOnRemoved(const CardDataC* card, DeckType location);
	void RefreshLimitationStatusOnAdded(const CardDataC* card, DeckType location);

	void ImportDeck();
	void ExportDeckToClipboard(bool plain_text);

	bool push_main(const CardDataC* pointer, int seq = -1, bool forced = false);
	bool push_extra(const CardDataC* pointer, int seq = -1, bool forced = false);
	bool push_side(const CardDataC* pointer, int seq = -1, bool forced = false);
	void pop_main(int seq);
	void pop_extra(int seq);
	void pop_side(int seq);
	bool check_limit(const CardDataC* pointer);
#define DECLARE_WITH_CACHE(type, name) type name;\
										type prev_##name;
	DECLARE_WITH_CACHE(uint64_t, filter_effect)
	DECLARE_WITH_CACHE(uint32_t, filter_type)
	DECLARE_WITH_CACHE(uint32_t, filter_type2)
	DECLARE_WITH_CACHE(uint32_t, filter_attrib)
	DECLARE_WITH_CACHE(uint32_t, filter_race)
	DECLARE_WITH_CACHE(uint32_t, filter_atktype)
	DECLARE_WITH_CACHE(int32_t, filter_atk)
	DECLARE_WITH_CACHE(uint32_t, filter_deftype)
	DECLARE_WITH_CACHE(int32_t, filter_def)
	DECLARE_WITH_CACHE(uint32_t, filter_lvtype)
	DECLARE_WITH_CACHE(uint32_t, filter_lv)
	DECLARE_WITH_CACHE(uint32_t, filter_scltype)
	DECLARE_WITH_CACHE(uint32_t, filter_scl)
	DECLARE_WITH_CACHE(uint32_t, filter_marks)
	DECLARE_WITH_CACHE(limitation_search_filters, filter_lm)
#undef DECLARE_WITH_CACHE

	irr::core::position2di mouse_pos;

	uint16_t main_and_extra_legend_count;
	uint16_t main_skill_count;
	Deck current_deck;
public:
	uint32_t hovered_code;
	int hovered_pos;
	int hovered_seq;
	int is_lastcard;
	int click_pos;
	bool is_draging;
	int scroll_pos;
	int dragx;
	int dragy;
	const CardDataC* dragging_pointer;
	int prev_deck;
	int prev_operation;


	uint16_t main_monster_count;
	uint16_t main_spell_count;
	uint16_t main_trap_count;

	uint16_t extra_fusion_count;
	uint16_t extra_xyz_count;
	uint16_t extra_synchro_count;
	uint16_t extra_link_count;

	uint16_t side_monster_count;
	uint16_t side_spell_count;
	uint16_t side_trap_count;
	LFList* filterList;
	std::map<std::wstring, std::vector<const CardDataC*>> searched_terms;
	std::vector<const CardDataC*> results;
	std::wstring result_string;
};

}

#endif //DECK_CON
