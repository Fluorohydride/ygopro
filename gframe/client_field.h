#ifndef CLIENT_FIELD_H
#define CLIENT_FIELD_H

#include "config.h"
#include <random>
#include <vector>
#include <set>
#include <map>

namespace ygo {

class ClientCard;

struct ChainInfo {
	irr::core::vector3df chain_pos;
	ClientCard* chain_card{ nullptr };
	int code{ 0 };
	int desc{ 0 };
	int controler{ 0 };
	int location{ 0 };
	int sequence{ 0 };
	bool solved{ false };
	std::set<ClientCard*> target;
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
	std::set<ClientCard*> overlay_cards;

	std::vector<ClientCard*> summonable_cards;
	std::vector<ClientCard*> spsummonable_cards;
	std::vector<ClientCard*> msetable_cards;
	std::vector<ClientCard*> ssetable_cards;
	std::vector<ClientCard*> reposable_cards;
	std::vector<ClientCard*> activatable_cards;
	std::vector<ClientCard*> attackable_cards;
	std::vector<ClientCard*> conti_cards;
	std::vector<std::pair<int,int>> activatable_descs;
	std::vector<int> select_options;
	std::vector<int> select_options_index;
	std::vector<ChainInfo> chains;
	int extra_p_count[2]{};

	size_t selected_option{ 0 };
	ClientCard* attacker{ nullptr };
	ClientCard* attack_target{ nullptr };
	unsigned int disabled_field{ 0 };
	unsigned int selectable_field{ 0 };
	unsigned int selected_field{ 0 };
	int select_min{ 0 };
	int select_max{ 0 };
	int must_select_count{ 0 };
	int select_curval_l{ 0 };
	int select_curval_h{ 0 };
	int select_sumval{ 0 };
	int select_mode{ 0 };
	int select_hint{0};
	bool select_cancelable{false};
	bool select_panalmode{ false };
	bool select_ready{ false };
	int announce_count{ 0 };
	int select_counter_count{ 0 };
	int select_counter_type{ 0 };
	std::vector<ClientCard*> selectable_cards;
	std::vector<ClientCard*> selected_cards;
	std::set<ClientCard*> selectsum_cards;
	std::vector<ClientCard*> selectsum_all;
	std::vector<unsigned int> declare_opcodes;
	std::vector<ClientCard*> display_cards;
	std::vector<int> sort_list;
	std::map<int, int> player_desc_hints[2];
	bool grave_act[2]{ false };
	bool remove_act[2]{ false };
	bool deck_act[2]{ false };
	bool extra_act[2]{ false };
	bool pzone_act[2]{ false };
	bool conti_act{ false };
	bool chain_forced{ false };
	ChainInfo current_chain;
	bool last_chain{ false };
	bool deck_reversed{ false };
	bool select_continuous{ false };
	bool cant_check_grave{ false };
	bool tag_surrender{ false };
	bool tag_teammate_surrender{ false };
	std::mt19937 rnd;

	ClientField();
	~ClientField() override;
	void Clear();
	void Initial(int player, int deckc, int extrac, int sidec = 0);
	void ResetSequence(std::vector<ClientCard*>& list, bool reset_height);
	ClientCard* GetCard(int controler, int location, int sequence, int sub_seq = 0);
	void AddCard(ClientCard* pcard, int controler, int location, int sequence);
	ClientCard* RemoveCard(int controler, int location, int sequence);
	void UpdateCard(int controler, int location, int sequence, unsigned char* data);
	void UpdateFieldCard(int controler, int location, unsigned char* data);
	void ClearCommandFlag();
	void ClearSelect();
	void ClearChainSelect();
	void ShowSelectCard(bool buttonok = false, bool is_continuous = false);
	void ShowChainCard();
	void ShowLocationCard();
	void ShowSelectOption(int select_hint = 0);
	void ReplaySwap();
	void RefreshAllCards();

	void GetChainLocation(int controler, int location, int sequence, irr::core::vector3df* t);
	void GetCardLocation(ClientCard* pcard, irr::core::vector3df* t, irr::core::vector3df* r, bool setTrans = false);
	void MoveCard(ClientCard* pcard, int frame);
	void FadeCard(ClientCard* pcard, int alpha, int frame);
	bool ShowSelectSum(bool panelmode);
	bool CheckSelectSum();
	bool CheckSelectTribute();
	void get_sum_params(unsigned int opParam, int& op1, int& op2);
	bool check_min(const std::set<ClientCard*>& left, std::set<ClientCard*>::const_iterator index, int min, int max);
	bool check_sel_sum_s(const std::set<ClientCard*>& left, int index, int acc);
	void check_sel_sum_t(const std::set<ClientCard*>& left, int acc);
	bool check_sum(std::set<ClientCard*>::const_iterator index, std::set<ClientCard*>::const_iterator end, int acc, int count);
	bool check_sel_sum_trib_s(const std::set<ClientCard*>& left, int index, int acc);
	void check_sel_sum_trib_t(const std::set<ClientCard*>& left, int acc);
	bool check_sum_trib(std::set<ClientCard*>::const_iterator index, std::set<ClientCard*>::const_iterator end, int acc);

	void UpdateDeclarableList();

	irr::gui::IGUIElement* panel{ nullptr };
	std::vector<int> ancard;
	int hovered_controler{ 0 };
	int hovered_location{ 0 };
	size_t hovered_sequence{ 0 };
	int command_controler{ 0 };
	int command_location{ 0 };
	size_t command_sequence{ 0 };
	ClientCard* hovered_card{ nullptr };
	int hovered_player{ 0 };
	ClientCard* clicked_card{ nullptr };
	ClientCard* command_card{ nullptr };
	ClientCard* highlighting_card{ nullptr };
	ClientCard* menu_card{ nullptr };
	int list_command{ 0 };

	bool OnEvent(const irr::SEvent& event) override;
	bool OnCommonEvent(const irr::SEvent& event);
	void GetHoverField(int x, int y);
	void ShowMenu(int flag, int x, int y);
	void HideMenu();
	void UpdateChainButtons();
	void ShowCancelOrFinishButton(int buttonOp);
	void SetShowMark(ClientCard* pcard, bool enable);
	void ShowCardInfoInList(ClientCard* pcard, irr::gui::IGUIElement* element, irr::gui::IGUIElement* parent);
	void SetResponseSelectedCards() const;
	void SetResponseSelectedOption() const;
	void CancelOrFinish();
};

}

//special cards
#define CARD_QUESTION		38723936

#endif //CLIENT_FIELD_H
