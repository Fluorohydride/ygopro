#include "game_config.h"
#include <irrlicht.h>
#include "random_fwd.h"
#include "config.h"
#include "deck_con.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "image_manager.h"
#include "game.h"
#include "duelclient.h"
#include "single_mode.h"
#include "client_card.h"
#ifndef __ANDROID__
#include <sstream>
#endif
#include <algorithm>
#include <unordered_map>

namespace ygo {

static int parse_filter(const wchar_t* pstr, unsigned int* type) {
	if(*pstr == L'=') {
		*type = 1;
		return BufferIO::GetVal(pstr + 1);
	} else if(*pstr >= L'0' && *pstr <= L'9') {
		*type = 1;
		return BufferIO::GetVal(pstr);
	} else if(*pstr == L'>') {
		if(*(pstr + 1) == L'=') {
			*type = 2;
			return BufferIO::GetVal(pstr + 2);
		} else {
			*type = 3;
			return BufferIO::GetVal(pstr + 1);
		}
	} else if(*pstr == L'<') {
		if(*(pstr + 1) == L'=') {
			*type = 4;
			return BufferIO::GetVal(pstr + 2);
		} else {
			*type = 5;
			return BufferIO::GetVal(pstr + 1);
		}
	} else if(*pstr == L'?') {
		*type = 6;
		return 0;
	}
	*type = 0;
	return 0;
}

static bool check_set_code(const CardDataC& data, const std::vector<unsigned int>& setcodes) {
	auto card_setcodes = data.setcodes;
	if (data.alias) {
		auto _data = gDataManager->GetCardData(data.alias);
		if(_data)
			card_setcodes = _data->setcodes;
	}
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
	mainGame->btnLeaveGame->setText(gDataManager->GetSysString(1306).c_str());
	mainGame->btnSideOK->setVisible(false);
	mainGame->btnSideShuffle->setVisible(false);
	mainGame->btnSideSort->setVisible(false);
	mainGame->btnSideReload->setVisible(false);
	mainGame->btnHandTest->setVisible(true);
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
	}
	mainGame->btnHandTest->setVisible(false);
	mainGame->device->setEventReceiver(&mainGame->menuHandler);
	mainGame->wACMessage->setVisible(false);
	mainGame->scrFilter->setVisible(false);
	mainGame->SetMessageWindow();
	int sel = mainGame->cbDBDecks->getSelected();
	if(sel >= 0)
		gGameConfig->lastdeck = mainGame->cbDBDecks->getItem(sel);
	gGameConfig->lastlflist = gdeckManager->_lfList[mainGame->cbDBLFList->getSelected()].hash;
	if(exit_on_return)
		mainGame->device->closeDevice();
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
			case BUTTON_HAND_TEST: {
				Terminate(false);
				open_file = true;
				open_file_name = EPRO_TEXT("hand-test-mode");
				SingleMode::singleSignal.SetNoWait(false);
				SingleMode::StartPlay();
				break;
			}
			case BUTTON_CLEAR_DECK: {
				gdeckManager->current_deck.main.clear();
				gdeckManager->current_deck.extra.clear();
				gdeckManager->current_deck.side.clear();
				break;
			}
			case BUTTON_SORT_DECK: {
				std::sort(gdeckManager->current_deck.main.begin(), gdeckManager->current_deck.main.end(), ClientCard::deck_sort_lv);
				std::sort(gdeckManager->current_deck.extra.begin(), gdeckManager->current_deck.extra.end(), ClientCard::deck_sort_lv);
				std::sort(gdeckManager->current_deck.side.begin(), gdeckManager->current_deck.side.end(), ClientCard::deck_sort_lv);
				break;
			}
			case BUTTON_SHUFFLE_DECK: {
				std::shuffle(
					gdeckManager->current_deck.main.begin(),
					gdeckManager->current_deck.main.end(),
					randengine(time(nullptr))
				);
				break;
			}
			case BUTTON_SAVE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0 && gdeckManager->SaveDeck(gdeckManager->current_deck, Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)))) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1335).c_str());
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_SAVE_DECK_AS: {
				const wchar_t* dname = mainGame->ebDeckname->getText();
				if(*dname == 0)
					break;
				int sel = -1;
				for(size_t i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
					if(!wcscmp(dname, mainGame->cbDBDecks->getItem(i))) {
						sel = i;
						break;
					}
				}
				if(sel >= 0) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1339).c_str());
					mainGame->PopupElement(mainGame->wACMessage, 30);
					break;
				} else {
					mainGame->cbDBDecks->addItem(dname);
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->getItemCount() - 1);
				}
				if(gdeckManager->SaveDeck(gdeckManager->current_deck, Utils::ToPathString(dname))) {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1335).c_str());
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_DELETE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				mainGame->stQMessage->setText(fmt::format(L"{}\n{}", mainGame->cbDBDecks->getItem(sel), gDataManager->GetSysString(1337)).c_str());
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.unlock();
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
						mainGame->stACMessage->setText(gDataManager->GetSysString(1339).c_str());
						mainGame->PopupElement(mainGame->wACMessage, 30);
						return false;
					}
				}
				if(gdeckManager->RenameDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)), Utils::ToPathString(dname))) {
					mainGame->cbDBDecks->removeItem(sel);
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->addItem(dname));
				} else {
					mainGame->stACMessage->setText(gDataManager->GetSysString(1364).c_str());
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
				if(gdeckManager->current_deck.main.size() != gdeckManager->pre_deck.main.size() || gdeckManager->current_deck.extra.size() != gdeckManager->pre_deck.extra.size()
						|| gdeckManager->current_deck.side.size() != gdeckManager->pre_deck.side.size()) {
					mainGame->PopupMessage(gDataManager->GetSysString(1410));
					break;
				}
				mainGame->ClearCardInfo();
				char deckbuf[1024];
				char* pdeck = deckbuf;
				BufferIO::Write<int32_t>(pdeck, gdeckManager->current_deck.main.size() + gdeckManager->current_deck.extra.size());
				BufferIO::Write<int32_t>(pdeck, gdeckManager->current_deck.side.size());
				for(size_t i = 0; i < gdeckManager->current_deck.main.size(); ++i)
					BufferIO::Write<int32_t>(pdeck, gdeckManager->current_deck.main[i]->code);
				for(size_t i = 0; i < gdeckManager->current_deck.extra.size(); ++i)
					BufferIO::Write<int32_t>(pdeck, gdeckManager->current_deck.extra[i]->code);
				for(size_t i = 0; i < gdeckManager->current_deck.side.size(); ++i)
					BufferIO::Write<int32_t>(pdeck, gdeckManager->current_deck.side[i]->code);
				DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
				break;
			}
			case BUTTON_SIDE_RELOAD: {
				gdeckManager->current_deck = gdeckManager->pre_deck;
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
				if(prev_operation == BUTTON_DELETE_DECK) {
					int sel = mainGame->cbDBDecks->getSelected();
					if(gdeckManager->DeleteDeck(gdeckManager->current_deck, Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)))) {
						mainGame->cbDBDecks->removeItem(sel);
						int count = mainGame->cbDBDecks->getItemCount();
						if(sel >= count)
							sel = count - 1;
						mainGame->cbDBDecks->setSelected(sel);
						if(sel != -1)
							gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)));
						mainGame->stACMessage->setText(gDataManager->GetSysString(1338).c_str());
						mainGame->PopupElement(mainGame->wACMessage, 20);
						prev_deck = sel;
					}
				} else if(prev_operation == BUTTON_LEAVE_GAME) {
					Terminate();
				} else if(prev_operation == COMBOBOX_DBDECKS) {
					int sel = mainGame->cbDBDecks->getSelected();
					gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)));
					prev_deck = sel;
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
			auto StartFilterIfLongerThan = [&](int length) {
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
				if (filterList->whitelist) { // heuristic to help with restricted card pools
					mainGame->chkAnime->setChecked(true);
					mainGame->cbLimit->setSelected(4); // unlimited
				}
				StartFilter(true);
				break;
			}
			case COMBOBOX_DBDECKS: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0)
					gdeckManager->LoadDeck(Utils::ToPathString(mainGame->cbDBDecks->getItem(sel)));
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
				case 0: {
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
			if(!isroot)
				break;
			if(mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			click_pos = hovered_pos;
			dragx = event.MouseInput.X;
			dragy = event.MouseInput.Y;
			draging_pointer = gDataManager->GetCardData(hovered_code);
			if(!draging_pointer)
				break;
			if(hovered_pos == 4) {
				if(!event.MouseInput.Shift && !check_limit(draging_pointer))
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
				pushed = push_main(draging_pointer, hovered_seq, event.MouseInput.Shift);
			else if(hovered_pos == 2)
				pushed = push_extra(draging_pointer, hovered_seq + is_lastcard, event.MouseInput.Shift);
			else if(hovered_pos == 3)
				pushed = push_side(draging_pointer, hovered_seq + is_lastcard, event.MouseInput.Shift);
			else if(hovered_pos == 4 && !mainGame->is_siding)
				pushed = true;
			if(!pushed) {
				if(click_pos == 1)
					push_main(draging_pointer);
				else if(click_pos == 2)
					push_extra(draging_pointer);
				else if(click_pos == 3)
					push_side(draging_pointer);
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
					push_side(draging_pointer);
				} else if(click_pos == 2) {
					push_side(draging_pointer);
				} else if(click_pos == 3) {
					if(!push_extra(draging_pointer))
						push_main(draging_pointer);
				} else {
					push_side(draging_pointer);
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
				if(event.KeyInput.Control) {
					auto deck_string = event.KeyInput.Shift ? gdeckManager->ExportDeckCardNames(gdeckManager->current_deck) : gdeckManager->ExportDeckBase64(gdeckManager->current_deck);
					if(deck_string) {
						mainGame->device->getOSOperator()->copyToClipboard(deck_string);
						mainGame->stACMessage->setText(L"Deck copied");
					} else {
						mainGame->stACMessage->setText(L"Deck not copied");
					}
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case irr::KEY_KEY_V: {
				if(event.KeyInput.Control && !mainGame->HasFocus(irr::gui::EGUIET_EDIT_BOX)) {
					const wchar_t* deck_string = mainGame->device->getOSOperator()->getTextFromClipboard();
					if(deck_string && wcsncmp(L"ydke://", deck_string, sizeof(L"ydke://") / sizeof(wchar_t) - 1) == 0) {
						gdeckManager->ImportDeckBase64(gdeckManager->current_deck, deck_string);
					}
				}
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
		static std::wstring to_open_deck;
		switch(event.DropEvent.DropType) {
			case irr::DROP_FILE: {
				irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
				if(root->getElementFromPoint({ event.DropEvent.X, event.DropEvent.Y }) != root)
					break;
				to_open_deck = event.DropEvent.Text;
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
					gdeckManager->ImportDeckBase64(gdeckManager->current_deck, event.DropEvent.Text);
					return true;
				}
				std::wstringstream ss(Utils::ToUpperNoAccents<std::wstring>(event.DropEvent.Text));
				std::wstring to;
				while(std::getline(ss, to)) {
					(to = to.substr(to.find_first_not_of(L" \n\r\t")));
					to.erase(to.find_last_not_of(L" \n\r\t") + 1);
					int code = BufferIO::GetVal(to.c_str());
					CardDataC* pointer = nullptr;
					if(code && (pointer = gDataManager->GetCardData(code))) {
					} else {
						for(auto& card : gDataManager->cards) {
							auto name = Utils::ToUpperNoAccents<std::wstring>(card.second.GetStrings()->name);
							if(name == to) {
								pointer = &card.second._data;
								break;
							}
						}
					}
					if(!pointer)
						continue;
					mouse_pos.set(event.DropEvent.X, event.DropEvent.Y);
					is_draging = true;
					hovered_code = code;
					draging_pointer = pointer;
					GetHoveredCard();
					if(hovered_pos == 3)
						push_side(draging_pointer, hovered_seq + is_lastcard);
					else {
						push_main(draging_pointer, hovered_seq) || push_extra(draging_pointer, hovered_seq + is_lastcard);
					}
					is_draging = false;
				}
				return true;
			}
			case irr::DROP_END:	{
				if(to_open_deck.size()) {
					if(Utils::GetFileExtension(to_open_deck) == L"ydk" && gdeckManager->LoadDeck(Utils::ToPathString(to_open_deck))) {
						auto name = Utils::GetFileName(to_open_deck);
						mainGame->ebDeckname->setText(name.c_str());
						mainGame->cbDBDecks->setSelected(-1);
					}
					to_open_deck.clear();
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
			if(gdeckManager->current_deck.main.size() > 40)
				lx = (gdeckManager->current_deck.main.size() - 41) / 4 + 11;
			if(x >= 750)
				px = lx - 1;
			else
				px = (x - 314) * (lx - 1) / 436;
			hovered_seq = py * lx + px;
			if(hovered_seq >= (int)gdeckManager->current_deck.main.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = gdeckManager->current_deck.main[hovered_seq]->code;
			}
		} else if(y >= 466 && y <= 530) {
			int lx = gdeckManager->current_deck.extra.size();
			hovered_pos = 2;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)gdeckManager->current_deck.extra.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = gdeckManager->current_deck.extra[hovered_seq]->code;
				if(x >= 772)
					is_lastcard = 1;
			}
		} else if (y >= 564 && y <= 628) {
			int lx = gdeckManager->current_deck.side.size();
			hovered_pos = 3;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)gdeckManager->current_deck.side.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = gdeckManager->current_deck.side[hovered_seq]->code;
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
	filter_lm = static_cast<limitation_search_filters>(mainGame->cbLimit->getSelected());
	if(filter_type == 1) {
		filter_attrib = mainGame->cbAttribute->getItemData(mainGame->cbAttribute->getSelected());
		filter_race = mainGame->cbRace->getItemData(mainGame->cbRace->getSelected());
		filter_atk = parse_filter(mainGame->ebAttack->getText(), &filter_atktype);
		filter_def = parse_filter(mainGame->ebDefense->getText(), &filter_deftype);
		filter_lv = parse_filter(mainGame->ebStar->getText(), &filter_lvtype);
		filter_scl = parse_filter(mainGame->ebScale->getText(), &filter_scltype);
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
		int trycode = BufferIO::GetVal(term.c_str());
		CardDataC* data = nullptr;
		if(trycode && (data = gDataManager->GetCardData(trycode))) {
			searched_terms[term] = { data };
			continue;
		}
		std::vector<std::wstring> tokens;
		int modif = 0;
		if(!term.empty()) {
			size_t start = 0;
			if(term.size() >= 2 && memcmp(L"!!", term.data(), sizeof(wchar_t) * 2) == 0) {
				modif |= SEARCH_MODIFIER::SEARCH_MODIFIER_NEGATIVE_LOOKUP;
				start += 2;
			}
			if(term.size() + start >= 1) {
				if(term[start] == L'@') {
					modif |= SEARCH_MODIFIER::SEARCH_MODIFIER_ARCHETYPE_ONLY;
					start++;
				}
				else if(term[start] == L'$') {
					modif |= SEARCH_MODIFIER::SEARCH_MODIFIER_NAME_ONLY;
					start++;
				}
			}
			tokens = Utils::TokenizeString<std::wstring>(term.data() + start, L"*");
		}
		std::vector<unsigned int> set_code = gDataManager->GetSetCode(tokens);
		if(tokens.empty())
			tokens.push_back(L"");
		wchar_t checkterm = term.size() ? term.front() : 0;
		std::vector<CardDataC*> result;
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
bool DeckBuilder::CheckCard(CardDataM* data, SEARCH_MODIFIER modifier, const std::vector<std::wstring>& tokens, const std::vector<unsigned int>& set_code) {
	if(data->_data.type & TYPE_TOKEN  || data->_data.ot & SCOPE_HIDDEN || ((data->_data.ot & SCOPE_OFFICIAL) != data->_data.ot && !mainGame->chkAnime->isChecked()))
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
	}
	if(filter_effect && !(data->_data.category & filter_effect))
		return false;
	if(filter_marks && (data->_data.link_marker & filter_marks) != filter_marks)
		return false;
	if(filter_lm) {
		unsigned int limitcode = data->_data.code;
		auto flit = filterList->content.find(limitcode);
		if(flit == filterList->content.end())
			limitcode = data->_data.alias ? data->_data.alias : data->_data.code;
		if(filter_lm <= LIMITATION_FILTER_SEMI_LIMITED && ((!filterList->content.count(limitcode) && !filterList->whitelist) || (filterList->content[limitcode] != filter_lm - 1)))
			return false;
		if(filter_lm == LIMITATION_FILTER_UNLIMITED) {
			if(filterList->whitelist) {
				if(!filterList->content.count(limitcode) || filterList->content[limitcode] < 3)
					return false;
			} else if(filterList->content.count(limitcode) && filterList->content[limitcode] < 3)
				return false;
		}
		if(filter_lm == LIMITATION_FILTER_OCG && data->_data.ot != SCOPE_OCG)
			return false;
		if(filter_lm == LIMITATION_FILTER_TCG && data->_data.ot != SCOPE_TCG)
			return false;
		if(filter_lm == LIMITATION_FILTER_TCG_OCG && data->_data.ot != SCOPE_OCG_TCG)
			return false;
		if(filter_lm == LIMITATION_FILTER_PRERELEASE && !(data->_data.ot & SCOPE_PRERELEASE))
			return false;
		if (filter_lm == LIMITATION_FILTER_SPEED && !(data->_data.ot & SCOPE_SPEED))
			return false;
		if (filter_lm == LIMITATION_FILTER_RUSH && !(data->_data.ot & SCOPE_RUSH))
			return false;
		if(filter_lm == LIMITATION_FILTER_ANIME && data->_data.ot != SCOPE_ANIME)
			return false;
		if(filter_lm == LIMITATION_FILTER_ILLEGAL && data->_data.ot != SCOPE_ILLEGAL)
			return false;
		if(filter_lm == LIMITATION_FILTER_VIDEOGAME && data->_data.ot != SCOPE_VIDEO_GAME)
			return false;
		if(filter_lm == LIMITATION_FILTER_CUSTOM && data->_data.ot != SCOPE_CUSTOM)
			return false;
	}
	if(tokens.size()) {
		const auto checkNeg = [negative = !!(modifier & SEARCH_MODIFIER::SEARCH_MODIFIER_NEGATIVE_LOOKUP)] (bool res) -> bool {
			if(negative)
				return !res;
			return res;
		};
		if(modifier & SEARCH_MODIFIER::SEARCH_MODIFIER_NAME_ONLY) {
			return checkNeg(Utils::ContainsSubstring(data->GetStrings()->name, tokens, true));
		} else if(modifier & SEARCH_MODIFIER::SEARCH_MODIFIER_ARCHETYPE_ONLY) {
			if(set_code.empty() && tokens.size() > 0 && tokens.front() != L"")
				return checkNeg(false);
			return checkNeg(check_set_code(data->_data, set_code));
		} else {
			return checkNeg((set_code.size() && check_set_code(data->_data, set_code)) || Utils::ContainsSubstring(data->GetStrings()->name, tokens, true)
					|| Utils::ContainsSubstring(data->GetStrings()->text, tokens, true));
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
		if(searched_terms.find(Utils::ToUpperNoAccents(gDataManager->GetName((*it)->code))) != searched_terms.end()) {
			std::iter_swap(left, it);
			++left;
		}
	}
	switch(mainGame->cbSortType->getSelected()) {
	case 0:
		std::sort(left, results.end(), ClientCard::deck_sort_lv);
		break;
	case 1:
		std::sort(left, results.end(), ClientCard::deck_sort_atk);
		break;
	case 2:
		std::sort(left, results.end(), ClientCard::deck_sort_def);
		break;
	case 3:
		std::sort(left, results.end(), ClientCard::deck_sort_name);
		break;
	}
}
bool DeckBuilder::push_main(CardDataC* pointer, int seq, bool forced) {
	if(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK) && pointer->type != (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = gdeckManager->current_deck.main;
	if(!mainGame->is_siding && !forced && (int)container.size() >= 60)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_extra(CardDataC* pointer, int seq, bool forced) {
	if(!(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) || pointer->type == (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = gdeckManager->current_deck.extra;
	if(!mainGame->is_siding && !forced && (int)container.size() >= 15)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_side(CardDataC* pointer, int seq, bool forced) {
	auto& container = gdeckManager->current_deck.side;
	if(!mainGame->is_siding && !forced && (int)container.size() >= 15)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
void DeckBuilder::pop_main(int seq) {
	auto& container = gdeckManager->current_deck.main;
	container.erase(container.begin() + seq);
	GetHoveredCard();
}
void DeckBuilder::pop_extra(int seq) {
	auto& container = gdeckManager->current_deck.extra;
	container.erase(container.begin() + seq);
	GetHoveredCard();
}
void DeckBuilder::pop_side(int seq) {
	auto& container = gdeckManager->current_deck.side;
	container.erase(container.begin() + seq);
	GetHoveredCard();
}
bool DeckBuilder::check_limit(CardDataC* pointer) {
	unsigned int limitcode = pointer->alias ? pointer->alias : pointer->code;
	int found = 0;
	std::unordered_set<int> limit_codes;
	auto f=[&](int code, int alias){
		if(filterList->content.find(code) != filterList->content.end())
			limit_codes.insert(code);
		else
			limit_codes.insert(alias);
	};
	auto f2 = [&](std::vector<CardDataC*>& list) {
		for(auto& it : list) {
			if(it->code == limitcode || it->alias == limitcode) {
				f(it->code, it->alias);
				found++;
			}
		}
	};
	f(pointer->code, limitcode);
	f2(gdeckManager->current_deck.main);
	f2(gdeckManager->current_deck.extra);
	f2(gdeckManager->current_deck.side);
	int limit = 3;
	for(int code : limit_codes) {
		auto flit = filterList->content.find(code);
		if(flit != filterList->content.end())
			limit = std::min(limit,flit->second);
	}
	if(limit_codes.empty() && filterList->whitelist)
		limit = 0;
	return limit > found;
}
}
