#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <irrlicht.h>
#include "client_updater.h"
#include "game_config.h"
#include "repo_manager.h"
#include "image_downloader.h"
#include "config.h"
#include "game.h"
#include "server_lobby.h"
#include "sound_manager.h"
#include "image_manager.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "dllinterface.h"
#include "replay.h"
#include "materials.h"
#include "duelclient.h"
#include "netserver.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "CGUICustomCheckBox/CGUICustomCheckBox.h"
#include "CGUICustomTable/CGUICustomTable.h"
#include "CGUICustomTabControl/CGUICustomTabControl.h"
#include "CGUISkinSystem/CGUISkinSystem.h"
#include "CGUICustomContextMenu/CGUICustomContextMenu.h"
#include "CGUICustomContextMenu/CGUICustomMenu.h"
#include "CGUICustomText/CGUICustomText.h"
#include "CGUIFileSelectListBox/CGUIFileSelectListBox.h"
#include "CProgressBar/CProgressBar.h"
#include "ResizeablePanel/ResizeablePanel.h"
#include "CGUITTFont/CGUITTFont.h"
#include "CGUIImageButton/CGUIImageButton.h"
#include "logging.h"
#include "utils_gui.h"
#include "custom_skin_enum.h"

#ifdef __ANDROID__
#include "CGUICustomComboBox/CGUICustomComboBox.h"
#define ADDComboBox(...) (gGameConfig->native_mouse ? env->addComboBox(__VA_ARGS__): irr::gui::CGUICustomComboBox::addCustomComboBox(env, __VA_ARGS__))
#define MATERIAL_GUARD(f) do {driver->enableMaterial2D(true); f; driver->enableMaterial2D(false);} while(false);
#else
#define ADDComboBox(...) env->addComboBox(__VA_ARGS__)
#define MATERIAL_GUARD(f) do {f;} while(false);
#endif

unsigned short PRO_VERSION = 0x1351;

namespace ygo {

bool Game::Initialize() {
	srand(time(0));
	dpi_scale = gGameConfig->dpi_scale;
	if(!device) {
		try {
			device = GUIUtils::CreateDevice(gGameConfig);
		} catch (...) {
			ErrorLog("Failed to create Irrlicht Engine device!");
			return false;
		}
	}
#ifndef __ANDROID__
	device->enableDragDrop(true, [](irr::core::vector2di pos, bool isFile) ->bool {
		if(isFile) {
			if(mainGame->dInfo.isInDuel || mainGame->dInfo.isInLobby || mainGame->is_siding
			   || mainGame->wRoomListPlaceholder->isVisible() || mainGame->wLanWindow->isVisible()
			   || mainGame->wCreateHost->isVisible() || mainGame->wHostPrepare->isVisible())
				return false;
			else
				return true;
		} else {
			auto elem = mainGame->env->getRootGUIElement()->getElementFromPoint(pos);
			if(elem && elem != mainGame->env->getRootGUIElement()) {
				if(elem->hasType(irr::gui::EGUIET_EDIT_BOX) && elem->isEnabled())
					return true;
				return false;
			}
			irr::core::position2di convpos = mainGame->Resize(pos.X, pos.Y, true);
			auto x = convpos.X;
			auto y = convpos.Y;
			if(mainGame->is_building && !mainGame->is_siding) {
				if(x >= 314 && x <= 794) {
					if((y >= 164 && y <= 435) || (y >= 466 && y <= 530) || (y >= 564 && y <= 628))
						return true;
				}
			}
		}
		return false;
	});
#endif
	filesystem = device->getFileSystem();
	coreloaded = true;
#ifdef YGOPRO_BUILD_DLL
	if(!(ocgcore = LoadOCGcore(gGameConfig->working_directory + EPRO_TEXT("./"))) && !(ocgcore = LoadOCGcore(gGameConfig->working_directory + EPRO_TEXT("./expansions/"))))
		coreloaded = false;
#endif
	skinSystem = new CGUISkinSystem((gGameConfig->working_directory + EPRO_TEXT("./skin")).c_str(), device);
	if(!skinSystem)
		ErrorLog("Couldn't create skin system");
	linePatternD3D = 0;
	linePatternGL = 0x0f0f;
	waitFrame = 0.0f;
	signalFrame = 0;
	showcard = 0;
	is_attacking = false;
	lpframe = 0;
	lpcstring = L"";
	always_chain = false;
	ignore_chain = false;
	chain_when_avail = false;
	is_building = false;
	showingcard = 0;
	cardimagetextureloading = false;
	menuHandler.prev_operation = 0;
	menuHandler.prev_sel = -1;
	dInfo = {};
	memset(chatTiming, 0, sizeof(chatTiming));
	driver = device->getVideoDriver();
	imageManager.SetDevice(device);
	if(!imageManager.Initial()) {
		ErrorLog("Failed to load textures!");
		return false;
	}
	RefreshAiDecks();
	discord.Initialize(filesystem->getWorkingDirectory().c_str());
	if(gGameConfig->discordIntegration)
		discord.UpdatePresence(DiscordWrapper::INITIALIZE);
	PopulateResourcesDirectories();
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gGameConfig->numfont.c_str(), Scale(16), {});
	adFont = irr::gui::CGUITTFont::createTTFont(env, gGameConfig->numfont.c_str(), Scale(12), {});
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gGameConfig->numfont.c_str(), Scale(48), {});
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gGameConfig->textfont.c_str(), Scale(gGameConfig->textfontsize), {});
	textFont = guiFont;
	if(!numFont || !textFont || !adFont || !lpcFont || !guiFont) {
		ErrorLog("Failed to load font(s)!");
		return false;
	}
	if(!ApplySkin(gGameConfig->skin, false, true)) {
		gGameConfig->skin = NoSkinLabel();
	}
	smgr = device->getSceneManager();
	wCommitsLog = env->addWindow(Scale(0, 0, 500 + 10, 400 + 35 + 35), false, gDataManager->GetSysString(1209).c_str());
	defaultStrings.emplace_back(wCommitsLog, 1209);
	wCommitsLog->setVisible(false);
	wCommitsLog->getCloseButton()->setEnabled(false);
	wCommitsLog->getCloseButton()->setVisible(false);
	stCommitLog = irr::gui::CGUICustomText::addCustomText(L"", false, env, wCommitsLog, -1, Scale(5, 30, 505, 430));
	stCommitLog->setWordWrap(true);
	((irr::gui::CGUICustomText*)stCommitLog)->enableScrollBar();
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stCommitLog)->setTouchControl(!gGameConfig->native_mouse);
#endif
	btnCommitLogExit = env->addButton(Scale(215, 435, 285, 460), wCommitsLog, BUTTON_REPO_CHANGELOG_EXIT, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCommitLogExit, 1211);
	chkCommitLogExpand = env->addCheckBox(false, Scale(295, 435, 500, 460), wCommitsLog, BUTTON_REPO_CHANGELOG_EXPAND, gDataManager->GetSysString(1447).c_str());
	defaultStrings.emplace_back(chkCommitLogExpand, 1447);
	mTopMenu = irr::gui::CGUICustomMenu::addCustomMenu(env);
	mRepositoriesInfo = mTopMenu->getSubMenu(mTopMenu->addItem(gDataManager->GetSysString(2045).c_str(), 1, true, true));
	mAbout = mTopMenu->getSubMenu(mTopMenu->addItem(gDataManager->GetSysString(1970).c_str(), 2, true, true));
	wAbout = env->addWindow(Scale(0, 0, 450, 700), false, L"", mAbout);
	wAbout->getCloseButton()->setVisible(false);
	wAbout->setDraggable(false);
	wAbout->setDrawTitlebar(false);
	wAbout->setDrawBackground(false);
	stAbout = irr::gui::CGUICustomText::addCustomText(L"Project Ignis: EDOPro\n"
											L"The bleeding-edge automatic duel simulator\n"
											L"\n"
											L"Copyright (C) 2020  Edoardo Lolletti (edo9300) and others\n"
											L"Card scripts and supporting resources by Project Ignis.\n"
											L"https://github.com/edo9300/edopro\n"
											L"https://github.com/edo9300/ygopro-core\n"
											L"https://github.com/ProjectIgnis/CardScripts\n"
											L"https://github.com/ProjectIgnis/BabelCDB\n"
											L"https://github.com/ProjectIgnis/windbot\n"
                                            L"Software components licensed under the GNU AGPLv3 or later. See LICENSE for more details.\n"
											L"Supporting resources and app icon are distributed under separate licenses in their subfolders.\n"
											L"\n"
											L"Project Ignis:\n"
											L"ahtelel, AlphaKretin, AndreOliveiraMendes, Cybercatman, Dragon3989, DyXel, edo9300, "
											L"EerieCode, Gideon, Hatter, Hel, Icematoro, kevinlul, Larry126, LogicalNonsense, "
											L"NaimSantos, pyrQ, Sanct, senpaizuri, Steeldarkeagel, Tungnon, WolfOfWolves, Yamato\n"
											L"Default background and icon: LogicalNonsense\n"
											L"Default fields: Icematoro\n"
											L"\n"
											L"Forked from Fluorohydride's YGOPro, maintainers DailyShana, mercury233.\n"
											L"Yu-Gi-Oh! is a trademark of Shueisha and Konami.\n"
											L"This project is not affiliated with or endorsed by Shueisha or Konami.", false, env, wAbout, -1, Scale(10, 10, 440, 690));
	((irr::gui::CGUICustomText*)stAbout)->enableScrollBar();
	((irr::gui::CGUICustomText*)stAbout)->setWordWrap(true);
	((irr::gui::CGUICustomContextMenu*)mAbout)->addItem(wAbout, -1);
	wAbout->setRelativePosition(irr::core::recti(0, 0, std::min(Scale(450), stAbout->getTextWidth() + Scale(20)), std::min(stAbout->getTextHeight() + Scale(20), Scale(700))));
	mVersion = mTopMenu->getSubMenu(mTopMenu->addItem(gDataManager->GetSysString(2040).c_str(), 3, true, true));
	wVersion = env->addWindow(Scale(0, 0, 300, 135), false, L"", mVersion);
	wVersion->getCloseButton()->setVisible(false);
	wVersion->setDraggable(false);
	wVersion->setDrawTitlebar(false);
	wVersion->setDrawBackground(false);
	auto formatVersion = []() {
		return fmt::format(L"Project Ignis: EDOPro | {}.{}.{} \"{}\"", EDOPRO_VERSION_MAJOR, EDOPRO_VERSION_MINOR, EDOPRO_VERSION_PATCH, EDOPRO_VERSION_CODENAME);
	};
	stVersion = env->addStaticText(formatVersion().c_str(), Scale(10, 10, 290, 35), false, false, wVersion);
	int titleWidth = stVersion->getTextWidth();
	stVersion->setRelativePosition(irr::core::recti(Scale(10), Scale(10), titleWidth + Scale(10), Scale(35)));
	stCoreVersion = env->addStaticText(L"", Scale(10, 40, 500, 65), false, false, wVersion);
	RefreshUICoreVersion();
	stExpectedCoreVersion = env->addStaticText(
		GetLocalizedExpectedCore().c_str(),
		Scale(10, 70, 290, 95), false, true, wVersion);
	stCompatVersion = env->addStaticText(
		GetLocalizedCompatVersion().c_str(),
		Scale(10, 100, 290, 125), false, true, wVersion);
	((irr::gui::CGUICustomContextMenu*)mVersion)->addItem(wVersion, -1);
	//main menu
	int mainMenuWidth = std::max(280, static_cast<int>(titleWidth / dpi_scale + 15));
	mainMenuLeftX = 510 - mainMenuWidth / 2;
	mainMenuRightX = 510 + mainMenuWidth / 2;
	wMainMenu = env->addWindow(Scale(mainMenuLeftX, 200, mainMenuRightX, 450), false, formatVersion().c_str());
	wMainMenu->getCloseButton()->setVisible(false);
	//wMainMenu->setVisible(!is_from_discord);
#define OFFSET(x1, y1, x2, y2) Scale(10, 30 + offset, mainMenuWidth - 10, 60 + offset)
	int offset = 0;
	btnOnlineMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_ONLINE_MULTIPLAYER, gDataManager->GetSysString(2042).c_str());
	defaultStrings.emplace_back(btnOnlineMode, 2042);
	offset += 35;
	btnLanMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, gDataManager->GetSysString(1200).c_str());
	defaultStrings.emplace_back(btnLanMode, 1200);
	offset += 35;
	btnSingleMode = env->addButton(OFFSET(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, gDataManager->GetSysString(1201).c_str());
	defaultStrings.emplace_back(btnSingleMode, 1201);
	offset += 35;
	btnReplayMode = env->addButton(OFFSET(10, 100, 270, 130), wMainMenu, BUTTON_REPLAY_MODE, gDataManager->GetSysString(1202).c_str());
	defaultStrings.emplace_back(btnReplayMode, 1202);
	offset += 35;
	btnDeckEdit = env->addButton(OFFSET(10, 135, 270, 165), wMainMenu, BUTTON_DECK_EDIT, gDataManager->GetSysString(1204).c_str());
	defaultStrings.emplace_back(btnDeckEdit, 1204);
	offset += 35;
	btnModeExit = env->addButton(OFFSET(10, 170, 270, 200), wMainMenu, BUTTON_MODE_EXIT, gDataManager->GetSysString(1210).c_str());
	defaultStrings.emplace_back(btnModeExit, 1210);
	offset += 35;
#undef OFFSET
	btnSingleMode->setEnabled(coreloaded);
	//lan mode
	wLanWindow = env->addWindow(Scale(220, 100, 800, 520), false, gDataManager->GetSysString(1200).c_str());
	defaultStrings.emplace_back(wLanWindow, 1200);
	wLanWindow->getCloseButton()->setVisible(false);
	wLanWindow->setVisible(false);
	irr::gui::IGUIElement* tmpptr = env->addStaticText(gDataManager->GetSysString(1220).c_str(), Scale(10, 30, 220, 50), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1220);
	ebNickName = env->addEditBox(gGameConfig->nickname.c_str(), Scale(110, 25, 450, 50), true, wLanWindow, EDITBOX_NICKNAME);
	ebNickName->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	lstHostList = env->addListBox(Scale(10, 60, 570, 320), wLanWindow, LISTBOX_LAN_HOST, true);
	lstHostList->setItemHeight(Scale(18));
	btnLanRefresh = env->addButton(Scale(240, 325, 340, 350), wLanWindow, BUTTON_LAN_REFRESH, gDataManager->GetSysString(1217).c_str());
	defaultStrings.emplace_back(btnLanRefresh, 1217);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1221).c_str(), Scale(10, 360, 220, 380), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1221);
	ebJoinHost = env->addEditBox(gGameConfig->lasthost.c_str(), Scale(110, 355, 350, 380), true, wLanWindow);
	ebJoinHost->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(gGameConfig->lastport.c_str(), Scale(360, 355, 420, 380), true, wLanWindow, EDITBOX_PORT_BOX);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1222).c_str(), Scale(10, 390, 220, 410), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1222);
	ebJoinPass = env->addEditBox(gGameConfig->roompass.c_str(), Scale(110, 385, 420, 410), true, wLanWindow);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnJoinHost = env->addButton(Scale(460, 355, 570, 380), wLanWindow, BUTTON_JOIN_HOST, gDataManager->GetSysString(1223).c_str());
	defaultStrings.emplace_back(btnJoinHost, 1223);
	btnJoinCancel = env->addButton(Scale(460, 385, 570, 410), wLanWindow, BUTTON_JOIN_CANCEL, gDataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnJoinCancel, 1212);
	btnCreateHost = env->addButton(Scale(460, 25, 570, 50), wLanWindow, BUTTON_CREATE_HOST, gDataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(btnCreateHost, 1224);
	btnCreateHost->setEnabled(coreloaded);
	//create host
	wCreateHost = env->addWindow(Scale(320, 100, 700, 520), false, gDataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(wCreateHost, 1224);
	wCreateHost->getCloseButton()->setVisible(false);
	wCreateHost->setVisible(false);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1226).c_str(), Scale(20, 30, 220, 50), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1226);
	cbHostLFList = ADDComboBox(Scale(140, 25, 300, 50), wCreateHost, COMBOBOX_HOST_LFLIST);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1225).c_str(), Scale(20, 60, 220, 80), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1225);
	cbRule = ADDComboBox(Scale(140, 55, 300, 80), wCreateHost);
	ReloadCBRule();
	cbRule->setSelected(gGameConfig->lastallowedcards);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1227).c_str(), Scale(20, 90, 220, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1227);
#define WStr(i) std::to_wstring(i).c_str()
	ebTeam1 = env->addEditBox(WStr(gGameConfig->team1count), Scale(140, 85, 170, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	auto vsstring = env->addStaticText(gDataManager->GetSysString(1380).c_str(), Scale(175, 85, 195, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(vsstring, 1380);
	vsstring->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebTeam2 = env->addEditBox(WStr(gGameConfig->team2count), Scale(200, 85, 230, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	vsstring = env->addStaticText(gDataManager->GetSysString(1381).c_str(), Scale(235, 85, 280, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(vsstring, 1381);
	vsstring->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	ebBestOf = env->addEditBox(WStr(gGameConfig->bestOf), Scale(285, 85, 315, 110), true, wCreateHost, EDITBOX_NUMERIC);
	ebBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRelayMode = env->addButton(Scale(325, 85, 370, 110), wCreateHost, -1, gDataManager->GetSysString(1247).c_str());
	defaultStrings.emplace_back(btnRelayMode, 1247);
	btnRelayMode->setIsPushButton(true);
	btnRelayMode->setPressed(gGameConfig->relayDuel);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1237).c_str(), Scale(20, 120, 320, 140), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1237);
	ebTimeLimit = env->addEditBox(WStr(gGameConfig->timeLimit), Scale(140, 115, 220, 140), true, wCreateHost, EDITBOX_NUMERIC);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1228).c_str(), Scale(20, 150, 320, 170), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1228);
	btnRuleCards = env->addButton(Scale(260, 325, 370, 350), wCreateHost, BUTTON_RULE_CARDS, gDataManager->GetSysString(1625).c_str());
	defaultStrings.emplace_back(btnRuleCards, 1625);
	wRules = env->addWindow(Scale(630, 100, 1000, 310), false, L"");
	wRules->getCloseButton()->setVisible(false);
	wRules->setDrawTitlebar(false);
	wRules->setDraggable(true);
	wRules->setVisible(false);
	btnRulesOK = env->addButton(Scale(135, 175, 235, 200), wRules, BUTTON_RULE_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnRulesOK, 1211);
	for(int i = 0; i < (sizeof(chkRules) / sizeof(irr::gui::IGUICheckBox*)); ++i) {
		chkRules[i] = env->addCheckBox(false, Scale(10 + (i % 2) * 150, 10 + (i / 2) * 20, 200 + (i % 2) * 120, 30 + (i / 2) * 20), wRules, CHECKBOX_EXTRA_RULE, gDataManager->GetSysString(1132 + i).c_str());
		defaultStrings.emplace_back(chkRules[i], 1132 + i);
	}
	extra_rules = gGameConfig->lastExtraRules;
	UpdateExtraRules(true);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1236).c_str(), Scale(20, 180, 220, 200), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1236);
	cbDuelRule = ADDComboBox(Scale(140, 175, 300, 200), wCreateHost, COMBOBOX_DUEL_RULE);
	duel_param = gGameConfig->lastDuelParam;
	forbiddentypes = gGameConfig->lastDuelForbidden;
	btnCustomRule = env->addButton(Scale(305, 175, 370, 200), wCreateHost, BUTTON_CUSTOM_RULE, gDataManager->GetSysString(1626).c_str());
	defaultStrings.emplace_back(btnCustomRule, 1626);
	wCustomRulesL = env->addWindow(Scale(20, 100, 320, 430), false, L"");
	wCustomRulesL->getCloseButton()->setVisible(false);
	wCustomRulesL->setDrawTitlebar(false);
	wCustomRulesL->setVisible(false);
	wCustomRulesR = env->addWindow(Scale(700, 100, 1000, 430), false, L"");
	wCustomRulesR->getCloseButton()->setVisible(false);
	wCustomRulesR->setDrawTitlebar(false);
	wCustomRulesR->setVisible(false);
	int spacingL = 0, spacingR = 0;
	tmpptr = env->addStaticText(gDataManager->GetSysString(1629).c_str(), Scale(10, 10 + spacingL * 20, 290, 30 + spacingL * 20), false, false, wCustomRulesL);
	defaultStrings.emplace_back(tmpptr, 1629);
	spacingL++;
	tmpptr = env->addStaticText(gDataManager->GetSysString(1629).c_str(), Scale(10, 10 + spacingR * 20, 290, 30 + spacingR * 20), false, false, wCustomRulesR);
	defaultStrings.emplace_back(tmpptr, 1629);
	spacingR++;
	for(int i = 0; i < 7; ++i, ++spacingR) {
		chkCustomRules[i] = env->addCheckBox(duel_param & 0x100<<i, Scale(10, 10 + spacingR * 20, 290, 30 + spacingR * 20), wCustomRulesR, CHECKBOX_OBSOLETE + i, gDataManager->GetSysString(1631 + i).c_str());
		defaultStrings.emplace_back(chkCustomRules[i], 1631 + i);
	}
	for(int i = 7; i < schkCustomRules; ++i, ++spacingL) {
		chkCustomRules[i] = env->addCheckBox(duel_param & 0x100 << i, Scale(10, 10 + spacingL * 20, 290, 30 + spacingL * 20), wCustomRulesL, CHECKBOX_OBSOLETE + i, gDataManager->GetSysString(1631 + i).c_str());
		defaultStrings.emplace_back(chkCustomRules[i], 1631 + i);
	}
	tmpptr = env->addStaticText(gDataManager->GetSysString(1628).c_str(), Scale(10, 10 + spacingR * 20, 290, 30 + spacingR * 20), false, false, wCustomRulesR);
	defaultStrings.emplace_back(tmpptr, 1628);
	const uint32 limits[] = { TYPE_FUSION, TYPE_SYNCHRO, TYPE_XYZ, TYPE_PENDULUM, TYPE_LINK };
#define TYPECHK(id,stringid) spacingR++;\
	chkTypeLimit[id] = env->addCheckBox(forbiddentypes & limits[id], Scale(10, 10 + spacingR * 20, 290, 30 + spacingR * 20), wCustomRulesR, -1, fmt::sprintf(gDataManager->GetSysString(1627), gDataManager->GetSysString(stringid)).c_str());
	TYPECHK(0, 1056);
	TYPECHK(1, 1063);
	TYPECHK(2, 1073);
	TYPECHK(3, 1074);
	TYPECHK(4, 1076);
#undef TYPECHK
	UpdateDuelParam();
	btnCustomRulesOK = env->addButton(Scale(55, 290, 155, 315), wCustomRulesR, BUTTON_CUSTOM_RULE_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCustomRulesOK, 1211);
	chkNoCheckDeck = env->addCheckBox(gGameConfig->noCheckDeck, Scale(20, 210, 170, 230), wCreateHost, -1, gDataManager->GetSysString(1229).c_str());
	defaultStrings.emplace_back(chkNoCheckDeck, 1229);
	chkNoShuffleDeck = env->addCheckBox(gGameConfig->noShuffleDeck, Scale(180, 210, 360, 230), wCreateHost, -1, gDataManager->GetSysString(1230).c_str());
	defaultStrings.emplace_back(chkNoShuffleDeck, 1230);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1231).c_str(), Scale(20, 240, 320, 260), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1231);
	ebStartLP = env->addEditBox(WStr(gGameConfig->startLP), Scale(140, 235, 220, 260), true, wCreateHost, EDITBOX_NUMERIC);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1232).c_str(), Scale(20, 270, 320, 290), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1232);
	ebStartHand = env->addEditBox(WStr(gGameConfig->startHand), Scale(140, 265, 220, 290), true, wCreateHost, EDITBOX_NUMERIC);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(gDataManager->GetSysString(1233).c_str(), Scale(20, 300, 320, 320), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1233);
	ebDrawCount = env->addEditBox(WStr(gGameConfig->drawCount), Scale(140, 295, 220, 320), true, wCreateHost, EDITBOX_NUMERIC);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
#undef WStr
	tmpptr = env->addStaticText(gDataManager->GetSysString(1234).c_str(), Scale(10, 330, 220, 350), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1234);
	ebServerName = env->addEditBox(gGameConfig->gamename.c_str(), Scale(110, 325, 250, 350), true, wCreateHost);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(1235).c_str(), Scale(10, 360, 220, 380), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1235);
	ebServerPass = env->addEditBox(L"", Scale(110, 355, 250, 380), true, wCreateHost);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnHostConfirm = env->addButton(Scale(260, 355, 370, 380), wCreateHost, BUTTON_HOST_CONFIRM, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnHostConfirm, 1211);
	btnHostCancel = env->addButton(Scale(260, 385, 370, 410), wCreateHost, BUTTON_HOST_CANCEL, gDataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnHostCancel, 1212);
	stHostPort = env->addStaticText(gDataManager->GetSysString(1238).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	defaultStrings.emplace_back(stHostPort, 1238);
	ebHostPort = env->addEditBox(gGameConfig->serverport.c_str(), Scale(110, 385, 250, 410), true, wCreateHost, EDITBOX_PORT_BOX);
	ebHostPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHostNotes = env->addStaticText(gDataManager->GetSysString(2024).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	defaultStrings.emplace_back(stHostNotes, 2024);
	stHostNotes->setVisible(false);
	ebHostNotes = env->addEditBox(L"", Scale(110, 385, 250, 410), true, wCreateHost);
	ebHostNotes->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebHostNotes->setVisible(false);
	//host(single)
	wHostPrepare = env->addWindow(Scale(270, 120, 750, 440), false, gDataManager->GetSysString(1250).c_str());
	defaultStrings.emplace_back(wHostPrepare, 1250);
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	wHostPrepareR = env->addWindow(Scale(750, 120, 950, 440), false, gDataManager->GetSysString(1625).c_str());
	defaultStrings.emplace_back(wHostPrepareR, 1625);
	wHostPrepareR->getCloseButton()->setVisible(false);
	wHostPrepareR->setVisible(false);
	wHostPrepareL = env->addWindow(Scale(70, 120, 270, 440), false, gDataManager->GetSysString(1628).c_str());
	defaultStrings.emplace_back(wHostPrepareL, 1628);
	wHostPrepareL->getCloseButton()->setVisible(false);
	wHostPrepareL->setVisible(false);
	auto wHostPrepareRrect = wHostPrepareR->getClientRect();
	wHostPrepareRrect.UpperLeftCorner.X += Scale(10);
	wHostPrepareRrect.LowerRightCorner.X -= Scale(10);
	stHostPrepRuleR = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepareR, -1, wHostPrepareRrect);
	((irr::gui::CGUICustomText*)stHostPrepRuleR)->enableScrollBar();
	stHostPrepRuleR->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stHostPrepRuleR)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stHostPrepRuleL = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepareL, -1, wHostPrepareRrect);
	((irr::gui::CGUICustomText*)stHostPrepRuleL)->enableScrollBar();
	stHostPrepRuleL->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stHostPrepRuleL)->setTouchControl(!gGameConfig->native_mouse);
#endif
	btnHostPrepDuelist = env->addButton(Scale(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, gDataManager->GetSysString(1251).c_str());
	defaultStrings.emplace_back(btnHostPrepDuelist, 1251);
	btnHostPrepWindBot = env->addButton(Scale(170, 30, 270, 55), wHostPrepare, BUTTON_HP_AI_TOGGLE, gDataManager->GetSysString(2050).c_str());
	defaultStrings.emplace_back(btnHostPrepWindBot, 2050);
	for(int i = 0; i < 6; ++i) {
		btnHostPrepKick[i] = env->addButton(Scale(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		stHostPrepDuelist[i] = env->addStaticText(L"", Scale(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		chkHostPrepReady[i] = env->addCheckBox(false, Scale(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	gBot.window = env->addWindow(Scale(750, 120, 960, 360), false, gDataManager->GetSysString(2051).c_str());
	defaultStrings.emplace_back(gBot.window, 2051);
	gBot.window->getCloseButton()->setVisible(false);
	gBot.window->setVisible(false);
	gBot.deckProperties = env->addStaticText(L"", Scale(10, 25, 200, 100), true, true, gBot.window);
	gBot.chkThrowRock = env->addCheckBox(gGameConfig->botThrowRock, Scale(10, 105, 200, 130), gBot.window, -1, gDataManager->GetSysString(2052).c_str());
	defaultStrings.emplace_back(gBot.chkThrowRock, 2052);
	gBot.chkMute = env->addCheckBox(gGameConfig->botMute, Scale(10, 135, 200, 160), gBot.window, -1, gDataManager->GetSysString(2053).c_str());
	defaultStrings.emplace_back(gBot.chkMute, 2053);
	gBot.cbBotDeck = ADDComboBox(Scale(10, 165, 200, 190), gBot.window, COMBOBOX_BOT_DECK);
	gBot.btnAdd = env->addButton(Scale(10, 200, 200, 225), gBot.window, BUTTON_BOT_ADD, gDataManager->GetSysString(2054).c_str());
	defaultStrings.emplace_back(gBot.btnAdd, 2054);
	btnHostPrepOB = env->addButton(Scale(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, gDataManager->GetSysString(1252).c_str());
	defaultStrings.emplace_back(btnHostPrepOB, 1252);
	stHostPrepOB = env->addStaticText(fmt::format(L"{} 0", gDataManager->GetSysString(1253)).c_str(), Scale(10, 210, 270, 230), false, false, wHostPrepare);
	defaultStrings.emplace_back(stHostPrepOB, 1253);
	stHostPrepRule = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare, -1, Scale(280, 30, 460, 230));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stHostPrepRule)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stHostPrepRule->setWordWrap(true);
	stDeckSelect = env->addStaticText(gDataManager->GetSysString(1254).c_str(), Scale(10, 235, 110, 255), false, false, wHostPrepare);
	defaultStrings.emplace_back(stDeckSelect, 1254);
	cbDeckSelect = ADDComboBox(Scale(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	cbDeckSelect2 = ADDComboBox(Scale(280, 230, 430, 255), wHostPrepare);
	cbDeckSelect2->setMaxSelectionRows(10);
	btnHostPrepReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_READY, gDataManager->GetSysString(1218).c_str());
	defaultStrings.emplace_back(btnHostPrepReady, 1218);
	btnHostPrepNotReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_NOTREADY, gDataManager->GetSysString(1219).c_str());
	defaultStrings.emplace_back(btnHostPrepNotReady, 1219);
	btnHostPrepNotReady->setVisible(false);
	btnHostPrepStart = env->addButton(Scale(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, gDataManager->GetSysString(1215).c_str());
	defaultStrings.emplace_back(btnHostPrepStart, 1215);
	btnHostPrepCancel = env->addButton(Scale(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, gDataManager->GetSysString(1210).c_str());
	defaultStrings.emplace_back(btnHostPrepCancel, 1210);
	//img
	wCardImg = env->addStaticText(L"", Scale(1, 1, 1 + CARD_IMG_WIDTH + 20, 1 + CARD_IMG_HEIGHT + 18), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(skin::CARDINFO_IMAGE_BACKGROUND_VAL);
	wCardImg->setVisible(false);
	imgCard = env->addImage(Scale(10, 9, 10 + CARD_IMG_WIDTH, 9 + CARD_IMG_HEIGHT), wCardImg);
	imgCard->setImage(imageManager.tCover[0]);
	imgCard->setScaleImage(true);
	imgCard->setUseAlphaChannel(true);
	//phase
	wPhase = env->addStaticText(L"", Scale(480, 310, 855, 330));
	wPhase->setVisible(false);
	btnDP = env->addButton(Scale(0, 0, 50, 20), wPhase, -1, L"\xff24\xff30");
	btnDP->setEnabled(false);
	btnDP->setPressed(true);
	btnDP->setVisible(false);
	btnSP = env->addButton(Scale(0, 0, 50, 20), wPhase, -1, L"\xff33\xff30");
	btnSP->setEnabled(false);
	btnSP->setPressed(true);
	btnSP->setVisible(false);
	btnM1 = env->addButton(Scale(160, 0, 210, 20), wPhase, -1, L"\xff2d\xff11");
	btnM1->setEnabled(false);
	btnM1->setPressed(true);
	btnM1->setVisible(false);
	btnBP = env->addButton(Scale(160, 0, 210, 20), wPhase, BUTTON_BP, L"\xff22\xff30");
	btnBP->setVisible(false);
	btnM2 = env->addButton(Scale(160, 0, 210, 20), wPhase, BUTTON_M2, L"\xff2d\xff12");
	btnM2->setVisible(false);
	btnEP = env->addButton(Scale(320, 0, 370, 20), wPhase, BUTTON_EP, L"\xff25\xff30");
	btnEP->setVisible(false);
	//tab
	infosExpanded = 0;
	wInfos = irr::gui::CGUICustomTabControl::addCustomTabControl(env, Scale(1, 275, 301, 639), 0, true);
	wInfos->setVisible(false);
	//info
	irr::gui::IGUITab* tabInfo = wInfos->addTab(gDataManager->GetSysString(1270).c_str());
	defaultStrings.emplace_back(tabInfo, 1270);
	stName = irr::gui::CGUICustomText::addCustomText(L"", true, env, tabInfo, -1, Scale(10, 10, 287, 32));
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	((irr::gui::CGUICustomText*)stName)->setTextAutoScrolling(irr::gui::CGUICustomText::LEFT_TO_RIGHT_BOUNCING, 0, 1.0f, 0, 120, 300);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stName)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 37, 287, 60));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stInfo)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stInfo->setWordWrap(true);
	stInfo->setOverrideColor(skin::CARDINFO_TYPES_COLOR_VAL);
	stDataInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 60, 287, 83));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stDataInfo)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stDataInfo->setWordWrap(true);
	stDataInfo->setOverrideColor(skin::CARDINFO_STATS_COLOR_VAL);
	stSetName = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 83, 287, 106));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stSetName)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stSetName->setWordWrap(true);
	stSetName->setOverrideColor(skin::CARDINFO_ARCHETYPE_TEXT_COLOR_VAL);
	stSetName->setVisible(!gGameConfig->chkHideSetname);
	stPasscodeScope = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 106, 287, 129));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stPasscodeScope)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stPasscodeScope->setWordWrap(true);
	stPasscodeScope->setOverrideColor(skin::CARDINFO_PASSCODE_SCOPE_TEXT_COLOR_VAL);
	stPasscodeScope->setVisible(!gGameConfig->hidePasscodeScope);
	stText = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 129, 287, 324));
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stText)->setTouchControl(!gGameConfig->native_mouse);
#endif
	((irr::gui::CGUICustomText*)stText)->enableScrollBar();
	stText->setWordWrap(true);
	//log
	tabLog =  wInfos->addTab(gDataManager->GetSysString(1271).c_str());
	defaultStrings.emplace_back(tabLog, 1271);
	lstLog = env->addListBox(Scale(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(Scale(18));
	btnClearLog = env->addButton(Scale(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, gDataManager->GetSysString(1272).c_str());
	defaultStrings.emplace_back(btnClearLog, 1272);
	btnExpandLog = env->addButton(Scale(40, 300, 140, 325), tabLog, BUTTON_EXPAND_INFOBOX, gDataManager->GetSysString(2043).c_str());
	defaultStrings.emplace_back(btnExpandLog, 2043);
	//chat
	tabChat = wInfos->addTab(gDataManager->GetSysString(1279).c_str());
	defaultStrings.emplace_back(tabChat, 1279);
	lstChat = env->addListBox(Scale(10, 10, 290, 290), tabChat, -1, false);
	lstChat->setItemHeight(Scale(18));
	btnClearChat = env->addButton(Scale(160, 300, 260, 325), tabChat, BUTTON_CLEAR_CHAT, gDataManager->GetSysString(1282).c_str());
	defaultStrings.emplace_back(btnClearChat, 1282);
	btnExpandChat = env->addButton(Scale(40, 300, 140, 325), tabChat, BUTTON_EXPAND_INFOBOX, gDataManager->GetSysString(2043).c_str());
	defaultStrings.emplace_back(btnExpandChat, 2043);
	//system
	irr::gui::IGUITab* _tabSystem = wInfos->addTab(gDataManager->GetSysString(1273).c_str());
	defaultStrings.emplace_back(_tabSystem, 1273);
	tabSystem = irr::gui::Panel::addPanel(env, _tabSystem, -1, Scale(0, 0, wInfos->getRelativePosition().getWidth() + 1, wInfos->getRelativePosition().getHeight()), true, false);
	auto tabPanel = tabSystem->getSubpanel();
	tabSettings.chkIgnoreOpponents = env->addCheckBox(gGameConfig->chkIgnore1, Scale(20, 20, 280, 45), tabPanel, -1, gDataManager->GetSysString(1290).c_str());
	defaultStrings.emplace_back(tabSettings.chkIgnoreOpponents, 1290);
	tabSettings.chkIgnoreSpectators = env->addCheckBox(gGameConfig->chkIgnore2, Scale(20, 50, 280, 75), tabPanel, -1, gDataManager->GetSysString(1291).c_str());
	defaultStrings.emplace_back(tabSettings.chkIgnoreSpectators, 1291);
	tabSettings.chkQuickAnimation = env->addCheckBox(gGameConfig->quick_animation, Scale(20, 80, 300, 105), tabPanel, CHECKBOX_QUICK_ANIMATION, gDataManager->GetSysString(1299).c_str());
	defaultStrings.emplace_back(tabSettings.chkQuickAnimation, 1299);
	tabSettings.chkAlternativePhaseLayout = env->addCheckBox(gGameConfig->alternative_phase_layout, Scale(20, 110, 300, 135), tabPanel, CHECKBOX_ALTERNATIVE_PHASE_LAYOUT, gDataManager->GetSysString(1298).c_str());
	defaultStrings.emplace_back(tabSettings.chkAlternativePhaseLayout, 1298);
	tabSettings.chkHideChainButtons = env->addCheckBox(gGameConfig->chkHideHintButton, Scale(20, 140, 280, 165), tabPanel, CHECKBOX_CHAIN_BUTTONS, gDataManager->GetSysString(1355).c_str());
	defaultStrings.emplace_back(tabSettings.chkHideChainButtons, 1355);
	tabSettings.chkAutoChainOrder = env->addCheckBox(gGameConfig->chkAutoChain, Scale(20, 170, 280, 195), tabPanel, -1, gDataManager->GetSysString(1276).c_str());
	defaultStrings.emplace_back(tabSettings.chkAutoChainOrder, 1276);
	tabSettings.chkNoChainDelay = env->addCheckBox(gGameConfig->chkWaitChain, Scale(20, 200, 280, 225), tabPanel, -1, gDataManager->GetSysString(1277).c_str());
	defaultStrings.emplace_back(tabSettings.chkNoChainDelay, 1277);
	// audio
	tabSettings.chkEnableSound = env->addCheckBox(gGameConfig->enablesound, Scale(20, 230, 280, 255), tabPanel, CHECKBOX_ENABLE_SOUND, gDataManager->GetSysString(2047).c_str());
	defaultStrings.emplace_back(tabSettings.chkEnableSound, 2047);
	tabSettings.stSoundVolume = env->addStaticText(gDataManager->GetSysString(2049).c_str(), Scale(20, 260, 80, 285), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stSoundVolume, 2049);
	tabSettings.scrSoundVolume = env->addScrollBar(true, Scale(85, 265, 280, 280), tabPanel, SCROLL_SOUND_VOLUME);
	tabSettings.scrSoundVolume->setMax(100);
	tabSettings.scrSoundVolume->setMin(0);
	tabSettings.scrSoundVolume->setPos(gGameConfig->soundVolume);
	tabSettings.scrSoundVolume->setLargeStep(1);
	tabSettings.scrSoundVolume->setSmallStep(1);
	tabSettings.chkEnableMusic = env->addCheckBox(gGameConfig->enablemusic, Scale(20, 290, 280, 315), tabPanel, CHECKBOX_ENABLE_MUSIC, gDataManager->GetSysString(2046).c_str());
	defaultStrings.emplace_back(tabSettings.chkEnableMusic, 2046);
	tabSettings.stMusicVolume = env->addStaticText(gDataManager->GetSysString(2048).c_str(), Scale(20, 320, 80, 345), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stMusicVolume, 2048);
	tabSettings.scrMusicVolume = env->addScrollBar(true, Scale(85, 325, 280, 340), tabPanel, SCROLL_MUSIC_VOLUME);
	tabSettings.scrMusicVolume->setMax(100);
	tabSettings.scrMusicVolume->setMin(0);
	tabSettings.scrMusicVolume->setPos(gGameConfig->musicVolume);
	tabSettings.scrMusicVolume->setLargeStep(1);
	tabSettings.scrMusicVolume->setSmallStep(1);
	tabSettings.stNoAudioBackend = env->addStaticText(gDataManager->GetSysString(2058).c_str(), Scale(20, 230, 280, 345), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stNoAudioBackend, 2058);
	tabSettings.stNoAudioBackend->setVisible(false);
	// end audio
	tabSettings.chkMAutoPos = env->addCheckBox(gGameConfig->chkMAutoPos, Scale(20, 350, 280, 375), tabPanel, -1, gDataManager->GetSysString(1274).c_str());
	defaultStrings.emplace_back(tabSettings.chkMAutoPos, 1274);
	tabSettings.chkSTAutoPos = env->addCheckBox(gGameConfig->chkSTAutoPos, Scale(20, 380, 280, 405), tabPanel, -1, gDataManager->GetSysString(1278).c_str());
	defaultStrings.emplace_back(tabSettings.chkSTAutoPos, 1278);
	tabSettings.chkRandomPos = env->addCheckBox(gGameConfig->chkRandomPos, Scale(40, 410, 280, 435), tabPanel, -1, gDataManager->GetSysString(1275).c_str());
	defaultStrings.emplace_back(tabSettings.chkRandomPos, 1275);
	// Check OnResize for button placement information
	btnTabShowSettings = env->addButton(Scale(20, 445, 280, 470), tabPanel, BUTTON_SHOW_SETTINGS, gDataManager->GetSysString(2059).c_str());
	defaultStrings.emplace_back(btnTabShowSettings, 2059);
	/* padding = */ env->addStaticText(L"", Scale(20, 475, 280, 485), false, true, tabPanel, -1, false);

	gSettings.window = env->addWindow(Scale(180, 85, 840, 535), false, gDataManager->GetSysString(1273).c_str());
	defaultStrings.emplace_back(gSettings.window, 1273);
	gSettings.window->setVisible(false);
	auto sRect = gSettings.window->getClientRect();
	gSettings.panel = irr::gui::Panel::addPanel(env, gSettings.window, -1, sRect, true, false);
	auto sPanel = gSettings.panel->getSubpanel();
	gSettings.chkShowScopeLabel = env->addCheckBox(gGameConfig->showScopeLabel, Scale(15, 5, 320, 30), sPanel, CHECKBOX_SHOW_SCOPE_LABEL, gDataManager->GetSysString(2076).c_str());
	defaultStrings.emplace_back(gSettings.chkShowScopeLabel, 2076);
	gSettings.chkShowFPS = env->addCheckBox(gGameConfig->showFPS, Scale(15, 35, 320, 60), sPanel, CHECKBOX_SHOW_FPS, gDataManager->GetSysString(1445).c_str());
	defaultStrings.emplace_back(gSettings.chkShowFPS, 1445);
	gSettings.chkFullscreen = env->addCheckBox(gGameConfig->fullscreen, Scale(15, 65, 320, 90), sPanel, CHECKBOX_FULLSCREEN, gDataManager->GetSysString(2060).c_str());
	defaultStrings.emplace_back(gSettings.chkFullscreen, 2060);
#ifdef __ANDROID__
	gSettings.chkFullscreen->setChecked(true);
	gSettings.chkFullscreen->setEnabled(false);
#elif defined(__APPLE__)
	gSettings.chkFullscreen->setEnabled(false);
#endif
	gSettings.chkScaleBackground = env->addCheckBox(gGameConfig->scale_background, Scale(15, 95, 320, 120), sPanel, CHECKBOX_SCALE_BACKGROUND, gDataManager->GetSysString(2061).c_str());
	defaultStrings.emplace_back(gSettings.chkScaleBackground, 2061);
	gSettings.chkAccurateBackgroundResize = env->addCheckBox(gGameConfig->accurate_bg_resize, Scale(15, 125, 320, 150), sPanel, CHECKBOX_ACCURATE_BACKGROUND_RESIZE, gDataManager->GetSysString(2062).c_str());
	defaultStrings.emplace_back(gSettings.chkAccurateBackgroundResize, 2062);
#ifdef __ANDROID__
	gSettings.chkAccurateBackgroundResize->setChecked(true);
	gSettings.chkAccurateBackgroundResize->setEnabled(false);
#endif
	gSettings.chkHideSetname = env->addCheckBox(gGameConfig->chkHideSetname, Scale(15, 155, 320, 180), sPanel, CHECKBOX_HIDE_ARCHETYPES, gDataManager->GetSysString(1354).c_str());
	defaultStrings.emplace_back(gSettings.chkHideSetname, 1354);
	gSettings.chkHidePasscodeScope = env->addCheckBox(gGameConfig->hidePasscodeScope, Scale(15, 185, 320, 210), sPanel, CHECKBOX_HIDE_PASSCODE_SCOPE, gDataManager->GetSysString(2063).c_str());
	defaultStrings.emplace_back(gSettings.chkHidePasscodeScope, 2063);
	gSettings.chkDrawFieldSpells = env->addCheckBox(gGameConfig->draw_field_spell, Scale(15, 215, 320, 240), sPanel, CHECKBOX_DRAW_FIELD_SPELLS, gDataManager->GetSysString(2068).c_str());
	defaultStrings.emplace_back(gSettings.chkDrawFieldSpells, 2068);
	gSettings.chkFilterBot = env->addCheckBox(gGameConfig->filterBot, Scale(15, 245, 320, 270), sPanel, CHECKBOX_FILTER_BOT, gDataManager->GetSysString(2069).c_str());
	defaultStrings.emplace_back(gSettings.chkFilterBot, 2069);
	gSettings.stCurrentSkin = env->addStaticText(gDataManager->GetSysString(2064).c_str(), Scale(15, 275, 90, 300), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stCurrentSkin, 2064);
	gSettings.cbCurrentSkin = ADDComboBox(Scale(95, 275, 320, 300), sPanel, COMBOBOX_CURRENT_SKIN);
	ReloadCBCurrentSkin();
	gSettings.btnReloadSkin = env->addButton(Scale(15, 305, 320, 330), sPanel, BUTTON_RELOAD_SKIN, gDataManager->GetSysString(2066).c_str());
	defaultStrings.emplace_back(gSettings.btnReloadSkin, 2066);
	gSettings.stCurrentLocale = env->addStaticText(gDataManager->GetSysString(2067).c_str(), Scale(15, 335, 90, 360), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stCurrentLocale, 2067);
	PopulateLocales();
	gSettings.cbCurrentLocale = ADDComboBox(Scale(95, 335, 320, 360), sPanel, COMBOBOX_CURRENT_LOCALE);
	int selectedLocale = gSettings.cbCurrentLocale->addItem(L"English");
	for(auto& _locale : locales) {
		auto& locale = _locale.first;
		auto itemIndex = gSettings.cbCurrentLocale->addItem(Utils::ToUnicodeIfNeeded(locale).c_str());
		if(gGameConfig->locale == locale) {
			selectedLocale = itemIndex;
		}
	}
	gSettings.cbCurrentLocale->setSelected(selectedLocale);
	gSettings.stDpiScale = env->addStaticText(gDataManager->GetSysString(2070).c_str(), Scale(15, 365, 90, 390), false, false, sPanel);
	defaultStrings.emplace_back(gSettings.stDpiScale, 2070);
	gSettings.ebDpiScale = env->addEditBox(fmt::to_wstring<int>(gGameConfig->dpi_scale * 100).c_str(), Scale(95, 365, 150, 390), true, sPanel, EDITBOX_NUMERIC);
	env->addStaticText(L"%", Scale(155, 365, 170, 390), false, false, sPanel);
	gSettings.ebDpiScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	gSettings.btnRestart = env->addButton(Scale(175, 365, 320, 390), sPanel, BUTTON_APPLY_RESTART, gDataManager->GetSysString(2071).c_str());
	defaultStrings.emplace_back(gSettings.btnRestart, 2071);

	gSettings.stAntiAlias = env->addStaticText(gDataManager->GetSysString(2075).c_str(), Scale(340, 5, 545, 30), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stAntiAlias, 2075);
	gSettings.ebAntiAlias = env->addEditBox(fmt::to_wstring(gGameConfig->antialias).c_str(), Scale(550, 5, 645, 30), true, sPanel, EDITBOX_NUMERIC);
	gSettings.chkVSync = env->addCheckBox(gGameConfig->vsync, Scale(340, 35, 645, 60), sPanel, CHECKBOX_VSYNC, gDataManager->GetSysString(2073).c_str());
	defaultStrings.emplace_back(gSettings.chkVSync, 2073);
	gSettings.stFPSCap = env->addStaticText(gDataManager->GetSysString(2074).c_str(), Scale(340, 65, 545, 90), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stFPSCap, 2074);
	gSettings.ebFPSCap = env->addEditBox(fmt::to_wstring(gGameConfig->maxFPS).c_str(), Scale(550, 65, 600, 90), true, sPanel, EDITBOX_FPS_CAP);
	gSettings.btnFPSCap = env->addButton(Scale(605, 65, 645, 90), sPanel, BUTTON_FPS_CAP, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(gSettings.btnFPSCap, 1211);
	gSettings.chkShowConsole = env->addCheckBox(gGameConfig->showConsole, Scale(340, 95, 645, 120), sPanel, -1, gDataManager->GetSysString(2072).c_str());
	defaultStrings.emplace_back(gSettings.chkShowConsole, 2072);
#ifndef _WIN32
	gSettings.chkShowConsole->setChecked(false);
	gSettings.chkShowConsole->setEnabled(false);
#endif
	gSettings.stCoreLogOutput = env->addStaticText(gDataManager->GetSysString(1998).c_str(), Scale(340, 125, 430, 150), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stCoreLogOutput, 1998);
	gSettings.cbCoreLogOutput = ADDComboBox(Scale(435, 125, 645, 150), sPanel, COMBOBOX_CORE_LOG_OUTPUT);
	ReloadCBCoreLogOutput();
	gSettings.chkSaveHandTest = env->addCheckBox(gGameConfig->saveHandTest, Scale(340, 155, 645, 180), sPanel, CHECKBOX_SAVE_HAND_TEST_REPLAY, gDataManager->GetSysString(2077).c_str());
	defaultStrings.emplace_back(gSettings.chkSaveHandTest, 2077);
	// audio
	gSettings.chkEnableSound = env->addCheckBox(gGameConfig->enablesound, Scale(340, 185, 645, 210), sPanel, CHECKBOX_ENABLE_SOUND, gDataManager->GetSysString(2047).c_str());
	defaultStrings.emplace_back(gSettings.chkEnableSound, 2047);
	gSettings.stSoundVolume = env->addStaticText(gDataManager->GetSysString(2049).c_str(), Scale(340, 215, 400, 240), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stSoundVolume, 2049);
	gSettings.scrSoundVolume = env->addScrollBar(true, Scale(405, 215, 645, 235), sPanel, SCROLL_SOUND_VOLUME);
	gSettings.scrSoundVolume->setMax(100);
	gSettings.scrSoundVolume->setMin(0);
	gSettings.scrSoundVolume->setPos(gGameConfig->soundVolume);
	gSettings.scrSoundVolume->setLargeStep(1);
	gSettings.scrSoundVolume->setSmallStep(1);
	gSettings.chkEnableMusic = env->addCheckBox(gGameConfig->enablemusic, Scale(340, 245, 645, 270), sPanel, CHECKBOX_ENABLE_MUSIC, gDataManager->GetSysString(2046).c_str());
	defaultStrings.emplace_back(gSettings.chkEnableMusic, 2046);
	gSettings.stMusicVolume = env->addStaticText(gDataManager->GetSysString(2048).c_str(), Scale(340, 275, 400, 300), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stMusicVolume, 2048);
	gSettings.scrMusicVolume = env->addScrollBar(true, Scale(405, 275, 645, 295), sPanel, SCROLL_MUSIC_VOLUME);
	gSettings.scrMusicVolume->setMax(100);
	gSettings.scrMusicVolume->setMin(0);
	gSettings.scrMusicVolume->setPos(gGameConfig->musicVolume);
	gSettings.scrMusicVolume->setLargeStep(1);
	gSettings.scrMusicVolume->setSmallStep(1);
	gSettings.chkLoopMusic = env->addCheckBox(gGameConfig->discordIntegration, Scale(340, 305, 645, 330), sPanel, CHECKBOX_LOOP_MUSIC, gDataManager->GetSysString(2079).c_str());
	defaultStrings.emplace_back(gSettings.chkLoopMusic, 2079);
	gSettings.stNoAudioBackend = env->addStaticText(gDataManager->GetSysString(2058).c_str(), Scale(340, 215, 645, 330), false, true, sPanel);
	defaultStrings.emplace_back(gSettings.stNoAudioBackend, 2058);
	gSettings.stNoAudioBackend->setVisible(false);
	// end audio
#ifdef DISCORD_APP_ID
	gSettings.chkDiscordIntegration = env->addCheckBox(gGameConfig->discordIntegration, Scale(340, 335, 645, 360), sPanel, CHECKBOX_DISCORD_INTEGRATION, gDataManager->GetSysString(2078).c_str());
	defaultStrings.emplace_back(gSettings.chkDiscordIntegration, 2078);
#endif
	gSettings.chkHideHandsInReplays = env->addCheckBox(gGameConfig->hideHandsInReplays, Scale(340, 365, 645, 390), sPanel, CHECKBOX_HIDE_HANDS_REPLAY, gDataManager->GetSysString(2080).c_str());
	defaultStrings.emplace_back(gSettings.chkHideHandsInReplays, 2080);
#ifdef UPDATE_URL
	gSettings.chkUpdates = env->addCheckBox(gGameConfig->noClientUpdates, Scale(340, 395, 645, 420), sPanel, -1, gDataManager->GetSysString(1466).c_str());
	defaultStrings.emplace_back(gSettings.chkUpdates, 1466);
#endif

	wBtnSettings = env->addWindow(Scale(0, 610, 30, 640));
	wBtnSettings->getCloseButton()->setVisible(false);
	wBtnSettings->setDraggable(false);
	wBtnSettings->setDrawTitlebar(false);
	wBtnSettings->setDrawBackground(false);
	auto dimBtnSettings = Scale(0, 0, 30, 30);
	btnSettings = irr::gui::CGUIImageButton::addImageButton(env, dimBtnSettings, wBtnSettings, BUTTON_SHOW_SETTINGS);
	btnSettings->setDrawBorder(false);
	btnSettings->setImageSize(dimBtnSettings.getSize());
	btnSettings->setImage(imageManager.tSettings);
	//log
	tabRepositories = wInfos->addTab(gDataManager->GetSysString(2045).c_str());
	defaultStrings.emplace_back(tabRepositories, 2045);
	mTabRepositories = irr::gui::CGUICustomContextMenu::addCustomContextMenu(env, tabRepositories, -1, Scale(1, 275, 301, 639));
	mTabRepositories->grab();
	//
	wHand = env->addWindow(Scale(500, 450, 825, 605), false, L"");
	wHand->getCloseButton()->setVisible(false);
	wHand->setDraggable(false);
	wHand->setDrawTitlebar(false);
	wHand->setVisible(false);
	for(int i = 0; i < 3; ++i) {
		auto dim = Scale(10 + 105 * i, 10, 105 + 105 * i, 144);
		btnHand[i] = irr::gui::CGUIImageButton::addImageButton(env, dim, wHand, BUTTON_HAND1 + i);
		btnHand[i]->setImageSize(dim.getSize());
		btnHand[i]->setImage(imageManager.tHand[i]);
	}
	//
	wFTSelect = env->addWindow(Scale(550, 240, 780, 340), false, L"");
	wFTSelect->getCloseButton()->setVisible(false);
	wFTSelect->setVisible(false);
	btnFirst = env->addButton(Scale(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, gDataManager->GetSysString(100).c_str());
	defaultStrings.emplace_back(btnFirst, 100);
	btnSecond = env->addButton(Scale(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, gDataManager->GetSysString(101).c_str());
	defaultStrings.emplace_back(btnSecond, 101);
	//message (310)
	wMessage = env->addWindow(Scale(510 - 175, 200, 510 + 175, 340), false, gDataManager->GetSysString(1216).c_str());
	defaultStrings.emplace_back(wMessage, 1216);
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wMessage, -1, Scale(20, 20, 350, 100));
	stMessage->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stMessage)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(Scale(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnMsgOK, 1211);
	//auto fade message (310)
	wACMessage = env->addWindow(Scale(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = irr::gui::CGUICustomText::addCustomText(L"", true, env, wACMessage, -1, Scale(0, 0, 350, 60), true);
	stACMessage->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stACMessage)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stACMessage->setBackgroundColor(skin::DUELFIELD_ANNOUNCE_TEXT_BACKGROUND_COLOR_VAL);
	auto tmp_color = skin::DUELFIELD_ANNOUNCE_TEXT_COLOR_VAL;
	if(tmp_color != 0)
		stACMessage->setOverrideColor(tmp_color);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(Scale(490, 200, 840, 340), false, gDataManager->GetSysString(560).c_str());
	defaultStrings.emplace_back(wQuery, 560);
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wQuery, -1, Scale(20, 20, 350, 100));
	stQMessage->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stQMessage)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(Scale(100, 105, 150, 130), wQuery, BUTTON_YES, gDataManager->GetSysString(1213).c_str());
	defaultStrings.emplace_back(btnYes, 1213);
	btnNo = env->addButton(Scale(200, 105, 250, 130), wQuery, BUTTON_NO, gDataManager->GetSysString(1214).c_str());
	defaultStrings.emplace_back(btnNo, 1214);
	//options (310)
	wOptions = env->addWindow(Scale(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions = irr::gui::CGUICustomText::addCustomText(L"", false, env, wOptions, -1, Scale(20, 20, 350, 100));
	stOptions->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stOptions)->setTouchControl(!gGameConfig->native_mouse);
#endif
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(Scale(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnOptionOK, 1211);
	btnOptionp = env->addButton(Scale(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, gDataManager->GetSysString(1432).c_str());
	defaultStrings.emplace_back(btnOptionp, 1432);
	btnOptionn = env->addButton(Scale(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, gDataManager->GetSysString(1433).c_str());
	defaultStrings.emplace_back(btnOptionn, 1433);
	for(int i = 0; i < 5; ++i) {
		btnOption[i] = env->addButton(Scale(10, 30 + 40 * i, 340, 60 + 40 * i), wOptions, BUTTON_OPTION_0 + i, L"");
	}
	scrOption = env->addScrollBar(false, Scale(350, 30, 365, 220), wOptions, SCROLL_OPTION_SELECT);
	scrOption->setLargeStep(1);
	scrOption->setSmallStep(1);
	scrOption->setMin(0);
	//pos select
	wPosSelect = env->addWindow(Scale(340, 200, 935, 410), false, gDataManager->GetSysString(561).c_str());
	defaultStrings.emplace_back(wPosSelect, 561);
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	irr::core::dimension2di imgsize = { Scale<irr::s32>(CARD_IMG_WIDTH * 0.5f), Scale<irr::s32>(CARD_IMG_HEIGHT * 0.5f) };
	btnPSAU = irr::gui::CGUIImageButton::addImageButton(env, Scale(10, 45, 150, 185), wPosSelect, BUTTON_POS_AU);
	btnPSAU->setImageSize(imgsize);
	btnPSAD = irr::gui::CGUIImageButton::addImageButton(env, Scale(155, 45, 295, 185), wPosSelect, BUTTON_POS_AD);
	btnPSAD->setImageSize(imgsize);
	btnPSAD->setImage(imageManager.tCover[0]);
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, Scale(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageSize(imgsize);
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, Scale(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageSize(imgsize);
	btnPSDD->setImageRotation(270);
	btnPSDD->setImage(imageManager.tCover[0]);
	//card select
	imgsize = { Scale<irr::s32>(CARD_IMG_WIDTH * 0.6f), Scale<irr::s32>(CARD_IMG_HEIGHT * 0.6f) };
	wCardSelect = env->addWindow(Scale(320, 100, 1000, 400), false, L"");
	wCardSelect->getCloseButton()->setVisible(false);
	wCardSelect->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stCardPos[i] = env->addStaticText(L"", Scale(30 + 125 * i, 30, 150 + 125 * i, 50), true, false, wCardSelect, -1, true);
		stCardPos[i]->setBackgroundColor(0xffffffff);
		stCardPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardSelect[i] = irr::gui::CGUIImageButton::addImageButton(env, Scale(30 + 125 * i, 55, 150 + 125 * i, 225), wCardSelect, BUTTON_CARD_0 + i);
		btnCardSelect[i]->setImageSize(imgsize);
	}
	scrCardList = env->addScrollBar(true, Scale(30, 235, 650, 255), wCardSelect, SCROLL_CARD_SELECT);
	scrCardList->setLargeStep(scrCardList->getMax());
	btnSelectOK = env->addButton(Scale(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnSelectOK, 1211);
	//card display
	wCardDisplay = env->addWindow(Scale(320, 100, 1000, 400), false, L"");
	wCardDisplay->getCloseButton()->setVisible(false);
	wCardDisplay->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stDisplayPos[i] = env->addStaticText(L"", Scale(30 + 125 * i, 30, 150 + 125 * i, 50), true, false, wCardDisplay, -1, true);
		stDisplayPos[i]->setBackgroundColor(0xffffffff);
		stDisplayPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardDisplay[i] = irr::gui::CGUIImageButton::addImageButton(env, Scale(30 + 125 * i, 55, 150 + 125 * i, 225), wCardDisplay, BUTTON_DISPLAY_0 + i);
		btnCardDisplay[i]->setImageSize(imgsize);
	}
	scrDisplayList = env->addScrollBar(true, Scale(30, 235, 650, 255), wCardDisplay, SCROLL_CARD_DISPLAY);
	btnDisplayOK = env->addButton(Scale(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnDisplayOK, 1211);
	//announce number
	wANNumber = env->addWindow(Scale(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  ADDComboBox(Scale(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(Scale(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnANNumberOK, 1211);
	//announce card
	wANCard = env->addWindow(Scale(430, 170, 840, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", Scale(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(Scale(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(Scale(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnANCardOK, 1211);
	//announce attribute
	wANAttribute = env->addWindow(Scale(500, 200, 830, 285), false, gDataManager->GetSysString(562).c_str());
	defaultStrings.emplace_back(wANAttribute, 562);
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
										   wANAttribute, CHECK_ATTRIBUTE, gDataManager->FormatAttribute(filter).c_str());
	//announce race
	wANRace = env->addWindow(Scale(480, 200, 850, 410), false, gDataManager->GetSysString(563).c_str());
	defaultStrings.emplace_back(wANRace, 563);
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 25; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
									  wANRace, CHECK_RACE, gDataManager->FormatRace(filter).c_str());
	//selection hint
	stHintMsg = env->addStaticText(L"", Scale(500, 60, 820, 90), true, false, 0, -1, false);
	stHintMsg->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
	tmp_color = skin::DUELFIELD_TOOLTIP_TEXT_COLOR_VAL;
	if(tmp_color != 0)
		stHintMsg->setOverrideColor(tmp_color);
	stHintMsg->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHintMsg->setVisible(false);
	//cmd menu
	wCmdMenu = env->addWindow(Scale(10, 10, 110, 179), false, L"");
	wCmdMenu->setDrawTitlebar(false);
	wCmdMenu->setVisible(false);
	wCmdMenu->getCloseButton()->setVisible(false);
	btnActivate = env->addButton(Scale(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, gDataManager->GetSysString(1150).c_str());
	defaultStrings.emplace_back(btnActivate, 1150);
	btnSummon = env->addButton(Scale(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, gDataManager->GetSysString(1151).c_str());
	defaultStrings.emplace_back(btnSummon, 1151);
	btnSPSummon = env->addButton(Scale(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, gDataManager->GetSysString(1152).c_str());
	defaultStrings.emplace_back(btnSPSummon, 1152);
	btnMSet = env->addButton(Scale(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, gDataManager->GetSysString(1153).c_str());
	defaultStrings.emplace_back(btnMSet, 1153);
	btnSSet = env->addButton(Scale(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, gDataManager->GetSysString(1153).c_str());
	defaultStrings.emplace_back(btnSSet, 1153);
	btnRepos = env->addButton(Scale(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, gDataManager->GetSysString(1154).c_str());
	defaultStrings.emplace_back(btnRepos, 1154);
	btnAttack = env->addButton(Scale(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, gDataManager->GetSysString(1157).c_str());
	defaultStrings.emplace_back(btnAttack, 1157);
	btnShowList = env->addButton(Scale(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, gDataManager->GetSysString(1158).c_str());
	defaultStrings.emplace_back(btnShowList, 1158);
	btnOperation = env->addButton(Scale(1, 169, 99, 189), wCmdMenu, BUTTON_CMD_ACTIVATE, gDataManager->GetSysString(1161).c_str());
	defaultStrings.emplace_back(btnOperation, 1161);
	btnReset = env->addButton(Scale(1, 190, 99, 210), wCmdMenu, BUTTON_CMD_RESET, gDataManager->GetSysString(1162).c_str());
	defaultStrings.emplace_back(btnReset, 1162);
	//deck edit
	wDeckEdit = env->addStaticText(L"", Scale(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	stBanlist = env->addStaticText(gDataManager->GetSysString(1300).c_str(), Scale(10, 9, 100, 29), false, false, wDeckEdit);
	defaultStrings.emplace_back(stBanlist, 1300);
	cbDBLFList = ADDComboBox(Scale(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	stDeck = env->addStaticText(gDataManager->GetSysString(1301).c_str(), Scale(10, 39, 100, 59), false, false, wDeckEdit);
	defaultStrings.emplace_back(stDeck, 1301);
	cbDBDecks = ADDComboBox(Scale(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);

	btnSaveDeck = env->addButton(Scale(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, gDataManager->GetSysString(1302).c_str());
	defaultStrings.emplace_back(btnSaveDeck, 1302);
	btnRenameDeck = env->addButton(Scale(5, 65, 75, 90), wDeckEdit, BUTTON_RENAME_DECK, gDataManager->GetSysString(1362).c_str());
	defaultStrings.emplace_back(btnRenameDeck, 1362);
	ebDeckname = env->addEditBox(L"", Scale(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECK_NAME);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(Scale(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, gDataManager->GetSysString(1303).c_str());
	defaultStrings.emplace_back(btnSaveDeckAs, 1303);
	btnShuffleDeck = env->addButton(Scale(5, 95, 75, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, gDataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnShuffleDeck, 1307);
	btnSortDeck = env->addButton(Scale(80, 95, 145, 120), wDeckEdit, BUTTON_SORT_DECK, gDataManager->GetSysString(1305).c_str());
	defaultStrings.emplace_back(btnSortDeck, 1305);
	btnClearDeck = env->addButton(Scale(155, 95, 220, 120), wDeckEdit, BUTTON_CLEAR_DECK, gDataManager->GetSysString(1304).c_str());
	defaultStrings.emplace_back(btnClearDeck, 1304);
	btnDeleteDeck = env->addButton(Scale(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, gDataManager->GetSysString(1308).c_str());
	defaultStrings.emplace_back(btnDeleteDeck, 1308);
	btnSideOK = env->addButton(Scale(510, 40, 820, 80), 0, BUTTON_SIDE_OK, gDataManager->GetSysString(1334).c_str());
	defaultStrings.emplace_back(btnSideOK, 1334);
	btnSideOK->setVisible(false);
	btnSideShuffle = env->addButton(Scale(310, 100, 370, 130), 0, BUTTON_SHUFFLE_DECK, gDataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnSideShuffle, 1307);
	btnSideShuffle->setVisible(false);
	btnSideSort = env->addButton(Scale(375, 100, 435, 130), 0, BUTTON_SORT_DECK, gDataManager->GetSysString(1305).c_str());
	defaultStrings.emplace_back(btnSideSort, 1305);
	btnSideSort->setVisible(false);
	btnSideReload = env->addButton(Scale(440, 100, 500, 130), 0, BUTTON_SIDE_RELOAD, gDataManager->GetSysString(1309).c_str());
	defaultStrings.emplace_back(btnSideReload, 1309);
	btnSideReload->setVisible(false);
	btnHandTest = env->addButton(Scale(205, 90, 295, 130), 0, BUTTON_HAND_TEST, gDataManager->GetSysString(1297).c_str());
	defaultStrings.emplace_back(btnHandTest, 1297);
	btnHandTest->setVisible(false);
	btnHandTest->setEnabled(coreloaded);
	//
	scrFilter = env->addScrollBar(false, Scale(999, 161, 1019, 629), 0, SCROLL_FILTER);
	scrFilter->setLargeStep(DECK_SEARCH_SCROLL_STEP);
	scrFilter->setSmallStep(DECK_SEARCH_SCROLL_STEP);
	scrFilter->setVisible(false);
	//sort type
	wSort = env->addStaticText(L"", Scale(930, 132, 1020, 156), true, false, 0, -1, true);
	cbSortType = ADDComboBox(Scale(10, 2, 85, 22), wSort, COMBOBOX_SORTTYPE);
	cbSortType->setMaxSelectionRows(10);
	ReloadCBSortType();
	wSort->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", Scale(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	stCategory = env->addStaticText(gDataManager->GetSysString(1311).c_str(), Scale(10, 5, 70, 25), false, false, wFilter);
	defaultStrings.emplace_back(stCategory, 1311);
	cbCardType = ADDComboBox(Scale(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	ReloadCBCardType();
	cbCardType2 = ADDComboBox(Scale(130, 3, 190, 23), wFilter, COMBOBOX_SECONDTYPE);
	cbCardType2->setMaxSelectionRows(20);
	cbCardType2->addItem(gDataManager->GetSysString(1310).c_str(), 0);
	chkAnime = env->addCheckBox(gGameConfig->chkAnime, Scale(10, 96, 150, 118), wFilter, CHECKBOX_SHOW_ANIME, gDataManager->GetSysString(1999).c_str());
	defaultStrings.emplace_back(chkAnime, 1999);
	stLimit = env->addStaticText(gDataManager->GetSysString(1315).c_str(), Scale(205, 5, 280, 25), false, false, wFilter);
	defaultStrings.emplace_back(stLimit, 1315);
	cbLimit = ADDComboBox(Scale(260, 3, 390, 23), wFilter, COMBOBOX_OTHER_FILT);
	cbLimit->setMaxSelectionRows(10);
	ReloadCBLimit();
	stAttribute = env->addStaticText(gDataManager->GetSysString(1319).c_str(), Scale(10, 28, 70, 48), false, false, wFilter);
	defaultStrings.emplace_back(stAttribute, 1319);
	cbAttribute = ADDComboBox(Scale(60, 26, 190, 46), wFilter, COMBOBOX_OTHER_FILT);
	cbAttribute->setMaxSelectionRows(10);
	ReloadCBAttribute();
	stRace = env->addStaticText(gDataManager->GetSysString(1321).c_str(), Scale(10, 51, 70, 71), false, false, wFilter);
	cbRace = ADDComboBox(Scale(60, 49, 190, 69), wFilter, COMBOBOX_OTHER_FILT);
	cbRace->setMaxSelectionRows(10);
	ReloadCBRace();
	stAttack = env->addStaticText(gDataManager->GetSysString(1322).c_str(), Scale(205, 28, 280, 48), false, false, wFilter);
	defaultStrings.emplace_back(stAttack, 1322);
	ebAttack = env->addEditBox(L"", Scale(260, 26, 340, 46), true, wFilter, EDITBOX_ATTACK);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stDefense = env->addStaticText(gDataManager->GetSysString(1323).c_str(), Scale(205, 51, 280, 71), false, false, wFilter);
	defaultStrings.emplace_back(stDefense, 1323);
	ebDefense = env->addEditBox(L"", Scale(260, 49, 340, 69), true, wFilter, EDITBOX_DEFENSE);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stStar = env->addStaticText(gDataManager->GetSysString(1324).c_str(), Scale(10, 74, 80, 94), false, false, wFilter);
	defaultStrings.emplace_back(stStar, 1324);
	ebStar = env->addEditBox(L"", Scale(60, 72, 100, 92), true, wFilter, EDITBOX_STAR);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stScale = env->addStaticText(gDataManager->GetSysString(1336).c_str(), Scale(110, 74, 150, 94), false, false, wFilter);
	defaultStrings.emplace_back(stScale, 1336);
	ebScale = env->addEditBox(L"", Scale(150, 72, 190, 92), true, wFilter, EDITBOX_SCALE);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stSearch = env->addStaticText(gDataManager->GetSysString(1325).c_str(), Scale(205, 74, 280, 94), false, false, wFilter);
	defaultStrings.emplace_back(stSearch, 1325);
	ebCardName = env->addEditBox(L"", Scale(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(Scale(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, gDataManager->GetSysString(1326).c_str());
	defaultStrings.emplace_back(btnEffectFilter, 1326);
	btnStartFilter = env->addButton(Scale(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, gDataManager->GetSysString(1327).c_str());
	defaultStrings.emplace_back(btnStartFilter, 1327);
	btnClearFilter = env->addButton(Scale(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, gDataManager->GetSysString(1304).c_str());
	defaultStrings.emplace_back(btnClearFilter, 1304);
	wCategories = env->addWindow(Scale(450, 60, 1000, 270), false, L"");
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(Scale(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCategoryOK, 1211);
	for(int i = 0; i < 32; ++i) {
		chkCategory[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, gDataManager->GetSysString(1100 + i).c_str());
		defaultStrings.emplace_back(chkCategory[i], 1100 + i);
	}
	btnMarksFilter = env->addButton(Scale(155, 96, 240, 118), wFilter, BUTTON_MARKS_FILTER, gDataManager->GetSysString(1374).c_str());
	defaultStrings.emplace_back(btnMarksFilter, 1374);
	wLinkMarks = env->addWindow(Scale(700, 30, 820, 150), false, L"");
	wLinkMarks->getCloseButton()->setVisible(false);
	wLinkMarks->setDrawTitlebar(false);
	wLinkMarks->setDraggable(false);
	wLinkMarks->setVisible(false);
	btnMarksOK = env->addButton(Scale(45, 45, 75, 75), wLinkMarks, BUTTON_MARKERS_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnMarksOK, 1211);
	btnMark[0] = env->addButton(Scale(10, 10, 40, 40), wLinkMarks, -1, L"\u2196");
	btnMark[1] = env->addButton(Scale(45, 10, 75, 40), wLinkMarks, -1, L"\u2191");
	btnMark[2] = env->addButton(Scale(80, 10, 110, 40), wLinkMarks, -1, L"\u2197");
	btnMark[3] = env->addButton(Scale(10, 45, 40, 75), wLinkMarks, -1, L"\u2190");
	btnMark[4] = env->addButton(Scale(80, 45, 110, 75), wLinkMarks, -1, L"\u2192");
	btnMark[5] = env->addButton(Scale(10, 80, 40, 110), wLinkMarks, -1, L"\u2199");
	btnMark[6] = env->addButton(Scale(45, 80, 75, 110), wLinkMarks, -1, L"\u2193");
	btnMark[7] = env->addButton(Scale(80, 80, 110, 110), wLinkMarks, -1, L"\u2198");
	for(int i=0;i<8;i++)
		btnMark[i]->setIsPushButton(true);
	//replay window
	wReplay = env->addWindow(Scale(220, 100, 800, 520), false, gDataManager->GetSysString(1202).c_str());
	defaultStrings.emplace_back(wReplay, 1202);
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wReplay, LISTBOX_REPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstReplayList->setWorkingPath(L"./replay", true);
	lstReplayList->addFilteredExtensions(coreloaded ? std::vector<std::wstring>{L"yrp", L"yrpx"} : std::vector<std::wstring>{ L"yrpx" });
	lstReplayList->setItemHeight(Scale(18));
	btnLoadReplay = env->addButton(Scale(470, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, gDataManager->GetSysString(1348).c_str());
	defaultStrings.emplace_back(btnLoadReplay, 1348);
	btnLoadReplay->setEnabled(false);
	btnDeleteReplay = env->addButton(Scale(360, 355, 460, 380), wReplay, BUTTON_DELETE_REPLAY, gDataManager->GetSysString(1361).c_str());
	defaultStrings.emplace_back(btnDeleteReplay, 1361);
	btnDeleteReplay->setEnabled(false);
	btnRenameReplay = env->addButton(Scale(360, 385, 460, 410), wReplay, BUTTON_RENAME_REPLAY, gDataManager->GetSysString(1362).c_str());
	defaultStrings.emplace_back(btnRenameReplay, 1362);
	btnRenameReplay->setEnabled(false);
	btnReplayCancel = env->addButton(Scale(470, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, gDataManager->GetSysString(1347).c_str());
	defaultStrings.emplace_back(btnReplayCancel, 1347);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(1349).c_str(), Scale(360, 30, 570, 50), false, true, wReplay);
	defaultStrings.emplace_back(tmpptr, 1349);
	stReplayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wReplay, -1, Scale(360, 60, 570, 350));
	stReplayInfo->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stReplayInfo)->setTouchControl(!gGameConfig->native_mouse);
#endif
	btnExportDeck = env->addButton(Scale(470, 325, 570, 350), wReplay, BUTTON_EXPORT_DECK, gDataManager->GetSysString(1358).c_str());
	defaultStrings.emplace_back(btnExportDeck, 1358);
	btnExportDeck->setEnabled(false);
	chkYrp = env->addCheckBox(false, Scale(360, 250, 560, 270), wReplay, -1, gDataManager->GetSysString(1356).c_str());
	defaultStrings.emplace_back(chkYrp, 1356);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(1353).c_str(), Scale(360, 275, 570, 295), false, true, wReplay);
	defaultStrings.emplace_back(tmpptr, 1353);
	ebRepStartTurn = env->addEditBox(L"", Scale(360, 300, 460, 320), true, wReplay, -1);
	ebRepStartTurn->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//single play window
	wSinglePlay = env->addWindow(Scale(220, 100, 800, 520), false, gDataManager->GetSysString(1201).c_str());
	defaultStrings.emplace_back(wSinglePlay, 1201);
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wSinglePlay, LISTBOX_SINGLEPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstSinglePlayList->setItemHeight(Scale(18));
	lstSinglePlayList->setWorkingPath(L"./puzzles", true);
	lstSinglePlayList->addFilteredExtensions({L"lua"});
	btnLoadSinglePlay = env->addButton(Scale(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, gDataManager->GetSysString(1357).c_str());
	defaultStrings.emplace_back(btnLoadSinglePlay, 1357);
	btnLoadSinglePlay->setEnabled(false);
	btnSinglePlayCancel = env->addButton(Scale(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, gDataManager->GetSysString(1210).c_str());
	defaultStrings.emplace_back(btnSinglePlayCancel, 1210);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(1352).c_str(), Scale(360, 30, 570, 50), false, true, wSinglePlay);
	defaultStrings.emplace_back(tmpptr, 1352);
	stSinglePlayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wSinglePlay, -1, Scale(360, 60, 570, 350));
	((irr::gui::CGUICustomText*)stSinglePlayInfo)->enableScrollBar();
	stSinglePlayInfo->setWordWrap(true);
#ifdef __ANDROID__
	((irr::gui::CGUICustomText*)stSinglePlayInfo)->setTouchControl(!gGameConfig->native_mouse);
#endif
	//replay save
	wReplaySave = env->addWindow(Scale(510, 200, 820, 320), false, gDataManager->GetSysString(1340).c_str());
	defaultStrings.emplace_back(wReplaySave, 1340);
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(1342).c_str(), Scale(20, 25, 290, 45), false, false, wReplaySave);
	defaultStrings.emplace_back(tmpptr, 1342);
	ebRSName =  env->addEditBox(L"", Scale(20, 50, 290, 70), true, wReplaySave, EDITBOX_REPLAY_NAME);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(Scale(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, gDataManager->GetSysString(1341).c_str());
	defaultStrings.emplace_back(btnRSYes, 1341);
	btnRSNo = env->addButton(Scale(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, gDataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnRSNo, 1212);
	//replay control
	wReplayControl = env->addStaticText(L"", Scale(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, gDataManager->GetSysString(1343).c_str());
	defaultStrings.emplace_back(btnReplayStart, 1343);
	btnReplayPause = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, gDataManager->GetSysString(1344).c_str());
	defaultStrings.emplace_back(btnReplayPause, 1344);
	btnReplayStep = env->addButton(Scale(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_STEP, gDataManager->GetSysString(1345).c_str());
	defaultStrings.emplace_back(btnReplayStep, 1345);
	btnReplayUndo = env->addButton(Scale(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_UNDO, gDataManager->GetSysString(1360).c_str());
	defaultStrings.emplace_back(btnReplayUndo, 1360);
	btnReplaySwap = env->addButton(Scale(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_SWAP, gDataManager->GetSysString(1346).c_str());
	defaultStrings.emplace_back(btnReplaySwap, 1346);
	btnReplayExit = env->addButton(Scale(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, gDataManager->GetSysString(1347).c_str());
	defaultStrings.emplace_back(btnReplayExit, 1347);
	//chat
	wChat = env->addWindow(Scale(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", Scale(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	ebChatInput->setAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT);
	//swap
	btnSpectatorSwap = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_REPLAY_SWAP, gDataManager->GetSysString(1346).c_str());
	defaultStrings.emplace_back(btnSpectatorSwap, 1346);
	btnSpectatorSwap->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, gDataManager->GetSysString(1292).c_str());
	defaultStrings.emplace_back(btnChainIgnore, 1292);
	btnChainAlways = env->addButton(Scale(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, gDataManager->GetSysString(1293).c_str());
	defaultStrings.emplace_back(btnChainAlways, 1293);
	btnChainWhenAvail = env->addButton(Scale(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, gDataManager->GetSysString(1294).c_str());
	defaultStrings.emplace_back(btnChainWhenAvail, 1294);
	btnChainIgnore->setIsPushButton(true);
	btnChainAlways->setIsPushButton(true);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	//shuffle
	btnShuffle = env->addButton(Scale(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, gDataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnShuffle, 1307);
	btnShuffle->setVisible(false);
	//cancel or finish
	btnCancelOrFinish = env->addButton(Scale(205, 230, 295, 265), 0, BUTTON_CANCEL_OR_FINISH, gDataManager->GetSysString(1295).c_str());
	defaultStrings.emplace_back(btnCancelOrFinish, 1295);
	btnCancelOrFinish->setVisible(false);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(Scale(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	//restart single
	btnRestartSingle = env->addButton(Scale(205, 90, 295, 165), 0, BUTTON_RESTART_SINGLE, gDataManager->GetSysString(1366).c_str());
	defaultStrings.emplace_back(btnRestartSingle, 1366);
	btnRestartSingle->setVisible(false);
	//tip
	stTip = env->addStaticText(L"", Scale(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
	tmp_color = skin::DUELFIELD_TOOLTIP_TEXT_COLOR_VAL;
	if(tmp_color != 0)
		stTip->setOverrideColor(tmp_color);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
	//tip for cards in select / display list
	stCardListTip = env->addStaticText(L"", Scale(0, 0, 150, 150), false, true, wCardSelect, TEXT_CARD_LIST_TIP, true);
	stCardListTip->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
	if(tmp_color != 0)
		stCardListTip->setOverrideColor(tmp_color);
	stCardListTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stCardListTip->setVisible(false);
	device->setEventReceiver(&menuHandler);
	if(!gSoundManager->IsUsable()) {
		tabSettings.DisableAudio();
		gSettings.DisableAudio();
	}

	//server lobby
	wRoomListPlaceholder = env->addStaticText(L"", Scale(1, 1, 1024 - 1, 640), false, false, 0, -1, false);
	//wRoomListPlaceholder->setAlignment(EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT);
	wRoomListPlaceholder->setVisible(false);

	auto roomlistcolor = skin::ROOMLIST_TEXTS_COLOR_VAL;

	//server choice dropdownlist
	irr::gui::IGUIStaticText* statictext = env->addStaticText(gDataManager->GetSysString(2041).c_str(), Scale(10, 30, 110, 50), false, false, wRoomListPlaceholder, -1, false); // 2041 = Server:
	defaultStrings.emplace_back(statictext, 2041);
	statictext->setOverrideColor(roomlistcolor);
	serverChoice = ADDComboBox(Scale(90, 25, 385, 50), wRoomListPlaceholder, SERVER_CHOICE);

	//online nickname
	statictext = env->addStaticText(gDataManager->GetSysString(1220).c_str(), Scale(10, 60, 110, 80), false, false, wRoomListPlaceholder, -1, false); // 1220 = Nickname:
	defaultStrings.emplace_back(statictext, 1220);
	statictext->setOverrideColor(roomlistcolor);
	ebNickNameOnline = env->addEditBox(gGameConfig->nickname.c_str(), Scale(90, 55, 275, 80), true, wRoomListPlaceholder, EDITBOX_NICKNAME);

	//top right host online game button
	btnCreateHost2 = env->addButton(Scale(904, 25, 1014, 50), wRoomListPlaceholder, BUTTON_CREATE_HOST2, gDataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(btnCreateHost2, 1224);
	btnCreateHost2->setAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	//filter dropdowns
	cbFilterRule = ADDComboBox(Scale(392, 25, 532, 50), wRoomListPlaceholder, CB_FILTER_ALLOWED_CARDS);
	//cbFilterMatchMode = ADDComboBox(Scale(392, 55, 532, 80), wRoomListPlaceholder, CB_FILTER_MATCH_MODE);
	cbFilterBanlist = ADDComboBox(Scale(392, 85, 532, 110), wRoomListPlaceholder, CB_FILTER_BANLIST);
	cbFilterRule->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	//cbFilterMatchMode->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	cbFilterBanlist->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	ReloadCBFilterRule();
	RefreshLFLists();

	/*cbFilterMatchMode->addItem(fmt::format(L"[{}]", gDataManager->GetSysString(1227)).c_str());
	cbFilterMatchMode->addItem(gDataManager->GetSysString(1244).c_str());
	cbFilterMatchMode->addItem(gDataManager->GetSysString(1245).c_str());
	cbFilterMatchMode->addItem(gDataManager->GetSysString(1246).c_str());*/
	//Scale(392, 55, 532, 80)
	ebOnlineTeam1 = env->addEditBox(L"0", Scale(140 + (392 - 140), 55, 170 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam1->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	stVersus = env->addStaticText(gDataManager->GetSysString(1380).c_str(), Scale(175 + (392 - 140), 55, 195 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	defaultStrings.emplace_back(stVersus, 1380);
	stVersus->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stVersus->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	stVersus->setOverrideColor(roomlistcolor);
	ebOnlineTeam2 = env->addEditBox(L"0", Scale(200 + (392 - 140), 55, 230 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam2->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	stBestof = env->addStaticText(gDataManager->GetSysString(1381).c_str(), Scale(235 + (392 - 140), 55, 280 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	defaultStrings.emplace_back(stBestof, 1381);
	stBestof->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	stBestof->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	stBestof->setOverrideColor(roomlistcolor);
	ebOnlineBestOf = env->addEditBox(L"0", Scale(285 + (392 - 140), 55, 315 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_NUMERIC);
	ebOnlineBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineBestOf->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	btnFilterRelayMode = env->addButton(Scale(325 + (392 - 140), 55, 370 + (392 - 140), 80), wRoomListPlaceholder, BUTTON_FILTER_RELAY, gDataManager->GetSysString(1247).c_str());
	defaultStrings.emplace_back(btnFilterRelayMode, 1247);
	btnFilterRelayMode->setIsPushButton(true);
	btnFilterRelayMode->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	//filter rooms textbox
	ebRoomNameText = env->addStaticText(gDataManager->GetSysString(2021).c_str(), Scale(572, 30, 682, 50), false, false, wRoomListPlaceholder); //2021 = Filter:
	defaultStrings.emplace_back(ebRoomNameText, 2021);
	ebRoomNameText->setOverrideColor(roomlistcolor);
	ebRoomName = env->addEditBox(L"", Scale(642, 25, 782, 50), true, wRoomListPlaceholder, EDIT_ONLINE_ROOM_NAME); //filter textbox
	ebRoomName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebRoomNameText->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);
	ebRoomName->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	//show locked rooms checkbox
	chkShowPassword = irr::gui::CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 55, 1024, 80), wRoomListPlaceholder, CHECK_SHOW_LOCKED_ROOMS, gDataManager->GetSysString(1994).c_str());
	defaultStrings.emplace_back(chkShowPassword, 1994);
	((irr::gui::CGUICustomCheckBox*)chkShowPassword)->setColor(roomlistcolor);
	chkShowPassword->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	//show active rooms checkbox
	chkShowActiveRooms = irr::gui::CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 85, 1024, 110), wRoomListPlaceholder, CHECK_SHOW_ACTIVE_ROOMS, gDataManager->GetSysString(1985).c_str());
	defaultStrings.emplace_back(chkShowActiveRooms, 1985);
	((irr::gui::CGUICustomCheckBox*)chkShowActiveRooms)->setColor(roomlistcolor);
	chkShowActiveRooms->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_UPPERLEFT);

	//show all rooms in a table
	roomListTable = irr::gui::CGUICustomTable::addCustomTable(env, Resize(1, 118, 1022, 550), wRoomListPlaceholder, TABLE_ROOMLIST, true);
	roomListTable->setResizableColumns(true);
	//roomListTable->setAlignment(EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT);
	roomListTable->addColumn(L" ");//lock
	roomListTable->addColumn(gDataManager->GetSysString(1225).c_str());//Allowed Cards:
	roomListTable->addColumn(gDataManager->GetSysString(1227).c_str());//Duel Mode:
	roomListTable->addColumn(gDataManager->GetSysString(1236).c_str());//master rule
	roomListTable->addColumn(gDataManager->GetSysString(1226).c_str());//Forbidden List:
	roomListTable->addColumn(gDataManager->GetSysString(2030).c_str());//Players:
	roomListTable->addColumn(gDataManager->GetSysString(2024).c_str());//Notes:
	roomListTable->addColumn(gDataManager->GetSysString(1988).c_str());//Status
	roomListTable->setColumnWidth(0, 30); //lock
	roomListTable->setColumnWidth(1, 110);//Allowed Cards:
	roomListTable->setColumnWidth(2, 150);//Duel Mode:
	roomListTable->setColumnWidth(3, 50);//master rule
	roomListTable->setColumnWidth(4, 130);//Forbidden List:
	roomListTable->setColumnWidth(5, 115);//Players:
	roomListTable->setColumnWidth(6, 355);//Notes:
	roomListTable->setColumnWidth(7, 60);//Status
	roomListTable->setColumnOrdering(0, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(1, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(2, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(3, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(4, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(5, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(6, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(7, irr::gui::EGCO_FLIP_ASCENDING_DESCENDING);

	//refresh button center bottom
	btnLanRefresh2 = env->addButton(Scale(462, 640 - 10 - 25 - 25 - 5, 562, 640 - 10 - 25 - 5), wRoomListPlaceholder, BUTTON_LAN_REFRESH2, gDataManager->GetSysString(1217).c_str());
	defaultStrings.emplace_back(btnLanRefresh2, 1217);
	btnLanRefresh2->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT);

	//server room password
	wRoomPassword = env->addWindow(Scale(357, 200, 667, 320), false, L"");
	wRoomPassword->getCloseButton()->setVisible(false);
	wRoomPassword->setVisible(false);
	wRoomPassword->setAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
 	tmpptr = env->addStaticText(gDataManager->GetSysString(2038).c_str(), Scale(20, 25, 290, 45), false, false, wRoomPassword);
	defaultStrings.emplace_back(tmpptr, 2038);
	ebRPName = env->addEditBox(L"", Scale(20, 50, 290, 70), true, wRoomPassword, -1);
	ebRPName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRPYes = env->addButton(Scale(70, 80, 140, 105), wRoomPassword, BUTTON_ROOMPASSWORD_OK, gDataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnRPYes, 1211);
	btnRPNo = env->addButton(Scale(170, 80, 240, 105), wRoomPassword, BUTTON_ROOMPASSWORD_CANCEL, gDataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnRPNo, 1212);

	//join cancel buttons
	btnJoinHost2 = env->addButton(Scale(1024 - 10 - 110, 640 - 20 - 25 - 25 - 5, 1024 - 10, 640 - 20 - 25 - 5), wRoomListPlaceholder, BUTTON_JOIN_HOST2, gDataManager->GetSysString(1223).c_str());
	defaultStrings.emplace_back(btnJoinHost2, 1223);
	btnJoinCancel2 = env->addButton(Scale(1024 - 10 - 110, 640 - 20 - 25, 1024 - 10, 640 - 20), wRoomListPlaceholder, BUTTON_JOIN_CANCEL2, gDataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnJoinCancel2, 1212);
	btnJoinHost2->setAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT);
	btnJoinCancel2->setAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT);


	//load server(s)
	LoadServers();
	env->getRootGUIElement()->bringToFront(wBtnSettings);
	env->getRootGUIElement()->bringToFront(mTopMenu);
	env->setFocus(wMainMenu);
#ifdef YGOPRO_BUILD_DLL
	if(!coreloaded) {
		stMessage->setText(gDataManager->GetSysString(1430).c_str());
		PopupElement(wMessage);
	}
#endif
	fpsCounter = env->addStaticText(L"", Scale(950, 620, 1024, 640), false, false);
	fpsCounter->setOverrideColor(skin::FPS_TEXT_COLOR_VAL);
	fpsCounter->setVisible(gGameConfig->showFPS);
	fpsCounter->setTextRestrainedInside(false);
#ifndef __ANDROID__
	fpsCounter->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_LOWERRIGHT);
#else
	fpsCounter->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_LOWERRIGHT);
#endif
	//update window
	updateWindow = env->addWindow(Scale(490, 200, 840, 340), true, L"");
	updateWindow->getCloseButton()->setVisible(false);
	updateWindow->setVisible(false);
	updateProgressText = env->addStaticText(L"", Scale(5, 5, 345, 90), false, true, updateWindow);
	updateProgressTop = new IProgressBar(env, Scale(5, 60, 335, 85), -1, updateWindow);
	updateProgressTop->addBorder(1);
	updateProgressTop->setProgress(0);
	updateProgressTop->setVisible(false);
	updateProgressTop->drop();
	updateSubprogressText = env->addStaticText(L"", Scale(5, 90, 345, 110), false, true, updateWindow);
	updateProgressBottom = new IProgressBar(env, Scale(5, 115, 335, 130), -1, updateWindow);
	updateProgressBottom->addBorder(1);
	updateProgressBottom->setProgress(0);
	updateProgressBottom->drop();

	hideChat = false;
	hideChatTimer = 0;
	delta_time = 0;

	Utils::CreateResourceFolders();

	LoadGithubRepositories();
	ApplySkin(EPRO_TEXT(""), true);
	if(selectedLocale)
		ApplyLocale(selectedLocale, true);
	return true;
}
void BuildProjectionMatrix(irr::core::matrix4& mProjection, irr::f32 left, irr::f32 right, irr::f32 bottom, irr::f32 top, irr::f32 znear, irr::f32 zfar) {
	mProjection.buildProjectionMatrixPerspectiveLH(right - left, top - bottom, znear, zfar);
	mProjection[8] = (left + right) / (left - right);
	mProjection[9] = (top + bottom) / (bottom - top);
}
bool Game::MainLoop() {
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, CAMERA_LEFT, CAMERA_RIGHT, CAMERA_BOTTOM, CAMERA_TOP, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);

	mProjection.buildCameraLookAtMatrixLH(irr::core::vector3df(FIELD_X, FIELD_Y, FIELD_Z), irr::core::vector3df(FIELD_X, 0, 0), irr::core::vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(irr::video::SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
#if defined (__linux__) && !defined(__ANDROID__)
	bool last_resize = false;
	irr::core::dimension2d<irr::u32> prev_window_size;
#endif
	irr::ITimer* timer = device->getTimer();
	uint32 cur_time = 0;
	uint32 prev_time = timer->getRealTime();
	float frame_counter = 0.0f;
	int fps = 0;
	bool was_connected = false;
	bool update_prompted = false;
	bool unzip_started = false;
	if(!driver->queryFeature(irr::video::EVDF_TEXTURE_NPOT)) {
		auto SetClamp = [](irr::video::SMaterialLayer layer[irr::video::MATERIAL_MAX_TEXTURES]) {
			layer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
			layer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		};
		SetClamp(matManager.mCard.TextureLayer);
		SetClamp(matManager.mTexture.TextureLayer);
		SetClamp(matManager.mBackLine.TextureLayer);
		SetClamp(matManager.mSelField.TextureLayer);
		SetClamp(matManager.mLinkedField.TextureLayer);
		SetClamp(matManager.mMutualLinkedField.TextureLayer);
		SetClamp(matManager.mOutLine.TextureLayer);
		SetClamp(matManager.mTRTexture.TextureLayer);
		SetClamp(matManager.mATK.TextureLayer);
		SetClamp(matManager.mCard.TextureLayer);
	}
	if (gGameConfig->fullscreen) {
		// Synchronize actual fullscreen state with config struct
		bool currentlyFullscreen = false;
		GUIUtils::ToggleFullscreen(device, currentlyFullscreen);
	}
	while(!restart && device->run()) {
		if(should_reload_skin) {
			should_reload_skin = false;
			if(Utils::ToPathString(gSettings.cbCurrentSkin->getItem(gSettings.cbCurrentSkin->getSelected())) != gGameConfig->skin) {
				gGameConfig->skin = Utils::ToPathString(gSettings.cbCurrentSkin->getItem(gSettings.cbCurrentSkin->getSelected()));
				ApplySkin(gGameConfig->skin);
			} else {
				ApplySkin(EPRO_TEXT(""), true);
			}
		}
		auto repos = gRepoManager->GetReadyRepos();
		if(!repos.empty()) {
			bool refresh_db = false;
			for(auto& repo : repos) {
				auto grepo = &repoInfoGui[repo->repo_path];
				UpdateRepoInfo(repo, grepo);
				auto data_path = Utils::ToPathString(repo->data_path);
				auto files = Utils::FindFiles(data_path, { EPRO_TEXT("cdb") }, 0);
				if(!repo->is_language) {
					for(auto& file : files)
						refresh_db = gDataManager->LoadDB(data_path + file) || refresh_db;
					gDataManager->LoadStrings(data_path + EPRO_TEXT("strings.conf"));
				} else {
					if(Utils::ToUTF8IfNeeded(gGameConfig->locale) == repo->language) {
						for(auto& file : files)
							refresh_db = gDataManager->LoadLocaleDB(data_path + file) || refresh_db;
						gDataManager->LoadLocaleStrings(data_path + EPRO_TEXT("strings.conf"));
					}
					auto langpath = Utils::ToPathString(repo->language);
					auto lang = Utils::ToUpperNoAccents(langpath);
					auto it = std::find_if(locales.begin(), locales.end(),
										   [&lang]
					(const std::pair<path_string, std::vector<path_string>>& locale)->bool
					{
						return Utils::ToUpperNoAccents(locale.first) == lang;
					});
					if(it != locales.end()) {
						it->second.push_back(data_path);
						ReloadElementsStrings();
					} else {
						Utils::MakeDirectory(EPRO_TEXT("./config/languages/") + langpath);
						locales.emplace_back(langpath, std::vector<path_string>{ data_path });
						gSettings.cbCurrentLocale->addItem(BufferIO::DecodeUTF8s(repo->language).c_str());
					}
				}
			}
			if(refresh_db && is_building && deckBuilder.results.size())
				deckBuilder.StartFilter(true);
		}
#ifdef YGOPRO_BUILD_DLL
		bool coreJustLoaded = false;
		if(!dInfo.isStarted && cores_to_load.size() && gRepoManager->GetUpdatingReposNumber() == 0) {
			for(auto& path : cores_to_load) {
				void* ncore = nullptr;
				if((ncore = ChangeOCGcore(gGameConfig->working_directory + path, ocgcore))) {
					corename = Utils::ToUnicodeIfNeeded(path);
					coreJustLoaded = true;
					ocgcore = ncore;
					if(!coreloaded) {
						coreloaded = true;
						btnSingleMode->setEnabled(true);
						btnCreateHost->setEnabled(true);
						btnHandTest->setEnabled(true);
						lstReplayList->addFilteredExtensions({ L"yrp", L"yrpx" });
					}
					break;
				}
			}
			cores_to_load.clear();
		}
		if (coreJustLoaded) {
			if (std::wstring(stMessage->getText()) == gDataManager->GetSysString(1430))
				HideElement(wMessage);
			RefreshUICoreVersion();
			env->setFocus(stACMessage);
			stACMessage->setText(fmt::format(gDataManager->GetSysString(1431).c_str(), corename).c_str());
			PopupElement(wACMessage, 30);
			coreJustLoaded = false;
		}
#endif //YGOPRO_BUILD_DLL
		for(auto& repo : gRepoManager->GetRepoStatus()) {
			repoInfoGui[repo.first].progress1->setProgress(repo.second);
			repoInfoGui[repo.first].progress2->setProgress(repo.second);
		}
		gSoundManager->Tick();
		fps++;
		auto now = timer->getRealTime();
		delta_time = now - prev_time;
		prev_time = now;
		cur_time += delta_time;
		bool resized = false;
		auto size = driver->getScreenSize();
#if defined (__linux__) && !defined(__ANDROID__)
		prev_window_size = window_size;
		window_size = size;
		if(prev_window_size != window_size && !last_resize) {
			last_resize = true;
		} else if(prev_window_size == window_size && last_resize) {
			last_resize = false;
#else
		if(window_size != size) {
#endif
			resized = true;
			window_size = size;
			window_scale.X = (window_size.Width / 1024.0) / gGameConfig->dpi_scale;
			window_scale.Y = (window_size.Height / 640.0) / gGameConfig->dpi_scale;
			cardimagetextureloading = false;
			OnResize();
		}
		frame_counter += (float)delta_time * 60.0f/1000.0f;
		float remainder;
		frame_counter = std::modf(frame_counter, &remainder);
		delta_frames = std::round(remainder);
		for(int i = 0; i < delta_frames; i++){
			linePatternD3D = (linePatternD3D + 1) % 30;
			linePatternGL = (linePatternGL << 1) | (linePatternGL >> 15);
		}
		atkframe += 0.1f * (float)delta_time * 60.0f / 1000.0f;
		atkdy = (float)sin(atkframe);
		driver->beginScene(true, true, irr::video::SColor(0, 0, 0, 0));
		gMutex.lock();
		if(dInfo.isInDuel) {
			if(dInfo.isReplay)
				discord.UpdatePresence(DiscordWrapper::REPLAY);
			else if(dInfo.isHandTest)
				discord.UpdatePresence(DiscordWrapper::HAND_TEST);
			else if(dInfo.isSingleMode)
				discord.UpdatePresence(DiscordWrapper::PUZZLE);
			else {
				if(dInfo.isStarted)
					discord.UpdatePresence(DiscordWrapper::DUEL_STARTED);
				else
					discord.UpdatePresence(DiscordWrapper::DUEL);
			}
			if (showcardcode == 1 || showcardcode == 3)
				gSoundManager->PlayBGM(SoundManager::BGM::WIN, gGameConfig->loopMusic);
			else if (showcardcode == 2)
				gSoundManager->PlayBGM(SoundManager::BGM::LOSE, gGameConfig->loopMusic);
			else if (dInfo.lp[0] > 0 && dInfo.lp[0] <= dInfo.lp[1] / 2)
				gSoundManager->PlayBGM(SoundManager::BGM::DISADVANTAGE, gGameConfig->loopMusic);
			else if (dInfo.lp[0] > 0 && dInfo.lp[0] >= dInfo.lp[1] * 2)
				gSoundManager->PlayBGM(SoundManager::BGM::ADVANTAGE, gGameConfig->loopMusic);
			else
				gSoundManager->PlayBGM(SoundManager::BGM::DUEL, gGameConfig->loopMusic);
			MATERIAL_GUARD(
			DrawBackImage(imageManager.tBackGround, resized);
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();)
		} else if(is_building) {

			if(is_siding)
				discord.UpdatePresence(DiscordWrapper::DECK_SIDING);
			else
				discord.UpdatePresence(DiscordWrapper::DECK);
			gSoundManager->PlayBGM(SoundManager::BGM::DECK, gGameConfig->loopMusic);
			DrawBackImage(imageManager.tBackGround_deck, resized);
			MATERIAL_GUARD(DrawDeckBd());
		} else {
			if(dInfo.isInLobby)
				discord.UpdatePresence(DiscordWrapper::IN_LOBBY);
			else
				discord.UpdatePresence(DiscordWrapper::MENU);
			gSoundManager->PlayBGM(SoundManager::BGM::MENU, gGameConfig->loopMusic);
			DrawBackImage(imageManager.tBackGround_menu, resized);
		}
#ifndef __ANDROID__
		// text width is actual size, other pixels are relative to the assumed 1024x640
		// so we recompensate for the scale factor and window resizing
		int fpsCounterWidth = fpsCounter->getTextWidth() / (dpi_scale * window_scale.X);
#else
		int fpsCounterWidth = fpsCounter->getTextWidth() / (dpi_scale * window_scale.X) + 20; // corner may be curved
#endif
		if (is_building || is_siding) {
			fpsCounter->setRelativePosition(Resize(205, CARD_IMG_HEIGHT + 1, 300, CARD_IMG_HEIGHT + 21));
		} else if (wChat->isVisible()) { // Move it above the chat box
			fpsCounter->setRelativePosition(Resize(1020 - fpsCounterWidth, 595, 1020, 615));
		} else { // bottom right of window with a little padding
			fpsCounter->setRelativePosition(Resize(1024 - fpsCounterWidth, 620, 1024, 640));
		}
		wBtnSettings->setVisible(!(is_building || is_siding || dInfo.isInDuel || open_file));
		MATERIAL_GUARD(DrawGUI();	DrawSpec(););
		if(cardimagetextureloading) {
			ShowCardInfo(showingcard);
		}
		gMutex.unlock();
		if(signalFrame > 0) {
			uint32 movetime = std::min((int)delta_time, signalFrame);
			signalFrame -= movetime;
			if(!signalFrame)
				frameSignal.Set();
		}
		if(waitFrame >= 0.0f) {
			waitFrame += (float)delta_time * 60.0f / 1000.0f;;
			if((int)std::round(waitFrame) % 90 == 0) {
				stHintMsg->setText(gDataManager->GetSysString(1390).c_str());
			} else if((int)std::round(waitFrame) % 90 == 30) {
				stHintMsg->setText(gDataManager->GetSysString(1391).c_str());
			} else if((int)std::round(waitFrame) % 90 == 60) {
				stHintMsg->setText(gDataManager->GetSysString(1392).c_str());
			}
		}
		driver->endScene();
		if(!closeSignal.try_lock())
			CloseDuelWindow();
		else
			closeSignal.unlock();
		if (DuelClient::try_needed) {
			DuelClient::try_needed = false;
			DuelClient::StartClient(DuelClient::temp_ip, DuelClient::temp_port, dInfo.secret.game_id, false);
		}
		popupCheck.lock();
		if(queued_msg.size()){
			env->addMessageBox(queued_caption.c_str(),queued_msg.c_str());
			queued_msg.clear();
			queued_caption.clear();
		}
		popupCheck.unlock();
		discord.Check();
		if(discord.connected && !was_connected) {
			was_connected = true;
			env->setFocus(stACMessage);
			stACMessage->setText(gDataManager->GetSysString(1437).c_str());
			PopupElement(wACMessage, 30);
		} else if(!discord.connected && was_connected) {
			was_connected = false;
			env->setFocus(stACMessage);
			stACMessage->setText(gDataManager->GetSysString(1438).c_str());
			PopupElement(wACMessage, 30);
		}
		if(!update_prompted && !(dInfo.isInDuel || dInfo.isInLobby || is_siding
			|| wRoomListPlaceholder->isVisible() || wLanWindow->isVisible()
			|| wCreateHost->isVisible() || wHostPrepare->isVisible()) && gClientUpdater->HasUpdate()) {
			gMutex.lock();
			menuHandler.prev_operation = ACTION_UPDATE_PROMPT;
			stQMessage->setText(fmt::format(L"{}\n{}", gDataManager->GetSysString(1460), gDataManager->GetSysString(1461)).c_str());
			SetCentered(wQuery);
			PopupElement(wQuery);
			gMutex.unlock();
			update_prompted = true;
		} else if (show_changelog) {
			gMutex.lock();
			menuHandler.prev_operation = ACTION_SHOW_CHANGELOG;
			stQMessage->setText(gDataManager->GetSysString(1443).c_str());
			SetCentered(wQuery);
			PopupElement(wQuery);
			gMutex.unlock();
			show_changelog = false;
		}
		if(!unzip_started && gClientUpdater->UpdateDownloaded()) {
			unzip_started = true;
			gClientUpdater->StartUnzipper(Game::UpdateUnzipBar, mainGame);
		}
#ifndef __ANDROID__
#ifdef __APPLE__
		// Recent versions of macOS break OpenGL vsync while offscreen, resulting in
		// astronomical FPS and CPU usage. As a workaround, while the game window is
		// fully occluded, the game is restricted to 30 FPS.
		int fpsLimit = device->isWindowActive() ? gGameConfig->maxFPS : 30;
		if (!device->isWindowActive() || (gGameConfig->maxFPS > 0 && !gGameConfig->vsync)) {
#else
		int fpsLimit = gGameConfig->maxFPS;
		if(gGameConfig->maxFPS > 0 && !gGameConfig->vsync) {
#endif
			int delta = std::round(fps * (1000.0f / fpsLimit) - cur_time);
			if(delta > 0) {
				auto t = timer->getRealTime();
				while((timer->getRealTime() - t) < delta) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
#endif
		while(cur_time >= 1000) {
			fpsCounter->setText(fmt::format(gDataManager->GetSysString(1444), fps).c_str());
			fps = 0;
			cur_time -= 1000;
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
		if(gGameConfig->maxFPS != -1 || gGameConfig->vsync)
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	discord.UpdatePresence(DiscordWrapper::TERMINATE);
	replaySignal.SetNoWait(true);
	frameSignal.SetNoWait(true);
	analyzeMutex.lock();
	DuelClient::StopClient(true);
	analyzeMutex.unlock();
	ClearTextures();
	if(dInfo.isSingleMode)
		SingleMode::StopPlay(true);
	if(dInfo.isReplay)
		ReplayMode::StopReplay(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	SaveConfig();
#ifdef YGOPRO_BUILD_DLL
	if(ocgcore)
		UnloadCore(ocgcore);
#endif //YGOPRO_BUILD_DLL
	//device->drop();
	return restart;
}
path_string Game::NoSkinLabel() {
	return Utils::ToPathString(gDataManager->GetSysString(2065));
}
bool Game::ApplySkin(const path_string& skinname, bool reload, bool firstrun) {
	static path_string prev_skin = EPRO_TEXT("");
	bool applied = true;
	auto reapply_colors = [&] () {
		wCardImg->setBackgroundColor(skin::CARDINFO_IMAGE_BACKGROUND_VAL);
		stInfo->setOverrideColor(skin::CARDINFO_TYPES_COLOR_VAL);
		stDataInfo->setOverrideColor(skin::CARDINFO_STATS_COLOR_VAL);
		stSetName->setOverrideColor(skin::CARDINFO_ARCHETYPE_TEXT_COLOR_VAL);
		stPasscodeScope->setOverrideColor(skin::CARDINFO_PASSCODE_SCOPE_TEXT_COLOR_VAL);
		stACMessage->setBackgroundColor(skin::DUELFIELD_ANNOUNCE_TEXT_BACKGROUND_COLOR_VAL);
		auto tmp_color = skin::DUELFIELD_ANNOUNCE_TEXT_COLOR_VAL;
		if(tmp_color != 0) {
			stACMessage->setOverrideColor(tmp_color);
		} else {
			stACMessage->enableOverrideColor(false);
		}
		stHintMsg->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
		tmp_color = skin::DUELFIELD_TOOLTIP_TEXT_COLOR_VAL;
		if(tmp_color != 0) {
			stHintMsg->setOverrideColor(tmp_color);
		} else {
			stHintMsg->enableOverrideColor(false);
		}
		stTip->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
		tmp_color = skin::DUELFIELD_TOOLTIP_TEXT_COLOR_VAL;
		if(tmp_color != 0)
			stTip->setOverrideColor(tmp_color);
		stCardListTip->setBackgroundColor(skin::DUELFIELD_TOOLTIP_TEXT_BACKGROUND_COLOR_VAL);
		if(tmp_color != 0)
			stCardListTip->setOverrideColor(tmp_color);
		stCardListTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		auto roomlistcolor = skin::ROOMLIST_TEXTS_COLOR_VAL;
		stVersus->setOverrideColor(roomlistcolor);
		stBestof->setOverrideColor(roomlistcolor);
		ebRoomNameText->setOverrideColor(roomlistcolor);
		((irr::gui::CGUICustomCheckBox*)chkShowPassword)->setColor(roomlistcolor);
		((irr::gui::CGUICustomCheckBox*)chkShowActiveRooms)->setColor(roomlistcolor);
		fpsCounter->setOverrideColor(skin::FPS_TEXT_COLOR_VAL);
		for(auto& repo : repoInfoGui) {
			repo.second.progress1->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
			repo.second.progress2->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
		}
		updateProgressTop->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
		updateProgressBottom->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
		btnPSAD->setImage(imageManager.tCover[0]);
		btnPSDD->setImage(imageManager.tCover[0]);
		btnSettings->setImage(imageManager.tSettings);
		btnHand[0]->setImage(imageManager.tHand[0]);
		btnHand[1]->setImage(imageManager.tHand[1]);
		btnHand[2]->setImage(imageManager.tHand[2]);
	};
	if(!skinSystem || ((skinname == prev_skin || (reload && prev_skin == EPRO_TEXT(""))) && !firstrun))
		return false;
	if(!reload)
		prev_skin = skinname;
	if(prev_skin == NoSkinLabel()) {
		auto skin = env->createSkin(irr::gui::EGST_WINDOWS_METALLIC);
		env->setSkin(skin);
		skin->drop();
		skin::ResetDefaults();
		imageManager.ResetTextures();
	} else {
		if(skinSystem->applySkin(prev_skin.c_str())) {
#define CLR(val1,val2,val3,val4) irr::video::SColor(val1,val2,val3,val4)
#define DECLR(what,val) skin::what##_VAL = skinSystem->getCustomColor(skin::what,val);
#include "custom_skin_enum.inl"
#undef DECLR
#undef CLR
		imageManager.ChangeTextures(EPRO_TEXT("./skin/") + prev_skin + EPRO_TEXT("/textures/"));
		} else {
			applied = false;
			auto skin = env->createSkin(irr::gui::EGST_WINDOWS_METALLIC);
			env->setSkin(skin);
			skin->drop();
			skin::ResetDefaults();
			imageManager.ResetTextures();
		}
	}
	auto skin = env->getSkin();
	skin->setFont(guiFont);
#define SKIN_SCALE(elem)skin->setSize(elem, Scale(skin->getSize(elem)));
	skin->setSize(irr::gui::EGDS_SCROLLBAR_SIZE, Scale(20));
	SKIN_SCALE(irr::gui::EGDS_MENU_HEIGHT)
	SKIN_SCALE(irr::gui::EGDS_WINDOW_BUTTON_WIDTH)
	SKIN_SCALE(irr::gui::EGDS_CHECK_BOX_WIDTH)
	SKIN_SCALE(irr::gui::EGDS_BUTTON_WIDTH)
	SKIN_SCALE(irr::gui::EGDS_BUTTON_HEIGHT)
	SKIN_SCALE(irr::gui::EGDS_TITLEBARTEXT_DISTANCE_X)
	SKIN_SCALE(irr::gui::EGDS_TITLEBARTEXT_DISTANCE_Y)
	SKIN_SCALE(irr::gui::EGDS_TEXT_DISTANCE_X)
	SKIN_SCALE(irr::gui::EGDS_TEXT_DISTANCE_Y)
	SKIN_SCALE(irr::gui::EGDS_MESSAGE_BOX_GAP_SPACE)
#undef SKIN_SCALE
	if(wInfos) {
		wInfos->setTabHeight(skin->getSize(irr::gui::EGDS_BUTTON_HEIGHT) + Scale(2));
		wInfos->setTabVerticalAlignment(irr::gui::EGUIA_UPPERLEFT);
	}
	if(prev_skin == NoSkinLabel()){
		for (int i = 0; i < irr::gui::EGDC_COUNT; ++i) {
			irr::video::SColor col = skin->getColor((irr::gui::EGUI_DEFAULT_COLOR)i);
			col.setAlpha(224);
			skin->setColor((irr::gui::EGUI_DEFAULT_COLOR)i, col);
		}
	}
	if(!firstrun)
		reapply_colors();
	if(wAbout)
		wAbout->setRelativePosition(irr::core::recti(0, 0, std::min(Scale(450), stAbout->getTextWidth() + Scale(20)), std::min(stAbout->getTextHeight() + Scale(40), Scale(700))));
	return applied;
}
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./deck/"), { EPRO_TEXT("ydk") })) {
		cbDeck->addItem(Utils::ToUnicodeIfNeeded(file.substr(0, file.size() - 4)).c_str());
	}
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(gGameConfig->lastdeck == cbDeck->getItem(i)) {
			cbDeck->setSelected(i);
			break;
		}
	}
}
void Game::RefreshLFLists() {
	cbHostLFList->clear();
	cbHostLFList->setSelected(0);
	cbDBLFList->clear();
	cbDBLFList->setSelected(0);
	auto prevFilter = std::max(0, cbFilterBanlist->getSelected());
	cbFilterBanlist->clear();
	cbFilterBanlist->addItem(fmt::format(L"[{}]", gDataManager->GetSysString(1226)).c_str());
	for (auto &list : gdeckManager->_lfList) {
		auto hostIndex = cbHostLFList->addItem(list.listName.c_str(), list.hash);
		auto deckIndex = cbDBLFList->addItem(list.listName.c_str(), list.hash);
		cbFilterBanlist->addItem(list.listName.c_str(), list.hash);
		if (gGameConfig->lastlflist == list.hash) {
			cbHostLFList->setSelected(hostIndex);
			cbDBLFList->setSelected(deckIndex);
		}
	}
	deckBuilder.filterList = &gdeckManager->_lfList[cbDBLFList->getSelected()];
	cbFilterBanlist->setSelected(prevFilter);
}
void Game::RefreshAiDecks() {
	gBot.bots.clear();
	std::ifstream windbots("WindBot/bots.json");
	if (windbots) {
		try {
			nlohmann::json j;
			windbots >> j;
			if (j.size() && j.is_array()) {
				for (auto& obj : j) {
					if (!obj["name"].is_string() || !obj["deck"].is_string() || !obj["difficulty"].is_number() || !obj["masterRules"].is_array())
						continue;
					WindBot bot;
					bot.name = BufferIO::DecodeUTF8s(obj["name"].get<std::string>());
					bot.deck = BufferIO::DecodeUTF8s(obj["deck"].get<std::string>());
					bot.difficulty = obj["difficulty"].get<int>();
					for (auto& masterRule : obj["masterRules"].get<std::vector<nlohmann::json>>()) {
						if (masterRule.is_number()) {
							bot.masterRules.insert(masterRule.get<int>());
						}
					}
					bot.version = CLIENT_VERSION;
#ifdef _WIN32
					bot.executablePath = filesystem->getAbsolutePath(EPRO_TEXT("./WindBot")).c_str();
#else
					if(gGameConfig->user_configs.size() && gGameConfig->user_configs["posixPathExtension"].is_string()) {
						bot.executablePath = gGameConfig->user_configs["posixPathExtension"].get<path_string>();
					} else if (gGameConfig->configs.size() && gGameConfig->configs["posixPathExtension"].is_string()) {
						bot.executablePath = gGameConfig->configs["posixPathExtension"].get<path_string>();
					} else {
						bot.executablePath = EPRO_TEXT("");
					}
#endif
					gBot.bots.push_back(bot);
				}
			}
		}
		catch (std::exception& e)
		{
			ErrorLog(fmt::format("Failed to load WindBot Ignite config json: {}", e.what()));
		}
	}
	else {
		ErrorLog("Failed to open WindBot Ignite config json!");
	}
}
void Game::RefreshReplay() {
	lstReplayList->resetPath();
}
void Game::RefreshSingleplay() {
	lstSinglePlayList->resetPath();
}
template<typename T>
inline void TrySaveInt(T& dest, const irr::gui::IGUIElement* src) {
	try {
		dest = static_cast<T>(std::stoul(src->getText()));
	}
	catch (...) {}
}
void Game::SaveConfig() {
	gGameConfig->nickname = ebNickName->getText();
	gGameConfig->lastallowedcards = cbRule->getSelected();
	gGameConfig->lastDuelParam = duel_param;
	gGameConfig->lastExtraRules = extra_rules;
	gGameConfig->lastDuelForbidden = forbiddentypes;
	TrySaveInt(gGameConfig->timeLimit, ebTimeLimit);
	TrySaveInt(gGameConfig->team1count, ebTeam1);
	TrySaveInt(gGameConfig->team2count, ebTeam2);
	TrySaveInt(gGameConfig->bestOf, ebBestOf);
	TrySaveInt(gGameConfig->startLP, ebStartLP);
	TrySaveInt(gGameConfig->startHand, ebStartHand);
	TrySaveInt(gGameConfig->drawCount, ebDrawCount);
	TrySaveInt(gGameConfig->antialias, gSettings.ebAntiAlias);
	gGameConfig->showConsole = gSettings.chkShowConsole->isChecked();
	gGameConfig->vsync = gSettings.chkVSync->isChecked();
	gGameConfig->relayDuel = btnRelayMode->isPressed();
	gGameConfig->noCheckDeck = chkNoCheckDeck->isChecked();
	gGameConfig->noShuffleDeck = chkNoShuffleDeck->isChecked();
	gGameConfig->botThrowRock = gBot.chkThrowRock->isChecked();
	gGameConfig->botMute = gBot.chkMute->isChecked();
	auto lastServerIndex = serverChoice->getSelected();
	if (lastServerIndex >= 0)
		gGameConfig->lastServer = serverChoice->getItem(lastServerIndex);
	gGameConfig->chkMAutoPos = tabSettings.chkMAutoPos->isChecked();
	gGameConfig->chkSTAutoPos = tabSettings.chkSTAutoPos->isChecked();
	gGameConfig->chkRandomPos = tabSettings.chkRandomPos->isChecked();
	gGameConfig->chkAutoChain = tabSettings.chkAutoChainOrder->isChecked();
	gGameConfig->chkWaitChain = tabSettings.chkNoChainDelay->isChecked();
	gGameConfig->chkIgnore1 = tabSettings.chkIgnoreOpponents->isChecked();
	gGameConfig->chkIgnore2 = tabSettings.chkIgnoreSpectators->isChecked();
	gGameConfig->chkHideHintButton = tabSettings.chkHideChainButtons->isChecked();
	gGameConfig->chkAnime = chkAnime->isChecked();
#ifdef UPDATE_URL
	gGameConfig->noClientUpdates = gSettings.chkUpdates->isChecked();
#endif
	gGameConfig->Save(EPRO_TEXT("./config/system.conf"));
}
Game::RepoGui* Game::AddGithubRepositoryStatusWindow(const GitRepo* repo) {
	std::wstring name = BufferIO::DecodeUTF8s(repo->repo_name);
	auto a = env->addWindow(Scale(0, 0, 470, 55), false, L"", mRepositoriesInfo);
	a->getCloseButton()->setVisible(false);
	a->setDraggable(false);
	a->setDrawTitlebar(false);
	a->setDrawBackground(false);
	env->addStaticText(name.c_str(), Scale(5, 5, 90 + 295, 20 + 5), false, false, a);
	auto& grepo = repoInfoGui[repo->repo_path];
	grepo.progress1 = new IProgressBar(env, Scale(5, 20 + 15, 170 + 295, 20 + 30), -1, a);
	grepo.progress1->addBorder(1);
	grepo.progress1->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
	grepo.progress1->drop();
	((irr::gui::CGUICustomContextMenu*)mRepositoriesInfo)->addItem(a, -1);
	grepo.history_button1 = env->addButton(Scale(90 + 295, 0, 170 + 295, 20 + 5), a, BUTTON_REPO_CHANGELOG, gDataManager->GetSysString(1443).c_str());
	defaultStrings.emplace_back(grepo.history_button1, 1443);
	grepo.history_button1->setEnabled(repo->ready);

	auto b = env->addWindow(Scale(0, 0, 10000, 55), false, L"", tabRepositories);
	b->getCloseButton()->setVisible(false);
	b->setDraggable(false);
	b->setDrawTitlebar(false);
	b->setDrawBackground(false);
	env->addStaticText(name.c_str(), Scale(5, 5, 300, 20 + 5), false, false, b);
	grepo.progress2 = new IProgressBar(env, Scale(5, 20 + 15, 300 - 5, 20 + 30), -1, b);
	grepo.progress2->addBorder(1);
	grepo.progress2->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
	grepo.progress2->drop();
	((irr::gui::CGUICustomContextMenu*)mTabRepositories)->addItem(b, -1);
	grepo.history_button2 = env->addButton(Scale(200, 5, 300 - 5, 20 + 10), b, BUTTON_REPO_CHANGELOG, gDataManager->GetSysString(1443).c_str());
	defaultStrings.emplace_back(grepo.history_button2, 1443);
	grepo.history_button2->setEnabled(repo->ready);
	return &grepo;
}
void Game::LoadGithubRepositories() {
	bool update_ready = true;
	for(auto& repo : gRepoManager->GetAllRepos()) {
		auto grepo = AddGithubRepositoryStatusWindow(repo);
		if(repo->ready && update_ready) {
			UpdateRepoInfo(repo, grepo);
			if(repo->is_language) {
				auto lang = Utils::ToPathString(repo->language);
				auto it = std::find_if(locales.begin(), locales.end(),
									   [&lang]
				(const std::pair<path_string, std::vector<path_string>>& locale)->bool {
					return locale.first == lang;
				});
				if(it != locales.end()) {
					it->second.push_back(Utils::ToPathString(repo->data_path));
				}
			}
		} else {
			update_ready = false;
		}
	}
}
void Game::UpdateRepoInfo(const GitRepo* repo, RepoGui* grepo) {
	if(repo->error.size()) {
		ErrorLog("The repo " + repo->url + " couldn't be cloned");
		ErrorLog("Error: " + repo->error);
		grepo->history_button1->setText(gDataManager->GetSysString(1434).c_str());
		defaultStrings.emplace_back(grepo->history_button1, 1434);
		grepo->history_button1->setEnabled(true);
		grepo->history_button2->setText(gDataManager->GetSysString(1434).c_str());
		defaultStrings.emplace_back(grepo->history_button2, 1434);
		grepo->history_button2->setEnabled(true);
		grepo->commit_history_full = fmt::format(L"{}\n{}",
												fmt::format(gDataManager->GetSysString(1435).c_str(), BufferIO::DecodeUTF8s(repo->url).c_str()).c_str(),
												fmt::format(gDataManager->GetSysString(1436).c_str(), BufferIO::DecodeUTF8s(repo->error).c_str()).c_str()
		);
		grepo->commit_history_partial = grepo->commit_history_full;
		return;
	}
	script_dirs.insert(script_dirs.begin(), Utils::ToPathString(repo->script_path));
	auto script_subdirs = Utils::FindSubfolders(Utils::ToPathString(repo->script_path));
	script_dirs.insert(script_dirs.begin(), script_subdirs.begin(), script_subdirs.end());
	pic_dirs.insert(pic_dirs.begin(), Utils::ToPathString(repo->pics_path));
	std::string text;
	std::for_each(repo->commit_history_full.begin(), repo->commit_history_full.end(), [&text](const std::string& n) { if(n.size()) { text += n + "\n\n"; }});
	if(text.size())
		text.erase(text.size() - 2, 2);
	grepo->commit_history_full = BufferIO::DecodeUTF8s(text);
	grepo->commit_history_partial.clear();
	if(repo->commit_history_partial.size()) {
		if(repo->commit_history_full.front() == repo->commit_history_partial.front() && repo->commit_history_full.back() == repo->commit_history_partial.back()) {
			grepo->commit_history_partial = grepo->commit_history_full;
		} else {
			text.clear();
			std::for_each(repo->commit_history_partial.begin(), repo->commit_history_partial.end(), [&text](const std::string& n) { if(n.size()) { text += n + "\n\n"; }});
			if(text.size())
				text.erase(text.size() - 2, 2);
			grepo->commit_history_partial = BufferIO::DecodeUTF8s(text);
		}
	} else {
		if(repo->warning.size()) {
			grepo->history_button1->setText(gDataManager->GetSysString(1448).c_str());
			grepo->commit_history_partial = fmt::format(L"{}\n{}\n\n{}",
				gDataManager->GetSysString(1449),
				gDataManager->GetSysString(1450),
				BufferIO::DecodeUTF8s(repo->warning)).c_str();
		} else {
			grepo->commit_history_partial = gDataManager->GetSysString(1446);
		}
	}
	grepo->history_button1->setEnabled(true);
	grepo->history_button2->setEnabled(true);
	if(repo->has_core) {
		cores_to_load.insert(cores_to_load.begin(), Utils::ToPathString(repo->core_path));
	}
	auto data_path = Utils::ToPathString(repo->data_path);
	auto lflist_path = Utils::ToPathString(repo->lflist_path);
	if (gdeckManager->LoadLFListSingle(data_path + EPRO_TEXT("lflist.conf")) || gdeckManager->LoadLFListFolder(lflist_path)) {
		gdeckManager->RefreshLFList();
		RefreshLFLists();
	}
}
void Game::LoadServers() {
	try {
		for(auto& _config : { std::ref(gGameConfig->user_configs), std::ref(gGameConfig->configs) }) {
			auto& config = _config.get();
			if(config.size() && config["servers"].is_array()) {
				for(auto& obj : config["servers"].get<std::vector<nlohmann::json>>()) {
					ServerInfo tmp_server;
					tmp_server.name = BufferIO::DecodeUTF8s(obj["name"].get<std::string>());
					tmp_server.address = BufferIO::DecodeUTF8s(obj["address"].get<std::string>());
					tmp_server.roomaddress = BufferIO::DecodeUTF8s(obj["roomaddress"].get<std::string>());
					tmp_server.roomlistport = obj["roomlistport"].get<int>();
					tmp_server.duelport = obj["duelport"].get<int>();
					int i = serverChoice->addItem(tmp_server.name.c_str());
					if(gGameConfig->lastServer == tmp_server.name)
						serverChoice->setSelected(i);
					ServerLobby::serversVector.push_back(std::move(tmp_server));
				}
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception occurred: ") + e.what());
	}
}
void Game::ShowCardInfo(uint32 code, bool resize, ImageManager::imgType type) {
	static ImageManager::imgType prevtype = ImageManager::imgType::ART;
	if(code == 0) {
		ClearCardInfo(0);
		return;
	}
	if (showingcard == code && !resize && !cardimagetextureloading)
		return;
	auto cd = gDataManager->GetCardData(code);
	if(!cd) {
		ClearCardInfo(0);
	}
	showingcard = code;
	int shouldrefresh = -1;
	auto img = imageManager.GetTextureCard(code, resize ? prevtype : type, false, true, &shouldrefresh);
	cardimagetextureloading = false;
	if(shouldrefresh == 2)
		cardimagetextureloading = true;
	imgCard->setImage(img);
	imgCard->setScaleImage(true);
	if(!cd)
		return;
	auto tmp_code = code;
	if(cd->alias && (cd->alias - code < CARD_ARTWORK_VERSIONS_OFFSET || code - cd->alias < CARD_ARTWORK_VERSIONS_OFFSET))
		tmp_code = cd->alias;
	stName->setText(gDataManager->GetName(tmp_code).c_str());
	stPasscodeScope->setText(fmt::format(L"[{:08}] {}", tmp_code, gDataManager->FormatScope(cd->ot)).c_str());
	stSetName->setText(L"");
	auto setcodes = cd->setcodes;
	if (cd->alias) {
		auto data = gDataManager->GetCardData(cd->alias);
		if(data)
			setcodes = data->setcodes;
	}
	if (setcodes.size()) {
		stSetName->setText((gDataManager->GetSysString(1329) + gDataManager->FormatSetName(setcodes)).c_str());
	}
	if(cd->type & TYPE_MONSTER) {
		stInfo->setText(fmt::format(L"[{}] {} {}", gDataManager->FormatType(cd->type), gDataManager->FormatAttribute(cd->attribute), gDataManager->FormatRace(cd->race)).c_str());
		std::wstring text;
		if(cd->type & TYPE_LINK){
			if(cd->attack < 0)
				text.append(fmt::format(L"?/LINK {}	  ", cd->level));
			else
				text.append(fmt::format(L"{}/LINK {}   ", cd->attack, cd->level));
			text.append(gDataManager->FormatLinkMarker(cd->link_marker));
		} else {
			text.append(fmt::format(L"[{}{}] ", (cd->type & TYPE_XYZ) ? L"\u2606" : L"\u2605", cd->level));
			if (cd->attack < 0 && cd->defense < 0)
				text.append(L"?/?");
			else if (cd->attack < 0)
				text.append(fmt::format(L"?/{}", cd->defense));
			else if (cd->defense < 0)
				text.append(fmt::format(L"{}/?", cd->attack));
			else
				text.append(fmt::format(L"{}/{}", cd->attack, cd->defense));
		}
		if(cd->type & TYPE_PENDULUM) {
			text.append(fmt::format(L"   {}/{}", cd->lscale, cd->rscale));
		}
		stDataInfo->setText(text.c_str());
	} else {
		if(cd->type & TYPE_SKILL) { // TYPE_SKILL created by hints
			// Hack: Race encodes the character for now
			stInfo->setText(fmt::format(L"[{}|{}]", gDataManager->FormatRace(cd->race, true), gDataManager->FormatType(cd->type)).c_str());
		} else {
			stInfo->setText(fmt::format(L"[{}]", gDataManager->FormatType(cd->type)).c_str());
		}
		if(cd->type & TYPE_LINK) {
			stDataInfo->setText(fmt::format(L"LINK {}   {}", cd->level, gDataManager->FormatLinkMarker(cd->link_marker)).c_str());
		} else
			stDataInfo->setText(L"");
	}
	RefreshCardInfoTextPositions();
	stText->setText(gDataManager->GetText(code).c_str());
}
void Game::RefreshCardInfoTextPositions() {
	const int xLeft = Scale(15);
	const int xRight = Scale(287 * window_scale.X);
	int offset = Scale(37);
	auto offsetIfVisibleWithContent = [&](irr::gui::IGUIStaticText* st) {
		if (st->isVisible() && wcscmp(st->getText(), L"")) {
			st->setRelativePosition(irr::core::recti(xLeft, offset, xRight, offset + st->getTextHeight()));
			offset += st->getTextHeight();
		}
	};
	offsetIfVisibleWithContent(stInfo);
	offsetIfVisibleWithContent(stDataInfo);
	offsetIfVisibleWithContent(stSetName);
	offsetIfVisibleWithContent(stPasscodeScope);
	stText->setRelativePosition(irr::core::recti(xLeft, offset, xRight, stText->getParent()->getAbsolutePosition().getHeight() - Scale(1)));
}
void Game::ClearCardInfo(int player) {
	imgCard->setImage(imageManager.tCover[player]);
	stName->setText(L"");
	stInfo->setText(L"");
	stDataInfo->setText(L"");
	stSetName->setText(L"");
	stPasscodeScope->setText(L"");
	stText->setText(L"");
	cardimagetextureloading = false;
	showingcard = 0;
}
void Game::AddChatMsg(const std::wstring& msg, int player, int type) {
	for(int i = 7; i > 0; --i) {
		chatMsg[i] = chatMsg[i - 1];
		chatTiming[i] = chatTiming[i - 1];
		chatType[i] = chatType[i - 1];
	}
	chatMsg[0].clear();
	chatTiming[0] = 1200.0f;
	chatType[0] = player;
	if(type == 0) {
		gSoundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.selfnames[player]);
	} else if(type == 1) {
		gSoundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.opponames[player]);
	} else if(type == 2) {
		switch(player) {
		case 7: //local name
			chatMsg[0].append(ebNickName->getText());
			break;
		case 8: //system custom message, no prefix.
			gSoundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
			chatMsg[0].append(gDataManager->GetSysString(1439).c_str());
			break;
		case 9: //error message
			chatMsg[0].append(gDataManager->GetSysString(1440).c_str());
			break;
		default: //from watcher or unknown
			if(player < 11 || player > 19)
				chatMsg[0].append(gDataManager->GetSysString(1441).c_str());
		}
	}
	chatMsg[0].append(L": ").append(msg);
	lstChat->addItem(chatMsg[0].c_str());
}
void Game::AddLog(const std::wstring& msg, int param) {
	logParam.push_back(param);
	lstLog->addItem(msg.c_str());
	if(!env->hasFocus(lstLog)) {
		lstLog->setSelected(-1);
	}
}
void Game::ClearChatMsg() {
	for(int i = 7; i >= 0; --i) {
		chatTiming[i] = 0;
	}
}
void Game::AddDebugMsg(const std::string& msg) {
	if (gGameConfig->coreLogOutput & CORE_LOG_TO_CHAT)
		AddChatMsg(BufferIO::DecodeUTF8s(msg), 9, 2);
	if (gGameConfig->coreLogOutput & CORE_LOG_TO_FILE)
		ErrorLog(fmt::format("{}: {}", BufferIO::EncodeUTF8s(gDataManager->GetSysString(1440)), msg));
}
void Game::ClearTextures() {
	matManager.mCard.setTexture(0, 0);
	imgCard->setImage(imageManager.tCover[0]);
	imgCard->setScaleImage(true);
	btnPSAU->setImage();
	btnPSDU->setImage();
	for(int i=0; i<=4; ++i) {
		btnCardSelect[i]->setImage();
		btnCardDisplay[i]->setImage();
	}
	imageManager.ClearTexture();
}
void Game::CloseDuelWindow() {
	for(auto wit = fadingList.begin(); wit != fadingList.end(); ++wit) {
		if(wit->isFadein)
			wit->autoFadeoutFrame = 1;
	}
	mTopMenu->setVisible(true);
	wACMessage->setVisible(false);
	wANAttribute->setVisible(false);
	wANCard->setVisible(false);
	wANNumber->setVisible(false);
	wANRace->setVisible(false);
	wCardImg->setVisible(false);
	wCardSelect->setVisible(false);
	wCardDisplay->setVisible(false);
	wCmdMenu->setVisible(false);
	wFTSelect->setVisible(false);
	wHand->setVisible(false);
	wInfos->setVisible(false);
	wMessage->setVisible(false);
	wOptions->setVisible(false);
	wPhase->setVisible(false);
	wPosSelect->setVisible(false);
	wQuery->setVisible(false);
	wReplayControl->setVisible(false);
	wReplaySave->setVisible(false);
	stHintMsg->setVisible(false);
	btnSideOK->setVisible(false);
	btnSideShuffle->setVisible(false);
	btnSideSort->setVisible(false);
	btnSideReload->setVisible(false);
	btnLeaveGame->setVisible(false);
	btnRestartSingle->setVisible(false);
	btnSpectatorSwap->setVisible(false);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	btnCancelOrFinish->setVisible(false);
	btnShuffle->setVisible(false);
	wChat->setVisible(false);
	lstLog->clear();
	logParam.clear();
	lstHostList->clear();
	DuelClient::hosts.clear();
	ClearTextures();
	stName->setText(L"");
	stInfo->setText(L"");
	stDataInfo->setText(L"");
	stSetName->setText(L"");
	stPasscodeScope->setText(L"");
	stText->setText(L"");
	stTip->setText(L"");
	cardimagetextureloading = false;
	showingcard = 0;
	closeDoneSignal.Set();
}
void Game::PopupMessage(const std::wstring& text,const std::wstring& caption) {
	popupCheck.lock();
	queued_msg = text;
	queued_caption = caption;
	popupCheck.unlock();
}
uint8 Game::LocalPlayer(uint8 player) {
	return dInfo.isFirst ? player : 1 - player;
}
void Game::UpdateDuelParam() {
	ReloadCBDuelRule();
	uint32 flag = 0, filter = 0x100;
	for (int i = 0; i < schkCustomRules; ++i, filter <<= 1)
		if (chkCustomRules[i]->isChecked()) {
			flag |= filter;
		}
	const uint32 limits[] = { TYPE_FUSION, TYPE_SYNCHRO, TYPE_XYZ, TYPE_PENDULUM, TYPE_LINK };
	uint32 flag2 = 0;
	for (int i = 0; i < (sizeof(chkTypeLimit) / sizeof(irr::gui::IGUICheckBox*)); ++i) {
		if (chkTypeLimit[i]->isChecked()) {
			flag2 |= limits[i];
		}
	}
	switch (flag) {
	case DUEL_MODE_SPEED: {
		cbDuelRule->setSelected(5);
		break;
	}
	case DUEL_MODE_RUSH: {
		cbDuelRule->setSelected(6);
		break;
	}
// NOTE: intentional case fallthrough
#define CHECK(MR) case DUEL_MODE_MR##MR:{ cbDuelRule->setSelected(MR - 1); if (flag2 == DUEL_MODE_MR##MR##_FORB) { cbDuelRule->removeItem(7); break; } }
	CHECK(1)
	CHECK(2)
	CHECK(3)
	CHECK(4)
	CHECK(5)
#undef CHECK
	default: {
		cbDuelRule->addItem(gDataManager->GetSysString(1630).c_str());
		cbDuelRule->setSelected(7);
		break;
	}
	}
	duel_param = flag;
	forbiddentypes = flag2;
}
void Game::UpdateExtraRules(bool set) {
	for(int i = 0; i < (sizeof(chkRules) / sizeof(irr::gui::IGUICheckBox*)); i++)
		chkRules[i]->setEnabled(true);
	if(set) {
		for(int flag = 1, i = 0; i < (sizeof(chkRules) / sizeof(irr::gui::IGUICheckBox*)); i++, flag = flag << 1)
			chkRules[i]->setChecked(extra_rules & flag);
		return;
	}
	if(chkRules[0]->isChecked()) {
		chkRules[1]->setEnabled(false);
		chkRules[4]->setEnabled(false);
	}
	if(chkRules[1]->isChecked()) {
		chkRules[0]->setEnabled(false);
		chkRules[4]->setEnabled(false);
	}
	if(chkRules[4]->isChecked()) {
		chkRules[0]->setEnabled(false);
		chkRules[1]->setEnabled(false);
	}
	if(chkRules[5]->isChecked()) {
		chkRules[6]->setEnabled(false);
		chkRules[7]->setEnabled(false);
	}
	if(chkRules[6]->isChecked()) {
		chkRules[5]->setEnabled(false);
		chkRules[7]->setEnabled(false);
	}
	if(chkRules[7]->isChecked()) {
		chkRules[5]->setEnabled(false);
		chkRules[6]->setEnabled(false);
	}
	extra_rules = 0;
	for(int flag = 1, i = 0; i < (sizeof(chkRules) / sizeof(irr::gui::IGUICheckBox*)); i++, flag = flag << 1) {
		if(chkRules[i]->isChecked())
			extra_rules |= flag;
	}
}
int Game::GetMasterRule(uint32 param, uint32 forbiddentypes, int* truerule) {
	if(truerule)
		*truerule = 0;
#define CHECK(MR) case DUEL_MODE_MR##MR:{ if (truerule && forbiddentypes == DUEL_MODE_MR##MR##_FORB) *truerule = MR; break; }
	switch(param) {
		CHECK(1)
		CHECK(2)
		CHECK(3)
		CHECK(4)
		CHECK(5)
	}
#undef CHECK
	if (truerule && !*truerule)
		*truerule = 6;
	if ((param & DUEL_PZONE) && (param & DUEL_SEPARATE_PZONE) && (param & DUEL_EMZONE))
		return 5;
	else if(param & DUEL_EMZONE)
		return 4;
	else if ((param & DUEL_PZONE) && (param & DUEL_SEPARATE_PZONE))
		return 3;
	else
		return 2;
}
void Game::SetPhaseButtons(bool visibility) {
	if(gGameConfig->alternative_phase_layout) {
		wPhase->setRelativePosition(Resize(940, 80, 990, 340));
		btnDP->setRelativePosition(Resize(0, 0, 50, 20));
		btnSP->setRelativePosition(Resize(0, 40, 50, 60));
		btnM1->setRelativePosition(Resize(0, 80, 50, 100));
		btnBP->setRelativePosition(Resize(0, 120, 50, 140));
		btnM2->setRelativePosition(Resize(0, 160, 50, 180));
		btnEP->setRelativePosition(Resize(0, 200, 50, 220));
		btnShuffle->setRelativePosition(Resize(0, 240, 50, 260));
	} else {
		// reset master rule 4 phase button position
		wPhase->setRelativePosition(Resize(480, 310, 855, 330));
		if(dInfo.duel_params & DUEL_3_COLUMNS_FIELD) {
			if(dInfo.duel_field >= 4) {
				wPhase->setRelativePosition(Resize(480, 290, 855, 350));
				btnShuffle->setRelativePosition(Resize(0, 40, 50, 60));
				btnDP->setRelativePosition(Resize(0, 40, 50, 60));
				btnSP->setRelativePosition(Resize(0, 40, 50, 60));
				btnM1->setRelativePosition(Resize(160, 20, 210, 40));
				btnBP->setRelativePosition(Resize(160, 20, 210, 40));
				btnM2->setRelativePosition(Resize(160, 20, 210, 40));
				btnEP->setRelativePosition(Resize(310, 0, 360, 20));
			} else {
				btnShuffle->setRelativePosition(Resize(65, 0, 115, 20));
				btnDP->setRelativePosition(Resize(65, 0, 115, 20));
				btnSP->setRelativePosition(Resize(65, 0, 115, 20));
				btnM1->setRelativePosition(Resize(130, 0, 180, 20));
				btnBP->setRelativePosition(Resize(195, 0, 245, 20));
				btnM2->setRelativePosition(Resize(260, 0, 310, 20));
				btnEP->setRelativePosition(Resize(260, 0, 310, 20));
			}
		} else {
			btnDP->setRelativePosition(Resize(0, 0, 50, 20));
			if(dInfo.duel_field >= 4) {
				btnSP->setRelativePosition(Resize(0, 0, 50, 20));
				btnM1->setRelativePosition(Resize(160, 0, 210, 20));
				btnBP->setRelativePosition(Resize(160, 0, 210, 20));
				btnM2->setRelativePosition(Resize(160, 0, 210, 20));
			} else {
				btnSP->setRelativePosition(Resize(65, 0, 115, 20));
				btnM1->setRelativePosition(Resize(130, 0, 180, 20));
				btnBP->setRelativePosition(Resize(195, 0, 245, 20));
				btnM2->setRelativePosition(Resize(260, 0, 310, 20));
			}
			btnEP->setRelativePosition(Resize(320, 0, 370, 20));
			btnShuffle->setRelativePosition(Resize(0, 0, 50, 20));
		}
	}
	if(visibility) {
		btnDP->setVisible(gGameConfig->alternative_phase_layout || btnDP->isSubElement());
		btnSP->setVisible(gGameConfig->alternative_phase_layout || btnSP->isSubElement());
		btnM1->setVisible(gGameConfig->alternative_phase_layout || btnM1->isSubElement());
		btnM2->setVisible(gGameConfig->alternative_phase_layout || btnM2->isSubElement());
		btnBP->setVisible(gGameConfig->alternative_phase_layout || btnBP->isSubElement());
		btnEP->setVisible(gGameConfig->alternative_phase_layout || btnEP->isSubElement());
	}
}
void Game::SetMessageWindow() {
	if(is_building || dInfo.isInDuel) {
		wMessage->setRelativePosition(ResizeWin(490, 200, 840, 340));
		wACMessage->setRelativePosition(ResizeWin(490, 240, 840, 300));
	} else {
		SetCentered(wMessage);
		SetCentered(wACMessage);
	}
}
bool Game::HasFocus(irr::gui::EGUI_ELEMENT_TYPE type) const {
	irr::gui::IGUIElement* focus = env->getFocus();
	return focus && focus->hasType(type);
}
void Game::RefreshUICoreVersion() {
	if (coreloaded) {
		int major, minor;
		OCG_GetVersion(&major, &minor);
		auto label = corename.length()
			? fmt::format(gDataManager->GetSysString(2013), major, minor, corename)
			: fmt::format(gDataManager->GetSysString(2010), major, minor);
		stCoreVersion->setText(label.c_str());
	} else {
		stCoreVersion->setText(L"");
	}
	auto w1 = stVersion->getTextWidth();
	auto w2 = stCoreVersion->getTextWidth();
	wVersion->setRelativePosition(irr::core::recti(0, 0, Scale(20) + std::max({ Scale(280), w1, w2 }), Scale(135)));
}
std::wstring Game::GetLocalizedExpectedCore() {
	return fmt::format(gDataManager->GetSysString(2011), OCG_VERSION_MAJOR, OCG_VERSION_MINOR);
}
std::wstring Game::GetLocalizedCompatVersion() {
	return fmt::format(gDataManager->GetSysString(2012), PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
}
void Game::ReloadCBSortType() {
	cbSortType->clear();
	for (int i = 1370; i <= 1373; i++)
		cbSortType->addItem(gDataManager->GetSysString(i).c_str());
}
void Game::ReloadCBCardType() {
	cbCardType->clear();
	cbCardType->addItem(gDataManager->GetSysString(1310).c_str());
	cbCardType->addItem(gDataManager->GetSysString(1312).c_str());
	cbCardType->addItem(gDataManager->GetSysString(1313).c_str());
	cbCardType->addItem(gDataManager->GetSysString(1314).c_str());
}
void Game::ReloadCBCardType2() {
	cbCardType2->clear();
	cbCardType2->setEnabled(true);
	switch (cbCardType->getSelected()) {
	case 0:
		cbCardType2->setEnabled(false);
		cbCardType2->addItem(gDataManager->GetSysString(1310).c_str(), 0);
		break;
	case 1:
		cbCardType2->addItem(gDataManager->GetSysString(1080).c_str(), 0);
		cbCardType2->addItem(gDataManager->GetSysString(1054).c_str(), TYPE_MONSTER + TYPE_NORMAL);
		cbCardType2->addItem(gDataManager->GetSysString(1055).c_str(), TYPE_MONSTER + TYPE_EFFECT);
		cbCardType2->addItem(gDataManager->GetSysString(1056).c_str(), TYPE_MONSTER + TYPE_FUSION);
		cbCardType2->addItem(gDataManager->GetSysString(1057).c_str(), TYPE_MONSTER + TYPE_RITUAL);
		cbCardType2->addItem(gDataManager->GetSysString(1063).c_str(), TYPE_MONSTER + TYPE_SYNCHRO);
		cbCardType2->addItem(gDataManager->GetSysString(1073).c_str(), TYPE_MONSTER + TYPE_XYZ);
		cbCardType2->addItem(gDataManager->GetSysString(1074).c_str(), TYPE_MONSTER + TYPE_PENDULUM);
		cbCardType2->addItem(gDataManager->GetSysString(1076).c_str(), TYPE_MONSTER + TYPE_LINK);
		cbCardType2->addItem(gDataManager->GetSysString(1075).c_str(), TYPE_MONSTER + TYPE_SPSUMMON);
		cbCardType2->addItem((gDataManager->GetSysString(1054) + L"|" + gDataManager->GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_TUNER);
		cbCardType2->addItem((gDataManager->GetSysString(1054) + L"|" + gDataManager->GetSysString(1074)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_PENDULUM);
		cbCardType2->addItem((gDataManager->GetSysString(1063) + L"|" + gDataManager->GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_SYNCHRO + TYPE_TUNER);
		cbCardType2->addItem(gDataManager->GetSysString(1062).c_str(), TYPE_MONSTER + TYPE_TUNER);
		cbCardType2->addItem(gDataManager->GetSysString(1061).c_str(), TYPE_MONSTER + TYPE_GEMINI);
		cbCardType2->addItem(gDataManager->GetSysString(1060).c_str(), TYPE_MONSTER + TYPE_UNION);
		cbCardType2->addItem(gDataManager->GetSysString(1059).c_str(), TYPE_MONSTER + TYPE_SPIRIT);
		cbCardType2->addItem(gDataManager->GetSysString(1071).c_str(), TYPE_MONSTER + TYPE_FLIP);
		cbCardType2->addItem(gDataManager->GetSysString(1072).c_str(), TYPE_MONSTER + TYPE_TOON);
		break;
	case 2:
		cbCardType2->addItem(gDataManager->GetSysString(1080).c_str(), 0);
		cbCardType2->addItem(gDataManager->GetSysString(1054).c_str(), TYPE_SPELL);
		cbCardType2->addItem(gDataManager->GetSysString(1066).c_str(), TYPE_SPELL + TYPE_QUICKPLAY);
		cbCardType2->addItem(gDataManager->GetSysString(1067).c_str(), TYPE_SPELL + TYPE_CONTINUOUS);
		cbCardType2->addItem(gDataManager->GetSysString(1057).c_str(), TYPE_SPELL + TYPE_RITUAL);
		cbCardType2->addItem(gDataManager->GetSysString(1068).c_str(), TYPE_SPELL + TYPE_EQUIP);
		cbCardType2->addItem(gDataManager->GetSysString(1069).c_str(), TYPE_SPELL + TYPE_FIELD);
		cbCardType2->addItem(gDataManager->GetSysString(1076).c_str(), TYPE_SPELL + TYPE_LINK);
		break;
	case 3:
		cbCardType2->addItem(gDataManager->GetSysString(1080).c_str(), 0);
		cbCardType2->addItem(gDataManager->GetSysString(1054).c_str(), TYPE_TRAP);
		cbCardType2->addItem(gDataManager->GetSysString(1067).c_str(), TYPE_TRAP + TYPE_CONTINUOUS);
		cbCardType2->addItem(gDataManager->GetSysString(1070).c_str(), TYPE_TRAP + TYPE_COUNTER);
		break;
	}
}
void Game::ReloadCBLimit() {
	cbLimit->clear();
	cbLimit->addItem(gDataManager->GetSysString(1310).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1316).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1317).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1318).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1320).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1900).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1901).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1902).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1903).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1910).c_str());
	cbLimit->addItem(gDataManager->GetSysString(1911).c_str());
	if (chkAnime->isChecked()) {
		cbLimit->addItem(gDataManager->GetSysString(1265).c_str());
		cbLimit->addItem(gDataManager->GetSysString(1266).c_str());
		cbLimit->addItem(gDataManager->GetSysString(1267).c_str());
		cbLimit->addItem(gDataManager->GetSysString(1268).c_str());
	}
}
void Game::ReloadCBAttribute() {
	cbAttribute->clear();
	cbAttribute->addItem(gDataManager->GetSysString(1310).c_str(), 0);
	for (int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(gDataManager->FormatAttribute(filter).c_str(), filter);
}
void Game::ReloadCBRace() {
	cbRace->clear();
	cbRace->addItem(gDataManager->GetSysString(1310).c_str(), 0);
	for (int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(gDataManager->FormatRace(filter).c_str(), filter);
}
void Game::ReloadCBFilterRule() {
	cbFilterRule->clear();
	cbFilterRule->addItem(fmt::format(L"[{}]", gDataManager->GetSysString(1225)).c_str());
	for (auto i = 1900; i <= 1904; ++i)
		cbFilterRule->addItem(gDataManager->GetSysString(i).c_str());
}
void Game::ReloadCBDuelRule() {
	cbDuelRule->clear();
	cbDuelRule->addItem(gDataManager->GetSysString(1260).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1261).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1262).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1263).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1264).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1258).c_str());
	cbDuelRule->addItem(gDataManager->GetSysString(1259).c_str());
}
void Game::ReloadCBRule() {
	cbRule->clear();
	for (auto i = 1900; i <= 1904; ++i)
		cbRule->addItem(gDataManager->GetSysString(i).c_str());
}
void Game::ReloadCBCurrentSkin() {
	gSettings.cbCurrentSkin->clear();
	int selectedSkin = gSettings.cbCurrentSkin->addItem(gDataManager->GetSysString(2065).c_str());
	auto skins = skinSystem->listSkins();
	for (int i = skins.size() - 1; i >= 0; i--) {
		auto itemIndex = gSettings.cbCurrentSkin->addItem(Utils::ToUnicodeIfNeeded(skins[i].c_str()).c_str());
		if (gGameConfig->skin == skins[i].c_str())
			selectedSkin = itemIndex;
	}
	gSettings.cbCurrentSkin->setSelected(selectedSkin);
}
void Game::ReloadCBCoreLogOutput() {
	gSettings.cbCoreLogOutput->clear();
	for (int i = CORE_LOG_NONE; i <= 3; i++) {
		auto itemIndex = gSettings.cbCoreLogOutput->addItem(gDataManager->GetSysString(2000 + i).c_str(), i);
		if (gGameConfig->coreLogOutput == i) {
			gSettings.cbCoreLogOutput->setSelected(itemIndex);
		}
	}
}
void Game::ReloadElementsStrings() {
	ShowCardInfo(showingcard, true);

	for(auto& elem : defaultStrings) {
		elem.first->setText(gDataManager->GetSysString(elem.second).c_str());
	}

	unsigned int nullLFlist = gdeckManager->_lfList.size() - 1;
	gdeckManager->_lfList[nullLFlist].listName = gDataManager->GetSysString(1442);
	auto prev = cbDBLFList->getSelected();
	cbDBLFList->removeItem(nullLFlist);
	cbDBLFList->addItem(gdeckManager->_lfList[nullLFlist].listName.c_str(), gdeckManager->_lfList[nullLFlist].hash);
	cbDBLFList->setSelected(prev);
	prev = cbHostLFList->getSelected();
	cbHostLFList->removeItem(nullLFlist);
	cbHostLFList->addItem(gdeckManager->_lfList[nullLFlist].listName.c_str(), gdeckManager->_lfList[nullLFlist].hash);
	cbHostLFList->setSelected(prev);

	prev = cbSortType->getSelected();
	ReloadCBSortType();
	cbSortType->setSelected(prev);

	prev = cbCardType->getSelected();
	ReloadCBCardType();
	cbCardType->setSelected(prev);

	prev = cbCardType2->getSelected();
	ReloadCBCardType2();
	cbCardType2->setSelected(prev);

	prev = cbLimit->getSelected();
	ReloadCBLimit();
	cbLimit->setSelected(prev);

	prev = cbAttribute->getSelected();
	ReloadCBAttribute();
	cbAttribute->setSelected(prev);

	prev = cbRace->getSelected();
	ReloadCBRace();
	cbRace->setSelected(prev);

	if(is_building) {
		btnLeaveGame->setText(gDataManager->GetSysString(1306).c_str());
	} else if(!dInfo.isReplay && !dInfo.isSingleMode && dInfo.player_type < (dInfo.team1 + dInfo.team2)) {
		btnLeaveGame->setText(gDataManager->GetSysString(1351).c_str());
	} else if(dInfo.player_type == 7) {
		btnLeaveGame->setText(gDataManager->GetSysString(1350).c_str());
	} else if(dInfo.isSingleMode) {
		btnLeaveGame->setText(gDataManager->GetSysString(1210).c_str());
	}

	prev = cbFilterRule->getSelected();
	ReloadCBFilterRule();
	cbFilterRule->setSelected(prev);

	prev = cbDuelRule->getSelected();
	if (prev >= 5) {
		UpdateDuelParam();
	} else {
		ReloadCBDuelRule();
		cbDuelRule->setSelected(prev);
	}

	prev = cbRule->getSelected();
	ReloadCBRule();
	cbRule->setSelected(prev);

	prev = gSettings.cbCoreLogOutput->getSelected();
	ReloadCBCoreLogOutput();
	gSettings.cbCoreLogOutput->setSelected(prev);

	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(1, gDataManager->GetSysString(1225).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(2, gDataManager->GetSysString(1227).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(3, gDataManager->GetSysString(1236).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(4, gDataManager->GetSysString(1226).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(5, gDataManager->GetSysString(2030).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(6, gDataManager->GetSysString(2024).c_str());
	((irr::gui::CGUICustomTable*)roomListTable)->setColumnText(7, gDataManager->GetSysString(1988).c_str());
	roomListTable->setColumnWidth(0, roomListTable->getColumnWidth(0));

	mTopMenu->setItemText(0, gDataManager->GetSysString(2045).c_str()); //mRepositoriesInfo
	mTopMenu->setItemText(1, gDataManager->GetSysString(1970).c_str()); //mAbout
	mTopMenu->setItemText(2, gDataManager->GetSysString(2040).c_str()); //mVersion
	RefreshUICoreVersion();
	stExpectedCoreVersion->setText(GetLocalizedExpectedCore().c_str());
	stCompatVersion->setText(GetLocalizedCompatVersion().c_str());

#define TYPECHK(id,stringid) chkTypeLimit[id]->setText(fmt::sprintf(gDataManager->GetSysString(1627), gDataManager->GetSysString(stringid)).c_str());
	TYPECHK(0, 1056);
	TYPECHK(1, 1063);
	TYPECHK(2, 1073);
	TYPECHK(3, 1074);
	TYPECHK(4, 1076);
#undef TYPECHK

	ReloadCBCurrentSkin();
}
void Game::OnResize() {
	const auto waboutpos = wAbout->getAbsolutePosition();
	stAbout->setRelativePosition(irr::core::recti(10, 10, std::min<uint32>(window_size.Width - waboutpos.UpperLeftCorner.X,
																		 std::min<uint32>(Scale(440), stAbout->getTextWidth() + Scale(10))),
												  std::min<uint32>(window_size.Height - waboutpos.UpperLeftCorner.Y,
																   std::min<uint32>(stAbout->getTextHeight() + Scale(10), Scale(690)))));
	wRoomListPlaceholder->setRelativePosition(irr::core::recti(0, 0, window_size.Width, window_size.Height));
	wMainMenu->setRelativePosition(ResizeWin(mainMenuLeftX, 200, mainMenuRightX, 450));
	wBtnSettings->setRelativePosition(ResizeWin(0, 610, 30, 640));
	SetCentered(wCommitsLog);
	SetCentered(updateWindow);
	wDeckEdit->setRelativePosition(Resize(309, 8, 605, 130));
	cbDBLFList->setRelativePosition(Resize(80, 5, 220, 30));
	cbDBDecks->setRelativePosition(Resize(80, 35, 220, 60));
	btnSaveDeck->setRelativePosition(Resize(225, 35, 290, 60));
	btnRenameDeck->setRelativePosition(Resize(5, 65, 75, 90));
	ebDeckname->setRelativePosition(Resize(80, 65, 220, 90));
	btnSaveDeckAs->setRelativePosition(Resize(225, 65, 290, 90));
	btnShuffleDeck->setRelativePosition(Resize(5, 95, 75, 120));
	btnSortDeck->setRelativePosition(Resize(80, 95, 145, 120));
	btnClearDeck->setRelativePosition(Resize(155, 95, 220, 120));
	btnDeleteDeck->setRelativePosition(Resize(225, 95, 290, 120));
	btnHandTest->setRelativePosition(Resize(205, 90, 295, 130));

	wSort->setRelativePosition(Resize(930, 132, 1020, 156));
	cbSortType->setRelativePosition(Resize(10, 2, 85, 22));
	wFilter->setRelativePosition(Resize(610, 8, 1020, 130));
	scrFilter->setRelativePosition(Resize(999, 161, 1019, 629));
	cbCardType->setRelativePosition(Resize(60, 3, 120, 23));
	cbCardType2->setRelativePosition(Resize(130, 3, 190, 23));
	cbRace->setRelativePosition(Resize(60, 49, 190, 69));
	cbAttribute->setRelativePosition(Resize(60, 26, 190, 46));
	cbLimit->setRelativePosition(Resize(260, 3, 390, 23));
	ebStar->setRelativePosition(Resize(60, 72, 100, 92));
	ebScale->setRelativePosition(Resize(150, 72, 190, 92));
	ebAttack->setRelativePosition(Resize(260, 26, 340, 46));
	ebDefense->setRelativePosition(Resize(260, 49, 340, 69));
	ebCardName->setRelativePosition(Resize(260, 72, 390, 92));
	btnEffectFilter->setRelativePosition(Resize(345, 28, 390, 69));
	btnStartFilter->setRelativePosition(Resize(327, 96, 390, 118));
	btnClearFilter->setRelativePosition(Resize(260, 96, 322, 118));
	btnMarksFilter->setRelativePosition(Resize(155, 96, 240, 118));
	chkAnime->setRelativePosition(Resize(10, 96, 150, 118));

	wCategories->setRelativePosition(ResizeWin(450, 60, 1000, 270));
	wLinkMarks->setRelativePosition(ResizeWin(700, 30, 820, 150));
	stBanlist->setRelativePosition(ResizeWin(10, 9, 100, 29));
	stDeck->setRelativePosition(ResizeWin(10, 39, 100, 59));
	stCategory->setRelativePosition(ResizeWin(10, 5, 70, 25));
	stLimit->setRelativePosition(ResizeWin(205, 5, 280, 25));
	stAttribute->setRelativePosition(ResizeWin(10, 28, 70, 48));
	stRace->setRelativePosition(ResizeWin(10, 51, 70, 71));
	stAttack->setRelativePosition(ResizeWin(205, 28, 280, 48));
	stDefense->setRelativePosition(ResizeWin(205, 51, 280, 71));
	stStar->setRelativePosition(ResizeWin(10, 74, 80, 94));
	stSearch->setRelativePosition(ResizeWin(205, 74, 280, 94));
	stScale->setRelativePosition(ResizeWin(110, 74, 150, 94));
	btnSideOK->setRelativePosition(Resize(510, 40, 820, 80));
	btnSideShuffle->setRelativePosition(Resize(310, 100, 370, 130));
	btnSideSort->setRelativePosition(Resize(375, 100, 435, 130));
	btnSideReload->setRelativePosition(Resize(440, 100, 500, 130));

	wLanWindow->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wCreateHost->setRelativePosition(ResizeWin(320, 100, 700, 520));
	if (dInfo.opponames.size() + dInfo.selfnames.size()>=5) {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 500));
		wHostPrepareR->setRelativePosition(ResizeWin(750, 120, 950, 500));
		wHostPrepareL->setRelativePosition(ResizeWin(70, 120, 270, 500));
	} else {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 440));
		wHostPrepareR->setRelativePosition(ResizeWin(750, 120, 950, 440));
		wHostPrepareL->setRelativePosition(ResizeWin(70, 120, 270, 440));
	}
	wRules->setRelativePosition(ResizeWin(630, 100, 1000, 310));
	wCustomRulesL->setRelativePosition(ResizeWin(20, 100, 320, 430));
	wCustomRulesR->setRelativePosition(ResizeWin(700, 100, 1000, 430));
	wReplay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wSinglePlay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	gBot.window->setRelativePosition(irr::core::position2di(wHostPrepare->getAbsolutePosition().LowerRightCorner.X, wHostPrepare->getAbsolutePosition().UpperLeftCorner.Y));

	wHand->setRelativePosition(ResizeWin(500, 450, 825, 605));
	wFTSelect->setRelativePosition(ResizeWin(550, 240, 780, 340));
	SetMessageWindow();
	wQuery->setRelativePosition(ResizeWin(490, 200, 840, 340));
	wOptions->setRelativePosition(ResizeWinFromCenter(0, 0, wOptions->getRelativePosition().getWidth(), wOptions->getRelativePosition().getHeight(), 135));
	wPosSelect->setRelativePosition(ResizeWin(340, 200, 935, 410));
	wCardSelect->setRelativePosition(ResizeWin(320, 100, 1000, 400));
	wCardDisplay->setRelativePosition(ResizeWin(320, 100, 1000, 400));
	wANNumber->setRelativePosition(ResizeWin(550, 200, 780, 295));
	wANCard->setRelativePosition(ResizeWin(430, 170, 840, 370));
	wANAttribute->setRelativePosition(ResizeWin(500, 200, 830, 285));
	wANRace->setRelativePosition(ResizeWin(480, 200, 850, 410));
	wReplaySave->setRelativePosition(ResizeWin(510, 200, 820, 320));
	stHintMsg->setRelativePosition(ResizeWin(500, 60, 820, 90));

	wCardImg->setRelativePosition(Resize(1, 1, 1 + CARD_IMG_WIDTH + 20, 1 + CARD_IMG_HEIGHT + 18));
	imgCard->setRelativePosition(Resize(10, 9, 10 + CARD_IMG_WIDTH, 9 + CARD_IMG_HEIGHT));
	wInfos->setRelativePosition(Resize(1, 275, (infosExpanded == 1) ? 1023 : 301, 639));
	for(auto& window : repoInfoGui) {
		window.second.progress2->setRelativePosition(Scale(5, 20 + 15, (300 - 8) * window_scale.X, 20 + 30));
		window.second.history_button2->setRelativePosition(irr::core::recti(ResizeX(200), 5, ResizeX(300 - 5), Scale(20 + 10)));
	}
	stName->setRelativePosition(Scale(10, 10, 287 * window_scale.X, 32));

	auto logParentHeight = lstLog->getParent()->getAbsolutePosition().getHeight();

	auto clearSize = Resize(160, 300 - Scale(7), 260, 325 - Scale(7));
	auto expandSize = Resize(40, 300 - Scale(7), 140, 325 - Scale(7));

	btnClearLog->setRelativePosition(clearSize);
	btnExpandLog->setRelativePosition(expandSize);

	btnClearChat->setRelativePosition(clearSize);
	btnExpandChat->setRelativePosition(expandSize);

	auto lstsSize = Resize(10, 10, infosExpanded ? 1012 : 290, 0);
	lstsSize.LowerRightCorner.Y = expandSize.UpperLeftCorner.Y - Scale(10);

	lstLog->setRelativePosition(lstsSize);
	lstChat->setRelativePosition(lstsSize);

	imageManager.ClearTexture(true);
	btnPSAD->setImage(imageManager.tCover[0]);
	btnPSDD->setImage(imageManager.tCover[0]);

	ShowCardInfo(showingcard, true);

	auto tabsystemParentPos = tabSystem->getParent()->getAbsolutePosition();
	tabSystem->setRelativePosition(irr::core::recti(0, 0, tabsystemParentPos.getWidth(), tabsystemParentPos.getHeight()));
	tabSettings.scrSoundVolume->setRelativePosition(irr::core::recti(Scale(85), Scale(265), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(280)));
	tabSettings.scrMusicVolume->setRelativePosition(irr::core::recti(Scale(85), Scale(325), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(340)));
	btnTabShowSettings->setRelativePosition(irr::core::recti(Scale(20), Scale(445), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(470)));

	SetCentered(gSettings.window);

	wChat->setRelativePosition(irr::core::recti(wInfos->getRelativePosition().LowerRightCorner.X + Scale(4), Scale<irr::s32>(615.0f  * window_scale.Y), (window_size.Width - Scale(4 * window_scale.X)), (window_size.Height - Scale(2))));

	if(dInfo.isSingleMode)
		btnLeaveGame->setRelativePosition(Resize(205, 5, 295, 45));
	else
		btnLeaveGame->setRelativePosition(Resize(205, 5, 295, 80));
	btnRestartSingle->setRelativePosition(Resize(205, 50, 295, 90));
	wReplayControl->setRelativePosition(Resize(205, 143, 295, 273));
	btnReplayStart->setRelativePosition(Resize(5, 5, 85, 25));
	btnReplayPause->setRelativePosition(Resize(5, 5, 85, 25));
	btnReplayStep->setRelativePosition(Resize(5, 55, 85, 75));
	btnReplayUndo->setRelativePosition(Resize(5, 80, 85, 100));
	btnReplaySwap->setRelativePosition(Resize(5, 30, 85, 50));
	btnReplayExit->setRelativePosition(Resize(5, 105, 85, 125));

	SetPhaseButtons();
	btnSpectatorSwap->setRelativePosition(Resize(205, 100, 295, 135));
	btnChainAlways->setRelativePosition(Resize(205, 140, 295, 175));
	btnChainIgnore->setRelativePosition(Resize(205, 100, 295, 135));
	btnChainWhenAvail->setRelativePosition(Resize(205, 180, 295, 215));
	btnCancelOrFinish->setRelativePosition(Resize(205, 230, 295, 265));

	roomListTable->setRelativePosition(irr::core::recti(ResizeX(1), chkShowActiveRooms->getRelativePosition().LowerRightCorner.Y + ResizeY(10), ResizeX(1024 - 2), btnLanRefresh2->getRelativePosition().UpperLeftCorner.Y - ResizeY(25)));
	roomListTable->setColumnWidth(0, roomListTable->getColumnWidth(0));
	roomListTable->addRow(roomListTable->getRowCount());
	roomListTable->removeRow(roomListTable->getRowCount() - 1);
	roomListTable->setSelected(-1);
}
irr::core::recti Game::Resize(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2) {
	x = x * window_scale.X;
	y = y * window_scale.Y;
	x2 = x2 * window_scale.X;
	y2 = y2 * window_scale.Y;
	return Scale(x, y, x2, y2);
}
irr::core::recti Game::Resize(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2, irr::s32 dx, irr::s32 dy, irr::s32 dx2, irr::s32 dy2) {
	x = x * window_scale.X + dx;
	y = y * window_scale.Y + dy;
	x2 = x2 * window_scale.X + dx2;
	y2 = y2 * window_scale.Y + dy2;
	return Scale(x, y, x2, y2);
}
irr::core::vector2di Game::Resize(irr::s32 x, irr::s32 y, bool reverse) {
	if(reverse) {
		x = (x / window_scale.X) / gGameConfig->dpi_scale;
		y = (y / window_scale.Y) / gGameConfig->dpi_scale;
	} else {
		x = x * window_scale.X * gGameConfig->dpi_scale;
		y = y * window_scale.Y * gGameConfig->dpi_scale;
	}
	return { x, y };
}
irr::core::recti Game::ResizeWin(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2, bool chat) {
	irr::s32 sx = x2 - x;
	irr::s32 sy = y2 - y;
	if(chat) {
		y = window_size.Height - sy;
		x2 = window_size.Width;
		y2 = y + sy;
		return Scale(x, y, x2, y2);
	}
	x = (x + sx / 2) * window_scale.X - sx / 2;
	y = (y + sy / 2) * window_scale.Y - sy / 2;
	x2 = sx + x;
	y2 = sy + y;
	return Scale(x, y, x2, y2);
}
void Game::SetCentered(irr::gui::IGUIElement* elem) {
	if(is_building || dInfo.isInDuel)
		elem->setRelativePosition(ResizeWinFromCenter(0, 0, elem->getRelativePosition().getWidth(), elem->getRelativePosition().getHeight(), Scale(155)));
	else
		elem->setRelativePosition(ResizeWinFromCenter(0, 0, elem->getRelativePosition().getWidth(), elem->getRelativePosition().getHeight()));
}
irr::core::recti Game::ResizeElem(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2, bool scale) {
	irr::s32 sx = x2 - x;
	irr::s32 sy = y2 - y;
	x = (x + sx / 2 - 100) * window_scale.X - sx / 2 + 100;
	y = y * window_scale.Y;
	x2 = sx + x;
	y2 = sy + y;
	return scale ? Scale(x, y, x2, y2) : irr::core::recti{x, y, x2, y2};
}
irr::core::recti Game::ResizePhaseHint(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2, irr::s32 width) {
	auto res = Resize(x, y, x2, y2);
	res.UpperLeftCorner.X -= width / 2;
	return res;
}
irr::core::recti Game::ResizeWinFromCenter(irr::s32 x, irr::s32 y, irr::s32 x2, irr::s32 y2, irr::s32 xoff, irr::s32 yoff) {
	auto size = driver->getScreenSize();
	irr::core::recti rect(0, 0, size.Width, size.Height);
	auto center = rect.getCenter();
	irr::core::dimension2du sizes((x + x2) / 2, (y + y2) / 2);
	return irr::core::recti((center.X - sizes.Width) + xoff, center.Y - sizes.Height + yoff, center.X + sizes.Width + xoff, center.Y + sizes.Height + yoff);
}
void Game::ValidateName(irr::gui::IGUIElement* obj) {
	std::wstring text = obj->getText();
	const wchar_t chars[] = L"<>:\"/\\|?*";
	for(auto& forbid : chars)
		text.erase(std::remove(text.begin(), text.end(), forbid), text.end());
	if(text.size() != wcslen(obj->getText()))
		obj->setText(text.c_str());
}
std::wstring Game::ReadPuzzleMessage(const std::wstring& script_name) {
	std::ifstream infile(Utils::ToPathString(script_name), std::ifstream::in);
	std::string str;
	std::string res = "";
	size_t start = std::string::npos;
	bool stop = false;
	while(std::getline(infile, str) && !stop) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		bool was_empty = str.empty();
		if(start == std::string::npos) {
			start = str.find("--[[message");
			if(start == std::string::npos)
				continue;
			str = str.substr(start + 11);
		}
		size_t end = str.find("]]");
		if(end != std::string::npos) {
			str = str.substr(0, end);
			stop = true;
		}
		if(str.empty() && !was_empty)
			continue;
		if(!res.empty() || was_empty)
			res += "\n";
		res += str;
	}
	return BufferIO::DecodeUTF8s(res);
}
std::vector<char> Game::LoadScript(const std::string& _name) {
	std::vector<char> buffer;
	std::ifstream script;
	path_string name = Utils::ToPathString(_name);
	for(auto& path : script_dirs) {
		if(path == EPRO_TEXT("archives")) {
			auto lockedIrrFile = Utils::FindFileInArchives(EPRO_TEXT("script/"), name);
			if(!lockedIrrFile)
				continue;
			buffer.resize(lockedIrrFile.reader->getSize());
			if (lockedIrrFile.reader->read(buffer.data(), buffer.size()) == buffer.size())
				return buffer;
		} else {
			script.open(path + name, std::ifstream::binary);
			if(script.is_open())
				break;
		}
	}
	if(!script.is_open()) {
		script.open(name, std::ifstream::binary);
		if(!script.is_open())
			return buffer;
	}
	buffer.insert(buffer.begin(), std::istreambuf_iterator<char>(script), std::istreambuf_iterator<char>());
	return buffer;
}
bool Game::LoadScript(OCG_Duel pduel, const std::string& script_name) {
	auto buf = LoadScript(script_name);
	return buf.size() && OCG_LoadScript(pduel, buf.data(), buf.size(), script_name.c_str());
}
OCG_Duel Game::SetupDuel(OCG_DuelOptions opts) {
	opts.cardReader = (OCG_DataReader)&DataManager::CardReader;
	opts.payload1 = &*(gDataManager);
	opts.scriptReader = (OCG_ScriptReader)&ScriptReader;
	opts.payload2 = this;
	opts.logHandler = (OCG_LogHandler)&MessageHandler;
	opts.payload3 = this;
	OCG_Duel pduel = nullptr;
	OCG_CreateDuel(&pduel, opts);
	LoadScript(pduel, "constant.lua");
	LoadScript(pduel, "utility.lua");
	return pduel;
}
int Game::ScriptReader(void* payload, OCG_Duel duel, const char* name) {
	return static_cast<Game*>(payload)->LoadScript(duel, name);
}
void Game::MessageHandler(void* payload, const char* string, int type) {
	Game* game = static_cast<Game*>(payload);
	std::stringstream ss(string);
	std::string str;
	while(std::getline(ss, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		game->AddDebugMsg(str);
		if(type > 1)
			fmt::print("{}\n", str);
	}
}
void Game::UpdateDownloadBar(int percentage, int cur, int tot, const char* filename, bool is_new, void* payload) {
	Game* game = static_cast<Game*>(payload);
	std::lock_guard<std::mutex> lk(game->gMutex);
	game->updateProgressBottom->setProgress(percentage);
	if(is_new)
		game->updateProgressText->setText(
			fmt::format(L"{}\n{}",
				fmt::format(gDataManager->GetSysString(1462), BufferIO::DecodeUTF8s(filename)),
				fmt::format(gDataManager->GetSysString(1464), cur, tot)
			).c_str());
}
void Game::UpdateUnzipBar(unzip_payload* payload) {
	UnzipperPayload* unzipper = static_cast<UnzipperPayload*>(payload->payload);
	Game* game = static_cast<Game*>(unzipper->payload);
	std::lock_guard<std::mutex> lk(game->gMutex);
	// current archive
	if(payload->is_new) {
		game->updateProgressText->setText(
			fmt::format(L"{}\n{}",
				fmt::format(gDataManager->GetSysString(1463), Utils::ToUnicodeIfNeeded(unzipper->filename)),
				fmt::format(gDataManager->GetSysString(1464), unzipper->cur, unzipper->tot)
			).c_str());
		game->updateProgressTop->setVisible(true);
		game->updateSubprogressText->setText(fmt::format(gDataManager->GetSysString(1465), Utils::ToUnicodeIfNeeded(payload->filename)).c_str());
	}
	game->updateProgressTop->setProgress(std::round((double)payload->cur / (double)payload->tot * 100));
	// current file in archive
	game->updateProgressBottom->setProgress(payload->percentage);
}
void Game::PopulateResourcesDirectories() {
	script_dirs.push_back(EPRO_TEXT("./expansions/script/"));
	auto expansions_subdirs = Utils::FindSubfolders(EPRO_TEXT("./expansions/script/"));
	script_dirs.insert(script_dirs.end(), expansions_subdirs.begin(), expansions_subdirs.end());
	script_dirs.push_back(EPRO_TEXT("archives"));
	script_dirs.push_back(EPRO_TEXT("./script/"));
	auto script_subdirs = Utils::FindSubfolders(EPRO_TEXT("./script/"));
	script_dirs.insert(script_dirs.end(), script_subdirs.begin(), script_subdirs.end());
	pic_dirs.push_back(EPRO_TEXT("./expansions/pics/"));
	pic_dirs.push_back(EPRO_TEXT("archives"));
	pic_dirs.push_back(EPRO_TEXT("./pics/"));
	cover_dirs.push_back(EPRO_TEXT("./expansions/pics/cover/"));
	cover_dirs.push_back(EPRO_TEXT("archives"));
	cover_dirs.push_back(EPRO_TEXT("./pics/cover/"));
	field_dirs.push_back(EPRO_TEXT("./expansions/pics/field/"));
	field_dirs.push_back(EPRO_TEXT("archives"));
	field_dirs.push_back(EPRO_TEXT("./pics/field/"));
}

void Game::PopulateLocales() {
	locales.clear();
	for(auto& locale : Utils::FindSubfolders(EPRO_TEXT("./config/languages/"), 1, false)) {
		locales.emplace_back(locale, std::vector<path_string>());
	}
}

void Game::ApplyLocale(int index, bool forced) {
	static int previndex = 0;
	if(index < 0 || index > locales.size())
		return;
	if(previndex == index && !forced)
		return;
	previndex = index;
	gDataManager->ClearLocaleStrings();
	gDataManager->ClearLocaleTexts();
	if(index > 0) {
		try {
			gGameConfig->locale = locales[index - 1].first;
			auto locale = fmt::format(EPRO_TEXT("./config/languages/{}"), gGameConfig->locale);
			for(auto& file : Utils::FindFiles(locale, { EPRO_TEXT("cdb") })) {
				gDataManager->LoadLocaleDB(fmt::format(EPRO_TEXT("{}/{}"), locale, file));
			}
			gDataManager->LoadLocaleStrings(fmt::format(EPRO_TEXT("{}/strings.conf"), locale));
			auto& extra = locales[index - 1].second;
			bool refresh_db = false;
			for(auto& path : extra) {
				for(auto& file : Utils::FindFiles(path, { EPRO_TEXT("cdb") }, 0))
					refresh_db = gDataManager->LoadLocaleDB(path + file) || refresh_db;
				gDataManager->LoadLocaleStrings(path + EPRO_TEXT("strings.conf"));
			}
			if(refresh_db && is_building && deckBuilder.results.size())
				deckBuilder.StartFilter(true);
		}
		catch(...) {
			return;
		}
	} else
		gGameConfig->locale = EPRO_TEXT("en");
	ReloadElementsStrings();
}

}
