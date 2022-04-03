#ifndef CLIENT_CARD_H
#define CLIENT_CARD_H

#include <matrix4.h>
#include <vector3d.h>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include "core_utils.h"

namespace ygo {

class ClientCard {
public:
	irr::core::matrix4 mTransform;
	irr::core::vector3df curPos;
	irr::core::vector3df curRot;
	irr::core::vector3df dPos;
	irr::core::vector3df dRot;
	irr::core::recti hand_collision;
	irr::f32 curAlpha = 255;
	irr::f32 dAlpha;
	int32_t aniFrame;
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
	bool is_public;
	uint32_t code;
	uint32_t chain_code;
	uint32_t alias;
	uint32_t type;
	uint32_t level;
	uint32_t rank;
	uint32_t link;
	uint32_t attribute;
	uint32_t race;
	int32_t attack;
	int32_t defense;
	int32_t base_attack;
	int32_t base_defense;
	uint32_t lscale;
	uint32_t rscale;
	uint32_t link_marker;
	uint32_t reason;
	uint32_t select_seq;
	uint8_t owner;
	uint8_t controler;
	uint32_t location;
	uint32_t sequence;
	uint8_t position;
	uint32_t status;
	uint32_t cover;
	uint8_t cHint;
	uint32_t chValue;
	uint32_t opParam;
	uint32_t symbol;
	uint32_t cmdFlag;
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

	void UpdateDrawCoordinates(bool setTrans = false);
	void SetCode(uint32_t new_code);
	void UpdateInfo(const CoreUtils::Query& query);
	void ClearTarget();
	static bool client_card_sort(ClientCard* c1, ClientCard* c2);
};

}

#endif //CLIENT_CARD_H
