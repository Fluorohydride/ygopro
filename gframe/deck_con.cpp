#include "config.h"
#include "deck_con.h"
#include "game.h"
#include <algorithm>

extern ygo::Game* mainGame;

namespace ygo {

bool DeckBuilder::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_CLEAR_DECK: {
				mainGame->deckManager.deckhost.main.clear();
				mainGame->deckManager.deckhost.extra.clear();
				mainGame->deckManager.deckhost.side.clear();
				break;
			}
			case BUTTON_SORT_DECK: {
				std::sort(mainGame->deckManager.deckhost.main.begin(), mainGame->deckManager.deckhost.main.end(), ClientCard::deck_sort_lv);
				std::sort(mainGame->deckManager.deckhost.extra.begin(), mainGame->deckManager.deckhost.extra.end(), ClientCard::deck_sort_lv);
				std::sort(mainGame->deckManager.deckhost.side.begin(), mainGame->deckManager.deckhost.side.end(), ClientCard::deck_sort_lv);
				break;
			}
			case BUTTON_SAVE_DECK: {
				mainGame->deckManager.SaveDeck(mainGame->deckManager.deckhost, mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
				mainGame->stACMessage->setText(L"保存成功");
				mainGame->PopupElement(mainGame->wACMessage, 20);
				break;
			}
			case BUTTON_SAVE_DECK_AS: {
				const wchar_t* dname = mainGame->ebDeckname->getText();
				if(*dname == 0)
					break;
				int sel = -1;
				for(int i = 0; i < mainGame->cbDBDecks->getItemCount(); ++i) {
					if(!wcscmp(dname, mainGame->cbDBDecks->getItem(i))) {
						sel = i;
						break;
					}
				}
				if(sel >= 0)
					mainGame->cbDBDecks->setSelected(sel);
				else {
					mainGame->cbDBDecks->addItem(dname);
					mainGame->cbDeckSel->addItem(dname);
					mainGame->cbDBDecks->setSelected(mainGame->cbDBDecks->getItemCount() - 1);
				}
				mainGame->deckManager.SaveDeck(mainGame->deckManager.deckhost, dname);
				mainGame->ebDeckname->setText(L"");
				mainGame->stACMessage->setText(L"保存成功");
				mainGame->PopupElement(mainGame->wACMessage, 20);
				break;
			}
			case BUTTON_DBEXIT: {
				mainGame->is_building = false;
				mainGame->wDeckEdit->setVisible(false);
				mainGame->wCategories->setVisible(false);
				mainGame->wFilter->setVisible(false);
				mainGame->wCardImg->setVisible(false);
				mainGame->wInfos->setVisible(false);
				mainGame->cbDeckSel->setSelected(mainGame->cbDBDecks->getSelected());
				mainGame->PopupElement(mainGame->wModeSelection);
				mainGame->device->setEventReceiver(&mainGame->dField);
				mainGame->imageManager.ClearTexture();
				mainGame->scrFilter->setVisible(false);
				break;
			}
			case BUTTON_EFFECT_FILTER: {
				mainGame->PopupElement(mainGame->wCategories);
				break;
			}
			case BUTTON_START_FILTER: {
				filter_type = mainGame->cbCardType->getSelected();
				filter_type2 = mainGame->cbCardType2->getItemData(mainGame->cbCardType2->getSelected());
				filter_series = mainGame->cbCardClass->getItemData(mainGame->cbCardClass->getSelected());
				filter_lm = mainGame->cbLimit->getSelected();
				if(filter_type > 1) {
					FilterCards();
					break;
				}
				filter_attrib = mainGame->cbAttribute->getItemData(mainGame->cbAttribute->getSelected());
				filter_race = mainGame->cbRace->getItemData(mainGame->cbRace->getSelected());
				const wchar_t* pstr = mainGame->ebAttack->getText();
				if(*pstr == 0) filter_atktype = 0;
				else {
					if(*pstr == L'=') {
						filter_atktype = 1;
						filter_atk = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_atktype = 1;
						filter_atk = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_atktype = 2;
							filter_atk = GetVal(pstr + 2);
						} else {
							filter_atktype = 3;
							filter_atk = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_atktype = 4;
							filter_atk = GetVal(pstr + 2);
						} else {
							filter_atktype = 5;
							filter_atk = GetVal(pstr + 1);
						}
					} else if(*pstr == L'?') {
						filter_atktype = 6;
					} else filter_atktype = 0;
				}
				pstr = mainGame->ebDefence->getText();
				if(*pstr == 0) filter_deftype = 0;
				else {
					if(*pstr == L'=') {
						filter_deftype = 1;
						filter_def = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_deftype = 1;
						filter_def = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_deftype = 2;
							filter_def = GetVal(pstr + 2);
						} else {
							filter_deftype = 3;
							filter_def = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_deftype = 4;
							filter_def = GetVal(pstr + 2);
						} else {
							filter_deftype = 5;
							filter_def = GetVal(pstr + 1);
						}
					} else if(*pstr == L'?') {
						filter_deftype = 6;
					} else filter_deftype = 0;
				}
				pstr = mainGame->ebStar->getText();
				if(*pstr == 0) filter_lvtype = 0;
				else {
					if(*pstr == L'=') {
						filter_lvtype = 1;
						filter_lv = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_lvtype = 1;
						filter_lv = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_lvtype = 2;
							filter_lv = GetVal(pstr + 2);
						} else {
							filter_lvtype = 3;
							filter_lv = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_lvtype = 4;
							filter_lv = GetVal(pstr + 2);
						} else {
							filter_lvtype = 5;
							filter_lv = GetVal(pstr + 1);
						}
					} else filter_lvtype = 0;
				}
				FilterCards();
				break;
			}
			case BUTTON_RESULT_FILTER: {
				filter_type = mainGame->cbCardType->getSelected();
				filter_type2 = mainGame->cbCardType2->getItemData(mainGame->cbCardType2->getSelected());
				filter_series = mainGame->cbCardClass->getItemData(mainGame->cbCardClass->getSelected());
				filter_lm = mainGame->cbLimit->getSelected();
				if(filter_type > 1) {
					FilterCardsFromResult();
					break;
				}
				filter_attrib = mainGame->cbAttribute->getItemData(mainGame->cbAttribute->getSelected());
				filter_race = mainGame->cbRace->getItemData(mainGame->cbRace->getSelected());
				const wchar_t* pstr = mainGame->ebAttack->getText();
				if(*pstr == 0) filter_atktype = 0;
				else {
					if(*pstr == L'=') {
						filter_atktype = 1;
						filter_atk = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_atktype = 1;
						filter_atk = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_atktype = 2;
							filter_atk = GetVal(pstr + 2);
						} else {
							filter_atktype = 3;
							filter_atk = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_atktype = 4;
							filter_atk = GetVal(pstr + 2);
						} else {
							filter_atktype = 5;
							filter_atk = GetVal(pstr + 1);
						}
					} else if(*pstr == L'?') {
						filter_atktype = 6;
					} else filter_atktype = 0;
				}
				pstr = mainGame->ebDefence->getText();
				if(*pstr == 0) filter_deftype = 0;
				else {
					if(*pstr == L'=') {
						filter_deftype = 1;
						filter_def = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_deftype = 1;
						filter_def = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_deftype = 2;
							filter_def = GetVal(pstr + 2);
						} else {
							filter_deftype = 3;
							filter_def = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_deftype = 4;
							filter_def = GetVal(pstr + 2);
						} else {
							filter_deftype = 5;
							filter_def = GetVal(pstr + 1);
						}
					} else if(*pstr == L'?') {
						filter_deftype = 6;
					} else filter_deftype = 0;
				}
				pstr = mainGame->ebStar->getText();
				if(*pstr == 0) filter_lvtype = 0;
				else {
					if(*pstr == L'=') {
						filter_lvtype = 1;
						filter_lv = GetVal(pstr + 1);
					} else if(*pstr >= L'0' && *pstr <= L'9') {
						filter_lvtype = 1;
						filter_lv = GetVal(pstr);
					} else if(*pstr == L'>') {
						if(*(pstr + 1) == L'=') {
							filter_lvtype = 2;
							filter_lv = GetVal(pstr + 2);
						} else {
							filter_lvtype = 3;
							filter_lv = GetVal(pstr + 1);
						}
					} else if(*pstr == L'<') {
						if(*(pstr + 1) == L'=') {
							filter_lvtype = 4;
							filter_lv = GetVal(pstr + 2);
						} else {
							filter_lvtype = 5;
							filter_lv = GetVal(pstr + 1);
						}
					} else filter_lvtype = 0;
				}
				FilterCardsFromResult();
				break;
			}
			case BUTTON_GETCODE: {
				const wchar_t* strcode = mainGame->ebCardCode->getText();
				if(*strcode) {
					int code = _wtoi(strcode);
					auto it = mainGame->dataManager._datas.find(code);
					if(it != mainGame->dataManager._datas.end()) {
						results.clear();
						results.push_back(it);
						myswprintf(result_string, L"1");
						mainGame->scrFilter->setVisible(false);
						mainGame->scrFilter->setPos(0);
						mainGame->cbCardClass->setSelected(0);
						mainGame->cbAttribute->setSelected(0);
						mainGame->cbRace->setSelected(0);
						mainGame->cbLimit->setSelected(0);
						mainGame->ebAttack->setText(L"");
						mainGame->ebDefence->setText(L"");
						mainGame->ebStar->setText(L"");
						mainGame->ebCardCode->setText(L"");
						filter_effect = 0;
						for(int i = 0; i < 32; ++i)
							mainGame->chkCategory[i]->setChecked(false);
					}
					break;
				}
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
			}
			break;
		}
		case irr::gui::EGET_COMBO_BOX_CHANGED: {
			switch(id) {
			case COMBOBOX_DBLFLIST: {
				filterList = mainGame->deckManager._lfList[mainGame->cbDBLFList->getSelected()].content;
				break;
			}
			case COMBOBOX_DBDECKS: {
				mainGame->deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
				break;
			}
			case COMBOBOX_MAINTYPE: {
				switch(mainGame->cbCardType->getSelected()) {
				case 0: {
					mainGame->cbCardType2->setEnabled(false);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefence->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					break;
				}
				case 1: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(true);
					mainGame->cbAttribute->setEnabled(true);
					mainGame->ebAttack->setEnabled(true);
					mainGame->ebDefence->setEnabled(true);
					mainGame->ebStar->setEnabled(true);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(L"(无)", 0);
					mainGame->cbCardType2->addItem(L"通常", TYPE_MONSTER + TYPE_NORMAL);
					mainGame->cbCardType2->addItem(L"效果", TYPE_MONSTER + TYPE_EFFECT);
					mainGame->cbCardType2->addItem(L"融合", TYPE_MONSTER + TYPE_FUSION);
					mainGame->cbCardType2->addItem(L"仪式", TYPE_MONSTER + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(L"同调", TYPE_MONSTER + TYPE_SYNCHRO);
					mainGame->cbCardType2->addItem(L"超量", TYPE_MONSTER + TYPE_XYZ);
					mainGame->cbCardType2->addItem(L"调整", TYPE_MONSTER + TYPE_TUNER);
					mainGame->cbCardType2->addItem(L"二重", TYPE_MONSTER + TYPE_DUAL);
					mainGame->cbCardType2->addItem(L"同盟", TYPE_MONSTER + TYPE_UNION);
					mainGame->cbCardType2->addItem(L"灵魂", TYPE_MONSTER + TYPE_SPIRIT);
					mainGame->cbCardType2->addItem(L"反转", TYPE_MONSTER + TYPE_FLIP);
					mainGame->cbCardType2->addItem(L"卡通", TYPE_MONSTER + TYPE_TOON);
					break;
				}
				case 2: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefence->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(L"(无)", 0);
					mainGame->cbCardType2->addItem(L"通常", TYPE_SPELL);
					mainGame->cbCardType2->addItem(L"速攻", TYPE_SPELL + TYPE_QUICKPLAY);
					mainGame->cbCardType2->addItem(L"永续", TYPE_SPELL + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(L"仪式", TYPE_SPELL + TYPE_RITUAL);
					mainGame->cbCardType2->addItem(L"装备", TYPE_SPELL + TYPE_EQUIP);
					mainGame->cbCardType2->addItem(L"场地", TYPE_SPELL + TYPE_FIELD);
					break;
				}
				case 3: {
					mainGame->cbCardType2->setEnabled(true);
					mainGame->cbRace->setEnabled(false);
					mainGame->cbAttribute->setEnabled(false);
					mainGame->ebAttack->setEnabled(false);
					mainGame->ebDefence->setEnabled(false);
					mainGame->ebStar->setEnabled(false);
					mainGame->cbCardType2->clear();
					mainGame->cbCardType2->addItem(L"(无)", 0);
					mainGame->cbCardType2->addItem(L"通常", TYPE_TRAP);
					mainGame->cbCardType2->addItem(L"永续", TYPE_TRAP + TYPE_CONTINUOUS);
					mainGame->cbCardType2->addItem(L"反击", TYPE_TRAP + TYPE_COUNTER);
					break;
				}
				}
			}
			}
		}
		}
		break;
	}
	case irr::EET_MOUSE_INPUT_EVENT: {
		switch(event.MouseInput.Event) {
		case irr::EMIE_LMOUSE_PRESSED_DOWN: {
			if(mainGame->wCategories->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			if(hovered_pos == 4) {
				int limit = 3;
				if(filterList->count(hovered_code))
					limit = (*filterList)[hovered_code];
				for(int i = 0; i < mainGame->deckManager.deckhost.main.size(); ++i)
					if(mainGame->deckManager.deckhost.main[i]->first == hovered_code)
						limit--;
				for(int i = 0; i < mainGame->deckManager.deckhost.extra.size(); ++i)
					if(mainGame->deckManager.deckhost.extra[i]->first == hovered_code)
						limit--;
				for(int i = 0; i < mainGame->deckManager.deckhost.side.size(); ++i)
					if(mainGame->deckManager.deckhost.side[i]->first == hovered_code)
						limit--;
				if(limit <= 0)
					break;
			}
			click_pos = hovered_pos;
			dragx = event.MouseInput.X;
			dragy = event.MouseInput.Y;
			draging_pointer = mainGame->dataManager.GetCodePointer(hovered_code);
			if(hovered_pos == 1)
				mainGame->deckManager.deckhost.main.erase(mainGame->deckManager.deckhost.main.begin() + hovered_seq);
			else if(hovered_pos == 2)
				mainGame->deckManager.deckhost.extra.erase(mainGame->deckManager.deckhost.extra.begin() + hovered_seq);
			else if(hovered_pos == 3)
				mainGame->deckManager.deckhost.side.erase(mainGame->deckManager.deckhost.side.begin() + hovered_seq);
			is_draging = true;
			break;
		}
		case irr::EMIE_LMOUSE_LEFT_UP: {
			if(!is_draging)
				break;
			is_draging = false;
			if((hovered_pos == 1 && (draging_pointer->second.type & 0x802040)) || (hovered_pos == 2 && !(draging_pointer->second.type & 0x802040)))
				hovered_pos = 0;
			if((hovered_pos == 1 || (hovered_pos == 0 && click_pos == 1)) && mainGame->deckManager.deckhost.main.size() < 60) {
				if(hovered_seq < mainGame->deckManager.deckhost.main.size())
					mainGame->deckManager.deckhost.main.insert(mainGame->deckManager.deckhost.main.begin() + hovered_seq, draging_pointer);
				else mainGame->deckManager.deckhost.main.push_back(draging_pointer);
			} else if((hovered_pos == 2 || (hovered_pos == 0 && click_pos == 2)) && mainGame->deckManager.deckhost.extra.size() < 15) {
				if(hovered_seq < mainGame->deckManager.deckhost.extra.size())
					mainGame->deckManager.deckhost.extra.insert(mainGame->deckManager.deckhost.extra.begin() + hovered_seq, draging_pointer);
				else mainGame->deckManager.deckhost.extra.push_back(draging_pointer);
			} else if((hovered_pos == 3 || (hovered_pos == 0 && click_pos == 3)) && mainGame->deckManager.deckhost.side.size() < 15) {
				if(hovered_seq < mainGame->deckManager.deckhost.side.size())
					mainGame->deckManager.deckhost.side.insert(mainGame->deckManager.deckhost.side.begin() + hovered_seq, draging_pointer);
				else mainGame->deckManager.deckhost.side.push_back(draging_pointer);
			}
			break;
		}
		case irr::EMIE_RMOUSE_LEFT_UP: {
			if(mainGame->wCategories->isVisible())
				break;
			if(hovered_pos == 0 || hovered_seq == -1)
				break;
			if(!is_draging)
				draging_pointer = mainGame->dataManager.GetCodePointer(hovered_code);
			if(hovered_pos == 1) {
				if(!is_draging)
					mainGame->deckManager.deckhost.main.erase(mainGame->deckManager.deckhost.main.begin() + hovered_seq);
				else if(mainGame->deckManager.deckhost.side.size() < 15) {
					mainGame->deckManager.deckhost.side.push_back(draging_pointer);
					is_draging = false;
				}
			} else if(hovered_pos == 2) {
				if(!is_draging)
					mainGame->deckManager.deckhost.extra.erase(mainGame->deckManager.deckhost.extra.begin() + hovered_seq);
				else if(mainGame->deckManager.deckhost.side.size() < 15) {
					mainGame->deckManager.deckhost.side.push_back(draging_pointer);
					is_draging = false;
				}
			} else if(hovered_pos == 3) {
				if(!is_draging)
					mainGame->deckManager.deckhost.side.erase(mainGame->deckManager.deckhost.side.begin() + hovered_seq);
				else {
					if((draging_pointer->second.type & 0x802040) && mainGame->deckManager.deckhost.extra.size() < 15) {
						mainGame->deckManager.deckhost.extra.push_back(draging_pointer);
						is_draging = false;
					} else if(!(draging_pointer->second.type & 0x802040) && mainGame->deckManager.deckhost.main.size() < 60) {
						mainGame->deckManager.deckhost.main.push_back(draging_pointer);
						is_draging = false;
					}
				}
			} else {
				if(is_draging) {
					if(mainGame->deckManager.deckhost.side.size() < 15) {
						mainGame->deckManager.deckhost.side.push_back(draging_pointer);
						is_draging = false;
					}
				} else {
					int limit = 3;
					if(filterList->count(hovered_code))
						limit = (*filterList)[hovered_code];
					for(int i = 0; i < mainGame->deckManager.deckhost.main.size(); ++i)
						if(mainGame->deckManager.deckhost.main[i]->first == hovered_code)
							limit--;
					for(int i = 0; i < mainGame->deckManager.deckhost.extra.size(); ++i)
						if(mainGame->deckManager.deckhost.extra[i]->first == hovered_code)
							limit--;
					for(int i = 0; i < mainGame->deckManager.deckhost.side.size(); ++i)
						if(mainGame->deckManager.deckhost.side[i]->first == hovered_code)
							limit--;
					if(limit <= 0)
						break;
					if((draging_pointer->second.type & 0x802040) && mainGame->deckManager.deckhost.extra.size() < 15) {
						mainGame->deckManager.deckhost.extra.push_back(draging_pointer);
					} else if(!(draging_pointer->second.type & 0x802040) && mainGame->deckManager.deckhost.main.size() < 60) {
						mainGame->deckManager.deckhost.main.push_back(draging_pointer);
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
				if(mainGame->deckManager.deckhost.main.size() > 40)
					lx = (mainGame->deckManager.deckhost.main.size() - 41) / 4 + 11;
				if(x >= 750)
					px = lx - 1;
				else px = (x - 314) * (lx - 1) / 436;
				if(py*lx + px >= mainGame->deckManager.deckhost.main.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_seq = py * lx + px;
					hovered_code = mainGame->deckManager.deckhost.main[hovered_seq]->first;
				}
			} else if(x >= 314 && x <= 794 && y >= 466 && y <= 530) {
				int lx = mainGame->deckManager.deckhost.extra.size();
				hovered_pos = 2;
				if(lx < 10)
					lx = 10;
				if(x >= 750)
					hovered_seq = lx - 1;
				else hovered_seq = (x - 314) * (lx - 1) / 436;
				if(hovered_seq >= mainGame->deckManager.deckhost.extra.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_code = mainGame->deckManager.deckhost.extra[hovered_seq]->first;
				}
			} else if (x >= 314 && x <= 794 && y >= 564 && y <= 628) {
				int lx = mainGame->deckManager.deckhost.side.size();
				hovered_pos = 3;
				if(lx < 10)
					lx = 10;
				if(x >= 750)
					hovered_seq = lx - 1;
				else hovered_seq = (x - 314) * (lx - 1) / 436;
				if(hovered_seq >= mainGame->deckManager.deckhost.side.size()) {
					hovered_seq = -1;
					hovered_code = 0;
				} else {
					hovered_code = mainGame->deckManager.deckhost.side[hovered_seq]->first;
				}
			} else if(x >= 810 && x <= 995 && y >= 165 && y <= 626) {
				hovered_pos = 4;
				hovered_seq = (y - 165) / 66;
				if(mainGame->scrFilter->getPos() + hovered_seq >= results.size()) {
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
					CardData cd;
					wchar_t formatBuffer[64];
					mainGame->dataManager.GetData(hovered_code, &cd);
					mainGame->imgCard->setImage(mainGame->imageManager.GetTexture(hovered_code));
					myswprintf(formatBuffer, L"%ls[%d]", mainGame->dataManager.GetName(hovered_code), hovered_code);
					mainGame->stName->setText(formatBuffer);
					if(cd.type & TYPE_MONSTER) {
						myswprintf(formatBuffer, L"[%ls] %ls/%ls", DataManager::FormatType(cd.type), DataManager::FormatRace(cd.race), DataManager::FormatAttribute(cd.attribute));
						mainGame->stInfo->setText(formatBuffer);
						formatBuffer[0] = L'[';
						for(int i = 1; i <= cd.level; ++i)
							formatBuffer[i] = L'★';
						formatBuffer[cd.level + 1] = L']';
						formatBuffer[cd.level + 2] = L' ';
						if(cd.attack < 0 && cd.defence < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"?/?");
						else if(cd.attack < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"?/%d", cd.defence);
						else if(cd.defence < 0)
							myswprintf(&formatBuffer[cd.level + 3], L"%d/?", cd.attack);
						else
							myswprintf(&formatBuffer[cd.level + 3], L"%d/%d", cd.attack, cd.defence);
						mainGame->stDataInfo->setText(formatBuffer);
						mainGame->stText->setRelativePosition(irr::core::position2di(15, 83));
					} else {
						myswprintf(formatBuffer, L"[%ls]", DataManager::FormatType(cd.type));
						mainGame->stInfo->setText(formatBuffer);
						mainGame->stDataInfo->setText(L"");
						mainGame->stText->setRelativePosition(irr::core::position2di(15, 60));
					}
					mainGame->SetStaticText(mainGame->stText, 270, mainGame->textFont, (wchar_t*)mainGame->dataManager.GetText(hovered_code));
				}
				if(pre_code)
					mainGame->imageManager.RemoveTexture(pre_code);
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
			device->postEventFromUser(e);
			break;
		}
		}
		break;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_R: {
			if(!event.KeyInput.PressedDown)
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			device->minimizeWindow();
			break;
		}
		}
		break;
	}
	}
	return false;
}
int DeckBuilder::GetVal(const wchar_t * pstr) {
	int ret = 0;
	while(*pstr >= L'0' && *pstr <= L'9') {
		ret = ret * 10 + (*pstr - L'0');
		pstr++;
	}
	return ret;
}
void DeckBuilder::FilterCards() {
	results.clear();
	for(code_pointer ptr = mainGame->dataManager._datas.begin(); ptr != mainGame->dataManager._datas.end(); ++ptr) {
		CardDataC& data = ptr->second;
		if(data.type & TYPE_TOKEN)
			continue;
		switch(filter_type) {
		case 1: {
			int type2 = data.type & 0xe03ef1;
			if(!(data.type & TYPE_MONSTER) || (filter_type2 == 0x21 && type2 != 0x21) || (data.type & filter_type2) != filter_type2)
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
				if((filter_deftype == 1 && data.defence != filter_def) || (filter_deftype == 2 && data.defence < filter_def)
				        || (filter_deftype == 3 && data.defence <= filter_def) || (filter_deftype == 4 && (data.defence > filter_def || data.defence < 0))
				        || (filter_deftype == 5 && (data.defence >= filter_def || data.defence < 0)) || (filter_deftype == 6 && data.defence != -2))
					continue;
			}
			if(filter_lvtype) {
				if((filter_lvtype == 1 && data.level != filter_lv) || (filter_lvtype == 2 && data.level < filter_lv)
				        || (filter_lvtype == 3 && data.level <= filter_lv) || (filter_lvtype == 4 && data.level > filter_lv)
				        || (filter_lvtype == 5 && data.level >= filter_lv))
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
			if(!filterList->count(ptr->first) || (*filterList)[ptr->first] != filter_lm - 1)
				continue;
		}
		if(filter_series) {
			uint32 settype = filter_series & 0xfff;
			uint32 setsubtype = filter_series & 0xf000;
			if (((data.setcode & 0xfff) != settype || ((data.setcode & 0xf000 & setsubtype) != setsubtype)) &&
			        (((data.setcode >> 16) & 0xfff) != settype || ((data.setcode >> 16) & 0xf000 & setsubtype) != setsubtype))
				continue;
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
	std::sort(results.begin(), results.end(), ClientCard::deck_sort_lv);
	mainGame->cbCardClass->setSelected(0);
	mainGame->cbAttribute->setSelected(0);
	mainGame->cbRace->setSelected(0);
	mainGame->cbLimit->setSelected(0);
	mainGame->ebAttack->setText(L"");
	mainGame->ebDefence->setText(L"");
	mainGame->ebStar->setText(L"");
	filter_effect = 0;
	for(int i = 0; i < 32; ++i)
		mainGame->chkCategory[i]->setChecked(false);
}
void DeckBuilder::FilterCardsFromResult() {
	int offset = 0;
	for(int i = 0; i < results.size(); ++i) {
		CardDataC& data = results[i]->second;
		if(data.type & TYPE_TOKEN)
			continue;
		switch(filter_type) {
		case 1: {
			int type2 = data.type & 0xe03ef1;
			if(!(data.type & TYPE_MONSTER) || (filter_type2 == 0x21 && type2 != 0x21) || (data.type & filter_type2) != filter_type2)
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
				if((filter_deftype == 1 && data.defence != filter_def) || (filter_deftype == 2 && data.defence < filter_def)
				        || (filter_deftype == 3 && data.defence <= filter_def) || (filter_deftype == 4 && (data.defence > filter_def || data.defence < 0))
				        || (filter_deftype == 5 && (data.defence >= filter_def || data.defence < 0)) || (filter_deftype == 6 && data.defence != -2))
					continue;
			}
			if(filter_lvtype) {
				if((filter_lvtype == 1 && data.level != filter_lv) || (filter_lvtype == 2 && data.level < filter_lv)
				        || (filter_lvtype == 3 && data.level <= filter_lv) || (filter_lvtype == 4 && data.level > filter_lv)
				        || (filter_lvtype == 5 && data.level >= filter_lv))
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
		if(filter_lm) {
			if(!filterList->count(results[i]->first) || (*filterList)[results[i]->first] != filter_lm - 1)
				continue;
		}
		if(filter_effect && !(data.category & filter_effect))
			continue;
		if(filter_series) {
			uint32 settype = filter_series & 0xfff;
			uint32 setsubtype = filter_series & 0xf000;
			if (((data.setcode & 0xfff) != settype || ((data.setcode & 0xf000 & setsubtype) != setsubtype)) &&
			        (((data.setcode >> 16) & 0xfff) != settype || ((data.setcode >> 16) & 0xf000 & setsubtype) != setsubtype))
				continue;
		}
		results[offset] = results[i];
		offset++;
	}
	results.resize(offset);
	myswprintf(result_string, L"%d", results.size());
	if(results.size() > 7) {
		mainGame->scrFilter->setVisible(true);
		mainGame->scrFilter->setMax(results.size() - 7);
		mainGame->scrFilter->setPos(0);
	} else {
		mainGame->scrFilter->setVisible(false);
		mainGame->scrFilter->setPos(0);
	}
	mainGame->cbCardClass->setSelected(0);
	mainGame->cbAttribute->setSelected(0);
	mainGame->cbRace->setSelected(0);
	mainGame->cbLimit->setSelected(0);
	mainGame->ebAttack->setText(L"");
	mainGame->ebDefence->setText(L"");
	mainGame->ebStar->setText(L"");
	filter_effect = 0;
	for(int i = 0; i < 32; ++i)
		mainGame->chkCategory[i]->setChecked(false);
}
}
