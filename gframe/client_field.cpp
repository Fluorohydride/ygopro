#include <stack>
#include "game_config.h"
#include <irrlicht.h>
#include "client_field.h"
#include "client_card.h"
#include "duelclient.h"
#include "data_manager.h"
#include "image_manager.h"
#include "game.h"
#include "materials.h"
#include "core_utils.h"
#include "CGUIImageButton/CGUIImageButton.h"
#include "CGUITTFont/CGUITTFont.h"
#include "custom_skin_enum.h"

namespace ygo {

ClientField::ClientField() {
	panel = 0;
	hovered_card = 0;
	clicked_card = 0;
	highlighting_card = 0;
	hovered_controler = 0;
	hovered_location = 0;
	hovered_sequence = 0;
	selectable_field = 0;
	selected_field = 0;
	deck_act = false;
	grave_act = false;
	remove_act = false;
	extra_act = false;
	pzone_act[0] = false;
	pzone_act[1] = false;
	conti_act = false;
	deck_reversed = false;
	conti_selecting = false;
	for(int p = 0; p < 2; ++p) {
		skills[p] = nullptr;
		mzone[p].resize(7, 0);
		szone[p].resize(8, 0);
	}
}
#define CLEAR_VECTOR(vec)for(auto& pcard : vec)\
							delete pcard;\
						vec.clear();\
						vec.shrink_to_fit();
void ClientField::Clear() {
	for(int i = 0; i < 2; ++i) {
		CLEAR_VECTOR(mzone[i]);
		CLEAR_VECTOR(szone[i]);
		mzone[i].resize(7, 0);
		szone[i].resize(8, 0);
		CLEAR_VECTOR(deck[i]);
		CLEAR_VECTOR(hand[i]);
		CLEAR_VECTOR(grave[i]);
		CLEAR_VECTOR(remove[i]);
		CLEAR_VECTOR(extra[i]);
	}
	CLEAR_VECTOR(limbo_temp);
	for(auto& pcard : overlay_cards)
		delete pcard;
	if(skills[0]) {
		delete skills[0];
		skills[0] = nullptr;
	}
	if(skills[1]) {
		delete skills[1];
		skills[1] = nullptr;
	}
	overlay_cards.clear();
	extra_p_count[0] = 0;
	extra_p_count[1] = 0;
	player_desc_hints[0].clear();
	player_desc_hints[1].clear();
	chains.clear();
	activatable_cards.clear();
	summonable_cards.clear();
	spsummonable_cards.clear();
	msetable_cards.clear();
	ssetable_cards.clear();
	reposable_cards.clear();
	attackable_cards.clear();
	disabled_field = 0;
	panel = 0;
	hovered_card = 0;
	clicked_card = 0;
	highlighting_card = 0;
	hovered_controler = 0;
	hovered_location = 0;
	hovered_sequence = 0;
	selectable_field = 0;
	selected_field = 0;
	deck_act = false;
	grave_act = false;
	remove_act = false;
	extra_act = false;
	pzone_act[0] = false;
	pzone_act[1] = false;
	conti_act = false;
	deck_reversed = false;
}
#undef CLEAR_VECTOR
void ClientField::Initial(int player, int deckc, int extrac) {
	ClientCard* pcard;
	for(int i = 0; i < deckc; ++i) {
		pcard = new ClientCard;
		deck[player].push_back(pcard);
		pcard->owner = player;
		pcard->controler = player;
		pcard->location = LOCATION_DECK;
		pcard->sequence = i;
		pcard->position = POS_FACEDOWN_DEFENSE;
		pcard->UpdateDrawCoordinates(true);
	}
	for(int i = 0; i < extrac; ++i) {
		pcard = new ClientCard;
		extra[player].push_back(pcard);
		pcard->owner = player;
		pcard->controler = player;
		pcard->location = LOCATION_EXTRA;
		pcard->sequence = i;
		pcard->position = POS_FACEDOWN_DEFENSE;
		pcard->UpdateDrawCoordinates(true);
	}
}
std::vector<ClientCard*>* ClientField::GetList(int location, int controler) {
	switch(location) {
	case LOCATION_DECK:
		return &deck[controler];
		break;
	case LOCATION_HAND:
		return &hand[controler];
		break;
	case LOCATION_MZONE:
		return &mzone[controler];
		break;
	case LOCATION_SZONE:
		return &szone[controler];
		break;
	case LOCATION_GRAVE:
		return &grave[controler];
		break;
	case LOCATION_REMOVED:
		return &remove[controler];
		break;
	case LOCATION_EXTRA:
		return &extra[controler];
		break;
	}
	return nullptr;
}
ClientCard* ClientField::GetCard(int controler, int location, int sequence, int sub_seq) {
	bool is_xyz = (location & LOCATION_OVERLAY) != 0;
	auto lst = GetList(location & (~LOCATION_OVERLAY), controler);
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
				deck[controler][i]->curPos += irr::core::vector3df(0, 0, 0.01f);
				deck[controler][i]->mTransform.setTranslation(deck[controler][i]->curPos);
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
		if(extra_p_count[controler] == 0 || (pcard->position & POS_FACEUP)) {
			extra[controler].push_back(pcard);
			pcard->sequence = extra[controler].size() - 1;
		} else {
			extra[controler].push_back(0);
			int p = extra[controler].size() - extra_p_count[controler] - 1;
			for(int i = extra[controler].size() - 1; i > p; --i) {
				extra[controler][i] = extra[controler][i - 1];
				extra[controler][i]->sequence++;
				extra[controler][i]->curPos += irr::core::vector3df(0, 0, 0.01f);
				extra[controler][i]->mTransform.setTranslation(extra[controler][i]->curPos);
			}
			extra[controler][p] = pcard;
			pcard->sequence = p;
		}
		if (pcard->position & POS_FACEUP)
			extra_p_count[controler]++;
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
		if (pcard->position & POS_FACEUP)
			extra_p_count[controler]--;
		break;
	}
	}
	pcard->location = 0;
	return pcard;
}
void ClientField::UpdateCard(int controler, int location, int sequence, char* data, int len) {
	ClientCard* pcard = GetCard(controler, location, sequence);
	if(pcard) {
		if(mainGame->dInfo.compat_mode) {
			len = BufferIO::Read<int32>(data);
		}
		CoreUtils::Query query(data, mainGame->dInfo.compat_mode, len);
		pcard->UpdateInfo(query);
	}
}
void ClientField::UpdateFieldCard(int controler, int location, char* data, int len) {
	auto lst = GetList(location, controler);
	if(!lst)
		return;
	CoreUtils::QueryStream queries(data, mainGame->dInfo.compat_mode, len);
	auto cit = lst->begin();
	for(auto& query : queries.queries) {
		auto pcard = *cit++;
		if(pcard)
			pcard->UpdateInfo(query);
	}
}
void ClientField::ClearCommandFlag() {
	for(auto& pcard : activatable_cards)  pcard->cmdFlag = 0;
	for(auto& pcard : summonable_cards)   pcard->cmdFlag = 0;
	for(auto& pcard : spsummonable_cards) pcard->cmdFlag = 0;
	for(auto& pcard : msetable_cards)     pcard->cmdFlag = 0;
	for(auto& pcard : ssetable_cards)     pcard->cmdFlag = 0;
	for(auto& pcard : reposable_cards)    pcard->cmdFlag = 0;
	for(auto& pcard : attackable_cards)   pcard->cmdFlag = 0;
	conti_cards.clear();
	deck_act = false;
	extra_act = false;
	grave_act = false;
	remove_act = false;
	pzone_act[0] = false;
	pzone_act[1] = false;
	conti_act = false;
}
void ClientField::ClearSelect() {
	for(auto& pcard : selectable_cards) {
		pcard->is_selectable = false;
		pcard->is_selected = false;
	}
}
void ClientField::ClearChainSelect() {
	for(auto& pcard : activatable_cards) {
		pcard->cmdFlag = 0;
		pcard->chain_code = 0;
		pcard->is_selectable = false;
		pcard->is_selected = false;
	}
	conti_cards.clear();
	deck_act = false;
	grave_act = false;
	remove_act = false;
	extra_act = false;
	conti_act = false;
}
// needs to be synchronized with EGET_SCROLL_BAR_CHANGED
void ClientField::ShowSelectCard(bool buttonok, bool chain) {
	int startpos;
	size_t ct;
	if(selectable_cards.size() <= 5) {
		startpos = 30 + 125 * (5 - selectable_cards.size()) / 2;
		ct = selectable_cards.size();
	} else {
		startpos = 30;
		ct = 5;
	}
	for(size_t i = 0; i < ct; ++i) {
		mainGame->stCardPos[i]->enableOverrideColor(false);
		// image
		if(selectable_cards[i]->code)
			mainGame->imageLoading[mainGame->btnCardSelect[i]] = selectable_cards[i]->code;
		else if(conti_selecting)
			mainGame->imageLoading[mainGame->btnCardSelect[i]] = selectable_cards[i]->chain_code;
		else
			mainGame->btnCardSelect[i]->setImage(mainGame->imageManager.tCover[selectable_cards[i]->controler]);
		mainGame->btnCardSelect[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 55, startpos + 120 + i * 125, 225));
		mainGame->btnCardSelect[i]->setPressed(false);
		mainGame->btnCardSelect[i]->setVisible(true);
		if(mainGame->dInfo.curMsg != MSG_SORT_CHAIN && mainGame->dInfo.curMsg != MSG_SORT_CARD) {
			// text
			std::wstring text = L"";
			if(conti_selecting)
				text = DataManager::unknown_string;
			else if(selectable_cards[i]->location == LOCATION_OVERLAY) {
				text = fmt::format(L"{}[{}]({})", gDataManager->FormatLocation(selectable_cards[i]->overlayTarget->location, selectable_cards[i]->overlayTarget->sequence),
					selectable_cards[i]->overlayTarget->sequence + 1, selectable_cards[i]->sequence + 1);
			} else if(selectable_cards[i]->location) {
				text = fmt::format(L"{}[{}]", gDataManager->FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
					selectable_cards[i]->sequence + 1);
			}
			mainGame->stCardPos[i]->setText(text.c_str());
			// color
			if (selectable_cards[i]->is_selected)
				mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELECTED_WINDOW_BACKGROUND_VAL);
			else {
				if(conti_selecting)
					mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
				else if(selectable_cards[i]->location == LOCATION_OVERLAY) {
					if(selectable_cards[i]->owner != selectable_cards[i]->overlayTarget->controler)
						mainGame->stCardPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_OVERLAY_TEXT_VAL);
					if(selectable_cards[i]->overlayTarget->controler)
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
					else
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
				} else if(selectable_cards[i]->location == LOCATION_DECK || selectable_cards[i]->location == LOCATION_EXTRA || selectable_cards[i]->location == LOCATION_REMOVED) {
					if(selectable_cards[i]->position & POS_FACEDOWN)
						mainGame->stCardPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_SET_TEXT_VAL);
					if(selectable_cards[i]->controler)
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
					else
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
				} else {
					if(selectable_cards[i]->controler)
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
					else
						mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
				}
			}
		} else {
			if(sort_list[i]) {
				mainGame->stCardPos[i]->setText(fmt::to_wstring(sort_list[i]).c_str());
			} else
				mainGame->stCardPos[i]->setText(L"");
			mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		}
		mainGame->stCardPos[i]->setVisible(true);
		mainGame->stCardPos[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 30, startpos + 120 + i * 125, 50));
	}
	if(selectable_cards.size() <= 5) {
		for(int i = selectable_cards.size(); i < 5; ++i) {
			mainGame->btnCardSelect[i]->setVisible(false);
			mainGame->stCardPos[i]->setVisible(false);
		}
		mainGame->scrCardList->setPos(0);
		mainGame->scrCardList->setVisible(false);
	} else {
		mainGame->scrCardList->setVisible(true);
		mainGame->scrCardList->setMin(0);
		mainGame->scrCardList->setMax((selectable_cards.size() - 5) * 10 + 9);
		mainGame->scrCardList->setPos(0);
	}
	mainGame->btnSelectOK->setVisible(buttonok);
	mainGame->PopupElement(mainGame->wCardSelect);
}
void ClientField::ShowChainCard() {
	int startpos;
	size_t ct;
	if(selectable_cards.size() <= 5) {
		startpos = 30 + 125 * (5 - selectable_cards.size()) / 2;
		ct = selectable_cards.size();
	} else {
		startpos = 30;
		ct = 5;
	}
	for(size_t i = 0; i < ct; ++i) {
		if(selectable_cards[i]->code)
			mainGame->imageLoading[mainGame->btnCardSelect[i]] = selectable_cards[i]->code;
		else
			mainGame->btnCardSelect[i]->setImage(mainGame->imageManager.tCover[selectable_cards[i]->controler]);
		mainGame->btnCardSelect[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 55, startpos + 120 + i * 125, 225));
		mainGame->btnCardSelect[i]->setPressed(false);
		mainGame->btnCardSelect[i]->setVisible(true);
		mainGame->stCardPos[i]->setText(fmt::format(L"{}[{}]", gDataManager->FormatLocation(selectable_cards[i]->location, selectable_cards[i]->sequence),
			selectable_cards[i]->sequence + 1).c_str());
		if(selectable_cards[i]->location == LOCATION_OVERLAY) {
			if(selectable_cards[i]->owner != selectable_cards[i]->overlayTarget->controler)
				mainGame->stCardPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_OVERLAY_TEXT_VAL);
			if(selectable_cards[i]->overlayTarget->controler)
				mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
			else
				mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		} else {
			if(selectable_cards[i]->controler)
				mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
			else
				mainGame->stCardPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		}
		mainGame->stCardPos[i]->setVisible(true);
		mainGame->stCardPos[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 30, startpos + 120 + i * 125, 50));
	} 
	if(selectable_cards.size() <= 5) {
		for(int i = selectable_cards.size(); i < 5; ++i) {
			mainGame->btnCardSelect[i]->setVisible(false);
			mainGame->stCardPos[i]->setVisible(false);
		}
		mainGame->scrCardList->setPos(0);
		mainGame->scrCardList->setVisible(false);
	} else {
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
void ClientField::ShowLocationCard() {
	int startpos;
	size_t ct;
	if(display_cards.size() <= 5) {
		startpos = 30 + 125 * (5 - display_cards.size()) / 2;
		ct = display_cards.size();
	} else {
		startpos = 30;
		ct = 5;
	}
	for(size_t i = 0; i < ct; ++i) {
		mainGame->stDisplayPos[i]->enableOverrideColor(false);
		if(display_cards[i]->code)
			mainGame->imageLoading[mainGame->btnCardDisplay[i]] = display_cards[i]->code;
		else
			mainGame->btnCardDisplay[i]->setImage(mainGame->imageManager.tCover[display_cards[i]->controler]);
		mainGame->btnCardDisplay[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 55, startpos + 120 + i * 125, 225));
		mainGame->btnCardDisplay[i]->setPressed(false);
		mainGame->btnCardDisplay[i]->setVisible(true);
		std::wstring text;
		if(display_cards[i]->location == LOCATION_OVERLAY) {
			text = fmt::format(L"{}[{}]({})", gDataManager->FormatLocation(display_cards[i]->overlayTarget->location, display_cards[i]->overlayTarget->sequence),
				display_cards[i]->overlayTarget->sequence + 1, display_cards[i]->sequence + 1);
		} else if(display_cards[i]->location) {
			text = fmt::format(L"{}[{}]", gDataManager->FormatLocation(display_cards[i]->location, display_cards[i]->sequence),
				display_cards[i]->sequence + 1);
		}
		mainGame->stDisplayPos[i]->setText(text.c_str());
		if(display_cards[i]->location == LOCATION_OVERLAY) {
			if(display_cards[i]->owner != display_cards[i]->overlayTarget->controler)
				mainGame->stDisplayPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_OVERLAY_TEXT_VAL);
			if(display_cards[i]->overlayTarget->controler)
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
			else 
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		} else if(display_cards[i]->location == LOCATION_EXTRA || display_cards[i]->location == LOCATION_REMOVED) {
			if(display_cards[i]->position & POS_FACEDOWN)
				mainGame->stDisplayPos[i]->setOverrideColor(skin::DUELFIELD_CARD_SELECT_WINDOW_SET_TEXT_VAL);
			if(display_cards[i]->controler)
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
			else 
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		} else {
			if(display_cards[i]->controler)
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_OPPONENT_WINDOW_BACKGROUND_VAL);
			else 
				mainGame->stDisplayPos[i]->setBackgroundColor(skin::DUELFIELD_CARD_SELF_WINDOW_BACKGROUND_VAL);
		}
		mainGame->stDisplayPos[i]->setVisible(true);
		mainGame->stDisplayPos[i]->setRelativePosition(mainGame->Scale<irr::s32>(startpos + i * 125, 30, startpos + 120 + i * 125, 50));
	}
	if(display_cards.size() <= 5) {
		for(int i = display_cards.size(); i < 5; ++i) {
			mainGame->btnCardDisplay[i]->setVisible(false);
			mainGame->stDisplayPos[i]->setVisible(false);
		}
		mainGame->scrDisplayList->setPos(0);
		mainGame->scrDisplayList->setVisible(false);
	} else {
		mainGame->scrDisplayList->setVisible(true);
		mainGame->scrDisplayList->setMin(0);
		mainGame->scrDisplayList->setMax((display_cards.size() - 5) * 10 + 9);
		mainGame->scrDisplayList->setPos(0);
	}
	mainGame->btnDisplayOK->setVisible(true);
	mainGame->PopupElement(mainGame->wCardDisplay);
}
void ClientField::ShowSelectOption(uint64 select_hint) {
	selected_option = 0;
	int count = select_options.size();
	bool quickmode = true;// (count <= 5);
	mainGame->gMutex.lock();
	for(auto option : select_options) {
		if(mainGame->guiFont->getDimension(gDataManager->GetDesc(option, mainGame->dInfo.compat_mode)).Width > 310) {
			quickmode = false;
			break;
		}
	}
	for(int i = 0; (i < count) && (i < 5) && quickmode; i++)
		mainGame->btnOption[i]->setText(gDataManager->GetDesc(select_options[i], mainGame->dInfo.compat_mode).c_str());
	irr::core::recti pos = mainGame->wOptions->getRelativePosition();
	if(count > 5 && quickmode)
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + mainGame->Scale(375);
	else
		pos.LowerRightCorner.X = pos.UpperLeftCorner.X + mainGame->Scale(350);
	if(quickmode) {
		mainGame->scrOption->setVisible(count > 5);
		mainGame->scrOption->setMax(count - 5);
		mainGame->scrOption->setMin(0);
		mainGame->scrOption->setPos(0);
		mainGame->stOptions->setVisible(false);
		mainGame->btnOptionp->setVisible(false);
		mainGame->btnOptionn->setVisible(false);
		mainGame->btnOptionOK->setVisible(false);
		for(int i = 0; i < 5; i++)
			mainGame->btnOption[i]->setVisible(i < count);
		int newheight = mainGame->Scale(30 + 40 * ((count > 5) ? 5 : count));
		int oldheight = pos.LowerRightCorner.Y - pos.UpperLeftCorner.Y;
		pos.UpperLeftCorner.Y = pos.UpperLeftCorner.Y + (oldheight - newheight) / 2;
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + newheight;
		mainGame->wOptions->setRelativePosition(pos);
	} else {
		mainGame->stOptions->setText(gDataManager->GetDesc(select_options[0], mainGame->dInfo.compat_mode).c_str());
		mainGame->stOptions->setVisible(true);
		mainGame->btnOptionp->setVisible(false);
		mainGame->btnOptionn->setVisible(count > 1);
		mainGame->btnOptionOK->setVisible(true);
		for(int i = 0; i < 5; i++)
			mainGame->btnOption[i]->setVisible(false);
		pos.LowerRightCorner.Y = pos.UpperLeftCorner.Y + mainGame->Scale(140);
		mainGame->wOptions->setRelativePosition(pos);
	}
	mainGame->wOptions->setText(gDataManager->GetDesc(select_hint ? select_hint : 555, mainGame->dInfo.compat_mode).c_str());
	mainGame->PopupElement(mainGame->wOptions);
	mainGame->gMutex.unlock();
}
void ClientField::ReplaySwap() {
	auto reset = [](ClientCard* pcard)->void {
		if(pcard) {
			pcard->controler = 1 - pcard->controler;
			pcard->UpdateDrawCoordinates(true);
			pcard->is_moving = false;
		}
	};
	auto resetloc = [&reset](std::vector<ClientCard*> zone)->void {
		for(auto& pcard : zone)
			reset(pcard);
	};
	std::swap(deck[0], deck[1]);
	std::swap(hand[0], hand[1]);
	std::swap(mzone[0], mzone[1]);
	std::swap(szone[0], szone[1]);
	std::swap(grave[0], grave[1]);
	std::swap(remove[0], remove[1]);
	std::swap(extra[0], extra[1]);
	std::swap(extra_p_count[0], extra_p_count[1]);
	std::swap(skills[0], skills[1]);
	for(int p = 0; p < 2; ++p) {
		resetloc(deck[p]);
		resetloc(hand[p]);
		resetloc(mzone[p]);
		resetloc(szone[p]);
		resetloc(grave[p]);
		resetloc(remove[p]);
		resetloc(extra[p]);
		reset(skills[p]);
	}
	for(auto& pcard : overlay_cards)
		reset(pcard);
	mainGame->dInfo.isFirst = !mainGame->dInfo.isFirst;
	mainGame->dInfo.isTeam1 = !mainGame->dInfo.isTeam1;
	mainGame->dInfo.isReplaySwapped = !mainGame->dInfo.isReplaySwapped;
	std::swap(mainGame->dInfo.lp[0], mainGame->dInfo.lp[1]);
	std::swap(mainGame->dInfo.strLP[0], mainGame->dInfo.strLP[1]);
	std::swap(mainGame->dInfo.current_player[0], mainGame->dInfo.current_player[1]);
	for(auto& chit : chains) {
		chit.controler = 1 - chit.controler;
		chit.UpdateDrawCoordinates();
	}
	disabled_field = (disabled_field >> 16) | (disabled_field << 16);
}
void ClientField::RefreshAllCards() {
	auto refresh = [](ClientCard* pcard) {
		if(pcard) {
			pcard->UpdateDrawCoordinates(true);
			pcard->is_moving = false;
		}
	};
	auto refreshloc = [&refresh](std::vector<ClientCard*> zone) {
		for(auto& pcard : zone)
			refresh(pcard);
	};
	for(int p = 0; p < 2; ++p) {
		refreshloc(deck[p]);
		refreshloc(hand[p]);
		refreshloc(mzone[p]);
		refreshloc(szone[p]);
		refreshloc(grave[p]);
		refreshloc(remove[p]);
		refreshloc(extra[p]);
		refresh(skills[p]);
	}
	for(auto& pcard : overlay_cards)
		refresh(pcard);
}
void ClientField::GetChainDrawCoordinates(int controler, int location, int sequence, irr::core::vector3df* t) {
	int field = (mainGame->dInfo.duel_field == 3 || mainGame->dInfo.duel_field == 5) ? 0 : 1;
	int speed = (mainGame->dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	if ((location & (~LOCATION_OVERLAY)) == LOCATION_HAND) {
		t->X = 2.95f;
		t->Y = (controler == 0) ? 3.15f : (-3.15f);
		t->Z = 0.03f;
		return;
	}
	irr::video::S3DVertex* loc = nullptr;
	switch((location & (~LOCATION_OVERLAY))) {
	case LOCATION_DECK: {
		loc = matManager.vFieldDeck[controler][speed];
		t->Z = deck[controler].size() * 0.01f + 0.03f;
		break;
	}
	case LOCATION_MZONE: {
		loc = matManager.vFieldMzone[controler][sequence];
		t->Z = 0.03f;
		break;
	}
	case LOCATION_SZONE: {
		loc = matManager.vFieldSzone[controler][sequence][field][speed];
		t->Z = 0.03f;
		break;
	}
	case LOCATION_GRAVE: {
		loc = matManager.vFieldGrave[controler][field][speed];
		t->Z = grave[controler].size() * 0.01f + 0.03f;
		break;
	}
	case LOCATION_REMOVED: {
		loc = matManager.vFieldRemove[controler][field][speed];
		t->Z = remove[controler].size() * 0.01f + 0.03f;
		break;
	}
	case LOCATION_EXTRA: {
		loc = matManager.vFieldExtra[controler][speed];
		t->Z = extra[controler].size() * 0.01f + 0.03f;
		break;
	}
	default:
		t->X = 0;
		t->Y = 0;
		t->Z = 0;
		return;
	}
	t->X = (loc[0].Pos.X + loc[1].Pos.X) / 2;
	t->Y = (loc[0].Pos.Y + loc[2].Pos.Y) / 2;
}
void ClientField::GetCardDrawCoordinates(ClientCard* pcard, irr::core::vector3df* t, irr::core::vector3df* r, bool setTrans) {
	static const irr::core::vector3df selfATK{ 0.0f, 0.0f, 0.0f };
	static const irr::core::vector3df selfDEF{ 0.0f, 0.0f, -irr::core::HALF_PI };
	static const irr::core::vector3df oppoATK{ 0.0f, 0.0f, irr::core::PI };
	static const irr::core::vector3df oppoDEF{ 0.0f, 0.0f, irr::core::HALF_PI };
	static const irr::core::vector3df facedown{ 0.0f, irr::core::PI, 0.0f };
	static const irr::core::vector3df handfaceup{ -FIELD_ANGLE, 0.0f, 0.0f };
	static const irr::core::vector3df handfacedown{ FIELD_ANGLE, irr::core::PI, 0.0f };
	auto GetMiddleX = [](irr::video::S3DVertex pos[4])->float {
		return (pos[0].Pos.X + pos[1].Pos.X) / 2.0f;
	};
	auto GetMiddleY = [](irr::video::S3DVertex pos[4])->float {
		return (pos[0].Pos.Y + pos[2].Pos.Y) / 2.0f;
	};
	if(!pcard->location) return;
	int controler = pcard->controler;
	int sequence = pcard->sequence;
	int location = pcard->location;
	int field = (mainGame->dInfo.duel_field == 3 || mainGame->dInfo.duel_field == 5) ? 0 : 1;
	int speed = (mainGame->dInfo.duel_params & DUEL_3_COLUMNS_FIELD) ? 1 : 0;
	auto GetPos = [&](int location) -> irr::video::S3DVertex* {
		switch(location) {
		case LOCATION_DECK:		return matManager.vFieldDeck[controler][speed];
		case LOCATION_MZONE:	return matManager.vFieldMzone[controler][sequence];
		case LOCATION_SZONE:	return matManager.vFieldSzone[controler][sequence][field][speed];
		case LOCATION_GRAVE:	return matManager.vFieldGrave[controler][field][speed];
		case LOCATION_REMOVED:	return matManager.vFieldRemove[controler][field][speed];
		case LOCATION_EXTRA:	return matManager.vFieldExtra[controler][speed];
		case LOCATION_SKILL:	return matManager.vSkillZone[controler][field][speed];
		case LOCATION_OVERLAY:
			if(!pcard->overlayTarget || pcard->overlayTarget->controler > 1)
				return nullptr;
			if(pcard->overlayTarget->location == LOCATION_MZONE)
				return matManager.vFieldMzone[pcard->overlayTarget->controler][pcard->overlayTarget->sequence];
			if(pcard->overlayTarget->location == LOCATION_SZONE)
				return matManager.vFieldSzone[pcard->overlayTarget->controler][pcard->overlayTarget->sequence][field][speed];
		default: return nullptr;
		}
	};

	if(location != LOCATION_HAND) {
		irr::video::S3DVertex* pos = GetPos(location);
		if(!pos)
			return;
		t->X = GetMiddleX(pos);
		t->Y = GetMiddleY(pos);
		t->Z = 0.01f;
		if(location == LOCATION_MZONE) {
			if(controler == 0)
				*r = (pcard->position & POS_DEFENSE) ? selfDEF : selfATK;
			else
				*r = (pcard->position & POS_DEFENSE) ? oppoDEF : oppoATK;
		} else
			*r = (controler == 0) ? selfATK : oppoATK;
		if(((location & (LOCATION_GRAVE | LOCATION_OVERLAY)) == 0) && ((location == LOCATION_DECK && deck_reversed == pcard->is_reversed) ||
			(location != LOCATION_DECK && pcard->position & POS_FACEDOWN))) {
			*r += facedown;
			if(location == LOCATION_MZONE && pcard->position & POS_DEFENSE)
				r->Y = irr::core::PI + 0.001f;
		}
		switch(location) {
			case LOCATION_DECK:
			case LOCATION_GRAVE:
			case LOCATION_REMOVED:
			case LOCATION_EXTRA:
			case LOCATION_SKILL: {
				t->Z += 0.01f * sequence;
				break;
			}
			case LOCATION_OVERLAY: {
				if(pcard->overlayTarget->controler == 0)
					*t = { t->X - 0.12f + 0.06f * sequence, t->Y + 0.06f, 0.005f + pcard->sequence * 0.0001f };
				else
					*t = { t->X + 0.12f - 0.06f * sequence, t->Y - 0.06f, 0.005f + pcard->sequence * 0.0001f };
				break;
			}
		}
	} else {
		const int count = hand[controler].size();
		const int max = (6 - speed * 2);
		const float off = (5.5f - 0.8f * count) / 2.0f + sequence * 0.8f;
		const float zoff1 = pcard->is_hovered ? 0.656f : 0.5f;
		const float zoff2 = (controler == 0) ? (0.001f * sequence) : (-0.001f * sequence);
		float off2 = sequence * (speed ? 2.4f : 4.0f) / (count - 1);
		if(speed && count > max) off2 += 0.8f;
		if(controler == 0) {
			if(count <= max)
				t->X = 1.55f + off;
			else
				t->X = 1.9f + off2;
			t->Y = 4.0f;
		} else {
			if(count <= max)
				t->X = 6.25f - off;
			else
				t->X = 5.9f - off2;
			t->Y = -3.4f;
		}
		if(pcard->is_hovered) t->Y -= 0.16f;
		t->Z = zoff1 + zoff2;
		if(pcard->code && (!mainGame->dInfo.isReplay || !gGameConfig->hideHandsInReplays || pcard->is_public || pcard->is_hovered))
			*r = handfaceup;
		else
			*r = handfacedown;
	}
	if(setTrans) {
		pcard->mTransform.setTranslation(*t);
		pcard->mTransform.setRotationRadians(*r);
	}
}
void ClientField::MoveCard(ClientCard * pcard, int frame) {
	float milliseconds = (float)frame * 1000.0f / 60.0f;
	irr::core::vector3df trans = pcard->curPos;
	irr::core::vector3df rot = pcard->curRot;
	GetCardDrawCoordinates(pcard, &trans, &rot);
	pcard->dPos = (trans - pcard->curPos) / milliseconds;
	float diff = rot.X - pcard->curRot.X;
	while (diff < 0) diff += irr::core::PI * 2;
	while (diff > irr::core::PI * 2)
		diff -= irr::core::PI * 2;
	if (diff < irr::core::PI)
		pcard->dRot.X = diff / milliseconds;
	else
		pcard->dRot.X = -(irr::core::PI * 2 - diff) / milliseconds;
	diff = rot.Y - pcard->curRot.Y;
	while (diff < 0) diff += irr::core::PI * 2;
	while (diff > irr::core::PI * 2) diff -= irr::core::PI * 2;
	if (diff < irr::core::PI)
		pcard->dRot.Y = diff / milliseconds;
	else
		pcard->dRot.Y = -(irr::core::PI * 2 - diff) / milliseconds;
	diff = rot.Z - pcard->curRot.Z;
	while (diff < 0) diff += irr::core::PI * 2;
	while (diff > irr::core::PI * 2) diff -= irr::core::PI * 2;
	if (diff < irr::core::PI)
		pcard->dRot.Z = diff / milliseconds;
	else
		pcard->dRot.Z = -(irr::core::PI * 2 - diff) / milliseconds;
	pcard->is_moving = true;
	pcard->refresh_on_stop = true;
	pcard->aniFrame = milliseconds;
}
void ClientField::FadeCard(ClientCard * pcard, int alpha, int frame) {
	float milliseconds = (float)frame * 1000.0f / 60.0f;
	pcard->dAlpha = (alpha - pcard->curAlpha) / milliseconds;
	pcard->is_fading = true;
	pcard->aniFrame = milliseconds;
}
bool ClientField::ShowSelectSum() {
	if(CheckSelectSum()) {
		if(selectsum_cards.size() == 0 || selectable_cards.size() == 0) {
			SetResponseSelectedCards();
			ShowCancelOrFinishButton(0);
			DuelClient::SendResponse();
			return true;
		} else {
			select_ready = true;
		}
	} else
		select_ready = false;
	bool panelmode = false;
	for (auto& card : selectable_cards) {
		if (card->location & (LOCATION_DECK+LOCATION_EXTRA+LOCATION_GRAVE+LOCATION_REMOVED+LOCATION_OVERLAY)) {
			panelmode = true;
			break;
		}
	}
	mainGame->wCardSelect->setVisible(false);
	mainGame->stCardListTip->setVisible(false);
	if(panelmode) {
		mainGame->dField.ShowSelectCard(select_ready);
	}
	mainGame->stHintMsg->setVisible(!panelmode);
	if (select_ready) {
		ShowCancelOrFinishButton(2);
	} else {
		ShowCancelOrFinishButton(0);
	}
	return false;
}
bool ClientField::CheckSelectSum() {
	std::set<ClientCard*> selable;
	for(auto& card : selectsum_all) {
		card->is_selectable = false;
		card->is_selected = false;
		selable.insert(card);
	}
	for(auto& card : must_select_cards) {
		card->is_selectable = true;
		card->is_selected = true;
		selable.erase(card);
	}
	for(auto& card : selected_cards) {
		card->is_selectable = true;
		card->is_selected = true;
		selable.erase(card);
	}
	selected_cards.insert(selected_cards.end(), must_select_cards.begin(), must_select_cards.end());
	selectsum_cards.clear();
	for(auto& card : selectable_cards) {
		SetShowMark(card, false);
	}
	mainGame->stCardListTip->setVisible(false);
	if (select_mode == 0) {
		bool ret = check_sel_sum_s(selable, 0, select_sumval);
		selectable_cards.clear();
		std::sort(mainGame->dField.must_select_cards.begin(), mainGame->dField.must_select_cards.end(), ClientCard::client_card_sort);
		for(auto& card : must_select_cards) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
			auto it = std::find(selected_cards.begin(), selected_cards.end(), card);
			if (it != selected_cards.end())
				selected_cards.erase(it);
		}
		std::sort(mainGame->dField.selected_cards.begin(), mainGame->dField.selected_cards.end(), ClientCard::client_card_sort);
		for(auto& card : selected_cards) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
		}
		std::vector<ClientCard*> tmp(selectsum_cards.begin(), selectsum_cards.end());
		std::sort(tmp.begin(), tmp.end(), ClientCard::client_card_sort);
		for(auto& card : tmp) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
		}
		return ret;
	} else {
		int mm = -1, mx = -1, max = 0, sumc = 0;
		bool ret = false;
		for (auto sit = selected_cards.begin(); sit != selected_cards.end(); ++sit) {
			int op1 = (*sit)->opParam & 0xffff;
			int op2 = (*sit)->opParam >> 16;
			int opmin = (op2 > 0 && op1 > op2) ? op2 : op1;
			int opmax = op2 > op1 ? op2 : op1;
			if (mm == -1 || opmin < mm)
				mm = opmin;
			if (mx == -1 || opmax < mx)
				mx = opmax;
			sumc += opmin;
			max += opmax;
		}
		if (select_sumval <= sumc) {
			for (auto& card : must_select_cards) {
				auto it = std::find(selected_cards.begin(), selected_cards.end(), card);
				if (it != selected_cards.end())
					selected_cards.erase(it);
			}
			return true;
		}
		if (select_sumval <= max && select_sumval > max - mx)
			ret = true;
		for(auto sit = selable.begin(); sit != selable.end(); ++sit) {
			int op1 = (*sit)->opParam & 0xffff;
			int op2 = (*sit)->opParam >> 16;
			int m = op1;
			int sums = sumc;
			sums += m;
			int ms = mm;
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
		std::sort(must_select_cards.begin(), must_select_cards.end(), ClientCard::client_card_sort);
		for(auto& card : must_select_cards) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
			auto it = std::find(selected_cards.begin(), selected_cards.end(), card);
			if (it != selected_cards.end())
				selected_cards.erase(it);
		}
		std::sort(selected_cards.begin(), selected_cards.end(), ClientCard::client_card_sort);
		for(auto& card : selected_cards) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
		}
		std::vector<ClientCard*> tmp(selectsum_cards.begin(), selectsum_cards.end());
		std::sort(tmp.begin(), tmp.end(), ClientCard::client_card_sort);
		for(auto& card : tmp) {
			card->is_selectable = true;
			selectable_cards.push_back(card);
		}
		return ret;
	}
}
bool ClientField::check_min(const std::set<ClientCard*>& left, std::set<ClientCard*>::const_iterator index, int min, int max) {
	if (index == left.end())
		return false;
	int op1 = (*index)->opParam & 0xffff;
	int op2 = (*index)->opParam >> 16;
	int m = (op2 > 0 && op1 > op2) ? op2 : op1;
	if (m >= min && m <= max)
		return true;
	++index;
	return (min > m && check_min(left, index, min - m, max - m))
	        || check_min(left, index, min, max);
}
bool ClientField::check_sel_sum_s(const std::set<ClientCard*>& left, int index, int acc) {
	if (acc < 0)
		return false;
	if (index == (int)selected_cards.size()) {
		if (acc == 0) {
			int count = selected_cards.size() - must_select_count;
			return count >= select_min && count <= select_max;
		}
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
void ClientField::check_sel_sum_t(const std::set<ClientCard*>& left, int acc) {
	int count = selected_cards.size() + 1 - must_select_count;
	for (auto sit = left.begin(); sit != left.end(); ++sit) {
		if (selectsum_cards.find(*sit) != selectsum_cards.end())
			continue;
		std::set<ClientCard*> testlist(left);
		testlist.erase(*sit);
		int l = (*sit)->opParam;
		int l1 = l & 0xffff;
		int l2 = l >> 16;
		if (check_sum(testlist.begin(), testlist.end(), acc - l1, count)
		        || (l2 > 0 && check_sum(testlist.begin(), testlist.end(), acc - l2, count))) {
			selectsum_cards.insert(*sit);
		}
	}
}
bool ClientField::check_sum(std::set<ClientCard*>::const_iterator index, std::set<ClientCard*>::const_iterator end, int acc, int count) {
	if (acc == 0)
		return count >= select_min && count <= select_max;
	if (acc < 0 || index == end)
		return false;
	int l = (*index)->opParam;
	int l1 = l & 0xffff;
	int l2 = l >> 16;
	if ((l1 == acc || (l2 > 0 && l2 == acc)) && (count + 1 >= select_min) && (count + 1 <= select_max))
		return true;
	++index;
	return (acc > l1 && check_sum(index, end, acc - l1, count + 1))
	       || (l2 > 0 && acc > l2 && check_sum(index, end, acc - l2, count + 1))
	       || check_sum(index, end, acc, count);
}
#define BINARY_OP(opcode,op) case opcode: {\
								if (stack.size() >= 2) {\
									int rhs = stack.top();\
									stack.pop();\
									int lhs = stack.top();\
									stack.pop();\
									stack.push(lhs op rhs);\
								}\
								break;\
							}
#define UNARY_OP(opcode,op) case opcode: {\
								if (stack.size() >= 1) {\
									int val = stack.top();\
									stack.pop();\
									stack.push(op val);\
								}\
								break;\
							}
#define UNARY_OP_OP(opcode,val,op) UNARY_OP(opcode,cd->val op)
#define GET_OP(opcode,val) case opcode: {\
								stack.push(cd->val);\
								break;\
							}
static bool is_declarable(CardDataC* cd, const std::vector<int64>& opcodes) {
	std::stack<int> stack;
	for(auto& opcode : opcodes) {
		switch(opcode) {
		BINARY_OP(OPCODE_ADD, +);
		BINARY_OP(OPCODE_SUB, -);
		BINARY_OP(OPCODE_MUL, *);
		BINARY_OP(OPCODE_DIV, /);
		BINARY_OP(OPCODE_AND, &&);
		BINARY_OP(OPCODE_OR, ||);
		UNARY_OP(OPCODE_NEG, -);
		UNARY_OP(OPCODE_NOT, !);
		BINARY_OP(OPCODE_BAND, &);
		BINARY_OP(OPCODE_BOR, | );
		UNARY_OP(OPCODE_BNOT, ~);
		BINARY_OP(OPCODE_BXOR, ^);
		BINARY_OP(OPCODE_LSHIFT, <<);
		BINARY_OP(OPCODE_RSHIFT, >>);
		UNARY_OP_OP(OPCODE_ISCODE, code, ==);
		UNARY_OP_OP(OPCODE_ISTYPE, type, &);
		UNARY_OP_OP(OPCODE_ISRACE, race, &);
		UNARY_OP_OP(OPCODE_ISATTRIBUTE, attribute, &);
		GET_OP(OPCODE_GETCODE, code);
		GET_OP(OPCODE_GETTYPE, type);
		GET_OP(OPCODE_GETRACE, race);
		GET_OP(OPCODE_GETATTRIBUTE, attribute);
		//GET_OP(OPCODE_GETSETCARD, setcode);
		case OPCODE_ISSETCARD: {
			if (stack.size() >= 1) {
				int set_code = stack.top();
				stack.pop();
				bool res = false;
				uint16 settype = set_code & 0xfff;
				uint16 setsubtype = set_code & 0xf000;
				for(auto& sc : cd->setcodes) {
					if((sc & 0xfff) == settype && (sc & 0xf000 & setsubtype) == setsubtype) {
						res = true;
						break;
					}
				}
				stack.push(res);
			}
			break;
		}
		default: {
			stack.push(opcode);
			break;
		}
		}
	}
	if(stack.size() != 1 || stack.top() == 0)
		return false;
	return cd->code == CARD_MARINE_DOLPHIN || cd->code == CARD_TWINKLE_MOSS
		|| (!cd->alias && (cd->type & (TYPE_MONSTER + TYPE_TOKEN)) != (TYPE_MONSTER + TYPE_TOKEN));
}
#undef BINARY_OP
#undef UNARY_OP
#undef UNARY_OP_OP
#undef GET_OP
void ClientField::UpdateDeclarableList(bool refresh) {
	auto pname = Utils::ToUpperNoAccents<std::wstring>(mainGame->ebANCard->getText());
	int trycode = BufferIO::GetVal(pname.c_str());
	auto cd = gDataManager->GetCardData(trycode);
	if(cd && is_declarable(cd, declare_opcodes)) {
		mainGame->lstANCard->clear();
		ancard.clear();
		mainGame->lstANCard->addItem(gDataManager->GetName(trycode).c_str());
		ancard.push_back(trycode);
		return;
	}
	if(pname.empty() && !refresh) {
		std::vector<int> cache;
		cache.swap(ancard);
		int sel = mainGame->lstANCard->getSelected();
		int selcode = (sel == -1) ? 0 : cache[sel];
		mainGame->lstANCard->clear();
		for(const auto& trycode : cache) {
			cd = gDataManager->GetCardData(trycode);
			if(cd && is_declarable(cd, declare_opcodes)) {
				ancard.push_back(trycode);
				auto name = gDataManager->GetName(trycode);
				mainGame->lstANCard->addItem(name.c_str());
				if(trycode == selcode)
					mainGame->lstANCard->setSelected(name.c_str());
			}
		}
		if(!ancard.empty())
			return;
	}
	mainGame->lstANCard->clear();
	ancard.clear();
	for(auto& card : gDataManager->cards) {
		auto strings = card.second.GetStrings();
		auto name = Utils::ToUpperNoAccents<std::wstring>(strings->name);
		if(Utils::ContainsSubstring(name, pname)) {
			//datas.alias can be double card names or alias
			if(is_declarable(&card.second._data, declare_opcodes)) {
				if(pname == name) { //exact match
					mainGame->lstANCard->insertItem(0, strings->name.c_str(), -1);
					ancard.insert(ancard.begin(), card.first);
				} else {
					mainGame->lstANCard->addItem(strings->name.c_str());
					ancard.push_back(card.first);
				}
			}
		}
	}
}
void ChainInfo::UpdateDrawCoordinates() {
	mainGame->dField.GetChainDrawCoordinates(controler, location, sequence, &chain_pos);
}
}
