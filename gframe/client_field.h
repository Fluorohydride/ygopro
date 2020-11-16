#ifndef CLIENT_FIELD_H
#define CLIENT_FIELD_H

#include <vector>
#include <set>
#include <map>
#include <IEventReceiver.h>
#include <vector3d.h>

namespace ygo {

class ClientCard;

struct ChainInfo {
	irr::core::vector3df chain_pos;
	ClientCard* chain_card;
	uint32_t code;
	uint64_t desc;
	uint8_t controler;
	uint8_t location;
	uint32_t sequence;
	bool solved;
	std::set<ClientCard*> target;
	void UpdateDrawCoordinates();
};

class ClientField: public irr::IEventReceiver {
public:
	std::vector<ClientCard*> deck[2];
	std::vector<ClientCard*> hand[2];
	std::vector<ClientCard*> mzone[2];
	std::vector<ClientCard*> szone[2];
	std::vector<ClientCard*> grave[2];
	std::vector<ClientCard*> remove[2];
	std::vector<ClientCard*> extra[2];
	std::vector<ClientCard*> limbo_temp;
	std::set<ClientCard*> overlay_cards;
	std::vector<ClientCard*> summonable_cards;
	std::vector<ClientCard*> spsummonable_cards;
	std::vector<ClientCard*> msetable_cards;
	std::vector<ClientCard*> ssetable_cards;
	std::vector<ClientCard*> reposable_cards;
	std::vector<ClientCard*> activatable_cards;
	std::vector<ClientCard*> attackable_cards;
	std::vector<ClientCard*> conti_cards;
	ClientCard* skills[2];
	std::vector<std::pair<uint64_t, uint8_t>> activatable_descs;
	std::vector<uint64_t> select_options;
	std::vector<ChainInfo> chains;
	int extra_p_count[2];

	size_t selected_option;
	ClientCard* attacker;
	ClientCard* attack_target;
	uint32_t disabled_field;
	uint32_t selectable_field;
	uint32_t selected_field;
	uint32_t select_min;
	uint32_t select_max;
	uint32_t must_select_count;
	uint32_t select_sumval;
	uint8_t select_mode;
	bool select_cancelable;
	bool select_ready;
	uint8_t announce_count;
	uint16_t select_counter_count;
	uint16_t select_counter_type;
	std::vector<ClientCard*> selectable_cards;
	std::vector<ClientCard*> selected_cards;
	std::vector<ClientCard*> must_select_cards;
	std::set<ClientCard*> selectsum_cards;
	std::vector<ClientCard*> selectsum_all;
	std::vector<uint64_t> declare_opcodes;
	std::vector<ClientCard*> display_cards;
	std::vector<int> sort_list;
	std::map<uint64_t, int> player_desc_hints[2];
	bool grave_act[2];
	bool remove_act[2];
	bool deck_act[2];
	bool extra_act[2];
	bool pzone_act[2];
	bool conti_act;
	bool chain_forced;
	ChainInfo current_chain;
	bool last_chain;
	bool deck_reversed;
	bool conti_selecting;

	ClientField();
	void Clear();
	void Initial(uint8_t player, uint32_t deckc, uint32_t extrac);
	std::vector<ClientCard*>* GetList(uint8_t location, uint8_t controler);
	ClientCard* GetCard(uint8_t controler, uint8_t location, uint32_t sequence, uint32_t sub_seq = 0);
	void AddCard(ClientCard* pcard, uint8_t controler, uint8_t location, uint32_t sequence);
	ClientCard* RemoveCard(uint8_t controler, uint8_t location, uint32_t sequence);
	void UpdateCard(uint8_t controler, uint8_t location, uint32_t sequence, char* data, uint32_t len = 0);
	void UpdateFieldCard(uint8_t controler, uint8_t location, char* data, uint32_t len = 0);
	void ClearCommandFlag();
	void ClearSelect();
	void ClearChainSelect();
	void ShowSelectCard(bool buttonok = false, bool chain = false);
	void ShowChainCard();
	void ShowLocationCard();
	void ShowSelectOption(uint64_t select_hint = 0, bool should_lock = true);
	void ReplaySwap();
	void RefreshAllCards();

	void GetChainDrawCoordinates(uint8_t controler, uint8_t location, uint32_t sequence, irr::core::vector3df* t);
	void GetCardDrawCoordinates(ClientCard* pcard, irr::core::vector3df* t, irr::core::vector3df* r, bool setTrans = false);
	void MoveCard(ClientCard* pcard, float frame);
	void FadeCard(ClientCard* pcard, float alpha, float frame);
	bool ShowSelectSum();
	bool CheckSelectSum();
	bool check_min(const std::set<ClientCard*>& left, std::set<ClientCard*>::const_iterator index, int min, int max);
	bool check_sel_sum_s(const std::set<ClientCard*>& left, int index, int acc);
	void check_sel_sum_t(const std::set<ClientCard*>& left, int acc);
	bool check_sum(std::set<ClientCard*>::const_iterator index, std::set<ClientCard*>::const_iterator end, int acc, uint32_t count);

	void UpdateDeclarableList(bool refresh = false);

	irr::gui::IGUIElement* panel;
	std::vector<uint32_t> ancard;
	uint8_t hovered_controler;
	uint16_t hovered_location;
	uint32_t hovered_sequence;
	uint8_t command_controler;
	uint16_t command_location;
	uint32_t command_sequence;
	ClientCard* hovered_card;
	uint8_t hovered_player;
	ClientCard* clicked_card;
	ClientCard* command_card;
	ClientCard* highlighting_card;
	uint16_t list_command;

	virtual bool OnEvent(const irr::SEvent& event);
	virtual bool OnCommonEvent(const irr::SEvent& event, bool& stopPropagation);
	void GetHoverField(int x, int y);
	void ShowMenu(int flag, int x, int y);
	void UpdateChainButtons(irr::gui::IGUIElement* caller = nullptr);
	void ShowCancelOrFinishButton(int buttonOp);
	void SetShowMark(ClientCard* pcard, bool enable);
	void ShowCardInfoInList(ClientCard* pcard, irr::gui::IGUIElement* element, irr::gui::IGUIElement* parent);
	void SetResponseSelectedCards() const;
	void SetResponseSelectedOption() const;
	void CancelOrFinish();
};

}

//special cards
#define CARD_MARINE_DOLPHIN	78734254
#define CARD_TWINKLE_MOSS	13857930

#define LOCATION_SKILL   0x400

#endif //CLIENT_FIELD_H
