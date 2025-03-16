#ifndef CLIENT_CARD_H
#define CLIENT_CARD_H

#include "config.h"
#include <vector>
#include <set>
#include <map>

namespace ygo {

class ClientCard {
public:
	irr::core::matrix4 mTransform;
	irr::core::vector3df curPos;
	irr::core::vector3df curRot;
	irr::core::vector3df dPos;
	irr::core::vector3df dRot;
	irr::u32 curAlpha{ 255 };
	irr::u32 dAlpha{ 0 };
	irr::u32 aniFrame{ 0 };
	bool is_moving{ false };
	bool is_fading{ false };
	bool is_hovered{ false };
	bool is_selectable{ false };
	bool is_selected{ false };
	bool is_showequip{ false };
	bool is_showtarget{ false };
	bool is_showchaintarget{ false };
	bool is_highlighting{ false };
	bool is_reversed{ false };

	unsigned int code{ 0 };
	unsigned int chain_code{ 0 };
	unsigned int alias{ 0 };
	unsigned int type{ 0 };
	unsigned int level{ 0 };
	unsigned int rank{ 0 };
	unsigned int link{ 0 };
	unsigned int attribute{ 0 };
	unsigned int race{ 0 };
	int attack{ 0 };
	int defense{ 0 };
	int base_attack{ 0 };
	int base_defense{ 0 };
	unsigned int lscale{ 0 };
	unsigned int rscale{ 0 };
	unsigned int link_marker{ 0 };
	unsigned int reason{ 0 };
	unsigned int select_seq{ 0 };
	unsigned char owner{ PLAYER_NONE };
	unsigned char controler{ PLAYER_NONE };
	unsigned char location{ 0 };
	unsigned char sequence{ 0 };
	unsigned char position{ 0 };
	unsigned int status{ 0 };
	unsigned char cHint{ 0 };
	unsigned int chValue{ 0 };
	unsigned int opParam{ 0 };
	unsigned int symbol{ 0 };
	unsigned int cmdFlag{ 0 };
	ClientCard* overlayTarget{ nullptr };
	std::vector<ClientCard*> overlayed;
	ClientCard* equipTarget{ nullptr };
	std::set<ClientCard*> equipped;
	std::set<ClientCard*> cardTarget;
	std::set<ClientCard*> ownerTarget;
	std::map<int, int> counters;
	std::map<int, int> desc_hints;
	wchar_t atkstring[16]{};
	wchar_t defstring[16]{};
	wchar_t lvstring[16]{};
	wchar_t linkstring[16]{};
	wchar_t lscstring[16]{};
	wchar_t rscstring[16]{};

	ClientCard() = default;
	~ClientCard();
	void SetCode(unsigned int x);
	void UpdateInfo(unsigned char* buf);
	void ClearTarget();
	void ClearData();
	static bool client_card_sort(ClientCard* c1, ClientCard* c2);
};

}

#endif //CLIENT_CARD_H
