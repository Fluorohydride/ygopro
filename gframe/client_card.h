#ifndef CLIENT_CARD_H
#define CLIENT_CARD_H

#include "config.h"
#include <vector>
#include <set>
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
	unsigned int level;
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
	wchar_t* name;
	wchar_t* text;
	wchar_t* desc[16];
};
typedef std::unordered_map<unsigned int, CardDataC>::const_iterator code_pointer;

class ClientCard {
public:
	irr::core::matrix4 mTransform;
	irr::core::vector3df curPos;
	irr::core::vector3df curRot;
	irr::core::vector3df dPos;
	irr::core::vector3df dRot;
	u32 curAlpha;
	u32 dAlpha;
	u32 aniFrame;
	bool is_moving;
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
	u8 sequence;
	u8 position;
	u8 is_disabled;
	u8 is_public;
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
	wchar_t atkstring[16];
	wchar_t defstring[16];
	wchar_t lvstring[16];
	wchar_t linkstring[16];
	wchar_t lscstring[16];
	wchar_t rscstring[16];

	ClientCard();
	void SetCode(int code);
	void UpdateInfo(char* buf);
	void ClearTarget();
	static bool client_card_sort(ClientCard* c1, ClientCard* c2);
	static bool deck_sort_lv(code_pointer l1, code_pointer l2);
	static bool deck_sort_atk(code_pointer l1, code_pointer l2);
	static bool deck_sort_def(code_pointer l1, code_pointer l2);
	static bool deck_sort_name(code_pointer l1, code_pointer l2);
};

}

#endif //CLIENT_CARD_H
