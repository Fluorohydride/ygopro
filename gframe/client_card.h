#ifndef CLIENT_CARD_H
#define CLIENT_CARD_H

#include "config.h"
#include "core_utils.h"
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

namespace ygo {

struct CardData {
	unsigned int code;
	unsigned int alias;
	unsigned long long setcode;
	unsigned int type;
	unsigned int level;
	unsigned int attribute;
	unsigned int race;
	int attack;
	int defense;
	unsigned int lscale;
	unsigned int rscale;
	unsigned int link_marker;
};
struct CardDataC {
	unsigned int code;
	unsigned int alias;
	unsigned long long setcode;
	unsigned int type;
	int level;
	unsigned int attribute;
	unsigned int race;
	int attack;
	int defense;
	unsigned int lscale;
	unsigned int rscale;
	unsigned int link_marker;
	unsigned int ot;
	unsigned int category;
};
struct CardString {
	std::wstring name;
	std::wstring text;
	std::wstring desc[16];
};

class ClientCard {
public:
	irr::core::matrix4 mTransform;
	irr::core::vector3df curPos;
	irr::core::vector3df curRot;
	irr::core::vector3df dPos;
	irr::core::vector3df dRot;
	f32 curAlpha;
	f32 dAlpha;
	s32 aniFrame;
	bool is_moving;
	bool refresh_on_stop;
	bool is_fading;
	bool is_hovered;
	bool is_selectable;
	bool is_selected;
	bool is_showequip;
	bool is_showtarget;
	bool is_showchaintarget;
	bool is_highlighting;
	bool is_reversed;
	u32 code;
	u32 chain_code;
	u32 alias;
	u32 type;
	u32 level;
	u32 rank;
	u32 link;
	u32 attribute;
	u32 race;
	s32 attack;
	s32 defense;
	s32 base_attack;
	s32 base_defense;
	u32 lscale;
	u32 rscale;
	u32 link_marker;
	u32 reason;
	u32 select_seq;
	u8 owner;
	u8 controler;
	u8 location;
	u32 sequence;
	u8 position;
	u32 status;
	u32 cover;
	u8 cHint;
	u32 chValue;
	u32 opParam;
	u32 symbol;
	u32 cmdFlag;
	ClientCard* overlayTarget;
	std::vector<ClientCard*> overlayed;
	ClientCard* equipTarget;
	std::set<ClientCard*> equipped;
	std::set<ClientCard*> cardTarget;
	std::set<ClientCard*> ownerTarget;
	std::map<int, int> counters;
	std::map<int, int> desc_hints;
	std::wstring atkstring;
	std::wstring defstring;
	std::wstring lvstring;
	std::wstring rkstring;
	std::wstring linkstring;
	std::wstring lscstring;
	std::wstring rscstring;

	ClientCard();
	void SetCode(int code);
	void UpdateInfo(const CoreUtils::Query& query);
	void ClearTarget();
	static bool client_card_sort(ClientCard* c1, ClientCard* c2);
	static bool deck_sort_lv(CardDataC* l1, CardDataC* l2);
	static bool deck_sort_atk(CardDataC* l1, CardDataC* l2);
	static bool deck_sort_def(CardDataC* l1, CardDataC* l2);
	static bool deck_sort_name(CardDataC* l1, CardDataC* l2);
};

}

#endif //CLIENT_CARD_H
