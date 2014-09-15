#include "client_field.h"
#include "client_card.h"
#include "data_manager.h"
#include "image_manager.h"
#include "game.h"
#include "materials.h"
#include "../ocgcore/field.h"

namespace ygo {

ClientField::ClientField() {
	panel = 0;
	hovered_card = 0;
	clicked_card = 0;
	highlighting_card = 0;
	deck_act = false;
	grave_act = false;
	remove_act = false;
	extra_act = false;
	pzone_act = false;
	deck_reversed = false;
	for(int p = 0; p < 2; ++p) {
		for(int i = 0; i < 5; ++i)
			mzone[p].push_back(0);
		for(int i = 0; i < 8; ++i)
			szone[p].push_back(0);
	}
}
void ClientField::Clear() {
	for(int i = 0; i < 2; ++i) {
		for(auto cit = deck[i].begin(); cit != deck[i].end(); ++cit)
			delete *cit;
		deck[i].clear();
		for(auto cit = hand[i].begin(); cit != hand[i].end(); ++cit)
			delete *cit;
		hand[i].clear();
		for(auto cit = mzone[i].begin(); cit != mzone[i].end(); ++cit) {
			if(*cit)
				delete *cit;
			*cit = 0;
		}
		for(auto cit = szone[i].begin(); cit != szone[i].end(); ++cit) {
			if(*cit)
				delete *cit;
			*cit = 0;
		}
		for(auto cit = grave[i].begin(); cit != grave[i].end(); ++cit)
			delete *cit;
		grave[i].clear();
		for(auto cit = remove[i].begin(); cit != remove[i].end(); ++cit)
			delete *cit;
		remove[i].clear();
		for(auto cit = extra[i].begin(); cit != extra[i].end(); ++cit)
			delete *cit;
		extra[i].clear();
	}
	for(auto sit = overlay_cards.begin(); sit != overlay_cards.end(); ++sit)
		delete *sit;
	overlay_cards.clear();
	chains.clear();
	disabled_field = 0;
	deck_act = false;
	grave_act = false;
	remove_act = false;
	extra_act = false;
	pzone_act = false;
	deck_reversed = false;
}
void ClientField::Initial(int player, int deckc, int extrac) {
	ClientCard* pcard;
	for(int i = 0; i < deckc; ++i) {
		pcard = new ClientCard;
		deck[player].push_back(pcard);
		pcard->owner = player;
		pcard->controler = player;
		pcard->location = 0x1;
		pcard->sequence = i;
		GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
		pcard->mTransform.setTranslation(pcard->curPos);
		pcard->mTransform.setRotationRadians(pcard->curRot);
	}
	for(int i = 0; i < extrac; ++i) {
		pcard = new ClientCard;
		extra[player].push_back(pcard);
		pcard->owner = player;
		pcard->controler = player;
		pcard->location = 0x40;
		pcard->sequence = i;
		pcard->position = POS_FACEDOWN_DEFENCE;
		GetCardLocation(pcard, &pcard->curPos, &pcard->curRot);
		pcard->mTransform.setTranslation(pcard->curPos);
		pcard->mTransform.setRotationRadians(pcard->curRot);
	}
}
ClientCard* ClientField::GetCard(int controler, int location, int sequence, int sub_seq) {
	std::vector<ClientCard*>* lst = 0;
	bool is_xyz = (location & 0x80) != 0;
	location &= 0x7f;
	switch(location) {
	case LOCATION_DECK:
		lst = &deck[controler];
		break;
	case LOCATION_HAND:
		lst = &hand[controler];
		break;
	case LOCATION_MZONE:
		lst = &mzone[controler];
		break;
	case LOCATION_SZONE:
		lst = &szone[controler];
		break;
	case LOCATION_GRAVE:
		lst = &grave[controler];
		break;
	case LOCATION_REMOVED:
		lst = &remove[controler];
		break;
	case LOCATION_EXTRA:
		lst = &extra[controler];
		break;
	}
	if(!lst)
		return 0;
	if(is_xyz) {
		if(sequence >= (int)lst->size())
			return 0;
		ClientCard* scard = (*lst)[sequence];
		if(scard && (int)scard->overlayed.size() > sub_seq)
			return scard->overlayed[sub_seq];
		else
			return 0;
	} else {
		if(sequence >= (int)lst->size())
			return 0;
		return (*lst)[sequence];
	}
}
void ClientField::AddCard(ClientCard* pcard, int controler, int location, int sequence) {
	pcard->controler = controler;
	pcard->location = location;
	pcard->sequence = sequence;
	switch(location) {
	case LOCATION_DECK: {
		if (sequence != 0 || deck[controler].size() == 0) {
			deck[controler].push_back(pcard);
			pcard->sequence = deck[controler].size() - 1;
		} else {
			deck[controler].push_back(0);
			for(int i = deck[controler].size() - 1; i > 0; --i) {
				deck[controler][i] = deck[controler][i - 1];
				deck[controler][i]->sequence++;
			}
			deck[controler][0] = pcard;
			pcard->sequence = 0;
		}
		pcard->is_reversed = false;
		break;
	}
	case LOCATION_HAND: {
		hand[controler].push_back(pcard);
		pcard->sequence = hand[controler].size() - 1;
		break;
	}
	case LOCATION_MZONE: {
		mzone[controler][sequence] = pcard;
		break;
	}
	case LOCATION_SZONE: {
		szone[controler][sequence] = pcard;
		break;
	}
	case LOCATION_GRAVE: {
		grave[controler].push_back(pcard);
		pcard->sequence = grave[controler].size() - 1;
		break;
	}
	case LOCATION_REMOVED: {
		remove[controler].push_back(pcard);
		pcard->sequence = remove[controler].size() - 1;
		break;
	}
	case LOCATION_EXTRA: {
		extra[controler].push_back(pcard);
		pcard->sequence = extra[controler].size() - 1;
		break;
	}
	}
}
ClientCard* ClientField::RemoveCard(int controler, int location, int sequence) {
	ClientCard* pcard = 0;
	switch (location) {
	case LOCATION_DECK: {
		pcard = deck[controler][sequence];
		for (size_t i = sequence; i < deck[controler].size() - 1; ++i) {
			deck[controler][i] = deck[controler][i + 1];
			deck[controler][i]->sequence--;
			deck[controler][i]->curPos -= irr::core::vector3df(0, 0, 0.01f);
			deck[controler][i]->mTransform.setTranslation(deck[controler][i]->curPos);
		}
		deck[controler].erase(deck[controler].end() - 1);
		break;
	}
	case LOCATION_HAND: {
		pcard = hand[controler][sequence];
		for (size_t i = sequence; i < hand[controler].size() - 1; ++i) {
			hand[controler][i] = hand[controler][i + 1];
			hand[controler][i]->sequence--;
		}
		hand[controler].erase(hand[controler].end() - 1);
		break;
	}
	case LOCATION_MZONE: {
		pcard = mzone[controler][sequence];
		mzone[controler][sequence] = 0;
		break;
	}
	case LOCATION_SZONE: {
		pcard = szone[controler][sequence];
		szone[controler][sequence] = 0;
		break;
	}
	case LOCATION_GRAVE: {
		pcard = grave[controler][sequence];
		for (size_t i = sequence; i < grave[controler].size() - 1; ++i) {
			grave[controler][i] = grave[controler][i + 1];
			grave[controler][i]->sequence--;
			grave[controler][i]->curPos -= irr::core::vector3df(0, 0, 0.01f);
			grave[controler][i]->mTransform.setTranslation(grave[controler][i]->curPos);
		}
		grave[controler].erase(grave[controler].end() - 1);
		break;
	}
	case LOCATION_REMOVED: {
		pcard = remove[controler][sequence];
		for (size_t i = sequence; i < remove[controler].size() - 1; ++i) {
			remove[controler][i] = remove[controler][i + 1];
			remove[controler][i]->sequence--;
			remove[controler][i]->curPos -= irr::core::vector3df(0, 0, 0.01f);
			remove[controler][i]->mTransform.setTranslation(remove[controler][i]->curPos);
		}
		remove[controler].erase(remove[controler].end() - 1);
		break;
	}
	case LOCATION_EXTRA: {
		pcard = extra[controler][sequence];
		for (size_t i = sequence; i < extra[controler].size() - 1; ++i) {
			extra[controler][i] = extra[controler][i + 1];
			extra[controler][i]->sequence--;
			extra[controler][i]->curPos -= irr::core::vector3df(0, 0, 0.01f);
			extra[controler][i]->mTransform.setTranslation(extra[controler][i]->curPos);
		}
		extra[controler].erase(extra[controler].end() - 1);
		break;
	}
	}
	pcard->location = 0;
	return pcard;
}
void ClientField::UpdateCard(int controler, int location, int sequence, char* data) {
	ClientCard* pcard = GetCard(controler, location, sequence);
	if(pcard)
		pcard->UpdateInfo(data + 4);
}
void ClientField::UpdateFieldCard(int controler, int location, char* data) {
	std::vector<ClientCard*>* lst = 0;
	std::vector<ClientCard*>::iterator cit;
	switch(location) {
	case LOCATION_DECK:
		lst = &deck[controler];
		break;
	case LOCATION_HAND:
		lst = &hand[controler];
		break;
	case LOCATION_MZONE:
		lst = &mzone[controler];
		break;
	case LOCATION_SZONE:
		lst = &szone[controler];
		break;
	case LOCATION_GRAVE:
		lst = &grave[controler];
		break;
	case LOCATION_REMOVED:
		lst = &remove[controler];
		break;
	case LOCATION_EXTRA:
		lst = &extra[controler];
		break;
	}
	if(!lst)
		return;
	int len;
	for(cit = lst->begin(); cit != lst->end(); ++cit) {
		len = BufferIO::ReadInt32(data);
		if(len > 8)
			(*cit)->UpdateInfo(data);
		data += len - 4;
	}
}
void ClientField::ClearCommandFlag() {
	std::vector<ClientCard*>::iterator cit;
	for(cit = activatable_cards.begin(); cit != activatable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = summonable_cards.begin(); cit != summonable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = spsummonable_cards.begin(); cit != spsummonable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = msetable_cards.begin(); cit != msetable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = ssetable_cards.begin(); cit != ssetable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = reposable_cards.begin(); cit != reposable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	for(cit = attackable_cards.begin(); cit != attackable_cards.end(); ++cit)
		(*cit)->cmdFlag = 0;
	deck_act = false;
	extra_act = false;
	grave_act = false;
	remove_act = false;
	pzone_act = false;
}
void ClientField::ClearSelect() {
	std::vector<ClientCard*>::iterator cit;
	for(cit = selectable_cards.begin(); cit != selectable_cards.end(); ++cit) {
		(*cit)->is_selectable = false;
		(*cit)->is_selected = false;
	}
}
void ClientField::ClearChainSelect() {
	std::vector<ClientCard*>::iterator cit;
	for(cit = activatable_cards.begin(); cit != activatable_cards.end(); ++cit) {
		(*cit)->cmdFlag = 0;
		(*cit)->is_selectable = false;
		(*cit)->is_selected = false;
	}
	grave_act = false;
	remove_act = false;
}
void ClientField::ShowSelectCard(bool buttonok) {
	if(selectable_cards.size() <= 5) {
		int startpos = 30 + 125 * (5 - selectable_cards.size()) / 2;
		for(size_t i = 0; i < selectable_cards.size(); ++i) {
			if(selectable_cards[i]->code)
				mainGame->imageLoading.insert(std::make_pair(mainGame->btnCardSelect[i], selectable_cards[i]->code));
			else
				mainGame->btnCardSelect[i]->setImage(imageManager.tCover);
			mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(startpos + i * 125, 55, startpos + 120 + i * 125, 225));
			mainGame->btnCardSelect[i]->setPressed(false);
			mainGame->btnCardSelect[i]->setVisible(true);
			if(mainGame->dInfo.curMsg != MSG_SORT_CHAIN && mainGame->dInfo.curMsg != MSG_SORT_CARD) {
				myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
					selectable_cards[i]->sequence + 1);
				mainGame->stCardPos[i]->setText(formatBuffer);
				mainGame->stCardPos[i]->setVisible(true);;
				if(selectable_cards[i]->controler)
					mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
				else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			} else {
				if(sort_list[i]) {
					myswprintf(formatBuffer, L"%d", sort_list[i]);
					mainGame->stCardPos[i]->setText(formatBuffer);
				} else mainGame->stCardPos[i]->setText(L"");
				mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			}
			mainGame->stCardPos[i]->setRelativePosition(rect<s32>(startpos + 10 + i * 125, 30, startpos + 109 + i * 125, 50));
		}
		for(int i = selectable_cards.size(); i < 5; ++i) {
			mainGame->btnCardSelect[i]->setVisible(false);
			mainGame->stCardPos[i]->setVisible(false);
		}
		mainGame->scrCardList->setPos(0);
		mainGame->scrCardList->setVisible(false);
	} else {
		for(int i = 0; i < 5; ++i) {
			if(selectable_cards[i]->code)
				mainGame->imageLoading.insert(std::make_pair(mainGame->btnCardSelect[i], selectable_cards[i]->code));
			else
				mainGame->btnCardSelect[i]->setImage(imageManager.tCover);
			mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
			mainGame->btnCardSelect[i]->setPressed(false);
			mainGame->btnCardSelect[i]->setVisible(true);
			if(mainGame->dInfo.curMsg != MSG_SORT_CHAIN && mainGame->dInfo.curMsg != MSG_SORT_CARD) {
				myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
					selectable_cards[i]->sequence + 1);
				mainGame->stCardPos[i]->setText(formatBuffer);
				mainGame->stCardPos[i]->setVisible(true);
				if(selectable_cards[i]->controler)
					mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
				else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			} else {
				if(sort_list[i]) {
					myswprintf(formatBuffer, L"%d", sort_list[i]);
					mainGame->stCardPos[i]->setText(formatBuffer);
				} else mainGame->stCardPos[i]->setText(L"");
				mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			}
			mainGame->stCardPos[i]->setRelativePosition(rect<s32>(40 + i * 125, 30, 139 + i * 125, 50));
		}
		mainGame->scrCardList->setVisible(true);
		mainGame->scrCardList->setMin(0);
		mainGame->scrCardList->setMax((selectable_cards.size() - 5) * 10 + 9);
		mainGame->scrCardList->setPos(0);
	}
	if(buttonok)
		mainGame->btnSelectOK->setVisible(true);
	else mainGame->btnSelectOK->setVisible(false);
	mainGame->PopupElement(mainGame->wCardSelect);
}
void ClientField::ShowChainCard() {
	if(selectable_cards.size() <= 5) {
		int startpos = 30 + 125 * (5 - selectable_cards.size()) / 2;
		for(size_t i = 0; i < selectable_cards.size(); ++i) {
			if(selectable_cards[i]->code)
				mainGame->imageLoading.insert(std::make_pair(mainGame->btnCardSelect[i], selectable_cards[i]->code));
			else
				mainGame->btnCardSelect[i]->setImage(imageManager.tCover);
			mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(startpos + i * 125, 55, startpos + 120 + i * 125, 225));
			mainGame->btnCardSelect[i]->setPressed(false);
			mainGame->btnCardSelect[i]->setVisible(true);
			myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
				selectable_cards[i]->sequence + 1);
			mainGame->stCardPos[i]->setText(formatBuffer);
			mainGame->stCardPos[i]->setVisible(true);;
			if(selectable_cards[i]->controler)
				mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
			else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			mainGame->stCardPos[i]->setRelativePosition(rect<s32>(startpos + 10 + i * 125, 30, startpos + 109 + i * 125, 50));
		}
		for(int i = selectable_cards.size(); i < 5; ++i) {
			mainGame->btnCardSelect[i]->setVisible(false);
			mainGame->stCardPos[i]->setVisible(false);
		}
		mainGame->scrCardList->setPos(0);
		mainGame->scrCardList->setVisible(false);
	} else {
		for(int i = 0; i < 5; ++i) {
			if(selectable_cards[i]->code)
				mainGame->imageLoading.insert(std::make_pair(mainGame->btnCardSelect[i], selectable_cards[i]->code));
			else
				mainGame->btnCardSelect[i]->setImage(imageManager.tCover);
			mainGame->btnCardSelect[i]->setRelativePosition(rect<s32>(30 + i * 125, 55, 30 + 120 + i * 125, 225));
			mainGame->btnCardSelect[i]->setPressed(false);
			mainGame->btnCardSelect[i]->setVisible(true);
			myswprintf(formatBuffer, L"%ls[%d]", dataManager.FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
				selectable_cards[i]->sequence + 1);
			mainGame->stCardPos[i]->setText(formatBuffer);
			mainGame->stCardPos[i]->setVisible(true);
			if(selectable_cards[i]->controler)
				mainGame->stCardPos[i]->setBackgroundColor(0xffd0d0d0);
			else mainGame->stCardPos[i]->setBackgroundColor(0xffffffff);
			mainGame->stCardPos[i]->setRelativePosition(rect<s32>(40 + i * 125, 30, 139 + i * 125, 50));
		}
		mainGame->scrCardList->setVisible(true);
		mainGame->scrCardList->setMin(0);
		mainGame->scrCardList->setMax((selectable_cards.size() - 5) * 10 + 9);
		mainGame->scrCardList->setPos(0);
	}
	if(!chain_forced)
		mainGame->btnSelectOK->setVisible(true);
	else mainGame->btnSelectOK->setVisible(false);
	mainGame->PopupElement(mainGame->wCardSelect);
}
void ClientField::ReplaySwap() {
	std::swap(deck[0], deck[1]);
	std::swap(hand[0], hand[1]);
	std::swap(mzone[0], mzone[1]);
	std::swap(szone[0], szone[1]);
	std::swap(grave[0], grave[1]);
	std::swap(remove[0], remove[1]);
	std::swap(extra[0], extra[1]);
	for(int p = 0; p < 2; ++p) {
		for(auto cit = deck[p].begin(); cit != deck[p].end(); ++cit) {
			(*cit)->controler = 1 - (*cit)->controler;
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = hand[p].begin(); cit != hand[p].end(); ++cit) {
			(*cit)->controler = 1 - (*cit)->controler;
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = mzone[p].begin(); cit != mzone[p].end(); ++cit) {
			if(*cit) {
				(*cit)->controler = 1 - (*cit)->controler;
				GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
				(*cit)->mTransform.setTranslation((*cit)->curPos);
				(*cit)->mTransform.setRotationRadians((*cit)->curRot);
				(*cit)->is_moving = false;
			}
		}
		for(auto cit = szone[p].begin(); cit != szone[p].end(); ++cit) {
			if(*cit) {
				(*cit)->controler = 1 - (*cit)->controler;
				GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
				(*cit)->mTransform.setTranslation((*cit)->curPos);
				(*cit)->mTransform.setRotationRadians((*cit)->curRot);
				(*cit)->is_moving = false;
			}
		}
		for(auto cit = grave[p].begin(); cit != grave[p].end(); ++cit) {
			(*cit)->controler = 1 - (*cit)->controler;
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = remove[p].begin(); cit != remove[p].end(); ++cit) {
			(*cit)->controler = 1 - (*cit)->controler;
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = extra[p].begin(); cit != extra[p].end(); ++cit) {
			(*cit)->controler = 1 - (*cit)->controler;
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
	}
	for(auto cit = overlay_cards.begin(); cit != overlay_cards.end(); ++cit) {
		(*cit)->controler = 1 - (*cit)->controler;
		GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
		(*cit)->mTransform.setTranslation((*cit)->curPos);
		(*cit)->mTransform.setRotationRadians((*cit)->curRot);
		(*cit)->is_moving = false;
	}
	mainGame->dInfo.isFirst = !mainGame->dInfo.isFirst;
	std::swap(mainGame->dInfo.lp[0], mainGame->dInfo.lp[1]);
	for(int i = 0; i < 16; ++i)
		std::swap(mainGame->dInfo.strLP[0][i], mainGame->dInfo.strLP[1][i]);
	for(int i = 0; i < 20; ++i)
		std::swap(mainGame->dInfo.hostname[i], mainGame->dInfo.clientname[i]);
	for(auto chit = chains.begin(); chit != chains.end(); ++chit) {
		chit->controler = 1 - chit->controler;
		GetChainLocation(chit->controler, chit->location, chit->sequence, &chit->chain_pos);
	}
	disabled_field = (disabled_field >> 16) | (disabled_field << 16);
}
void ClientField::RefreshAllCards() {
	for(int p = 0; p < 2; ++p) {
		for(auto cit = deck[p].begin(); cit != deck[p].end(); ++cit) {
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = hand[p].begin(); cit != hand[p].end(); ++cit) {
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = mzone[p].begin(); cit != mzone[p].end(); ++cit) {
			if(*cit) {
				GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
				(*cit)->mTransform.setTranslation((*cit)->curPos);
				(*cit)->mTransform.setRotationRadians((*cit)->curRot);
				(*cit)->is_moving = false;
			}
		}
		for(auto cit = szone[p].begin(); cit != szone[p].end(); ++cit) {
			if(*cit) {
				GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
				(*cit)->mTransform.setTranslation((*cit)->curPos);
				(*cit)->mTransform.setRotationRadians((*cit)->curRot);
				(*cit)->is_moving = false;
			}
		}
		for(auto cit = grave[p].begin(); cit != grave[p].end(); ++cit) {
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = remove[p].begin(); cit != remove[p].end(); ++cit) {
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
		for(auto cit = extra[p].begin(); cit != extra[p].end(); ++cit) {
			GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
			(*cit)->mTransform.setTranslation((*cit)->curPos);
			(*cit)->mTransform.setRotationRadians((*cit)->curRot);
			(*cit)->is_moving = false;
		}
	}
	for(auto cit = overlay_cards.begin(); cit != overlay_cards.end(); ++cit) {
		GetCardLocation(*cit, &(*cit)->curPos, &(*cit)->curRot);
		(*cit)->mTransform.setTranslation((*cit)->curPos);
		(*cit)->mTransform.setRotationRadians((*cit)->curRot);
		(*cit)->is_moving = false;
	}
}
void ClientField::GetChainLocation(int controler, int location, int sequence, irr::core::vector3df* t) {
	t->X = 0;
	t->Y = 0;
	t->Z = 0;
	switch((location & 0x7f)) {
	case LOCATION_DECK: {
		if (controler == 0) {
			t->X = (matManager.vFields[0].Pos.X + matManager.vFields[1].Pos.X) / 2;
			t->Y = (matManager.vFields[0].Pos.Y + matManager.vFields[2].Pos.Y) / 2;
			t->Z = deck[controler].size() * 0.01f + 0.03f;
		} else {
			t->X = (matManager.vFields[68].Pos.X + matManager.vFields[69].Pos.X) / 2;
			t->Y = (matManager.vFields[68].Pos.Y + matManager.vFields[70].Pos.Y) / 2;
			t->Z = deck[controler].size() * 0.01f + 0.03f;
		}
		break;
	}
	case LOCATION_HAND: {
		if (controler == 0) {
			t->X = 2.95f;
			t->Y = 3.15f;
			t->Z = 0.03f;
		} else {
			t->X = 2.95f;
			t->Y = -3.15f;
			t->Z = 0.03f;
		}
		break;
	}
	case LOCATION_MZONE: {
		if (controler == 0) {
			t->X = (matManager.vFields[16].Pos.X + matManager.vFields[17].Pos.X) / 2 + 1.1f * sequence;
			t->Y = (matManager.vFields[16].Pos.Y + matManager.vFields[18].Pos.Y) / 2;
			t->Z = 0.03f;
		} else {
			t->X = (matManager.vFields[84].Pos.X + matManager.vFields[85].Pos.X) / 2 - 1.1f * sequence;
			t->Y = (matManager.vFields[84].Pos.Y + matManager.vFields[86].Pos.Y) / 2;
			t->Z = 0.03f;
		}
		break;
	}
	case LOCATION_SZONE: {
		if (controler == 0) {
			if (sequence <= 4) {
				t->X = (matManager.vFields[36].Pos.X + matManager.vFields[37].Pos.X) / 2 + 1.1f * sequence;
				t->Y = (matManager.vFields[36].Pos.Y + matManager.vFields[38].Pos.Y) / 2;
				t->Z = 0.03f;
			} else if (sequence == 5) {
				t->X = (matManager.vFields[56].Pos.X + matManager.vFields[57].Pos.X) / 2;
				t->Y = (matManager.vFields[56].Pos.Y + matManager.vFields[58].Pos.Y) / 2;
				t->Z = 0.03f;
			} else if (sequence == 6) {
				t->X = (matManager.vFields[60].Pos.X + matManager.vFields[61].Pos.X) / 2;
				t->Y = (matManager.vFields[60].Pos.Y + matManager.vFields[62].Pos.Y) / 2;
				t->Z = 0.03f;
			} else {
				t->X = (matManager.vFields[64].Pos.X + matManager.vFields[65].Pos.X) / 2;
				t->Y = (matManager.vFields[64].Pos.Y + matManager.vFields[66].Pos.Y) / 2;
				t->Z = 0.03f;
			}
		} else {
			if (sequence <= 4) {
				t->X = (matManager.vFields[104].Pos.X + matManager.vFields[105].Pos.X) / 2 - 1.1f * sequence;
				t->Y = (matManager.vFields[104].Pos.Y + matManager.vFields[106].Pos.Y) / 2;
				t->Z = 0.03f;
			} else if (sequence == 5) {
				t->X = (matManager.vFields[124].Pos.X + matManager.vFields[125].Pos.X) / 2;
				t->Y = (matManager.vFields[124].Pos.Y + matManager.vFields[126].Pos.Y) / 2;
				t->Z = 0.03f;
			} else if (sequence == 6) {
				t->X = (matManager.vFields[128].Pos.X + matManager.vFields[129].Pos.X) / 2;
				t->Y = (matManager.vFields[128].Pos.Y + matManager.vFields[130].Pos.Y) / 2;
				t->Z = 0.03f;
			} else {
				t->X = (matManager.vFields[132].Pos.X + matManager.vFields[133].Pos.X) / 2;
				t->Y = (matManager.vFields[132].Pos.Y + matManager.vFields[134].Pos.Y) / 2;
				t->Z = 0.03f;
			}
		}
		break;
	}
	case LOCATION_GRAVE: {
		if (controler == 0) {
			t->X = (matManager.vFields[4].Pos.X + matManager.vFields[5].Pos.X) / 2;
			t->Y = (matManager.vFields[4].Pos.Y + matManager.vFields[6].Pos.Y) / 2;
			t->Z = grave[controler].size() * 0.01f + 0.03f;
		} else {
			t->X = (matManager.vFields[72].Pos.X + matManager.vFields[73].Pos.X) / 2;
			t->Y = (matManager.vFields[72].Pos.Y + matManager.vFields[74].Pos.Y) / 2;
			t->Z = grave[controler].size() * 0.01f + 0.03f;
		}
		break;
	}
	case LOCATION_REMOVED: {
		if (controler == 0) {
			t->X = (matManager.vFields[12].Pos.X + matManager.vFields[13].Pos.X) / 2;
			t->Y = (matManager.vFields[12].Pos.Y + matManager.vFields[14].Pos.Y) / 2;
			t->Z = remove[controler].size() * 0.01f + 0.03f;
		} else {
			t->X = (matManager.vFields[80].Pos.X + matManager.vFields[81].Pos.X) / 2;
			t->Y = (matManager.vFields[80].Pos.Y + matManager.vFields[82].Pos.Y) / 2;
			t->Z = remove[controler].size() * 0.01f + 0.03f;
		}
		break;
	}
	case LOCATION_EXTRA: {
		if (controler == 0) {
			t->X = (matManager.vFields[8].Pos.X + matManager.vFields[9].Pos.X) / 2;
			t->Y = (matManager.vFields[8].Pos.Y + matManager.vFields[10].Pos.Y) / 2;
			t->Z = extra[controler].size() * 0.01f + 0.03f;
		} else {
			t->X = (matManager.vFields[76].Pos.X + matManager.vFields[77].Pos.X) / 2;
			t->Y = (matManager.vFields[76].Pos.Y + matManager.vFields[78].Pos.Y) / 2;
			t->Z = extra[controler].size() * 0.01f + 0.03f;
		}
		break;
	}
	}
}
void ClientField::GetCardLocation(ClientCard* pcard, irr::core::vector3df* t, irr::core::vector3df* r, bool setTrans) {
	int controler = pcard->controler;
	int sequence = pcard->sequence;
	int location = pcard->location;
	switch (location) {
	case LOCATION_DECK: {
		if (controler == 0) {
			t->X = (matManager.vFields[0].Pos.X + matManager.vFields[1].Pos.X) / 2;
			t->Y = (matManager.vFields[0].Pos.Y + matManager.vFields[2].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			if(deck_reversed == pcard->is_reversed) {
				r->X = 0.0f;
				r->Y = 3.1415926f;
				r->Z = 0.0f;
			} else {
				r->X = 0.0f;
				r->Y = 0.0f;
				r->Z = 0.0f;
			}
		} else {
			t->X = (matManager.vFields[68].Pos.X + matManager.vFields[69].Pos.X) / 2;
			t->Y = (matManager.vFields[68].Pos.Y + matManager.vFields[70].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			if(deck_reversed == pcard->is_reversed) {
				r->X = 0.0f;
				r->Y = 3.1415926f;
				r->Z = 3.1415926f;
			} else {
				r->X = 0.0f;
				r->Y = 0.0f;
				r->Z = 3.1415926f;
			}
		}
		break;
	}
	case 0:
	case LOCATION_HAND: {
		int count = hand[controler].size();
		if (controler == 0) {
			if (count <= 6)
				t->X = (5.5f - 0.8f * count) / 2 + 1.55f + sequence * 0.8f;
			else
				t->X = 1.9f + sequence * 4.0f / (count - 1);
			if (pcard->is_hovered) {
				t->Y = 3.84f;
				t->Z = 0.656f + 0.001f * sequence;
			} else {
				t->Y = 4.0f;
				t->Z = 0.5f + 0.001f * sequence;
			}
			if(pcard->code) {
				r->X = -0.798056f;
				r->Y = 0.0f;
				r->Z = 0.0f;
			} else {
				r->X = 0.798056f;
				r->Y = 3.1415926f;
				r->Z = 0;
			}
		} else {
			if (count <= 6)
				t->X = 6.25f - (5.5f - 0.8f * count) / 2 - sequence * 0.8f;
			else
				t->X = 5.9f - sequence * 4.0f / (count - 1);
			if (pcard->is_hovered) {
				t->Y = -3.56f;
				t->Z = 0.656f - 0.001f * sequence;
			} else {
				t->Y = -3.4f;
				t->Z = 0.5f - 0.001f * sequence;
			}
			if (pcard->code == 0) {
				r->X = 0.798056f;
				r->Y = 3.1415926f;
				r->Z = 0;
			} else {
				r->X = -0.798056f;
				r->Y = 0;
				r->Z = 0;
			}
		}
		break;
	}
	case LOCATION_MZONE: {
		if (controler == 0) {
			t->X = (matManager.vFields[16].Pos.X + matManager.vFields[17].Pos.X) / 2 + 1.1f * sequence;
			t->Y = (matManager.vFields[16].Pos.Y + matManager.vFields[18].Pos.Y) / 2;
			t->Z = 0.01f;
			if (pcard->position & POS_DEFENCE) {
				r->X = 0.0f;
				r->Z = -3.1415926f / 2.0f;
				if (pcard->position & POS_FACEDOWN)
					r->Y = 3.1415926f + 0.001f;
				else r->Y = 0.0f;
			} else {
				r->X = 0.0f;
				r->Z = 0.0f;
				if (pcard->position & POS_FACEDOWN)
					r->Y = 3.1415926f;
				else r->Y = 0.0f;
			}
		} else {
			t->X = (matManager.vFields[84].Pos.X + matManager.vFields[85].Pos.X) / 2 - 1.1f * sequence;
			t->Y = (matManager.vFields[84].Pos.Y + matManager.vFields[86].Pos.Y) / 2;
			t->Z = 0.01f;
			if (pcard->position & POS_DEFENCE) {
				r->X = 0.0f;
				r->Z = 3.1415926f / 2.0f;
				if (pcard->position & POS_FACEDOWN)
					r->Y = 3.1415926f + 0.001f;
				else r->Y = 0.0f;
			} else {
				r->X = 0.0f;
				r->Z = 3.1415926f;
				if (pcard->position & POS_FACEDOWN)
					r->Y = 3.1415926f;
				else r->Y = 0.0f;
			}
		}
		break;
	}
	case LOCATION_SZONE: {
		if (controler == 0) {
			if (sequence <= 4) {
				t->X = (matManager.vFields[36].Pos.X + matManager.vFields[37].Pos.X) / 2 + 1.1f * sequence;
				t->Y = (matManager.vFields[36].Pos.Y + matManager.vFields[38].Pos.Y) / 2;
				t->Z = 0.01f;
			} else if (sequence == 5) {
				t->X = (matManager.vFields[56].Pos.X + matManager.vFields[57].Pos.X) / 2;
				t->Y = (matManager.vFields[56].Pos.Y + matManager.vFields[58].Pos.Y) / 2;
				t->Z = 0.01f;
			} else if (sequence == 6) {
				t->X = (matManager.vFields[60].Pos.X + matManager.vFields[61].Pos.X) / 2;
				t->Y = (matManager.vFields[60].Pos.Y + matManager.vFields[62].Pos.Y) / 2;
				t->Z = 0.01f;
			} else {
				t->X = (matManager.vFields[64].Pos.X + matManager.vFields[65].Pos.X) / 2;
				t->Y = (matManager.vFields[64].Pos.Y + matManager.vFields[66].Pos.Y) / 2;
				t->Z = 0.01f;
			}
			r->X = 0.0f;
			r->Z = 0.0f;
			if (pcard->position & POS_FACEDOWN)
				r->Y = 3.1415926f;
			else r->Y = 0.0f;
		} else {
			if (sequence <= 4) {
				t->X = (matManager.vFields[104].Pos.X + matManager.vFields[105].Pos.X) / 2 - 1.1f * sequence;
				t->Y = (matManager.vFields[104].Pos.Y + matManager.vFields[106].Pos.Y) / 2;
				t->Z = 0.01f;
			} else if (sequence == 5) {
				t->X = (matManager.vFields[124].Pos.X + matManager.vFields[125].Pos.X) / 2;
				t->Y = (matManager.vFields[124].Pos.Y + matManager.vFields[126].Pos.Y) / 2;
				t->Z = 0.01f;
			} else if (sequence == 6) {
				t->X = (matManager.vFields[128].Pos.X + matManager.vFields[129].Pos.X) / 2;
				t->Y = (matManager.vFields[128].Pos.Y + matManager.vFields[130].Pos.Y) / 2;
				t->Z = 0.01f;
			} else {
				t->X = (matManager.vFields[132].Pos.X + matManager.vFields[133].Pos.X) / 2;
				t->Y = (matManager.vFields[132].Pos.Y + matManager.vFields[134].Pos.Y) / 2;
				t->Z = 0.01f;
			}
			r->X = 0.0f;
			r->Z = 3.1415926f;
			if (pcard->position & POS_FACEDOWN)
				r->Y = 3.1415926f;
			else r->Y = 0.0f;
		}
		break;
	}
	case LOCATION_GRAVE: {
		if (controler == 0) {
			t->X = (matManager.vFields[4].Pos.X + matManager.vFields[5].Pos.X) / 2;
			t->Y = (matManager.vFields[4].Pos.Y + matManager.vFields[6].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			r->X = 0.0f;
			r->Y = 0.0f;
			r->Z = 0.0f;
		} else {
			t->X = (matManager.vFields[72].Pos.X + matManager.vFields[73].Pos.X) / 2;
			t->Y = (matManager.vFields[72].Pos.Y + matManager.vFields[74].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			r->X = 0.0f;
			r->Y = 0.0f;
			r->Z = 3.1415926f;
		}
		break;
	}
	case LOCATION_REMOVED: {
		if (controler == 0) {
			t->X = (matManager.vFields[12].Pos.X + matManager.vFields[13].Pos.X) / 2;
			t->Y = (matManager.vFields[12].Pos.Y + matManager.vFields[14].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			if(pcard->position & POS_FACEUP) {
				r->X = 0.0f;
				r->Y = 0.0f;
				r->Z = 0.0f;
			} else {
				r->X = 0.0f;
				r->Y = 3.1415926f;
				r->Z = 0.0f;
			}
		} else {
			t->X = (matManager.vFields[80].Pos.X + matManager.vFields[81].Pos.X) / 2;
			t->Y = (matManager.vFields[80].Pos.Y + matManager.vFields[82].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			if(pcard->position & POS_FACEUP) {
				r->X = 0.0f;
				r->Y = 0.0f;
				r->Z = 3.1415926f;
			} else {
				r->X = 0.0f;
				r->Y = 3.1415926f;
				r->Z = 3.1415926f;
			}
		}
		break;
	}
	case LOCATION_EXTRA: {
		if (controler == 0) {
			t->X = (matManager.vFields[8].Pos.X + matManager.vFields[9].Pos.X) / 2;
			t->Y = (matManager.vFields[8].Pos.Y + matManager.vFields[10].Pos.Y) / 2;
			t->Z = 0.01f + 0.01f * sequence;
			r->X = 0.0f;
			if(pcard->position & POS_FACEUP)
				r->Y = 0.0f;
			else r->Y = 3.1415926f;
			r->Z = 0.0f;
		} else {
			t->X = (matManager.vFields[76].Pos.X + matManager.vFields[77].Pos.X) / 2;
			t->Y = (matManager.vFields[76].Pos.Y + matManager.vFields[78].Pos.Y) / 2;
			t->Z = 0.01f * sequence;
			r->X = 0.0f;
			if(pcard->position & POS_FACEUP)
				r->Y = 0.0f;
			else r->Y = 3.1415926f;
			r->Z = 3.1415926f;
		}
		break;
	}
	case LOCATION_OVERLAY: {
		if (pcard->overlayTarget->location != 0x4) {
			return;
		}
		int oseq = pcard->overlayTarget->sequence;
		if (pcard->overlayTarget->controler == 0) {
			t->X = (matManager.vFields[16].Pos.X + matManager.vFields[17].Pos.X) / 2 + 1.1f * oseq - 0.12f + 0.06f * sequence;
			t->Y = (matManager.vFields[16].Pos.Y + matManager.vFields[18].Pos.Y) / 2 + 0.05f;
			t->Z = 0.005f + pcard->sequence * 0.0001f;
			r->X = 0.0f;
			r->Y = 0.0f;
			r->Z = 0.0f;
		} else {
			t->X = (matManager.vFields[84].Pos.X + matManager.vFields[85].Pos.X) / 2 - 1.1f * oseq + 0.12f - 0.06f * sequence;
			t->Y = (matManager.vFields[84].Pos.Y + matManager.vFields[86].Pos.Y) / 2 - 0.05f;
			t->Z = 0.005f + pcard->sequence * 0.0001f;
			r->X = 0.0f;
			r->Y = 0.0f;
			r->Z = 3.1415926f;
		}
		break;
	}
	}
	if(setTrans) {
		pcard->mTransform.setTranslation(*t);
		pcard->mTransform.setRotationRadians(*r);
	}
}
void ClientField::MoveCard(ClientCard * pcard, int frame) {
	irr::core::vector3df trans = pcard->curPos;
	irr::core::vector3df rot = pcard->curRot;
	GetCardLocation(pcard, &trans, &rot);
	pcard->dPos = (trans - pcard->curPos) / frame;
	float diff = rot.X - pcard->curRot.X;
	while (diff < 0) diff += 3.1415926f * 2;
	while (diff > 3.1415926f * 2)
		diff -= 3.1415926f * 2;
	if (diff < 3.1415926f)
		pcard->dRot.X = diff / frame;
	else
		pcard->dRot.X = -(3.1415926f * 2 - diff) / frame;
	diff = rot.Y - pcard->curRot.Y;
	while (diff < 0) diff += 3.1415926f * 2;
	while (diff > 3.1415926f * 2) diff -= 3.1415926f * 2;
	if (diff < 3.1415926f)
		pcard->dRot.Y = diff / frame;
	else
		pcard->dRot.Y = -(3.1415926f * 2 - diff) / frame;
	diff = rot.Z - pcard->curRot.Z;
	while (diff < 0) diff += 3.1415926f * 2;
	while (diff > 3.1415926f * 2) diff -= 3.1415926f * 2;
	if (diff < 3.1415926f)
		pcard->dRot.Z = diff / frame;
	else
		pcard->dRot.Z = -(3.1415926f * 2 - diff) / frame;
	pcard->is_moving = true;
	pcard->aniFrame = frame;
}
void ClientField::FadeCard(ClientCard * pcard, int alpha, int frame) {
	pcard->dAlpha = (alpha - pcard->curAlpha) / frame;
	pcard->is_fading = true;
	pcard->aniFrame = frame;
}
bool ClientField::CheckSelectSum() {
	std::set<ClientCard*> selable;
	std::set<ClientCard*>::iterator sit;
	for(size_t i = 0; i < selectsum_all.size(); ++i) {
		selectsum_all[i]->is_selectable = false;
		selectsum_all[i]->is_selected = false;
		selable.insert(selectsum_all[i]);
	}
	for(size_t i = 0; i < selected_cards.size(); ++i) {
		selected_cards[i]->is_selectable = true;
		selected_cards[i]->is_selected = true;
		selable.erase(selected_cards[i]);
	}
	selectsum_cards.clear();
	bool ret;
	if (select_mode == 0) {
		ret = check_sel_sum_s(selable, 0, select_sumval);
		selectable_cards.clear();
		for(sit = selectsum_cards.begin(); sit != selectsum_cards.end(); ++sit) {
			(*sit)->is_selectable = true;
			selectable_cards.push_back(*sit);
		}
		return ret;
	} else {
		int op1, op2, mm = -1, ms, m, max = 0, sumc = 0, sums;
		ret = false;
		for (size_t i = 0; i < selected_cards.size(); ++i) {
			op1 = selected_cards[i]->opParam & 0xffff;
			op2 = selected_cards[i]->opParam >> 16;
			m = (op2 > 0 && op1 > op2) ? op2 : op1;
			max += op2 > op1 ? op2 : op1;
			if (mm == -1 || m < mm)
				mm = m;
			sumc += m;
		}
		if (select_sumval <= sumc)
			return true;
		if (select_sumval <= max)
			ret = true;
		for(sit = selable.begin(); sit != selable.end(); ++sit) {
			op1 = (*sit)->opParam & 0xffff;
			op2 = (*sit)->opParam >> 16;
			m = op1;
			sums = sumc;
			sums += m;
			ms = mm;
			if (ms == -1 || m < ms)
				ms = m;
			if (sums >= select_sumval) {
				if (sums - ms < select_sumval)
					selectsum_cards.insert(*sit);
			} else {
				std::set<ClientCard*> left(selable);
				left.erase(*sit);
				if (check_min(left, left.begin(), select_sumval - sums, select_sumval - sums + ms - 1))
					selectsum_cards.insert(*sit);
			}
			if (op2 == 0)
				continue;
			m = op2;
			sums = sumc;
			sums += m;
			ms = mm;
			if (ms == -1 || m < ms)
				ms = m;
			if (sums >= select_sumval) {
				if (sums - ms < select_sumval)
					selectsum_cards.insert(*sit);
			} else {
				std::set<ClientCard*> left(selable);
				left.erase(*sit);
				if (check_min(left, left.begin(), select_sumval - sums, select_sumval - sums + ms - 1))
					selectsum_cards.insert(*sit);
			}
		}
		selectable_cards.clear();
		for(sit = selectsum_cards.begin(); sit != selectsum_cards.end(); ++sit) {
			(*sit)->is_selectable = true;
			selectable_cards.push_back(*sit);
		}
		return ret;
	}
}
bool ClientField::check_min(std::set<ClientCard*>& left, std::set<ClientCard*>::iterator index, int min, int max) {
	if (index == left.end())
		return false;
	int op1 = (*index)->opParam & 0xffff;
	int op2 = (*index)->opParam >> 16;
	int m = (op2 > 0 && op1 > op2) ? op2 : op1;
	if (m >= min && m <= max)
		return true;
	index++;
	return (min > m && check_min(left, index, min - m, max - m))
	        || check_min(left, index, min, max);
}
bool ClientField::check_sel_sum_s(std::set<ClientCard*>& left, int index, int acc) {
	if (index == (int)selected_cards.size()) {
		if (acc == 0)
			return true;
		check_sel_sum_t(left, acc);
		return false;
	}
	int l = selected_cards[index]->opParam;
	int l1 = l & 0xffff;
	int l2 = l >> 16;
	bool res1 = false, res2 = false;
	res1 = check_sel_sum_s(left, index + 1, acc - l1);
	if (l2 > 0)
		res2 = check_sel_sum_s(left, index + 1, acc - l2);
	return res1 || res2;
}
void ClientField::check_sel_sum_t(std::set<ClientCard*>& left, int acc) {
	std::set<ClientCard*>::iterator sit;
	for (sit = left.begin(); sit != left.end(); ++sit) {
		if (selectsum_cards.find(*sit) != selectsum_cards.end())
			continue;
		std::set<ClientCard*> testlist(left);
		testlist.erase(*sit);
		int l = (*sit)->opParam;
		int l1 = l & 0xffff;
		int l2 = l >> 16;
		if (check_sum(testlist, testlist.begin(), acc - l1, selected_cards.size() + 1)
		        || (l2 > 0 && check_sum(testlist, testlist.begin(), acc - l2, selected_cards.size() + 1))) {
			selectsum_cards.insert(*sit);
		}
	}
}
bool ClientField::check_sum(std::set<ClientCard*>& testlist, std::set<ClientCard*>::iterator index, int acc, int count) {
	if (acc == 0)
		return count >= select_min && count <= select_max;
	if (acc < 0 || index == testlist.end())
		return false;
	int l = (*index)->opParam;
	int l1 = l & 0xffff;
	int l2 = l >> 16;
	if ((l1 == acc || (l2 > 0 && l2 == acc)) && (count + 1 >= select_min) && (count + 1 <= select_max))
		return true;
	index++;
	return (acc > l1 && check_sum(testlist, index, acc - l1, count + 1))
	       || (l2 > 0 && acc > l2 && check_sum(testlist, index, acc - l2, count + 1))
	       || check_sum(testlist, index, acc, count);
}
}
