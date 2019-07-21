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
void ClientCard::SetCode(int code) {
	if((location == LOCATION_HAND) && (this->code != (unsigned int)code)) {
		this->code = code;
		if(!mainGame->dInfo.isCatchingUp)
			mainGame->dField.MoveCard(this, 5);
	} else
		this->code = code;
}
void ClientCard::UpdateInfo(char* buf) {
	int flag = BufferIO::Read<int32_t>(buf);
	if(flag == 0)
		return;
	int pdata;
	if(flag & QUERY_CODE) {
		pdata = BufferIO::Read<int32_t>(buf);
		if((location == LOCATION_HAND) && ((unsigned int)pdata != code)) {
			code = pdata;
			if(!mainGame->dInfo.isCatchingUp)
				mainGame->dField.MoveCard(this, 5);
		} else
			code = pdata;
	}
	if(flag & QUERY_POSITION) {
		pdata = (BufferIO::Read<int32_t>(buf) >> 24) & 0xff;
		if((location & (LOCATION_EXTRA | LOCATION_REMOVED)) && (u8)pdata != position) {
			position = pdata;
			mainGame->dField.MoveCard(this, 1);
		} else
			position = pdata;
	}
	if(flag & QUERY_ALIAS)
		alias = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_TYPE)
		type = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_LEVEL) {
		pdata = BufferIO::Read<int32_t>(buf);
		if(level != (unsigned int)pdata) {
			level = pdata;
			lvstring = fmt::format(L"L{}",level);
		}
	}
	if(flag & QUERY_RANK) {
		pdata = BufferIO::Read<int32_t>(buf);
		if(rank != (unsigned int)pdata) {
			rank = pdata;
			rkstring = fmt::format(L"R{}", rank);
		}
	}
	if(flag & QUERY_ATTRIBUTE)
		attribute = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_RACE)
		race = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_ATTACK) {
		attack = BufferIO::Read<int32_t>(buf);
		if(attack < 0) {
			atkstring = L"?";
		} else
			atkstring = fmt::to_wstring(attack);
	}
	if(flag & QUERY_DEFENSE) {
		defense = BufferIO::Read<int32_t>(buf);
		if(type & TYPE_LINK) {
			defstring = L"-";
		} else if(defense < 0) {
			defstring = L"?";
		} else
			defstring = fmt::to_wstring(defense);
	}
	if(flag & QUERY_BASE_ATTACK)
		base_attack = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_BASE_DEFENSE)
		base_defense = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_REASON)
		reason = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_REASON_CARD)
		buf += 10;
	if(flag & QUERY_EQUIP_CARD) {
		loc_info info = read_location_info(buf);
		ClientCard* ecard = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence);
		equipTarget = ecard;
		ecard->equipped.insert(this);
	}
	if(flag & QUERY_TARGET_CARD) {
		int count = BufferIO::Read<int32_t>(buf);
		for(int i = 0; i < count; ++i) {
			loc_info info = read_location_info(buf);
			ClientCard* tcard = mainGame->dField.GetCard(mainGame->LocalPlayer(info.controler), info.location, info.sequence);
			cardTarget.insert(tcard);
			tcard->ownerTarget.insert(this);
		}
	}
	if(flag & QUERY_OVERLAY_CARD) {
		int count = BufferIO::Read<int32_t>(buf);
		for(int i = 0; i < count; ++i) {
			overlayed[i]->SetCode(BufferIO::Read<int32_t>(buf));
		}
	}
	if(flag & QUERY_COUNTERS) {
		int count = BufferIO::Read<int32_t>(buf);
		for(int i = 0; i < count; ++i) {
			int ctype = BufferIO::Read<uint16_t>(buf);
			int ccount = BufferIO::Read<uint16_t>(buf);
			counters[ctype] = ccount;
		}
	}
	if(flag & QUERY_OWNER)
		owner = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_STATUS)
		status = BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_IS_PUBLIC)
		BufferIO::Read<int32_t>(buf);
	if(flag & QUERY_LSCALE) {
		lscale = BufferIO::Read<int32_t>(buf);
		lscstring = fmt::to_wstring(lscale);
	}
	if(flag & QUERY_RSCALE) {
		rscale = BufferIO::Read<int32_t>(buf);
		rscstring = fmt::to_wstring(rscale);
	}
	if(flag & QUERY_LINK) {
		pdata = BufferIO::Read<int32_t>(buf);
		if (link != (unsigned int)pdata) {
			link = pdata;
			linkstring = fmt::format(L"L{}", link);
		}
		pdata = BufferIO::Read<int32_t>(buf);
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
loc_info ClientCard::read_location_info(char*& p) {
	loc_info info;
	info.controler = BufferIO::Read<uint8_t>(p);
	info.location = BufferIO::Read<uint8_t>(p);
	if (mainGame->dInfo.compat_mode) {
		info.sequence = BufferIO::Read<uint8_t>(p);
		info.position = BufferIO::Read<uint8_t>(p);
	} else {
		info.sequence = BufferIO::Read<int32_t>(p);
		info.position = BufferIO::Read<int32_t>(p);
	}
	return info;
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
bool ClientCard::deck_sort_lv(CardDataC* p1, CardDataC* p2) {
	if((p1->type & 0x7) != (p2->type & 0x7))
		return (p1->type & 0x7) < (p2->type & 0x7);
	if((p1->type & 0x7) == 1) {
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
	}
	if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
		return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
	return p1->code < p2->code;
}
bool ClientCard::deck_sort_atk(CardDataC* p1, CardDataC* p2) {
	if((p1->type & 0x7) != (p2->type & 0x7))
		return (p1->type & 0x7) < (p2->type & 0x7);
	if((p1->type & 0x7) == 1) {
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
	}
	if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
		return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
	return p1->code < p2->code;
}
bool ClientCard::deck_sort_def(CardDataC* p1, CardDataC* p2) {
	if((p1->type & 0x7) != (p2->type & 0x7))
		return (p1->type & 0x7) < (p2->type & 0x7);
	if((p1->type & 0x7) == 1) {
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
	}
	if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
		return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
	return p1->code < p2->code;
}
bool ClientCard::deck_sort_name(CardDataC* p1, CardDataC* p2) {
	int res = dataManager.GetName(p1->code).compare(dataManager.GetName(p2->code));
	if(res != 0)
		return res < 0;
	return p1->code < p2->code;
}
}
