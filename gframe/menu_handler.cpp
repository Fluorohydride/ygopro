#include "config.h"
#include "menu_handler.h"
#include "game.h"

namespace ygo {

bool MenuHandler::OnEvent(const irr::SEvent& event) {
	switch(event.EventType) {
	case irr::EET_GUI_EVENT: {
		s32 id = event.GUIEvent.Caller->getID();
		irr::gui::IGUIEnvironment* env = mainGame->device->getGUIEnvironment();
		switch(event.GUIEvent.EventType) {
		case irr::gui::EGET_BUTTON_CLICKED: {
			switch(id) {
			case BUTTON_MODE_EXIT: {
				mainGame->device->closeDevice();
				break;
			}
			case BUTTON_LAN_MODE: {
				mainGame->HideElement(mainGame->wMainMenu, false, mainGame->wLanWindow);
				break;
			}
			case BUTTON_JOIN_HOST: {
				break;
			}
			case BUTTON_JOIN_CANCEL: {
				mainGame->HideElement(mainGame->wLanWindow, false, mainGame->wMainMenu);
				break;
			}
			case BUTTON_CREATE_HOST: {
				mainGame->HideElement(mainGame->wLanWindow, false, mainGame->wHostWindow);
				break;
			}
			case BUTTON_HOST_CONFIRM: {
				break;
			}
			case BUTTON_HOST_CANCEL: {
				mainGame->HideElement(mainGame->wHostWindow, false, mainGame->wLanWindow);
				break;
			}
			case BUTTON_DECK_EDIT: {
				mainGame->RefreshDeck(mainGame->cbDBDecks);
				if(mainGame->cbDBDecks->getSelected() != -1)
					mainGame->deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
				mainGame->HideElement(mainGame->wMainMenu);
				mainGame->is_building = true;
				mainGame->wInfos->setVisible(true);
				mainGame->wCardImg->setVisible(true);
				mainGame->wDeckEdit->setVisible(true);
				mainGame->wFilter->setVisible(true);
				mainGame->deckBuilder.filterList = mainGame->deckManager._lfList[0].content;;
				mainGame->cbDBLFList->setSelected(0);
				mainGame->device->setEventReceiver(&mainGame->deckBuilder);
				mainGame->cbCardType->setSelected(0);
				mainGame->cbCardType2->setSelected(0);
				mainGame->cbAttribute->setSelected(0);
				mainGame->cbRace->setSelected(0);
				mainGame->ebAttack->setText(L"");
				mainGame->ebDefence->setText(L"");
				mainGame->ebStar->setText(L"");
				mainGame->cbCardType2->setEnabled(false);
				mainGame->cbAttribute->setEnabled(false);
				mainGame->cbRace->setEnabled(false);
				mainGame->ebAttack->setEnabled(false);
				mainGame->ebDefence->setEnabled(false);
				mainGame->ebStar->setEnabled(false);
				mainGame->deckBuilder.filter_effect = 0;
				mainGame->deckBuilder.result_string[0] = L'0';
				mainGame->deckBuilder.result_string[1] = 0;
				mainGame->deckBuilder.results.clear();
				mainGame->deckBuilder.is_draging = false;
				for(int i = 0; i < 32; ++i)
					mainGame->chkCategory[i]->setChecked(false);
				break;
			}
			/*			case BUTTON_LAN_START_SERVER: {
							if(mainGame->cbDeckSel->getSelected() == -1)
								break;
							if(!mainGame->deckManager.LoadDeck(mainGame->cbDeckSel->getItem(mainGame->cbDeckSel->getSelected()))) {
								mainGame->stModeStatus->setText(L"无效卡组");
								break;
							}
							if(!mainGame->chkNoCheckDeck->isChecked()
							        && !mainGame->deckManager.CheckLFList(mainGame->deckManager.deckhost, mainGame->cbLFlist->getSelected())) {
								mainGame->stModeStatus->setText(L"无效卡组或者卡组不符合禁卡表规范");
								break;
							}
							if(_wtoi(mainGame->ebStartLP->getText()) == 0)
								mainGame->ebStartLP->setText(L"8000");
							if(_wtoi(mainGame->ebStartHand->getText()) == 0)
								mainGame->ebStartLP->setText(L"5");
							if(_wtoi(mainGame->ebDrawCount->getText()) == 0)
								mainGame->ebStartLP->setText(L"1");
							if(mainGame->netManager.CreateHost(mainGame->cbIPList->getSelected())) {
								mainGame->btnLanStartServer->setEnabled(false);
								mainGame->btnLanCancelServer->setEnabled(true);
								mainGame->btnLanConnect->setEnabled(false);
								mainGame->btnRefreshList->setEnabled(false);
								mainGame->btnLoadReplay->setEnabled(false);
								mainGame->btnDeckEdit->setEnabled(false);
								mainGame->stModeStatus->setText(L"等待连接...");
							}
							break;
						}
						case BUTTON_LAN_CANCEL_SERVER: {
							mainGame->netManager.CancelHost();
							mainGame->stModeStatus->setText(L"");
							break;
						}
						case BUTTON_LAN_REFRESH: {
							if(mainGame->netManager.RefreshHost(mainGame->cbIPList->getSelected())) {
								mainGame->btnLanStartServer->setEnabled(false);
								mainGame->btnLanConnect->setEnabled(false);
								mainGame->btnRefreshList->setEnabled(false);
								mainGame->btnLoadReplay->setEnabled(false);
								mainGame->btnDeckEdit->setEnabled(false);
							}
							break;
						}
						case BUTTON_LAN_CONNECT: {
							if(mainGame->cbDeckSel->getSelected() == -1)
								break;
							if(!mainGame->deckManager.LoadDeck(mainGame->cbDeckSel->getItem(mainGame->cbDeckSel->getSelected()))) {
								mainGame->stModeStatus->setText(L"无效卡组");
								break;
							}
							if(mainGame->netManager.JoinHost()) {
								mainGame->btnLanStartServer->setEnabled(false);
								mainGame->btnLanConnect->setEnabled(false);
								mainGame->btnRefreshList->setEnabled(false);
								mainGame->btnLoadReplay->setEnabled(false);
								mainGame->btnDeckEdit->setEnabled(false);
								mainGame->stModeStatus->setText(L"连接中...");
							}
							break;
						}
						case BUTTON_LOAD_REPLAY: {
							if(mainGame->lstReplayList->getSelected() == -1)
								break;
							if(!mainGame->lastReplay.OpenReplay(mainGame->lstReplayList->getListItem(mainGame->lstReplayList->getSelected()))) {
								mainGame->stModeStatus->setText(L"录像损坏或丢失，无法播放");
								break;
							}
							mainGame->stModeStatus->setText(L"");
							Thread::NewThread(Game::ReplayThread, &mainGame->dInfo);
							break;
						}
						*/
			}
		}
		break;
		}
		return false;
	}
	case irr::EET_KEY_INPUT_EVENT: {
		switch(event.KeyInput.Key) {
		case irr::KEY_KEY_R: {
			if(!event.KeyInput.PressedDown)
				mainGame->textFont->setTransparency(true);
			break;
		}
		case irr::KEY_ESCAPE: {
			mainGame->device->minimizeWindow();
			break;
		}
		}
		break;
	}
	}
	return false;
}

}
