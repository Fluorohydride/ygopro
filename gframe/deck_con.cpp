#include "game_config.h"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <fmt/format.h>
#include <irrlicht.h>
#include "config.h"
#include "deck_con.h"
#include "utils.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "image_manager.h"
#include "game.h"
#include "duelclient.h"
#include "single_mode.h"
#include "client_card.h"

namespace ygo {

static int parse_filter(const wchar_t* pstr, uint32_t& type) {
	if(*pstr == L'=') {
		type = 1;
		return BufferIO::GetVal(pstr + 1);
	}
	if(*pstr >= L'0' && *pstr <= L'9') {
		type = 1;
		return BufferIO::GetVal(pstr);
	}
	if(*pstr == L'>') {
		if(*(pstr + 1) == L'=') {
			type = 2;
			return BufferIO::GetVal(pstr + 2);
		}
		type = 3;
		return BufferIO::GetVal(pstr + 1);
	}
	if(*pstr == L'<') {
		if(*(pstr + 1) == L'=') {
			type = 4;
			return BufferIO::GetVal(pstr + 2);
		}
		type = 5;
		return BufferIO::GetVal(pstr + 1);
	}
	if(*pstr == L'?') {
		type = 6;
		return 0;
	}
	type = 0;
	return 0;
}

static bool check_set_code(const CardDataC& data, const std::vector<uint16_t>& setcodes) {
	const auto& card_setcodes = [&data] {
		if(data.alias) {
			auto _data = gDataManager->GetCardData(data.alias);
			if(_data)
				return _data->setcodes;
		}
		return data.setcodes;
	}();
	if(setcodes.empty())
		return card_setcodes.empty();
	for(auto& set_code : setcodes) {
		if(std::find(card_setcodes.begin(), card_setcodes.end(), set_code) != card_setcodes.end())
			return true;
	}
	return false;
}

void DeckBuilder::Initialize(bool refresh) {
	mainGame->is_building = true;
	mainGame->is_siding = false;
	if(refresh)
		mainGame->ClearCardInfo();
	mainGame->mTopMenu->setVisible(false);
	mainGame->wInfos->setVisible(true);
	mainGame->wCardImg->setVisible(true);
	mainGame->wDeckEdit->setVisible(true);
	mainGame->wFilter->setVisible(true);
	mainGame->wSort->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(gDataManager->GetSysString(1306).data());
	mainGame->btnSideOK->setVisible(false);
	mainGame->btnSideShuffle->setVisible(false);
	mainGame->btnSideSort->setVisible(false);
	mainGame->btnSideReload->setVisible(false);
	mainGame->btnHandTest->setVisible(true);
	mainGame->btnHandTestSettings->setVisible(true);
	mainGame->btnYdkeManage->setVisible(true);
	filterList = &gdeckManager->_lfList[mainGame->cbDBLFList->getSelected()];
	if(refresh) {
		ClearSearch();
	} else if(results.size()) {
		auto oldscrpos = scroll_pos;
		auto oldscrbpos = mainGame->scrFilter->getPos();
		StartFilter();
		scroll_pos = oldscrpos;
		mainGame->scrFilter->setPos(oldscrbpos);
	}
	mouse_pos.set(0, 0);
	hovered_code = 0;
	hovered_pos = 0;
	hovered_seq = -1;
	is_lastcard = 0;
	is_draging = false;
	prev_deck = mainGame->cbDBDecks->getSelected();
	prev_operation = 0;
	mainGame->SetMessageWindow();
	mainGame->device->setEventReceiver(this);
}
void DeckBuilder::Terminate(bool showmenu) {
	mainGame->is_building = false;
	mainGame->is_siding = false;
	if(showmenu) {
		mainGame->ClearCardInfo();
		mainGame->mTopMenu->setVisible(true);
	}
	mainGame->wDeckEdit->setVisible(false);
	mainGame->wCategories->setVisible(false);
	mainGame->wFilter->setVisible(false);
	mainGame->wSort->setVisible(false);
	if(showmenu) {
		mainGame->wCardImg->setVisible(false);
		mainGame->wInfos->setVisible(false);
		mainGame->btnLeaveGame->setVisible(false);
		mainGame->PopupElement(mainGame->wMainMenu);
		mainGame->ClearTextures();
		mainGame->ClearCardInfo(0);
		gdeckManager->ClearDummies();
	}
	mainGame->btnHandTest->setVisible(false);
	mainGame->btnHandTestSettings->setVisible(false);
	mainGame->btnYdkeManage->setVisible(false);
	mainGame->wHandTest->setVisible(false);
	mainGame->device->setEventReceiver(&mainGame->menuHandler);
	mainGame->wACMessage->setVisible(false);
	mainGame->scrFilter->setVisible(false);
	mainGame->SetMessageWindow();
	int sel = mainGame->cbDBDecks->getSelected();
	if(sel >= 0)
		gGameConfig->lastdeck = mainGame->cbDBDecks->getItem(sel);
	gGameConfig->lastlflist = gdeckManager->_lfList[mainGame->cbDBLFList->getSelected()].hash;
}
void DeckBuilder::ImportDeck() {
	const wchar_t* deck_string = Utils::OSOperator->getTextFromClipboard();
	if(deck_string) {
		if(wcsncmp(L"ydke://", deck_string, sizeof(L"ydke://") / sizeof(wchar_t) - 1) == 0)
			DeckManager::ImportDeckBase64(current_deck, deck_string);
		else
			(void)DeckManager::ImportDeckBase64Omega(current_deck, deck_string);
		RefreshLimitationStatus();
	}
}
void DeckBuilder::ExportDeckToClipboard(bool plain_text) {
	auto deck_string = plain_text ? DeckManager::ExportDeckCardNames(current_deck) : DeckManager::ExportDeckBase64(current_deck);
	if(deck_string) {
		Utils::OSOperator->copyToClipboard(deck_string);
		mainGame->stACMessage->setText(gDataManager->GetSysString(1368).data());
	} else {
		mainGame->stACMessage->setText(gDataManager->GetSysString(1369).data());
	}
	mainGame->PopupElement(mainGame->wACMessage, 20);
}
bool DeckBuilder::OnEvent(const irr::SEvent& event) {
	bool stopPropagation = false;
	if(mainGame->dField.OnCommonEvent(event, stopPropagation))
		return stopPropagation;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		int id = event.GUIEvent.Caller->getID();
		if(mainGame->wCategories->isVisible() && id != BUTTON_CATEGORY_OK)
			break;
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO)
			break;
		if(mainGame->wLinkMarks->isVisible() && id != BUTTON_MARKERS_OK)
			break;
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_HAND_TEST_SETTINGS: {
				mainGame->PopupElement(mainGame->wHandTest);
				break;
			}
			case BUTTON_HAND_TEST:
			case BUTTON_HAND_TEST_START: {
				Terminate(false);
				SingleMode::DuelOptions options("hand-test-mode");
				options.handTestNoOpponent = mainGame->chkHandTestNoOpponent->isChecked();
				try {
					options.startingDrawCount = std::stoi(mainGame->ebHandTestStartHand->getText());
				} catch(...) {}
#define CHECK(MR) case (MR - 1):{  options.duelFlags = DUEL_MODE_MR##MR; break; }
				switch (mainGame->cbHandTestDuelRule->getSelected()) {
				CHECK(1)
				CHECK(2)
				CHECK(3)
				CHECK(4)
				CHECK(5)
				case 5: {
					options.duelFlags = DUEL_MODE_SPEED;
					break;
				}
				case 6: {
					options.duelFlags = DUEL_MODE_RUSH;
					break;
				}
				case 7: {
					options.duelFlags = DUEL_MODE_GOAT;
					break;
				}
				}
#undef CHECK
				options.duelFlags |= mainGame->chkHandTestNoShuffle->isChecked() ? DUEL_PSEUDO_SHUFFLE : 0;
				SingleMode::singleSignal.SetNoWait(false);
				SingleMode::StartPlay(std::move(options));
				break;
			}
			case BUTTON_HAND_TEST_CANCEL: {
				mainGame->HideElement(mainGame->wHandTest);
				mainGame->env->setFocus(mainGame->btnHandTestSettings);
				break;
			}
			case BUTTON_DECK_YDKE_MANAGE: {
				mainGame->PopupElement(mainGame->wYdkeManage);
				break;
			}
			case BUTTON_IMPORT_YDKE: {
				ImportDeck();
				break;
			}
			case BUTTON_EXPORT_YDKE: {
				ExportDeckToClipboard(false);
				break;
			}
			case BUTTON_EXPORT_DECK_PLAINTEXT: {
				ExportDeckToClipboard(true);
				break;
			}
			case BUTTON_CLOSE_YDKE_WINDOW: {
				mainGame->HideElement(mainGame->wYdkeManage);
				mainGame->env->setFocus(mainGame->btnYdkeManage);
				break;
			}
			case BUTTON_CLEAR_DECK: {
#ifdef __ANDROID__
				std::lock_guard<std::mutex> lock(mainGame->gMutex);
				mainGame->stQMessage->setText(fmt::format(L"{}", gDataManager->GetSysString(2004)).data());
				mainGame->PopupElement(mainGame->wQuery);
				prev_operation = id;
#else
				current_deck.main.clear();
				current_deck.extra.clear();
				current_deck.side.clear();
#endif
				break;
			}
			case BUTTON_SORT_DECK: {
				std::sort(current_deck.main.begin(), current_deck.main.end(), DataManager::deck_sort_lv);
				std::sort(current_deck.extra.begin(), current_deck.extra.end(), DataManager::deck_sort_lv);
				std::sort(current_deck.side.begin(), current_deck.side.end(), DataManager::deck_sort_lv);
				break;
			}
			case BUTTON_SHUFFLE_DECK: {
				std::shuffle(
					current_deck.main.begin(),
					current_deck.main.end(),
					Utils::GetRandomNumberGenerator()
				);
				break;
			}
			case BUTTON_SAVE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0 && gdeckManager->SaveDeck(current_deck, Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)))) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1335).data());
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_SAVE_DECK_AS: {
				epro::wstringview dname(mainGame->ebDeckname->getText());
				if(dname.empty())
					break;
				int sel = -1;
				{
					const auto upper = Utils::ToUpperNoAccents<std::wstring>({ dname.data(), dname.size() });
					for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
						if(Utils::EqualIgnoreCaseFirst<epro::wstringview>(upper, mainGame->cbDBDecks->getItem(i))) {
							sel = i;
							break;
						}
					}
				}
				if(sel >= 0) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1339).data());
					mainGame->PopupElement(mainGame->wACMessage, 40);
					break;
				} else {
					mainGame->cbDBDecks->addItem(dname.data());
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->getItemCount() - 1);
				}
				if(gdeckManager->SaveDeck(current_deck, Utils::ToPathString(dname))) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1335).data());
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_DELETE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel == -1)
					break;
				std::lock_guard<std::mutex> lock(mainGame->gMutex);
				mainGame->stQMessage->setText(fmt::format(L"{}\n{}", mainGame->cbDBDecks->getItem(sel), gDataManager->GetSysString(1337)).data());
				mainGame->PopupElement(mainGame->wQuery);
				prev_operation = id;
				break;
			}
			case BUTTON_RENAME_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				const wchar_t* dname = mainGame->ebDeckname->getText();
				if(sel == -1 || *dname == 0 || !wcscmp(dname, mainGame->cbDBDecks->getItem(sel)))
					break;
				for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
					if(i == sel)continue;
					if(!wcscmp(dname, mainGame->cbDBDecks->getItem(i))) {
						mainGame->stACMessage->setText(gDataManager->GetSysString(1339).data());
						mainGame->PopupElement(mainGame->wACMessage, 30);
						return false;
					}
				}
				if(DeckManager::RenameDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)), Utils::ToPathString(dname))) {
					mainGame->cbDBDecks->removeItem(sel);
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->addItem(dname));
				} else {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1364).data());
					mainGame->PopupElement(mainGame->wACMessage, 30);
				}
				break;
			}
			case BUTTON_LEAVE_GAME: {
				Terminate();
				break;
			}
			case BUTTON_EFFECT_FILTER: {
				mainGame->PopupElement(mainGame->wCategories);
				break;
			}
			case BUTTON_START_FILTER: {
				StartFilter();
				break;
			}
			case BUTTON_CLEAR_FILTER: {
				ClearSearch();
				break;
			}
			case BUTTON_CATEGORY_OK: {
				filter_effect = 0;
				long long filter = 0x1;
				for(int i = 0; i < 32; ++i, filter <<= 1)
					if(mainGame->chkCategory[i]->isChecked())
						filter_effect |= filter;
				mainGame->HideElement(mainGame->wCategories);
				break;
			}
			case BUTTON_SIDE_OK: {
				auto old_skills = DeckManager::TypeCount(gdeckManager->pre_deck.main, TYPE_SKILL);
				auto new_skills = DeckManager::TypeCount(current_deck.main, TYPE_SKILL);
				if((current_deck.main.size() - new_skills) != (gdeckManager->pre_deck.main.size() - old_skills)
				   || current_deck.extra.size() != gdeckManager->pre_deck.extra.size()) {
					mainGame->PopupMessage(gDataManager->GetSysString(1408));
					break;
				}
				mainGame->ClearCardInfo();
				const auto& deck = current_deck;
				uint8_t deckbuf[0xf000];
				auto* pdeck = deckbuf;
				static constexpr auto max_deck_size = sizeof(deckbuf) / sizeof(uint32_t) - 2;
				const auto totsize = deck.main.size() + deck.extra.size() + deck.side.size();
				if(totsize > max_deck_size) {
					mainGame->PopupMessage(gDataManager->GetSysString(1410));
					break;
				}
				BufferIO::Write<uint32_t>(pdeck, deck.main.size() + deck.extra.size());
				BufferIO::Write<uint32_t>(pdeck, deck.side.size());
				for(const auto& pcard : deck.main)
					BufferIO::Write<uint32_t>(pdeck, pcard->code);
				for(const auto& pcard : deck.extra)
					BufferIO::Write<uint32_t>(pdeck, pcard->code);
				for(const auto& pcard : deck.side)
					BufferIO::Write<uint32_t>(pdeck, pcard->code);
				DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
				gdeckManager->sent_deck = current_deck;
				break;
			}
			case BUTTON_SIDE_RELOAD: {
				current_deck = gdeckManager->pre_deck;
				RefreshLimitationStatus();
				break;
			}
			case BUTTON_MSG_OK: {
				mainGame->HideElement(mainGame->wMessage);
				mainGame->actionSignal.Set();
				break;
			}
			case BUTTON_YES: {
				mainGame->HideElement(mainGame->wQuery);
				if(!mainGame->is_building || mainGame->is_siding)
					break;
				switch(prev_operation) {
				case BUTTON_DELETE_DECK : {
					int sel = mainGame->cbDBDecks->getSelected();
					if(DeckManager::DeleteDeck(current_deck, Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)))) {
						mainGame->cbDBDecks->removeItem(sel);
						int count = mainGame->cbDBDecks->getItemCount();
						if(sel >= count)
							sel = count - 1;
						mainGame->cbDBDecks->setSelected(sel);
						if(sel != -1)
							gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)), nullptr, true);
						mainGame->stACMessage->setText(gDataManager->GetSysString(1338).data());
						mainGame->PopupElement(mainGame->wACMessage, 20);
						prev_deck = sel;
					}
					break;
				}
				case BUTTON_LEAVE_GAME: {
					Terminate();
					break;
				}
				case COMBOBOX_DBDECKS: {
					int sel = mainGame->cbDBDecks->getSelected();
					gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)), nullptr, true);
					prev_deck = sel;
				}
				case BUTTON_CLEAR_DECK: {
					current_deck.main.clear();
					current_deck.extra.clear();
					current_deck.side.clear();
				}
				}
				prev_operation = 0;
				break;
			}
			case BUTTON_NO: {
				mainGame->HideElement(mainGame->wQuery);
				if (prev_operation == COMBOBOX_DBDECKS) {
					mainGame->cbDBDecks->setSelected(prev_deck);
				}
				prev_operation = 0;
				break;
			}
			case BUTTON_MARKS_FILTER: {
				mainGame->PopupElement(mainGame->wLinkMarks);
				break;
			}
			case BUTTON_MARKERS_OK: {
				filter_marks = 0;
				if (mainGame->btnMark[0]->isPressed())
					filter_marks |= 0100;
				if (mainGame->btnMark[1]->isPressed())
					filter_marks |= 0200;
				if (mainGame->btnMark[2]->isPressed())
					filter_marks |= 0400;
				if (mainGame->btnMark[3]->isPressed())
					filter_marks |= 0010;
				if (mainGame->btnMark[4]->isPressed())
					filter_marks |= 0040;
				if (mainGame->btnMark[5]->isPressed())
					filter_marks |= 0001;
				if (mainGame->btnMark[6]->isPressed())
					filter_marks |= 0002;
				if (mainGame->btnMark[7]->isPressed())
					filter_marks |= 0004;
				mainGame->HideElement(mainGame->wLinkMarks);
				StartFilter(true);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_ATTACK:
			case EDITBOX_DEFENSE:
			case EDITBOX_STAR:
			case EDITBOX_SCALE:
			case EDITBOX_KEYWORD: {
				StartFilter();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			auto caller = event.GUIEvent.Caller;
			auto StartFilterIfLongerThan = [&](size_t length) {
				std::wstring filter = caller->getText();
				if (filter.size() > length)
					StartFilter();
			};
			switch (id) {
			case EDITBOX_ATTACK:
			case EDITBOX_DEFENSE:
			case EDITBOX_KEYWORD: {
				StartFilterIfLongerThan(2);
				break;
			}
			case EDITBOX_STAR:
			case EDITBOX_SCALE: {
				StartFilter();
				break;
			}
			case EDITBOX_DECK_NAME: {
				mainGame->ValidateName(caller);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_DBLFLIST: {
				filterList = &gdeckManager->_lfList[mainGame->cbDBLFList->getSelected()];
				mainGame->ReloadCBLimit();
				StartFilter(true);
				break;
			}
			case COMBOBOX_DBDECKS: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0)
					gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)), nullptr, true);
				prev_deck = sel;
				break;
			}
			case COMBOBOX_MAINTYPE: {
				mainGame->ReloadCBCardType2();
				mainGame->cbAttribute->setSelected(0);
				mainGame->cbRace->setSelected(0);
				mainGame->ebAttack->setText(L"");
				mainGame->ebDefense->setText(L"");
				mainGame->ebStar->setText(L"");
				mainGame->ebScale->setText(L"");
				switch(mainGame->cbCardType->getSelected()) {
				case 0:
				case 4: {
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					break;
				}
				case 1: {
					mainGame->cbRace->setEnabled(true);
					mainGame->cbAttribute->setEnabled(true);
					mainGame->ebAttack->setEnabled(true);
					mainGame->ebDefense->setEnabled(true);
					mainGame->ebStar->setEnabled(true);
					mainGame->ebScale->setEnabled(true);
					break;
				}
				case 2: {
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					break;
				}
				case 3: {
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					break;
				}
				}
				StartFilter(true);
				break;
			}
			case COMBOBOX_SECONDTYPE:
			case COMBOBOX_OTHER_FILT: {
				if (id==COMBOBOX_SECONDTYPE && mainGame->cbCardType->getSelected() == 1) {
					if (mainGame->cbCardType2->getSelected() == 8) {
						mainGame->ebDefense->setEnabled(false);
						mainGame->ebDefense->setText(L"");
					} else {
						mainGame->ebDefense->setEnabled(true);
					}
				}
				StartFilter(true);
				break; 
			}
			case COMBOBOX_SORTTYPE: {
				SortList();
				mainGame->env->setFocus(0);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
				case SCROLL_FILTER: {
					GetHoveredCard();
					break;
				}
			}
			break;
		}
		case irr::gui::EGET_CHECKBOX_CHANGED: {
			switch (id) {
				case CHECKBOX_SHOW_ANIME: {
					int prevLimit = mainGame->cbLimit->getSelected();
					mainGame->ReloadCBLimit();
					if (prevLimit < 8)
						mainGame->cbLimit->setSelected(prevLimit);
					StartFilter(true);
					break;
				}
			}
			break;
		}
		default: break;
		}
		break;
	}
	case irr::EET_MOUSE_INPUT_EVENT: {
		bool isroot = mainGame->env->getRootGUIElement()->getElementFromPoint(mouse_pos) == mainGame->env->getRootGUIElement();
		switch(event.MouseInput.Event) {
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			if(is_draging)
				break;
			if(!isroot)
				break;
			if(mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			click_pos = hovered_pos;
			dragx = event.MouseInput.X;
			dragy = event.MouseInput.Y;
			if(!hovered_code || !(dragging_pointer = gDataManager->GetCardData(hovered_code)))
				break;
			if(hovered_pos == 4) {
				if(!event.MouseInput.Shift && !check_limit(dragging_pointer))
					break;
			}
			is_draging = true;
			if(hovered_pos == 1)
				pop_main(hovered_seq);
			else if(hovered_pos == 2)
				pop_extra(hovered_seq);
			else if(hovered_pos == 3)
				pop_side(hovered_seq);
			mouse_pos.set(event.MouseInput.X, event.MouseInput.Y);
			GetHoveredCard();
			break;
		}
		case irr::EMIE_LMOUSE_LEFT_UP: {
			if(!isroot)
				break;
			if(!is_draging) {
				mouse_pos.set(event.MouseInput.X, event.MouseInput.Y);
				GetHoveredCard();
				break;
			}
			bool pushed = false;
			if(hovered_pos == 1)
				pushed = push_main(dragging_pointer, hovered_seq, event.MouseInput.Shift);
			else if(hovered_pos == 2)
				pushed = push_extra(dragging_pointer, hovered_seq + is_lastcard, event.MouseInput.Shift);
			else if(hovered_pos == 3)
				pushed = push_side(dragging_pointer, hovered_seq + is_lastcard, event.MouseInput.Shift);
			else if(hovered_pos == 4 && !mainGame->is_siding)
				pushed = true;
			if(!pushed) {
				if(click_pos == 1)
					push_main(dragging_pointer);
				else if(click_pos == 2)
					push_extra(dragging_pointer);
				else if(click_pos == 3)
					push_side(dragging_pointer);
			}
			is_draging = false;
			break;
		}
		case irr::EMIE_RMOUSE_LEFT_UP: {
			if(!isroot)
				break;
			if(mainGame->is_siding) {
				if(is_draging)
					break;
				if(hovered_pos == 0 || hovered_seq == -1)
					break;
				auto pointer = gDataManager->GetCardData(hovered_code);
				if(!pointer)
					break;
				if(hovered_pos == 1) {
					if(push_side(pointer))
						pop_main(hovered_seq);
				} else if(hovered_pos == 2) {
					if(push_side(pointer))
						pop_extra(hovered_seq);
				} else {
					if(push_extra(pointer) || push_main(pointer))
						pop_side(hovered_seq);
				}
				break;
			}
			if(mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if(!is_draging) {
				if(hovered_pos == 0 || hovered_seq == -1)
					break;
				if(hovered_pos == 1) {
					pop_main(hovered_seq);
				} else if(hovered_pos == 2) {
					pop_extra(hovered_seq);
				} else if(hovered_pos == 3) {
					pop_side(hovered_seq);
				} else {
					auto pointer = gDataManager->GetCardData(hovered_code);
					if(!pointer || !check_limit(pointer))
						break;
					if (event.MouseInput.Shift) {
						push_side(pointer);
					}
					else {
						if (!push_extra(pointer) && !push_main(pointer))
							push_side(pointer);
					}
				}
			} else {
				if(click_pos == 1) {
					push_side(dragging_pointer);
				} else if(click_pos == 2) {
					push_side(dragging_pointer);
				} else if(click_pos == 3) {
					if(!push_extra(dragging_pointer))
						push_main(dragging_pointer);
				} else {
					push_side(dragging_pointer);
				}
				is_draging = false;
			}
			break;
		}
		case irr::EMIE_MMOUSE_LEFT_UP: {
			if(!isroot)
				break;
			if (mainGame->is_siding)
				break;
			if (mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if (hovered_pos == 0 || hovered_seq == -1)
				break;
			if (is_draging)
				break;
			auto pointer = gDataManager->GetCardData(hovered_code);
			if(!pointer || (!event.MouseInput.Shift && !check_limit(pointer)))
				break;
			if (hovered_pos == 1) {
				if(!push_main(pointer))
					push_side(pointer);
			} else if (hovered_pos == 2) {
				if(!push_extra(pointer))
					push_side(pointer);
			} else if (hovered_pos == 3) {
				if(!push_side(pointer) && !push_extra(pointer))
					push_main(pointer);
			} else {
				if(!push_extra(pointer) && !push_main(pointer))
					push_side(pointer);
			}
			break;
		}
		case irr::EMIE_MOUSE_MOVED: {
			mouse_pos.set(event.MouseInput.X, event.MouseInput.Y);
			GetHoveredCard();
			break;
		}
		case irr::EMIE_MOUSE_WHEEL: {
			if(!mainGame->scrFilter->isVisible())
				break;
			if(!mainGame->Resize(805, 160, 1020, 630).isPointInside(mouse_pos))
				break;
			if(event.MouseInput.Wheel < 0) {
				if(mainGame->scrFilter->getPos() < mainGame->scrFilter->getMax())
					mainGame->scrFilter->setPos(mainGame->scrFilter->getPos() + DECK_SEARCH_SCROLL_STEP);
			} else {
				if(mainGame->scrFilter->getPos() > 0)
					mainGame->scrFilter->setPos(mainGame->scrFilter->getPos() - DECK_SEARCH_SCROLL_STEP);
			}
			GetHoveredCard();
			return true;
		}
		default: break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		if(event.KeyInput.PressedDown && !mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
			switch(event.KeyInput.Key) {
			case irr::KEY_KEY_C: {
				if(event.KeyInput.Control)
					ExportDeckToClipboard(event.KeyInput.Shift);
				break;
			}
			case irr::KEY_KEY_V: {
				if(event.KeyInput.Control && !mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX))
					ImportDeck();
				break;
			}
			default:
				break;
			}
		}
		break;
	}
#ifndef __ANDROID__
	case irr::EET_DROP_EVENT: {
		static std::wstring to_open_file;
		switch(event.DropEvent.DropType) {
			case irr::DROP_FILE: {
				irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
				if(root->getElementFromPoint({ event.DropEvent.X, event.DropEvent.Y }) != root)
					break;
				to_open_file = event.DropEvent.Text;
				break;
			}
			case irr::DROP_TEXT: {
				if(!event.DropEvent.Text)
					break;
				if(mainGame->is_siding)
					break;
				irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
				if(root->getElementFromPoint({ event.DropEvent.X, event.DropEvent.Y }) != root)
					break;
				if(wcsncmp(L"ydke://", event.DropEvent.Text, sizeof(L"ydke://") / sizeof(wchar_t) - 1) == 0) {
					gdeckManager->ImportDeckBase64(current_deck, event.DropEvent.Text);
					return true;
				}
				if(gdeckManager->ImportDeckBase64Omega(current_deck, event.DropEvent.Text))
					return true;
				std::wstringstream ss(Utils::ToUpperNoAccents<std::wstring>(event.DropEvent.Text));
				std::wstring to;
				int firstcode = 0;
				while(std::getline(ss, to)) {
					auto pos = to.find_first_not_of(L" \n\r\t");
					if(pos != std::wstring::npos && pos != 0)
						to.erase(to.begin(), to.begin() + pos);
					pos = to.find_last_not_of(L" \n\r\t");
					if(pos != std::wstring::npos) {
						if(pos < to.size())
							pos++;
						to.erase(pos);
					}
					uint32_t code = BufferIO::GetVal(to.data());
					const CardDataC* pointer = nullptr;
					if(!code || !(pointer = gDataManager->GetCardData(code))) {
						for(auto& card : gDataManager->cards) {
							const auto& name = card.second.GetStrings().uppercase_name;
							if(name == to) {
								pointer = &card.second._data;
								break;
							}
						}
					}
					if(!pointer)
						continue;
					if(!firstcode)
						firstcode = pointer->code;
					mouse_pos.set(event.DropEvent.X, event.DropEvent.Y);
					is_draging = true;
					dragging_pointer = pointer;
					GetHoveredCard();
					if(hovered_pos == 3)
						push_side(dragging_pointer, hovered_seq + is_lastcard, true);
					else {
						push_main(dragging_pointer, hovered_seq, true) || push_extra(dragging_pointer, hovered_seq + is_lastcard, true);
					}
					is_draging = false;
				}
				if(firstcode)
					mainGame->ShowCardInfo(firstcode);
				return true;
			}
			case irr::DROP_END:	{
				if(to_open_file.size()) {
					auto extension = Utils::GetFileExtension(to_open_file);
					if(!mainGame->is_siding && extension == L"ydk" && gdeckManager->LoadDeck(Utils::ToPathString(to_open_file), nullptr, true)) {
						auto name = Utils::GetFileName(to_open_file);
						mainGame->ebDeckname->setText(name.data());
						mainGame->cbDBDecks->setSelected(-1);
					} else if(extension == L"pem" || extension == L"cer" || extension == L"crt") {
						gGameConfig->override_ssl_certificate_path = BufferIO::EncodeUTF8(to_open_file);
					}
					to_open_file.clear();
				}
				break;
			}
			default: break;
		}
		break;
	}
#endif
	default: break;
	}
	return false;
}
void DeckBuilder::GetHoveredCard() {
	irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
	if(root->getElementFromPoint(mouse_pos) != root)
		return;
	irr::core::position2di pos = mainGame->Resize(mouse_pos.X, mouse_pos.Y, true);
	int x = pos.X;
	int y = pos.Y;
	int pre_code = hovered_code;
	hovered_pos = 0;
	hovered_code = 0;
	is_lastcard = 0;
	if(x >= 314 && x <= 794) {
		if(y >= 164 && y <= 435) {
			int lx = 10, px, py = (y - 164) / 68;
			hovered_pos = 1;
			if(current_deck.main.size() > 40)
				lx = (current_deck.main.size() - 41) / 4 + 11;
			if(x >= 750)
				px = lx - 1;
			else
				px = (x - 314) * (lx - 1) / 436;
			hovered_seq = py * lx + px;
			if(hovered_seq >= (int)current_deck.main.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = current_deck.main[hovered_seq]->code;
			}
		} else if(y >= 466 && y <= 530) {
			int lx = current_deck.extra.size();
			hovered_pos = 2;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)current_deck.extra.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = current_deck.extra[hovered_seq]->code;
				if(x >= 772)
					is_lastcard = 1;
			}
		} else if (y >= 564 && y <= 628) {
			int lx = current_deck.side.size();
			hovered_pos = 3;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)current_deck.side.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = current_deck.side[hovered_seq]->code;
				if(x >= 772)
					is_lastcard = 1;
			}
		}
	} else if(x >= 810 && x <= 995 && y >= 165 && y <= 626) {
		const int offset = (mainGame->scrFilter->getPos() % DECK_SEARCH_SCROLL_STEP) * -1.f * 0.65f;
		hovered_pos = 4;
		hovered_seq = (y - 165 - offset) / 66;
		int pos = floor(mainGame->scrFilter->getPos() / DECK_SEARCH_SCROLL_STEP) + hovered_seq;
		if(pos >= (int)results.size()) {
			hovered_seq = -1;
			hovered_code = 0;
		} else {
			hovered_code = results[pos]->code;
		}
	}
	if(is_draging) {
		dragx = mouse_pos.X;
		dragy = mouse_pos.Y;
	}
	if(!is_draging && pre_code != hovered_code) {
		if(hovered_code)
			mainGame->ShowCardInfo(hovered_code);
	}
}
#define CHECK_AND_SET(x) if(x != prev_##x) {\
	res = true;\
	}\
	prev_##x = x;
bool DeckBuilder::FiltersChanged() {
	bool res = false;
	CHECK_AND_SET(filter_effect);
	CHECK_AND_SET(filter_type);
	CHECK_AND_SET(filter_type2);
	CHECK_AND_SET(filter_attrib);
	CHECK_AND_SET(filter_race);
	CHECK_AND_SET(filter_atktype);
	CHECK_AND_SET(filter_atk);
	CHECK_AND_SET(filter_deftype);
	CHECK_AND_SET(filter_def);
	CHECK_AND_SET(filter_lvtype);
	CHECK_AND_SET(filter_lv);
	CHECK_AND_SET(filter_scltype);
	CHECK_AND_SET(filter_scl);
	CHECK_AND_SET(filter_marks);
	CHECK_AND_SET(filter_lm);
	return res;
}
#undef CHECK_AND_SET
void DeckBuilder::StartFilter(bool force_refresh) {
	filter_type = mainGame->cbCardType->getSelected();
	filter_type2 = mainGame->cbCardType2->getItemData(mainGame->cbCardType2->getSelected());
	filter_lm = static_cast<limitation_search_filters>(mainGame->cbLimit->getItemData(mainGame->cbLimit->getSelected()));
	if(filter_type == 1) {
		filter_attrib = mainGame->cbAttribute->getItemData(mainGame->cbAttribute->getSelected());
		filter_race = mainGame->cbRace->getItemData(mainGame->cbRace->getSelected());
		filter_atk = parse_filter(mainGame->ebAttack->getText(), filter_atktype);
		filter_def = parse_filter(mainGame->ebDefense->getText(), filter_deftype);
		filter_lv = parse_filter(mainGame->ebStar->getText(), filter_lvtype);
		filter_scl = parse_filter(mainGame->ebScale->getText(), filter_scltype);
	}
	FilterCards(force_refresh);
	GetHoveredCard();
}
void DeckBuilder::FilterCards(bool force_refresh) {
	results.clear();
	std::vector<std::wstring> searchterms;
	if(wcslen(mainGame->ebCardName->getText())) {
		searchterms = Utils::TokenizeString<std::wstring>(Utils::ToUpperNoAccents<std::wstring>(mainGame->ebCardName->getText()), L"||");
	} else
		searchterms = { L"" };
	if(FiltersChanged() || force_refresh)
		searched_terms.clear();
	//removes no longer existing search terms from the cache
	for(auto it = searched_terms.cbegin(); it != searched_terms.cend();) {
		if(std::find(searchterms.begin(), searchterms.end(), it->first) == searchterms.end())
			it = searched_terms.erase(it);
		else
			++it;
	}
	//removes search terms already cached
	for(auto it = searchterms.cbegin(); it != searchterms.cend();) {
		if(searched_terms.count((*it)))
			it = searchterms.erase(it);
		else
			it++;
	}
	for(const auto& term : searchterms) {
		int trycode = BufferIO::GetVal(term.data());
		const CardDataC* data = nullptr;
		if(trycode && (data = gDataManager->GetCardData(trycode))) {
			searched_terms[term] = { data };
			continue;
		}
		std::vector<std::wstring> tokens;
		int modif = 0;
		if(!term.empty()) {
			size_t start = 0;
			if(term.size() >= 2 && memcmp(L"!!", term.data(), sizeof(wchar_t) * 2) == 0) {
				modif |= SEARCH_MODIFIER_NEGATIVE_LOOKUP;
				start += 2;
			}
			if(term.size() + start >= 1) {
				if(term[start] == L'@') {
					modif |= SEARCH_MODIFIER_ARCHETYPE_ONLY;
					start++;
				}
				else if(term[start] == L'$') {
					modif |= SEARCH_MODIFIER_NAME_ONLY;
					start++;
				}
			}
			tokens = Utils::TokenizeString<std::wstring>(term.data() + start, L"*");
		}
		auto set_code = gDataManager->GetSetCode(tokens);
		if(tokens.empty())
			tokens.push_back(L"");
		wchar_t checkterm = term.size() ? term.front() : 0;
		std::vector<const CardDataC*> result;
		for(auto& card : gDataManager->cards) {
			if(CheckCard(&card.second, static_cast<SEARCH_MODIFIER>(modif), tokens, set_code))
				result.push_back(&card.second._data);
		}
		if(result.size())
			searched_terms[term] = result;
	}
	for(const auto& res : searched_terms) {
		results.insert(results.end(), res.second.begin(), res.second.end());
	}
	SortList();
	auto ip = std::unique(results.begin(), results.end());
	results.resize(std::distance(results.begin(), ip));
	result_string = fmt::to_wstring(results.size());
	scroll_pos = 0;
	if(results.size() > 7) {
		mainGame->scrFilter->setVisible(true);
		mainGame->scrFilter->setMax((results.size() - 7) * DECK_SEARCH_SCROLL_STEP);
	} else {
		mainGame->scrFilter->setVisible(false);
	}
	mainGame->scrFilter->setPos(0);
}
bool DeckBuilder::CheckCard(CardDataM* data, SEARCH_MODIFIER modifier, const std::vector<std::wstring>& tokens, const std::vector<uint16_t>& set_code) {
	if(data->_data.type & TYPE_TOKEN  || data->_data.ot & SCOPE_HIDDEN || ((data->_data.ot & SCOPE_OFFICIAL) != data->_data.ot && (!mainGame->chkAnime->isChecked() && !filterList->whitelist)))
		return false;
	switch(filter_type) {
	case 1: {
		if(!(data->_data.type & TYPE_MONSTER) || (data->_data.type & filter_type2) != filter_type2)
			return false;
		if(filter_race && data->_data.race != filter_race)
			return false;
		if(filter_attrib && data->_data.attribute != filter_attrib)
			return false;
		if(filter_atktype) {
			if((filter_atktype == 1 && data->_data.attack != filter_atk) || (filter_atktype == 2 && data->_data.attack < filter_atk)
				|| (filter_atktype == 3 && data->_data.attack <= filter_atk) || (filter_atktype == 4 && (data->_data.attack > filter_atk || data->_data.attack < 0))
				|| (filter_atktype == 5 && (data->_data.attack >= filter_atk || data->_data.attack < 0)) || (filter_atktype == 6 && data->_data.attack != -2))
				return false;
		}
		if(filter_deftype) {
			if((filter_deftype == 1 && data->_data.defense != filter_def) || (filter_deftype == 2 && data->_data.defense < filter_def)
				|| (filter_deftype == 3 && data->_data.defense <= filter_def) || (filter_deftype == 4 && (data->_data.defense > filter_def || data->_data.defense < 0))
				|| (filter_deftype == 5 && (data->_data.defense >= filter_def || data->_data.defense < 0)) || (filter_deftype == 6 && data->_data.defense != -2)
				|| (data->_data.type & TYPE_LINK))
				return false;
		}
		if(filter_lvtype) {
			if((filter_lvtype == 1 && data->_data.level != filter_lv) || (filter_lvtype == 2 && data->_data.level < filter_lv)
				|| (filter_lvtype == 3 && data->_data.level <= filter_lv) || (filter_lvtype == 4 && data->_data.level > filter_lv)
				|| (filter_lvtype == 5 && data->_data.level >= filter_lv) || filter_lvtype == 6)
				return false;
		}
		if(filter_scltype) {
			if((filter_scltype == 1 && data->_data.lscale != filter_scl) || (filter_scltype == 2 && data->_data.lscale < filter_scl)
				|| (filter_scltype == 3 && data->_data.lscale <= filter_scl) || (filter_scltype == 4 && (data->_data.lscale > filter_scl))
				|| (filter_scltype == 5 && (data->_data.lscale >= filter_scl)) || filter_scltype == 6
				|| !(data->_data.type & TYPE_PENDULUM))
				return false;
		}
		break;
	}
	case 2: {
		if(!(data->_data.type & TYPE_SPELL))
			return false;
		if(filter_type2 && data->_data.type != filter_type2)
			return false;
		break;
	}
	case 3: {
		if(!(data->_data.type & TYPE_TRAP))
			return false;
		if(filter_type2 && data->_data.type != filter_type2)
			return false;
		break;
	}
	case 4: {
		if(!(data->_data.type & TYPE_SKILL))
			return false;
		break;
	}
	}
	if(filter_effect && !(data->_data.category & filter_effect))
		return false;
	if(filter_marks && (data->_data.link_marker & filter_marks) != filter_marks)
		return false;
	if((filter_lm != LIMITATION_FILTER_NONE || filterList->whitelist) && filter_lm != LIMITATION_FILTER_ALL) {
		auto flit = filterList->GetLimitationIterator(&data->_data);
		int count = 3;
		if(flit == filterList->content.end()) {
			if(filterList->whitelist)
				count = -1;
		} else
			count = flit->second;
		switch(filter_lm) {
			case LIMITATION_FILTER_BANNED:
			case LIMITATION_FILTER_LIMITED:
			case LIMITATION_FILTER_SEMI_LIMITED:
				if(count != filter_lm - 1)
					return false;
				break;
			case LIMITATION_FILTER_UNLIMITED:
				if(count < 3)
					return false;
				break;
			case LIMITATION_FILTER_OCG:
				if(data->_data.ot != SCOPE_OCG)
					return false;
				break;
			case LIMITATION_FILTER_TCG:
				if(data->_data.ot != SCOPE_TCG)
					return false;
				break;
			case LIMITATION_FILTER_TCG_OCG:
				if(data->_data.ot != SCOPE_OCG_TCG)
					return false;
				break;
			case LIMITATION_FILTER_PRERELEASE:
				if(!(data->_data.ot & SCOPE_PRERELEASE))
					return false;
				break;
			case LIMITATION_FILTER_SPEED:
				if(!(data->_data.ot & SCOPE_SPEED))
					return false;
				break;
			case LIMITATION_FILTER_RUSH:
				if(!(data->_data.ot & SCOPE_RUSH))
					return false;
				break;
			case LIMITATION_FILTER_ANIME:
				if(data->_data.ot != SCOPE_ANIME)
					return false;
				break;
			case LIMITATION_FILTER_ILLEGAL:
				if(data->_data.ot != SCOPE_ILLEGAL)
					return false;
				break;
			case LIMITATION_FILTER_VIDEOGAME:
				if(data->_data.ot != SCOPE_VIDEO_GAME)
					return false;
				break;
			case LIMITATION_FILTER_CUSTOM:
				if(data->_data.ot != SCOPE_CUSTOM)
					return false;
				break;
			default:
				break;
		}
		if(filterList->whitelist && count < 0)
			return false;
	}
	if(tokens.size()) {
		const auto checkNeg = [negative = !!(modifier & SEARCH_MODIFIER_NEGATIVE_LOOKUP)] (bool res) -> bool {
			if(negative)
				return !res;
			return res;
		};
		const auto& strings = data->GetStrings();
		if(modifier & SEARCH_MODIFIER_NAME_ONLY) {
			return checkNeg(Utils::ContainsSubstring(strings.uppercase_name, tokens));
		} else if(modifier & SEARCH_MODIFIER_ARCHETYPE_ONLY) {
			if(set_code.empty() && tokens.size() > 0 && tokens.front().size())
				return checkNeg(false);
			return checkNeg(check_set_code(data->_data, set_code));
		} else {
			return checkNeg((set_code.size() && check_set_code(data->_data, set_code)) || Utils::ContainsSubstring(strings.uppercase_name, tokens)
					|| Utils::ContainsSubstring(strings.uppercase_text, tokens));
		}
	}
	return true;
}
void DeckBuilder::ClearSearch() {
	mainGame->cbCardType->setSelected(0);
	mainGame->cbCardType2->setSelected(0);
	mainGame->cbCardType2->setEnabled(false);
	mainGame->cbRace->setEnabled(false);
	mainGame->cbAttribute->setEnabled(false);
	mainGame->ebAttack->setEnabled(false);
	mainGame->ebDefense->setEnabled(false);
	mainGame->ebStar->setEnabled(false);
	mainGame->ebScale->setEnabled(false);
	mainGame->ebCardName->setText(L"");
	mainGame->scrFilter->setVisible(false);
	searched_terms.clear();
	ClearFilter();
	results.clear();
	result_string = L"0";
	scroll_pos = 0;
	mainGame->env->setFocus(mainGame->ebCardName);
}
void DeckBuilder::ClearFilter() {
	mainGame->cbAttribute->setSelected(0);
	mainGame->cbRace->setSelected(0);
	mainGame->cbLimit->setSelected(0);
	mainGame->ebAttack->setText(L"");
	mainGame->ebDefense->setText(L"");
	mainGame->ebStar->setText(L"");
	mainGame->ebScale->setText(L"");
	filter_effect = 0;
	for(int i = 0; i < 32; ++i)
		mainGame->chkCategory[i]->setChecked(false);
	filter_marks = 0;
	for(int i = 0; i < 8; i++)
		mainGame->btnMark[i]->setPressed(false);
}
void DeckBuilder::SortList() {
	auto left = results.begin();
	for(auto it = results.begin(); it != results.end(); ++it) {
		if(searched_terms.find(std::wstring{ gDataManager->GetUppercaseName((*it)->code) }) != searched_terms.end()) {
			std::iter_swap(left, it);
			++left;
		}
	}
	switch(mainGame->cbSortType->getSelected()) {
	case 0:
		std::sort(left, results.end(), DataManager::deck_sort_lv);
		break;
	case 1:
		std::sort(left, results.end(), DataManager::deck_sort_atk);
		break;
	case 2:
		std::sort(left, results.end(), DataManager::deck_sort_def);
		break;
	case 3:
		std::sort(left, results.end(), DataManager::deck_sort_name);
		break;
	}
}
void DeckBuilder::RefreshLimitationStatus() {
	main_and_extra_legend_count = DeckManager::OTCount(current_deck.main, SCOPE_LEGEND) + DeckManager::OTCount(current_deck.extra, SCOPE_LEGEND);
	main_skill_count = DeckManager::TypeCount(current_deck.main, TYPE_SKILL);
	main_monster_count = DeckManager::TypeCount(current_deck.main, TYPE_MONSTER);
	main_spell_count = DeckManager::TypeCount(current_deck.main, TYPE_SPELL);
	main_trap_count = DeckManager::TypeCount(current_deck.main, TYPE_TRAP);

	extra_fusion_count = DeckManager::TypeCount(current_deck.extra, TYPE_MONSTER);
	extra_xyz_count = DeckManager::TypeCount(current_deck.extra, TYPE_XYZ);
	extra_synchro_count = DeckManager::TypeCount(current_deck.extra, TYPE_SYNCHRO);
	extra_link_count = DeckManager::TypeCount(current_deck.extra, TYPE_LINK);

	side_monster_count = DeckManager::TypeCount(current_deck.side, TYPE_MONSTER);
	side_spell_count = DeckManager::TypeCount(current_deck.side, TYPE_SPELL);
	side_trap_count = DeckManager::TypeCount(current_deck.side, TYPE_TRAP);
}
void DeckBuilder::RefreshLimitationStatusOnRemoved(const CardDataC* card, DeckType location) {
	switch(location) {
		case DeckType::MAIN:
		{
			if(card->ot & SCOPE_LEGEND)
				--main_and_extra_legend_count;
			if(card->type & TYPE_MONSTER)
				--main_monster_count;
			if(card->type & TYPE_SPELL)
				--main_spell_count;
			if(card->type & TYPE_TRAP)
				--main_trap_count;
			if(card->type & TYPE_SKILL)
				--main_skill_count;
			break;
		}
		case DeckType::EXTRA:
		{
			if(card->ot & SCOPE_LEGEND)
				--main_and_extra_legend_count;
			if(card->type & TYPE_FUSION)
				--extra_fusion_count;
			if(card->type & TYPE_XYZ)
				--extra_xyz_count;
			if(card->type & TYPE_SYNCHRO)
				--extra_synchro_count;
			if(card->type & TYPE_LINK)
				--extra_link_count;
			break;
		}
		case DeckType::SIDE:
		{
			if(card->type & TYPE_MONSTER)
				--side_monster_count;
			if(card->type & TYPE_SPELL)
				--side_spell_count;
			if(card->type & TYPE_TRAP)
				--side_trap_count;
			break;
		}
	}
}
void DeckBuilder::RefreshLimitationStatusOnAdded(const CardDataC* card, DeckType location) {
	switch(location) {
		case DeckType::MAIN:
		{
			if(card->ot & SCOPE_LEGEND)
				++main_and_extra_legend_count;
			if(card->type & TYPE_MONSTER)
				++main_monster_count;
			if(card->type & TYPE_SPELL)
				++main_spell_count;
			if(card->type & TYPE_TRAP)
				++main_trap_count;
			if(card->type & TYPE_SKILL)
				++main_skill_count;
			break;
		}
		case DeckType::EXTRA:
		{
			if(card->ot & SCOPE_LEGEND)
				++main_and_extra_legend_count;
			if(card->type & TYPE_FUSION)
				++extra_fusion_count;
			if(card->type & TYPE_XYZ)
				++extra_xyz_count;
			if(card->type & TYPE_SYNCHRO)
				++extra_synchro_count;
			if(card->type & TYPE_LINK)
				++extra_link_count;
			break;
		}
		case DeckType::SIDE:
		{
			if(card->type & TYPE_MONSTER)
				++side_monster_count;
			if(card->type & TYPE_SPELL)
				++side_spell_count;
			if(card->type & TYPE_TRAP)
				++side_trap_count;
			break;
		}
	}
}
bool DeckBuilder::push_main(const CardDataC* pointer, int seq, bool forced) {
	if(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK) && pointer->type != (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = current_deck.main;
	if(!forced && !mainGame->is_siding) {
		if(main_and_extra_legend_count >= 1 && (pointer->ot & SCOPE_LEGEND))
			return false;
		if(main_skill_count >= 1 && (pointer->type & TYPE_SKILL))
			return false;
		if(container.size() >= 60)
			return false;
	}
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	RefreshLimitationStatusOnAdded(pointer, DeckType::MAIN);
	return true;
}
bool DeckBuilder::push_extra(const CardDataC* pointer, int seq, bool forced) {
	if(!(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) || pointer->type == (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = current_deck.extra;
	if(!forced && !mainGame->is_siding) {
		if(main_and_extra_legend_count >= 1 && (pointer->ot & SCOPE_LEGEND))
			return false;
		if(container.size() >= 15)
			return false;
	}
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	RefreshLimitationStatusOnAdded(pointer, DeckType::EXTRA);
	return true;
}
bool DeckBuilder::push_side(const CardDataC* pointer, int seq, bool forced) {
	auto& container = current_deck.side;
	if(!mainGame->is_siding && !forced && container.size() >= 15)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	RefreshLimitationStatusOnAdded(pointer, DeckType::SIDE);
	return true;
}
void DeckBuilder::pop_main(int seq) {
	auto& container = current_deck.main;
	auto it = container.begin() + seq;
	auto pcard = *it;
	container.erase(it);
	GetHoveredCard();
	RefreshLimitationStatusOnRemoved(pcard, DeckType::MAIN);
}
void DeckBuilder::pop_extra(int seq) {
	auto& container = current_deck.extra;
	auto it = container.begin() + seq;
	auto pcard = *it;
	container.erase(it);
	GetHoveredCard();
	RefreshLimitationStatusOnRemoved(pcard, DeckType::EXTRA);
}
void DeckBuilder::pop_side(int seq) {
	auto& container = current_deck.side;
	auto it = container.begin() + seq;
	auto pcard = *it;
	container.erase(it);
	GetHoveredCard();
	RefreshLimitationStatusOnRemoved(pcard, DeckType::SIDE);
}
bool DeckBuilder::check_limit(const CardDataC* pointer) {
	uint32_t limitcode = pointer->alias ? pointer->alias : pointer->code;
	int found = 0;
	int limit = filterList->whitelist ? 0 : 3;
	auto endit = filterList->content.end();
	auto it = filterList->GetLimitationIterator(pointer);
	if(it != endit)
		limit = it->second;
	if(limit == 0)
		return false;
	const auto& deck = current_deck;
	for(auto* plist : { &deck.main, &deck.extra, &deck.side }) {
		for(auto& pcard : *plist) {
			if(pcard->code == limitcode || pcard->alias == limitcode) {
				if((it = filterList->content.find(pcard->code)) != endit)
					limit = std::min(limit, it->second);
				else if((it = filterList->content.find(pcard->alias)) != endit)
					limit = std::min(limit, it->second);
				found++;
			}
			if(limit <= found)
				return false;
		}
	}
	return true;
}
void DeckBuilder::RefreshCurrentDeck() {
	gdeckManager->RefreshDeck(current_deck);
	RefreshLimitationStatus();
}
}
