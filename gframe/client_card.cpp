#include "client_card.h"
#include "game.h"
#include "data_manager.h"
#include "common.h"

namespace ygo {

ClientCard::ClientCard() {
	curAlpha = 255;
	dAlpha = 0;
	aniFrame = 0;
	is_moving = false;
	refresh_on_stop = false;
	is_fading = false;
	is_hovered = false;
	is_selectable = false;
	is_selected = false;
	is_showequip = false;
	is_showtarget = false;
	is_showchaintarget = false;
	is_highlighting = false;
	status = 0;
	is_reversed = false;
	is_public = false;
	cmdFlag = 0;
	code = 0;
	cover = 0;
	chain_code = 0;
	location = 0;
	type = 0;
	alias = 0;
	level = 0;
	rank = 0;
	link = 0;
	race = 0;
	attribute = 0;
	attack = 0;
	defense = 0;
	base_attack = 0;
	base_defense = 0;
	lscale = 0;
	rscale = 0;
	link_marker = 0;
	position = 0;
	cHint = 0;
	chValue = 0;
	atkstring = L"";
	defstring = L"";
	lvstring = L"";
	linkstring = L"";
	rkstring = L"";
	rscstring = L"";
	lscstring = L"";
	overlayTarget = 0;
	equipTarget = 0;
}
void ClientCard::UpdateDrawCoordinates(bool setTrans) {
	mainGame->dField.GetCardDrawCoordinates(this, &curPos, &curRot, setTrans);
}
void ClientCard::SetCode(uint32_t code) {
	if((location == LOCATION_HAND) && (this->code != code)) {
		this->code = code;
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->dField.MoveCard(this, 5);
	} else
		this->code = code;
}
#define CHECK_AND_SET(_query, value)if(query.flag & _query) value = query.value;
void ClientCard::UpdateInfo(const CoreUtils::Query& query) {
	CHECK_AND_SET(QUERY_ALIAS, alias)
	CHECK_AND_SET(QUERY_TYPE, type)
	CHECK_AND_SET(QUERY_ATTRIBUTE, attribute)
	CHECK_AND_SET(QUERY_RACE, race)
	CHECK_AND_SET(QUERY_BASE_ATTACK, base_attack)
	CHECK_AND_SET(QUERY_BASE_DEFENSE, base_defense)
	CHECK_AND_SET(QUERY_REASON, reason)
	CHECK_AND_SET(QUERY_OWNER, owner)
	CHECK_AND_SET(QUERY_STATUS, status)
	CHECK_AND_SET(QUERY_COVER, cover)
	if(query.flag & QUERY_CODE) {
		if((location == LOCATION_HAND) && (query.code != code)) {
			code = query.code;
			if(!mainGame->dInfo.isCatchingUp)
				mainGame->dField.MoveCard(this, 5);
		} else
			code = query.code;
	}
	if(query.flag & QUERY_POSITION) {
		if((location & (LOCATION_EXTRA | LOCATION_REMOVED)) && query.position != position) {
			position = query.position;
			mainGame->dField.MoveCard(this, 1);
		} else
			position = query.position;
	}
	if(query.flag & QUERY_LEVEL) {
		level = query.level;
		lvstring = fmt::format(L"L{}",level);
	}
	if(query.flag & QUERY_RANK) {
		rank = query.rank;
		rkstring = fmt::format(L"R{}", rank);
	}
	if(query.flag & QUERY_ATTACK) {
		attack = query.attack;
		if(attack < 0) {
			atkstring = L"?";
		} else
			atkstring = fmt::to_wstring(attack);
	}
	if(query.flag & QUERY_DEFENSE) {
		defense = query.defense;
		if(type & TYPE_LINK) {
			defstring = L"-";
		} else if(defense < 0) {
			defstring = L"?";
		} else
			defstring = fmt::to_wstring(defense);
	}
	/*if(query.flag & QUERY_REASON_CARD) {

	}*/
	if(query.flag & QUERY_EQUIP_CARD) {
		ClientCard* ecard = mainGame->dField.GetCard(mainGame->LocalPlayer(query.equip_card.controler), query.equip_card.location, query.equip_card.sequence);
		if(ecard) {
			equipTarget = ecard;
			ecard->equipped.insert(this);
		}
	}
	if(query.flag & QUERY_TARGET_CARD) {
		for(auto& card : query.target_cards) {
			ClientCard* tcard = mainGame->dField.GetCard(mainGame->LocalPlayer(card.controler), card.location, card.sequence);
			cardTarget.insert(tcard);
			tcard->ownerTarget.insert(this);
		}
	}
	if(query.flag & QUERY_OVERLAY_CARD) {
		int i = 0;
		for(auto& code : query.overlay_cards) {
			overlayed[i++]->SetCode(code);
		}
	}
	if(query.flag & QUERY_COUNTERS) {
		for(auto& counter : query.counters) {
			int ctype = counter & 0xffff;
			int ccount = counter >> 16;
			counters[ctype] = ccount;
		}
	}
	if(query.flag & QUERY_IS_PUBLIC) {
		if(is_public != query.is_public && !mainGame->dInfo.isCatchingUp) {
			is_public = query.is_public;
			mainGame->dField.MoveCard(this, 5);
		} else
			is_public = query.is_public;
	}
	if(query.flag & QUERY_LSCALE) {
		lscale = query.lscale;
		lscstring = fmt::to_wstring(lscale);
	}
	if(query.flag & QUERY_RSCALE) {
		rscale = query.rscale;
		rscstring = fmt::to_wstring(rscale);
	}
	if(query.flag & QUERY_LINK) {
		if (link != query.link) {
			link = query.link;
			linkstring = fmt::format(L"L{}", link);
		}
		if (link_marker != query.link_marker) {
			link_marker = query.link_marker;
		}
	}
}
void ClientCard::ClearTarget() {
	for(auto cit = cardTarget.begin(); cit != cardTarget.end(); ++cit) {
		(*cit)->is_showtarget = false;
		(*cit)->ownerTarget.erase(this);
	}
	for(auto cit = ownerTarget.begin(); cit != ownerTarget.end(); ++cit) {
		(*cit)->is_showtarget = false;
		(*cit)->cardTarget.erase(this);
	}
	cardTarget.clear();
	ownerTarget.clear();
}
bool ClientCard::client_card_sort(ClientCard* c1, ClientCard* c2) {
	int32 cp1 = c1->overlayTarget ? c1->overlayTarget->controler : c1->controler;
	int32 cp2 = c2->overlayTarget ? c2->overlayTarget->controler : c2->controler;
	if(cp1 != cp2)
		return cp1 < cp2;
	if(c1->location != c2->location)
		return c1->location < c2->location;
	if(c1->location & LOCATION_OVERLAY)
		if(c1->overlayTarget != c2->overlayTarget)
			return c1->overlayTarget->sequence < c2->overlayTarget->sequence;
		else return c1->sequence < c2->sequence;
	else {
		if(c1->location & (LOCATION_DECK | LOCATION_GRAVE | LOCATION_REMOVED | LOCATION_EXTRA)) {
			for(size_t i = 0; i < mainGame->dField.chains.size(); ++i) {
				auto chit = mainGame->dField.chains[i];
				if(c1 == chit.chain_card || chit.target.find(c1) != chit.target.end())
					return true;
			}
			return c1->sequence > c2->sequence;
		}
		else
			return c1->sequence < c2->sequence;
	}
}
bool is_skill(uint32_t type) {
	return (type & (TYPE_SKILL | TYPE_ACTION));
}
bool check_both_skills(uint32_t type1, uint32_t type2) {
	return is_skill(type1) && is_skill(type2);
}
bool check_either_skills(uint32_t type1, uint32_t type2) {
	return is_skill(type1) || is_skill(type2);
}
bool check_skills(CardDataC* p1, CardDataC* p2) {
	if(check_both_skills(p1->type, p2->type)) {
		if((p1->type & 0xfffffff8) == (p2->type & 0xfffffff8)) {
			return p1->code < p2->code;
		} else {
			return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
		}
	}
	return is_skill(p2->type);
}
bool card_sorter(CardDataC* p1, CardDataC* p2, std::function<bool(CardDataC* p1, CardDataC* p2)> sortoop) {
	if(check_either_skills(p1->type, p2->type))
		return check_skills(p1, p2);
	if((p1->type & 0x7) != (p2->type & 0x7))
		return (p1->type & 0x7) < (p2->type & 0x7);
	if((p1->type & 0x7) == 1) {
		return sortoop(p1, p2);
	}
	if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
		return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
	return p1->code < p2->code;
}
bool ClientCard::deck_sort_lv(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2) {
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		return p1->code < p2->code;
	});
}
bool ClientCard::deck_sort_atk(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2) {
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->code < p2->code;
	});
}
bool ClientCard::deck_sort_def(CardDataC* p1, CardDataC* p2) {
	return card_sorter(p1, p2, [](CardDataC* p1, CardDataC* p2) {
		if(p1->defense != p2->defense)
			return p1->defense > p2->defense;
		if(p1->attack != p2->attack)
			return p1->attack > p2->attack;
		if(p1->level != p2->level)
			return p1->level > p2->level;
		int type1 = (p1->type & 0x48020c0) ? (p1->type & 0x48020c1) : (p1->type & 0x31);
		int type2 = (p2->type & 0x48020c0) ? (p2->type & 0x48020c1) : (p2->type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->code < p2->code;
	});
}
bool ClientCard::deck_sort_name(CardDataC* p1, CardDataC* p2) {
	int res = gDataManager->GetName(p1->code).compare(gDataManager->GetName(p2->code));
	if(res != 0)
		return res < 0;
	return p1->code < p2->code;
}
}
