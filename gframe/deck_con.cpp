#include "config.h"
#include "deck_con.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "image_manager.h"
#include "game.h"
#include "duelclient.h"
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

static bool check_set_code(CardDataC* data, std::vector<unsigned int>& setcodes) {
	unsigned long long card_setcode = data->setcode;
	if (data->alias) {
		auto aptr = dataManager._datas.find(data->alias);
		if (aptr != dataManager._datas.end())
			card_setcode = aptr->second->setcode;
	}
	if(setcodes.empty())
		return !!card_setcode;
	for(auto& set_code : setcodes) {
		auto sc = card_setcode;
		int settype = set_code & 0xfff;
		int setsubtype = set_code & 0xf000;
		while(sc) {
			if((sc & 0xfff) == settype && (sc & 0xf000 & setsubtype) == setsubtype)
				return true;
			sc = sc >> 16;
		}
	}
	return false;
}

void DeckBuilder::Initialize() {
	mainGame->is_building = true;
	mainGame->is_siding = false;
	mainGame->ClearCardInfo();
	mainGame->mTopMenu->setVisible(false);
	mainGame->wInfos->setVisible(true);
	mainGame->wCardImg->setVisible(true);
	mainGame->wDeckEdit->setVisible(true);
	mainGame->wFilter->setVisible(true);
	mainGame->wSort->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(dataManager.GetSysString(1306).c_str());
	mainGame->btnSideOK->setVisible(false);
	mainGame->btnSideShuffle->setVisible(false);
	mainGame->btnSideSort->setVisible(false);
	mainGame->btnSideReload->setVisible(false);
	filterList = &deckManager._lfList[0];
	mainGame->cbDBLFList->setSelected(0);
	ClearSearch();
	mouse_pos.set(0, 0);
	hovered_code = 0;
	hovered_pos = 0;
	hovered_seq = -1;
	is_lastcard = 0;
	is_draging = false;
	prev_deck = mainGame->cbDBDecks->getSelected();
	prev_operation = 0;
	scroll_pos = 0;
	mainGame->SetMesageWindow();
	mainGame->device->setEventReceiver(this);
}
void DeckBuilder::Terminate() {
	mainGame->is_building = false;
	mainGame->ClearCardInfo();
	mainGame->mTopMenu->setVisible(true);
	mainGame->wDeckEdit->setVisible(false);
	mainGame->wCategories->setVisible(false);
	mainGame->wFilter->setVisible(false);
	mainGame->wSort->setVisible(false);
	mainGame->wCardImg->setVisible(false);
	mainGame->wInfos->setVisible(false);
	mainGame->btnLeaveGame->setVisible(false);
	mainGame->PopupElement(mainGame->wMainMenu);
	mainGame->device->setEventReceiver(&mainGame->menuHandler);
	mainGame->wACMessage->setVisible(false);
	mainGame->ClearTextures();
	mainGame->ClearCardInfo(0);
	mainGame->scrFilter->setVisible(false);
	mainGame->SetMesageWindow();
	int sel = mainGame->cbDBDecks->getSelected();
	if(sel >= 0)
		mainGame->gameConf.lastdeck = mainGame->cbDBDecks->getItem(sel);
	if(exit_on_return)
		mainGame->device->closeDevice();
}
bool DeckBuilder::OnEvent(const irr::SEvent& event) {
	if(mainGame->dField.OnCommonEvent(event))
		return false;
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		if(mainGame->wCategories->isVisible() && id != BUTTON_CATEGORY_OK)
			break;
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO)
			break;
		if(mainGame->wLinkMarks->isVisible() && id != BUTTON_MARKERS_OK)
			break;
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_DECK: {
				deckManager.current_deck.main.clear();
				deckManager.current_deck.extra.clear();
				deckManager.current_deck.side.clear();
				break;
			}
			case BUTTON_SORT_DECK: {
				std::sort(deckManager.current_deck.main.begin(), deckManager.current_deck.main.end(), ClientCard::deck_sort_lv);
				std::sort(deckManager.current_deck.extra.begin(), deckManager.current_deck.extra.end(), ClientCard::deck_sort_lv);
				std::sort(deckManager.current_deck.side.begin(), deckManager.current_deck.side.end(), ClientCard::deck_sort_lv);
				break;
			}
			case BUTTON_SHUFFLE_DECK: {
				std::random_shuffle(deckManager.current_deck.main.begin(), deckManager.current_deck.main.end());
				break;
			}
			case BUTTON_SAVE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0 && deckManager.SaveDeck(deckManager.current_deck, mainGame->cbDBDecks->getItem(sel))) {
					mainGame->stACMessage->setText(dataManager.GetSysString(1335).c_str());
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
				if(sel >= 0)
					mainGame->cbDBDecks->setSelected(sel);
				else {
					mainGame->cbDBDecks->addItem(dname);
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->getItemCount() - 1);
				}
				if(deckManager.SaveDeck(deckManager.current_deck, dname)) {
					mainGame->stACMessage->setText(dataManager.GetSysString(1335).c_str());
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_DELETE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.lock();
				mainGame->stQMessage->setText(fmt::format(L"{}\n{}", mainGame->cbDBDecks->getItem(sel), dataManager.GetSysString(1337)).c_str());
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.unlock();
				prev_operation = id;
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
				if(deckManager.current_deck.main.size() != deckManager.pre_deck.main.size() || deckManager.current_deck.extra.size() != deckManager.pre_deck.extra.size()
				        || deckManager.current_deck.side.size() != deckManager.pre_deck.side.size()) {
					mainGame->PopupMessage(dataManager.GetSysString(1410));
					break;
				}
				mainGame->ClearCardInfo();
				char deckbuf[1024];
				char* pdeck = deckbuf;
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
				for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->code);
				for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->code);
				for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->code);
				DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
				break;
			}
			case BUTTON_SIDE_RELOAD: {
				deckManager.current_deck = deckManager.pre_deck;
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
					if(deckManager.DeleteDeck(deckManager.current_deck, mainGame->cbDBDecks->getItem(sel))) {
						mainGame->cbDBDecks->removeItem(sel);
						int count = mainGame->cbDBDecks->getItemCount();
						if(sel >= count)
							sel = count - 1;
						mainGame->cbDBDecks->setSelected(sel);
						if(sel != -1)
							deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
						mainGame->stACMessage->setText(dataManager.GetSysString(1338).c_str());
						mainGame->PopupElement(mainGame->wACMessage, 20);
						prev_deck = sel;
					}
				} else if(prev_operation == BUTTON_LEAVE_GAME) {
					Terminate();
				} else if(prev_operation == COMBOBOX_DBDECKS) {
					int sel = mainGame->cbDBDecks->getSelected();
					deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
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
			case EDITBOX_KEYWORD: {
				StartFilter();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_EDITBOX_CHANGED: {
			switch (id) {
			case EDITBOX_KEYWORD: {
				std::wstring filter = mainGame->ebCardName->getText();
				if (filter.size() > 2) {
					StartFilter();
				}
				break;
			}
			case EDITBOX_DECK_NAME: {
				mainGame->ValidateName(event.GUIEvent.Caller);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_DBLFLIST: {
				filterList = &deckManager._lfList[mainGame->cbDBLFList->getSelected()];
				StartFilter(true);
				break;
			}
			case COMBOBOX_DBDECKS: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel >= 0)
					deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
				prev_deck = sel;
				break;
			}
			case COMBOBOX_MAINTYPE: {
				mainGame->cbCardType2->setSelected(0);
				mainGame->cbAttribute->setSelected(0);
				mainGame->cbRace->setSelected(0);
				mainGame->ebAttack->setText(L"");
				mainGame->ebDefense->setText(L"");
				mainGame->ebStar->setText(L"");
				mainGame->ebScale->setText(L"");
				switch(mainGame->cbCardType->getSelected()) {
				case 0: {
					mainGame->cbCardType2->setEnabled(false);
					mainGame->cbCardType2->setSelected(0);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					break;
				}
				case 1: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(true);
					mainGame->cbAttribute->setEnabled(true);
					mainGame->ebAttack->setEnabled(true);
					mainGame->ebDefense->setEnabled(true);
					mainGame->ebStar->setEnabled(true);
					mainGame->ebScale->setEnabled(true);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080).c_str(), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054).c_str(), TYPE_MONSTER + TYPE_NORMAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1055).c_str(), TYPE_MONSTER + TYPE_EFFECT);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1056).c_str(), TYPE_MONSTER + TYPE_FUSION);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1057).c_str(), TYPE_MONSTER + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1063).c_str(), TYPE_MONSTER + TYPE_SYNCHRO);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1073).c_str(), TYPE_MONSTER + TYPE_XYZ);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1074).c_str(), TYPE_MONSTER + TYPE_PENDULUM);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1076).c_str(), TYPE_MONSTER + TYPE_LINK);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1075).c_str(), TYPE_MONSTER + TYPE_SPSUMMON);
					mainGame->cbCardType2->addItem((dataManager.GetSysString(1054) + L"|" + dataManager.GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_TUNER);
					mainGame->cbCardType2->addItem((dataManager.GetSysString(1054) + L"|" + dataManager.GetSysString(1074)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_PENDULUM);
					mainGame->cbCardType2->addItem((dataManager.GetSysString(1063) + L"|" + dataManager.GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_SYNCHRO + TYPE_TUNER);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1062).c_str(), TYPE_MONSTER + TYPE_TUNER);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1061).c_str(), TYPE_MONSTER + TYPE_DUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1060).c_str(), TYPE_MONSTER + TYPE_UNION);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1059).c_str(), TYPE_MONSTER + TYPE_SPIRIT);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1071).c_str(), TYPE_MONSTER + TYPE_FLIP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1072).c_str(), TYPE_MONSTER + TYPE_TOON);
					break;
				}
				case 2: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080).c_str(), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054).c_str(), TYPE_SPELL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1066).c_str(), TYPE_SPELL + TYPE_QUICKPLAY);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1067).c_str(), TYPE_SPELL + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1057).c_str(), TYPE_SPELL + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1068).c_str(), TYPE_SPELL + TYPE_EQUIP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1069).c_str(), TYPE_SPELL + TYPE_FIELD);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1076).c_str(), TYPE_SPELL + TYPE_LINK);
					break;
				}
				case 3: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefense->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->ebScale->setEnabled(false);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080).c_str(), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054).c_str(), TYPE_TRAP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1067).c_str(), TYPE_TRAP + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1070).c_str(), TYPE_TRAP + TYPE_COUNTER);
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
					mainGame->cbLimit->clear();
					mainGame->cbLimit->addItem(dataManager.GetSysString(1310).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1316).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1317).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1318).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1320).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1240).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1241).c_str());
					mainGame->cbLimit->addItem(dataManager.GetSysString(1242).c_str());
					if(mainGame->chkAnime->isChecked()) {
						mainGame->cbLimit->addItem(dataManager.GetSysString(1264).c_str());
						mainGame->cbLimit->addItem(dataManager.GetSysString(1265).c_str());
						mainGame->cbLimit->addItem(dataManager.GetSysString(1266).c_str());
						mainGame->cbLimit->addItem(dataManager.GetSysString(1267).c_str());
					}
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
		switch(event.MouseInput.Event) {
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
			if(root->getElementFromPoint(mouse_pos) != root)
				break;
			if(mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			click_pos = hovered_pos;
			dragx = event.MouseInput.X;
			dragy = event.MouseInput.Y;
			draging_pointer = dataManager.GetCardData(hovered_code);
			if(!draging_pointer)
				break;
			if(hovered_pos == 4) {
				if(!check_limit(draging_pointer))
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
			if(!is_draging) {
				mouse_pos.set(event.MouseInput.X, event.MouseInput.Y);
				GetHoveredCard();
				break;
			}
			bool pushed = false;
			if(hovered_pos == 1)
				pushed = push_main(draging_pointer, hovered_seq);
			else if(hovered_pos == 2)
				pushed = push_extra(draging_pointer, hovered_seq + is_lastcard);
			else if(hovered_pos == 3)
				pushed = push_side(draging_pointer, hovered_seq + is_lastcard);
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
			if(mainGame->is_siding) {
				if(is_draging)
					break;
				if(hovered_pos == 0 || hovered_seq == -1)
					break;
				auto pointer = dataManager.GetCardData(hovered_code);
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
					auto pointer = dataManager.GetCardData(hovered_code);
					if(!pointer)
						break;
					if(event.MouseInput.Shift)
						push_side(pointer);
					else {
						if (!check_limit(pointer))
							break;
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
			if (mainGame->is_siding)
				break;
			if (mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if (hovered_pos == 0 || hovered_seq == -1)
				break;
			if (is_draging)
				break;
			auto pointer = dataManager.GetCardData(hovered_code);
			if(!pointer || !check_limit(pointer))
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
			break;
		}
		default: break;
		}
		break;
	}
	default: break;
	}
	return false;
}
void DeckBuilder::GetHoveredCard() {
	irr::gui::IGUIElement* root = mainGame->env->getRootGUIElement();
	if(root->getElementFromPoint(mouse_pos) != root)
		return;
	position2di pos = mainGame->Resize(mouse_pos.X, mouse_pos.Y, true);
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
			if(deckManager.current_deck.main.size() > 40)
				lx = (deckManager.current_deck.main.size() - 41) / 4 + 11;
			if(x >= 750)
				px = lx - 1;
			else
				px = (x - 314) * (lx - 1) / 436;
			hovered_seq = py * lx + px;
			if(hovered_seq >= (int)deckManager.current_deck.main.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = deckManager.current_deck.main[hovered_seq]->code;
			}
		} else if(y >= 466 && y <= 530) {
			int lx = deckManager.current_deck.extra.size();
			hovered_pos = 2;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)deckManager.current_deck.extra.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = deckManager.current_deck.extra[hovered_seq]->code;
				if(x >= 772)
					is_lastcard = 1;
			}
		} else if (y >= 564 && y <= 628) {
			int lx = deckManager.current_deck.side.size();
			hovered_pos = 3;
			if(lx < 10)
				lx = 10;
			if(x >= 750)
				hovered_seq = lx - 1;
			else
				hovered_seq = (x - 314) * (lx - 1) / 436;
			if(hovered_seq >= (int)deckManager.current_deck.side.size()) {
				hovered_seq = -1;
				hovered_code = 0;
			} else {
				hovered_code = deckManager.current_deck.side[hovered_seq]->code;
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
}
void DeckBuilder::FilterCards(bool force_refresh) {
	results.clear();
	std::vector<std::wstring> searchterms;
	if(wcslen(mainGame->ebCardName->getText())) {
		searchterms = Game::TokenizeString<std::wstring>(Game::StringtoUpper(mainGame->ebCardName->getText()), L"+");
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
	for(auto term : searchterms) {
		int trycode = BufferIO::GetVal(term.c_str());
		if(trycode && dataManager.GetData(trycode, nullptr)) {
			searched_terms[term] = { dataManager.GetCardData(trycode) }; // verified by GetData()
			continue;
		}
		std::vector<std::wstring> tokens;
		if(!term.empty()) {
			if(term.front() == L'@' || term.front() == L'$') {
				if(term.size() > 1)
					tokens = Game::TokenizeString<std::wstring>(&term[1], L"*");
			} else {
				tokens = Game::TokenizeString<std::wstring>(term, L"*");
			}
		}
		std::vector<unsigned int> set_code = dataManager.GetSetCode(tokens);
		if(tokens.empty())
			tokens.push_back(L"");
		auto strpointer = dataManager._strings.begin();
		wchar_t checkterm = term.size() ? term.front() : 0;
		std::vector<CardDataC*> result;
		for(auto ptr = dataManager._datas.begin(); ptr != dataManager._datas.end(); ptr++, strpointer++) {
			if(CheckCard(ptr->second, strpointer->second, checkterm, tokens, set_code))
				result.push_back(ptr->second);
		}
		if(result.size())
			searched_terms[term] = result;
	}
	for(auto& res : searched_terms) {
		results.insert(results.end(), res.second.begin(), res.second.end());
	}
	SortList();
	auto ip = std::unique(results.begin(), results.end());
	results.resize(std::distance(results.begin(), ip));
	result_string = fmt::to_wstring(results.size());
	if(results.size() > 7) {
		scroll_pos = 0;
		mainGame->scrFilter->setVisible(true);
		mainGame->scrFilter->setMax((results.size() - 7) * DECK_SEARCH_SCROLL_STEP);
		mainGame->scrFilter->setPos(0);
	} else {
		scroll_pos = 0;
		mainGame->scrFilter->setVisible(false);
		mainGame->scrFilter->setPos(0);
	}
}
bool DeckBuilder::CheckCard(CardDataC* data, const CardString& text, const wchar_t& checkchar, std::vector<std::wstring>& tokens, std::vector<unsigned int>& set_code) {
	if(data->type & TYPE_TOKEN || (data->ot > 3 && !mainGame->chkAnime->isChecked()))
		return false;
	switch(filter_type) {
	case 1: {
		if(!(data->type & TYPE_MONSTER) || (data->type & filter_type2) != filter_type2)
			return false;
		if(filter_race && data->race != filter_race)
			return false;
		if(filter_attrib && data->attribute != filter_attrib)
			return false;
		if(filter_atktype) {
			if((filter_atktype == 1 && data->attack != filter_atk) || (filter_atktype == 2 && data->attack < filter_atk)
				|| (filter_atktype == 3 && data->attack <= filter_atk) || (filter_atktype == 4 && (data->attack > filter_atk || data->attack < 0))
				|| (filter_atktype == 5 && (data->attack >= filter_atk || data->attack < 0)) || (filter_atktype == 6 && data->attack != -2))
				return false;
		}
		if(filter_deftype) {
			if((filter_deftype == 1 && data->defense != filter_def) || (filter_deftype == 2 && data->defense < filter_def)
				|| (filter_deftype == 3 && data->defense <= filter_def) || (filter_deftype == 4 && (data->defense > filter_def || data->defense < 0))
				|| (filter_deftype == 5 && (data->defense >= filter_def || data->defense < 0)) || (filter_deftype == 6 && data->defense != -2)
				|| (data->type & TYPE_LINK))
				return false;
		}
		if(filter_lvtype) {
			if((filter_lvtype == 1 && data->level != filter_lv) || (filter_lvtype == 2 && data->level < filter_lv)
				|| (filter_lvtype == 3 && data->level <= filter_lv) || (filter_lvtype == 4 && data->level > filter_lv)
				|| (filter_lvtype == 5 && data->level >= filter_lv) || filter_lvtype == 6)
				return false;
		}
		if(filter_scltype) {
			if((filter_scltype == 1 && data->lscale != filter_scl) || (filter_scltype == 2 && data->lscale < filter_scl)
				|| (filter_scltype == 3 && data->lscale <= filter_scl) || (filter_scltype == 4 && (data->lscale > filter_scl || data->lscale == 0))
				|| (filter_scltype == 5 && (data->lscale >= filter_scl || data->lscale == 0)) || filter_scltype == 6
				|| !(data->type & TYPE_PENDULUM))
				return false;
		}
		break;
	}
	case 2: {
		if(!(data->type & TYPE_SPELL))
			return false;
		if(filter_type2 && data->type != filter_type2)
			return false;
		break;
	}
	case 3: {
		if(!(data->type & TYPE_TRAP))
			return false;
		if(filter_type2 && data->type != filter_type2)
			return false;
		break;
	}
	}
	if(filter_effect && !(data->category & filter_effect))
		return false;
	if(filter_marks && (data->link_marker & filter_marks) != filter_marks)
		return false;
	if(filter_lm) {
		unsigned int limitcode = data->code;
		auto flit = filterList->content.find(limitcode);
		if(flit == filterList->content.end())
			limitcode = data->alias ? data->alias : data->code;
		if(filter_lm <= LIMITATION_FILTER_SEMI_LIMITED && ((!filterList->content.count(limitcode) && !filterList->whitelist) || (filterList->content[limitcode] != filter_lm - 1)))
			return false;
		if(filter_lm == LIMITATION_FILTER_UNLIMITED) {
			if(filterList->whitelist) {
				if(!filterList->content.count(limitcode) || filterList->content[limitcode] < 3)
					return false;
			} else if(filterList->content.count(limitcode) && filterList->content[limitcode] < 3)
				return false;
		}
		if(filter_lm == LIMITATION_FILTER_OCG && data->ot != 0x1)
			return false;
		if(filter_lm == LIMITATION_FILTER_TCG && data->ot != 0x2)
			return false;
		if(filter_lm == LIMITATION_FILTER_TCG_OCG && data->ot != 0x3)
			return false;
		if(filter_lm == LIMITATION_FILTER_ANIME && data->ot != 0x4)
			return false;
		if(filter_lm == LIMITATION_FILTER_ILLEGAL && data->ot != 0x8)
			return false;
		if(filter_lm == LIMITATION_FILTER_VIDEOGAME && data->ot != 0x10)
			return false;
		if(filter_lm == LIMITATION_FILTER_CUSTOM && data->ot != 0x20)
			return false;
	}
	if(tokens.size()) {
		if(checkchar == L'$') {
			return Game::CompareStrings(text.name, tokens, true);
		} else if(checkchar == L'@') {
			if(set_code.empty() && tokens.size() > 0 && tokens.front() != L"")
				return false;
			return check_set_code(data, set_code);
		} else {
			return (set_code.size() && check_set_code(data, set_code)) || Game::CompareStrings(text.name, tokens, true) || Game::CompareStrings(text.text, tokens, true);
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
	searched_terms.clear();
	ClearFilter();
	results.clear();
	result_string = L"0";
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
		if(searched_terms.find(Game::StringtoUpper(dataManager.GetName((*it)->code))) != searched_terms.end()) {
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
bool DeckBuilder::push_main(CardDataC* pointer, int seq) {
	if(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK) && pointer->type != (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = deckManager.current_deck.main;
	if(!mainGame->is_siding && (int)container.size() >= 60)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_extra(CardDataC* pointer, int seq) {
	if(!(pointer->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)) || pointer->type == (TYPE_SPELL | TYPE_LINK))
		return false;
	auto& container = deckManager.current_deck.extra;
	if(!mainGame->is_siding && (int)container.size() >= 15)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_side(CardDataC* pointer, int seq) {
	auto& container = deckManager.current_deck.side;
	if(!mainGame->is_siding && (int)container.size() >= 15)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	GetHoveredCard();
	return true;
}
void DeckBuilder::pop_main(int seq) {
	auto& container = deckManager.current_deck.main;
	container.erase(container.begin() + seq);
	GetHoveredCard();
}
void DeckBuilder::pop_extra(int seq) {
	auto& container = deckManager.current_deck.extra;
	container.erase(container.begin() + seq);
	GetHoveredCard();
}
void DeckBuilder::pop_side(int seq) {
	auto& container = deckManager.current_deck.side;
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
	f2(deckManager.current_deck.main);
	f2(deckManager.current_deck.extra);
	f2(deckManager.current_deck.side);
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
