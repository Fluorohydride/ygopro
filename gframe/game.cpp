#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

#include "config.h"
#include "game.h"
#include "sound_manager.h"
#include "image_manager.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "replay.h"
#include "materials.h"
#include "duelclient.h"
#include "netserver.h"
#include "replay_mode.h"
#include "single_mode.h"
#include "CGUICustomCheckBox/CGUICustomCheckBox.h"
#include "CGUICustomTable/CGUICustomTable.h"

unsigned short PRO_VERSION = 0x1348;

nlohmann::json configs;

namespace ygo {

Game* mainGame;

bool Game::Initialize() {
	srand(time(0));
	LoadConfig();
	is_fullscreen = false;
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = gameConf.antialias;
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if(gameConf.use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
#endif
		params.DriverType = irr::video::EDT_OPENGL;
	params.Vsync = gameConf.use_vsync;
	params.WindowSize = irr::core::dimension2d<u32>(Scale(1024), Scale(640));
	device = irr::createDeviceEx(params);
	if(!device) {
		ErrorLog("Failed to create Irrlicht Engine device!");
		return false;
	}
	filesystem = device->getFileSystem();
	coreloaded = true;
#ifdef YGOPRO_BUILD_DLL
	if(!(ocgcore = LoadOCGcore(TEXT("./"))) && !(ocgcore = LoadOCGcore(TEXT("./expansions/"))))
		coreloaded = false;
#endif
	auto logger = device->getLogger();
	logger->setLogLevel(ELL_NONE);
	// Apply skin
	if (gameConf.skin_index >= 0)
	{
		skinSystem = new CGUISkinSystem("skin", device);
		core::array<core::stringw> skins = skinSystem->listSkins();
		if ((size_t)gameConf.skin_index < skins.size())
		{
			int index = skins.size() - gameConf.skin_index - 1; // reverse index
			skinSystem->applySkin(skins[index].c_str());
		}
	}
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
	deckManager.LoadLFList();
	driver = device->getVideoDriver();
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	imageManager.SetDevice(device);
	if(!imageManager.Initial()) {
		ErrorLog("Failed to load textures!");
		return false;
	}
	LoadPicUrls();
	if(!dataManager.LoadDB(TEXT("cards.cdb")))
		ErrorLog("cards.cdb not found in the root, loading anyways!");
	LoadExpansionDB();
	LoadZipArchives();
	LoadArchivesDB();
	if(!dataManager.LoadStrings(TEXT("strings.conf"))) {
		ErrorLog("Failed to load strings!");
		return false;
	}
	discord.Initialize(filesystem->getWorkingDirectory().c_str());
	PopulateResourcesDirectories();
	dataManager.LoadStrings(TEXT("./expansions/strings.conf"));
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), Scale(16));
	adFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), Scale(12));
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), Scale(48));
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.textfont.c_str(), Scale(gameConf.textfontsize));
	textFont = guiFont;
	if(!numFont || !textFont) {
		ErrorLog("Failed to load font(s)!");
		return false;
	}
	auto skin = env->getSkin();
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
	smgr = device->getSceneManager();
	device->setWindowCaption(L"EDOPro");
	device->setResizable(true);
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON hBigIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	HWND hWnd;
	irr::video::SExposedVideoData exposedData = driver->getExposedVideoData();
	if(driver->getDriverType() == irr::video::EDT_DIRECT3D9)
		hWnd = reinterpret_cast<HWND>(exposedData.D3D9.HWnd);
	else
		hWnd = reinterpret_cast<HWND>(exposedData.OpenGLWin32.HWnd);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (long)hSmallIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (long)hBigIcon);
	DragAcceptFiles(hWnd, TRUE);
#endif
	wCommitsLog = env->addWindow(Scale(0, 0, 500 + 10, 400 + 35 + 35), false, L"Update log");
	wCommitsLog->setVisible(false);
	wCommitsLog->getCloseButton()->setEnabled(false);
	wCommitsLog->getCloseButton()->setVisible(false);
	stCommitLog = irr::gui::CGUICustomText::addCustomText(L"", false, env, wCommitsLog, -1, Scale(5, 30, 505, 430));
	stCommitLog->setWordWrap(true);
	((CGUICustomText*)stCommitLog)->enableScrollBar();
	btnCommitLogExit = env->addButton(Scale(215, 435, 285, 460), wCommitsLog, BUTTON_REPO_CHANGELOG_EXIT, L"OK");
	mTopMenu = irr::gui::CGUICustomMenu::addCustomMenu(env);
	mRepositoriesInfo = mTopMenu->getSubMenu(mTopMenu->addItem(dataManager.GetSysString(2045).c_str(), 1, true, true));
	mAbout = mTopMenu->getSubMenu(mTopMenu->addItem(dataManager.GetSysString(1970).c_str(), 2, true, true));
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
								 L"This project is not affiliated or endorsed by Shueisha or Konami.",
								 Scale(10, 10, 440, 690), false, false, wAbout);
	((CGUICustomContextMenu*)mAbout)->addItem(wAbout, -1);
	//main menu
	wMainMenu = env->addWindow(Scale(370, 200, 650, 415), false, fmt::format(L"EDOPro by Project Ignis | {:X}.0{:X}.{:X}", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf).c_str());
	wMainMenu->getCloseButton()->setVisible(false);
	//wMainMenu->setVisible(!is_from_discord);
#define OFFSET(x1, y1, x2, y2) Scale(10, 30 + offset, 270, 60 + offset)
	int offset = 0;
	btnOnlineMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_ONLINE_MULTIPLAYER, dataManager.GetSysString(2042).c_str());
	offset += 35;
	btnLanMode = env->addButton(OFFSET(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200).c_str());
	offset += 35;
	btnSingleMode = env->addButton(OFFSET(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201).c_str());
	offset += 35;
	btnReplayMode = env->addButton(OFFSET(10, 100, 270, 130), wMainMenu, BUTTON_REPLAY_MODE, dataManager.GetSysString(1202).c_str());
	offset += 35;
	btnDeckEdit = env->addButton(OFFSET(10, 135, 270, 165), wMainMenu, BUTTON_DECK_EDIT, dataManager.GetSysString(1204).c_str());
	offset += 35;
	btnModeExit = env->addButton(OFFSET(10, 170, 270, 200), wMainMenu, BUTTON_MODE_EXIT, dataManager.GetSysString(1210).c_str());
	offset += 35;
#undef OFFSET
	btnSingleMode->setEnabled(coreloaded);
	//lan mode
	wLanWindow = env->addWindow(Scale(220, 100, 800, 520), false, dataManager.GetSysString(1200).c_str());
	wLanWindow->getCloseButton()->setVisible(false);
	wLanWindow->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1220).c_str(), Scale(10, 30, 220, 50), false, false, wLanWindow);
	ebNickName = env->addEditBox(gameConf.nickname.c_str(), Scale(110, 25, 450, 50), true, wLanWindow, EDITBOX_NICKNAME);
	ebNickName->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	lstHostList = env->addListBox(Scale(10, 60, 570, 320), wLanWindow, LISTBOX_LAN_HOST, true);
	lstHostList->setItemHeight(Scale(18));
	btnLanRefresh = env->addButton(Scale(240, 325, 340, 350), wLanWindow, BUTTON_LAN_REFRESH, dataManager.GetSysString(1217).c_str());
	env->addStaticText(dataManager.GetSysString(1221).c_str(), Scale(10, 360, 220, 380), false, false, wLanWindow);
	ebJoinHost = env->addEditBox(gameConf.lasthost.c_str(), Scale(110, 355, 350, 380), true, wLanWindow);
	ebJoinHost->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(gameConf.lastport.c_str(), Scale(360, 355, 420, 380), true, wLanWindow, EDITBOX_PORT_BOX);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1222).c_str(), Scale(10, 390, 220, 410), false, false, wLanWindow);
	ebJoinPass = env->addEditBox(gameConf.roompass.c_str(), Scale(110, 385, 420, 410), true, wLanWindow);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnJoinHost = env->addButton(Scale(460, 355, 570, 380), wLanWindow, BUTTON_JOIN_HOST, dataManager.GetSysString(1223).c_str());
	btnJoinCancel = env->addButton(Scale(460, 385, 570, 410), wLanWindow, BUTTON_JOIN_CANCEL, dataManager.GetSysString(1212).c_str());
	btnCreateHost = env->addButton(Scale(460, 25, 570, 50), wLanWindow, BUTTON_CREATE_HOST, dataManager.GetSysString(1224).c_str());
	btnCreateHost->setEnabled(coreloaded);
	//create host
	wCreateHost = env->addWindow(Scale(320, 100, 700, 520), false, dataManager.GetSysString(1224).c_str());
	wCreateHost->getCloseButton()->setVisible(false);
	wCreateHost->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1226).c_str(), Scale(20, 30, 220, 50), false, false, wCreateHost);
	cbLFlist = env->addComboBox(Scale(140, 25, 300, 50), wCreateHost);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbLFlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);
	env->addStaticText(dataManager.GetSysString(1225).c_str(), Scale(20, 60, 220, 80), false, false, wCreateHost);
	cbRule = env->addComboBox(Scale(140, 55, 300, 80), wCreateHost);
	cbRule->addItem(dataManager.GetSysString(1240).c_str());
	cbRule->addItem(dataManager.GetSysString(1241).c_str());
	cbRule->addItem(dataManager.GetSysString(1242).c_str());
	cbRule->addItem(dataManager.GetSysString(1243).c_str());
	env->addStaticText(dataManager.GetSysString(1227).c_str(), Scale(20, 90, 220, 110), false, false, wCreateHost);
	ebTeam1 = env->addEditBox(L"1", Scale(140, 85, 170, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	auto vsstring = env->addStaticText(L"vs.", Scale(175, 85, 195, 110), false, false, wCreateHost);
	vsstring->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebTeam2 = env->addEditBox(L"1", Scale(200, 85, 230, 110), true, wCreateHost, EDITBOX_TEAM_COUNT);
	ebTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	vsstring = env->addStaticText(L"Best of", Scale(235, 85, 280, 110), false, false, wCreateHost);
	vsstring->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	ebBestOf = env->addEditBox(L"1", Scale(285, 85, 315, 110), true, wCreateHost);
	ebBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRelayMode = env->addButton(Scale(325, 85, 370, 110), wCreateHost, -1, L"Relay");
	btnRelayMode->setIsPushButton(true);
	env->addStaticText(dataManager.GetSysString(1237).c_str(), Scale(20, 120, 320, 140), false, false, wCreateHost);
	ebTimeLimit = env->addEditBox(L"180", Scale(140, 115, 220, 140), true, wCreateHost);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1228).c_str(), Scale(20, 150, 320, 170), false, false, wCreateHost);
	btnRuleCards = env->addButton(Scale(260, 325, 370, 350), wCreateHost, BUTTON_RULE_CARDS, dataManager.GetSysString(1625).c_str());
	wRules = env->addWindow(Scale(630, 100, 1000, 310), false, L"");
	wRules->getCloseButton()->setVisible(false);
	wRules->setDrawTitlebar(false);
	wRules->setDraggable(true);
	wRules->setVisible(false);
	btnRulesOK = env->addButton(Scale(135, 175, 235, 200), wRules, BUTTON_RULE_OK, dataManager.GetSysString(1211).c_str());
	for(int i = 0; i < 13; ++i)
		chkRules[i] = env->addCheckBox(false, Scale(10 + (i % 2) * 150, 10 + (i / 2) * 20, 200 + (i % 2) * 120, 30 + (i / 2) * 20), wRules, CHECKBOX_EXTRA_RULE, dataManager.GetSysString(1132 + i).c_str());
	extra_rules = 0;
	env->addStaticText(dataManager.GetSysString(1236).c_str(), Scale(20, 180, 220, 200), false, false, wCreateHost);
	cbDuelRule = env->addComboBox(Scale(140, 175, 300, 200), wCreateHost, COMBOBOX_DUEL_RULE);
	cbDuelRule->addItem(dataManager.GetSysString(1260).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1261).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1262).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1263).c_str());
	cbDuelRule->setSelected(DEFAULT_DUEL_RULE - 1);
	btnCustomRule = env->addButton(Scale(305, 175, 370, 200), wCreateHost, BUTTON_CUSTOM_RULE, dataManager.GetSysString(1626).c_str());
	wCustomRules = env->addWindow(Scale(700, 100, 910, 410), false, L"");
	wCustomRules->getCloseButton()->setVisible(false);
	wCustomRules->setDrawTitlebar(false);
	wCustomRules->setDraggable(true);
	wCustomRules->setVisible(false);
	int spacing = 0;
	env->addStaticText(dataManager.GetSysString(1629).c_str(), Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	spacing++;
	for(int i = 0; i < 6; ++i, ++spacing)
		chkCustomRules[i] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 390 + i, dataManager.GetSysString(1631 + i).c_str());
	env->addStaticText(dataManager.GetSysString(1628).c_str(), Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	spacing++;
	chkTypeLimit[0] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1056)).c_str());
	spacing++;
	chkTypeLimit[1] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1063)).c_str());
	spacing++;
	chkTypeLimit[2] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1073)).c_str());
	spacing++;
	chkTypeLimit[3] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1074)).c_str());
	spacing++;
	chkTypeLimit[4] = env->addCheckBox(false, Scale(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 353 + spacing, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1076)).c_str());
	btnCustomRulesOK = env->addButton(Scale(55, 270, 155, 295), wCustomRules, BUTTON_CUSTOM_RULE_OK, dataManager.GetSysString(1211).c_str());
	forbiddentypes = DUEL_MODE_MR4_FORB;
	duel_param = DUEL_MODE_MR4;
	chkNoCheckDeck = env->addCheckBox(false, Scale(20, 210, 170, 230), wCreateHost, -1, dataManager.GetSysString(1229).c_str());
	chkNoShuffleDeck = env->addCheckBox(false, Scale(180, 210, 360, 230), wCreateHost, -1, dataManager.GetSysString(1230).c_str());
	env->addStaticText(dataManager.GetSysString(1231).c_str(), Scale(20, 240, 320, 260), false, false, wCreateHost);
	ebStartLP = env->addEditBox(L"8000", Scale(140, 235, 220, 260), true, wCreateHost);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1232).c_str(), Scale(20, 270, 320, 290), false, false, wCreateHost);
	ebStartHand = env->addEditBox(L"5", Scale(140, 265, 220, 290), true, wCreateHost);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1233).c_str(), Scale(20, 300, 320, 320), false, false, wCreateHost);
	ebDrawCount = env->addEditBox(L"1", Scale(140, 295, 220, 320), true, wCreateHost);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1234).c_str(), Scale(10, 330, 220, 350), false, false, wCreateHost);
	ebServerName = env->addEditBox(gameConf.gamename.c_str(), Scale(110, 325, 250, 350), true, wCreateHost);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1235).c_str(), Scale(10, 360, 220, 380), false, false, wCreateHost);
	ebServerPass = env->addEditBox(L"", Scale(110, 355, 250, 380), true, wCreateHost);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnHostConfirm = env->addButton(Scale(260, 355, 370, 380), wCreateHost, BUTTON_HOST_CONFIRM, dataManager.GetSysString(1211).c_str());
	btnHostCancel = env->addButton(Scale(260, 385, 370, 410), wCreateHost, BUTTON_HOST_CANCEL, dataManager.GetSysString(1212).c_str());
	stHostPort = env->addStaticText(dataManager.GetSysString(1238).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	ebHostPort = env->addEditBox(gameConf.serverport.c_str(), Scale(110, 385, 250, 410), true, wCreateHost, EDITBOX_PORT_BOX);
	ebHostPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHostNotes = env->addStaticText(dataManager.GetSysString(2024).c_str(), Scale(10, 390, 220, 410), false, false, wCreateHost);
	stHostNotes->setVisible(false);
	ebHostNotes = env->addEditBox(L"", Scale(110, 385, 250, 410), true, wCreateHost);
	ebHostNotes->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebHostNotes->setVisible(false);
	//host(single)
	wHostPrepare = env->addWindow(Scale(270, 120, 750, 440), false, dataManager.GetSysString(1250).c_str());
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	wHostPrepare2 = env->addWindow(Scale(750, 120, 950, 440), false, dataManager.GetSysString(1625).c_str());
	wHostPrepare2->getCloseButton()->setVisible(false);
	wHostPrepare2->setVisible(false);
	stHostPrepRule2 = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare2, -1, Scale(10, 30, 460, 350));
	stHostPrepRule2->setWordWrap(true);
	btnHostPrepDuelist = env->addButton(Scale(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, dataManager.GetSysString(1251).c_str());
	for(int i = 0; i < 6; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", Scale(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(Scale(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, Scale(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	btnHostPrepOB = env->addButton(Scale(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252).c_str());
	stHostPrepOB = env->addStaticText(fmt::format(L"{} 0", dataManager.GetSysString(1253)).c_str(), Scale(10, 210, 270, 230), false, false, wHostPrepare);
	stHostPrepRule = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare, -1, Scale(280, 30, 460, 230));
	stHostPrepRule->setWordWrap(true);
	stDeckSelect = env->addStaticText(dataManager.GetSysString(1254).c_str(), Scale(10, 235, 110, 255), false, false, wHostPrepare);
	cbDeckSelect = env->addComboBox(Scale(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	cbDeckSelect2 = env->addComboBox(Scale(280, 230, 430, 255), wHostPrepare);
	cbDeckSelect2->setMaxSelectionRows(10);
	btnHostPrepReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_READY, dataManager.GetSysString(1218).c_str());
	btnHostPrepNotReady = env->addButton(Scale(170, 180, 270, 205), wHostPrepare, BUTTON_HP_NOTREADY, dataManager.GetSysString(1219).c_str());
	btnHostPrepNotReady->setVisible(false);
	btnHostPrepStart = env->addButton(Scale(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, dataManager.GetSysString(1215).c_str());
	btnHostPrepCancel = env->addButton(Scale(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, dataManager.GetSysString(1210).c_str());
	//img
	wCardImg = env->addStaticText(L"", Scale(1, 1, 1 + CARD_IMG_WIDTH + 20, 1 + CARD_IMG_HEIGHT + 18), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(0xc0c0c0c0);
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
	irr::gui::IGUITab* tabInfo = wInfos->addTab(dataManager.GetSysString(1270).c_str());
	stName = irr::gui::CGUICustomText::addCustomText(L"", true, env, tabInfo, -1, Scale(10, 10, 287, 32));
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	((CGUICustomText*)stName)->setTextAutoScrolling(irr::gui::CGUICustomText::LEFT_TO_RIGHT_BOUNCING, 0, 1.0f, 0, 120, 300);
	stInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 37, 287, 60));
	stInfo->setWordWrap(true);
	stInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stDataInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 60, 287, 83));
	stDataInfo->setWordWrap(true);
	stDataInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stSetName = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 83, 287, 106));
	stSetName->setWordWrap(true);
	stSetName->setOverrideColor(SColor(255, 0, 0, 255));
	stText = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, Scale(15, 106, 287, 324));
	((CGUICustomText*)stText)->enableScrollBar();
	stText->setWordWrap(true);
	//log
	tabLog =  wInfos->addTab(dataManager.GetSysString(1271).c_str());
	lstLog = env->addListBox(Scale(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(Scale(18));
	btnClearLog = env->addButton(Scale(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, dataManager.GetSysString(1272).c_str());
	btnExpandLog = env->addButton(Scale(40, 300, 140, 325), tabLog, BUTTON_EXPAND_INFOBOX, dataManager.GetSysString(2043).c_str());
	//chat
	tabChat = wInfos->addTab(dataManager.GetSysString(1279).c_str());
	lstChat = env->addListBox(Scale(10, 10, 290, 290), tabChat, -1, false);
	lstChat->setItemHeight(Scale(18));
	btnClearChat = env->addButton(Scale(160, 300, 260, 325), tabChat, BUTTON_CLEAR_CHAT, dataManager.GetSysString(1282).c_str());
	btnExpandChat = env->addButton(Scale(40, 300, 140, 325), tabChat, BUTTON_EXPAND_INFOBOX, dataManager.GetSysString(2043).c_str());
	//system
	irr::gui::IGUITab* _tabSystem = wInfos->addTab(dataManager.GetSysString(1273).c_str());
        auto aaa = Scale(0, 0, wInfos->getRelativePosition().getWidth() + 1, wInfos->getRelativePosition().getHeight());
        tabSystem = Panel::addPanel(env, _tabSystem, -1, aaa, true, false);
	auto tabPanel = tabSystem->getSubpanel();
	chkMAutoPos = env->addCheckBox(false, Scale(20, 20, 280, 45), tabPanel, -1, dataManager.GetSysString(1274).c_str());
	chkMAutoPos->setChecked(gameConf.chkMAutoPos != 0);
	chkSTAutoPos = env->addCheckBox(false, Scale(20, 50, 280, 75), tabPanel, -1, dataManager.GetSysString(1278).c_str());
	chkSTAutoPos->setChecked(gameConf.chkSTAutoPos != 0);
	chkRandomPos = env->addCheckBox(false, Scale(40, 80, 300, 105), tabPanel, -1, dataManager.GetSysString(1275).c_str());
	chkRandomPos->setChecked(gameConf.chkRandomPos != 0);
	chkAutoChain = env->addCheckBox(false, Scale(20, 110, 280, 135), tabPanel, -1, dataManager.GetSysString(1276).c_str());
	chkAutoChain->setChecked(gameConf.chkAutoChain != 0);
	chkWaitChain = env->addCheckBox(false, Scale(20, 140, 280, 165), tabPanel, -1, dataManager.GetSysString(1277).c_str());
	chkWaitChain->setChecked(gameConf.chkWaitChain != 0);
	chkHideHintButton = env->addCheckBox(false, Scale(20, 170, 280, 195), tabPanel, CHECKBOX_CHAIN_BUTTONS, dataManager.GetSysString(1355).c_str());
	chkHideHintButton->setChecked(gameConf.chkHideHintButton != 0);
	chkIgnore1 = env->addCheckBox(false, Scale(20, 200, 280, 225), tabPanel, -1, dataManager.GetSysString(1290).c_str());
	chkIgnore1->setChecked(gameConf.chkIgnore1 != 0);
	chkIgnore2 = env->addCheckBox(false, Scale(20, 230, 280, 255), tabPanel, -1, dataManager.GetSysString(1291).c_str());
	chkIgnore2->setChecked(gameConf.chkIgnore2 != 0);
	chkEnableMusic = env->addCheckBox(gameConf.enablemusic, Scale(20, 260, 280, 285), tabPanel, CHECKBOX_ENABLE_MUSIC, dataManager.GetSysString(2046).c_str());
	chkEnableMusic->setChecked(gameConf.enablemusic);
	chkEnableSound = env->addCheckBox(gameConf.enablesound, Scale(20, 290, 280, 315), tabPanel, CHECKBOX_ENABLE_SOUND, dataManager.GetSysString(2047).c_str());
	chkEnableSound->setChecked(gameConf.enablesound);
	stMusicVolume = env->addStaticText(dataManager.GetSysString(2048).c_str(), Scale(20, 320, 80, 345), false, true, tabPanel, -1, false);
	scrMusicVolume = env->addScrollBar(true, Scale(85, 325, 280, 340), tabPanel, SCROLL_MUSIC_VOLUME);
	scrMusicVolume->setMax(100);
	scrMusicVolume->setMin(0);
	scrMusicVolume->setPos(gameConf.musicVolume * 100);
	scrMusicVolume->setLargeStep(1);
	scrMusicVolume->setSmallStep(1);
	stSoundVolume = env->addStaticText(dataManager.GetSysString(2049).c_str(), Scale(20, 350, 80, 375), false, true, tabPanel, -1, false);
	scrSoundVolume = env->addScrollBar(true, Scale(85, 355, 280, 370), tabPanel, SCROLL_SOUND_VOLUME);
	scrSoundVolume->setMax(100);
	scrSoundVolume->setMin(0);
	scrSoundVolume->setPos(gameConf.soundVolume * 100);
	scrSoundVolume->setLargeStep(1);
	scrSoundVolume->setSmallStep(1);
	chkQuickAnimation = env->addCheckBox(false, Scale(20, 380, 280, 405), tabPanel, CHECKBOX_QUICK_ANIMATION, dataManager.GetSysString(1299).c_str());
	chkQuickAnimation->setChecked(gameConf.quick_animation != 0);
	//log
	tabRepositories = wInfos->addTab(dataManager.GetSysString(2045).c_str());
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
		((CGUIButton*)btnHand[i])->setImage(imageManager.tHand[i]);
	}
	//
	wFTSelect = env->addWindow(Scale(550, 240, 780, 340), false, L"");
	wFTSelect->getCloseButton()->setVisible(false);
	wFTSelect->setVisible(false);
	btnFirst = env->addButton(Scale(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, dataManager.GetSysString(100).c_str());
	btnSecond = env->addButton(Scale(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, dataManager.GetSysString(101).c_str());
	//message (310)
	wMessage = env->addWindow(Scale(510 - 175, 200, 510 + 175, 340), false, dataManager.GetSysString(1216).c_str());
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wMessage, -1, Scale(20, 20, 350, 100));
	stMessage->setWordWrap(true);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(Scale(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, dataManager.GetSysString(1211).c_str());
	//auto fade message (310)
	wACMessage = env->addWindow(Scale(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = irr::gui::CGUICustomText::addCustomText(L"", true, env, wACMessage, -1, Scale(0, 0, 350, 60), true);
	stACMessage->setWordWrap(true);
	stACMessage->setBackgroundColor(0xc0c0c0ff);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(Scale(490, 200, 840, 340), false, dataManager.GetSysString(560).c_str());
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wQuery, -1, Scale(20, 20, 350, 100));
	stQMessage->setWordWrap(true);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(Scale(100, 105, 150, 130), wQuery, BUTTON_YES, dataManager.GetSysString(1213).c_str());
	btnNo = env->addButton(Scale(200, 105, 250, 130), wQuery, BUTTON_NO, dataManager.GetSysString(1214).c_str());
	//options (310)
	wOptions = env->addWindow(Scale(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions = irr::gui::CGUICustomText::addCustomText(L"", false, env, wOptions, -1, Scale(20, 20, 350, 100));
	stOptions->setWordWrap(true);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(Scale(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, dataManager.GetSysString(1211).c_str());
	btnOptionp = env->addButton(Scale(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, L"<<<");
	btnOptionn = env->addButton(Scale(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, L">>>");
	for(int i = 0; i < 5; ++i) {
		btnOption[i] = env->addButton(Scale(10, 30 + 40 * i, 340, 60 + 40 * i), wOptions, BUTTON_OPTION_0 + i, L"");
	}
	scrOption = env->addScrollBar(false, Scale(350, 30, 365, 220), wOptions, SCROLL_OPTION_SELECT);
	scrOption->setLargeStep(1);
	scrOption->setSmallStep(1);
	scrOption->setMin(0);
	//pos select
	wPosSelect = env->addWindow(Scale(340, 200, 935, 410), false, dataManager.GetSysString(561).c_str());
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	core::dimension2di imgsize = { Scale<s32>(CARD_IMG_WIDTH * 0.5f), Scale<s32>(CARD_IMG_HEIGHT * 0.5f) };
	btnPSAU = irr::gui::CGUIImageButton::addImageButton(env, Scale(10, 45, 150, 185), wPosSelect, BUTTON_POS_AU);
	btnPSAU->setImageSize(imgsize);
	btnPSAD = irr::gui::CGUIImageButton::addImageButton(env, Scale(155, 45, 295, 185), wPosSelect, BUTTON_POS_AD);
	btnPSAD->setImageSize(imgsize);
	((CGUIButton*)btnPSAD)->setImage(imageManager.tCover[0]);
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, Scale(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageSize(imgsize);
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, Scale(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageSize(imgsize);
	btnPSDD->setImageRotation(270);
	((CGUIButton*)btnPSDD)->setImage(imageManager.tCover[0]);
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
	btnSelectOK = env->addButton(Scale(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, dataManager.GetSysString(1211).c_str());
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
	btnDisplayOK = env->addButton(Scale(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, dataManager.GetSysString(1211).c_str());
	//announce number
	wANNumber = env->addWindow(Scale(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(Scale(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(Scale(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, dataManager.GetSysString(1211).c_str());
	//announce card
	wANCard = env->addWindow(Scale(430, 170, 840, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", Scale(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(Scale(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(Scale(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, dataManager.GetSysString(1211).c_str());
	//announce attribute
	wANAttribute = env->addWindow(Scale(500, 200, 830, 285), false, dataManager.GetSysString(562).c_str());
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, dataManager.FormatAttribute(filter).c_str());
	//announce race
	wANRace = env->addWindow(Scale(480, 200, 850, 410), false, dataManager.GetSysString(563).c_str());
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 25; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, dataManager.FormatRace(filter).c_str());
	//selection hint
	stHintMsg = env->addStaticText(L"", Scale(500, 60, 820, 90), true, false, 0, -1, false);
	stHintMsg->setBackgroundColor(0xc0ffffff);
	stHintMsg->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHintMsg->setVisible(false);
	//cmd menu
	wCmdMenu = env->addWindow(Scale(10, 10, 110, 179), false, L"");
	wCmdMenu->setDrawTitlebar(false);
	wCmdMenu->setVisible(false);
	wCmdMenu->getCloseButton()->setVisible(false);
	btnActivate = env->addButton(Scale(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1150).c_str());
	btnSummon = env->addButton(Scale(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, dataManager.GetSysString(1151).c_str());
	btnSPSummon = env->addButton(Scale(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, dataManager.GetSysString(1152).c_str());
	btnMSet = env->addButton(Scale(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, dataManager.GetSysString(1153).c_str());
	btnSSet = env->addButton(Scale(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, dataManager.GetSysString(1153).c_str());
	btnRepos = env->addButton(Scale(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, dataManager.GetSysString(1154).c_str());
	btnAttack = env->addButton(Scale(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, dataManager.GetSysString(1157).c_str());
	btnShowList = env->addButton(Scale(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, dataManager.GetSysString(1158).c_str());
	btnOperation = env->addButton(Scale(1, 169, 99, 189), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1161).c_str());
	btnReset = env->addButton(Scale(1, 190, 99, 210), wCmdMenu, BUTTON_CMD_RESET, dataManager.GetSysString(1162).c_str());
	//deck edit
	wDeckEdit = env->addStaticText(L"", Scale(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	stBanlist = env->addStaticText(dataManager.GetSysString(1300).c_str(), Scale(10, 9, 100, 29), false, false, wDeckEdit);
	cbDBLFList = env->addComboBox(Scale(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	stDeck = env->addStaticText(dataManager.GetSysString(1301).c_str(), Scale(10, 39, 100, 59), false, false, wDeckEdit);
	cbDBDecks = env->addComboBox(Scale(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);
	RefreshLFLists();
	btnSaveDeck = env->addButton(Scale(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, dataManager.GetSysString(1302).c_str());
	ebDeckname = env->addEditBox(L"", Scale(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECK_NAME);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(Scale(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, dataManager.GetSysString(1303).c_str());
	btnDeleteDeck = env->addButton(Scale(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, dataManager.GetSysString(1308).c_str());
	btnShuffleDeck = env->addButton(Scale(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307).c_str());
	btnSortDeck = env->addButton(Scale(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, dataManager.GetSysString(1305).c_str());
	btnClearDeck = env->addButton(Scale(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, dataManager.GetSysString(1304).c_str());
	btnSideOK = env->addButton(Scale(510, 40, 820, 80), 0, BUTTON_SIDE_OK, dataManager.GetSysString(1334).c_str());
	btnSideOK->setVisible(false);
	btnSideShuffle = env->addButton(Scale(310, 100, 370, 130), 0, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307).c_str());
	btnSideShuffle->setVisible(false);
	btnSideSort = env->addButton(Scale(375, 100, 435, 130), 0, BUTTON_SORT_DECK, dataManager.GetSysString(1305).c_str());
	btnSideSort->setVisible(false);
	btnSideReload = env->addButton(Scale(440, 100, 500, 130), 0, BUTTON_SIDE_RELOAD, dataManager.GetSysString(1309).c_str());
	btnSideReload->setVisible(false);
	btnHandTest = env->addButton(Scale(205, 90, 295, 130), 0, BUTTON_HAND_TEST, dataManager.GetSysString(1297).c_str());
	btnHandTest->setVisible(false);
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
		cbSortType->addItem(dataManager.GetSysString(i).c_str());
	wSort->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", Scale(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	stCategory = env->addStaticText(dataManager.GetSysString(1311).c_str(), Scale(10, 5, 70, 25), false, false, wFilter);
	cbCardType = env->addComboBox(Scale(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(dataManager.GetSysString(1310).c_str());
	cbCardType->addItem(dataManager.GetSysString(1312).c_str());
	cbCardType->addItem(dataManager.GetSysString(1313).c_str());
	cbCardType->addItem(dataManager.GetSysString(1314).c_str());
	cbCardType2 = env->addComboBox(Scale(130, 3, 190, 23), wFilter, COMBOBOX_SECONDTYPE);
	cbCardType2->setMaxSelectionRows(20);
	cbCardType2->addItem(dataManager.GetSysString(1310).c_str(), 0);
	chkAnime = env->addCheckBox(false, Scale(10, 96, 150, 118), wFilter, CHECKBOX_SHOW_ANIME, dataManager.GetSysString(1999).c_str());
	chkAnime->setChecked(gameConf.chkAnime != 0);
	stLimit = env->addStaticText(dataManager.GetSysString(1315).c_str(), Scale(205, 5, 280, 25), false, false, wFilter);
	cbLimit = env->addComboBox(Scale(260, 3, 390, 23), wFilter, COMBOBOX_OTHER_FILT);
	cbLimit->setMaxSelectionRows(10);
	cbLimit->addItem(dataManager.GetSysString(1310).c_str());
	cbLimit->addItem(dataManager.GetSysString(1316).c_str());
	cbLimit->addItem(dataManager.GetSysString(1317).c_str());
	cbLimit->addItem(dataManager.GetSysString(1318).c_str());
	cbLimit->addItem(dataManager.GetSysString(1320).c_str());
	cbLimit->addItem(dataManager.GetSysString(1240).c_str());
	cbLimit->addItem(dataManager.GetSysString(1241).c_str());
	cbLimit->addItem(dataManager.GetSysString(1242).c_str());
	if(chkAnime->isChecked()) {
		cbLimit->addItem(dataManager.GetSysString(1264).c_str());
		cbLimit->addItem(dataManager.GetSysString(1265).c_str());
		cbLimit->addItem(dataManager.GetSysString(1266).c_str());
		cbLimit->addItem(dataManager.GetSysString(1267).c_str());
	}
	stAttribute = env->addStaticText(dataManager.GetSysString(1319).c_str(), Scale(10, 28, 70, 48), false, false, wFilter);
	cbAttribute = env->addComboBox(Scale(60, 26, 190, 46), wFilter, COMBOBOX_OTHER_FILT);
	cbAttribute->setMaxSelectionRows(10);
	cbAttribute->addItem(dataManager.GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(dataManager.FormatAttribute(filter).c_str(), filter);
	stRace = env->addStaticText(dataManager.GetSysString(1321).c_str(), Scale(10, 51, 70, 71), false, false, wFilter);
	cbRace = env->addComboBox(Scale(60, 49, 190, 69), wFilter, COMBOBOX_OTHER_FILT);
	cbRace->setMaxSelectionRows(10);
	cbRace->addItem(dataManager.GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(dataManager.FormatRace(filter).c_str(), filter);
	stAttack = env->addStaticText(dataManager.GetSysString(1322).c_str(), Scale(205, 28, 280, 48), false, false, wFilter);
	ebAttack = env->addEditBox(L"", Scale(260, 26, 340, 46), true, wFilter);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stDefense = env->addStaticText(dataManager.GetSysString(1323).c_str(), Scale(205, 51, 280, 71), false, false, wFilter);
	ebDefense = env->addEditBox(L"", Scale(260, 49, 340, 69), true, wFilter);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stStar = env->addStaticText(dataManager.GetSysString(1324).c_str(), Scale(10, 74, 80, 94), false, false, wFilter);
	ebStar = env->addEditBox(L"", Scale(60, 72, 100, 92), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stScale = env->addStaticText(dataManager.GetSysString(1336).c_str(), Scale(110, 74, 150, 94), false, false, wFilter);
	ebScale = env->addEditBox(L"", Scale(150, 72, 190, 92), true, wFilter);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stSearch = env->addStaticText(dataManager.GetSysString(1325).c_str(), Scale(205, 74, 280, 94), false, false, wFilter);
	ebCardName = env->addEditBox(L"", Scale(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(Scale(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, dataManager.GetSysString(1326).c_str());
	btnStartFilter = env->addButton(Scale(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, dataManager.GetSysString(1327).c_str());
	btnClearFilter = env->addButton(Scale(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, dataManager.GetSysString(1304).c_str());
	wCategories = env->addWindow(Scale(450, 60, 1000, 270), false, L"");
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(Scale(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, dataManager.GetSysString(1211).c_str());
	for(int i = 0; i < 32; ++i)
		chkCategory[i] = env->addCheckBox(false, Scale(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, dataManager.GetSysString(1100 + i).c_str());
	btnMarksFilter = env->addButton(Scale(155, 96, 240, 118), wFilter, BUTTON_MARKS_FILTER, dataManager.GetSysString(1374).c_str());
	wLinkMarks = env->addWindow(Scale(700, 30, 820, 150), false, L"");
	wLinkMarks->getCloseButton()->setVisible(false);
	wLinkMarks->setDrawTitlebar(false);
	wLinkMarks->setDraggable(false);
	wLinkMarks->setVisible(false);
	btnMarksOK = env->addButton(Scale(45, 45, 75, 75), wLinkMarks, BUTTON_MARKERS_OK, dataManager.GetSysString(1211).c_str());
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
	wReplay = env->addWindow(Scale(220, 100, 800, 520), false, dataManager.GetSysString(1202).c_str());
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wReplay, LISTBOX_REPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstReplayList->setWorkingPath(L"./replay", true);
	lstReplayList->addFilteredExtensions(coreloaded ? std::vector<std::wstring>{L"yrp", L"yrpx"} : std::vector<std::wstring>{ L"yrpx" });
	lstReplayList->setItemHeight(Scale(18));
	btnLoadReplay = env->addButton(Scale(470, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, dataManager.GetSysString(1348).c_str());
	btnLoadReplay->setEnabled(false);
	btnDeleteReplay = env->addButton(Scale(360, 355, 460, 380), wReplay, BUTTON_DELETE_REPLAY, dataManager.GetSysString(1361).c_str());
	btnDeleteReplay->setEnabled(false);
	btnRenameReplay = env->addButton(Scale(360, 385, 460, 410), wReplay, BUTTON_RENAME_REPLAY, dataManager.GetSysString(1362).c_str());
	btnRenameReplay->setEnabled(false);
	btnReplayCancel = env->addButton(Scale(470, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, dataManager.GetSysString(1347).c_str());
	env->addStaticText(dataManager.GetSysString(1349).c_str(), Scale(360, 30, 570, 50), false, true, wReplay);
	stReplayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wReplay, -1, Scale(360, 60, 570, 350));
	stReplayInfo->setWordWrap(true);
	btnExportDeck = env->addButton(Scale(470, 325, 570, 350), wReplay, BUTTON_EXPORT_DECK, dataManager.GetSysString(1358).c_str());
	btnExportDeck->setEnabled(false);
	chkYrp = env->addCheckBox(false, Scale(360, 250, 560, 270), wReplay, -1, dataManager.GetSysString(1356).c_str());
	env->addStaticText(dataManager.GetSysString(1353).c_str(), Scale(360, 275, 570, 295), false, true, wReplay);
	ebRepStartTurn = env->addEditBox(L"", Scale(360, 300, 460, 320), true, wReplay, -1);
	ebRepStartTurn->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//single play window
	wSinglePlay = env->addWindow(Scale(220, 100, 800, 520), false, dataManager.GetSysString(1201).c_str());
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wSinglePlay, LISTBOX_SINGLEPLAY_LIST, Scale(10, 30, 350, 400), filesystem, true, true, false);
	lstSinglePlayList->setItemHeight(Scale(18));
	lstSinglePlayList->setWorkingPath(L"./puzzles", true);
	lstSinglePlayList->addFilteredExtensions({L"lua"});
	btnLoadSinglePlay = env->addButton(Scale(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, dataManager.GetSysString(1357).c_str());
	btnLoadSinglePlay->setEnabled(false);
	btnSinglePlayCancel = env->addButton(Scale(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, dataManager.GetSysString(1210).c_str());
	env->addStaticText(dataManager.GetSysString(1352).c_str(), Scale(360, 30, 570, 50), false, true, wSinglePlay);
	stSinglePlayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wSinglePlay, -1, Scale(360, 60, 570, 350));
	((CGUICustomText*)stSinglePlayInfo)->enableScrollBar();
	stSinglePlayInfo->setWordWrap(true);
	//replay save
	wReplaySave = env->addWindow(Scale(510, 200, 820, 320), false, dataManager.GetSysString(1340).c_str());
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1342).c_str(), Scale(20, 25, 290, 45), false, false, wReplaySave);
	ebRSName =  env->addEditBox(L"", Scale(20, 50, 290, 70), true, wReplaySave, EDITBOX_REPLAY_NAME);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(Scale(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, dataManager.GetSysString(1341).c_str());
	btnRSNo = env->addButton(Scale(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, dataManager.GetSysString(1212).c_str());
	//replay control
	wReplayControl = env->addStaticText(L"", Scale(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, dataManager.GetSysString(1343).c_str());
	btnReplayPause = env->addButton(Scale(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, dataManager.GetSysString(1344).c_str());
	btnReplayStep = env->addButton(Scale(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_STEP, dataManager.GetSysString(1345).c_str());
	btnReplayUndo = env->addButton(Scale(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_UNDO, dataManager.GetSysString(1360).c_str());
	btnReplaySwap = env->addButton(Scale(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346).c_str());
	btnReplayExit = env->addButton(Scale(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, dataManager.GetSysString(1347).c_str());
	//chat
	wChat = env->addWindow(Scale(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", Scale(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	ebChatInput->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	//swap
	btnSpectatorSwap = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346).c_str());
	btnSpectatorSwap->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(Scale(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, dataManager.GetSysString(1292).c_str());
	btnChainAlways = env->addButton(Scale(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, dataManager.GetSysString(1293).c_str());
	btnChainWhenAvail = env->addButton(Scale(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, dataManager.GetSysString(1294).c_str());
	btnChainIgnore->setIsPushButton(true);
	btnChainAlways->setIsPushButton(true);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	//shuffle
	btnShuffle = env->addButton(Scale(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, dataManager.GetSysString(1307).c_str());
	btnShuffle->setVisible(false);
	//cancel or finish
	btnCancelOrFinish = env->addButton(Scale(205, 230, 295, 265), 0, BUTTON_CANCEL_OR_FINISH, dataManager.GetSysString(1295).c_str());
	btnCancelOrFinish->setVisible(false);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(Scale(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	//restart single
	btnRestartSingle = env->addButton(Scale(205, 90, 295, 165), 0, BUTTON_RESTART_SINGLE, dataManager.GetSysString(1366).c_str());
	btnRestartSingle->setVisible(false);
	//tip
	stTip = env->addStaticText(L"", Scale(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setBackgroundColor(0xc0ffffff);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
	//tip for cards in select / display list
	stCardListTip = env->addStaticText(L"", Scale(0, 0, 150, 150), false, true, wCardSelect, TEXT_CARD_LIST_TIP, true);
	stCardListTip->setBackgroundColor(0xc0ffffff);
	stCardListTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stCardListTip->setVisible(false);
	device->setEventReceiver(&menuHandler);
	soundManager = std::make_unique<SoundManager>();
	if(!soundManager->Init(gameConf.soundVolume, gameConf.musicVolume, gameConf.enablesound, gameConf.enablemusic, nullptr)) {
		chkEnableSound->setChecked(false);
		chkEnableSound->setEnabled(false);
		chkEnableSound->setVisible(false);
		chkEnableMusic->setChecked(false);
		chkEnableMusic->setEnabled(false);
		chkEnableMusic->setVisible(false);
		scrMusicVolume->setVisible(false);
		stMusicVolume->setVisible(false);
		scrSoundVolume->setVisible(false);
		stSoundVolume->setVisible(false);
		chkQuickAnimation->setRelativePosition(Scale(20, 260, 280, 285));
	}

	//server lobby
	wRoomListPlaceholder = env->addStaticText(L"", Scale(1, 1, 1024 - 1, 640), false, false, 0, -1, false);
	//wRoomListPlaceholder->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	wRoomListPlaceholder->setVisible(false);

	//server choice dropdownlist
	irr::gui::IGUIStaticText* statictext = env->addStaticText(dataManager.GetSysString(2041).c_str(), Scale(10, 30, 110, 50), false, false, wRoomListPlaceholder, -1, false); // 2041 = Server:
	statictext->setOverrideColor(SColor(255, 255, 255, 255));
	serverChoice = env->addComboBox(Scale(90, 25, 385, 50), wRoomListPlaceholder, SERVER_CHOICE);

	//online nickname
	statictext = env->addStaticText(dataManager.GetSysString(1220).c_str(), Scale(10, 60, 110, 80), false, false, wRoomListPlaceholder, -1, false); // 1220 = Nickname:
	statictext->setOverrideColor(SColor(255, 255, 255, 255));
	ebNickNameOnline = env->addEditBox(gameConf.nickname.c_str(), Scale(90, 55, 275, 80), true, wRoomListPlaceholder, EDITBOX_NICKNAME);

	//top right host online game button
	btnCreateHost2 = env->addButton(Scale(904, 25, 1014, 50), wRoomListPlaceholder, BUTTON_CREATE_HOST2, dataManager.GetSysString(1224).c_str());
	btnCreateHost2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//filter dropdowns
	cbFilterRule = env->addComboBox(Scale(392, 25, 532, 50), wRoomListPlaceholder, CB_FILTER_ALLOWED_CARDS);
	//cbFilterMatchMode = env->addComboBox(Scale(392, 55, 532, 80), wRoomListPlaceholder, CB_FILTER_MATCH_MODE);
	cbFilterBanlist = env->addComboBox(Scale(392, 85, 532, 110), wRoomListPlaceholder, CB_FILTER_BANLIST);
	cbFilterRule->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	//cbFilterMatchMode->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	cbFilterBanlist->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	cbFilterRule->addItem(fmt::format(L"[{}]", dataManager.GetSysString(1225)).c_str());
	cbFilterRule->addItem(dataManager.GetSysString(1240).c_str());
	cbFilterRule->addItem(dataManager.GetSysString(1241).c_str());
	cbFilterRule->addItem(dataManager.GetSysString(1242).c_str());
	cbFilterRule->addItem(dataManager.GetSysString(1243).c_str());

	cbFilterBanlist->addItem(fmt::format(L"[{}]", dataManager.GetSysString(1226)).c_str());
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbFilterBanlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);

	/*cbFilterMatchMode->addItem(fmt::format(L"[{}]", dataManager.GetSysString(1227)).c_str());
	cbFilterMatchMode->addItem(dataManager.GetSysString(1244).c_str());
	cbFilterMatchMode->addItem(dataManager.GetSysString(1245).c_str());
	cbFilterMatchMode->addItem(dataManager.GetSysString(1246).c_str());*/
	//Scale(392, 55, 532, 80)
	ebOnlineTeam1 = env->addEditBox(L"0", Scale(140 + (392 - 140), 55, 170 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam1->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	vsstring = env->addStaticText(L"vs.", Scale(175 + (392 - 140), 55, 195 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	vsstring->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	vsstring->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	vsstring->setOverrideColor(SColor(255, 255, 255, 255));
	ebOnlineTeam2 = env->addEditBox(L"0", Scale(200 + (392 - 140), 55, 230 + (392 - 140), 80), true, wRoomListPlaceholder, EDITBOX_TEAM_COUNT);
	ebOnlineTeam2->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineTeam2->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	vsstring = env->addStaticText(L"Best of", Scale(235 + (392 - 140), 55, 280 + (392 - 140), 80), true, false, wRoomListPlaceholder);
	vsstring->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	vsstring->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	vsstring->setOverrideColor(SColor(255, 255, 255, 255));
	ebOnlineBestOf = env->addEditBox(L"0", Scale(285 + (392 - 140), 55, 315 + (392 - 140), 80), true, wRoomListPlaceholder);
	ebOnlineBestOf->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebOnlineBestOf->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	btnFilterRelayMode = env->addButton(Scale(325 + (392 - 140), 55, 370 + (392 - 140), 80), wRoomListPlaceholder, -1, L"Relay");
	btnFilterRelayMode->setIsPushButton(true);
	btnFilterRelayMode->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//filter rooms textbox
	ebRoomNameText = env->addStaticText(dataManager.GetSysString(2021).c_str(), Scale(572, 30, 682, 50), false, false, wRoomListPlaceholder); //2021 = Filter: 
	ebRoomNameText->setOverrideColor(SColor(255, 255, 255, 255));
	ebRoomName = env->addEditBox(L"", Scale(642, 25, 782, 50), true, wRoomListPlaceholder, EDIT_ONLINE_ROOM_NAME); //filter textbox
	ebRoomName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebRoomNameText->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	ebRoomName->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show locked rooms checkbox
    chkShowPassword = CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 55, 800, 80), wRoomListPlaceholder, CHECK_SHOW_LOCKED_ROOMS, dataManager.GetSysString(1994).c_str());
	chkShowPassword->setName(L"White");
	chkShowPassword->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show active rooms checkbox
	chkShowActiveRooms = CGUICustomCheckBox::addCustomCheckBox(false, env, Scale(642, 85, 800, 110), wRoomListPlaceholder, CHECK_SHOW_ACTIVE_ROOMS, dataManager.GetSysString(1985).c_str());
	chkShowActiveRooms->setName(L"White");
	chkShowActiveRooms->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);

	//show all rooms in a table
	roomListTable = CGUICustomTable::addCustomTable(env, Resize(1, 118, 1022, 550), wRoomListPlaceholder, TABLE_ROOMLIST, true);
	roomListTable->setResizableColumns(true);
	//roomListTable->setAlignment(EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_LOWERRIGHT);
	roomListTable->addColumn(L" ");//lock
	roomListTable->addColumn(dataManager.GetSysString(1225).c_str());//Allowed Cards:
	roomListTable->addColumn(dataManager.GetSysString(1227).c_str());//Duel Mode:
	roomListTable->addColumn(dataManager.GetSysString(1236).c_str());//master rule
	roomListTable->addColumn(dataManager.GetSysString(1226).c_str());//Forbidden List:
	roomListTable->addColumn(dataManager.GetSysString(2030).c_str());//Players:
	roomListTable->addColumn(dataManager.GetSysString(2024).c_str());//Notes:
	roomListTable->addColumn(dataManager.GetSysString(1988).c_str());//Status
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
	btnLanRefresh2 = env->addButton(Scale(462, 640 - 10 - 25 - 25 - 5, 562, 640 - 10 - 25 - 5), wRoomListPlaceholder, BUTTON_LAN_REFRESH2, dataManager.GetSysString(1217).c_str());
	btnLanRefresh2->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);

	//server room password
	wRoomPassword = env->addWindow(Scale(357, 200, 667, 320), false, L"");
	wRoomPassword->getCloseButton()->setVisible(false);
	wRoomPassword->setVisible(false);
	wRoomPassword->setAlignment(EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER, EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(2042).c_str(), Scale(20, 25, 290, 45), false, false, wRoomPassword);
	ebRPName = env->addEditBox(L"", Scale(20, 50, 290, 70), true, wRoomPassword, -1);
	ebRPName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRPYes = env->addButton(Scale(70, 80, 140, 105), wRoomPassword, BUTTON_ROOMPASSWORD_OK, dataManager.GetSysString(1211).c_str());
	btnRPNo = env->addButton(Scale(170, 80, 240, 105), wRoomPassword, BUTTON_ROOMPASSWORD_CANCEL, dataManager.GetSysString(1212).c_str());

	//join cancel buttons
	btnJoinHost2 = env->addButton(Scale(1024 - 10 - 110, 640 - 10 - 25 - 25 - 5, 1024 - 10, 640 - 10 - 25 - 5), wRoomListPlaceholder, BUTTON_JOIN_HOST2, dataManager.GetSysString(1223).c_str());
	btnJoinCancel2 = env->addButton(Scale(1024 - 10 - 110, 640 - 10 - 25, 1024 - 10, 640 - 10), wRoomListPlaceholder, BUTTON_JOIN_CANCEL2, dataManager.GetSysString(1212).c_str());
	btnJoinHost2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);
	btnJoinCancel2->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT);


	//load server(s)
	LoadServers();

	env->getSkin()->setFont(guiFont);
	env->setFocus(wMainMenu);
#ifdef YGOPRO_BUILD_DLL
	if(!coreloaded) {
		stMessage->setText(L"Couldn't load the duel api, you'll be limited to replay watching and online mode until the api is downloaded.");
		PopupElement(wMessage);
	}
#endif
	for (u32 i = 0; i < EGDC_COUNT; ++i) {
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(224);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
	hideChat = false;
	hideChatTimer = 0;
	delta_time = 0;

	Utils::CreateResourceFolders();

	LoadGithubRepositories();
	return true;
}
void Game::MainLoop() {
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, CAMERA_LEFT, CAMERA_RIGHT, CAMERA_BOTTOM, CAMERA_TOP, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);
	
	mProjection.buildCameraLookAtMatrixLH(vector3df(FIELD_X, FIELD_Y, FIELD_Z), vector3df(FIELD_X, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
	irr::ITimer* timer = device->getTimer();
	uint32 cur_time = 0;
	uint32 prev_time = timer->getRealTime();
	float frame_counter = 0.0f;
	int fps = 0;
	bool discord_message_shown = false;
	std::wstring corename;
	if(gameConf.fullscreen)
		Utils::ToggleFullscreen();
	while(device->run()) {
		auto repos = repoManager.GetReadyRepos();
		if(!repos.empty()) {
			bool refresh_db = false;
			for(auto& repo : repos) {
				if(repo.error.size()) {
					ErrorLog("The repo " + repo.url + " couldn't be cloned");
					ErrorLog("Error: " + repo.error);
					continue;
				}
				script_dirs.insert(script_dirs.begin(), Utils::ParseFilename(repo.script_path));
				pic_dirs.insert(pic_dirs.begin(), Utils::ParseFilename(repo.pics_path));
				auto data_path = Utils::ParseFilename(repo.data_path);
				auto files = Utils::FindfolderFiles(data_path, { TEXT("cdb") }, 0);
				for(auto& file : files)
					refresh_db = dataManager.LoadDB(data_path + file) || refresh_db;
				dataManager.LoadStrings(data_path + TEXT("strings.conf"));
				if(deckManager.LoadLFListSingle(data_path + TEXT("lflist.conf")) || deckManager.LoadLFListFolder(data_path + TEXT("lflists/"))) {
					deckManager.RefreshLFList();
					RefreshLFLists();
				}
				if(repo.has_core) {
					cores_to_load.insert(cores_to_load.begin(), Utils::ParseFilename(repo.core_path));
				}
				std::string text;
				std::for_each(repo.commit_history_full.begin(), repo.commit_history_full.end(), [&text](const std::string& n) { text += n + "\n\n"; });
				if(text.size())
					text.erase(text.size() - 2, 2);
				repoInfoGui[repo.repo_path].commit_history_full = BufferIO::DecodeUTF8s(text);
				repoInfoGui[repo.repo_path].commit_history_partial.clear();
				if(repo.commit_history_partial.size()) {
					if(repo.commit_history_full.front() == repo.commit_history_partial.front() && repo.commit_history_full.back() == repo.commit_history_partial.back()) {
						repoInfoGui[repo.repo_path].commit_history_partial = repoInfoGui[repo.repo_path].commit_history_full;
					} else {
						text.clear();
						std::for_each(repo.commit_history_partial.begin(), repo.commit_history_partial.end(), [&text](const std::string& n) { text += n + "\n\n"; });
						if(text.size())
							text.erase(text.size() - 2, 2);
						repoInfoGui[repo.repo_path].commit_history_partial = BufferIO::DecodeUTF8s(text);
					}
				}
				repoInfoGui[repo.repo_path].history_button1->setEnabled(true);

			}
			if(refresh_db && is_building && deckBuilder.results.size())
				deckBuilder.StartFilter(true);
		}
#ifdef YGOPRO_BUILD_DLL
		if(!dInfo.isStarted && cores_to_load.size() && repoManager.GetUpdatingRepos() == 0) {
			for(auto& path : cores_to_load) {
				void* ncore = nullptr;
				if((ncore = ChangeOCGcore(path, ocgcore))) {
					corename = Utils::ToUnicodeIfNeeded(path);
					ocgcore = ncore;
					if(!coreloaded) {
						coreloaded = true;
						btnSingleMode->setEnabled(true);
						btnCreateHost->setEnabled(true);
						lstReplayList->addFilteredExtensions({ L"yrp", L"yrpx" });
					}
					break;
				}
			}
			cores_to_load.clear();
		}
		if(corename.size() && ((!wMessage->isVisible()) || wMessage->isVisible() && std::wstring(stMessage->getText()) == L"Couldn't load the duel api, you'll be limited to replay watching and online mode until the api is downloaded.")) {
			stMessage->setText(fmt::format(L"Successfully loaded duel api from {}", corename).c_str());
			PopupElement(wMessage);
			corename.clear();
		}
#endif //YGOPRO_BUILD_DLL
		for(auto& repo : repoManager.GetRepoStatus()) {
			repoInfoGui[repo.first].progress1->setProgress(repo.second);
			repoInfoGui[repo.first].progress2->setProgress(repo.second);
		}
		soundManager->Tick();
		fps++;
		auto now = timer->getRealTime();
		delta_time = now - prev_time;
		prev_time = now;
		cur_time += delta_time;
		dimension2du size = driver->getScreenSize();
		if(window_size != size) {
			window_size = size;
			window_scale.X = (window_size.Width / 1024.0) / gameConf.dpi_scale;
			window_scale.Y = (window_size.Height / 640.0) / gameConf.dpi_scale;
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
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
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
				soundManager->PlayBGM(SoundManager::BGM::WIN);
			else if (showcardcode == 2)
				soundManager->PlayBGM(SoundManager::BGM::LOSE);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] <= dInfo.lp[LocalPlayer(1)] / 2)
				soundManager->PlayBGM(SoundManager::BGM::DISADVANTAGE);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] >= dInfo.lp[LocalPlayer(1)] * 2)
				soundManager->PlayBGM(SoundManager::BGM::ADVANTAGE);
			else
				soundManager->PlayBGM(SoundManager::BGM::DUEL);
			DrawBackImage(imageManager.tBackGround);
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();
		} else if(is_building) {
			if(is_siding)
				discord.UpdatePresence(DiscordWrapper::DECK_SIDING);
			else
				discord.UpdatePresence(DiscordWrapper::DECK);
			soundManager->PlayBGM(SoundManager::BGM::DECK);
			DrawBackImage(imageManager.tBackGround_deck);
			DrawDeckBd();
		} else {
			if(dInfo.isInLobby)
				discord.UpdatePresence(DiscordWrapper::IN_LOBBY);
			else
				discord.UpdatePresence(DiscordWrapper::MENU);
			soundManager->PlayBGM(SoundManager::BGM::MENU);
			DrawBackImage(imageManager.tBackGround_menu);
		}
		DrawGUI();
		DrawSpec();
		if(cardimagetextureloading) {
			ShowCardInfo(showingcard, false);
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
				stHintMsg->setText(dataManager.GetSysString(1390).c_str());
			} else if((int)std::round(waitFrame) % 90 == 30) {
				stHintMsg->setText(dataManager.GetSysString(1391).c_str());
			} else if((int)std::round(waitFrame) % 90 == 60) {
				stHintMsg->setText(dataManager.GetSysString(1392).c_str());
			}
		}
		driver->endScene();
		if(!closeSignal.try_lock())
			CloseDuelWindow();
		else
			closeSignal.unlock();
		if(gameConf.max_fps && !gameConf.use_vsync) {
			if(cur_time < fps * std::round(1000.0f / (float)gameConf.max_fps) - 20)
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
		while(cur_time >= 1000) {
			device->setWindowCaption(fmt::format(L"EDOPro FPS: {}", fps).c_str());
			fps = 0;
			cur_time -= 1000;
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
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
		if(discord.connected && !discord_message_shown) {
			discord_message_shown = true;
			env->setFocus(stACMessage);
			stACMessage->setText(L"Connected to Discord");
			PopupElement(wACMessage, 30);
		}
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
}
void Game::BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar) {
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
void Game::LoadZipArchives() {
	IFileArchive* tmp_archive = nullptr;
	for(auto& file : Utils::FindfolderFiles(TEXT("./expansions/"), { TEXT("zip") })) {
		filesystem->addFileArchive((TEXT("./expansions/") + file).c_str(), true, false, EFAT_ZIP, "", &tmp_archive);
		if(tmp_archive) {
			archives.emplace_back(tmp_archive);
		}
	}
}
void Game::LoadExpansionDB() {
	for (auto& file : Utils::FindfolderFiles(TEXT("./expansions/"), { TEXT("cdb") }, 2))
		dataManager.LoadDB(TEXT("./expansions/") + file);
}
void Game::LoadArchivesDB() {
	for(auto& archive: archives) {
		auto files = Utils::FindfolderFiles(archive, TEXT(""), { TEXT("cdb") }, 3);
		for(auto& index : files) {
			auto reader = archive.archive->createAndOpenFile(index);
			if(reader == nullptr)
				continue;
			std::vector<char> buffer;
			buffer.resize(reader->getSize());
			reader->read(buffer.data(), buffer.size());
			reader->drop();
			dataManager.LoadDBFromBuffer(buffer);
		}
	}
}
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
	for(auto& file : Utils::FindfolderFiles(TEXT("./deck/"), { TEXT("ydk") })) {
		cbDeck->addItem(Utils::ToUnicodeIfNeeded(file.substr(0, file.size() - 4)).c_str());
	}
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(gameConf.lastdeck == cbDeck->getItem(i)) {
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
		if (gameConf.lastlflist == list.hash) {
			cbDBLFList->setSelected(i);
		}
	}
	deckBuilder.filterList = &deckManager._lfList[mainGame->cbDBLFList->getSelected()];
}
void Game::RefreshReplay() {
	lstReplayList->resetPath();
}
void Game::RefreshSingleplay() {
	lstSinglePlayList->resetPath();
}
void Game::LoadConfig() {
	gameConf.antialias = 0;
	gameConf.use_d3d = false;
	gameConf.use_vsync = true;
	gameConf.max_fps = 60;
	gameConf.game_version = 0;
	gameConf.fullscreen = false;
	gameConf.serverport = L"7911";
	gameConf.textfontsize = 12;
	gameConf.nickname = L"";
	gameConf.gamename = L"";
	gameConf.lastdeck = L"";
	gameConf.lastlflist = 0;
	gameConf.numfont = L"";
	gameConf.textfont = L"";
	gameConf.lasthost = L"";
	gameConf.lastport = L"";
	gameConf.roompass = L"";
	//settings
	gameConf.chkMAutoPos = 0;
	gameConf.chkSTAutoPos = 1;
	gameConf.chkRandomPos = 0;
	gameConf.chkAutoChain = 0;
	gameConf.chkWaitChain = 0;
	gameConf.chkIgnore1 = 0;
	gameConf.chkIgnore2 = 0;
	gameConf.chkHideSetname = 0;
	gameConf.chkHideHintButton = 0;
	gameConf.skin_index = -1;
	gameConf.enablemusic = true;
	gameConf.enablesound = true;
	gameConf.musicVolume = 1.0;
	gameConf.soundVolume = 1.0;
	gameConf.draw_field_spell = 1;
	gameConf.quick_animation = 0;
	gameConf.chkAnime = 0;
	gameConf.dpi_scale = 1.0f;
	std::ifstream conf_file("system.conf", std::ifstream::in);
	if(!conf_file.is_open())
		return;
	std::string str;
	while(std::getline(conf_file, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if(str.empty() || str.at(0) == '#') {
			continue;
		}
		pos = str.find_first_of("=");
		if(pos == std::wstring::npos)
			continue;
		auto type = str.substr(0, pos - 1);
		str = str.substr(pos + 2);
		if(type == "antialias")
			gameConf.antialias = std::stoi(str);
		else if(type == "use_d3d")
			gameConf.use_d3d = std::stoi(str);
		else if(type == "use_vsync")
			gameConf.use_vsync = std::stoi(str);
		else if(type == "max_fps") {
			auto val = std::stoi(str);
			if(val >= 0)
				gameConf.max_fps = val;
		} else if(type == "fullscreen")
			gameConf.fullscreen = std::stoi(str);
		else if(type == "errorlog")
			enable_log = std::stoi(str);
		else if(type == "nickname")
			gameConf.nickname = BufferIO::DecodeUTF8s(str);
		else if(type == "gamename")
			gameConf.gamename = BufferIO::DecodeUTF8s(str);
		else if(type == "lastdeck")
			gameConf.lastdeck = BufferIO::DecodeUTF8s(str);
		else if(type == "lastlflist") {
			auto val = std::stoi(str);
			gameConf.lastlflist = val >= 0 ? val : 0;
		} else if(type == "textfont") {
			pos = str.find(L' ');
			if(pos == std::wstring::npos) {
				gameConf.textfont = BufferIO::DecodeUTF8s(str);
				continue;
			}
			gameConf.textfont = BufferIO::DecodeUTF8s(str.substr(0, pos));
			gameConf.textfontsize = std::stoi(str.substr(pos));
		} else if(type == "numfont")
			gameConf.numfont = BufferIO::DecodeUTF8s(str);
		else if(type == "serverport")
			gameConf.serverport = BufferIO::DecodeUTF8s(str);
		else if(type == "lasthost")
			gameConf.lasthost = BufferIO::DecodeUTF8s(str);
		else if(type == "lastport")
			gameConf.lastport = BufferIO::DecodeUTF8s(str);
		else if(type == "roompass")
			gameConf.roompass = BufferIO::DecodeUTF8s(str);
		else if(type == "game_version") {
			int version = std::stoi(str);
			if(version) {
				PRO_VERSION = std::stoi(str);
				gameConf.game_version = PRO_VERSION;
			}
		}
		else if(type == "automonsterpos")
			gameConf.chkMAutoPos = std::stoi(str);
		else if(type == "autospellpos")
			gameConf.chkSTAutoPos = std::stoi(str);
		else if(type == "randompos")
			gameConf.chkRandomPos = std::stoi(str);
		else if(type == "autochain")
			gameConf.chkAutoChain = std::stoi(str);
		else if(type == "waitchain")
			gameConf.chkWaitChain = std::stoi(str);
		else if(type == "mute_opponent")
			gameConf.chkIgnore1 = std::stoi(str);
		else if(type == "mute_spectators")
			gameConf.chkIgnore1 = std::stoi(str);
		else if(type == "hide_setname")
			gameConf.chkHideSetname = std::stoi(str);
		else if(type == "hide_hint_button")
			gameConf.chkHideHintButton = std::stoi(str);
		else if(type == "draw_field_spell")
			gameConf.draw_field_spell = std::stoi(str);
		else if(type == "quick_animation")
			gameConf.quick_animation = std::stoi(str);
		else if(type == "show_unofficial")
			gameConf.chkAnime = std::stoi(str);
		else if(type == "dpi_scale")
			gameConf.dpi_scale = std::stof(str);
		else if(type == "skin_index")
			gameConf.skin_index = std::stoi(str);
		else if(type == "enable_music")
			gameConf.enablemusic = !!std::stoi(str);
		else if(type == "enable_sound")
			gameConf.enablesound = !!std::stoi(str);
		else if(type == "music_volume")
			gameConf.musicVolume = std::stof(str)/100.0f;
		else if(type == "sound_volume")
			gameConf.soundVolume = std::stof(str)/100.0f;
	}
	conf_file.close();
	if(configs.empty()) {
		conf_file.open(TEXT("configs.json"), std::ifstream::in);
		try {
			conf_file >> configs;
		}
		catch(std::exception& e) {
			ErrorLog(std::string("Exception ocurred: ") + e.what());
		}
		conf_file.close();
	}
}
void Game::SaveConfig() {
	std::ofstream conf_file("system.conf", std::ofstream::out);
	if(!conf_file.is_open())
		return;
	conf_file << "#Configuration file\n";
	conf_file << "#Nickname & Gamename should be less than 20 characters\n";
	conf_file << "#The following parameters use 0 for 'disabled' or 1 for 'enabled':\n";
	conf_file << "#use_d3d, use_vsync, fullscreen, automonsterpos, autospellpos, randompos, autochain, waitchain, mute_opponent, mute_spectators,\n";
	conf_file <<  "hide_setname,hide_hint_button, draw_field_spell, quick_animation, show_unofficial, skin_index, enable_sound, enable_music\n";
	conf_file << "use_d3d = "			<< std::to_string(gameConf.use_d3d ? 1 : 0) << "\n";
	conf_file << "use_vsync = "			<< std::to_string(gameConf.use_vsync ? 1 : 0) << "\n";
	conf_file << "#limit the framerate, 0 unlimited, default 60\n";
	conf_file << "max_fps = "			<< std::to_string(gameConf.max_fps) << "\n";
	conf_file << "fullscreen = "		<< std::to_string(is_fullscreen ? 1 : 0) << "\n";
	conf_file << "antialias = "			<< std::to_string(gameConf.antialias) << "\n";
	conf_file << "errorlog = "			<< std::to_string(enable_log) << "\n";
	conf_file << "nickname = "			<< BufferIO::EncodeUTF8s(ebNickName->getText()) << "\n";
	conf_file << "gamename = "			<< BufferIO::EncodeUTF8s(gameConf.gamename) << "\n";
	conf_file << "lastdeck = "			<< BufferIO::EncodeUTF8s(gameConf.lastdeck) << "\n";
	conf_file << "lastlflist = "		<< std::to_string(gameConf.lastlflist) << "\n";
	conf_file << "textfont = "			<< BufferIO::EncodeUTF8s(gameConf.textfont) << " " << std::to_string(gameConf.textfontsize) << "\n";
	conf_file << "numfont = "			<< BufferIO::EncodeUTF8s(gameConf.numfont) << "\n";
	conf_file << "serverport = "		<< BufferIO::EncodeUTF8s(gameConf.serverport) << "\n";
	conf_file << "lasthost = "			<< BufferIO::EncodeUTF8s(gameConf.lasthost) << "\n";
	conf_file << "lastport = "			<< BufferIO::EncodeUTF8s(gameConf.lastport) << "\n";
	conf_file << "game_version = "		<< std::to_string(gameConf.game_version) << "\n";
	conf_file << "automonsterpos = "	<< std::to_string(chkMAutoPos->isChecked() ? 1 : 0) << "\n";
	conf_file << "autospellpos = "		<< std::to_string(chkSTAutoPos->isChecked() ? 1 : 0) << "\n";
	conf_file << "randompos = "			<< std::to_string(chkRandomPos->isChecked() ? 1 : 0) << "\n";
	conf_file << "autochain = "			<< std::to_string(chkAutoChain->isChecked() ? 1 : 0) << "\n";
	conf_file << "waitchain = "			<< std::to_string(chkWaitChain->isChecked() ? 1 : 0) << "\n";
	conf_file << "mute_opponent = "		<< std::to_string(chkIgnore1->isChecked() ? 1 : 0) << "\n";
	conf_file << "mute_spectators = "	<< std::to_string(chkIgnore2->isChecked() ? 1 : 0) << "\n";
	conf_file << "hide_setname = "		<< std::to_string(gameConf.chkHideSetname ? 1 : 0) << "\n";
	conf_file << "hide_hint_button = "	<< std::to_string(chkHideHintButton->isChecked() ? 1 : 0) << "\n";
	conf_file << "draw_field_spell = "	<< std::to_string(gameConf.draw_field_spell) << "\n";
	conf_file << "quick_animation = "	<< std::to_string(gameConf.quick_animation) << "\n";
	conf_file << "#shows the unofficial cards in deck edit, which includes anime, customs, etc\n";
	conf_file << "show_unofficial = "	<< std::to_string(chkAnime->isChecked() ? 1 : 0) << "\n";
	conf_file << "dpi_scale = "			<< std::to_string(gameConf.dpi_scale) << "\n";
	conf_file << "#if skins from the skin folder are in use\n";
	conf_file << "skin_index = "		<< std::to_string(gameConf.skin_index) << "\n";
	conf_file << "enable_music = "		<< std::to_string(chkEnableMusic->isChecked() ? 1 : 0) << "\n";
	conf_file << "enable_sound = "		<< std::to_string(chkEnableSound->isChecked() ? 1 : 0) << "\n";
	conf_file << "#integers between 0 and 100\n";
	int vol = gameConf.musicVolume * 100;
	if(vol < 0) vol = 0; else if(vol > 100) vol = 100;
	conf_file << "music_volume = "		<< std::to_string(vol) << "\n";
	vol = gameConf.soundVolume * 100;if(vol < 0) vol = 0; else if(vol > 100) vol = 100;
	conf_file << "sound_volume = "		<< std::to_string(vol) << "\n";
	conf_file.close();
}
void Game::LoadPicUrls() {
	try {
		if(configs.size() && configs["urls"].is_array()) {
			for(auto& obj : configs["urls"].get<std::vector<nlohmann::json>>()) {
				if(obj["url"].get<std::string>() == "default") {
					if(obj["type"].get<std::string>() == "pic") {
#ifdef DEFAULT_PIC_URL
						imageManager.AddDownloadResource({ DEFAULT_PIC_URL, ImageManager::ART });
#else
						continue;
#endif
					} else {
#ifdef DEFAULT_FIELD_URL
						imageManager.AddDownloadResource({ DEFAULT_FIELD_URL, ImageManager::FIELD });
#else
						continue;
#endif
					}
				} else {
					auto type = obj["type"].get<std::string>();
					imageManager.AddDownloadResource({ obj["url"].get<std::string>(), type == "field" ? ImageManager::FIELD : (type == "pic") ? ImageManager::ART : ImageManager::COVER });
				}
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception ocurred: ") + e.what());
	}
}
void Game::AddGithubRepositoryStatusWindow(const RepoManager::GitRepo& repo) {
	std::wstring name = BufferIO::DecodeUTF8s(repo.repo_name);
	auto a = env->addWindow(Scale(0, 0, 470, 55), false, L"", mRepositoriesInfo);
	a->getCloseButton()->setVisible(false);
	a->setDraggable(false);
	a->setDrawTitlebar(false);
	a->setDrawBackground(false);
	env->addStaticText(name.c_str(), Scale(5, 5, 90 + 295, 20 + 5), false, false, a);
	repoInfoGui[repo.repo_path].progress1 = new IProgressBar(env, Scale(5, 20 + 15, 170 + 295, 20 + 30), -1, a);
	repoInfoGui[repo.repo_path].progress1->addBorder(1);
	repoInfoGui[repo.repo_path].progress1->drop();
	((CGUICustomContextMenu*)mRepositoriesInfo)->addItem(a, -1);
	repoInfoGui[repo.repo_path].history_button1 = env->addButton(Scale(90 + 295, 0, 170 + 295, 20 + 5), a, BUTTON_REPO_CHANGELOG, L"Changelog");
	repoInfoGui[repo.repo_path].history_button1->setEnabled(false);

	auto b = env->addWindow(Scale(0, 0, 300, 55), false, L"", tabRepositories);
	b->getCloseButton()->setVisible(false);
	b->setDraggable(false);
	b->setDrawTitlebar(false);
	b->setDrawBackground(false);
	env->addStaticText(name.c_str(), Scale(5, 5, 300, 20 + 5), false, false, b);
	repoInfoGui[repo.repo_path].progress2 = new IProgressBar(env, Scale(5, 20 + 15, 300 - 5, 20 + 30), -1, b);
	repoInfoGui[repo.repo_path].progress2->addBorder(1);
	repoInfoGui[repo.repo_path].progress2->drop();
	((CGUICustomContextMenu*)mTabRepositories)->addItem(b, -1);
}
#define JSON_SET_IF_VALID(field, jsontype, cpptype)if(obj[#field].is_##jsontype())\
													tmp_repo.field = obj[#field].get<cpptype>();
void Game::LoadGithubRepositories() {
	try {
		if(configs.size() && configs["repos"].is_array()) {
			for(auto& obj : configs["repos"].get<std::vector<nlohmann::json>>()) {
				if(obj["should_read"].is_boolean() && !obj["should_read"].get<bool>())
					continue;
				RepoManager::GitRepo tmp_repo;
				JSON_SET_IF_VALID(url, string, std::string);
				JSON_SET_IF_VALID(should_update, boolean, bool);
				if(tmp_repo.url == "default") {
#ifdef DEFAULT_LIVE_URL
					tmp_repo.url = DEFAULT_LIVE_URL;
#ifdef YGOPRO_BUILD_DLL
					tmp_repo.has_core = true;
#endif
#else
					continue;
#endif //DEFAULT_LIVE_URL
				} else if(tmp_repo.url == "default_anime") {
#ifdef DEFAULT_LIVEANIME_URL
					tmp_repo.url = DEFAULT_LIVEANIME_URL;
#else
					continue;
#endif //DEFAULT_LIVEANIME_URL
				} else {
					JSON_SET_IF_VALID(repo_path, string, std::string);
					JSON_SET_IF_VALID(repo_name, string, std::string);
					JSON_SET_IF_VALID(data_path, string, std::string);
					JSON_SET_IF_VALID(script_path, string, std::string);
					JSON_SET_IF_VALID(pics_path, string, std::string);
#ifdef YGOPRO_BUILD_DLL
					JSON_SET_IF_VALID(core_path, string, std::string);
					JSON_SET_IF_VALID(has_core, boolean, bool);
#endif
				}
				if(tmp_repo.Sanitize()) {
					repoManager.AddRepo(tmp_repo);
					AddGithubRepositoryStatusWindow(tmp_repo);
				}
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception ocurred: ") + e.what());
	}
}
#undef JSON_SET_IF_VALID
void Game::LoadServers() {
	try {
		if(configs.size() && configs["servers"].is_array()) {
			for(auto& obj : configs["servers"].get<std::vector<nlohmann::json>>()) {
				ServerInfo tmp_server;
				tmp_server.name = BufferIO::DecodeUTF8s(obj["name"].get<std::string>());
				tmp_server.address = BufferIO::DecodeUTF8s(obj["address"].get<std::string>());
				tmp_server.roomaddress = BufferIO::DecodeUTF8s(obj["roomaddress"].get<std::string>());
				tmp_server.roomlistport = obj["roomlistport"].get<int>();
				tmp_server.duelport = obj["duelport"].get<int>();
				mainGame->serverChoice->addItem(tmp_server.name.c_str());
				serversVector.push_back(std::move(tmp_server));
			}
		}
	}
	catch(std::exception& e) {
		ErrorLog(std::string("Exception ocurred: ") + e.what());
	}
}
void Game::ShowCardInfo(int code, bool resize) {
	if (showingcard == code && !resize && !cardimagetextureloading)
		return;
	showingcard = code;
	int shouldrefresh = -1;
	auto img = imageManager.GetTextureCard(code, ImageManager::ART, false, true, &shouldrefresh);
	cardimagetextureloading = false;
	if(shouldrefresh == 2)
		cardimagetextureloading = true;
	CardData cd;
	if(!dataManager.GetData(code, &cd))
		memset(&cd, 0, sizeof(CardData));
	imgCard->setImage(img);
	imgCard->setScaleImage(true);
	int tmp_code = code;
	if(cd.alias && (cd.alias - code < CARD_ARTWORK_VERSIONS_OFFSET || code - cd.alias < CARD_ARTWORK_VERSIONS_OFFSET))
		tmp_code = cd.alias;
	stName->setText(fmt::format(L"{}[{:08}]", dataManager.GetName(tmp_code), tmp_code).c_str());
	stSetName->setText(L"");
	if(!gameConf.chkHideSetname) {
		unsigned long long sc = cd.setcode;
		if(cd.alias) {
			auto aptr = dataManager._datas.find(cd.alias);
			if(aptr != dataManager._datas.end())
				sc = aptr->second->setcode;
		}
		if(sc) {
			stSetName->setText((dataManager.GetSysString(1329) + dataManager.FormatSetName(sc)).c_str());
		}
	}
	std::wstring text = L"";
	if(cd.type & TYPE_MONSTER) {
		stInfo->setText(fmt::format(L"[{}] {}/{}", dataManager.FormatType(cd.type), dataManager.FormatRace(cd.race), dataManager.FormatAttribute(cd.attribute)).c_str());
		if(cd.type & TYPE_LINK){
			if(cd.attack < 0)
				text.append(L"?/Link ").append(fmt::format(L"{}	", cd.level));
			else
				text.append(fmt::format(L"{}/Link {}	", cd.attack, cd.level));
			text.append(dataManager.FormatLinkMarker(cd.link_marker));
		} else {
			text.append(fmt::format(L"[{}{}] ", (cd.type & TYPE_XYZ) ? L"\u2606" : L"\u2605", cd.level));
			if (cd.attack < 0 && cd.defense < 0)
				text.append(L"?/?");
			else if (cd.attack < 0)
				text.append(fmt::format(L"?/{}", cd.defense));
			else if (cd.defense < 0)
				text.append(fmt::format(L"{}/?", cd.attack));
			else
				text.append(fmt::format(L"{}/{}", cd.attack, cd.defense));
		}
		if(cd.type & TYPE_PENDULUM) {
			text.append(fmt::format(L"   {}/{}", cd.lscale, cd.rscale));
		}
		stDataInfo->setText(text.c_str());
	} else {
		stInfo->setText(fmt::format(L"[{}]", dataManager.FormatType(cd.type)).c_str());
		if(cd.type & TYPE_LINK) {
			stDataInfo->setText(fmt::format(L"Link {}", cd.level, dataManager.FormatLinkMarker(cd.link_marker)).c_str());
		} else
			stDataInfo->setText(L"");
	}
	int offset = Scale(37);
	stInfo->setRelativePosition(recti(Scale(15), offset, Scale(287 * window_scale.X), offset + stInfo->getTextHeight()));
	offset += stInfo->getTextHeight();
	if(wcscmp(stDataInfo->getText(), L"")) {
		stDataInfo->setRelativePosition(recti(Scale(15), offset, Scale(287 * window_scale.X), offset + stDataInfo->getTextHeight()));
		offset += stDataInfo->getTextHeight();
	}
	if(wcscmp(stSetName->getText(), L"")) {
		stSetName->setRelativePosition(recti(Scale(15), offset, Scale(287 * window_scale.X), offset + stSetName->getTextHeight()));
		offset += stSetName->getTextHeight();
	}
	stText->setRelativePosition(recti(Scale(15), offset, Scale(287 * window_scale.X), Scale(324 * window_scale.Y)));
	stText->setText(dataManager.GetText(code).c_str());
}
void Game::ClearCardInfo(int player) {
	imgCard->setImage(imageManager.tCover[player]);
	stName->setText(L"");
	stInfo->setText(L"");
	stDataInfo->setText(L"");
	stSetName->setText(L"");
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
		soundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.hostname[player]);
	} else if(type == 1) {
		soundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
		chatMsg[0].append(dInfo.clientname[player]);
	} else if(type == 2) {
		switch(player) {
		case 7: //local name
			chatMsg[0].append(ebNickName->getText());
			break;
		case 8: //system custom message, no prefix.
			soundManager->PlaySoundEffect(SoundManager::SFX::CHAT);
			chatMsg[0].append(L"[System]");
			break;
		case 9: //error message
			chatMsg[0].append(L"[Script Error]");
			break;
		default: //from watcher or unknown
			if(player < 11 || player > 19)
				chatMsg[0].append(L"[---]");
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
		ErrorLog("[Script Error]: " + msg);
}
void Game::ErrorLog(const std::string& msg) {
	std::ofstream log("error.log", std::ofstream::app);
	if(!log.is_open())
		return;
	time_t nowtime = time(NULL);
	tm *localedtime = localtime(&nowtime);
	char timebuf[40];
	strftime(timebuf, 40, "%Y-%m-%d %H:%M:%S", localedtime);
	log << "[" << timebuf << "]" << msg << std::endl;
	log.close();
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
std::wstring Game::LocalName(int local_player) {
	return local_player == 0 ? dInfo.hostname[0] : dInfo.clientname[0];
}
void Game::UpdateDuelParam() {
	uint32 flag = 0, filter = 0x100;
	for (int i = 0; i < 6; ++i, filter <<= 1)
		if (chkCustomRules[i]->isChecked()) {
			flag |= filter;
		}
	uint32 limits[] = { TYPE_FUSION, TYPE_SYNCHRO, TYPE_XYZ, TYPE_PENDULUM, TYPE_LINK };
	uint32 flag2 = 0;
	for (int i = 0; i < 5; ++i)
		if (chkTypeLimit[i]->isChecked()) {
			flag2 |= limits[i];
		}
	cbDuelRule->clear();
	cbDuelRule->addItem(dataManager.GetSysString(1260).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1261).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1262).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1263).c_str());
	switch (flag) {
	case DUEL_MODE_MR1: {
		cbDuelRule->setSelected(0);
		if (flag2 == DUEL_MODE_MR1_FORB)
			break;
	}
	case DUEL_MODE_MR2: {
		cbDuelRule->setSelected(1);
		if (flag2 == DUEL_MODE_MR2_FORB)
			break;
	}
	case DUEL_MODE_MR3: {
		cbDuelRule->setSelected(2);
		if (flag2 == DUEL_MODE_MR3_FORB)
			break;
	}
	case DUEL_MODE_MR4: {
		cbDuelRule->setSelected(3);
		if (flag2 == DUEL_MODE_MR4_FORB)
			break;
	}
	default: {
		cbDuelRule->addItem(dataManager.GetSysString(1630).c_str());
		cbDuelRule->setSelected(4);
		break;
	}
	}
	duel_param = flag;
	forbiddentypes = flag2;
}
void Game::UpdateExtraRules() {
	for(int i = 0; i < 13; i++)
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
	for(int flag = 1, i = 0; i < 13; i++, flag = flag << 1) {
		if(chkRules[i]->isChecked())
			extra_rules |= flag;
	}
}
int Game::GetMasterRule(uint32 param, uint32 forbiddentypes, int* truerule) {
	if(truerule)
		*truerule = 0;
	switch(param) {
	case DUEL_MODE_MR1: {
		if (truerule)
			*truerule = 1;
		if (forbiddentypes == DUEL_MODE_MR1_FORB)
			return 1;
	}
	case DUEL_MODE_MR2: {
		if (truerule)
			*truerule = 2;
		if (forbiddentypes == DUEL_MODE_MR2_FORB)
			return 2;
	}
	case DUEL_MODE_MR3: {
		if (truerule)
			*truerule = 3;
		if (forbiddentypes == DUEL_MODE_MR3_FORB)
			return 3;
	}
	case DUEL_MODE_MR4: {
		if (truerule)
			*truerule = 4;
		if (forbiddentypes == DUEL_MODE_MR4_FORB)
			return 4;
	}
	default: {
		if (truerule && !*truerule)
			*truerule = 5;
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
void Game::SetMesageWindow() {
	if(is_building || dInfo.isInDuel) {
		wMessage->setRelativePosition(ResizeWin(490, 200, 840, 340));
		wACMessage->setRelativePosition(ResizeWin(490, 240, 840, 300));
	} else {
		SetCentered(wMessage);
		SetCentered(wACMessage);
		/*wMessage->setRelativePosition(ResizeWin(510 - 175, 200, 510 + 175, 340));
		wACMessage->setRelativePosition(ResizeWin(510 - 175, 240, 510 + 175, 300));*/
	}
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

	wLanWindow->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wCreateHost->setRelativePosition(ResizeWin(320, 100, 700, 520));
	if (dInfo.clientname.size() + dInfo.hostname.size()>=5) {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 500));
		wHostPrepare2->setRelativePosition(ResizeWin(750, 120, 950, 500));
	} else {
		wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750, 440));
		wHostPrepare2->setRelativePosition(ResizeWin(750, 120, 950, 440));
	}
	wRules->setRelativePosition(ResizeWin(630, 100, 1000, 310));
	wCustomRules->setRelativePosition(ResizeWin(700, 100, 910, 410));
	wReplay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wSinglePlay->setRelativePosition(ResizeWin(220, 100, 800, 520));

	wHand->setRelativePosition(ResizeWin(500, 450, 825, 605));
	wFTSelect->setRelativePosition(ResizeWin(550, 240, 780, 340));
	SetMesageWindow();
	//wACMessage->setRelativePosition(ResizeWin(490, 240, 840, 300));
	wQuery->setRelativePosition(ResizeWin(490, 200, 840, 340));
	auto pos = wOptions->getRelativePosition();
	wOptions->setRelativePosition(ResizeWin(490, 200, 490 + (pos.LowerRightCorner.X - pos.UpperLeftCorner.X), 200 + (pos.LowerRightCorner.Y - pos.UpperLeftCorner.Y)));
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
	}
	stName->setRelativePosition(Scale(10, 10, 287 * window_scale.X, 32));
	lstLog->setRelativePosition(Resize(10, 10, infosExpanded ? 1012 : 290, 290));
	lstChat->setRelativePosition(Resize(10, 10, infosExpanded ? 1012 : 290, 290));
	imageManager.ClearTexture(true);

	if(showingcard)
		ShowCardInfo(showingcard, true);
	btnClearLog->setRelativePosition(Resize(160, 300, 260, 325));
	btnExpandLog->setRelativePosition(Resize(40, 300, 140, 325));
	btnClearChat->setRelativePosition(Resize(160, 300, 260, 325));
	btnExpandChat->setRelativePosition(Resize(40, 300, 140, 325));
	tabSystem->setRelativePosition(Resize(0, 0, 300, 364));
	scrMusicVolume->setRelativePosition(rect<s32>(Scale(85), Scale(325), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(340)));
	scrSoundVolume->setRelativePosition(rect<s32>(Scale(85), Scale(355), std::min(tabSystem->getSubpanel()->getRelativePosition().getWidth() - 21, Scale(300)), Scale(370)));

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
		x = (x / window_scale.X) / gameConf.dpi_scale;
		y = (y / window_scale.Y) / gameConf.dpi_scale;
	} else {
		x = x * window_scale.X * gameConf.dpi_scale;
		y = y * window_scale.Y * gameConf.dpi_scale;
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
void Game::SetCentered(irr::gui::IGUIElement * elem) {
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
recti Game::ResizeWinFromCenter(s32 x, s32 y, s32 x2, s32 y2) {
	auto size = driver->getScreenSize();
	recti rect(0, 0, size.Width, size.Height);
	auto center = rect.getCenter();
	core::dimension2d<u32> sizes((x + x2) / 2, (y + y2) / 2);
	recti rect2(x, y, x2, y2);
	return recti((center.X - sizes.Width), center.Y - sizes.Height, center.X + sizes.Width, center.Y + sizes.Height);
}
void Game::ValidateName(irr::gui::IGUIElement* obj) {
	std::wstring text = obj->getText();
	const wchar_t chars[] = L"<>:\"/\\|?*";
	for(auto& forbid : chars)
		text.erase(std::remove(text.begin(), text.end(), forbid), text.end());
	obj->setText(text.c_str());
}
#define CHK_RNG(range_start, range_end) (c >= range_start && c <= range_end)
std::wstring Game::StringtoUpper(std::wstring input) {
	std::transform(input.begin(), input.end(), input.begin(), [](wchar_t c) {
		if(CHK_RNG(192, 197) || CHK_RNG(224, 229)) {
			return L'A';
		}
		if(CHK_RNG(192, 197) || CHK_RNG(224, 229)) {
			return L'E';
		}
		if(CHK_RNG(200, 203) || CHK_RNG(232, 235)) {
			return L'I';
		}
		if(CHK_RNG(210, 214) || CHK_RNG(242, 246)) {
			return L'O';
		}
		if(CHK_RNG(217, 220) || CHK_RNG(249, 252)) {
			return L'U';
		}
		if(c == 209 || c == 241) {
			return L'N';
		}
		return (wchar_t)::towupper(c);
	});
	return input;
}
#undef CHK_RNG
bool Game::CompareStrings(std::wstring input, const std::vector<std::wstring>& tokens, bool transform_input, bool transform_token) {
	if(input.empty() || tokens.empty())
		return false;
	if(transform_input)
		input = StringtoUpper(input);
	std::vector<std::wstring> alttokens;
	if(transform_token) {
		alttokens = tokens;
		for(auto& token : alttokens)
			token = StringtoUpper(token);
	}
	std::size_t pos;
	for(auto& token : transform_token ? alttokens : tokens) {
		if((pos = input.find(token)) == std::wstring::npos)
			return false;
		input = input.substr(pos + 1);
	}
	return true;
}
bool Game::CompareStrings(std::wstring input, std::wstring second_term, bool transform_input, bool transform_term) {
	if(input.empty() && !second_term.empty())
		return false;
	if(second_term.empty())
		return true;
	if(transform_input)
		input = StringtoUpper(input);
	if(transform_term)
		second_term = StringtoUpper(second_term);
	return input.find(second_term) != std::wstring::npos;
}
std::wstring Game::ReadPuzzleMessage(const std::wstring& script_name) {
	std::ifstream infile(Utils::ParseFilename(script_name), std::ifstream::in);
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
	path_string name = Utils::ParseFilename(_name);
	for(auto& path : script_dirs) {
		if(path == TEXT("archives")) {
			auto reader = Utils::FindandOpenFileFromArchives(TEXT("script"), name);
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
	opts.payload1 = &dataManager;
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
	static_cast<Game*>(payload)->AddDebugMsg(string);
	if(type > 1)
		printf("%s\n", string);
}
void Game::PopulateResourcesDirectories() {
	script_dirs.push_back(TEXT("./expansions/script/"));
	auto expansions_subdirs = Utils::FindSubfolders(TEXT("./expansions/script/"));
	script_dirs.insert(script_dirs.end(), expansions_subdirs.begin(), expansions_subdirs.end());
	script_dirs.push_back(TEXT("archives"));
	script_dirs.push_back(TEXT("./script/"));
	auto script_subdirs = Utils::FindSubfolders(TEXT("./script/"));
	script_dirs.insert(script_dirs.end(), script_subdirs.begin(), script_subdirs.end());
	pic_dirs.push_back(TEXT("./expansions/pics/"));
	pic_dirs.push_back(TEXT("archives"));
	pic_dirs.push_back(TEXT("./pics/"));
	cover_dirs.push_back(TEXT("./expansions/pics/cover/"));
	cover_dirs.push_back(TEXT("archives"));
	cover_dirs.push_back(TEXT("./pics/cover/"));
	field_dirs.push_back(TEXT("./expansions/pics/field/"));
	field_dirs.push_back(TEXT("archives"));
	field_dirs.push_back(TEXT("./pics/field/"));
}


}
