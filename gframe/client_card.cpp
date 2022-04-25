#include <fmt/format.h>
#include "game.h"
#include "data_manager.h"
#include "common.h"
#include "client_card.h"

namespace ygo {

void ClientCard::UpdateDrawCoordinates(bool setTrans) {
	mainGame->dField.GetCardDrawCoordinates(this, &curPos, &curRot, setTrans);
}
template <typename T>
static inline bool IsDifferent(T& what, T _new) {
	return std::exchange(what, _new) != what;
}
void ClientCard::SetCode(uint32_t new_code) {
	if(!IsDifferent(code, new_code))
		return;
	if(location != LOCATION_HAND)
		return;
	if(mainGame->dInfo.isCatchingUp)
		return;
	mainGame->dField.MoveCard(this, 5);
}
#define CHECK_AND_SET(_query, value) do { if(query.flag & _query) value = query.value; } while(0)
void ClientCard::UpdateInfo(const CoreUtils::Query& query) {
	CHECK_AND_SET(QUERY_ALIAS, alias);
	CHECK_AND_SET(QUERY_TYPE, type);
	CHECK_AND_SET(QUERY_ATTRIBUTE, attribute);
	CHECK_AND_SET(QUERY_RACE, race);
	CHECK_AND_SET(QUERY_BASE_ATTACK, base_attack);
	CHECK_AND_SET(QUERY_BASE_DEFENSE, base_defense);
	CHECK_AND_SET(QUERY_REASON, reason);
	CHECK_AND_SET(QUERY_OWNER, owner);
	CHECK_AND_SET(QUERY_STATUS, status);
	CHECK_AND_SET(QUERY_COVER, cover);
	if(query.flag & QUERY_CODE)
		SetCode(query.code);
	if(query.flag & QUERY_POSITION) {
		if(IsDifferent(position, static_cast<uint8_t>(query.position)) && (location & (LOCATION_EXTRA | LOCATION_REMOVED)))
			mainGame->dField.MoveCard(this, 1);
	}
	if(query.flag & QUERY_LEVEL) {
		if(IsDifferent(level, query.level) || lvstring.empty())
			lvstring = fmt::format(L"L{}",level);
	}
	if(query.flag & QUERY_RANK) {
		if(IsDifferent(rank, query.rank) || rkstring.empty())
			rkstring = fmt::format(L"R{}", rank);
	}
	if(query.flag & QUERY_ATTACK) {
		if(IsDifferent(attack, query.attack) || atkstring.empty()) {
			if(attack < 0) {
				atkstring = L"?";
			} else
				atkstring = fmt::to_wstring(attack);
		}
	}
	if(query.flag & QUERY_DEFENSE) {
		if(IsDifferent(defense, query.defense) || defstring.empty()) {
			if(type & TYPE_LINK) {
				defstring = L"-";
			} else if(defense < 0) {
				defstring = L"?";
			} else
				defstring = fmt::to_wstring(defense);
		}
	}
	/*if(query.flag & QUERY_REASON_CARD) {

	}*/
	if(query.flag & QUERY_EQUIP_CARD) {
		const auto& equip = query.equip_card;
		ClientCard* ecard = mainGame->dField.GetCard(mainGame->LocalPlayer(equip.controler), equip.location, equip.sequence);
		if(ecard) {
			equipTarget = ecard;
			ecard->equipped.insert(this);
		}
	}
	if(query.flag & QUERY_TARGET_CARD) {
		for(const auto& card : query.target_cards) {
			ClientCard* tcard = mainGame->dField.GetCard(mainGame->LocalPlayer(card.controler), card.location, card.sequence);
			cardTarget.insert(tcard);
			tcard->ownerTarget.insert(this);
		}
	}
	if(query.flag & QUERY_OVERLAY_CARD) {
		size_t i = 0;
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
		if(IsDifferent(is_public, !!query.is_public) && !mainGame->dInfo.isCatchingUp)
			mainGame->dField.MoveCard(this, 5);
	}
	if(query.flag & QUERY_LSCALE) {
		if(IsDifferent(lscale, query.lscale))
			lscstring = fmt::to_wstring(lscale);
	}
	if(query.flag & QUERY_RSCALE) {
		if(IsDifferent(rscale, query.rscale))
			rscstring = fmt::to_wstring(rscale);
	}
	if(query.flag & QUERY_LINK) {
		if(IsDifferent(link, query.link) || linkstring.empty())
			linkstring = fmt::format(L"L{}", link);
		link_marker = query.link_marker;
	}
}
void ClientCard::ClearTarget() {
	for(auto& pcard : cardTarget) {
		pcard->is_showtarget = false;
		pcard->ownerTarget.erase(this);
	}
	for(auto& pcard : ownerTarget) {
		pcard->is_showtarget = false;
		pcard->ownerTarget.erase(this);
	}
	cardTarget.clear();
	ownerTarget.clear();
}
bool ClientCard::client_card_sort(ClientCard* c1, ClientCard* c2) {
	uint8_t cp1 = c1->overlayTarget ? c1->overlayTarget->controler : c1->controler;
	uint8_t cp2 = c2->overlayTarget ? c2->overlayTarget->controler : c2->controler;
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
}
