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
bool DeckBuilder::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		if(mainGame->wCategories->isVisible() && id != BUTTON_CATEGORY_OK)
			break;
		if(mainGame->wQuery->isVisible() && id != BUTTON_YES && id != BUTTON_NO)
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
				if(deckManager.SaveDeck(deckManager.current_deck, mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()))) {
					mainGame->stACMessage->setText(dataManager.GetSysString(1335));
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
					mainGame->stACMessage->setText(dataManager.GetSysString(1335));
					mainGame->PopupElement(mainGame->wACMessage, 20);
				}
				break;
			}
			case BUTTON_DELETE_DECK: {
				if(mainGame->cbDBDecks->getSelected() == -1)
					break;
				mainGame->gMutex.Lock();
				mainGame->SetStaticText(mainGame->stQMessage, 310, mainGame->textFont, (wchar_t*)dataManager.GetSysString(1337));
				mainGame->PopupElement(mainGame->wQuery);
				mainGame->gMutex.Unlock();
				break;
			}
			case BUTTON_LEAVE_GAME: {
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
				imageManager.ClearTexture();
				mainGame->scrFilter->setVisible(false);
				if(mainGame->cbDBDecks->getSelected() != -1) {
					BufferIO::CopyWStr(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()), mainGame->gameConf.lastdeck, 64);
				}
				if(exit_on_return)
					mainGame->device->closeDevice();
				break;
			}
			case BUTTON_EFFECT_FILTER: {
				mainGame->PopupElement(mainGame->wCategories);
				break;
			}
			case BUTTON_START_FILTER: {
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
			case BUTTON_YES: {
				mainGame->HideElement(mainGame->wQuery);
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
				}
				break;
			}
			case BUTTON_NO: {
				mainGame->HideElement(mainGame->wQuery);
				break;
			}
			}
			break;
		}
		case irr::gui::EGET_SCROLL_BAR_CHANGED: {
			switch(id) {
			case SCROLL_CARDTEXT: {
				u32 pos = mainGame->scrCardText->getPos();
				mainGame->SetStaticText(mainGame->stText, mainGame->stText->getRelativePosition().getWidth()-25, mainGame->textFont, mainGame->showingtext, pos);
				break;
			}
			break;
			}
		}
		case irr::gui::EGET_EDITBOX_ENTER: {
			switch(id) {
			case EDITBOX_KEYWORD: {
				irr::SEvent me;
				me.EventType = irr::EET_GUI_EVENT;
				me.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
				me.GUIEvent.Caller = mainGame->btnStartFilter;
				me.GUIEvent.Element = mainGame->btnStartFilter;
				mainGame->device->postEventFromUser(me);
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
				deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
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
			}
		}
		default: break;
		}
		break;
	}
	case irr::EET_MOUSE_INPUT_EVENT: {
		switch(event.MouseInput.Event) {
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			position2d<s32> mouse_pos = position2d<s32>(event.MouseInput.X, event.MouseInput.Y);
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
			unsigned int limitcode = draging_pointer->second.alias ? draging_pointer->second.alias : draging_pointer->first;
			if(hovered_pos == 4) {
				int limit = 3;
				if(filterList->count(limitcode))
					limit = (*filterList)[limitcode];
				for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
					if(deckManager.current_deck.main[i]->first == limitcode
					        || deckManager.current_deck.main[i]->second.alias == limitcode)
						limit--;
				for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
					if(deckManager.current_deck.extra[i]->first == limitcode
					        || deckManager.current_deck.extra[i]->second.alias == limitcode)
						limit--;
				for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
					if(deckManager.current_deck.side[i]->first == limitcode
					        || deckManager.current_deck.side[i]->second.alias == limitcode)
						limit--;
				if(limit <= 0)
					break;
			}
			if(hovered_pos == 1)
				deckManager.current_deck.main.erase(deckManager.current_deck.main.begin() + hovered_seq);
			else if(hovered_pos == 2)
				deckManager.current_deck.extra.erase(deckManager.current_deck.extra.begin() + hovered_seq);
			else if(hovered_pos == 3)
				deckManager.current_deck.side.erase(deckManager.current_deck.side.begin() + hovered_seq);
			is_draging = true;
			break;
		}
		case irr::EMIE_LMOUSE_LEFT_UP: {
			if(!is_draging)
				break;
			if(!mainGame->is_siding) {
				if((hovered_pos == 1 && (draging_pointer->second.type & 0x802040)) || (hovered_pos == 2 && !(draging_pointer->second.type & 0x802040)))
					hovered_pos = 0;
				if((hovered_pos == 1 || (hovered_pos == 0 && click_pos == 1)) && deckManager.current_deck.main.size() < 60) {
					if(hovered_seq == -1)
						deckManager.current_deck.main.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.main.size() && hovered_pos)
						deckManager.current_deck.main.insert(deckManager.current_deck.main.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.main.push_back(draging_pointer);
					is_draging = false;
				} else if((hovered_pos == 2 || (hovered_pos == 0 && click_pos == 2)) && deckManager.current_deck.extra.size() < 15) {
					if(hovered_seq == -1)
						deckManager.current_deck.extra.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.extra.size() && hovered_pos)
						deckManager.current_deck.extra.insert(deckManager.current_deck.extra.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.extra.push_back(draging_pointer);
					is_draging = false;
				} else if((hovered_pos == 3 || (hovered_pos == 0 && click_pos == 3)) && deckManager.current_deck.side.size() < 15) {
					if(hovered_seq == -1)
						deckManager.current_deck.side.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.side.size() && hovered_pos)
						deckManager.current_deck.side.insert(deckManager.current_deck.side.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.side.push_back(draging_pointer);
					is_draging = false;
				} else if (hovered_pos == 4)
					is_draging = false;
			} else {
				if((hovered_pos == 1 && (draging_pointer->second.type & 0x802040)) || (hovered_pos == 2 && !(draging_pointer->second.type & 0x802040)) || hovered_pos == 4)
					hovered_pos = 0;
				if((hovered_pos == 1 || (hovered_pos == 0 && click_pos == 1)) && deckManager.current_deck.main.size() < 65) {
					if(hovered_seq == -1)
						deckManager.current_deck.main.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.main.size() && hovered_pos)
						deckManager.current_deck.main.insert(deckManager.current_deck.main.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.main.push_back(draging_pointer);
					is_draging = false;
				} else if((hovered_pos == 2 || (hovered_pos == 0 && click_pos == 2)) && deckManager.current_deck.extra.size() < 20) {
					if(hovered_seq == -1)
						deckManager.current_deck.extra.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.extra.size() && hovered_pos)
						deckManager.current_deck.extra.insert(deckManager.current_deck.extra.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.extra.push_back(draging_pointer);
					is_draging = false;
				} else if((hovered_pos == 3 || (hovered_pos == 0 && click_pos == 3)) && deckManager.current_deck.side.size() < 20) {
					if(hovered_seq == -1)
						deckManager.current_deck.side.push_back(draging_pointer);
					else if(hovered_seq < (int)deckManager.current_deck.side.size() && hovered_pos)
						deckManager.current_deck.side.insert(deckManager.current_deck.side.begin() + hovered_seq, draging_pointer);
					else deckManager.current_deck.side.push_back(draging_pointer);
					is_draging = false;
				}
			}
			if(is_draging) {
				if(click_pos == 1)
					deckManager.current_deck.main.push_back(draging_pointer);
				else if(click_pos == 2)
					deckManager.current_deck.extra.push_back(draging_pointer);
				else if(click_pos == 3)
					deckManager.current_deck.side.push_back(draging_pointer);
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
				draging_pointer = dataManager.GetCodePointer(hovered_code);
				if(draging_pointer == dataManager._datas.end())
					break;
				if(hovered_pos == 1) {
					if(deckManager.current_deck.side.size() < 20) {
						deckManager.current_deck.main.erase(deckManager.current_deck.main.begin() + hovered_seq);
						deckManager.current_deck.side.push_back(draging_pointer);
					}
				} else if(hovered_pos == 2) {
					if(deckManager.current_deck.side.size() < 20) {
						deckManager.current_deck.extra.erase(deckManager.current_deck.extra.begin() + hovered_seq);
						deckManager.current_deck.side.push_back(draging_pointer);
					}
				} else {
					if((draging_pointer->second.type & 0x802040) && deckManager.current_deck.extra.size() < 20) {
						deckManager.current_deck.side.erase(deckManager.current_deck.side.begin() + hovered_seq);
						deckManager.current_deck.extra.push_back(draging_pointer);
					}
					if(!(draging_pointer->second.type & 0x802040) && deckManager.current_deck.main.size() < 64) {
						deckManager.current_deck.side.erase(deckManager.current_deck.side.begin() + hovered_seq);
						deckManager.current_deck.main.push_back(draging_pointer);
					}
				}
				break;
			}
			if(mainGame->wCategories->isVisible() || mainGame->wQuery->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			if(!is_draging) {
				draging_pointer = dataManager.GetCodePointer(hovered_code);
				if(draging_pointer == dataManager._datas.end())
					break;
			}
			if(hovered_pos == 1) {
				if(!is_draging)
					deckManager.current_deck.main.erase(deckManager.current_deck.main.begin() + hovered_seq);
				else if(deckManager.current_deck.side.size() < 15) {
					deckManager.current_deck.side.push_back(draging_pointer);
					is_draging = false;
				}
			} else if(hovered_pos == 2) {
				if(!is_draging)
					deckManager.current_deck.extra.erase(deckManager.current_deck.extra.begin() + hovered_seq);
				else if(deckManager.current_deck.side.size() < 15) {
					deckManager.current_deck.side.push_back(draging_pointer);
					is_draging = false;
				}
			} else if(hovered_pos == 3) {
				if(!is_draging)
					deckManager.current_deck.side.erase(deckManager.current_deck.side.begin() + hovered_seq);
				else {
					if((draging_pointer->second.type & 0x802040) && deckManager.current_deck.extra.size() < 15) {
						deckManager.current_deck.extra.push_back(draging_pointer);
						is_draging = false;
					} else if(!(draging_pointer->second.type & 0x802040) && deckManager.current_deck.main.size() < 60) {
						deckManager.current_deck.main.push_back(draging_pointer);
						is_draging = false;
					}
				}
			} else {
				if(is_draging) {
					if(deckManager.current_deck.side.size() < 15) {
						deckManager.current_deck.side.push_back(draging_pointer);
						is_draging = false;
					}
				} else {
					unsigned int limitcode = draging_pointer->second.alias ? draging_pointer->second.alias : draging_pointer->first;
					int limit = 3;
					if(filterList->count(limitcode))
						limit = (*filterList)[limitcode];
					for(size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
						if(deckManager.current_deck.main[i]->first == limitcode
						        || deckManager.current_deck.main[i]->second.alias == limitcode)
							limit--;
					for(size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
						if(deckManager.current_deck.extra[i]->first == limitcode
						        || deckManager.current_deck.extra[i]->second.alias == limitcode)
							limit--;
					for(size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
						if(deckManager.current_deck.side[i]->first == limitcode
						        || deckManager.current_deck.side[i]->second.alias == limitcode)
							limit--;
					if(limit <= 0)
						break;
					if((draging_pointer->second.type & 0x802040) && deckManager.current_deck.extra.size() < 15) {
						deckManager.current_deck.extra.push_back(draging_pointer);
					} else if(!(draging_pointer->second.type & 0x802040) && deckManager.current_deck.main.size() < 60) {
						deckManager.current_deck.main.push_back(draging_pointer);
					}
				}
			}
			break;
		}
		case irr::EMIE_MOUSE_MOVED: {
			int x = event.MouseInput.X;
			int y = event.MouseInput.Y;
			int pre_code = hovered_code;
			if(x >= 314 && x <= 794 && y >= 164 && y <= 435) {
				int lx = 10, px, py = (y - 164) / 68;
				hovered_pos = 1;
				if(deckManager.current_deck.main.size() > 40)
					lx = (deckManager.current_deck.main.size() - 41) / 4 + 11;
				if(x >= 750)
					px = lx - 1;
				else px = (x - 314) * (lx - 1) / 436;
				if(py*lx + px >= (int)deckManager.current_deck.main.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_seq = py * lx + px;
					hovered_code = deckManager.current_deck.main[hovered_seq]->first;
				}
			} else if(x >= 314 && x <= 794 && y >= 466 && y <= 530) {
				int lx = deckManager.current_deck.extra.size();
				hovered_pos = 2;
				if(lx < 10)
					lx = 10;
				if(x >= 750)
					hovered_seq = lx - 1;
				else hovered_seq = (x - 314) * (lx - 1) / 436;
				if(hovered_seq >= (int)deckManager.current_deck.extra.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_code = deckManager.current_deck.extra[hovered_seq]->first;
				}
			} else if (x >= 314 && x <= 794 && y >= 564 && y <= 628) {
				int lx = deckManager.current_deck.side.size();
				hovered_pos = 3;
				if(lx < 10)
					lx = 10;
				if(x >= 750)
					hovered_seq = lx - 1;
				else hovered_seq = (x - 314) * (lx - 1) / 436;
				if(hovered_seq >= (int)deckManager.current_deck.side.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_code = deckManager.current_deck.side[hovered_seq]->first;
				}
			} else if(x >= 810 && x <= 995 && y >= 165 && y <= 626) {
				hovered_pos = 4;
				hovered_seq = (y - 165) / 66;
				if(mainGame->scrFilter->getPos() + hovered_seq >= (int)results.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_code = results[mainGame->scrFilter->getPos() + hovered_seq]->first;
				}
			} else {
				hovered_pos = 0;
				hovered_code = 0;
			}
			if(is_draging) {
				dragx = x;
				dragy = y;
			}
			if(!is_draging && pre_code != hovered_code) {
				if(hovered_code) {
					mainGame->ShowCardInfo(hovered_code);
				}
				if(pre_code)
					imageManager.RemoveTexture(pre_code);
			}
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
			SEvent e = event;
			e.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
			mainGame->device->postEventFromUser(e);
			break;
		}
		default: break;
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_R: {
			if(!event.KeyInput.PressedDown && !mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			if(!mainGame->HasFocus(EGUIET_EDIT_BOX))
				mainGame->device->minimizeWindow();
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
				        || (filter_deftype == 5 && (data.defense >= filter_def || data.defense < 0)) || (filter_deftype == 6 && data.defense != -2))
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
				        || (filter_scltype == 5 && (data.lscale >= filter_scl || data.lscale == 0)) || filter_scltype == 6)
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
		if(filter_lm) {
			if(filter_lm <= 3 && (!filterList->count(ptr->first) || (*filterList)[ptr->first] != filter_lm - 1))
				continue;
			if(filter_lm == 4 && data.ot != 1)
				continue;
			if(filter_lm == 5 && data.ot != 2)
				continue;
		}
		if(pstr) {
			if(pstr[0] == L'$') {
				if(wcsstr(text.name, &pstr[1]) == 0)
					continue;
			} else if(pstr[0] == L'@' && set_code) {
				unsigned long long sc = data.setcode;
				if(data.alias) {
					auto aptr = dataManager._datas.find(data.alias);
					if(aptr != dataManager._datas.end())
						sc = aptr->second.setcode;
				}
				bool res = false;
				int settype = set_code & 0xfff;
				int setsubtype = set_code & 0xf000;
				while(sc) {
					if ((sc & 0xfff) == settype && (sc & 0xf000 & setsubtype) == setsubtype)
						res = true;
					sc = sc >> 16;
				}
				if(!res) continue;
			} else {
				if(wcsstr(text.name, pstr) == 0 && wcsstr(text.text, pstr) == 0)
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
}
void DeckBuilder::SortList() {
	switch(mainGame->cbSortType->getSelected()) {
	case 0:
		std::sort(results.begin(), results.end(), ClientCard::deck_sort_lv);
		break;
	case 1:
		std::sort(results.begin(), results.end(), ClientCard::deck_sort_atk);
		break;
	case 2:
		std::sort(results.begin(), results.end(), ClientCard::deck_sort_def);
		break;
	case 3:
		std::sort(results.begin(), results.end(), ClientCard::deck_sort_name);
		break;
	}
}

}
