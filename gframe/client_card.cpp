#include "client_card.h"
#include "client_field.h"
#include "data_manager.h"
#include "game.h"

namespace ygo {

ClientCard::ClientCard() {
	curAlpha = 255;
	dAlpha = 0;
	aniFrame = 0;
	is_moving = false;
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
	cmdFlag = 0;
	code = 0;
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
	atkstring[0] = 0;
	defstring[0] = 0;
	lvstring[0] = 0;
	linkstring[0] = 0;
	rscstring[0] = 0;
	lscstring[0] = 0;
	overlayTarget = 0;
	equipTarget = 0;
}
void ClientCard::SetCode(int code) {
	if((location == LOCATION_HAND) && (this->code != (unsigned int)code)) {
		this->code = code;
		mainGame->dField.MoveCard(this, 5);
	} else
		this->code = code;
}
void ClientCard::UpdateInfo(char* buf) {
	int flag = BufferIO::ReadInt32(buf);
	if(flag == 0)
		return;
	int pdata;
	if(flag & QUERY_CODE) {
		pdata = BufferIO::ReadInt32(buf);
		if((location == LOCATION_HAND) && ((unsigned int)pdata != code)) {
			code = pdata;
			mainGame->dField.MoveCard(this, 5);
		} else
			code = pdata;
	}
	if(flag & QUERY_POSITION) {
		pdata = (BufferIO::ReadInt32(buf) >> 24) & 0xff;
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
		pdata = BufferIO::ReadInt32(buf);
		if(level != (unsigned int)pdata) {
			level = pdata;
			myswprintf(lvstring, L"L%d", level);
		}
	}
	if(flag & QUERY_RANK) {
		pdata = BufferIO::ReadInt32(buf);
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
		int c = BufferIO::ReadInt8(buf);
		int l = BufferIO::ReadInt8(buf);
		int s = BufferIO::ReadInt8(buf);
		BufferIO::ReadInt8(buf);
		ClientCard* ecard = mainGame->dField.GetCard(mainGame->LocalPlayer(c), l, s);
		equipTarget = ecard;
		ecard->equipped.insert(this);
	}
	if(flag & QUERY_TARGET_CARD) {
		int count = BufferIO::ReadInt32(buf);
		for(int i = 0; i < count; ++i) {
			int c = BufferIO::ReadInt8(buf);
			int l = BufferIO::ReadInt8(buf);
			int s = BufferIO::ReadInt8(buf);
			BufferIO::ReadInt8(buf);
			ClientCard* tcard = mainGame->dField.GetCard(mainGame->LocalPlayer(c), l, s);
			cardTarget.insert(tcard);
			tcard->ownerTarget.insert(this);
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
		pdata = BufferIO::ReadInt32(buf);
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
	if(c1->is_selected != c2->is_selected)
		return c1->is_selected < c2->is_selected;
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
bool ClientCard::deck_sort_lv(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		if(p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if(p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool ClientCard::deck_sort_atk(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		if(p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if(p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		if(p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool ClientCard::deck_sort_def(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		if(p1->second.defense != p2->second.defense)
			return p1->second.defense > p2->second.defense;
		if(p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		if(p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		int type1 = (p1->second.type & 0x48020c0) ? (p1->second.type & 0x48020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x48020c0) ? (p2->second.type & 0x48020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool ClientCard::deck_sort_name(code_pointer p1, code_pointer p2) {
	const wchar_t* name1 = dataManager.GetName(p1->first);
	const wchar_t* name2 = dataManager.GetName(p2->first);
	int res = wcscmp(name1, name2);
	if(res != 0)
		return res < 0;
	return p1->first < p2->first;
}
}
