#ifndef DECK_CON_H
#define DECK_CON_H

#include <unordered_map>
#include <vector>
#include <random>
#include <irrlicht.h>
#include "data_manager.h"
#include "deck_manager.h"

namespace ygo {

struct DeckLayout {
	float mul{1.0f};
	float cw{44.0f}, ch{64.0f};
	// Main deck
	float dy{68.0f};
	float dx{0.0f};
	int lx{10};
	int rows{4};
	float left{0.0f}, top{0.0f};
	// Pack mode
	bool pack_scroll{false};
	int  pack_scroll_pos{0};
	// Extra deck
	float ex_dx{0.0f}, ex_left{0.0f}, ex_top{0.0f}, ex_bot{0.0f};
	int   ex_lx{0};
	// Side deck
	float sd_dx{0.0f}, sd_left{0.0f}, sd_top{0.0f}, sd_bot{0.0f};
	int   sd_lx{0};
	// Search panel
	float sr_row_h{66.0f};
	float sr_top_px{165.0f};
};

class DeckBuilder: public irr::IEventReceiver {
public:
	DeckBuilder();
	bool OnEvent(const irr::SEvent& event) override;
	void Initialize();
	void Terminate();
	void GetHoveredCard();
	void FilterCards();
	void StartFilter();
	void ClearFilter();
	void InstantSearch();
	void ClearSearch();
	void SortList();

	void RefreshDeckList();
	void RefreshReadonly(int catesel);
	void RefreshPackListScroll();
	void ChangeCategory(int catesel);
	void ShowDeckManage();
	void ShowBigCard(int code, float zoom);
	void ZoomBigCard(irr::s32 centerx = -1, irr::s32 centery = -1);
	void CloseBigCard();

	bool CardNameContains(const wchar_t *haystack, const wchar_t *needle);

	bool push_main(code_pointer pointer, int seq = -1);
	bool push_extra(code_pointer pointer, int seq = -1);
	bool push_side(code_pointer pointer, int seq = -1);
	void pop_main(int seq);
	void pop_extra(int seq);
	void pop_side(int seq);
	bool check_limit(code_pointer pointer);

	unsigned long long filter_effect{};
	unsigned int filter_type{};
	unsigned int filter_type2{};
	unsigned int filter_attrib{};
	unsigned int filter_race{};
	unsigned int filter_atktype{};
	int filter_atk{};
	unsigned int filter_deftype{};
	int filter_def{};
	unsigned int filter_lvtype{};
	unsigned int filter_lv{};
	unsigned int filter_scltype{};
	unsigned int filter_scl{};
	unsigned int filter_marks{};
	int filter_lm{};
	irr::core::vector2di mouse_pos;
	int hovered_code{};
	int hovered_pos{};
	int hovered_seq{ -1 };
	int is_lastcard{};
	int click_pos{};
	bool is_draging{};
	bool is_starting_dragging{};
	int dragx{};
	int dragy{};
	int bigcard_code{};
	float bigcard_zoom{};
	size_t pre_mainc{};
	size_t pre_extrac{};
	size_t pre_sidec{};
	code_pointer draging_pointer;
	int prev_category{};
	int prev_deck{};
	irr::s32 prev_operation{};
	int prev_sel{ -1 };
	bool is_modified{};
	bool readonly{};
	bool showing_pack{};
	DeckLayout layout{};
	std::mt19937 rnd;

	const LFList* filterList{};
	std::vector<code_pointer> results;
	wchar_t result_string[8]{};
	std::vector<std::wstring> expansionPacks;
};

}

#endif //DECK_CON
