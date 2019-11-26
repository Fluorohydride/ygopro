#ifndef CLIENT_CARD_H
#define CLIENT_CARD_H

#include <irrlicht.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include "core_utils.h"

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
	irr::f32 curAlpha;
	irr::f32 dAlpha;
	irr::s32 aniFrame;
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
	irr::u32 code;
	irr::u32 chain_code;
	irr::u32 alias;
	irr::u32 type;
	irr::u32 level;
	irr::u32 rank;
	irr::u32 link;
	irr::u32 attribute;
	irr::u32 race;
	irr::s32 attack;
	irr::s32 defense;
	irr::s32 base_attack;
	irr::s32 base_defense;
	irr::u32 lscale;
	irr::u32 rscale;
	irr::u32 link_marker;
	irr::u32 reason;
	irr::u32 select_seq;
	irr::u8 owner;
	irr::u8 controler;
	irr::u8 location;
	irr::u32 sequence;
	irr::u8 position;
	irr::u32 status;
	irr::u32 cover;
	irr::u8 cHint;
	irr::u32 chValue;
	irr::u32 opParam;
	irr::u32 symbol;
	irr::u32 cmdFlag;
	ClientCard* overlayTarget;
	std::vector<ClientCard*> overlayed;
	ClientCard* equipTarget;
	std::set<ClientCard*> equipped;
	std::set<ClientCard*> cardTarget;
	std::set<ClientCard*> ownerTarget;
	std::map<int, int> counters;
	std::map<irr::u64, int> desc_hints;
	std::wstring atkstring;
	std::wstring defstring;
	std::wstring lvstring;
	std::wstring rkstring;
	std::wstring linkstring;
	std::wstring lscstring;
	std::wstring rscstring;

	ClientCard();
	void SetCode(irr::u32 code);
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
