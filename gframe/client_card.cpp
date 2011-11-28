#include "client_card.h"
#include "game.h"

extern ygo::Game* mainGame;

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
	is_highlighting = false;
	is_disabled = false;
	cmdFlag = 0;
	code = 0;
	type = 0;
	alias = 0;
	level = 0;
	rank = 0;
	race = 0;
	attribute = 0;
	attack = 0;
	defence = 0;
	base_attack = 0;
	base_defence = 0;
	turnCounter = 0;
	atkstring[0] = 0;
	defstring[0] = 0;
	lvstring[0] = 0;
	overlayTarget = 0;
	equipTarget = 0;
}
void ClientCard::SetCode(int code) {
	if(location == LOCATION_HAND && controler == 1 && this->code != code)
		mainGame->dField.MoveCard(this, 5);
	this->code = code;
}
void ClientCard::UpdateInfo(char* buf) {
	int flag = NetManager::ReadInt32(buf);
	if(flag == 0)
		return;
	int pdata;
	if(flag & QUERY_CODE) {
		pdata = NetManager::ReadInt32(buf);
		if(location == LOCATION_HAND && controler == 1 && pdata != code)
			mainGame->dField.MoveCard(this, 5);
		code = pdata;
	}
	if(flag & QUERY_POSITION)
		pdata = NetManager::ReadInt32(buf);
	if(flag & QUERY_ALIAS)
		alias = NetManager::ReadInt32(buf);
	if(flag & QUERY_TYPE)
		type = NetManager::ReadInt32(buf);
	if(flag & QUERY_LEVEL) {
		pdata = NetManager::ReadInt32(buf);
		if(pdata && level != pdata) {
			level = pdata;
			myswprintf(lvstring, L"L%d", level);
		}
	}
	if(flag & QUERY_RANK) {
		pdata = NetManager::ReadInt32(buf);
		if(pdata && rank != pdata) {
			rank = pdata;
			myswprintf(lvstring, L"R%d", rank);
		}
	}
	if(flag & QUERY_ATTRIBUTE)
		attribute = NetManager::ReadInt32(buf);
	if(flag & QUERY_RACE)
		race = NetManager::ReadInt32(buf);
	if(flag & QUERY_ATTACK) {
		attack = NetManager::ReadInt32(buf);
		if(attack < 0) {
			atkstring[0] = '?';
			atkstring[1] = 0;
		} else
			myswprintf(atkstring, L"%d", attack);
	}
	if(flag & QUERY_DEFENCE) {
		defence = NetManager::ReadInt32(buf);
		if(defence < 0) {
			defstring[0] = '?';
			defstring[1] = 0;
		} else
			myswprintf(defstring, L"%d", defence);
	}
	if(flag & QUERY_BASE_ATTACK)
		base_attack = NetManager::ReadInt32(buf);
	if(flag & QUERY_BASE_DEFENCE)
		base_defence = NetManager::ReadInt32(buf);
	if(flag & QUERY_REASON)
		reason = NetManager::ReadInt32(buf);
	if(flag & QUERY_IS_DISABLED)
		is_disabled = NetManager::ReadInt32(buf);
	if(flag & QUERY_IS_PUBLIC)
		is_public = NetManager::ReadInt32(buf);
}
void ClientCard::ClearTarget() {
	for(auto cit = cardTarget.begin(); cit != cardTarget.end(); ++cit)
		(*cit)->ownerTarget.erase(this);
	for(auto cit = ownerTarget.begin(); cit != ownerTarget.end(); ++cit)
		(*cit)->cardTarget.erase(this);
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
		if(c1->location & 0x71)
			return c1->sequence > c2->sequence;
		else
			return c1->sequence < c2->sequence;
	}
}
bool ClientCard::deck_sort_atk(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		int type1 = (p1->second.type & 0x8020c0) ? (p1->second.type & 0x8020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x8020c0) ? (p2->second.type & 0x8020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->second.attack != p2->second.attack)
			return p1->second.attack > p2->second.attack;
		else return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool ClientCard::deck_sort_def(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		int type1 = (p1->second.type & 0x8020c0) ? (p1->second.type & 0x8020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x8020c0) ? (p2->second.type & 0x8020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->second.defence != p2->second.defence)
			return p1->second.defence > p2->second.defence;
		else return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
bool ClientCard::deck_sort_lv(code_pointer p1, code_pointer p2) {
	if((p1->second.type & 0x7) != (p2->second.type & 0x7))
		return (p1->second.type & 0x7) < (p2->second.type & 0x7);
	if((p1->second.type & 0x7) == 1) {
		int type1 = (p1->second.type & 0x8020c0) ? (p1->second.type & 0x8020c1) : (p1->second.type & 0x31);
		int type2 = (p2->second.type & 0x8020c0) ? (p2->second.type & 0x8020c1) : (p2->second.type & 0x31);
		if(type1 != type2)
			return type1 < type2;
		if(p1->second.level != p2->second.level)
			return p1->second.level > p2->second.level;
		else return p1->first < p2->first;
	}
	if((p1->second.type & 0xfffffff8) != (p2->second.type & 0xfffffff8))
		return (p1->second.type & 0xfffffff8) < (p2->second.type & 0xfffffff8);
	return p1->first < p2->first;
}
}
