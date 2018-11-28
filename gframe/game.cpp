#include "config.h"
#include "game.h"
#include "image_manager.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "replay.h"
#include "materials.h"
#include "duelclient.h"
#include "netserver.h"
#include "single_mode.h"
#include <sstream>
#include <fstream>

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

unsigned short PRO_VERSION = 0x1342;

namespace ygo {

Game* mainGame;

bool Game::Initialize() {
	srand(time(0));
	LoadConfig();
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = gameConf.antialias;
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_
	if(gameConf.use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
#endif
		params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<u32>(1024, 640);
	if(gameConf.fullscreen) {
		IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
		params.WindowSize = nulldevice->getVideoModeList()->getDesktopResolution();
		nulldevice->drop();
		params.Fullscreen = true;
	}
	device = irr::createDeviceEx(params);
	if(!device) {
		ErrorLog("Failed to create Irrlicht Engine device!");
		return false;
	}
	filesystem = device->getFileSystem();
#ifdef _DEBUG
	auto logger = device->getLogger();
	logger->setLogLevel(ELL_ERROR);
#endif
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
	waitFrame = 0;
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
	for(int i = 0; i < 3; i++) {
		dInfo.clientname[i].reserve(20);
		dInfo.hostname[i].reserve(20);
	}
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
	LoadExpansionDB();
	if(!dataManager.LoadDB("cards.cdb")) {
		ErrorLog("Failed to load card database (cards.cdb)!");
		return false;
	}
	if(!dataManager.LoadStrings("strings.conf")) {
		ErrorLog("Failed to load strings!");
		return false;
	}
	PopulateResourcesDirectories();
	dataManager.LoadStrings("./expansions/strings.conf");
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), 16);
	adFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), 12);
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont.c_str(), 48);
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.textfont.c_str(), gameConf.textfontsize);
	textFont = guiFont;
	if(!numFont || !textFont) {
		ErrorLog("Failed to load font(s)!");
		return false;
	}
	smgr = device->getSceneManager();
	device->setWindowCaption(L"EDOPro");
	device->setResizable(true);
#ifdef _WIN32
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON hBigIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	HWND hWnd;
	irr::video::SExposedVideoData exposedData = driver->getExposedVideoData();
	if(gameConf.use_d3d)
		hWnd = reinterpret_cast<HWND>(exposedData.D3D9.HWnd);
	else
		hWnd = reinterpret_cast<HWND>(exposedData.OpenGLWin32.HWnd);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (long)hSmallIcon);
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (long)hBigIcon);
#endif
	//main menu
	wMainMenu = env->addWindow(rect<s32>(370, 200, 650, 415), false, fmt::format(L"EDOPro Version:{:X}.0{:X}.{:X}", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf).c_str());
	wMainMenu->getCloseButton()->setVisible(false);
	btnLanMode = env->addButton(rect<s32>(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200).c_str());
	btnSingleMode = env->addButton(rect<s32>(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201).c_str());
	btnReplayMode = env->addButton(rect<s32>(10, 100, 270, 130), wMainMenu, BUTTON_REPLAY_MODE, dataManager.GetSysString(1202).c_str());
//	btnTestMode = env->addButton(rect<s32>(10, 135, 270, 165), wMainMenu, BUTTON_TEST_MODE, dataManager.GetSysString(1203).c_str());
	btnDeckEdit = env->addButton(rect<s32>(10, 135, 270, 165), wMainMenu, BUTTON_DECK_EDIT, dataManager.GetSysString(1204).c_str());
	btnModeExit = env->addButton(rect<s32>(10, 170, 270, 200), wMainMenu, BUTTON_MODE_EXIT, dataManager.GetSysString(1210).c_str());
	//lan mode
	wLanWindow = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1200).c_str());
	wLanWindow->getCloseButton()->setVisible(false);
	wLanWindow->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1220).c_str(), rect<s32>(10, 30, 220, 50), false, false, wLanWindow);
	ebNickName = env->addEditBox(gameConf.nickname.c_str(), rect<s32>(110, 25, 450, 50), true, wLanWindow);
	ebNickName->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	lstHostList = env->addListBox(rect<s32>(10, 60, 570, 320), wLanWindow, LISTBOX_LAN_HOST, true);
	lstHostList->setItemHeight(18);
	btnLanRefresh = env->addButton(rect<s32>(240, 325, 340, 350), wLanWindow, BUTTON_LAN_REFRESH, dataManager.GetSysString(1217).c_str());
	env->addStaticText(dataManager.GetSysString(1221).c_str(), rect<s32>(10, 360, 220, 380), false, false, wLanWindow);
	ebJoinHost = env->addEditBox(gameConf.lasthost.c_str(), rect<s32>(110, 355, 350, 380), true, wLanWindow);
	ebJoinHost->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(gameConf.lastport.c_str(), rect<s32>(360, 355, 420, 380), true, wLanWindow, EDITBOX_PORT_BOX);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1222).c_str(), rect<s32>(10, 390, 220, 410), false, false, wLanWindow);
	ebJoinPass = env->addEditBox(gameConf.roompass.c_str(), rect<s32>(110, 385, 420, 410), true, wLanWindow);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnJoinHost = env->addButton(rect<s32>(460, 355, 570, 380), wLanWindow, BUTTON_JOIN_HOST, dataManager.GetSysString(1223).c_str());
	btnJoinCancel = env->addButton(rect<s32>(460, 385, 570, 410), wLanWindow, BUTTON_JOIN_CANCEL, dataManager.GetSysString(1212).c_str());
	btnCreateHost = env->addButton(rect<s32>(460, 25, 570, 50), wLanWindow, BUTTON_CREATE_HOST, dataManager.GetSysString(1224).c_str());
	//create host
	wCreateHost = env->addWindow(rect<s32>(320, 100, 700, 520), false, dataManager.GetSysString(1224).c_str());
	wCreateHost->getCloseButton()->setVisible(false);
	wCreateHost->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1226).c_str(), rect<s32>(20, 30, 220, 50), false, false, wCreateHost);
	cbLFlist = env->addComboBox(rect<s32>(140, 25, 300, 50), wCreateHost);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbLFlist->addItem(deckManager._lfList[i].listName.c_str(), deckManager._lfList[i].hash);
	env->addStaticText(dataManager.GetSysString(1225).c_str(), rect<s32>(20, 60, 220, 80), false, false, wCreateHost);
	cbRule = env->addComboBox(rect<s32>(140, 55, 300, 80), wCreateHost);
	cbRule->addItem(dataManager.GetSysString(1240).c_str());
	cbRule->addItem(dataManager.GetSysString(1241).c_str());
	cbRule->addItem(dataManager.GetSysString(1242).c_str());
	cbRule->addItem(dataManager.GetSysString(1243).c_str());
	env->addStaticText(dataManager.GetSysString(1227).c_str(), rect<s32>(20, 90, 220, 110), false, false, wCreateHost);
	cbMatchMode = env->addComboBox(rect<s32>(140, 85, 300, 110), wCreateHost);
	cbMatchMode->addItem(dataManager.GetSysString(1244).c_str());
	cbMatchMode->addItem(dataManager.GetSysString(1245).c_str());
	cbMatchMode->addItem(dataManager.GetSysString(1246).c_str());
	cbMatchMode->addItem(dataManager.GetSysString(1247).c_str());
	env->addStaticText(dataManager.GetSysString(1237).c_str(), rect<s32>(20, 120, 320, 140), false, false, wCreateHost);
	ebTimeLimit = env->addEditBox(L"180", rect<s32>(140, 115, 220, 140), true, wCreateHost);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1228).c_str(), rect<s32>(20, 150, 320, 170), false, false, wCreateHost);
	btnRuleCards = env->addButton(rect<s32>(260, 325, 370, 350), wCreateHost, BUTTON_RULE_CARDS, dataManager.GetSysString(1625).c_str());
	wRules = env->addWindow(rect<s32>(630, 100, 1000, 310), false, L"");
	wRules->getCloseButton()->setVisible(false);
	wRules->setDrawTitlebar(false);
	wRules->setDraggable(true);
	wRules->setVisible(false);
	btnRulesOK = env->addButton(rect<s32>(135, 175, 235, 200), wRules, BUTTON_RULE_OK, dataManager.GetSysString(1211).c_str());
	for(int i = 0; i < 13; ++i)
		chkRules[i] = env->addCheckBox(false, recti(10 + (i % 2) * 150, 10 + (i / 2) * 20, 200 + (i % 2) * 120, 30 + (i / 2) * 20), wRules, CHECKBOX_EXTRA_RULE, dataManager.GetSysString(1132 + i).c_str());
	mainGame->extra_rules = 0;
	env->addStaticText(dataManager.GetSysString(1236).c_str(), rect<s32>(20, 180, 220, 200), false, false, wCreateHost);
	cbDuelRule = env->addComboBox(rect<s32>(140, 175, 300, 200), wCreateHost, COMBOBOX_DUEL_RULE);
	cbDuelRule->addItem(dataManager.GetSysString(1260).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1261).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1262).c_str());
	cbDuelRule->addItem(dataManager.GetSysString(1263).c_str());
	cbDuelRule->setSelected(DEFAULT_DUEL_RULE - 1);
	btnCustomRule = env->addButton(rect<s32>(305, 175, 370, 200), wCreateHost, BUTTON_CUSTOM_RULE, dataManager.GetSysString(1626).c_str());
	wCustomRules = env->addWindow(rect<s32>(700, 100, 910, 410), false, L"");
	wCustomRules->getCloseButton()->setVisible(false);
	wCustomRules->setDrawTitlebar(false);
	wCustomRules->setDraggable(true);
	wCustomRules->setVisible(false);
	int spacing = 0;
	env->addStaticText(dataManager.GetSysString(1629).c_str(), rect<s32>(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	spacing++;
	for(int i = 0; i < 6; ++i, ++spacing)
		chkCustomRules[i] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 390 + i, dataManager.GetSysString(1631 + i).c_str());
	env->addStaticText(dataManager.GetSysString(1628).c_str(), rect<s32>(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	spacing++;
	chkTypeLimit[0] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1056)).c_str());
	spacing++;
	chkTypeLimit[1] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1063)).c_str());
	spacing++;
	chkTypeLimit[2] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1073)).c_str());
	spacing++;
	chkTypeLimit[3] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1074)).c_str());
	spacing++;
	chkTypeLimit[4] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 353 + spacing, fmt::sprintf(dataManager.GetSysString(1627), dataManager.GetSysString(1076)).c_str());
	btnCustomRulesOK = env->addButton(rect<s32>(55, 270, 155, 295), wCustomRules, BUTTON_CUSTOM_RULE_OK, dataManager.GetSysString(1211).c_str());
	forbiddentypes = MASTER_RULE_4_FORB;
	duel_param = MASTER_RULE_4;
	chkNoCheckDeck = env->addCheckBox(false, rect<s32>(20, 210, 170, 230), wCreateHost, -1, dataManager.GetSysString(1229).c_str());
	chkNoShuffleDeck = env->addCheckBox(false, rect<s32>(180, 210, 360, 230), wCreateHost, -1, dataManager.GetSysString(1230).c_str());
	env->addStaticText(dataManager.GetSysString(1231).c_str(), rect<s32>(20, 240, 320, 260), false, false, wCreateHost);
	ebStartLP = env->addEditBox(L"8000", rect<s32>(140, 235, 220, 260), true, wCreateHost);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1232).c_str(), rect<s32>(20, 270, 320, 290), false, false, wCreateHost);
	ebStartHand = env->addEditBox(L"5", rect<s32>(140, 265, 220, 290), true, wCreateHost);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1233).c_str(), rect<s32>(20, 300, 320, 320), false, false, wCreateHost);
	ebDrawCount = env->addEditBox(L"1", rect<s32>(140, 295, 220, 320), true, wCreateHost);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1234).c_str(), rect<s32>(10, 330, 220, 350), false, false, wCreateHost);
	ebServerName = env->addEditBox(gameConf.gamename.c_str(), rect<s32>(110, 325, 250, 350), true, wCreateHost);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1235).c_str(), rect<s32>(10, 360, 220, 380), false, false, wCreateHost);
	ebServerPass = env->addEditBox(L"", rect<s32>(110, 355, 250, 380), true, wCreateHost);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnHostConfirm = env->addButton(rect<s32>(260, 355, 370, 380), wCreateHost, BUTTON_HOST_CONFIRM, dataManager.GetSysString(1211).c_str());
	btnHostCancel = env->addButton(rect<s32>(260, 385, 370, 410), wCreateHost, BUTTON_HOST_CANCEL, dataManager.GetSysString(1212).c_str());
	env->addStaticText(dataManager.GetSysString(1238).c_str(), rect<s32>(10, 390, 220, 410), false, false, wCreateHost);
	ebHostPort = env->addEditBox(gameConf.serverport.c_str(), rect<s32>(110, 385, 250, 410), true, wCreateHost, EDITBOX_PORT_BOX);
	ebHostPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//host(single)
	wHostPrepare = env->addWindow(rect<s32>(270, 120, 750, 440), false, dataManager.GetSysString(1250).c_str());
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	wHostPrepare2 = env->addWindow(rect<s32>(750, 120, 950, 440), false, dataManager.GetSysString(1625).c_str());
	wHostPrepare2->getCloseButton()->setVisible(false);
	wHostPrepare2->setVisible(false);
	stHostPrepRule2 = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare2, -1, rect<s32>(10, 30, 460, 350));
	stHostPrepRule2->setWordWrap(true);
	btnHostPrepDuelist = env->addButton(rect<s32>(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, dataManager.GetSysString(1251).c_str());
	for(int i = 0; i < 6; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	btnHostPrepOB = env->addButton(rect<s32>(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252).c_str());
	stHostPrepOB = env->addStaticText((dataManager.GetSysString(1253) + L"0").c_str(), rect<s32>(10, 210, 270, 230), false, false, wHostPrepare);
	stHostPrepRule = irr::gui::CGUICustomText::addCustomText(L"", false, env, wHostPrepare, -1, rect<s32>(280, 30, 460, 230));
	stHostPrepRule->setWordWrap(true);
	stDeckSelect = env->addStaticText(dataManager.GetSysString(1254).c_str(), rect<s32>(10, 235, 110, 255), false, false, wHostPrepare);
	cbDeckSelect = env->addComboBox(rect<s32>(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	cbDeckSelect2 = env->addComboBox(rect<s32>(280, 230, 430, 255), wHostPrepare);
	cbDeckSelect2->setMaxSelectionRows(10);
	btnHostPrepReady = env->addButton(rect<s32>(170, 180, 270, 205), wHostPrepare, BUTTON_HP_READY, dataManager.GetSysString(1218).c_str());
	btnHostPrepNotReady = env->addButton(rect<s32>(170, 180, 270, 205), wHostPrepare, BUTTON_HP_NOTREADY, dataManager.GetSysString(1219).c_str());
	btnHostPrepNotReady->setVisible(false);
	btnHostPrepStart = env->addButton(rect<s32>(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, dataManager.GetSysString(1215).c_str());
	btnHostPrepCancel = env->addButton(rect<s32>(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, dataManager.GetSysString(1210).c_str());
	//img
	wCardImg = env->addStaticText(L"", rect<s32>(1, 1, 1 + CARD_IMG_WIDTH + 20, 1 + CARD_IMG_HEIGHT + 18), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(0xc0c0c0c0);
	wCardImg->setVisible(false);
	imgCard = env->addImage(rect<s32>(10, 9, 10 + CARD_IMG_WIDTH, 9 + CARD_IMG_HEIGHT), wCardImg);
	imgCard->setImage(imageManager.tCover[0]);
	imgCard->setScaleImage(true);
	imgCard->setUseAlphaChannel(true);
	//phase
	wPhase = env->addStaticText(L"", rect<s32>(480, 310, 855, 330));
	wPhase->setVisible(false);
	btnDP = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, -1, L"\xff24\xff30");
	btnDP->setEnabled(false);
	btnDP->setPressed(true);
	btnDP->setVisible(false);
	btnSP = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, -1, L"\xff33\xff30");
	btnSP->setEnabled(false);
	btnSP->setPressed(true);
	btnSP->setVisible(false);
	btnM1 = env->addButton(rect<s32>(160, 0, 210, 20), wPhase, -1, L"\xff2d\xff11");
	btnM1->setEnabled(false);
	btnM1->setPressed(true);
	btnM1->setVisible(false);
	btnBP = env->addButton(rect<s32>(160, 0, 210, 20), wPhase, BUTTON_BP, L"\xff22\xff30");
	btnBP->setVisible(false);
	btnM2 = env->addButton(rect<s32>(160, 0, 210, 20), wPhase, BUTTON_M2, L"\xff2d\xff12");
	btnM2->setVisible(false);
	btnEP = env->addButton(rect<s32>(320, 0, 370, 20), wPhase, BUTTON_EP, L"\xff25\xff30");
	btnEP->setVisible(false);
	//tab
	wInfos = env->addTabControl(rect<s32>(1, 275, 301, 639), 0, true);
	wInfos->setVisible(false);
	//info
	irr::gui::IGUITab* tabInfo = wInfos->addTab(dataManager.GetSysString(1270).c_str());
	stName = irr::gui::CGUICustomText::addCustomText(L"", true, env, tabInfo, -1, rect<s32>(10, 10, 287, 32));
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	((CGUICustomText*)stName)->setTextAutoScrolling(irr::gui::CGUICustomText::LEFT_TO_RIGHT_BOUNCING, 0, 1.0f, 0, 120, 300);
	stInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, rect<s32>(15, 37, 287, 60));
	stInfo->setWordWrap(true);
	stInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stDataInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, rect<s32>(15, 60, 287, 83));
	stDataInfo->setWordWrap(true);
	stDataInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stSetName = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, rect<s32>(15, 83, 287, 106));
	stSetName->setWordWrap(true);
	stSetName->setOverrideColor(SColor(255, 0, 0, 255));
	stText = irr::gui::CGUICustomText::addCustomText(L"", false, env, tabInfo, -1, rect<s32>(15, 106, 287, 324));
	((CGUICustomText*)stText)->enableScrollBar(0, 0.07f);
	stText->setWordWrap(true);
	//log
	irr::gui::IGUITab* tabLog =  wInfos->addTab(dataManager.GetSysString(1271).c_str());
	lstLog = env->addListBox(rect<s32>(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(18);
	btnClearLog = env->addButton(rect<s32>(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, dataManager.GetSysString(1272).c_str());
	//system
	irr::gui::IGUITab* tabSystem = wInfos->addTab(dataManager.GetSysString(1273).c_str());
	chkMAutoPos = env->addCheckBox(false, rect<s32>(20, 20, 280, 45), tabSystem, -1, dataManager.GetSysString(1274).c_str());
	chkMAutoPos->setChecked(gameConf.chkMAutoPos != 0);
	chkSTAutoPos = env->addCheckBox(false, rect<s32>(20, 50, 280, 75), tabSystem, -1, dataManager.GetSysString(1278).c_str());
	chkSTAutoPos->setChecked(gameConf.chkSTAutoPos != 0);
	chkRandomPos = env->addCheckBox(false, rect<s32>(40, 80, 300, 105), tabSystem, -1, dataManager.GetSysString(1275).c_str());
	chkRandomPos->setChecked(gameConf.chkRandomPos != 0);
	chkAutoChain = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, -1, dataManager.GetSysString(1276).c_str());
	chkAutoChain->setChecked(gameConf.chkAutoChain != 0);
	chkWaitChain = env->addCheckBox(false, rect<s32>(20, 140, 280, 165), tabSystem, -1, dataManager.GetSysString(1277).c_str());
	chkWaitChain->setChecked(gameConf.chkWaitChain != 0);
	chkHideHintButton = env->addCheckBox(false, rect<s32>(20, 170, 280, 195), tabSystem, -1, dataManager.GetSysString(1355).c_str());
	chkHideHintButton->setChecked(gameConf.chkHideHintButton != 0);
	chkIgnore1 = env->addCheckBox(false, rect<s32>(20, 200, 280, 225), tabSystem, -1, dataManager.GetSysString(1290).c_str());
	chkIgnore1->setChecked(gameConf.chkIgnore1 != 0);
	chkIgnore2 = env->addCheckBox(false, rect<s32>(20, 230, 280, 255), tabSystem, -1, dataManager.GetSysString(1291).c_str());
	chkIgnore2->setChecked(gameConf.chkIgnore2 != 0);
	chkEnableSound = env->addCheckBox(gameConf.enablesound, rect<s32>(140, 260, 300, 285), tabSystem, -1, dataManager.GetSysString(2046).c_str());
	chkEnableSound->setChecked(gameConf.enablesound);
	chkEnableMusic = env->addCheckBox(gameConf.enablemusic, rect<s32>(20, 260, 140, 285), tabSystem, CHECKBOX_ENABLE_MUSIC, dataManager.GetSysString(2047).c_str());
	chkEnableMusic->setChecked(gameConf.enablemusic);
	stVolume = env->addStaticText(L"Volume", rect<s32>(20, 290, 80, 310), false, true, tabSystem, -1, false);
	srcVolume = env->addScrollBar(true, rect<s32>(85, 295, 280, 310), tabSystem, SCROLL_VOLUME);
	srcVolume->setMax(100);
	srcVolume->setMin(0);
	srcVolume->setPos(gameConf.volume * 100);
	srcVolume->setLargeStep(1);
	srcVolume->setSmallStep(1);
	chkQuickAnimation = env->addCheckBox(false, rect<s32>(20, 315, 280, 340), tabSystem, CHECKBOX_QUICK_ANIMATION, dataManager.GetSysString(1299).c_str());
	chkQuickAnimation->setChecked(gameConf.quick_animation != 0);
	//
	wHand = env->addWindow(rect<s32>(500, 450, 825, 605), false, L"");
	wHand->getCloseButton()->setVisible(false);
	wHand->setDraggable(false);
	wHand->setDrawTitlebar(false);
	wHand->setVisible(false);
	for(int i = 0; i < 3; ++i) {
		btnHand[i] = env->addButton(rect<s32>(10 + 105 * i, 10, 105 + 105 * i, 144), wHand, BUTTON_HAND1 + i, L"");
		btnHand[i]->setImage(imageManager.tHand[i]);
	}
	//
	wFTSelect = env->addWindow(rect<s32>(550, 240, 780, 340), false, L"");
	wFTSelect->getCloseButton()->setVisible(false);
	wFTSelect->setVisible(false);
	btnFirst = env->addButton(rect<s32>(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, dataManager.GetSysString(100).c_str());
	btnSecond = env->addButton(rect<s32>(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, dataManager.GetSysString(101).c_str());
	//message (310)
	wMessage = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(1216).c_str());
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wMessage, -1, rect<s32>(20, 20, 350, 100));
	stMessage->setWordWrap(true);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(rect<s32>(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, dataManager.GetSysString(1211).c_str());
	//auto fade message (310)
	wACMessage = env->addWindow(rect<s32>(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = irr::gui::CGUICustomText::addCustomText(L"", true, env, wACMessage, -1, rect<s32>(0, 0, 350, 60), true);
	stACMessage->setWordWrap(true);
	stACMessage->setBackgroundColor(0xc0c0c0ff);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(560).c_str());
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage = irr::gui::CGUICustomText::addCustomText(L"", false, env, wQuery, -1, rect<s32>(20, 20, 350, 100));
	stQMessage->setWordWrap(true);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(rect<s32>(100, 105, 150, 130), wQuery, BUTTON_YES, dataManager.GetSysString(1213).c_str());
	btnNo = env->addButton(rect<s32>(200, 105, 250, 130), wQuery, BUTTON_NO, dataManager.GetSysString(1214).c_str());
	//options (310)
	wOptions = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions = irr::gui::CGUICustomText::addCustomText(L"", false, env, wOptions, -1, rect<s32>(20, 20, 350, 100));
	stOptions->setWordWrap(true);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(rect<s32>(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, dataManager.GetSysString(1211).c_str());
	btnOptionp = env->addButton(rect<s32>(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, L"<<<");
	btnOptionn = env->addButton(rect<s32>(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, L">>>");
	for(int i = 0; i < 5; ++i) {
		btnOption[i] = env->addButton(rect<s32>(10, 30 + 40 * i, 340, 60 + 40 * i), wOptions, BUTTON_OPTION_0 + i, L"");
	}
	scrOption = env->addScrollBar(false, rect<s32>(350, 30, 365, 220), wOptions, SCROLL_OPTION_SELECT);
	scrOption->setLargeStep(1);
	scrOption->setSmallStep(1);
	mainGame->scrOption->setMin(0);
	//pos select
	wPosSelect = env->addWindow(rect<s32>(340, 200, 935, 410), false, dataManager.GetSysString(561).c_str());
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	btnPSAU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(10, 45, 150, 185), wPosSelect, BUTTON_POS_AU);
	btnPSAU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(155, 45, 295, 185), wPosSelect, BUTTON_POS_AD);
	btnPSAD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD->setImage(imageManager.tCover[0], rect<s32>(0, 0, CARD_IMG_WIDTH, CARD_IMG_HEIGHT));
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDD->setImageRotation(270);
	btnPSDD->setImage(imageManager.tCover[0], rect<s32>(0, 0, CARD_IMG_WIDTH, CARD_IMG_HEIGHT));
	//card select
	wCardSelect = env->addWindow(rect<s32>(320, 100, 1000, 400), false, L"");
	wCardSelect->getCloseButton()->setVisible(false);
	wCardSelect->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stCardPos[i] = env->addStaticText(L"", rect<s32>(30 + 125 * i, 30, 150 + 125 * i, 50), true, false, wCardSelect, -1, true);
		stCardPos[i]->setBackgroundColor(0xffffffff);
		stCardPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardSelect[i] = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(30 + 125 * i, 55, 150 + 125 * i, 225), wCardSelect, BUTTON_CARD_0 + i);
		btnCardSelect[i]->setImageScale(core::vector2df(0.6f, 0.6f));
	}
	scrCardList = env->addScrollBar(true, rect<s32>(30, 235, 650, 255), wCardSelect, SCROLL_CARD_SELECT);
	btnSelectOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, dataManager.GetSysString(1211).c_str());
	//card display
	wCardDisplay = env->addWindow(rect<s32>(320, 100, 1000, 400), false, L"");
	wCardDisplay->getCloseButton()->setVisible(false);
	wCardDisplay->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stDisplayPos[i] = env->addStaticText(L"", rect<s32>(30 + 125 * i, 30, 150 + 125 * i, 50), true, false, wCardDisplay, -1, true);
		stDisplayPos[i]->setBackgroundColor(0xffffffff);
		stDisplayPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardDisplay[i] = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(30 + 125 * i, 55, 150 + 125 * i, 225), wCardDisplay, BUTTON_DISPLAY_0 + i);
		btnCardDisplay[i]->setImageScale(core::vector2df(0.6f, 0.6f));
	}
	scrDisplayList = env->addScrollBar(true, rect<s32>(30, 235, 650, 255), wCardDisplay, SCROLL_CARD_DISPLAY);
	btnDisplayOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, dataManager.GetSysString(1211).c_str());
	//announce number
	wANNumber = env->addWindow(rect<s32>(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(rect<s32>(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(rect<s32>(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, dataManager.GetSysString(1211).c_str());
	//announce card
	wANCard = env->addWindow(rect<s32>(430, 170, 840, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", rect<s32>(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(rect<s32>(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(rect<s32>(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, dataManager.GetSysString(1211).c_str());
	//announce attribute
	wANAttribute = env->addWindow(rect<s32>(500, 200, 830, 285), false, dataManager.GetSysString(562).c_str());
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, dataManager.FormatAttribute(filter).c_str());
	//announce race
	wANRace = env->addWindow(rect<s32>(480, 200, 850, 410), false, dataManager.GetSysString(563).c_str());
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 25; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, dataManager.FormatRace(filter).c_str());
	//selection hint
	stHintMsg = env->addStaticText(L"", rect<s32>(500, 60, 820, 90), true, false, 0, -1, false);
	stHintMsg->setBackgroundColor(0xc0ffffff);
	stHintMsg->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHintMsg->setVisible(false);
	//cmd menu
	wCmdMenu = env->addWindow(rect<s32>(10, 10, 110, 179), false, L"");
	wCmdMenu->setDrawTitlebar(false);
	wCmdMenu->setVisible(false);
	wCmdMenu->getCloseButton()->setVisible(false);
	btnActivate = env->addButton(rect<s32>(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1150).c_str());
	btnSummon = env->addButton(rect<s32>(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, dataManager.GetSysString(1151).c_str());
	btnSPSummon = env->addButton(rect<s32>(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, dataManager.GetSysString(1152).c_str());
	btnMSet = env->addButton(rect<s32>(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, dataManager.GetSysString(1153).c_str());
	btnSSet = env->addButton(rect<s32>(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, dataManager.GetSysString(1153).c_str());
	btnRepos = env->addButton(rect<s32>(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, dataManager.GetSysString(1154).c_str());
	btnAttack = env->addButton(rect<s32>(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, dataManager.GetSysString(1157).c_str());
	btnShowList = env->addButton(rect<s32>(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, dataManager.GetSysString(1158).c_str());
	btnOperation = env->addButton(rect<s32>(1, 169, 99, 189), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1161).c_str());
	btnReset = env->addButton(rect<s32>(1, 190, 99, 210), wCmdMenu, BUTTON_CMD_RESET, dataManager.GetSysString(1162).c_str());
	//deck edit
	wDeckEdit = env->addStaticText(L"", rect<s32>(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	stBanlist = env->addStaticText(dataManager.GetSysString(1300).c_str(), rect<s32>(10, 9, 100, 29), false, false, wDeckEdit);
	cbDBLFList = env->addComboBox(rect<s32>(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	stDeck = env->addStaticText(dataManager.GetSysString(1301).c_str(), rect<s32>(10, 39, 100, 59), false, false, wDeckEdit);
	cbDBDecks = env->addComboBox(rect<s32>(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbDBLFList->addItem(deckManager._lfList[i].listName.c_str());
	btnSaveDeck = env->addButton(rect<s32>(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, dataManager.GetSysString(1302).c_str());
	ebDeckname = env->addEditBox(L"", rect<s32>(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECK_NAME);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(rect<s32>(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, dataManager.GetSysString(1303).c_str());
	btnDeleteDeck = env->addButton(rect<s32>(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, dataManager.GetSysString(1308).c_str());
	btnShuffleDeck = env->addButton(rect<s32>(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307).c_str());
	btnSortDeck = env->addButton(rect<s32>(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, dataManager.GetSysString(1305).c_str());
	btnClearDeck = env->addButton(rect<s32>(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, dataManager.GetSysString(1304).c_str());
	btnSideOK = env->addButton(rect<s32>(510, 40, 820, 80), 0, BUTTON_SIDE_OK, dataManager.GetSysString(1334).c_str());
	btnSideOK->setVisible(false);
	btnSideShuffle = env->addButton(rect<s32>(310, 100, 370, 130), 0, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307).c_str());
	btnSideShuffle->setVisible(false);
	btnSideSort = env->addButton(rect<s32>(375, 100, 435, 130), 0, BUTTON_SORT_DECK, dataManager.GetSysString(1305).c_str());
	btnSideSort->setVisible(false);
	btnSideReload = env->addButton(rect<s32>(440, 100, 500, 130), 0, BUTTON_SIDE_RELOAD, dataManager.GetSysString(1309).c_str());
	btnSideReload->setVisible(false);
	//
	scrFilter = env->addScrollBar(false, recti(999, 161, 1019, 629), 0, SCROLL_FILTER);
	scrFilter->setLargeStep(100);
	scrFilter->setSmallStep(10);
	scrFilter->setVisible(false);
	//sort type
	wSort = env->addStaticText(L"", rect<s32>(930, 132, 1020, 156), true, false, 0, -1, true);
	cbSortType = env->addComboBox(rect<s32>(10, 2, 85, 22), wSort, COMBOBOX_SORTTYPE);
	cbSortType->setMaxSelectionRows(10);
	for(int i = 1370; i <= 1373; i++)
		cbSortType->addItem(dataManager.GetSysString(i).c_str());
	wSort->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", rect<s32>(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	stCategory = env->addStaticText(dataManager.GetSysString(1311).c_str(), rect<s32>(10, 5, 70, 25), false, false, wFilter);
	cbCardType = env->addComboBox(rect<s32>(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(dataManager.GetSysString(1310).c_str());
	cbCardType->addItem(dataManager.GetSysString(1312).c_str());
	cbCardType->addItem(dataManager.GetSysString(1313).c_str());
	cbCardType->addItem(dataManager.GetSysString(1314).c_str());
	cbCardType2 = env->addComboBox(rect<s32>(130, 3, 190, 23), wFilter, COMBOBOX_SECONDTYPE);
	cbCardType2->setMaxSelectionRows(20);
	cbCardType2->addItem(dataManager.GetSysString(1310).c_str(), 0);
	chkAnime = env->addCheckBox(false, recti(10, 96, 150, 118), wFilter, CHECKBOX_SHOW_ANIME, dataManager.GetSysString(1999).c_str());
	chkAnime->setChecked(gameConf.chkAnime != 0);
	stLimit = env->addStaticText(dataManager.GetSysString(1315).c_str(), rect<s32>(205, 5, 280, 25), false, false, wFilter);
	cbLimit = env->addComboBox(rect<s32>(260, 3, 390, 23), wFilter, COMBOBOX_OTHER_FILT);
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
		cbLimit->addItem(dataManager.GetSysString(1243).c_str());
		cbLimit->addItem(L"Illegal");
		cbLimit->addItem(L"VG");
		cbLimit->addItem(L"Custom");
	}
	stAttribute = env->addStaticText(dataManager.GetSysString(1319).c_str(), rect<s32>(10, 28, 70, 48), false, false, wFilter);
	cbAttribute = env->addComboBox(rect<s32>(60, 26, 190, 46), wFilter, COMBOBOX_OTHER_FILT);
	cbAttribute->setMaxSelectionRows(10);
	cbAttribute->addItem(dataManager.GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(dataManager.FormatAttribute(filter).c_str(), filter);
	stRace = env->addStaticText(dataManager.GetSysString(1321).c_str(), rect<s32>(10, 51, 70, 71), false, false, wFilter);
	cbRace = env->addComboBox(rect<s32>(60, 49, 190, 69), wFilter, COMBOBOX_OTHER_FILT);
	cbRace->setMaxSelectionRows(10);
	cbRace->addItem(dataManager.GetSysString(1310).c_str(), 0);
	for(int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(dataManager.FormatRace(filter).c_str(), filter);
	stAttack = env->addStaticText(dataManager.GetSysString(1322).c_str(), rect<s32>(205, 28, 280, 48), false, false, wFilter);
	ebAttack = env->addEditBox(L"", rect<s32>(260, 26, 340, 46), true, wFilter);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stDefense = env->addStaticText(dataManager.GetSysString(1323).c_str(), rect<s32>(205, 51, 280, 71), false, false, wFilter);
	ebDefense = env->addEditBox(L"", rect<s32>(260, 49, 340, 69), true, wFilter);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stStar = env->addStaticText(dataManager.GetSysString(1324).c_str(), rect<s32>(10, 74, 80, 94), false, false, wFilter);
	ebStar = env->addEditBox(L"", rect<s32>(60, 72, 100, 92), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stScale = env->addStaticText(dataManager.GetSysString(1336).c_str(), rect<s32>(110, 74, 150, 94), false, false, wFilter);
	ebScale = env->addEditBox(L"", rect<s32>(150, 72, 190, 92), true, wFilter);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stSearch = env->addStaticText(dataManager.GetSysString(1325).c_str(), rect<s32>(205, 74, 280, 94), false, false, wFilter);
	ebCardName = env->addEditBox(L"", rect<s32>(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(rect<s32>(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, dataManager.GetSysString(1326).c_str());
	btnStartFilter = env->addButton(rect<s32>(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, dataManager.GetSysString(1327).c_str());
	btnClearFilter = env->addButton(rect<s32>(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, dataManager.GetSysString(1304).c_str());
	wCategories = env->addWindow(rect<s32>(450, 60, 1000, 270), false, L"");
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(rect<s32>(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, dataManager.GetSysString(1211).c_str());
	for(int i = 0; i < 32; ++i)
		chkCategory[i] = env->addCheckBox(false, recti(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, dataManager.GetSysString(1100 + i).c_str());
	btnMarksFilter = env->addButton(rect<s32>(155, 96, 240, 118), wFilter, BUTTON_MARKS_FILTER, dataManager.GetSysString(1374).c_str());
	wLinkMarks = env->addWindow(rect<s32>(700, 30, 820, 150), false, L"");
	wLinkMarks->getCloseButton()->setVisible(false);
	wLinkMarks->setDrawTitlebar(false);
	wLinkMarks->setDraggable(false);
	wLinkMarks->setVisible(false);
	btnMarksOK = env->addButton(recti(45, 45, 75, 75), wLinkMarks, BUTTON_MARKERS_OK, dataManager.GetSysString(1211).c_str());
	btnMark[0] = env->addButton(recti(10, 10, 40, 40), wLinkMarks, -1, L"\u2196");
	btnMark[1] = env->addButton(recti(45, 10, 75, 40), wLinkMarks, -1, L"\u2191");
	btnMark[2] = env->addButton(recti(80, 10, 110, 40), wLinkMarks, -1, L"\u2197");
	btnMark[3] = env->addButton(recti(10, 45, 40, 75), wLinkMarks, -1, L"\u2190");
	btnMark[4] = env->addButton(recti(80, 45, 110, 75), wLinkMarks, -1, L"\u2192");
	btnMark[5] = env->addButton(recti(10, 80, 40, 110), wLinkMarks, -1, L"\u2199");
	btnMark[6] = env->addButton(recti(45, 80, 75, 110), wLinkMarks, -1, L"\u2193");
	btnMark[7] = env->addButton(recti(80, 80, 110, 110), wLinkMarks, -1, L"\u2198");
	for(int i=0;i<8;i++)
		btnMark[i]->setIsPushButton(true);
	//replay window
	wReplay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1202).c_str());
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wReplay, LISTBOX_REPLAY_LIST, rect<s32>(10, 30, 350, 400), filesystem, true, true, false);
	lstReplayList->setWorkingPath("./replay");
	lstReplayList->addFilteredExtensions(std::vector<io::path>{"yrp", "yrpx"});
	lstReplayList->setItemHeight(18);
	btnLoadReplay = env->addButton(rect<s32>(470, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, dataManager.GetSysString(1348).c_str());
	btnDeleteReplay = env->addButton(rect<s32>(360, 355, 460, 380), wReplay, BUTTON_DELETE_REPLAY, dataManager.GetSysString(1361).c_str());
	btnRenameReplay = env->addButton(rect<s32>(360, 385, 460, 410), wReplay, BUTTON_RENAME_REPLAY, dataManager.GetSysString(1362).c_str());
	btnReplayCancel = env->addButton(rect<s32>(470, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, dataManager.GetSysString(1347).c_str());
	env->addStaticText(dataManager.GetSysString(1349).c_str(), rect<s32>(360, 30, 570, 50), false, true, wReplay);
	stReplayInfo = irr::gui::CGUICustomText::addCustomText(L"", false, env, wReplay, -1, rect<s32>(360, 60, 570, 350));
	stReplayInfo->setWordWrap(true);
	chkYrp = env->addCheckBox(false, recti(360, 250, 560, 270), wReplay, -1, dataManager.GetSysString(1356).c_str());
	env->addStaticText(dataManager.GetSysString(1353).c_str(), rect<s32>(360, 275, 570, 295), false, true, wReplay);
	ebRepStartTurn = env->addEditBox(L"", rect<s32>(360, 300, 460, 320), true, wReplay, -1);
	ebRepStartTurn->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//single play window
	wSinglePlay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1201).c_str());
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = irr::gui::CGUIFileSelectListBox::addFileSelectListBox(env, wSinglePlay, LISTBOX_SINGLEPLAY_LIST, rect<s32>(10, 30, 350, 400), filesystem, true, true, false);
	lstSinglePlayList->setItemHeight(18);
	lstSinglePlayList->setWorkingPath("./single");
	lstSinglePlayList->addFilteredExtensions(std::vector<io::path>{"lua"});
	btnLoadSinglePlay = env->addButton(rect<s32>(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, dataManager.GetSysString(1211).c_str());
	btnSinglePlayCancel = env->addButton(rect<s32>(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, dataManager.GetSysString(1210).c_str());
	env->addStaticText(dataManager.GetSysString(1352).c_str(), rect<s32>(360, 30, 570, 50), false, true, wSinglePlay);
	stSinglePlayInfo = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wSinglePlay);
	//replay save
	wReplaySave = env->addWindow(rect<s32>(510, 200, 820, 320), false, dataManager.GetSysString(1340).c_str());
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1342).c_str(), rect<s32>(20, 25, 290, 45), false, false, wReplaySave);
	ebRSName =  env->addEditBox(L"", rect<s32>(20, 50, 290, 70), true, wReplaySave, EDITBOX_REPLAY_NAME);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(rect<s32>(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, dataManager.GetSysString(1341).c_str());
	btnRSNo = env->addButton(rect<s32>(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, dataManager.GetSysString(1212).c_str());
	//replay control
	wReplayControl = env->addStaticText(L"", rect<s32>(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, dataManager.GetSysString(1343).c_str());
	btnReplayPause = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, dataManager.GetSysString(1344).c_str());
	btnReplayStep = env->addButton(rect<s32>(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_STEP, dataManager.GetSysString(1345).c_str());
	btnReplayUndo = env->addButton(rect<s32>(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_UNDO, dataManager.GetSysString(1360).c_str());
	btnReplaySwap = env->addButton(rect<s32>(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346).c_str());
	btnReplayExit = env->addButton(rect<s32>(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, dataManager.GetSysString(1347).c_str());
	//chat
	wChat = env->addWindow(rect<s32>(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", rect<s32>(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	//swap
	btnSpectatorSwap = env->addButton(rect<s32>(205, 100, 295, 135), 0, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346).c_str());
	btnSpectatorSwap->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(rect<s32>(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, dataManager.GetSysString(1292).c_str());
	btnChainAlways = env->addButton(rect<s32>(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, dataManager.GetSysString(1293).c_str());
	btnChainWhenAvail = env->addButton(rect<s32>(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, dataManager.GetSysString(1294).c_str());
	btnChainIgnore->setIsPushButton(true);
	btnChainAlways->setIsPushButton(true);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	//shuffle
	btnShuffle = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, dataManager.GetSysString(1307).c_str());
	btnShuffle->setVisible(false);
	//cancel or finish
	btnCancelOrFinish = env->addButton(rect<s32>(205, 230, 295, 265), 0, BUTTON_CANCEL_OR_FINISH, dataManager.GetSysString(1295).c_str());
	btnCancelOrFinish->setVisible(false);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(rect<s32>(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	//tip
	stTip = env->addStaticText(L"", rect<s32>(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setBackgroundColor(0xc0ffffff);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
	//tip for cards in select / display list
	stCardListTip = env->addStaticText(L"", rect<s32>(0, 0, 150, 150), false, true, wCardSelect, TEXT_CARD_LIST_TIP, true);
	stCardListTip->setBackgroundColor(0xc0ffffff);
	stCardListTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stCardListTip->setVisible(false);
	device->setEventReceiver(&menuHandler);
	RefreshBGMList();
	LoadConfig();
	env->getSkin()->setFont(guiFont);
	env->setFocus(wMainMenu);
	for (u32 i = 0; i < EGDC_COUNT; ++i) {
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(224);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
	
	board.x = FIELD_X;
	board.y = FIELD_Y;
	board.z = FIELD_Z;
	board.atan = atan(FIELD_Y / FIELD_Z);
	
	engineSound = irrklang::createIrrKlangDevice();
	engineMusic = irrklang::createIrrKlangDevice();
	hideChat = false;
	hideChatTimer = 0;

	Utils::CreateResourceFolders();

	return true;
}
void Game::MainLoop() {
	wchar_t cap[256];
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, -0.90f, 0.45f, -0.42f, 0.42f, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);
	
	mProjection.buildCameraLookAtMatrixLH(vector3df(board.x, board.y, board.z), vector3df(board.x, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
	irr::ITimer* timer = device->getTimer();
	timer->setTime(0);
	int fps = 0;
	int cur_time = 0;
	while(device->run()) {
		dimension2du size = driver->getScreenSize();
		if(window_size != size) {
			window_size = size;
			cardimagetextureloading = false;
			OnResize();
		}
		linePatternD3D = (linePatternD3D + 1) % 30;
		linePatternGL = (linePatternGL << 1) | (linePatternGL >> 15);
		atkframe += 0.1f;
		atkdy = (float)sin(atkframe);
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		gMutex.Lock();
		if(dInfo.isStarted) {
			if (showcardcode == 1 || showcardcode == 3)
				PlayMusic("./sound/duelwin.mp3", true);
			else if (showcardcode == 2)
				PlayMusic("./sound/duellose.mp3", true);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] <= dInfo.lp[LocalPlayer(1)] / 2)
				PlayMusic("./sound/song-disadvantage.mp3", true);
			else if (dInfo.lp[0] > 0 && dInfo.lp[LocalPlayer(0)] >= dInfo.lp[LocalPlayer(1)] * 2)
				PlayMusic("./sound/song-advantage.mp3", true);
			else
				PlayBGM();
			DrawBackImage(imageManager.tBackGround);
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();
		} else if(is_building) {
			engineSound->stopAllSounds();
			DrawBackImage(imageManager.tBackGround_deck);
			DrawDeckBd();
			PlayMusic("./sound/deck.mp3", true);
		} else {
			engineSound->stopAllSounds();
			DrawBackImage(imageManager.tBackGround_menu);
			PlayMusic("./sound/menu.mp3", true);
		}
		DrawGUI();
		DrawSpec();
		if(cardimagetextureloading) {
			ShowCardInfo(showingcard, false);
		}
		gMutex.Unlock();
		if(signalFrame > 0) {
			signalFrame--;
			if(!signalFrame)
				frameSignal.Set();
		}
		if(waitFrame >= 0) {
			waitFrame++;
			if(waitFrame % 90 == 0) {
				stHintMsg->setText(dataManager.GetSysString(1390).c_str());
			} else if(waitFrame % 90 == 30) {
				stHintMsg->setText(dataManager.GetSysString(1391).c_str());
			} else if(waitFrame % 90 == 60) {
				stHintMsg->setText(dataManager.GetSysString(1392).c_str());
			}
		}
		driver->endScene();
		if(closeSignal.Wait(0))
			CloseDuelWindow();
		fps++;
		cur_time = timer->getTime();
		if(cur_time < fps * 17 - 20)
#ifdef _WIN32
			Sleep(20);
#else
			usleep(20000);
#endif
		if(cur_time >= 1000) {
			device->setWindowCaption(fmt::format(L"EDOPro FPS: {}", fps).c_str());
			fps = 0;
			cur_time -= 1000;
			timer->setTime(0);
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
		if (DuelClient::try_needed) {
			DuelClient::try_needed = false;
			DuelClient::StartClient(DuelClient::temp_ip, DuelClient::temp_port, false);
		}
	}
	DuelClient::StopClient(true);
	if(dInfo.isSingleMode)
		SingleMode::StopPlay(true);
#ifdef _WIN32
	Sleep(500);
#else
	usleep(500000);
#endif
	SaveConfig();
	engineSound->drop();
	engineMusic->drop();
//	device->drop();
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
void Game::LoadExpansionDB() {
	auto files = Utils::FindfolderFiles(L"./expansions/", L".cdb", 2);
	for (auto& file : files)
		dataManager.LoadDB(BufferIO::EncodeUTF8s(L"./expansions/" + file).c_str());
}
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
	auto files = Utils::FindfolderFiles(L"./deck/", L".ydk");
	for(auto& file : files) {
		cbDeck->addItem(file.substr(0, file.size() - 4).c_str());
	}
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(gameConf.lastdeck == cbDeck->getItem(i)) {
			cbDeck->setSelected(i);
			break;
		}
	}
}
void Game::RefreshReplay() {
	lstReplayList->resetPath();
}
void Game::RefreshSingleplay() {
	lstSinglePlayList->resetPath();
}
void Game::RefreshBGMList() {
	auto files = Utils::FindfolderFiles(L"./sound/BGM/", L".mp3");
	for(auto& file : files) {
		BGMList.push_back(BufferIO::EncodeUTF8s(file));
	}
}
void Game::LoadConfig() {
	gameConf.antialias = 0;
	gameConf.fullscreen = false;
	gameConf.serverport = L"7911";
	gameConf.textfontsize = 12;
	gameConf.nickname = L"";
	gameConf.gamename = L"";
	gameConf.lastdeck = L"";
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
	gameConf.enablesound = true;
	gameConf.volume = 1.0;
	gameConf.enablemusic = true;
	gameConf.draw_field_spell = 1;
	gameConf.quick_animation = 0;
	gameConf.chkAnime = 0;
	std::ifstream conf_file("system.conf", std::ifstream::in);
	if(!conf_file.is_open())
		return;
	std::string str;
	while(std::getline(conf_file, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.erase(0, pos);
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
		else if(type == "fullscreen")
			gameConf.fullscreen = std::stoi(str);
		else if(type == "errorlog")
			enable_log = std::stoi(str);
		else if(type == "nickname")
			gameConf.nickname = BufferIO::DecodeUTF8s(str);
		else if(type == "gamename")
			gameConf.gamename = BufferIO::DecodeUTF8s(str);
		else if(type == "lastdeck")
			gameConf.lastdeck = BufferIO::DecodeUTF8s(str);
		else if(type == "textfont") {
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
		else if(type == "game_version")
			PRO_VERSION = std::stoi(str);
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
		else if(type == "show_anime")
			gameConf.chkAnime = std::stoi(str);
		else if(type == "enable_sound")
			gameConf.enablesound = !!std::stoi(str);
		else if(type == "skin_index")
			gameConf.skin_index = std::stoi(str);
		else if(type == "volume")
			gameConf.volume = std::stof(str)/100.0f;
		else if(type == "enable_music")
			gameConf.enablemusic = !!std::stoi(str);
	}
	conf_file.close();
}
void Game::SaveConfig() {
	std::ofstream conf_file("system.conf", std::ofstream::out);
	if(!conf_file.is_open())
		return;
	conf_file << "#config file\n#nickname & gamename should be less than 20 characters\n";
	conf_file << "use_d3d = "			<< std::to_string(gameConf.use_d3d ? 1 : 0) << "\n";
	conf_file << "fullscreen = "		<< std::to_string(gameConf.fullscreen ? 1 : 0) << "\n";
	conf_file << "antialias = "			<< std::to_string(gameConf.antialias) << "\n";
	conf_file << "errorlog = "			<< std::to_string(enable_log) << "\n";
	conf_file << "nickname = "			<< BufferIO::EncodeUTF8s(ebNickName->getText()) << "\n";
	conf_file << "gamename = "			<< BufferIO::EncodeUTF8s(gameConf.gamename) << "\n";
	conf_file << "lastdeck = "			<< BufferIO::EncodeUTF8s(gameConf.lastdeck) << "\n";
	conf_file << "textfont = "			<< BufferIO::EncodeUTF8s(gameConf.textfont) << " " << std::to_string(gameConf.textfontsize) << "\n";
	conf_file << "numfont = "			<< BufferIO::EncodeUTF8s(gameConf.numfont) << "\n";
	conf_file << "serverport = "		<< BufferIO::EncodeUTF8s(gameConf.serverport) << "\n";
	conf_file << "lasthost = "			<< BufferIO::EncodeUTF8s(gameConf.lasthost) << "\n";
	conf_file << "lastport = "			<< BufferIO::EncodeUTF8s(gameConf.lastport) << "\n";
	conf_file << "game_version = "		<< std::to_string(PRO_VERSION) << "\n";
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
	conf_file << "show_anime = "		<< std::to_string(chkAnime->isChecked() ? 1 : 0) << "\n";
	conf_file << "skin_index = "		<< std::to_string(gameConf.skin_index) << "\n";
	conf_file << "enable_sound = "		<< std::to_string(chkEnableSound->isChecked() ? 1 : 0) << "\n";
	conf_file << "enable_music = "		<< std::to_string(chkEnableMusic->isChecked() ? 1 : 0) << "\n";
	conf_file << "#Volume of sound and music, between 0 and 100\n";
	int vol = gameConf.volume * 100;
	if(vol < 0) vol = 0; else if(vol > 100) vol = 100;
	conf_file << "volume = "			<< std::to_string(vol) << "\n";
	conf_file.close();
}
bool Game::PlayChant(unsigned int code) {
	std::string sound(fmt::format("./sound/chants/{}.wav", code));
	if(filesystem->existFile(sound.c_str())) {
		if (!engineSound->isCurrentlyPlaying(sound.c_str()))
			PlaySoundEffect((char*)sound.c_str());
		return true;
	}
	return false;
}
void Game::PlaySoundEffect(char* sound) {
	if(chkEnableSound->isChecked() && (!dInfo.isReplay || !dInfo.isReplaySkiping)) {
		engineSound->play2D(sound);
		engineSound->setSoundVolume(gameConf.volume);
	}
}
void Game::PlayMusic(char* song, bool loop) {
	if(chkEnableMusic->isChecked()) {
		if(!engineMusic->isCurrentlyPlaying(song)) {
			engineMusic->stopAllSounds();
			engineMusic->play2D(song, loop);
			engineMusic->setSoundVolume(gameConf.volume);
		}
	}
}
void Game::PlayBGM() {
	if(chkEnableMusic->isChecked() && BGMList.size() > 0) {
		static bool is_playing = false;
		static std::string strBuffer;
		if(is_playing && !engineMusic->isCurrentlyPlaying(strBuffer.c_str()))
			is_playing = false;
		if(!is_playing) {
			int count = BGMList.size();
			int bgm = rand() % count;
			strBuffer = "./sound/BGM/" + BGMList[bgm];
		}
		if(!engineMusic->isCurrentlyPlaying(strBuffer.c_str())) {
			engineMusic->stopAllSounds();
			engineMusic->play2D(strBuffer.c_str(), true);
			engineMusic->setSoundVolume(gameConf.volume);
			is_playing = true;
		}
	}
}
void Game::ShowCardInfo(int code, bool resize) {
	if (showingcard == code && !resize && !cardimagetextureloading)
		return;
	showingcard = code;
	int shouldrefresh = -1;
	auto img = imageManager.GetTexture(code, false, true, &shouldrefresh);
	cardimagetextureloading = false;
	if(shouldrefresh == 2) {
		cardimagetextureloading = true;
		return;
	}
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
				sc = aptr->second.setcode;
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
	int offset = 37;
	stInfo->setRelativePosition(rect<s32>(15, offset, 287 * window_size.Width / 1024, offset + stInfo->getTextHeight()));
	offset += stInfo->getTextHeight();
	if(wcscmp(stDataInfo->getText(), L"")) {
		stDataInfo->setRelativePosition(rect<s32>(15, offset, 287 * window_size.Width / 1024, offset + stDataInfo->getTextHeight()));
		offset += stDataInfo->getTextHeight();
	}
	if(wcscmp(stSetName->getText(), L"")) {
		stSetName->setRelativePosition(rect<s32>(15, offset, 287 * window_size.Width / 1024, offset + stSetName->getTextHeight()));
		offset += stSetName->getTextHeight();
	}
	stText->setRelativePosition(rect<s32>(15, offset, 287 * window_size.Width / 1024, 324 * window_size.Height / 640));
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
void Game::AddChatMsg(const std::wstring& msg, int player) {
	for(int i = 7; i > 0; --i) {
		chatMsg[i] = chatMsg[i - 1];
		chatTiming[i] = chatTiming[i - 1];
		chatType[i] = chatType[i - 1];
	}
	chatMsg[0].clear();
	chatTiming[0] = 1200;
	chatType[0] = player;
	switch(player) {
	case 0: //host 1
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.hostname[0]);
		break;
	case 1: //client 1
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[0]);
		break;
	case 2: //host 2
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.hostname[1]);
		break;
	case 3: //client 2
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[1]);
		break;
	case 4: //host 3
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.hostname[2]);
		break;
	case 5: //client 3
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[2]);
		break;
	case 7: //local name
		chatMsg[0].append(ebNickName->getText());
		break;
	case 8: //system custom message, no prefix.
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(L"[System]");
		break;
	case 9: //error message
		chatMsg[0].append(L"[Script Error]");
		break;
	default: //from watcher or unknown
		if(player < 11 || player > 19)
			chatMsg[0].append(L"[---]");
	}
	chatMsg[0].append(L": ").append(msg);
}
void Game::ClearChatMsg() {
	for(int i = 7; i >= 0; --i) {
		chatTiming[i] = 0;
	}
}
void Game::AddDebugMsg(const std::string& msg) {
	if (enable_log & 0x1) {
		AddChatMsg(BufferIO::DecodeUTF8s(msg), 9);
	}
	if (enable_log & 0x2) {
		ErrorLog("[Script Error]: " + msg);
	}
}
void Game::ErrorLog(const std::string& msg) {
	FILE* fp = fopen("error.log", "at");
	if(!fp)
		return;
	time_t nowtime = time(NULL);
	struct tm *localedtime = localtime(&nowtime);
	char timebuf[40];
	strftime(timebuf, 40, "%Y-%m-%d %H:%M:%S", localedtime);
	fprintf(fp, "[%s]%s\n", timebuf, msg.c_str());
	fclose(fp);
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
int Game::LocalPlayer(int player) {
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
	case MASTER_RULE_1: {
		cbDuelRule->setSelected(0);
		if (flag2 == MASTER_RULE_1_FORB)
			break;
	}
	case MASTER_RULE_2: {
		cbDuelRule->setSelected(1);
		if (flag2 == MASTER_RULE_2_FORB)
			break;
	}
	case MASTER_RULE_3: {
		cbDuelRule->setSelected(2);
		if (flag2 == MASTER_RULE_3_FORB)
			break;
	}
	case MASTER_RULE_4: {
		cbDuelRule->setSelected(3);
		if (flag2 == MASTER_RULE_4_FORB)
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
	switch(param) {
	case MASTER_RULE_1: {
		if (truerule)
			*truerule = 1;
		if (forbiddentypes == MASTER_RULE_1_FORB)
			return 1;
	}
	case MASTER_RULE_2: {
		if (truerule)
			*truerule = 2;
		if (forbiddentypes == MASTER_RULE_2_FORB)
			return 2;
	}
	case MASTER_RULE_3: {
		if (truerule)
			*truerule = 3;
		if (forbiddentypes == MASTER_RULE_3_FORB)
			return 3;
	}
	case MASTER_RULE_4: {
		if (truerule)
			*truerule = 4;
		if (forbiddentypes == MASTER_RULE_4_FORB)
			return 4;
	}
	default: {
		if (truerule)
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
void Game::OnResize() {
	wMainMenu->setRelativePosition(ResizeWin(370, 200, 650, 415));
	wDeckEdit->setRelativePosition(Resize(309, 8, 605, 130));
	cbDBLFList->setRelativePosition(Resize(80, 5, 220, 30));
	cbDBDecks->setRelativePosition(Resize(80, 35, 220, 60));
	btnClearDeck->setRelativePosition(Resize(115, 99, 165, 120));
	btnSortDeck->setRelativePosition(Resize(60, 99, 110, 120));
	btnShuffleDeck->setRelativePosition(Resize(5, 99, 55, 120));
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
	if (dInfo.isRelay) {
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
	wMessage->setRelativePosition(ResizeWin(490, 200, 840, 340));
	wACMessage->setRelativePosition(ResizeWin(490, 240, 840, 300));
	wQuery->setRelativePosition(ResizeWin(490, 200, 840, 340));
	auto pos = wOptions->getRelativePosition();
	wOptions->setRelativePosition(ResizeWin(490, 200, 490 + (pos.LowerRightCorner.X - pos.UpperLeftCorner.X), 200 + (pos.LowerRightCorner.Y - pos.UpperLeftCorner.Y)));
	wPosSelect->setRelativePosition(ResizeWin(340, 200, 935, 410));
	wCardSelect->setRelativePosition(ResizeWin(320, 100, 1000, 400));
	wANNumber->setRelativePosition(ResizeWin(550, 200, 780, 295));
	wANCard->setRelativePosition(ResizeWin(430, 170, 840, 370));
	wANAttribute->setRelativePosition(ResizeWin(500, 200, 830, 285));
	wANRace->setRelativePosition(ResizeWin(480, 200, 850, 410));
	wReplaySave->setRelativePosition(ResizeWin(510, 200, 820, 320));
	stHintMsg->setRelativePosition(ResizeWin(500, 60, 820, 90));

	wCardImg->setRelativePosition(Resize(1, 1, 1 + CARD_IMG_WIDTH + 20, 1 + CARD_IMG_HEIGHT + 18));
	imgCard->setRelativePosition(Resize(10, 9, 10 + CARD_IMG_WIDTH, 9 + CARD_IMG_HEIGHT));
	wInfos->setRelativePosition(Resize(1, 275, 301, 639));
	stName->setRelativePosition(recti(10, 10, 287 * window_size.Width / 1024, 32));
	lstLog->setRelativePosition(Resize(10, 10, 290, 290));
	imageManager.ClearTexture(true);

	if(showingcard)
		ShowCardInfo(showingcard, true);
	btnClearLog->setRelativePosition(Resize(160, 300, 260, 325));
	srcVolume->setRelativePosition(rect<s32>(85, 295, wInfos->getRelativePosition().LowerRightCorner.X - 21, 310));

	wChat->setRelativePosition(ResizeWin(wInfos->getRelativePosition().LowerRightCorner.X + 6, 615, 1020, 640, true));
	ebChatInput->setRelativePosition(recti(3, 2, window_size.Width - wChat->getRelativePosition().UpperLeftCorner.X - 6, 22));

	btnLeaveGame->setRelativePosition(Resize(205, 5, 295, 80));
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
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2) {
	x = x * window_size.Width / 1024;
	y = y * window_size.Height / 640;
	x2 = x2 * window_size.Width / 1024;
	y2 = y2 * window_size.Height / 640;
	return recti(x, y, x2, y2);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2) {
	x = x * window_size.Width / 1024 + dx;
	y = y * window_size.Height / 640 + dy;
	x2 = x2 * window_size.Width / 1024 + dx2;
	y2 = y2 * window_size.Height / 640 + dy2;
	return recti(x, y, x2, y2);
}
vector2d<s32> Game::Resize(s32 x, s32 y, bool reverse) {
	if(reverse) {
		x = x * 1024 / window_size.Width;
		y = y * 640 / window_size.Height;
	} else {
		x = x * window_size.Width / 1024;
		y = y * window_size.Height / 640;
	}
	return vector2d<s32>(x, y);
}
recti Game::ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat) {
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	if(chat) {
		y = window_size.Height - sy;
		x2 = window_size.Width;
		y2 = y + sy;
		return recti(x, y, x2, y2);
	}
	x = (x + sx / 2) * window_size.Width / 1024 - sx / 2;
	y = (y + sy / 2) * window_size.Height / 640 - sy / 2;
	x2 = sx + x;
	y2 = sy + y;
	return recti(x, y, x2, y2);
}
recti Game::ResizeElem(s32 x, s32 y, s32 x2, s32 y2) {
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	x = (x + sx / 2 - 100) * window_size.Width / 1024 - sx / 2 + 100;
	y = y * window_size.Height / 640;
	x2 = sx + x;
	y2 = sy + y;
	return recti(x, y, x2, y2);
}
void Game::ValidateName(irr::gui::IGUIElement* obj) {
	std::wstring text = obj->getText();
	const wchar_t chars[] = L"<>:\"/\\|?*";
	for(auto& forbid : chars)
		text.erase(std::remove(text.begin(), text.end(), forbid), text.end());
	obj->setText(text.c_str());
}
std::vector<std::wstring> Game::tokenize(std::wstring input, const std::wstring& token) {
	std::vector<std::wstring> res;
	std::size_t pos;
	while((pos = input.find(token)) != std::wstring::npos) {
		if(pos != 0)
			res.push_back(input.substr(0, pos));
		input = input.substr(pos + 1);
	}
	if(input.size())
		res.push_back(input);
	return res;
}
bool Game::CompareStrings(std::wstring input, const std::vector<std::wstring>& tokens, bool transform_input, bool transform_token) {
	if(input.empty() || tokens.empty())
		return false;
	if(transform_input)
		std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	std::vector<std::wstring> alttokens;
	if(transform_token) {
		alttokens = tokens;
		for(auto& token : alttokens)
			std::transform(token.begin(), token.end(), token.begin(), ::toupper);
	}
	std::size_t pos;
	for(auto& token : transform_token ? alttokens : tokens) {
		if((pos = input.find(token)) == std::wstring::npos)
			return false;
		input = input.substr(pos + 1);
	}
	return true;
}
bool Game::CompareStrings(std::wstring input, const std::wstring & second_term, bool transform_input, bool transform_term) {
	if(input.empty() || second_term.empty())
		return false;
	if(transform_input)
		std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	std::vector<std::wstring> tokens;
	tokens.push_back(second_term);
	if(transform_term)
		std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);
	return CompareStrings(input, tokens);
}
std::wstring Game::ReadPuzzleMessage(const std::wstring& script_name) {
#ifdef _WIN32
	std::ifstream infile(script_name, std::ifstream::in);
#else
	std::ifstream infile(BufferIO::EncodeUTF8s(script_name), std::ifstream::in);
#endif
	std::string str;
	std::string res = "";
	size_t start = std::string::npos;
	bool stop = false;
	while(std::getline(infile, str) && !stop) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.erase(0, pos);
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
byte* Game::ScriptReader(const char* script_name, int* slen) {
	static std::string buffer;
	IReadFile* file = nullptr;
	for(auto& path : mainGame->resource_dirs) {
		file = mainGame->filesystem->createAndOpenFile((path + script_name).c_str());
		if(file)
			break;
	}
	if(!file && !(file = mainGame->filesystem->createAndOpenFile(script_name)))
		return 0;
	const auto size = file->getSize();
	buffer.reserve(size);
	*slen = file->read(&buffer[0], size);
	file->drop();
	return (byte*)&buffer[0];
}
int Game::MessageHandler(long fduel, int type) {
	if(!enable_log)
		return 0;
	char msgbuf[1024];
	get_log_message(fduel, (byte*)msgbuf);
	mainGame->AddDebugMsg(msgbuf);
	return 0;
}
void Game::PopulateResourcesDirectories() {
#define LF(x) resource_dirs.push_back(filesystem->getAbsolutePath(x).c_str());
	LF("./expansions/script/");
	LF("./expansions/pics/");
	LF("./pics/");
	LF("./script/");
#undef LF
}


}
