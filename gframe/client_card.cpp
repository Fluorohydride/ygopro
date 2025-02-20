#include "client_card.h"
#include "client_field.h"
#include "game.h"

namespace ygo {

ClientCard::~ClientCard() {
	ClearTarget();
	if (equipTarget) {
		equipTarget->is_showequip = false;
		equipTarget->equipped.erase(this);
		equipTarget = nullptr;
	}
	for (const auto& card : equipped) {
		card->is_showequip = false;
		card->equipTarget = nullptr;
	}
	equipped.clear();
	if (overlayTarget) {
		for (auto it = overlayTarget->overlayed.begin(); it != overlayTarget->overlayed.end(); ) {
			if (*it == this) {
				it = overlayTarget->overlayed.erase(it);
			}
			else
				++it;
		}
		overlayTarget = nullptr;
	}
	for (auto& card : overlayed) {
		card->overlayTarget = nullptr;
	}
	overlayed.clear();
}
void ClientCard::SetCode(unsigned int x) {
	if((location == LOCATION_HAND) && (code != x)) {
		code = x;
		mainGame->dField.MoveCard(this, 5);
	} else
		code = x;
}
void ClientCard::UpdateInfo(unsigned char* buf) {
	int flag = BufferIO::ReadInt32(buf);
	if (flag == 0) {
		ClearData();
		return;
	}
	if(flag & QUERY_CODE) {
		int pdata = BufferIO::ReadInt32(buf);
		if (!pdata)
			ClearData();
		if((location == LOCATION_HAND) && ((unsigned int)pdata != code)) {
			code = pdata;
			mainGame->dField.MoveCard(this, 5);
		} else
			code = pdata;
	}
	if(flag & QUERY_POSITION) {
		int pdata = (BufferIO::ReadInt32(buf) >> 24) & 0xff;
		if((location & (LOCATION_EXTRA | LOCATION_REMOVED)) && (u8)pdata != position) {
			position = pdata;
			mainGame->dField.MoveCard(this, 1);
		} else
			position = pdata;
	}
	if(flag & QUERY_ALIAS)
		alias = BufferIO::ReadInt32(buf);
	if(flag & QUERY_TYPE)
		type = BufferIO::ReadInt32(buf);
	if(flag & QUERY_LEVEL) {
		int pdata = BufferIO::ReadInt32(buf);
		if(level != (unsigned int)pdata) {
			level = pdata;
			myswprintf(lvstring, L"L%d", level);
		}
	}
	if(flag & QUERY_RANK) {
		int pdata = BufferIO::ReadInt32(buf);
		if(pdata && rank != (unsigned int)pdata) {
			rank = pdata;
			myswprintf(lvstring, L"R%d", rank);
		}
	}
	if(flag & QUERY_ATTRIBUTE)
		attribute = BufferIO::ReadInt32(buf);
	if(flag & QUERY_RACE)
		race = BufferIO::ReadInt32(buf);
	if(flag & QUERY_ATTACK) {
		attack = BufferIO::ReadInt32(buf);
		if(attack < 0) {
			atkstring[0] = '?';
			atkstring[1] = 0;
		} else
			myswprintf(atkstring, L"%d", attack);
	}
	if(flag & QUERY_DEFENSE) {
		defense = BufferIO::ReadInt32(buf);
		if(type & TYPE_LINK) {
			defstring[0] = '-';
			defstring[1] = 0;
		} else if(defense < 0) {
			defstring[0] = '?';
			defstring[1] = 0;
		} else
			myswprintf(defstring, L"%d", defense);
	}
	if(flag & QUERY_BASE_ATTACK)
		base_attack = BufferIO::ReadInt32(buf);
	if(flag & QUERY_BASE_DEFENSE)
		base_defense = BufferIO::ReadInt32(buf);
	if(flag & QUERY_REASON)
		reason = BufferIO::ReadInt32(buf);
	if(flag & QUERY_REASON_CARD)
		buf += 4;
	if(flag & QUERY_EQUIP_CARD) {
		int c = BufferIO::ReadUInt8(buf);
		unsigned int l = BufferIO::ReadUInt8(buf);
		int s = BufferIO::ReadUInt8(buf);
		BufferIO::ReadUInt8(buf);
		ClientCard* ecard = mainGame->dField.GetCard(mainGame->LocalPlayer(c), l, s);
		if (ecard) {
			equipTarget = ecard;
			ecard->equipped.insert(this);
		}
	}
	if(flag & QUERY_TARGET_CARD) {
		int count = BufferIO::ReadInt32(buf);
		for(int i = 0; i < count; ++i) {
			int c = BufferIO::ReadUInt8(buf);
			unsigned int l = BufferIO::ReadUInt8(buf);
			int s = BufferIO::ReadUInt8(buf);
			BufferIO::ReadUInt8(buf);
			ClientCard* tcard = mainGame->dField.GetCard(mainGame->LocalPlayer(c), l, s);
			if (tcard) {
				cardTarget.insert(tcard);
				tcard->ownerTarget.insert(this);
			}
		}
	}
	if(flag & QUERY_OVERLAY_CARD) {
		int count = BufferIO::ReadInt32(buf);
		for(int i = 0; i < count; ++i) {
			overlayed[i]->SetCode(BufferIO::ReadInt32(buf));
		}
	}
	if(flag & QUERY_COUNTERS) {
		int count = BufferIO::ReadInt32(buf);
		for(int i = 0; i < count; ++i) {
			int ctype = BufferIO::ReadInt16(buf);
			int ccount = BufferIO::ReadInt16(buf);
			counters[ctype] = ccount;
		}
	}
	if(flag & QUERY_OWNER)
		owner = BufferIO::ReadInt32(buf);
	if(flag & QUERY_STATUS)
		status = BufferIO::ReadInt32(buf);
	if(flag & QUERY_LSCALE) {
		lscale = BufferIO::ReadInt32(buf);
		myswprintf(lscstring, L"%d", lscale);
	}
	if(flag & QUERY_RSCALE) {
		rscale = BufferIO::ReadInt32(buf);
		myswprintf(rscstring, L"%d", rscale);
	}
	if(flag & QUERY_LINK) {
		int pdata = BufferIO::ReadInt32(buf);
		if (link != (unsigned int)pdata) {
			link = pdata;
		}
		myswprintf(linkstring, L"L\x2012%d", link);
		pdata = BufferIO::ReadInt32(buf);
		if (link_marker != (unsigned int)pdata) {
			link_marker = pdata;
		}
	}
}
void ClientCard::ClearTarget() {
	for (const auto& pcard : cardTarget) {
		pcard->is_showtarget = false;
		pcard->ownerTarget.erase(this);
	}
	for (const auto& pcard : ownerTarget) {
		pcard->is_showtarget = false;
		pcard->cardTarget.erase(this);
	}
	cardTarget.clear();
	ownerTarget.clear();
}
void ClientCard::ClearData() {
	alias = 0;
	type = 0;
	level = 0;
	rank = 0;
	race = 0;
	attribute = 0;
	attack = 0;
	defense = 0;
	base_attack = 0;
	base_defense = 0;
	lscale = 0;
	rscale = 0;
	link = 0;
	link_marker = 0;
	status = 0;
	
	atkstring[0] = 0;
	defstring[0] = 0;
	lvstring[0] = 0;
	linkstring[0] = 0;
	rscstring[0] = 0;
	lscstring[0] = 0;
	counters.clear();
}
bool ClientCard::client_card_sort(ClientCard* c1, ClientCard* c2) {
	if(c1->is_selected != c2->is_selected)
		return c1->is_selected < c2->is_selected;
	int cp1 = c1->overlayTarget ? c1->overlayTarget->controler : c1->controler;
	int cp2 = c2->overlayTarget ? c2->overlayTarget->controler : c2->controler;
	if(cp1 != cp2)
		return cp1 < cp2;
	if(c1->location != c2->location)
		return c1->location < c2->location;
	if (c1->location == LOCATION_OVERLAY) {
		if (c1->overlayTarget != c2->overlayTarget)
			return c1->overlayTarget->sequence < c2->overlayTarget->sequence;
		else
			return c1->sequence < c2->sequence;
	}
	else if (c1->location == LOCATION_DECK) {
		return c1->sequence > c2->sequence;
	}
	else if (c1->location & (LOCATION_GRAVE | LOCATION_REMOVED | LOCATION_EXTRA)) {
		auto it1 = std::find_if(mainGame->dField.chains.rbegin(), mainGame->dField.chains.rend(), [c1](const ChainInfo& ch) {
			return c1 == ch.chain_card || ch.target.find(c1) != ch.target.end();
		});
		auto it2 = std::find_if(mainGame->dField.chains.rbegin(), mainGame->dField.chains.rend(), [c2](const ChainInfo& ch) {
			return c2 == ch.chain_card || ch.target.find(c2) != ch.target.end();
		});
		if (it1 != mainGame->dField.chains.rend() || it2 != mainGame->dField.chains.rend()) {
			return it1 < it2;
		}
		return c1->sequence > c2->sequence;
	}
	else {
		return c1->sequence < c2->sequence;
	}
}
}
