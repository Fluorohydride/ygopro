#include "config.h"
#include "deck_con.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "image_manager.h"
#include "game.h"
#include "duelclient.h"
#include <algorithm>

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

static bool check_set_code(const CardDataC& data, int set_code) {
	unsigned long long sc = data.setcode;
	if (data.alias) {
		auto aptr = dataManager._datas.find(data.alias);
		if (aptr != dataManager._datas.end())
			sc = aptr->second.setcode;
	}
	bool res = false;
	int settype = set_code & 0xfff;
	int setsubtype = set_code & 0xf000;
	while (sc) {
		if ((sc & 0xfff) == settype && (sc & 0xf000 & setsubtype) == setsubtype)
			res = true;
		sc = sc >> 16;
	}
	return res;
}

void DeckBuilder::Initialize() {
	mainGame->is_building = true;
	mainGame->is_siding = false;
	mainGame->wInfos->setVisible(true);
	mainGame->wCardImg->setVisible(true);
	mainGame->wDeckEdit->setVisible(true);
	mainGame->wFilter->setVisible(true);
	mainGame->wSort->setVisible(true);
	mainGame->btnLeaveGame->setVisible(true);
	mainGame->btnLeaveGame->setText(dataManager.GetSysString(1306));
	mainGame->btnSideOK->setVisible(false);
	filterList = deckManager._lfList[0].content;
	mainGame->cbDBLFList->setSelected(0);
	ClearSearch();
	mouse_pos.set(0, 0);
	hovered_code = 0;
	hovered_pos = 0;
	hovered_seq = -1;
	is_lastcard = 0;
	is_draging = false;
	is_starting_dragging = false;
	prev_deck = mainGame->cbDBDecks->getSelected();
	prev_operation = 0;
	is_modified = false;
	mainGame->device->setEventReceiver(this);
}
void DeckBuilder::Terminate() {
	mainGame->is_building = false;
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
	mainGame->scrFilter->setVisible(false);
	int sel = mainGame->cbDBDecks->getSelected();
	if(sel >= 0)
		BufferIO::CopyWStr(mainGame->cbDBDecks->getItem(sel), mainGame->gameConf.lastdeck, 64);
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
				mainGame->gMutex.Lock();
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)dataManager.GetSysString(1339));
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.Unlock();
				prev_operation = id;
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
					mainGame->stACMessage->setText(dataManager.GetSysString(1335));
					mainGame->PopupElement(mainGame->wACMessage, 20);
					is_modified = false;
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
					mainGame->stACMessage->setText(dataManager.GetSysString(1335));
					mainGame->PopupElement(mainGame->wACMessage, 20);
					is_modified = false;
				}
				break;
			}
			case BUTTON_DELETE_DECK: {
				int sel = mainGame->cbDBDecks->getSelected();
				if(sel == -1)
					break;
				mainGame->gMutex.Lock();
				wchar_t textBuffer[256];
				myswprintf(textBuffer, L"%ls\n%ls", mainGame->cbDBDecks->getItem(sel), dataManager.GetSysString(1337));
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)textBuffer);
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.Unlock();
				prev_operation = id;
				break;
			}
			case BUTTON_LEAVE_GAME: {
				if(is_modified && !mainGame->chkIgnoreDeckChanges->isChecked()) {
					mainGame->gMutex.Lock();
					mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)dataManager.GetSysString(1356));
					mainGame->PopupElement(mainGame->wQuery);
					mainGame->gMutex.Unlock();
					prev_operation = id;
					break;
				}
				Terminate();
				break;
			}
			case BUTTON_EFFECT_FILTER: {
				mainGame->PopupElement(mainGame->wCategories);
				break;
			}
			case BUTTON_START_FILTER: {
				StartFilter();
				if(!mainGame->gameConf.separate_clear_button)
					ClearFilter();
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
				if(deckManager.current_deck.main.size() != pre_mainc || deckManager.current_deck.extra.size() != pre_extrac
				        || deckManager.current_deck.side.size() != pre_sidec) {
					mainGame->env->addMessageBox(L"", dataManager.GetSysString(1410));
					break;
				}
				mainGame->imgCard->setImage(imageManager.tCover[0]);
				char deckbuf[1024];
				char* pdeck = deckbuf;
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
				for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->first);
				for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->first);
				for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
					BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->first);
				DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
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
				if(prev_operation == BUTTON_CLEAR_DECK) {
					deckManager.current_deck.main.clear();
					deckManager.current_deck.extra.clear();
					deckManager.current_deck.side.clear();
				} else if(prev_operation == BUTTON_DELETE_DECK) {
					int sel = mainGame->cbDBDecks->getSelected();
					if(deckManager.DeleteDeck(deckManager.current_deck, mainGame->cbDBDecks->getItem(sel))) {
						mainGame->cbDBDecks->removeItem(sel);
						int count = mainGame->cbDBDecks->getItemCount();
						if(sel >= count)
							sel = count - 1;
						mainGame->cbDBDecks->setSelected(sel);
						if(sel != -1)
							deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
						mainGame->stACMessage->setText(dataManager.GetSysString(1338));
						mainGame->PopupElement(mainGame->wACMessage, 20);
						prev_deck = sel;
						is_modified = false;
					}
				} else if(prev_operation == BUTTON_LEAVE_GAME) {
					Terminate();
				} else if(prev_operation == COMBOBOX_DBDECKS) {
					int sel = mainGame->cbDBDecks->getSelected();
					deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
					prev_deck = sel;
					is_modified = false;
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
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
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
			switch(id) {
			case EDITBOX_KEYWORD: {
				if(mainGame->gameConf.auto_search_limit >= 0 && (wcslen(mainGame->ebCardName->getText()) >= mainGame->gameConf.auto_search_limit))
					StartFilter();
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_DBLFLIST: {
				filterList = deckManager._lfList[mainGame->cbDBLFList->getSelected()].content;
				break;
			}
			case COMBOBOX_DBDECKS: {
				if(is_modified && !mainGame->chkIgnoreDeckChanges->isChecked()) {
					mainGame->gMutex.Lock();
					mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)dataManager.GetSysString(1356));
					mainGame->PopupElement(mainGame->wQuery);
					mainGame->gMutex.Unlock();
					prev_operation = id;
					break;
				}
				int sel = mainGame->cbDBDecks->getSelected();
				deckManager.LoadDeck(mainGame->cbDBDecks->getItem(sel));
				prev_deck = sel;
				is_modified = false;
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
					wchar_t normaltuner[32];
					wchar_t normalpen[32];
					wchar_t syntuner[32];
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(true);
					mainGame->cbAttribute->setEnabled(true);
					mainGame->ebAttack->setEnabled(true);
					mainGame->ebDefense->setEnabled(true);
					mainGame->ebStar->setEnabled(true);
					mainGame->ebScale->setEnabled(true);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054), TYPE_MONSTER + TYPE_NORMAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1055), TYPE_MONSTER + TYPE_EFFECT);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1056), TYPE_MONSTER + TYPE_FUSION);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1057), TYPE_MONSTER + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1063), TYPE_MONSTER + TYPE_SYNCHRO);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1073), TYPE_MONSTER + TYPE_XYZ);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1074), TYPE_MONSTER + TYPE_PENDULUM);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1076), TYPE_MONSTER + TYPE_LINK);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1075), TYPE_MONSTER + TYPE_SPSUMMON);
					myswprintf(normaltuner, L"%ls|%ls", dataManager.GetSysString(1054), dataManager.GetSysString(1062));
					mainGame->cbCardType2->addItem(normaltuner, TYPE_MONSTER + TYPE_NORMAL + TYPE_TUNER);
					myswprintf(normalpen, L"%ls|%ls", dataManager.GetSysString(1054), dataManager.GetSysString(1074));
					mainGame->cbCardType2->addItem(normalpen, TYPE_MONSTER + TYPE_NORMAL + TYPE_PENDULUM);
					myswprintf(syntuner, L"%ls|%ls", dataManager.GetSysString(1063), dataManager.GetSysString(1062));
					mainGame->cbCardType2->addItem(syntuner, TYPE_MONSTER + TYPE_SYNCHRO + TYPE_TUNER);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1062), TYPE_MONSTER + TYPE_TUNER);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1061), TYPE_MONSTER + TYPE_DUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1060), TYPE_MONSTER + TYPE_UNION);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1059), TYPE_MONSTER + TYPE_SPIRIT);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1071), TYPE_MONSTER + TYPE_FLIP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1072), TYPE_MONSTER + TYPE_TOON);
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
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054), TYPE_SPELL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1066), TYPE_SPELL + TYPE_QUICKPLAY);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1067), TYPE_SPELL + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1057), TYPE_SPELL + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1068), TYPE_SPELL + TYPE_EQUIP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1069), TYPE_SPELL + TYPE_FIELD);
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
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1080), 0);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1054), TYPE_TRAP);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1067), TYPE_TRAP + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(dataManager.GetSysString(1070), TYPE_TRAP + TYPE_COUNTER);
					break;
				}
				}
				break;
			}
			case COMBOBOX_SORTTYPE: {
				SortList();
				mainGame->env->setFocus(0);
				break;
			}
			case COMBOBOX_SECONDTYPE: {
				if (mainGame->cbCardType->getSelected() == 1) {
					if (mainGame->cbCardType2->getSelected() == 8) {
						mainGame->ebDefense->setEnabled(false);
						mainGame->ebDefense->setText(L"");
					} else {
						mainGame->ebDefense->setEnabled(true);
					}
				}
				break;
			}
			}
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
			draging_pointer = dataManager.GetCodePointer(hovered_code);
			if(draging_pointer == dataManager._datas.end())
				break;
			if(hovered_pos == 4) {
				if(!check_limit(draging_pointer))
					break;
			}
			is_starting_dragging = true;
			break;
		}
		case irr::EMIE_LMOUSE_LEFT_UP: {
			is_starting_dragging = false;
			if(!is_draging)
				break;
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
				auto pointer = dataManager.GetCodePointer(hovered_code);
				if(pointer == dataManager._datas.end())
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
					auto pointer = dataManager.GetCodePointer(hovered_code);
					if(pointer == dataManager._datas.end())
						break;
					if(!check_limit(pointer))
						break;
					if(!push_extra(pointer) && !push_main(pointer))
						push_side(pointer);
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
			auto pointer = dataManager.GetCodePointer(hovered_code);
			if(!check_limit(pointer))
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
			if(is_starting_dragging) {
				is_draging = true;
				if(hovered_pos == 1)
					pop_main(hovered_seq);
				else if(hovered_pos == 2)
					pop_extra(hovered_seq);
				else if(hovered_pos == 3)
					pop_side(hovered_seq);
				is_starting_dragging = false;
			}
			mouse_pos.set(event.MouseInput.X, event.MouseInput.Y);
			GetHoveredCard();
			break;
		}
		case irr::EMIE_MOUSE_WHEEL: {
			if(!mainGame->scrFilter->isVisible())
				break;
			if(event.MouseInput.Wheel < 0) {
				if(mainGame->scrFilter->getPos() < mainGame->scrFilter->getMax())
					mainGame->scrFilter->setPos(mainGame->scrFilter->getPos() + 1);
			} else {
				if(mainGame->scrFilter->getPos() > 0)
					mainGame->scrFilter->setPos(mainGame->scrFilter->getPos() - 1);
			}
			GetHoveredCard();
			break;
		}
		default: break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
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
	int x = mouse_pos.X;
	int y = mouse_pos.Y;
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
				hovered_code = deckManager.current_deck.main[hovered_seq]->first;
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
				hovered_code = deckManager.current_deck.extra[hovered_seq]->first;
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
				hovered_code = deckManager.current_deck.side[hovered_seq]->first;
				if(x >= 772)
					is_lastcard = 1;
			}
		}
	} else if(x >= 810 && x <= 995 && y >= 165 && y <= 626) {
		hovered_pos = 4;
		hovered_seq = (y - 165) / 66;
		int pos = mainGame->scrFilter->getPos() + hovered_seq;
		if(pos >= (int)results.size()) {
			hovered_seq = -1;
			hovered_code = 0;
		} else {
			hovered_code = results[pos]->first;
		}
	}
	if(is_draging) {
		dragx = x;
		dragy = y;
	}
	if(!is_draging && pre_code != hovered_code) {
		if(hovered_code)
			mainGame->ShowCardInfo(hovered_code);
		if(pre_code)
			imageManager.RemoveTexture(pre_code);
	}
}
void DeckBuilder::StartFilter() {
	filter_type = mainGame->cbCardType->getSelected();
	filter_type2 = mainGame->cbCardType2->getItemData(mainGame->cbCardType2->getSelected());
	filter_lm = mainGame->cbLimit->getSelected();
	if(filter_type == 1) {
		filter_attrib = mainGame->cbAttribute->getItemData(mainGame->cbAttribute->getSelected());
		filter_race = mainGame->cbRace->getItemData(mainGame->cbRace->getSelected());
		filter_atk = parse_filter(mainGame->ebAttack->getText(), &filter_atktype);
		filter_def = parse_filter(mainGame->ebDefense->getText(), &filter_deftype);
		filter_lv = parse_filter(mainGame->ebStar->getText(), &filter_lvtype);
		filter_scl = parse_filter(mainGame->ebScale->getText(), &filter_scltype);
	}
	FilterCards();
}
void DeckBuilder::FilterCards() {
	results.clear();
	const wchar_t* pstr = mainGame->ebCardName->getText();
	int trycode = BufferIO::GetVal(pstr);
	if(dataManager.GetData(trycode, 0)) {
		auto ptr = dataManager.GetCodePointer(trycode);	// verified by GetData()
		results.push_back(ptr);
		mainGame->scrFilter->setVisible(false);
		mainGame->scrFilter->setPos(0);
		myswprintf(result_string, L"%d", results.size());
		return;
	}
	unsigned int set_code = 0;
	if(pstr[0] == L'@')
		set_code = dataManager.GetSetCode(&pstr[1]);
	else
		set_code = dataManager.GetSetCode(&pstr[0]);
	if(pstr[0] == 0 || (pstr[0] == L'$' && pstr[1] == 0) || (pstr[0] == L'@' && pstr[1] == 0))
		pstr = 0;
	auto strpointer = dataManager._strings.begin();
	for(code_pointer ptr = dataManager._datas.begin(); ptr != dataManager._datas.end(); ++ptr, ++strpointer) {
		const CardDataC& data = ptr->second;
		const CardString& text = strpointer->second;
		if(data.type & TYPE_TOKEN)
			continue;
		switch(filter_type) {
		case 1: {
			if(!(data.type & TYPE_MONSTER) || (data.type & filter_type2) != filter_type2)
				continue;
			if(filter_race && data.race != filter_race)
				continue;
			if(filter_attrib && data.attribute != filter_attrib)
				continue;
			if(filter_atktype) {
				if((filter_atktype == 1 && data.attack != filter_atk) || (filter_atktype == 2 && data.attack < filter_atk)
				        || (filter_atktype == 3 && data.attack <= filter_atk) || (filter_atktype == 4 && (data.attack > filter_atk || data.attack < 0))
				        || (filter_atktype == 5 && (data.attack >= filter_atk || data.attack < 0)) || (filter_atktype == 6 && data.attack != -2))
					continue;
			}
			if(filter_deftype) {
				if((filter_deftype == 1 && data.defense != filter_def) || (filter_deftype == 2 && data.defense < filter_def)
				        || (filter_deftype == 3 && data.defense <= filter_def) || (filter_deftype == 4 && (data.defense > filter_def || data.defense < 0))
				        || (filter_deftype == 5 && (data.defense >= filter_def || data.defense < 0)) || (filter_deftype == 6 && data.defense != -2)
				        || (data.type & TYPE_LINK))
					continue;
			}
			if(filter_lvtype) {
				if((filter_lvtype == 1 && data.level != filter_lv) || (filter_lvtype == 2 && data.level < filter_lv)
				        || (filter_lvtype == 3 && data.level <= filter_lv) || (filter_lvtype == 4 && data.level > filter_lv)
				        || (filter_lvtype == 5 && data.level >= filter_lv) || filter_lvtype == 6)
					continue;
			}
			if(filter_scltype) {
				if((filter_scltype == 1 && data.lscale != filter_scl) || (filter_scltype == 2 && data.lscale < filter_scl)
				        || (filter_scltype == 3 && data.lscale <= filter_scl) || (filter_scltype == 4 && (data.lscale > filter_scl || data.lscale == 0))
				        || (filter_scltype == 5 && (data.lscale >= filter_scl || data.lscale == 0)) || filter_scltype == 6
				        || !(data.type & TYPE_PENDULUM))
					continue;
			}
			break;
		}
		case 2: {
			if(!(data.type & TYPE_SPELL))
				continue;
			if(filter_type2 && data.type != filter_type2)
				continue;
			break;
		}
		case 3: {
			if(!(data.type & TYPE_TRAP))
				continue;
			if(filter_type2 && data.type != filter_type2)
				continue;
			break;
		}
		}
		if(filter_effect && !(data.category & filter_effect))
			continue;
		if(filter_marks && (data.link_marker & filter_marks)!= filter_marks)
			continue;
		if(filter_lm) {
			if(filter_lm <= 3 && (!filterList->count(ptr->first) || (*filterList)[ptr->first] != filter_lm - 1))
				continue;
			if(filter_lm == 4 && data.ot != 1)
				continue;
			if(filter_lm == 5 && data.ot != 2)
				continue;
			if(filter_lm == 6 && data.ot != 3)
				continue;
			if(filter_lm == 7 && data.ot != 4)
				continue;
		}
		if(pstr) {
			if(pstr[0] == L'$') {
				if(!CardNameContains(text.name, &pstr[1]))
					continue;
			} else if(pstr[0] == L'@' && set_code) {
				if(!check_set_code(data, set_code)) continue;
			} else {
				if(!CardNameContains(text.name, pstr) && wcsstr(text.text, pstr) == 0
					&& (!set_code || !check_set_code(data, set_code)))
					continue;
			}
		}
		results.push_back(ptr);
	}
	myswprintf(result_string, L"%d", results.size());
	if(results.size() > 7) {
		mainGame->scrFilter->setVisible(true);
		mainGame->scrFilter->setMax(results.size() - 7);
		mainGame->scrFilter->setPos(0);
	} else {
		mainGame->scrFilter->setVisible(false);
		mainGame->scrFilter->setPos(0);
	}
	SortList();
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
	ClearFilter();
	results.clear();
	myswprintf(result_string, L"%d", 0);
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
	const wchar_t* pstr = mainGame->ebCardName->getText();
	for(auto it = results.begin(); it != results.end(); ++it) {
		if(wcscmp(pstr, dataManager.GetName((*it)->first)) == 0) {
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
static inline wchar_t NormalizeChar(wchar_t c) {
	/*
	// Convert all symbols and punctuations to space.
	if (c != 0 && c < 128 && !isalnum(c)) {
		return ' ';
	}
	*/
	// Convert latin chararacters to uppercase to ignore case.
	if (c < 128 && isalpha(c)) {
		return toupper(c);
	}
	// Remove some accentued characters that are not supported by the editbox.
	if (c >= 232 && c <= 235) {
		return 'E';
	}
	if (c >= 238 && c <= 239) {
		return 'I';
	}
	return c;
}
bool DeckBuilder::CardNameContains(const wchar_t *haystack, const wchar_t *needle)
{
	if (!needle[0]) {
		return true;
	}
	if (!haystack) {
		return false;
	}
	int i = 0;
	int j = 0;
	while (haystack[i]) {
		wchar_t ca = NormalizeChar(haystack[i]);
		wchar_t cb = NormalizeChar(needle[j]);
		if (ca == cb) {
			j++;
			if (!needle[j]) {
				return true;
			}
		} else {
			j = 0;
		}
		i++;
	}
	return false;
}
bool DeckBuilder::push_main(code_pointer pointer, int seq) {
	if(pointer->second.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK))
		return false;
	auto& container = deckManager.current_deck.main;
	int maxc = mainGame->is_siding ? 64 : 60;
	if((int)container.size() >= maxc)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	is_modified = true;
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_extra(code_pointer pointer, int seq) {
	if(!(pointer->second.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_LINK)))
		return false;
	auto& container = deckManager.current_deck.extra;
	int maxc = mainGame->is_siding ? 20 : 15;
	if((int)container.size() >= maxc)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	is_modified = true;
	GetHoveredCard();
	return true;
}
bool DeckBuilder::push_side(code_pointer pointer, int seq) {
	auto& container = deckManager.current_deck.side;
	int maxc = mainGame->is_siding ? 20 : 15;
	if((int)container.size() >= maxc)
		return false;
	if(seq >= 0 && seq < (int)container.size())
		container.insert(container.begin() + seq, pointer);
	else
		container.push_back(pointer);
	is_modified = true;
	GetHoveredCard();
	return true;
}
void DeckBuilder::pop_main(int seq) {
	auto& container = deckManager.current_deck.main;
	container.erase(container.begin() + seq);
	is_modified = true;
	GetHoveredCard();
}
void DeckBuilder::pop_extra(int seq) {
	auto& container = deckManager.current_deck.extra;
	container.erase(container.begin() + seq);
	is_modified = true;
	GetHoveredCard();
}
void DeckBuilder::pop_side(int seq) {
	auto& container = deckManager.current_deck.side;
	container.erase(container.begin() + seq);
	is_modified = true;
	GetHoveredCard();
}
bool DeckBuilder::check_limit(code_pointer pointer) {
	unsigned int limitcode = pointer->second.alias ? pointer->second.alias : pointer->first;
	int limit = 3;
	auto flit = filterList->find(limitcode);
	if(flit != filterList->end())
		limit = flit->second;
	for(auto it = deckManager.current_deck.main.begin(); it != deckManager.current_deck.main.end(); ++it) {
		if((*it)->first == limitcode || (*it)->second.alias == limitcode)
			limit--;
	}
	for(auto it = deckManager.current_deck.extra.begin(); it != deckManager.current_deck.extra.end(); ++it) {
		if((*it)->first == limitcode || (*it)->second.alias == limitcode)
			limit--;
	}
	for(auto it = deckManager.current_deck.side.begin(); it != deckManager.current_deck.side.end(); ++it) {
		if((*it)->first == limitcode || (*it)->second.alias == limitcode)
			limit--;
	}
	return limit > 0;
}
}
