#include <sstream>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif
#ifdef __ANDROID__
#include <COGLES2ExtensionHandler.h>
#include <COGLESExtensionHandler.h>
#include <COGLES2Driver.h>
#include <COGLESDriver.h>
#include "porting_android.h"
#endif
#include <irrlicht.h>
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
#define MATERIAL_GUARD(f) do {mainGame->driver->enableMaterial2D(true); f; mainGame->driver->enableMaterial2D(false);} while(false);
#else
#define MATERIAL_GUARD(f) do {f;} while(false);
#endif

//extern ygo::RepoManager* repoManager; //gframe.cpp

unsigned short PRO_VERSION = 0x1348;

//nlohmann::json configs;

namespace ygo {

Game* mainGame;

bool Game::Initialize(std::shared_ptr<DataHandler> handlers) {
	globalHandlers = handlers;
	srand(time(0));
	//LoadConfig();
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = globalHandlers->configs->antialias;
#ifndef __ANDROID__
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if(globalHandlers->configs->use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
#endif
		params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<u32>(Scale(1024), Scale(640));
#else
	/*if(glversion == 0) {
		params.DriverType = irr::video::EDT_OGLES1;
	} else {*/
		params.DriverType = irr::video::EDT_OGLES1;
	//}
	params.PrivateData = porting::app_global;
	params.Bits = 24;
	params.ZBufferBits = 16;
	params.AntiAlias = 0;
	params.WindowSize = irr::core::dimension2d<u32>(0, 0);
#endif
	params.Vsync = globalHandlers->configs->use_vsync;
	device = irr::createDeviceEx(params);
	if(!device) {
		ErrorLog("Failed to create Irrlicht Engine device!");
		return false;
	}
	filesystem = device->getFileSystem();
#ifdef __ANDROID__
	porting::mainDevice = device;
#endif
	coreloaded = true;
#ifdef YGOPRO_BUILD_DLL
	if(!(ocgcore = LoadOCGcore(globalHandlers->configs->working_directory + EPRO_TEXT("./"))) && !(ocgcore = LoadOCGcore(globalHandlers->configs->working_directory + EPRO_TEXT("./expansions/"))))
		coreloaded = false;
#endif
	skinSystem = new CGUISkinSystem((globalHandlers->configs->working_directory + EPRO_TEXT("./skin")).c_str(), device);
	if(!skinSystem)
		ErrorLog("Couldn't create skin system");
	auto logger = device->getLogger();
	logger->setLogLevel(ELL_NONE);
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
#ifdef __ANDROID__
	if(driver->getDriverType() == irr::video::EDT_OGLES2) {
		isNPOTSupported = ((irr::video::COGLES2Driver*)driver)->queryOpenGLFeature(irr::video::COGLES2ExtensionHandler::IRR_OES_texture_npot);
	} else {
		isNPOTSupported = ((irr::video::COGLES1Driver*)driver)->queryOpenGLFeature(irr::video::COGLES1ExtensionHandler::IRR_OES_texture_npot);
	}
	if(isNPOTSupported) {
		driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	} else {
		driver->setTextureCreationFlag(irr::video::ETCF_ALLOW_NON_POWER_2, true);
		driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	}
#else
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
#endif
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	imageManager.SetDevice(device);
	if(!imageManager.Initial()) {
		ErrorLog("Failed to load textures!");
		return false;
	}
	LoadPicUrls();
	RefreshAiDecks();
	discord.Initialize(filesystem->getWorkingDirectory().c_str());
	mainGame->discord.UpdatePresence(DiscordWrapper::INITIALIZE);
	PopulateResourcesDirectories();
	deckManager.LoadLFList();
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, globalHandlers->configs->numfont.c_str(), Scale(16));
	adFont = irr::gui::CGUITTFont::createTTFont(env, globalHandlers->configs->numfont.c_str(), Scale(12));
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, globalHandlers->configs->numfont.c_str(), Scale(48));
	guiFont = irr::gui::CGUITTFont::createTTFont(env, globalHandlers->configs->textfont.c_str(), Scale(globalHandlers->configs->textfontsize));
	textFont = guiFont;
	if(!numFont || !textFont || !adFont || !lpcFont || !guiFont) {
		ErrorLog("Failed to load font(s)!");
		return false;
	}
	if(!ApplySkin(globalHandlers->configs->skin, false, true)) {
		globalHandlers->configs->skin = NoSkinLabel();
	}
	smgr = device->getSceneManager();
	device->setWindowCaption(L"EDOPro by Project Ignis");
	device->setResizable(true);
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON hBigIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	HWND hWnd = reinterpret_cast<HWND>(driver->getExposedVideoData().D3D9.HWnd);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (long)hSmallIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (long)hBigIcon);
	DragAcceptFiles(hWnd, TRUE);
#endif
	wCommitsLog = env->addWindow(Scale(0, 0, 500 + 10, 400 + 35 + 35), false, globalHandlers->dataManager->GetSysString(1209).c_str());
	defaultStrings.emplace_back(wCommitsLog, 1209);
	wCommitsLog->setVisible(false);
	wCommitsLog->getCloseButton()->setEnabled(false);
	wCommitsLog->getCloseButton()->setVisible(false);
	stCommitLog = irr::gui::CGUICustomText::addCustomText(L"", false, env, wCommitsLog, -1, Scale(5, 30, 505, 430));
	stCommitLog->setWordWrap(true);
	((CGUICustomText*)stCommitLog)->enableScrollBar();
	btnCommitLogExit = env->addButton(Scale(215, 435, 285, 460), wCommitsLog, BUTTON_REPO_CHANGELOG_EXIT, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCommitLogExit, 1211);
	chkCommitLogExpand = env->addCheckBox(false, Scale(295, 435, 500, 460), wCommitsLog, BUTTON_REPO_CHANGELOG_EXPAND, globalHandlers->dataManager->GetSysString(1447).c_str());
	defaultStrings.emplace_back(chkCommitLogExpand, 1447);
	mTopMenu = irr::gui::CGUICustomMenu::addCustomMenu(env);
	mRepositoriesInfo = mTopMenu->getSubMenu(mTopMenu->addItem(globalHandlers->dataManager->GetSysString(2045).c_str(), 1, true, true));
	mAbout = mTopMenu->getSubMenu(mTopMenu->addItem(globalHandlers->dataManager->GetSysString(1970).c_str(), 2, true, true));
	wAbout = env->addWindow(Scale(0, 0, 450, 700), false, L"", mAbout);
	wAbout->getCloseButton()->setVisible(false);
	wAbout->setDraggable(false);
	wAbout->setDrawTitlebar(false);
	wAbout->setDrawBackground(false);
	stAbout = env->addStaticText(L"EDOPro by Project Ignis\n"
								 L"The bleeding-edge automatic duel simulator\n"
								 L"\n"
								 L"Copyright (C) 2019  Edoardo Lolletti (edo9300) and others\n"
								 L"Card scripts by Project Ignis\n"
								 L"Licensed under the GNU AGPLv3 or later. See LICENSE for more details.\n"
								 L"https://github.com/edo9300/ygopro\n"
								 L"https://github.com/edo9300/ygopro-core\n"
								 L"More links and contributors to be added\n"
								 L"\n"
								 L"Forked from Fluorohydride's YGOPro.\n"
								 L"\n"
								 L"Yu-Gi-Oh! is a trademark of Shueisha and Konami.\n"
								 L"This project is not affiliated with or endorsed by Shueisha or Konami.",
								 Scale(10, 10, 440, 690), false, true, wAbout);
	((CGUICustomContextMenu*)mAbout)->addItem(wAbout, -1);
	wAbout->setRelativePosition(recti(0, 0, std::min(Scale(450), stAbout->getTextWidth() + Scale(20)), std::min(stAbout->getTextHeight() + Scale(20), Scale(700))));
	//main menu
	wMainMenu = env->addWindow(Scale(370, 200, 650, 415), false, fmt::format(L"EDOPro by Project Ignis | v{:X}.0{:X}.{:X}", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf).c_str());
	wMainMenu->getCloseButton()->setVisible(false);
	//wMainMenu->setVisible(!is_from_discord);
#define OFFSET(x1, y1, x2, y2) Scale(10, 30 + offset, 270, 60 + offset)
	int offset = 0;
	btnOnlineMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_ONLINE_MULTIPLAYER, globalHandlers->dataManager->GetSysString(2042).c_str());
	defaultStrings.emplace_back(btnOnlineMode, 2042);
	offset += 35;
	btnLanMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, globalHandlers->dataManager->GetSysString(1200).c_str());
	defaultStrings.emplace_back(btnLanMode, 1200);
	offset += 35;
	btnSingleMode = env->addButton(OFFSET(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, globalHandlers->dataManager->GetSysString(1201).c_str());
	defaultStrings.emplace_back(btnSingleMode, 1201);
	offset += 35;
	btnReplayMode = env->addButton(OFFSET(10, 100, 270, 130), wMainMenu, BUTTON_REPLAY_MODE, globalHandlers->dataManager->GetSysString(1202).c_str());
	defaultStrings.emplace_back(btnReplayMode, 1202);
	offset += 35;
	btnDeckEdit = env->addButton(OFFSET(10, 135, 270, 165), wMainMenu, BUTTON_DECK_EDIT, globalHandlers->dataManager->GetSysString(1204).c_str());
	defaultStrings.emplace_back(btnDeckEdit, 1204);
	offset += 35;
	btnModeExit = env->addButton(OFFSET(10, 170, 270, 200), wMainMenu, BUTTON_MODE_EXIT, globalHandlers->dataManager->GetSysString(1210).c_str());
	defaultStrings.emplace_back(btnModeExit, 1210);
	offset += 35;
#undef OFFSET
	btnSingleMode->setEnabled(coreloaded);
	//lan mode
	wLanWindow = env->addWindow(Scale(220, 100, 800, 520), false, globalHandlers->dataManager->GetSysString(1200).c_str());
	defaultStrings.emplace_back(wLanWindow, 1200);
	wLanWindow->getCloseButton()->setVisible(false);
	wLanWindow->setVisible(false);
	irr::gui::IGUIElement* tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1220).c_str(), Scale(10, 30, 220, 50), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1220);
	ebNickName = env->addEditBox(globalHandlers->configs->nickname.c_str(), Scale(110, 25, 450, 50), true, wLanWindow, EDITBOX_NICKNAME);
	ebNickName->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	lstHostList = env->addListBox(Scale(10, 60, 570, 320), wLanWindow, LISTBOX_LAN_HOST, true);
	lstHostList->setItemHeight(Scale(18));
	btnLanRefresh = env->addButton(Scale(240, 325, 340, 350), wLanWindow, BUTTON_LAN_REFRESH, globalHandlers->dataManager->GetSysString(1217).c_str());
	defaultStrings.emplace_back(btnLanRefresh, 1217);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1221).c_str(), Scale(10, 360, 220, 380), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1221);
	ebJoinHost = env->addEditBox(globalHandlers->configs->lasthost.c_str(), Scale(110, 355, 350, 380), true, wLanWindow);
	ebJoinHost->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(globalHandlers->configs->lastport.c_str(), Scale(360, 355, 420, 380), true, wLanWindow, EDITBOX_PORT_BOX);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1222).c_str(), Scale(10, 390, 220, 410), false, false, wLanWindow);
	defaultStrings.emplace_back(tmpptr, 1222);
	ebJoinPass = env->addEditBox(globalHandlers->configs->roompass.c_str(), Scale(110, 385, 420, 410), true, wLanWindow);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnJoinHost = env->addButton(Scale(460, 355, 570, 380), wLanWindow, BUTTON_JOIN_HOST, globalHandlers->dataManager->GetSysString(1223).c_str());
	defaultStrings.emplace_back(btnJoinHost, 1223);
	btnJoinCancel = env->addButton(Scale(460, 385, 570, 410), wLanWindow, BUTTON_JOIN_CANCEL, globalHandlers->dataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnJoinCancel, 1212);
	btnCreateHost = env->addButton(Scale(460, 25, 570, 50), wLanWindow, BUTTON_CREATE_HOST, globalHandlers->dataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(btnCreateHost, 1224);
	btnCreateHost->setEnabled(coreloaded);
	//create host
	wCreateHost = env->addWindow(Scale(320, 100, 700, 520), false, globalHandlers->dataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(wCreateHost, 1224);
	wCreateHost->getCloseButton()->setVisible(false);
	wCreateHost->setVisible(false);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1226).c_str(), Scale(20, 30, 220, 50), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1226);
	cbLFlist = env->addComboBox(Scale(140, 25, 300, 50), wCreateHost);
	for (unsigned int i = 0; i < deckManager._lfList.size(); ++i) {
		cbLFlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);
		if (globalHandlers->configs->lastlflist == deckManager._lfList[i].hash)
			cbLFlist->setSelected(i);
	}
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1225).c_str(), Scale(20, 60, 220, 80), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1225);
	cbRule = env->addComboBox(Scale(140, 55, 300, 80), wCreateHost);
	for (auto i = 1900; i <= 1904; ++i)
		cbRule->addItem(globalHandlers->dataManager->GetSysString(i).c_str());
	cbRule->setSelected(globalHandlers->configs->lastallowedcards);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1227).c_str(), Scale(20, 90, 220, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1227);
	ebTeam1 = env->addEditBox(L"1", Scale(140, 85, 170, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	auto vsstring = env->addStaticText(globalHandlers->dataManager->GetSysString(1380).c_str(), Scale(175, 85, 195, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(vsstring, 1380);
	vsstring->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebTeam2 = env->addEditBox(L"1", Scale(200, 85, 230, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	vsstring = env->addStaticText(globalHandlers->dataManager->GetSysString(1381).c_str(), Scale(235, 85, 280, 110), false, false, wCreateHost);
	defaultStrings.emplace_back(vsstring, 1381);
	vsstring->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	ebBestOf = env->addEditBox(L"1", Scale(285, 85, 315, 110), true, wCreateHost);
	ebBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRelayMode = env->addButton(Scale(325, 85, 370, 110), wCreateHost, -1, globalHandlers->dataManager->GetSysString(1247).c_str());
	defaultStrings.emplace_back(btnRelayMode, 1247);
	btnRelayMode->setIsPushButton(true);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1237).c_str(), Scale(20, 120, 320, 140), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1237);
	ebTimeLimit = env->addEditBox(L"180", Scale(140, 115, 220, 140), true, wCreateHost);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1228).c_str(), Scale(20, 150, 320, 170), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1228);
	btnRuleCards = env->addButton(Scale(260, 325, 370, 350), wCreateHost, BUTTON_RULE_CARDS, globalHandlers->dataManager->GetSysString(1625).c_str());
	defaultStrings.emplace_back(btnRuleCards, 1625);
	wRules = env->addWindow(Scale(630, 100, 1000, 310), false, L"");
	wRules->getCloseButton()->setVisible(false);
	wRules->setDrawTitlebar(false);
	wRules->setDraggable(true);
	wRules->setVisible(false);
	btnRulesOK = env->addButton(Scale(135, 175, 235, 200), wRules, BUTTON_RULE_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnRulesOK, 1211);
	for(int i = 0; i < 14; ++i) {
		chkRules[i] = env->addCheckBox(false, Scale(10 + (i % 2) * 150, 10 + (i / 2) * 20, 200 + (i % 2) * 120, 30 + (i / 2) * 20), wRules, CHECKBOX_EXTRA_RULE, globalHandlers->dataManager->GetSysString(1132 + i).c_str());
		defaultStrings.emplace_back(chkRules[i], 1132 + i);
	}
	extra_rules = 0;
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1236).c_str(), Scale(20, 180, 220, 200), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1236);
	cbDuelRule = env->addComboBox(Scale(140, 175, 300, 200), wCreateHost, COMBOBOX_DUEL_RULE);
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1260).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1261).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1262).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1263).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1264).c_str());
	cbDuelRule->setSelected(DEFAULT_DUEL_RULE - 1);
	btnCustomRule = env->addButton(Scale(305, 175, 370, 200), wCreateHost, BUTTON_CUSTOM_RULE, globalHandlers->dataManager->GetSysString(1626).c_str());
	defaultStrings.emplace_back(btnCustomRule, 1626);
	wCustomRules = env->addWindow(Scale(700, 100, 910, 430), false, L"");
	wCustomRules->getCloseButton()->setVisible(false);
	wCustomRules->setDrawTitlebar(false);
	wCustomRules->setDraggable(true);
	wCustomRules->setVisible(false);
	int spacing = 0;
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1629).c_str(), Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	defaultStrings.emplace_back(tmpptr, 1629);
	spacing++;
	for(int i = 0; i < (sizeof(chkCustomRules) / sizeof(irr::gui::IGUICheckBox*)); ++i, ++spacing) {
		chkCustomRules[i] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 390 + i, globalHandlers->dataManager->GetSysString(1631 + i).c_str());
		defaultStrings.emplace_back(chkCustomRules[i], 1631 + i);
	}
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1628).c_str(), Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	defaultStrings.emplace_back(tmpptr, 1628);
#define TYPECHK(id,stringid) spacing++;\
	chkTypeLimit[id] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(globalHandlers->dataManager->GetSysString(1627), globalHandlers->dataManager->GetSysString(stringid)).c_str());
	TYPECHK(0, 1056);
	TYPECHK(1, 1063);
	TYPECHK(2, 1073);
	TYPECHK(3, 1074);
	TYPECHK(4, 1076);
#undef TYPECHK
	btnCustomRulesOK = env->addButton(Scale(55, 290, 155, 315), wCustomRules, BUTTON_CUSTOM_RULE_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCustomRulesOK, 1211);
	forbiddentypes = DUEL_MODE_MR5_FORB;
	duel_param = DUEL_MODE_MR5;
	chkNoCheckDeck = env->addCheckBox(false, Scale(20, 210, 170, 230), wCreateHost, -1, globalHandlers->dataManager->GetSysString(1229).c_str());
	defaultStrings.emplace_back(chkNoCheckDeck, 1229);
	chkNoShuffleDeck = env->addCheckBox(false, Scale(180, 210, 360, 230), wCreateHost, -1, globalHandlers->dataManager->GetSysString(1230).c_str());
	defaultStrings.emplace_back(chkNoShuffleDeck, 1230);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1231).c_str(), Scale(20, 240, 320, 260), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1231);
	ebStartLP = env->addEditBox(L"8000", Scale(140, 235, 220, 260), true, wCreateHost);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1232).c_str(), Scale(20, 270, 320, 290), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1232);
	ebStartHand = env->addEditBox(L"5", Scale(140, 265, 220, 290), true, wCreateHost);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1233).c_str(), Scale(20, 300, 320, 320), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1233);
	ebDrawCount = env->addEditBox(L"1", Scale(140, 295, 220, 320), true, wCreateHost);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1234).c_str(), Scale(10, 330, 220, 350), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1234);
	ebServerName = env->addEditBox(globalHandlers->configs->gamename.c_str(), Scale(110, 325, 250, 350), true, wCreateHost);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1235).c_str(), Scale(10, 360, 220, 380), false, false, wCreateHost);
	defaultStrings.emplace_back(tmpptr, 1235);
	ebServerPass = env->addEditBox(L"", Scale(110, 355, 250, 380), true, wCreateHost);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnHostConfirm = env->addButton(Scale(260, 355, 370, 380), wCreateHost, BUTTON_HOST_CONFIRM, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnHostConfirm, 1211);
	btnHostCancel = env->addButton(Scale(260, 385, 370, 410), wCreateHost, BUTTON_HOST_CANCEL, globalHandlers->dataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnHostCancel, 1212);
	stHostPort = env->addStaticText(globalHandlers->dataManager->GetSysString(1238).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	defaultStrings.emplace_back(stHostPort, 1238);
	ebHostPort = env->addEditBox(globalHandlers->configs->serverport.c_str(), Scale(110, 385, 250, 410), true, wCreateHost, EDITBOX_PORT_BOX);
	ebHostPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHostNotes = env->addStaticText(globalHandlers->dataManager->GetSysString(2024).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	defaultStrings.emplace_back(stHostNotes, 2024);
	stHostNotes->setVisible(false);
	ebHostNotes = env->addEditBox(L"", Scale(110, 385, 250, 410), true, wCreateHost);
	ebHostNotes->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebHostNotes->setVisible(false);
	//host(single)
	wHostPrepare = env->addWindow(Scale(270, 120, 750, 440), false, globalHandlers->dataManager->GetSysString(1250).c_str());
	defaultStrings.emplace_back(wHostPrepare, 1250);
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	wHostPrepare2 = env->addWindow(Scale(750, 120, 950, 440), false, globalHandlers->dataManager->GetSysString(1625).c_str());
	defaultStrings.emplace_back(wHostPrepare2, 1625);
	wHostPrepare2->getCloseButton()->setVisible(false);
	wHostPrepare2->setVisible(false);
	stHostPrepRule2 = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare2, -1, Scale(10, 30, 460, 350));
	stHostPrepRule2->setWordWrap(true);
	btnHostPrepDuelist = env->addButton(Scale(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, globalHandlers->dataManager->GetSysString(1251).c_str());
	defaultStrings.emplace_back(btnHostPrepDuelist, 1251);
	btnHostPrepWindBot = env->addButton(Scale(170, 30, 270, 55), wHostPrepare, BUTTON_HP_AI_TOGGLE, globalHandlers->dataManager->GetSysString(2050).c_str());
	defaultStrings.emplace_back(btnHostPrepWindBot, 2050);
	for(int i = 0; i < 6; ++i) {
		btnHostPrepKick[i] = env->addButton(Scale(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		stHostPrepDuelist[i] = env->addStaticText(L"", Scale(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		chkHostPrepReady[i] = env->addCheckBox(false, Scale(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	gBot.window = env->addWindow(Scale(750, 120, 960, 360), false, globalHandlers->dataManager->GetSysString(2051).c_str());
	defaultStrings.emplace_back(gBot.window, 2051);
	gBot.window->getCloseButton()->setVisible(false);
	gBot.window->setVisible(false);
	gBot.deckProperties = env->addStaticText(L"", Scale(10, 25, 200, 100), true, true, gBot.window);
	gBot.chkThrowRock = env->addCheckBox(false, Scale(10, 105, 200, 130), gBot.window, -1, globalHandlers->dataManager->GetSysString(2052).c_str());
	defaultStrings.emplace_back(gBot.chkThrowRock, 2052);
	gBot.chkMute = env->addCheckBox(false, Scale(10, 135, 200, 160), gBot.window, -1, globalHandlers->dataManager->GetSysString(2053).c_str());
	defaultStrings.emplace_back(gBot.chkMute, 2053);
	gBot.deckBox = env->addComboBox(Scale(10, 165, 200, 190), gBot.window, COMBOBOX_BOT_DECK);
	gBot.btnAdd = env->addButton(Scale(10, 200, 200, 225), gBot.window, BUTTON_BOT_ADD, globalHandlers->dataManager->GetSysString(2054).c_str());
	defaultStrings.emplace_back(gBot.btnAdd, 2054);
	btnHostPrepOB = env->addButton(Scale(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, globalHandlers->dataManager->GetSysString(1252).c_str());
	defaultStrings.emplace_back(btnHostPrepOB, 1252);
	stHostPrepOB = env->addStaticText(fmt::format(L"{} 0", globalHandlers->dataManager->GetSysString(1253)).c_str(), Scale(10, 210, 270, 230), false, false, wHostPrepare);
	defaultStrings.emplace_back(stHostPrepOB, 1253);
	stHostPrepRule = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare, -1, Scale(280, 30, 460, 230));
	stHostPrepRule->setWordWrap(true);
	stDeckSelect = env->addStaticText(globalHandlers->dataManager->GetSysString(1254).c_str(), Scale(10, 235, 110, 255), false, false, wHostPrepare);
	defaultStrings.emplace_back(stDeckSelect, 1254);
	cbDeckSelect = env->addComboBox(Scale(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	cbDeckSelect2 = env->addComboBox(Scale(280, 230, 430, 255), wHostPrepare);
	cbDeckSelect2->setMaxSelectionRows(10);
	btnHostPrepReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_READY, globalHandlers->dataManager->GetSysString(1218).c_str());
	defaultStrings.emplace_back(btnHostPrepReady, 1218);
	btnHostPrepNotReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_NOTREADY, globalHandlers->dataManager->GetSysString(1219).c_str());
	defaultStrings.emplace_back(btnHostPrepNotReady, 1219);
	btnHostPrepNotReady->setVisible(false);
	btnHostPrepStart = env->addButton(Scale(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, globalHandlers->dataManager->GetSysString(1215).c_str());
	defaultStrings.emplace_back(btnHostPrepStart, 1215);
	btnHostPrepCancel = env->addButton(Scale(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, globalHandlers->dataManager->GetSysString(1210).c_str());
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
	irr::gui::IGUITab* tabInfo = wInfos->addTab(globalHandlers->dataManager->GetSysString(1270).c_str());
	defaultStrings.emplace_back(tabInfo, 1270);
	stName = irr::gui::CGUICustomText::addCustomText(L"", true, env, tabInfo, -1, Scale(10, 10, 287, 32));
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	((CGUICustomText*)stName)->setTextAutoScrolling(irr::gui::CGUICustomText::LEFT_TO_RIGHT_BOUNCING, 0, 1.0f, 0, 120, 300);
	stInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 37, 287, 60));
	stInfo->setWordWrap(true);
	stInfo->setOverrideColor(skin::CARDINFO_TYPES_COLOR_VAL);
	stDataInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 60, 287, 83));
	stDataInfo->setWordWrap(true);
	stDataInfo->setOverrideColor(skin::CARDINFO_STATS_COLOR_VAL);
	stSetName = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 83, 287, 106));
	stSetName->setWordWrap(true);
	stSetName->setOverrideColor(skin::CARDINFO_ARCHETYPE_TEXT_COLOR_VAL);
	stSetName->setVisible(!globalHandlers->configs->chkHideSetname);
	stPasscodeScope = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 106, 287, 129));
	stPasscodeScope->setWordWrap(true);
	stPasscodeScope->setOverrideColor(skin::CARDINFO_PASSCODE_SCOPE_TEXT_COLOR_VAL);
	stPasscodeScope->setVisible(!globalHandlers->configs->hidePasscodeScope);
	stText = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 129, 287, 324));
	((CGUICustomText*)stText)->enableScrollBar();
	stText->setWordWrap(true);
	//log
	tabLog =  wInfos->addTab(globalHandlers->dataManager->GetSysString(1271).c_str());
	defaultStrings.emplace_back(tabLog, 1271);
	lstLog = env->addListBox(Scale(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(Scale(18));
	btnClearLog = env->addButton(Scale(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, globalHandlers->dataManager->GetSysString(1272).c_str());
	defaultStrings.emplace_back(btnClearLog, 1272);
	btnExpandLog = env->addButton(Scale(40, 300, 140, 325), tabLog, BUTTON_EXPAND_INFOBOX, globalHandlers->dataManager->GetSysString(2043).c_str());
	defaultStrings.emplace_back(btnExpandLog, 2043);
	//chat
	tabChat = wInfos->addTab(globalHandlers->dataManager->GetSysString(1279).c_str());
	defaultStrings.emplace_back(tabChat, 1279);
	lstChat = env->addListBox(Scale(10, 10, 290, 290), tabChat, -1, false);
	lstChat->setItemHeight(Scale(18));
	btnClearChat = env->addButton(Scale(160, 300, 260, 325), tabChat, BUTTON_CLEAR_CHAT, globalHandlers->dataManager->GetSysString(1282).c_str());
	defaultStrings.emplace_back(btnClearChat, 1282);
	btnExpandChat = env->addButton(Scale(40, 300, 140, 325), tabChat, BUTTON_EXPAND_INFOBOX, globalHandlers->dataManager->GetSysString(2043).c_str());
	defaultStrings.emplace_back(btnExpandChat, 2043);
	//system
	irr::gui::IGUITab* _tabSystem = wInfos->addTab(globalHandlers->dataManager->GetSysString(1273).c_str());
	defaultStrings.emplace_back(_tabSystem, 1273);
	tabSystem = Panel::addPanel(env, _tabSystem, -1, Scale(0, 0, wInfos->getRelativePosition().getWidth() + 1, wInfos->getRelativePosition().getHeight()), true, false);
	auto tabPanel = tabSystem->getSubpanel();
	tabSettings.chkIgnoreOpponents = env->addCheckBox(globalHandlers->configs->chkIgnore1, Scale(20, 20, 280, 45), tabPanel, -1, globalHandlers->dataManager->GetSysString(1290).c_str());
	defaultStrings.emplace_back(tabSettings.chkIgnoreOpponents, 1290);
	tabSettings.chkIgnoreSpectators = env->addCheckBox(globalHandlers->configs->chkIgnore2, Scale(20, 50, 280, 75), tabPanel, -1, globalHandlers->dataManager->GetSysString(1291).c_str());
	defaultStrings.emplace_back(tabSettings.chkIgnoreSpectators, 1291);
	tabSettings.chkQuickAnimation = env->addCheckBox(globalHandlers->configs->quick_animation, Scale(20, 80, 300, 105), tabPanel, CHECKBOX_QUICK_ANIMATION, globalHandlers->dataManager->GetSysString(1299).c_str());
	defaultStrings.emplace_back(tabSettings.chkQuickAnimation, 1299);
	tabSettings.chkHideChainButtons = env->addCheckBox(globalHandlers->configs->chkHideHintButton, Scale(20, 110, 280, 135), tabPanel, CHECKBOX_CHAIN_BUTTONS, globalHandlers->dataManager->GetSysString(1355).c_str());
	defaultStrings.emplace_back(tabSettings.chkHideChainButtons, 1355);
	tabSettings.chkAutoChainOrder = env->addCheckBox(globalHandlers->configs->chkAutoChain, Scale(20, 140, 280, 165), tabPanel, -1, globalHandlers->dataManager->GetSysString(1276).c_str());
	defaultStrings.emplace_back(tabSettings.chkAutoChainOrder, 1276);
	tabSettings.chkNoChainDelay = env->addCheckBox(globalHandlers->configs->chkWaitChain, Scale(20, 170, 280, 195), tabPanel, -1, globalHandlers->dataManager->GetSysString(1277).c_str());
	defaultStrings.emplace_back(tabSettings.chkNoChainDelay, 1277);
	tabSettings.chkEnableSound = env->addCheckBox(globalHandlers->configs->enablesound, Scale(20, 200, 280, 225), tabPanel, CHECKBOX_ENABLE_SOUND, globalHandlers->dataManager->GetSysString(2047).c_str());
	defaultStrings.emplace_back(tabSettings.chkEnableSound, 2047);
	tabSettings.stSoundVolume = env->addStaticText(globalHandlers->dataManager->GetSysString(2049).c_str(), Scale(20, 230, 80, 255), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stSoundVolume, 2049);
	tabSettings.scrSoundVolume = env->addScrollBar(true, Scale(85, 235, 280, 250), tabPanel, SCROLL_SOUND_VOLUME);
	tabSettings.scrSoundVolume->setMax(100);
	tabSettings.scrSoundVolume->setMin(0);
	tabSettings.scrSoundVolume->setPos(globalHandlers->configs->soundVolume);
	tabSettings.scrSoundVolume->setLargeStep(1);
	tabSettings.scrSoundVolume->setSmallStep(1);
	tabSettings.chkEnableMusic = env->addCheckBox(globalHandlers->configs->enablemusic, Scale(20, 260, 280, 285), tabPanel, CHECKBOX_ENABLE_MUSIC, globalHandlers->dataManager->GetSysString(2046).c_str());
	defaultStrings.emplace_back(tabSettings.chkEnableMusic, 2046);
	tabSettings.stMusicVolume = env->addStaticText(globalHandlers->dataManager->GetSysString(2048).c_str(), Scale(20, 290, 80, 315), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stMusicVolume, 2048);
	tabSettings.scrMusicVolume = env->addScrollBar(true, Scale(85, 295, 280, 310), tabPanel, SCROLL_MUSIC_VOLUME);
	tabSettings.scrMusicVolume->setMax(100);
	tabSettings.scrMusicVolume->setMin(0);
	tabSettings.scrMusicVolume->setPos(globalHandlers->configs->musicVolume);
	tabSettings.scrMusicVolume->setLargeStep(1);
	tabSettings.scrMusicVolume->setSmallStep(1);
	tabSettings.stNoAudioBackend = env->addStaticText(globalHandlers->dataManager->GetSysString(2058).c_str(), Scale(20, 200, 280, 315), false, true, tabPanel);
	defaultStrings.emplace_back(tabSettings.stNoAudioBackend, 2058);
	tabSettings.stNoAudioBackend->setVisible(false);
	tabSettings.chkMAutoPos = env->addCheckBox(globalHandlers->configs->chkMAutoPos, Scale(20, 320, 280, 345), tabPanel, -1, globalHandlers->dataManager->GetSysString(1274).c_str());
	defaultStrings.emplace_back(tabSettings.chkMAutoPos, 1274);
	tabSettings.chkSTAutoPos = env->addCheckBox(globalHandlers->configs->chkSTAutoPos, Scale(20, 350, 280, 375), tabPanel, -1, globalHandlers->dataManager->GetSysString(1278).c_str());
	defaultStrings.emplace_back(tabSettings.chkSTAutoPos, 1278);
	tabSettings.chkRandomPos = env->addCheckBox(globalHandlers->configs->chkRandomPos, Scale(40, 380, 280, 405), tabPanel, -1, globalHandlers->dataManager->GetSysString(1275).c_str());
	defaultStrings.emplace_back(tabSettings.chkRandomPos, 1275);
	btnTabShowSettings = env->addButton(Scale(20, 410, 280, 435), tabPanel, BUTTON_SHOW_SETTINGS, globalHandlers->dataManager->GetSysString(2059).c_str());
	defaultStrings.emplace_back(btnTabShowSettings, 2059);
	/* padding = */ env->addStaticText(L"", Scale(20, 440, 280, 450), false, true, tabPanel, -1, false);

	gSettings.window = env->addWindow(Scale(220, 100, 800, 520), false, globalHandlers->dataManager->GetSysString(1273).c_str());
	defaultStrings.emplace_back(gSettings.window, 1273);
	gSettings.window->getCloseButton()->setVisible(false);
	gSettings.window->setVisible(false);
	gSettings.btnClose = env->addButton(Scale(555, 5, 575, 25), gSettings.window, BUTTON_HIDE_SETTINGS, L"X");
	gSettings.chkShowFPS = env->addCheckBox(globalHandlers->configs->showFPS, Scale(20, 35, 280, 60), gSettings.window, CHECKBOX_SHOW_FPS, globalHandlers->dataManager->GetSysString(1445).c_str());
	defaultStrings.emplace_back(gSettings.chkShowFPS, 1445);
	gSettings.chkFullscreen = env->addCheckBox(globalHandlers->configs->fullscreen, Scale(20, 65, 280, 90), gSettings.window, CHECKBOX_FULLSCREEN, globalHandlers->dataManager->GetSysString(2060).c_str());
	defaultStrings.emplace_back(gSettings.chkFullscreen, 2060);
#ifdef __ANDROID__
	gSettings.chkFullscreen->setChecked(true);
	gSettings.chkFullscreen->setEnabled(false);
#elif defined(__APPLE__)
	gSettings.chkFullscreen->setEnabled(false);
#endif
	gSettings.chkScaleBackground = env->addCheckBox(globalHandlers->configs->scale_background, Scale(20, 95, 280, 120), gSettings.window, CHECKBOX_SCALE_BACKGROUND, globalHandlers->dataManager->GetSysString(2061).c_str());
	defaultStrings.emplace_back(gSettings.chkScaleBackground, 2061);
	gSettings.chkAccurateBackgroundResize = env->addCheckBox(globalHandlers->configs->accurate_bg_resize, Scale(20, 125, 280, 150), gSettings.window, CHECKBOX_ACCURATE_BACKGROUND_RESIZE, globalHandlers->dataManager->GetSysString(2062).c_str());
	defaultStrings.emplace_back(gSettings.chkAccurateBackgroundResize, 2062);
#ifdef __ANDROID__
	gSettings.chkAccurateBackgroundResize->setChecked(true);
	gSettings.chkAccurateBackgroundResize->setEnabled(false);
#endif
	gSettings.chkHideSetname = env->addCheckBox(globalHandlers->configs->chkHideSetname, Scale(20, 155, 280, 180), gSettings.window, CHECKBOX_HIDE_ARCHETYPES, globalHandlers->dataManager->GetSysString(1354).c_str());
	defaultStrings.emplace_back(gSettings.chkHideSetname, 1354);
	gSettings.chkHidePasscodeScope = env->addCheckBox(globalHandlers->configs->hidePasscodeScope, Scale(20, 185, 280, 210), gSettings.window, CHECKBOX_HIDE_PASSCODE_SCOPE, globalHandlers->dataManager->GetSysString(2063).c_str());
	defaultStrings.emplace_back(gSettings.chkHidePasscodeScope, 2063);
	gSettings.chkDrawFieldSpells = env->addCheckBox(globalHandlers->configs->chkHideSetname, Scale(20, 215, 280, 240), gSettings.window, CHECKBOX_DRAW_FIELD_SPELLS, dataManager.GetSysString(2068).c_str());
	defaultStrings.emplace_back(gSettings.chkDrawFieldSpells, 2068);
	gSettings.chkFilterBot = env->addCheckBox(globalHandlers->configs->filterBot, Scale(20, 245, 280, 270), gSettings.window, CHECKBOX_FILTER_BOT, dataManager.GetSysString(2069).c_str());
	defaultStrings.emplace_back(gSettings.chkFilterBot, 2069);
	gSettings.stCurrentSkin = env->addStaticText(globalHandlers->dataManager->GetSysString(2064).c_str(), Scale(20, 275, 80, 300), false, true, gSettings.window);
	defaultStrings.emplace_back(gSettings.stCurrentSkin, 2064);
	gSettings.cbCurrentSkin = env->addComboBox(Scale(85, 275, 280, 300), gSettings.window, COMBOBOX_CURRENT_SKIN);
	int selectedSkin = gSettings.cbCurrentSkin->addItem(globalHandlers->dataManager->GetSysString(2065).c_str()); // NoSkinLabel "none"
	auto skins = skinSystem->listSkins();
	for (int i = skins.size() - 1; i >= 0; i--) {
		auto itemIndex = gSettings.cbCurrentSkin->addItem(Utils::ToUnicodeIfNeeded(skins[i].c_str()).c_str());
		if (globalHandlers->configs->skin == skins[i].c_str()) {
			selectedSkin = itemIndex;
		}
	}
	gSettings.cbCurrentSkin->setSelected(selectedSkin);
	gSettings.btnReloadSkin = env->addButton(Scale(20, 305, 280, 330), gSettings.window, BUTTON_RELOAD_SKIN, globalHandlers->dataManager->GetSysString(2066).c_str());
	defaultStrings.emplace_back(gSettings.btnReloadSkin, 2066);
	gSettings.stCurrentLocale = env->addStaticText(globalHandlers->dataManager->GetSysString(2067).c_str(), Scale(20, 335, 80, 360), false, true, gSettings.window);
	defaultStrings.emplace_back(gSettings.stCurrentLocale, 2067);
	PopulateLocales();
	gSettings.cbCurrentLocale = env->addComboBox(Scale(85, 335, 280, 360), gSettings.window, COMBOBOX_CURRENT_LOCALE);
	int selectedLocale = gSettings.cbCurrentLocale->addItem(L"English");
	for(auto& locale : locales) {
		auto itemIndex = gSettings.cbCurrentLocale->addItem(Utils::ToUnicodeIfNeeded(locale).c_str());
		if(globalHandlers->configs->locale == locale) {
			selectedLocale = itemIndex;
		}
	}
	gSettings.cbCurrentLocale->setSelected(selectedLocale);
	//log
	tabRepositories = wInfos->addTab(globalHandlers->dataManager->GetSysString(2045).c_str());
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
		btnHand[i] = irr::gui::CGUIImageButton::addImageButton(env, Scale(10 + 105 * i, 10, 105 + 105 * i, 144), wHand, BUTTON_HAND1 + i);
		auto a = Scale<s32>(10 + 105 * i, 10, 105 + 105 * i, 144).getSize();
		btnHand[i]->setImageSize(a);
		btnHand[i]->setImage(imageManager.tHand[i]);
	}
	//
	wFTSelect = env->addWindow(Scale(550, 240, 780, 340), false, L"");
	wFTSelect->getCloseButton()->setVisible(false);
	wFTSelect->setVisible(false);
	btnFirst = env->addButton(Scale(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, globalHandlers->dataManager->GetSysString(100).c_str());
	defaultStrings.emplace_back(btnFirst, 100);
	btnSecond = env->addButton(Scale(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, globalHandlers->dataManager->GetSysString(101).c_str());
	defaultStrings.emplace_back(btnSecond, 101);
	//message (310)
	wMessage = env->addWindow(Scale(510 - 175, 200, 510 + 175, 340), false, globalHandlers->dataManager->GetSysString(1216).c_str());
	defaultStrings.emplace_back(wMessage, 1216);
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wMessage, -1, Scale(20, 20, 350, 100));
	stMessage->setWordWrap(true);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(Scale(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnMsgOK, 1211);
	//auto fade message (310)
	wACMessage = env->addWindow(Scale(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = irr::gui::CGUICustomText::addCustomText(L"", true, env, wACMessage, -1, Scale(0, 0, 350, 60), true);
	stACMessage->setWordWrap(true);
	stACMessage->setBackgroundColor(skin::DUELFIELD_ANNOUNCE_TEXT_BACKGROUND_COLOR_VAL);
	auto tmp_color = skin::DUELFIELD_ANNOUNCE_TEXT_COLOR_VAL;
	if(tmp_color != 0)
		stACMessage->setOverrideColor(tmp_color);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(Scale(490, 200, 840, 340), false, globalHandlers->dataManager->GetSysString(560).c_str());
	defaultStrings.emplace_back(wQuery, 560);
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wQuery, -1, Scale(20, 20, 350, 100));
	stQMessage->setWordWrap(true);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(Scale(100, 105, 150, 130), wQuery, BUTTON_YES, globalHandlers->dataManager->GetSysString(1213).c_str());
	defaultStrings.emplace_back(btnYes, 1213);
	btnNo = env->addButton(Scale(200, 105, 250, 130), wQuery, BUTTON_NO, globalHandlers->dataManager->GetSysString(1214).c_str());
	defaultStrings.emplace_back(btnNo, 1214);
	//options (310)
	wOptions = env->addWindow(Scale(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions = irr::gui::CGUICustomText::addCustomText(L"", false, env, wOptions, -1, Scale(20, 20, 350, 100));
	stOptions->setWordWrap(true);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(Scale(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnOptionOK, 1211);
	btnOptionp = env->addButton(Scale(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, globalHandlers->dataManager->GetSysString(1432).c_str());
	defaultStrings.emplace_back(btnOptionp, 1432);
	btnOptionn = env->addButton(Scale(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, globalHandlers->dataManager->GetSysString(1433).c_str());
	defaultStrings.emplace_back(btnOptionn, 1433);
	for(int i = 0; i < 5; ++i) {
		btnOption[i] = env->addButton(Scale(10, 30 + 40 * i, 340, 60 + 40 * i), wOptions, BUTTON_OPTION_0 + i, L"");
	}
	scrOption = env->addScrollBar(false, Scale(350, 30, 365, 220), wOptions, SCROLL_OPTION_SELECT);
	scrOption->setLargeStep(1);
	scrOption->setSmallStep(1);
	scrOption->setMin(0);
	//pos select
	wPosSelect = env->addWindow(Scale(340, 200, 935, 410), false, globalHandlers->dataManager->GetSysString(561).c_str());
	defaultStrings.emplace_back(wPosSelect, 561);
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	core::dimension2di imgsize = { Scale<s32>(CARD_IMG_WIDTH * 0.5f), Scale<s32>(CARD_IMG_HEIGHT * 0.5f) };
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
	imgsize = { Scale<s32>(CARD_IMG_WIDTH * 0.6f), Scale<s32>(CARD_IMG_HEIGHT * 0.6f) };
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
	btnSelectOK = env->addButton(Scale(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
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
	btnDisplayOK = env->addButton(Scale(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnDisplayOK, 1211);
	//announce number
	wANNumber = env->addWindow(Scale(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(Scale(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(Scale(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnANNumberOK, 1211);
	//announce card
	wANCard = env->addWindow(Scale(430, 170, 840, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", Scale(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(Scale(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(Scale(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnANCardOK, 1211);
	//announce attribute
	wANAttribute = env->addWindow(Scale(500, 200, 830, 285), false, globalHandlers->dataManager->GetSysString(562).c_str());
	defaultStrings.emplace_back(wANAttribute, 562);
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
										   wANAttribute, CHECK_ATTRIBUTE, globalHandlers->dataManager->FormatAttribute(filter).c_str());
	//announce race
	wANRace = env->addWindow(Scale(480, 200, 850, 410), false, globalHandlers->dataManager->GetSysString(563).c_str());
	defaultStrings.emplace_back(wANRace, 563);
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 25; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
									  wANRace, CHECK_RACE, globalHandlers->dataManager->FormatRace(filter).c_str());
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
	btnActivate = env->addButton(Scale(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, globalHandlers->dataManager->GetSysString(1150).c_str());
	defaultStrings.emplace_back(btnActivate, 1150);
	btnSummon = env->addButton(Scale(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, globalHandlers->dataManager->GetSysString(1151).c_str());
	defaultStrings.emplace_back(btnSummon, 1151);
	btnSPSummon = env->addButton(Scale(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, globalHandlers->dataManager->GetSysString(1152).c_str());
	defaultStrings.emplace_back(btnSPSummon, 1152);
	btnMSet = env->addButton(Scale(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, globalHandlers->dataManager->GetSysString(1153).c_str());
	defaultStrings.emplace_back(btnMSet, 1153);
	btnSSet = env->addButton(Scale(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, globalHandlers->dataManager->GetSysString(1153).c_str());
	defaultStrings.emplace_back(btnSSet, 1153);
	btnRepos = env->addButton(Scale(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, globalHandlers->dataManager->GetSysString(1154).c_str());
	defaultStrings.emplace_back(btnRepos, 1154);
	btnAttack = env->addButton(Scale(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, globalHandlers->dataManager->GetSysString(1157).c_str());
	defaultStrings.emplace_back(btnAttack, 1157);
	btnShowList = env->addButton(Scale(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, globalHandlers->dataManager->GetSysString(1158).c_str());
	defaultStrings.emplace_back(btnShowList, 1158);
	btnOperation = env->addButton(Scale(1, 169, 99, 189), wCmdMenu, BUTTON_CMD_ACTIVATE, globalHandlers->dataManager->GetSysString(1161).c_str());
	defaultStrings.emplace_back(btnOperation, 1161);
	btnReset = env->addButton(Scale(1, 190, 99, 210), wCmdMenu, BUTTON_CMD_RESET, globalHandlers->dataManager->GetSysString(1162).c_str());
	defaultStrings.emplace_back(btnReset, 1162);
	//deck edit
	wDeckEdit = env->addStaticText(L"", Scale(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	stBanlist = env->addStaticText(globalHandlers->dataManager->GetSysString(1300).c_str(), Scale(10, 9, 100, 29), false, false, wDeckEdit);
	defaultStrings.emplace_back(stBanlist, 1300);
	cbDBLFList = env->addComboBox(Scale(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	stDeck = env->addStaticText(globalHandlers->dataManager->GetSysString(1301).c_str(), Scale(10, 39, 100, 59), false, false, wDeckEdit);
	defaultStrings.emplace_back(stDeck, 1301);
	cbDBDecks = env->addComboBox(Scale(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);
	RefreshLFLists();
	btnSaveDeck = env->addButton(Scale(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, globalHandlers->dataManager->GetSysString(1302).c_str());
	defaultStrings.emplace_back(btnSaveDeck, 1302);
	ebDeckname = env->addEditBox(L"", Scale(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECK_NAME);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(Scale(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, globalHandlers->dataManager->GetSysString(1303).c_str());
	defaultStrings.emplace_back(btnSaveDeckAs, 1303);
	btnDeleteDeck = env->addButton(Scale(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, globalHandlers->dataManager->GetSysString(1308).c_str());
	defaultStrings.emplace_back(btnDeleteDeck, 1308);
	btnShuffleDeck = env->addButton(Scale(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, globalHandlers->dataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnShuffleDeck, 1307);
	btnSortDeck = env->addButton(Scale(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, globalHandlers->dataManager->GetSysString(1305).c_str());
	defaultStrings.emplace_back(btnSortDeck, 1305);
	btnClearDeck = env->addButton(Scale(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, globalHandlers->dataManager->GetSysString(1304).c_str());
	defaultStrings.emplace_back(btnClearDeck, 1304);
	btnRenameDeck = env->addButton(Scale(170, 99, 220, 120), wDeckEdit, BUTTON_RENAME_DECK, globalHandlers->dataManager->GetSysString(1362).c_str());
	defaultStrings.emplace_back(btnRenameDeck, 1362);
	btnSideOK = env->addButton(Scale(510, 40, 820, 80), 0, BUTTON_SIDE_OK, globalHandlers->dataManager->GetSysString(1334).c_str());
	defaultStrings.emplace_back(btnSideOK, 1334);
	btnSideOK->setVisible(false);
	btnSideShuffle = env->addButton(Scale(310, 100, 370, 130), 0, BUTTON_SHUFFLE_DECK, globalHandlers->dataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnSideShuffle, 1307);
	btnSideShuffle->setVisible(false);
	btnSideSort = env->addButton(Scale(375, 100, 435, 130), 0, BUTTON_SORT_DECK, globalHandlers->dataManager->GetSysString(1305).c_str());
	defaultStrings.emplace_back(btnSideSort, 1305);
	btnSideSort->setVisible(false);
	btnSideReload = env->addButton(Scale(440, 100, 500, 130), 0, BUTTON_SIDE_RELOAD, globalHandlers->dataManager->GetSysString(1309).c_str());
	defaultStrings.emplace_back(btnSideReload, 1309);
	btnSideReload->setVisible(false);
	btnHandTest = env->addButton(Scale(205, 90, 295, 130), 0, BUTTON_HAND_TEST, globalHandlers->dataManager->GetSysString(1297).c_str());
	defaultStrings.emplace_back(btnHandTest, 1297);
	btnHandTest->setVisible(false);
	btnHandTest->setEnabled(coreloaded);
	//
	scrFilter = env->addScrollBar(false, Scale(999, 161, 1019, 629), 0, SCROLL_FILTER);
	scrFilter->setLargeStep(100);
	scrFilter->setSmallStep(10);
	scrFilter->setVisible(false);
	//sort type
	wSort = env->addStaticText(L"", Scale(930, 132, 1020, 156), true, false, 0, -1, true);
	cbSortType = env->addComboBox(Scale(10, 2, 85, 22), wSort, COMBOBOX_SORTTYPE);
	cbSortType->setMaxSelectionRows(10);
	for(int i = 1370; i <= 1373; i++)
		cbSortType->addItem(globalHandlers->dataManager->GetSysString(i).c_str());
	wSort->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", Scale(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	stCategory = env->addStaticText(globalHandlers->dataManager->GetSysString(1311).c_str(), Scale(10, 5, 70, 25), false, false, wFilter);
	defaultStrings.emplace_back(stCategory, 1311);
	cbCardType = env->addComboBox(Scale(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1310).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1312).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1313).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1314).c_str());
	cbCardType2 = env->addComboBox(Scale(130, 3, 190, 23), wFilter, COMBOBOX_SECONDTYPE);
	cbCardType2->setMaxSelectionRows(20);
	cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
	chkAnime = env->addCheckBox(globalHandlers->configs->chkAnime, Scale(10, 96, 150, 118), wFilter, CHECKBOX_SHOW_ANIME, globalHandlers->dataManager->GetSysString(1999).c_str());
	defaultStrings.emplace_back(chkAnime, 1999);
	stLimit = env->addStaticText(globalHandlers->dataManager->GetSysString(1315).c_str(), Scale(205, 5, 280, 25), false, false, wFilter);
	defaultStrings.emplace_back(stLimit, 1315);
	cbLimit = env->addComboBox(Scale(260, 3, 390, 23), wFilter, COMBOBOX_OTHER_FILT);
	cbLimit->setMaxSelectionRows(10);
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1310).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1316).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1317).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1318).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1320).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1900).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1901).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1902).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1903).c_str());
	if(chkAnime->isChecked()) {
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1265).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1266).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1267).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1268).c_str());
	}
	stAttribute = env->addStaticText(globalHandlers->dataManager->GetSysString(1319).c_str(), Scale(10, 28, 70, 48), false, false, wFilter);
	defaultStrings.emplace_back(stAttribute, 1319);
	cbAttribute = env->addComboBox(Scale(60, 26, 190, 46), wFilter, COMBOBOX_OTHER_FILT);
	cbAttribute->setMaxSelectionRows(10);
	cbAttribute->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(globalHandlers->dataManager->FormatAttribute(filter).c_str(), filter);
	stRace = env->addStaticText(globalHandlers->dataManager->GetSysString(1321).c_str(), Scale(10, 51, 70, 71), false, false, wFilter);
	cbRace = env->addComboBox(Scale(60, 49, 190, 69), wFilter, COMBOBOX_OTHER_FILT);
	cbRace->setMaxSelectionRows(10);
	cbRace->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(globalHandlers->dataManager->FormatRace(filter).c_str(), filter);
	stAttack = env->addStaticText(globalHandlers->dataManager->GetSysString(1322).c_str(), Scale(205, 28, 280, 48), false, false, wFilter);
	defaultStrings.emplace_back(stAttack, 1322);
	ebAttack = env->addEditBox(L"", Scale(260, 26, 340, 46), true, wFilter, EDITBOX_ATTACK);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stDefense = env->addStaticText(globalHandlers->dataManager->GetSysString(1323).c_str(), Scale(205, 51, 280, 71), false, false, wFilter);
	defaultStrings.emplace_back(stDefense, 1323);
	ebDefense = env->addEditBox(L"", Scale(260, 49, 340, 69), true, wFilter, EDITBOX_DEFENSE);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stStar = env->addStaticText(globalHandlers->dataManager->GetSysString(1324).c_str(), Scale(10, 74, 80, 94), false, false, wFilter);
	defaultStrings.emplace_back(stStar, 1324);
	ebStar = env->addEditBox(L"", Scale(60, 72, 100, 92), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER, EDITBOX_STAR);
	stScale = env->addStaticText(globalHandlers->dataManager->GetSysString(1336).c_str(), Scale(110, 74, 150, 94), false, false, wFilter);
	defaultStrings.emplace_back(stScale, 1336);
	ebScale = env->addEditBox(L"", Scale(150, 72, 190, 92), true, wFilter, EDITBOX_SCALE);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stSearch = env->addStaticText(globalHandlers->dataManager->GetSysString(1325).c_str(), Scale(205, 74, 280, 94), false, false, wFilter);
	defaultStrings.emplace_back(stSearch, 1325);
	ebCardName = env->addEditBox(L"", Scale(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(Scale(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, globalHandlers->dataManager->GetSysString(1326).c_str());
	defaultStrings.emplace_back(btnEffectFilter, 1326);
	btnStartFilter = env->addButton(Scale(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, globalHandlers->dataManager->GetSysString(1327).c_str());
	defaultStrings.emplace_back(btnStartFilter, 1327);
	btnClearFilter = env->addButton(Scale(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, globalHandlers->dataManager->GetSysString(1304).c_str());
	defaultStrings.emplace_back(btnClearFilter, 1304);
	wCategories = env->addWindow(Scale(450, 60, 1000, 270), false, L"");
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(Scale(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnCategoryOK, 1211);
	for(int i = 0; i < 32; ++i) {
		chkCategory[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, globalHandlers->dataManager->GetSysString(1100 + i).c_str());
		defaultStrings.emplace_back(chkCategory[i], 1100 + i);
	}
	btnMarksFilter = env->addButton(Scale(155, 96, 240, 118), wFilter, BUTTON_MARKS_FILTER, globalHandlers->dataManager->GetSysString(1374).c_str());
	defaultStrings.emplace_back(btnMarksFilter, 1374);
	wLinkMarks = env->addWindow(Scale(700, 30, 820, 150), false, L"");
	wLinkMarks->getCloseButton()->setVisible(false);
	wLinkMarks->setDrawTitlebar(false);
	wLinkMarks->setDraggable(false);
	wLinkMarks->setVisible(false);
	btnMarksOK = env->addButton(Scale(45, 45, 75, 75), wLinkMarks, BUTTON_MARKERS_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
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
	wReplay = env->addWindow(Scale(220, 100, 800, 520), false, globalHandlers->dataManager->GetSysString(1202).c_str());
	defaultStrings.emplace_back(wReplay, 1202);
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wReplay, LISTBOX_REPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstReplayList->setWorkingPath(L"./replay", true);
	lstReplayList->addFilteredExtensions(coreloaded ? std::vector<std::wstring>{L"yrp", L"yrpx"} : std::vector<std::wstring>{ L"yrpx" });
	lstReplayList->setItemHeight(Scale(18));
	btnLoadReplay = env->addButton(Scale(470, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, globalHandlers->dataManager->GetSysString(1348).c_str());
	defaultStrings.emplace_back(btnLoadReplay, 1348);
	btnLoadReplay->setEnabled(false);
	btnDeleteReplay = env->addButton(Scale(360, 355, 460, 380), wReplay, BUTTON_DELETE_REPLAY, globalHandlers->dataManager->GetSysString(1361).c_str());
	defaultStrings.emplace_back(btnDeleteReplay, 1361);
	btnDeleteReplay->setEnabled(false);
	btnRenameReplay = env->addButton(Scale(360, 385, 460, 410), wReplay, BUTTON_RENAME_REPLAY, globalHandlers->dataManager->GetSysString(1362).c_str());
	defaultStrings.emplace_back(btnRenameReplay, 1362);
	btnRenameReplay->setEnabled(false);
	btnReplayCancel = env->addButton(Scale(470, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, globalHandlers->dataManager->GetSysString(1347).c_str());
	defaultStrings.emplace_back(btnReplayCancel, 1347);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1349).c_str(), Scale(360, 30, 570, 50), false, true, wReplay);
	defaultStrings.emplace_back(tmpptr, 1349);
	stReplayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wReplay, -1, Scale(360, 60, 570, 350));
	stReplayInfo->setWordWrap(true);
	btnExportDeck = env->addButton(Scale(470, 325, 570, 350), wReplay, BUTTON_EXPORT_DECK, globalHandlers->dataManager->GetSysString(1358).c_str());
	defaultStrings.emplace_back(btnExportDeck, 1358);
	btnExportDeck->setEnabled(false);
	chkYrp = env->addCheckBox(false, Scale(360, 250, 560, 270), wReplay, -1, globalHandlers->dataManager->GetSysString(1356).c_str());
	defaultStrings.emplace_back(chkYrp, 1356);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1353).c_str(), Scale(360, 275, 570, 295), false, true, wReplay);
	defaultStrings.emplace_back(tmpptr, 1353);
	ebRepStartTurn = env->addEditBox(L"", Scale(360, 300, 460, 320), true, wReplay, -1);
	ebRepStartTurn->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//single play window
	wSinglePlay = env->addWindow(Scale(220, 100, 800, 520), false, globalHandlers->dataManager->GetSysString(1201).c_str());
	defaultStrings.emplace_back(wSinglePlay, 1201);
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wSinglePlay, LISTBOX_SINGLEPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstSinglePlayList->setItemHeight(Scale(18));
	lstSinglePlayList->setWorkingPath(L"./puzzles", true);
	lstSinglePlayList->addFilteredExtensions({L"lua"});
	btnLoadSinglePlay = env->addButton(Scale(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, globalHandlers->dataManager->GetSysString(1357).c_str());
	defaultStrings.emplace_back(btnLoadSinglePlay, 1357);
	btnLoadSinglePlay->setEnabled(false);
	btnSinglePlayCancel = env->addButton(Scale(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, globalHandlers->dataManager->GetSysString(1210).c_str());
	defaultStrings.emplace_back(btnSinglePlayCancel, 1210);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1352).c_str(), Scale(360, 30, 570, 50), false, true, wSinglePlay);
	defaultStrings.emplace_back(tmpptr, 1352);
	stSinglePlayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wSinglePlay, -1, Scale(360, 60, 570, 350));
	((CGUICustomText*)stSinglePlayInfo)->enableScrollBar();
	stSinglePlayInfo->setWordWrap(true);
	//replay save
	wReplaySave = env->addWindow(Scale(510, 200, 820, 320), false, globalHandlers->dataManager->GetSysString(1340).c_str());
	defaultStrings.emplace_back(wReplaySave, 1340);
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(1342).c_str(), Scale(20, 25, 290, 45), false, false, wReplaySave);
	defaultStrings.emplace_back(tmpptr, 1342);
	ebRSName =  env->addEditBox(L"", Scale(20, 50, 290, 70), true, wReplaySave, EDITBOX_REPLAY_NAME);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(Scale(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, globalHandlers->dataManager->GetSysString(1341).c_str());
	defaultStrings.emplace_back(btnRSYes, 1341);
	btnRSNo = env->addButton(Scale(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, globalHandlers->dataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnRSNo, 1212);
	//replay control
	wReplayControl = env->addStaticText(L"", Scale(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, globalHandlers->dataManager->GetSysString(1343).c_str());
	defaultStrings.emplace_back(btnReplayStart, 1343);
	btnReplayPause = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, globalHandlers->dataManager->GetSysString(1344).c_str());
	defaultStrings.emplace_back(btnReplayPause, 1344);
	btnReplayStep = env->addButton(Scale(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_STEP, globalHandlers->dataManager->GetSysString(1345).c_str());
	defaultStrings.emplace_back(btnReplayStep, 1345);
	btnReplayUndo = env->addButton(Scale(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_UNDO, globalHandlers->dataManager->GetSysString(1360).c_str());
	defaultStrings.emplace_back(btnReplayUndo, 1360);
	btnReplaySwap = env->addButton(Scale(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_SWAP, globalHandlers->dataManager->GetSysString(1346).c_str());
	defaultStrings.emplace_back(btnReplaySwap, 1346);
	btnReplayExit = env->addButton(Scale(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, globalHandlers->dataManager->GetSysString(1347).c_str());
	defaultStrings.emplace_back(btnReplayExit, 1347);
	//chat
	wChat = env->addWindow(Scale(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", Scale(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	ebChatInput->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	//swap
	btnSpectatorSwap = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_REPLAY_SWAP, globalHandlers->dataManager->GetSysString(1346).c_str());
	defaultStrings.emplace_back(btnSpectatorSwap, 1346);
	btnSpectatorSwap->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, globalHandlers->dataManager->GetSysString(1292).c_str());
	defaultStrings.emplace_back(btnChainIgnore, 1292);
	btnChainAlways = env->addButton(Scale(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, globalHandlers->dataManager->GetSysString(1293).c_str());
	defaultStrings.emplace_back(btnChainAlways, 1293);
	btnChainWhenAvail = env->addButton(Scale(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, globalHandlers->dataManager->GetSysString(1294).c_str());
	defaultStrings.emplace_back(btnChainWhenAvail, 1294);
	btnChainIgnore->setIsPushButton(true);
	btnChainAlways->setIsPushButton(true);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	//shuffle
	btnShuffle = env->addButton(Scale(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, globalHandlers->dataManager->GetSysString(1307).c_str());
	defaultStrings.emplace_back(btnShuffle, 1307);
	btnShuffle->setVisible(false);
	//cancel or finish
	btnCancelOrFinish = env->addButton(Scale(205, 230, 295, 265), 0, BUTTON_CANCEL_OR_FINISH, globalHandlers->dataManager->GetSysString(1295).c_str());
	defaultStrings.emplace_back(btnCancelOrFinish, 1295);
	btnCancelOrFinish->setVisible(false);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(Scale(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	//restart single
	btnRestartSingle = env->addButton(Scale(205, 90, 295, 165), 0, BUTTON_RESTART_SINGLE, globalHandlers->dataManager->GetSysString(1366).c_str());
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
	if(!globalHandlers->sounds->IsUsable()) {
		tabSettings.chkEnableSound->setVisible(false);
		tabSettings.stSoundVolume->setVisible(false);
		tabSettings.scrSoundVolume->setVisible(false);
		tabSettings.chkEnableMusic->setVisible(false);
		tabSettings.stMusicVolume->setVisible(false);
		tabSettings.scrMusicVolume->setVisible(false);
		tabSettings.stNoAudioBackend->setVisible(true);
	}

	//server lobby
	wRoomListPlaceholder = env->addStaticText(L"", Scale(1, 1, 1024 - 1, 640), false, false, 0, -1, false);
	//wRoomListPlaceholder->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	wRoomListPlaceholder->setVisible(false);

	auto roomlistcolor = skin::ROOMLIST_TEXTS_COLOR_VAL;

	//server choice dropdownlist
	irr::gui::IGUIStaticText* statictext = env->addStaticText(globalHandlers->dataManager->GetSysString(2041).c_str(), Scale(10, 30, 110, 50), false, false, wRoomListPlaceholder, -1, false); // 2041 = Server:
	defaultStrings.emplace_back(statictext, 2041);
	statictext->setOverrideColor(roomlistcolor);
	serverChoice = env->addComboBox(Scale(90, 25, 385, 50), wRoomListPlaceholder, SERVER_CHOICE);

	//online nickname
	statictext = env->addStaticText(globalHandlers->dataManager->GetSysString(1220).c_str(), Scale(10, 60, 110, 80), false, false, wRoomListPlaceholder, -1, false); // 1220 = Nickname:
	defaultStrings.emplace_back(statictext, 1220);
	statictext->setOverrideColor(roomlistcolor);
	ebNickNameOnline = env->addEditBox(globalHandlers->configs->nickname.c_str(), Scale(90, 55, 275, 80), true, wRoomListPlaceholder, EDITBOX_NICKNAME);

	//top right host online game button
	btnCreateHost2 = env->addButton(Scale(904, 25, 1014, 50), wRoomListPlaceholder, BUTTON_CREATE_HOST2, globalHandlers->dataManager->GetSysString(1224).c_str());
	defaultStrings.emplace_back(btnCreateHost2, 1224);
	btnCreateHost2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//filter dropdowns
	cbFilterRule = env->addComboBox(Scale(392, 25, 532, 50), wRoomListPlaceholder, CB_FILTER_ALLOWED_CARDS);
	//cbFilterMatchMode = env->addComboBox(Scale(392, 55, 532, 80), wRoomListPlaceholder, CB_FILTER_MATCH_MODE);
	cbFilterBanlist = env->addComboBox(Scale(392, 85, 532, 110), wRoomListPlaceholder, CB_FILTER_BANLIST);
	cbFilterRule->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	//cbFilterMatchMode->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	cbFilterBanlist->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	cbFilterRule->addItem(fmt::format(L"[{}]", globalHandlers->dataManager->GetSysString(1225)).c_str());
	for (auto i = 1900; i <= 1904; ++i)
		cbFilterRule->addItem(globalHandlers->dataManager->GetSysString(i).c_str());

	cbFilterBanlist->addItem(fmt::format(L"[{}]", globalHandlers->dataManager->GetSysString(1226)).c_str());
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbFilterBanlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);

	/*cbFilterMatchMode->addItem(fmt::format(L"[{}]", globalHandlers->dataManager->GetSysString(1227)).c_str());
	cbFilterMatchMode->addItem(globalHandlers->dataManager->GetSysString(1244).c_str());
	cbFilterMatchMode->addItem(globalHandlers->dataManager->GetSysString(1245).c_str());
	cbFilterMatchMode->addItem(globalHandlers->dataManager->GetSysString(1246).c_str());*/
	//Scale(392, 55, 532, 80)
	ebOnlineTeam1 = env->addEditBox(L"0", Scale(140 + (392 - 140), 55, 170 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam1->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	stVersus = env->addStaticText(globalHandlers->dataManager->GetSysString(1380).c_str(), Scale(175 + (392 - 140), 55, 195 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	defaultStrings.emplace_back(stVersus, 1380);
	stVersus->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stVersus->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	stVersus->setOverrideColor(roomlistcolor);
	ebOnlineTeam2 = env->addEditBox(L"0", Scale(200 + (392 - 140), 55, 230 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam2->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	stBestof = env->addStaticText(globalHandlers->dataManager->GetSysString(1381).c_str(), Scale(235 + (392 - 140), 55, 280 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	defaultStrings.emplace_back(stBestof, 1381);
	stBestof->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	stBestof->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	stBestof->setOverrideColor(roomlistcolor);
	ebOnlineBestOf = env->addEditBox(L"0", Scale(285 + (392 - 140), 55, 315 + (392 - 140), 80), true, wRoomListPlaceholder);
	ebOnlineBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineBestOf->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	btnFilterRelayMode = env->addButton(Scale(325 + (392 - 140), 55, 370 + (392 - 140), 80), wRoomListPlaceholder, -1, globalHandlers->dataManager->GetSysString(1247).c_str());
	defaultStrings.emplace_back(btnFilterRelayMode, 1247);
	btnFilterRelayMode->setIsPushButton(true);
	btnFilterRelayMode->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//filter rooms textbox
	ebRoomNameText = env->addStaticText(globalHandlers->dataManager->GetSysString(2021).c_str(), Scale(572, 30, 682, 50), false, false, wRoomListPlaceholder); //2021 = Filter:
	defaultStrings.emplace_back(ebRoomNameText, 2021);
	ebRoomNameText->setOverrideColor(roomlistcolor);
	ebRoomName = env->addEditBox(L"", Scale(642, 25, 782, 50), true, wRoomListPlaceholder, EDIT_ONLINE_ROOM_NAME); //filter textbox
	ebRoomName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebRoomNameText->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	ebRoomName->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show locked rooms checkbox
	chkShowPassword = CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 55, 800, 80), wRoomListPlaceholder, CHECK_SHOW_LOCKED_ROOMS, globalHandlers->dataManager->GetSysString(1994).c_str());
	defaultStrings.emplace_back(chkShowPassword, 1994);
	((CGUICustomCheckBox*)chkShowPassword)->setColor(roomlistcolor);
	chkShowPassword->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show active rooms checkbox
	chkShowActiveRooms = CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 85, 800, 110), wRoomListPlaceholder, CHECK_SHOW_ACTIVE_ROOMS, globalHandlers->dataManager->GetSysString(1985).c_str());
	defaultStrings.emplace_back(chkShowActiveRooms, 1985);
	((CGUICustomCheckBox*)chkShowActiveRooms)->setColor(roomlistcolor);
	chkShowActiveRooms->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show all rooms in a table
	roomListTable = CGUICustomTable::addCustomTable(env, Resize(1, 118, 1022, 550), wRoomListPlaceholder, TABLE_ROOMLIST, true);
	roomListTable->setResizableColumns(true);
	//roomListTable->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	roomListTable->addColumn(L" ");//lock
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(1225).c_str());//Allowed Cards:
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(1227).c_str());//Duel Mode:
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(1236).c_str());//master rule
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(1226).c_str());//Forbidden List:
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(2030).c_str());//Players:
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(2024).c_str());//Notes:
	roomListTable->addColumn(globalHandlers->dataManager->GetSysString(1988).c_str());//Status
	roomListTable->setColumnWidth(0, 30); //lock
	roomListTable->setColumnWidth(1, 110);//Allowed Cards:
	roomListTable->setColumnWidth(2, 100);//Duel Mode:
	roomListTable->setColumnWidth(3, 50);//master rule
	roomListTable->setColumnWidth(4, 135);//Forbidden List:
	roomListTable->setColumnWidth(5, 115);//Players:
	roomListTable->setColumnWidth(6, 405);//Notes:
	roomListTable->setColumnWidth(7, 140);//Status
	roomListTable->setColumnOrdering(0, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(1, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(2, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(3, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(4, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(5, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(6, EGCO_FLIP_ASCENDING_DESCENDING);
	roomListTable->setColumnOrdering(7, EGCO_FLIP_ASCENDING_DESCENDING);

	//refresh button center bottom
	btnLanRefresh2 = env->addButton(Scale(462, 640 - 10 - 25 - 25 - 5, 562, 640 - 10 - 25 - 5), wRoomListPlaceholder, BUTTON_LAN_REFRESH2, globalHandlers->dataManager->GetSysString(1217).c_str());
	defaultStrings.emplace_back(btnLanRefresh2, 1217);
	btnLanRefresh2->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

	//server room password
	wRoomPassword = env->addWindow(Scale(357, 200, 667, 320), false, L"");
	wRoomPassword->getCloseButton()->setVisible(false);
	wRoomPassword->setVisible(false);
	wRoomPassword->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER);
 	tmpptr = env->addStaticText(globalHandlers->dataManager->GetSysString(2038).c_str(), Scale(20, 25, 290, 45), false, false, wRoomPassword);
	defaultStrings.emplace_back(tmpptr, 2038);
	ebRPName = env->addEditBox(L"", Scale(20, 50, 290, 70), true, wRoomPassword, -1);
	ebRPName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRPYes = env->addButton(Scale(70, 80, 140, 105), wRoomPassword, BUTTON_ROOMPASSWORD_OK, globalHandlers->dataManager->GetSysString(1211).c_str());
	defaultStrings.emplace_back(btnRPYes, 1211);
	btnRPNo = env->addButton(Scale(170, 80, 240, 105), wRoomPassword, BUTTON_ROOMPASSWORD_CANCEL, globalHandlers->dataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnRPNo, 1212);

	//join cancel buttons
	btnJoinHost2 = env->addButton(Scale(1024 - 10 - 110, 640 - 10 - 25 - 25 - 5, 1024 - 10, 640 - 10 - 25 - 5), wRoomListPlaceholder, BUTTON_JOIN_HOST2, globalHandlers->dataManager->GetSysString(1223).c_str());
	defaultStrings.emplace_back(btnJoinHost2, 1223);
	btnJoinCancel2 = env->addButton(Scale(1024 - 10 - 110, 640 - 10 - 25, 1024 - 10, 640 - 10), wRoomListPlaceholder, BUTTON_JOIN_CANCEL2, globalHandlers->dataManager->GetSysString(1212).c_str());
	defaultStrings.emplace_back(btnJoinCancel2, 1212);
	btnJoinHost2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	btnJoinCancel2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


	//load server(s)
	LoadServers();

	env->getRootGUIElement()->bringToFront(mTopMenu);
	env->setFocus(wMainMenu);
#ifdef YGOPRO_BUILD_DLL
	if(!coreloaded) {
		stMessage->setText(globalHandlers->dataManager->GetSysString(1430).c_str());
		PopupElement(wMessage);
	}
#endif
	fpsCounter = env->addStaticText(L"", Scale(950, 620, 1024, 640), false, false);
	fpsCounter->setOverrideColor(skin::FPS_TEXT_COLOR_VAL);
	fpsCounter->setVisible(globalHandlers->configs->showFPS);
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
void BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar) {
	for(int i = 0; i < 16; ++i)
		mProjection[i] = 0;
	mProjection[0] = 2.0f * znear / (right - left);
	mProjection[5] = 2.0f * znear / (top - bottom);
	mProjection[8] = (left + right) / (left - right);
	mProjection[9] = (top + bottom) / (bottom - top);
	mProjection[10] = zfar / (zfar - znear);
	mProjection[11] = 1.0f;
	mProjection[14] = znear * zfar / (znear - zfar);
}
bool Game::MainLoop() {
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, CAMERA_LEFT, CAMERA_RIGHT, CAMERA_BOTTOM, CAMERA_TOP, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);
	
	mProjection.buildCameraLookAtMatrixLH(vector3df(FIELD_X, FIELD_Y, FIELD_Z), vector3df(FIELD_X, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(irr::video::SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
#if defined (__linux__) && !defined(__ANDROID__)
	bool last_resize = false;
	dimension2du prev_window_size;
#endif
	irr::ITimer* timer = device->getTimer();
	uint32 cur_time = 0;
	uint32 prev_time = timer->getRealTime();
	float frame_counter = 0.0f;
	int fps = 0;
	bool was_connected = false;
	std::wstring corename;
#ifdef __ANDROID__
	ogles2Solid = 0;
	ogles2TrasparentAlpha = 0;
	ogles2BlendTexture = 0;
	ogles2Solid = irr::video::EMT_SOLID;
	ogles2TrasparentAlpha = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
	ogles2BlendTexture = irr::video::EMT_ONETEXTURE_BLEND;
	matManager.mCard.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2BlendTexture;
	matManager.mTexture.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2TrasparentAlpha;
	matManager.mBackLine.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2BlendTexture;
	matManager.mSelField.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2BlendTexture;
	matManager.mOutLine.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2Solid;
	matManager.mTRTexture.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2TrasparentAlpha;
	matManager.mATK.MaterialType = (irr::video::E_MATERIAL_TYPE)ogles2BlendTexture;
	if(!isNPOTSupported) {
		matManager.mCard.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mCard.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mTexture.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mTexture.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mBackLine.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mBackLine.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mSelField.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mSelField.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mOutLine.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mOutLine.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mTRTexture.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mTRTexture.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mATK.TextureLayer[0].TextureWrapU = irr::video::ETC_CLAMP_TO_EDGE;
		matManager.mATK.TextureLayer[0].TextureWrapV = irr::video::ETC_CLAMP_TO_EDGE;
	}
#endif
	if (globalHandlers->configs->fullscreen) {
		// Synchronize actual fullscreen state with config struct
		bool currentlyFullscreen = false;
		GUIUtils::ToggleFullscreen(device, currentlyFullscreen);
	}
	while(device->run()) {
		auto repos = globalHandlers->gitManager->GetReadyRepos();
		if(!repos.empty()) {
			bool refresh_db = false;
			for(auto& repo : repos) {
				auto grepo = &repoInfoGui[repo->repo_path];
				UpdateRepoInfo(repo, grepo);
				auto data_path = Utils::ToPathString(repo->data_path);
				auto files = Utils::FindFiles(data_path, { EPRO_TEXT("cdb") }, 0);
				for(auto& file : files)
					refresh_db = globalHandlers->dataManager->LoadDB(data_path + file) || refresh_db;
				globalHandlers->dataManager->LoadStrings(data_path + EPRO_TEXT("strings.conf"));
			}
			if(refresh_db && is_building && deckBuilder.results.size())
				deckBuilder.StartFilter(true);
		}
#ifdef YGOPRO_BUILD_DLL
		if(!dInfo.isStarted && cores_to_load.size() && globalHandlers->gitManager->GetUpdatingRepos() == 0) {
			for(auto& path : cores_to_load) {
				void* ncore = nullptr;
				if((ncore = ChangeOCGcore(globalHandlers->configs->working_directory + path, ocgcore))) {
					corename = Utils::ToUnicodeIfNeeded(path);
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
		if(corename.size() && (
			!wMessage->isVisible() ||
			std::wstring(stMessage->getText()) == globalHandlers->dataManager->GetSysString(1430)
		)) {
			stMessage->setText(fmt::format(globalHandlers->dataManager->GetSysString(1431).c_str(), corename).c_str());
			PopupElement(wMessage);
			corename.clear();
		}
#endif //YGOPRO_BUILD_DLL
		for(auto& repo : globalHandlers->gitManager->GetRepoStatus()) {
			repoInfoGui[repo.first].progress1->setProgress(repo.second);
			repoInfoGui[repo.first].progress2->setProgress(repo.second);
		}
		globalHandlers->sounds->Tick();
		fps++;
		auto now = timer->getRealTime();
		delta_time = now - prev_time;
		prev_time = now;
		cur_time += delta_time;
		bool resized = false;
		dimension2du size = driver->getScreenSize();
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
			window_scale.X = (window_size.Width / 1024.0) / globalHandlers->configs->dpi_scale;
			window_scale.Y = (window_size.Height / 640.0) / globalHandlers->configs->dpi_scale;
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
			else if(dInfo.isSingleMode)
				discord.UpdatePresence(DiscordWrapper::PUZZLE);
			else {
				if(dInfo.isStarted)
					discord.UpdatePresence(DiscordWrapper::DUEL_STARTED);
				else
					discord.UpdatePresence(DiscordWrapper::DUEL);
			}
			if (showcardcode == 1 || showcardcode == 3)
				globalHandlers->sounds->PlayBGM(SoundManager::BGM::WIN);
			else if (showcardcode == 2)
				globalHandlers->sounds->PlayBGM(SoundManager::BGM::LOSE);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] <= dInfo.lp[LocalPlayer(1)] / 2)
				globalHandlers->sounds->PlayBGM(SoundManager::BGM::DISADVANTAGE);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] >= dInfo.lp[LocalPlayer(1)] * 2)
				globalHandlers->sounds->PlayBGM(SoundManager::BGM::ADVANTAGE);
			else
				globalHandlers->sounds->PlayBGM(SoundManager::BGM::DUEL);
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
			globalHandlers->sounds->PlayBGM(SoundManager::BGM::DECK);
			DrawBackImage(imageManager.tBackGround_deck, resized);
			MATERIAL_GUARD(DrawDeckBd());
		} else {
			if(dInfo.isInLobby)
				discord.UpdatePresence(DiscordWrapper::IN_LOBBY);
			else
				discord.UpdatePresence(DiscordWrapper::MENU);
			globalHandlers->sounds->PlayBGM(SoundManager::BGM::MENU);
			DrawBackImage(imageManager.tBackGround_menu, resized);
		}
		if (is_building || is_siding) {
			fpsCounter->setRelativePosition(Resize(205, CARD_IMG_HEIGHT + 1, 300, CARD_IMG_HEIGHT + 21));
		} else if (wChat->isVisible()) { // Move it above the chat box
			fpsCounter->setRelativePosition(Resize(1024 - fpsCounter->getTextWidth() - 2, 600, 1024, 620));
		} else { // bottom right of window with a little padding
			fpsCounter->setRelativePosition(Resize(1024 - fpsCounter->getTextWidth() - 2, 620, 1024, 640));
		}
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
				stHintMsg->setText(globalHandlers->dataManager->GetSysString(1390).c_str());
			} else if((int)std::round(waitFrame) % 90 == 30) {
				stHintMsg->setText(globalHandlers->dataManager->GetSysString(1391).c_str());
			} else if((int)std::round(waitFrame) % 90 == 60) {
				stHintMsg->setText(globalHandlers->dataManager->GetSysString(1392).c_str());
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
			stACMessage->setText(globalHandlers->dataManager->GetSysString(1437).c_str());
			PopupElement(wACMessage, 30);
		} else if(!discord.connected && was_connected) {
			was_connected = false;
			env->setFocus(stACMessage);
			stACMessage->setText(globalHandlers->dataManager->GetSysString(1438).c_str());
			PopupElement(wACMessage, 30);
		}
#ifndef __ANDROID__
#ifdef __APPLE__
		// Recent versions of macOS break OpenGL vsync while offscreen, resulting in
		// astronomical FPS and CPU usage. As a workaround, while the game window is
		// fully occluded, the game is restricted to 30 FPS.
		int fpsLimit = device->isWindowActive() ? globalHandlers->configs->max_fps : 30;
		if (!device->isWindowActive() || (globalHandlers->configs->max_fps && !globalHandlers->configs->use_vsync)) {
#else
		int fpsLimit = globalHandlers->configs->max_fps;
		if (globalHandlers->configs->max_fps && !globalHandlers->configs->use_vsync) {
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
			fpsCounter->setText(fmt::format(globalHandlers->dataManager->GetSysString(1444), fps).c_str());
			fps = 0;
			cur_time -= 1000;
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
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
	return false;
}
path_string Game::NoSkinLabel() {
	return Utils::ToPathString(globalHandlers->dataManager->GetSysString(2065));
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
		((CGUICustomCheckBox*)chkShowPassword)->setColor(roomlistcolor);
		((CGUICustomCheckBox*)chkShowActiveRooms)->setColor(roomlistcolor);
		fpsCounter->setOverrideColor(skin::FPS_TEXT_COLOR_VAL);
		for(auto& repo : repoInfoGui) {
			repo.second.progress1->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
			repo.second.progress2->setColors(skin::PROGRESSBAR_FILL_COLOR_VAL, skin::PROGRESSBAR_EMPTY_COLOR_VAL);
		}
	};
	if(!skinSystem || ((skinname == prev_skin || (reload && prev_skin == EPRO_TEXT(""))) && !firstrun))
		return false;
	if(!reload)
		prev_skin = skinname;
	if(prev_skin == NoSkinLabel()) {
		auto skin = env->createSkin(gui::EGST_WINDOWS_METALLIC);
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
			auto skin = env->createSkin(gui::EGST_WINDOWS_METALLIC);
			env->setSkin(skin);
			skin->drop();
			skin::ResetDefaults();
			imageManager.ResetTextures();
		}
	}
	auto skin = env->getSkin();
	skin->setFont(guiFont);
#define SKIN_SCALE(elem)skin->setSize(elem, Scale(skin->getSize(elem)));
	skin->setSize(EGDS_SCROLLBAR_SIZE, Scale(20));
	SKIN_SCALE(EGDS_MENU_HEIGHT)
	SKIN_SCALE(EGDS_WINDOW_BUTTON_WIDTH)
	SKIN_SCALE(EGDS_CHECK_BOX_WIDTH)
	SKIN_SCALE(EGDS_BUTTON_WIDTH)
	SKIN_SCALE(EGDS_BUTTON_HEIGHT)
	SKIN_SCALE(EGDS_TITLEBARTEXT_DISTANCE_X)
	SKIN_SCALE(EGDS_TITLEBARTEXT_DISTANCE_Y)
	SKIN_SCALE(EGDS_TEXT_DISTANCE_X)
	SKIN_SCALE(EGDS_TEXT_DISTANCE_Y)
	SKIN_SCALE(EGDS_MESSAGE_BOX_GAP_SPACE)
#undef SKIN_SCALE
	if(wInfos) {
		wInfos->setTabHeight(skin->getSize(EGDS_BUTTON_HEIGHT) + Scale(2));
		wInfos->setTabVerticalAlignment(irr::gui::EGUIA_UPPERLEFT);
	}
	if(prev_skin == NoSkinLabel()){
		for (u32 i = 0; i < EGDC_COUNT; ++i) {
			irr::video::SColor col = skin->getColor((EGUI_DEFAULT_COLOR)i);
			col.setAlpha(224);
			skin->setColor((EGUI_DEFAULT_COLOR)i, col);
		}
	}
	if(!firstrun)
		reapply_colors();
	if(wAbout)
		wAbout->setRelativePosition(recti(0, 0, std::min(Scale(450), stAbout->getTextWidth() + Scale(20)), std::min(stAbout->getTextHeight() + Scale(40), Scale(700))));
	return applied;
}
void Game::LoadZipArchives() {
	irr::io::IFileArchive* tmp_archive = nullptr;
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("zip") })) {
		filesystem->addFileArchive((EPRO_TEXT("./expansions/") + file).c_str(), true, false, irr::io::EFAT_ZIP, "", &tmp_archive);
		if(tmp_archive) {
			Utils::archives.emplace_back(tmp_archive);
		}
	}
}/*
void Game::LoadExpansionDB() {
	for (auto& file : Utils::FindFiles(EPRO_TEXT("./expansions/"), { EPRO_TEXT("cdb") }, 2))
		globalHandlers->dataManager->LoadDB(EPRO_TEXT("./expansions/") + file);
}
void Game::LoadArchivesDB() {
	for(auto& archive: Utils::archives) {
		auto files = Utils::FindFiles(archive, EPRO_TEXT(""), { EPRO_TEXT("cdb") }, 3);
		for(auto& index : files) {
			auto reader = archive->createAndOpenFile(index);
			if(reader == nullptr)
				continue;
			std::vector<char> buffer;
			buffer.resize(reader->getSize());
			reader->read(buffer.data(), buffer.size());
			reader->drop();
			globalHandlers->dataManager->LoadDBFromBuffer(buffer);
		}
	}
}*/
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
	for(auto& file : Utils::FindFiles(EPRO_TEXT("./deck/"), { EPRO_TEXT("ydk") })) {
		cbDeck->addItem(Utils::ToUnicodeIfNeeded(file.substr(0, file.size() - 4)).c_str());
	}
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(globalHandlers->configs->lastdeck == cbDeck->getItem(i)) {
			cbDeck->setSelected(i);
			break;
		}
	}
}
void Game::RefreshLFLists() {
	cbDBLFList->clear();
	cbDBLFList->setSelected(0);
	for (auto &list : deckManager._lfList) {
		auto i = cbDBLFList->addItem(list.listName.c_str());
		if (globalHandlers->configs->lastlflist == list.hash) {
			cbDBLFList->setSelected(i);
		}
	}
	deckBuilder.filterList = &deckManager._lfList[mainGame->cbDBLFList->getSelected()];
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
					bot.version = PRO_VERSION;
#ifdef _WIN32
					bot.executablePath = filesystem->getAbsolutePath(EPRO_TEXT("./WindBot")).c_str();
#else
					if (globalHandlers->configs->configs.size() && globalHandlers->configs->configs["posixPathExtension"].is_string()) {
						bot.executablePath = globalHandlers->configs->configs["posixPathExtension"].get<path_string>();
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
void Game::LoadConfig() {
	/*globalHandlers->configs->Load("./config/system.conf");
	if(configs.empty()) {
		std::ifstream conf_file(EPRO_TEXT("./config/configs.json"), std::ifstream::in);
		try {
			conf_file >> configs;
		}
		catch(std::exception& e) {
			ErrorLog(std::string("Exception occurred: ") + e.what());
		}
	}*/
}
void Game::SaveConfig() {
	globalHandlers->configs->nickname = ebNickName->getText();
	globalHandlers->configs->lastallowedcards = cbRule->getSelected();
	globalHandlers->configs->chkMAutoPos = tabSettings.chkMAutoPos->isChecked();
	globalHandlers->configs->chkSTAutoPos = tabSettings.chkSTAutoPos->isChecked();
	globalHandlers->configs->chkRandomPos = tabSettings.chkRandomPos->isChecked();
	globalHandlers->configs->chkAutoChain = tabSettings.chkAutoChainOrder->isChecked();
	globalHandlers->configs->chkWaitChain = tabSettings.chkNoChainDelay->isChecked();
	globalHandlers->configs->chkIgnore1 = tabSettings.chkIgnoreOpponents->isChecked();
	globalHandlers->configs->chkIgnore2 = tabSettings.chkIgnoreSpectators->isChecked();
	globalHandlers->configs->chkHideHintButton = tabSettings.chkHideChainButtons->isChecked();
	globalHandlers->configs->chkAnime = chkAnime->isChecked();
	globalHandlers->configs->Save("./config/system.conf");
}
void Game::LoadPicUrls() {
	try {
		if(globalHandlers->configs->configs.size() && globalHandlers->configs->configs["urls"].is_array()) {
			for(auto& obj : globalHandlers->configs->configs["urls"].get<std::vector<nlohmann::json>>()) {
				if(obj["url"].get<std::string>() == "default") {
					if(obj["type"].get<std::string>() == "pic") {
#ifdef DEFAULT_PIC_URL
						mainGame->globalHandlers->imageDownloader->AddDownloadResource({ DEFAULT_PIC_URL, ImageDownloader::ART });
#else
						continue;
#endif
					} else {
#ifdef DEFAULT_FIELD_URL
						mainGame->globalHandlers->imageDownloader->AddDownloadResource({ DEFAULT_FIELD_URL, ImageDownloader::FIELD });
#else
						continue;
#endif
					}
				} else {
					auto type = obj["type"].get<std::string>();
					mainGame->globalHandlers->imageDownloader->AddDownloadResource({ obj["url"].get<std::string>(), type == "field" ? ImageDownloader::FIELD : (type == "pic") ? ImageDownloader::ART : ImageDownloader::COVER });
				}
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception occurred: ") + e.what());
	}
}
Game::RepoGui* Game::AddGithubRepositoryStatusWindow(const RepoManager::GitRepo* repo) {
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
	((CGUICustomContextMenu*)mRepositoriesInfo)->addItem(a, -1);
	grepo.history_button1 = env->addButton(Scale(90 + 295, 0, 170 + 295, 20 + 5), a, BUTTON_REPO_CHANGELOG, globalHandlers->dataManager->GetSysString(1443).c_str());
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
	((CGUICustomContextMenu*)mTabRepositories)->addItem(b, -1);
	grepo.history_button2 = env->addButton(Scale(200, 5, 300 - 5, 20 + 10), b, BUTTON_REPO_CHANGELOG, globalHandlers->dataManager->GetSysString(1443).c_str());
	defaultStrings.emplace_back(grepo.history_button2, 1443);
	grepo.history_button2->setEnabled(repo->ready);
	return &grepo;
}
void Game::LoadGithubRepositories() {
	bool update_ready = true;
	for(auto& repo : globalHandlers->gitManager->GetAllRepos()) {
		auto grepo = AddGithubRepositoryStatusWindow(repo);
		if(repo->ready && update_ready) {
			UpdateRepoInfo(repo, grepo);
		} else {
			update_ready = false;
		}
	}
}
void Game::UpdateRepoInfo(const RepoManager::GitRepo* repo, RepoGui* grepo) {
	if(repo->error.size()) {
		ErrorLog("The repo " + repo->url + " couldn't be cloned");
		ErrorLog("Error: " + repo->error);
		grepo->history_button1->setText(globalHandlers->dataManager->GetSysString(1434).c_str());
		defaultStrings.emplace_back(grepo->history_button1, 1434);
		grepo->history_button1->setEnabled(true);
		grepo->history_button2->setText(globalHandlers->dataManager->GetSysString(1434).c_str());
		defaultStrings.emplace_back(grepo->history_button2, 1434);
		grepo->history_button2->setEnabled(true);
		grepo->commit_history_full = fmt::format(L"{}\n{}",
												fmt::format(globalHandlers->dataManager->GetSysString(1435).c_str(), BufferIO::DecodeUTF8s(repo->url).c_str()).c_str(),
												fmt::format(globalHandlers->dataManager->GetSysString(1436).c_str(), BufferIO::DecodeUTF8s(repo->error).c_str()).c_str()
		);
		grepo->commit_history_partial = grepo->commit_history_full;
	}
	script_dirs.insert(script_dirs.begin(), Utils::ToPathString(repo->script_path));
	auto script_subdirs = Utils::FindSubfolders(Utils::ToPathString(repo->script_path));
	script_dirs.insert(script_dirs.begin(), script_subdirs.begin(), script_subdirs.end());
	pic_dirs.insert(pic_dirs.begin(), Utils::ToPathString(repo->pics_path));
	std::string text;
	std::for_each(repo->commit_history_full.begin(), repo->commit_history_full.end(), [&text](const std::string& n) { text += n + "\n\n"; });
	if(text.size())
		text.erase(text.size() - 2, 2);
	grepo->commit_history_full = BufferIO::DecodeUTF8s(text);
	grepo->commit_history_partial.clear();
	if(repo->commit_history_partial.size()) {
		if(repo->commit_history_full.front() == repo->commit_history_partial.front() && repo->commit_history_full.back() == repo->commit_history_partial.back()) {
			grepo->commit_history_partial = grepo->commit_history_full;
		} else {
			text.clear();
			std::for_each(repo->commit_history_partial.begin(), repo->commit_history_partial.end(), [&text](const std::string& n) { text += n + "\n\n"; });
			if(text.size())
				text.erase(text.size() - 2, 2);
			grepo->commit_history_partial = BufferIO::DecodeUTF8s(text);
		}
	} else {
		grepo->commit_history_partial = globalHandlers->dataManager->GetSysString(1446);
	}
	grepo->history_button1->setEnabled(true);
	grepo->history_button2->setEnabled(true);
	if(repo->has_core) {
		cores_to_load.insert(cores_to_load.begin(), Utils::ToPathString(repo->core_path));
	}
	auto data_path = Utils::ToPathString(repo->data_path);
	if(deckManager.LoadLFListSingle(data_path + EPRO_TEXT("lflist.conf")) || deckManager.LoadLFListFolder(data_path + EPRO_TEXT("lflists/"))) {
		deckManager.RefreshLFList();
		RefreshLFLists();
	}
}
void Game::LoadServers() {
	try {
		if(globalHandlers->configs->configs.size() && globalHandlers->configs->configs["servers"].is_array()) {
			for(auto& obj : globalHandlers->configs->configs["servers"].get<std::vector<nlohmann::json>>()) {
				ServerInfo tmp_server;
				tmp_server.name = BufferIO::DecodeUTF8s(obj["name"].get<std::string>());
				tmp_server.address = BufferIO::DecodeUTF8s(obj["address"].get<std::string>());
				tmp_server.roomaddress = BufferIO::DecodeUTF8s(obj["roomaddress"].get<std::string>());
				tmp_server.roomlistport = obj["roomlistport"].get<int>();
				tmp_server.duelport = obj["duelport"].get<int>();
				mainGame->serverChoice->addItem(tmp_server.name.c_str());
				ServerLobby::serversVector.push_back(std::move(tmp_server));
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception occurred: ") + e.what());
	}
}
void Game::ShowCardInfo(int code, bool resize, ImageManager::imgType type) {
	static ImageManager::imgType prevtype = ImageManager::imgType::ART;
	if(code == 0) {
		ClearCardInfo(0);
		return;
	}
	if (showingcard == code && !resize && !cardimagetextureloading)
		return;
	auto cd = globalHandlers->dataManager->GetCardData(code);
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
	int tmp_code = code;
	if(cd->alias && (cd->alias - code < CARD_ARTWORK_VERSIONS_OFFSET || code - cd->alias < CARD_ARTWORK_VERSIONS_OFFSET))
		tmp_code = cd->alias;
	stName->setText(globalHandlers->dataManager->GetName(tmp_code).c_str());
	stPasscodeScope->setText(fmt::format(L"[{:08}] {}", tmp_code, globalHandlers->dataManager->FormatScope(cd->ot)).c_str());
	stSetName->setText(L"");
	auto setcodes = cd->setcodes;
	if (cd->alias) {
		auto data = globalHandlers->dataManager->GetCardData(cd->alias);
		if(data)
			setcodes = data->setcodes;
	}
	if (setcodes.size()) {
		stSetName->setText((globalHandlers->dataManager->GetSysString(1329) + globalHandlers->dataManager->FormatSetName(setcodes)).c_str());
	}
	if(cd->type & TYPE_MONSTER) {
		stInfo->setText(fmt::format(L"[{}] {} {}", globalHandlers->dataManager->FormatType(cd->type), globalHandlers->dataManager->FormatAttribute(cd->attribute), globalHandlers->dataManager->FormatRace(cd->race)).c_str());
		std::wstring text;
		if(cd->type & TYPE_LINK){
			if(cd->attack < 0)
				text.append(fmt::format(L"?/LINK {}	  ", cd->level));
			else
				text.append(fmt::format(L"{}/LINK {}   ", cd->attack, cd->level));
			text.append(globalHandlers->dataManager->FormatLinkMarker(cd->link_marker));
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
			stInfo->setText(fmt::format(L"[{}|{}]", globalHandlers->dataManager->FormatRace(cd->race, true), globalHandlers->dataManager->FormatType(cd->type)).c_str());
		} else {
			stInfo->setText(fmt::format(L"[{}]", globalHandlers->dataManager->FormatType(cd->type)).c_str());
		}
		if(cd->type & TYPE_LINK) {
			stDataInfo->setText(fmt::format(L"LINK {}   {}", cd->level, globalHandlers->dataManager->FormatLinkMarker(cd->link_marker)).c_str());
		} else
			stDataInfo->setText(L"");
	}
	RefreshCardInfoTextPositions();
	stText->setText(globalHandlers->dataManager->GetText(code).c_str());
}
void Game::RefreshCardInfoTextPositions() {
	const int xLeft = Scale(15);
	const int xRight = Scale(287 * window_scale.X);
	int offset = Scale(37);
	auto offsetIfVisibleWithContent = [&](IGUIStaticText* st) {
		if (st->isVisible() && wcscmp(st->getText(), L"")) {
			st->setRelativePosition(recti(xLeft, offset, xRight, offset + st->getTextHeight()));
			offset += st->getTextHeight();
		}
	};
	offsetIfVisibleWithContent(stInfo);
	offsetIfVisibleWithContent(stDataInfo);
	offsetIfVisibleWithContent(stSetName);
	offsetIfVisibleWithContent(stPasscodeScope);
	stText->setRelativePosition(recti(xLeft, offset, xRight, stText->getParent()->getAbsolutePosition().getHeight() - Scale(1)));
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
		globalHandlers->sounds->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.selfnames[player]);
	} else if(type == 1) {
		globalHandlers->sounds->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.opponames[player]);
	} else if(type == 2) {
		switch(player) {
		case 7: //local name
			chatMsg[0].append(ebNickName->getText());
			break;
		case 8: //system custom message, no prefix.
			globalHandlers->sounds->PlaySoundEffect(SoundManager::SFX::CHAT);
			chatMsg[0].append(globalHandlers->dataManager->GetSysString(1439).c_str());
			break;
		case 9: //error message
			chatMsg[0].append(globalHandlers->dataManager->GetSysString(1440).c_str());
			break;
		default: //from watcher or unknown
			if(player < 11 || player > 19)
				chatMsg[0].append(globalHandlers->dataManager->GetSysString(1441).c_str());
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
	if (enable_log & 0x1)
		AddChatMsg(BufferIO::DecodeUTF8s(msg), 9, 2);
	if (enable_log & 0x2)
		ErrorLog(fmt::format("{}: {}", BufferIO::EncodeUTF8s(globalHandlers->dataManager->GetSysString(1440)), msg));
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
	uint32 flag = 0, filter = 0x100;
	for (int i = 0; i < (sizeof(chkCustomRules)/sizeof(irr::gui::IGUICheckBox*)); ++i, filter <<= 1)
		if (chkCustomRules[i]->isChecked()) {
			flag |= filter;
		}
	uint32 limits[] = { TYPE_FUSION, TYPE_SYNCHRO, TYPE_XYZ, TYPE_PENDULUM, TYPE_LINK };
	uint32 flag2 = 0;
	for (int i = 0; i < (sizeof(chkTypeLimit) / sizeof(irr::gui::IGUICheckBox*)); ++i)
		if (chkTypeLimit[i]->isChecked()) {
			flag2 |= limits[i];
		}
	cbDuelRule->clear();
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1260).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1261).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1262).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1263).c_str());
	cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1264).c_str());
#define CHECK(MR) case DUEL_MODE_MR##MR:{ cbDuelRule->setSelected(MR - 1); if (flag2 == DUEL_MODE_MR##MR##_FORB) break; }
	switch (flag) {
	CHECK(1)
	CHECK(2)
	CHECK(3)
	CHECK(4)
	CHECK(5)
	default: {
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1630).c_str());
		cbDuelRule->setSelected(5);
		break;
	}
	}
#undef CHECK
	duel_param = flag;
	forbiddentypes = flag2;
}
void Game::UpdateExtraRules() {
	for(int i = 0; i < 14; i++)
		chkRules[i]->setEnabled(true);
	if(chkRules[0]->isChecked()) {
		chkRules[1]->setEnabled(false);
		chkRules[5]->setEnabled(false);
	}
	if(chkRules[1]->isChecked()) {
		chkRules[0]->setEnabled(false);
		chkRules[5]->setEnabled(false);
	}
	if(chkRules[5]->isChecked()) {
		chkRules[0]->setEnabled(false);
		chkRules[1]->setEnabled(false);
	}
	if(chkRules[6]->isChecked()) {
		chkRules[7]->setEnabled(false);
		chkRules[8]->setEnabled(false);
	}
	if(chkRules[7]->isChecked()) {
		chkRules[6]->setEnabled(false);
		chkRules[8]->setEnabled(false);
	}
	if(chkRules[8]->isChecked()) {
		chkRules[6]->setEnabled(false);
		chkRules[7]->setEnabled(false);
	}
	extra_rules = 0;
	for(int flag = 1, i = 0; i < 14; i++, flag = flag << 1) {
		if(chkRules[i]->isChecked())
			extra_rules |= flag;
	}
}
int Game::GetMasterRule(uint32 param, uint32 forbiddentypes, int* truerule) {
	if(truerule)
		*truerule = 0;
#define CHECK(MR) case DUEL_MODE_MR##MR:{ if (truerule) *truerule = MR; if (forbiddentypes == DUEL_MODE_MR##MR##_FORB) return MR; }
	switch(param) {
	CHECK(1)
	CHECK(2)
	CHECK(3)
	CHECK(4)
	case DUEL_MODE_MR5: {
		if (truerule)
			*truerule = 5;
		if (forbiddentypes == DUEL_MODE_MR5_FORB)
			return 4;
	}
	default: {
		if (truerule && !*truerule)
			*truerule = 6;
		if ((param & DUEL_PZONE) && (param & DUEL_SEPARATE_PZONE) && (param & DUEL_EMZONE))
			return 5;
		else if(param & DUEL_EMZONE)
			return 4;
		else if (param & DUEL_PZONE)
			return 3;
		else
			return 2;
	}
	}
#undef CHECK
}
void Game::SetPhaseButtons() {
	// reset master rule 4 phase button position
	wPhase->setRelativePosition(Resize(480, 310, 855, 330));
	if (dInfo.extraval & 0x1) {
		if (dInfo.duel_field >= 4) {
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
		if (dInfo.duel_field >= 4) {
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
void Game::ReloadElementsStrings() {
	ShowCardInfo(showingcard, true);

	for(auto& elem : defaultStrings) {
		elem.first->setText(globalHandlers->dataManager->GetSysString(elem.second).c_str());
	}

	unsigned int nullLFlist = deckManager._lfList.size() - 1;
	deckManager._lfList[nullLFlist].listName = globalHandlers->dataManager->GetSysString(1442);
	auto prev = cbDBLFList->getSelected();
	cbDBLFList->removeItem(nullLFlist);
	cbDBLFList->addItem(deckManager._lfList[nullLFlist].listName.c_str(), deckManager._lfList[nullLFlist].hash);
	cbDBLFList->setSelected(prev);
	prev = cbLFlist->getSelected();
	cbLFlist->removeItem(nullLFlist);
	cbLFlist->addItem(deckManager._lfList[nullLFlist].listName.c_str(), deckManager._lfList[nullLFlist].hash);
	cbLFlist->setSelected(prev);

	prev = cbSortType->getSelected();
	cbSortType->clear();
	for(int i = 1370; i <= 1373; i++)
		cbSortType->addItem(globalHandlers->dataManager->GetSysString(i).c_str());
	cbSortType->setSelected(prev);
	prev = cbCardType->getSelected();
	cbCardType->clear();
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1310).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1312).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1313).c_str());
	cbCardType->addItem(globalHandlers->dataManager->GetSysString(1314).c_str());
	cbCardType->setSelected(prev);

	prev = cbCardType2->getSelected();
	cbCardType2->clear();
	switch(cbCardType->getSelected()) {
		case 0:
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
			break;
		case 1:
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1080).c_str(), 0);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1054).c_str(), TYPE_MONSTER + TYPE_NORMAL);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1055).c_str(), TYPE_MONSTER + TYPE_EFFECT);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1056).c_str(), TYPE_MONSTER + TYPE_FUSION);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1057).c_str(), TYPE_MONSTER + TYPE_RITUAL);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1063).c_str(), TYPE_MONSTER + TYPE_SYNCHRO);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1073).c_str(), TYPE_MONSTER + TYPE_XYZ);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1074).c_str(), TYPE_MONSTER + TYPE_PENDULUM);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1076).c_str(), TYPE_MONSTER + TYPE_LINK);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1075).c_str(), TYPE_MONSTER + TYPE_SPSUMMON);
			cbCardType2->addItem((globalHandlers->dataManager->GetSysString(1054) + L"|" + globalHandlers->dataManager->GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_TUNER);
			cbCardType2->addItem((globalHandlers->dataManager->GetSysString(1054) + L"|" + globalHandlers->dataManager->GetSysString(1074)).c_str(), TYPE_MONSTER + TYPE_NORMAL + TYPE_PENDULUM);
			cbCardType2->addItem((globalHandlers->dataManager->GetSysString(1063) + L"|" + globalHandlers->dataManager->GetSysString(1062)).c_str(), TYPE_MONSTER + TYPE_SYNCHRO + TYPE_TUNER);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1062).c_str(), TYPE_MONSTER + TYPE_TUNER);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1061).c_str(), TYPE_MONSTER + TYPE_GEMINI);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1060).c_str(), TYPE_MONSTER + TYPE_UNION);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1059).c_str(), TYPE_MONSTER + TYPE_SPIRIT);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1071).c_str(), TYPE_MONSTER + TYPE_FLIP);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1072).c_str(), TYPE_MONSTER + TYPE_TOON);
			break;
		case 2:
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1080).c_str(), 0);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1054).c_str(), TYPE_SPELL);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1066).c_str(), TYPE_SPELL + TYPE_QUICKPLAY);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1067).c_str(), TYPE_SPELL + TYPE_CONTINUOUS);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1057).c_str(), TYPE_SPELL + TYPE_RITUAL);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1068).c_str(), TYPE_SPELL + TYPE_EQUIP);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1069).c_str(), TYPE_SPELL + TYPE_FIELD);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1076).c_str(), TYPE_SPELL + TYPE_LINK);
			break;
		case 3:
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1080).c_str(), 0);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1054).c_str(), TYPE_TRAP);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1067).c_str(), TYPE_TRAP + TYPE_CONTINUOUS);
			cbCardType2->addItem(globalHandlers->dataManager->GetSysString(1070).c_str(), TYPE_TRAP + TYPE_COUNTER);
			break;
	}
	cbCardType2->setSelected(prev);

	prev = cbLimit->getSelected();
	cbLimit->clear();
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1310).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1316).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1317).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1318).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1320).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1900).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1901).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1902).c_str());
	cbLimit->addItem(globalHandlers->dataManager->GetSysString(1903).c_str());
	if(chkAnime->isChecked()) {
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1265).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1266).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1267).c_str());
		cbLimit->addItem(globalHandlers->dataManager->GetSysString(1268).c_str());
	}
	cbLimit->setSelected(prev);

	prev = cbAttribute->getSelected();
	cbAttribute->clear();
	cbAttribute->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(globalHandlers->dataManager->FormatAttribute(filter).c_str(), filter);
	cbAttribute->setSelected(prev);

	prev = cbRace->getSelected();
	cbRace->clear();
	cbRace->addItem(globalHandlers->dataManager->GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(globalHandlers->dataManager->FormatRace(filter).c_str(), filter);
	cbRace->setSelected(prev);

	if(is_building) {
		btnLeaveGame->setText(globalHandlers->dataManager->GetSysString(1306).c_str());
	} else if(!dInfo.isReplay && !dInfo.isSingleMode && dInfo.player_type < (dInfo.team1 + dInfo.team2)) {
		btnLeaveGame->setText(globalHandlers->dataManager->GetSysString(1351).c_str());
	} else if(dInfo.player_type == 7) {
		btnLeaveGame->setText(globalHandlers->dataManager->GetSysString(1350).c_str());
	} else if(dInfo.isSingleMode) {
		btnLeaveGame->setText(globalHandlers->dataManager->GetSysString(1210).c_str());
	}

	prev = cbFilterRule->getSelected();
	cbFilterRule->clear();
	cbFilterRule->addItem(fmt::format(L"[{}]", globalHandlers->dataManager->GetSysString(1225)).c_str());
	for(auto i = 1900; i <= 1904; ++i)
		cbFilterRule->addItem(globalHandlers->dataManager->GetSysString(i).c_str());
	cbFilterRule->setSelected(prev);

	prev = cbFilterBanlist->getSelected();
	cbFilterBanlist->clear();
	cbFilterBanlist->addItem(fmt::format(L"[{}]", globalHandlers->dataManager->GetSysString(1226)).c_str());
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbFilterBanlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);
	cbFilterBanlist->setSelected(prev);
	
	prev = cbDuelRule->getSelected();
	if (prev >= 5) {
		UpdateDuelParam();
	}
	else {
		cbDuelRule->clear();
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1260).c_str());
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1261).c_str());
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1262).c_str());
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1263).c_str());
		cbDuelRule->addItem(globalHandlers->dataManager->GetSysString(1264).c_str());
		cbDuelRule->setSelected(prev);
	}

	prev = cbRule->getSelected();
	cbRule->clear();
	for(auto i = 1900; i <= 1904; ++i)
		cbRule->addItem(globalHandlers->dataManager->GetSysString(i).c_str());
	cbRule->setSelected(prev);

	((CGUICustomTable*)roomListTable)->setColumnText(1, globalHandlers->dataManager->GetSysString(1225).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(2, globalHandlers->dataManager->GetSysString(1227).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(3, globalHandlers->dataManager->GetSysString(1236).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(4, globalHandlers->dataManager->GetSysString(1226).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(5, globalHandlers->dataManager->GetSysString(2030).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(6, globalHandlers->dataManager->GetSysString(2024).c_str());
	((CGUICustomTable*)roomListTable)->setColumnText(7, globalHandlers->dataManager->GetSysString(1988).c_str());
	roomListTable->setColumnWidth(0, roomListTable->getColumnWidth(0));

	mTopMenu->setItemText(0, globalHandlers->dataManager->GetSysString(2045).c_str()); //mRepositoriesInfo
	mTopMenu->setItemText(1, globalHandlers->dataManager->GetSysString(1970).c_str()); //mAbout

#define TYPECHK(id,stringid) chkTypeLimit[id]->setText(fmt::sprintf(globalHandlers->dataManager->GetSysString(1627), globalHandlers->dataManager->GetSysString(stringid)).c_str());
	TYPECHK(0, 1056);
	TYPECHK(1, 1063);
	TYPECHK(2, 1073);
	TYPECHK(3, 1074);
	TYPECHK(4, 1076);
#undef TYPECHK

	prev = gSettings.cbCurrentSkin->getSelected();
	gSettings.cbCurrentSkin->clear();
	gSettings.cbCurrentSkin->addItem(globalHandlers->dataManager->GetSysString(2065).c_str());
	auto skins = skinSystem->listSkins();
	for(int i = skins.size() - 1; i >= 0; i--) {
		gSettings.cbCurrentSkin->addItem(Utils::ToUnicodeIfNeeded(skins[i].c_str()).c_str());
	}
	gSettings.cbCurrentSkin->setSelected(prev);
}
void Game::OnResize() {
	wRoomListPlaceholder->setRelativePosition(recti(0, 0, mainGame->window_size.Width, mainGame->window_size.Height));
	wMainMenu->setRelativePosition(ResizeWin(370, 200, 650, 450));
	SetCentered(wCommitsLog);
	wDeckEdit->setRelativePosition(Resize(309, 8, 605, 130));
	cbDBLFList->setRelativePosition(Resize(80, 5, 220, 30));
	cbDBDecks->setRelativePosition(Resize(80, 35, 220, 60));
	btnClearDeck->setRelativePosition(Resize(115, 99, 165, 120));
	btnSortDeck->setRelativePosition(Resize(60, 99, 110, 120));
	btnShuffleDeck->setRelativePosition(Resize(5, 99, 55, 120));
	btnHandTest->setRelativePosition(Resize(205, 90, 295, 130));
	btnSaveDeck->setRelativePosition(Resize(225, 35, 290, 60));
	btnSaveDeckAs->setRelativePosition(Resize(225, 65, 290, 90));
	ebDeckname->setRelativePosition(Resize(80, 65, 220, 90));

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
	btnDeleteDeck->setRelativePosition(Resize(225, 95, 290, 120));
	btnRenameDeck->setRelativePosition(Resize(170, 99, 220, 120));

	wLanWindow->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wCreateHost->setRelativePosition(ResizeWin(320, 100, 700, 520));
	if (dInfo.opponames.size() + dInfo.selfnames.size()>=5) {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 500));
		wHostPrepare2->setRelativePosition(ResizeWin(750, 120, 950, 500));
	} else {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 440));
		wHostPrepare2->setRelativePosition(ResizeWin(750, 120, 950, 440));
	}
	wRules->setRelativePosition(ResizeWin(630, 100, 1000, 310));
	wCustomRules->setRelativePosition(ResizeWin(700, 100, 910, 430));
	wReplay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wSinglePlay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	gBot.window->setRelativePosition(core::position2di(wHostPrepare->getAbsolutePosition().LowerRightCorner.X, wHostPrepare->getAbsolutePosition().UpperLeftCorner.Y));

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
		window.second.history_button2->setRelativePosition(recti(ResizeX(200), 5, ResizeX(300 - 5), Scale(20 + 10)));
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

	ShowCardInfo(showingcard, true);

	auto tabsystemParentPos = tabSystem->getParent()->getAbsolutePosition();
	tabSystem->setRelativePosition(recti(0, 0, tabsystemParentPos.getWidth(), tabsystemParentPos.getHeight()));
	tabSettings.scrSoundVolume->setRelativePosition(rect<s32>(Scale(85), Scale(235), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(250)));
	tabSettings.scrMusicVolume->setRelativePosition(rect<s32>(Scale(85), Scale(295), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(310)));
	btnTabShowSettings->setRelativePosition(rect<s32>(Scale(20), Scale(415), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(435)));

	SetCentered(gSettings.window);

	wChat->setRelativePosition(rect<s32>(wInfos->getRelativePosition().LowerRightCorner.X + Scale(4), Scale<s32>(615.0f  * window_scale.Y), (window_size.Width - Scale(4 * window_scale.X)), (window_size.Height - Scale(2))));

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

	roomListTable->setRelativePosition(recti(ResizeX(1), chkShowActiveRooms->getRelativePosition().LowerRightCorner.Y + ResizeY(10), ResizeX(1024 - 2), btnLanRefresh2->getRelativePosition().UpperLeftCorner.Y - ResizeY(25)));
	roomListTable->setColumnWidth(0, roomListTable->getColumnWidth(0));
	roomListTable->addRow(roomListTable->getRowCount());
	roomListTable->removeRow(roomListTable->getRowCount() - 1);
	roomListTable->setSelected(-1);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2) {
	x = x * window_scale.X;
	y = y * window_scale.Y;
	x2 = x2 * window_scale.X;
	y2 = y2 * window_scale.Y;
	return Scale(x, y, x2, y2);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2) {
	x = x * window_scale.X + dx;
	y = y * window_scale.Y + dy;
	x2 = x2 * window_scale.X + dx2;
	y2 = y2 * window_scale.Y + dy2;
	return Scale(x, y, x2, y2);
}
vector2d<s32> Game::Resize(s32 x, s32 y, bool reverse) {
	if(reverse) {
		x = (x / window_scale.X) / globalHandlers->configs->dpi_scale;
		y = (y / window_scale.Y) / globalHandlers->configs->dpi_scale;
	} else {
		x = x * window_scale.X * globalHandlers->configs->dpi_scale;
		y = y * window_scale.Y * globalHandlers->configs->dpi_scale;
	}
	return { x, y };
}
recti Game::ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat) {
	s32 sx = x2 - x;
	s32 sy = y2 - y;
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
recti Game::ResizeElem(s32 x, s32 y, s32 x2, s32 y2, bool scale) {
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	x = (x + sx / 2 - 100) * window_scale.X - sx / 2 + 100;
	y = y * window_scale.Y;
	x2 = sx + x;
	y2 = sy + y;
	return scale ? Scale(x, y, x2, y2) : recti{x, y, x2, y2};
}
recti Game::ResizePhaseHint(s32 x, s32 y, s32 x2, s32 y2, s32 width) {
	auto res = Resize(x, y, x2, y2);
	res.UpperLeftCorner.X -= width / 2;
	return res;
}
recti Game::ResizeWinFromCenter(s32 x, s32 y, s32 x2, s32 y2, s32 xoff, s32 yoff) {
	auto size = driver->getScreenSize();
	recti rect(0, 0, size.Width, size.Height);
	auto center = rect.getCenter();
	core::dimension2d<u32> sizes((x + x2) / 2, (y + y2) / 2);
	return recti((center.X - sizes.Width) + xoff, center.Y - sizes.Height + yoff, center.X + sizes.Width + xoff, center.Y + sizes.Height + yoff);
}
void Game::ValidateName(irr::gui::IGUIElement* obj) {
	std::wstring text = obj->getText();
	const wchar_t chars[] = L"<>:\"/\\|?*";
	for(auto& forbid : chars)
		text.erase(std::remove(text.begin(), text.end(), forbid), text.end());
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
			auto reader = Utils::FindFileInArchives(EPRO_TEXT("script/"), name);
			if(reader == nullptr)
				continue;
			buffer.resize(reader->getSize());
			bool readed = reader->read(buffer.data(), buffer.size()) == buffer.size();
			reader->drop();
			if(readed)
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
	opts.payload1 = &*(globalHandlers->dataManager);
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
			std::cout << str << std::endl;
	}
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
	locales = Utils::FindSubfolders(EPRO_TEXT("./config/languages/"), 1, false);
}

void Game::ApplyLocale(int index, bool forced) {
	static int previndex = 0;
	if(index < 0 || index > locales.size())
		return;
	if(previndex == index && !forced)
		return;
	previndex = index;
	globalHandlers->dataManager->ClearLocaleStrings();
	globalHandlers->dataManager->ClearLocaleTexts();
	if(index > 0) {
		try {
			globalHandlers->configs->locale = locales[index - 1];
			auto locale = fmt::format(EPRO_TEXT("./config/languages/{}"), locales[index - 1]);
			for(auto& file : Utils::FindFiles(locale, { EPRO_TEXT("cdb") })) {
				globalHandlers->dataManager->LoadLocaleDB(fmt::format(EPRO_TEXT("{}/{}"), locale, file));
			}
			globalHandlers->dataManager->LoadLocaleStrings(fmt::format(EPRO_TEXT("{}/strings.conf"), locale));
		}
		catch(...) {
			return;
		}
	} else
		globalHandlers->configs->locale = EPRO_TEXT("en");
	ReloadElementsStrings();
}

}
