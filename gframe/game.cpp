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
#include "../ocgcore/duel.h"
#include <sstream>
#include "utils.h"

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
	if(gameConf.use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
		params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<u32>(1024, 640);
	if(gameConf.fullscreen) {
		IrrlichtDevice *nulldevice = createDevice(video::EDT_NULL);
		params.WindowSize = nulldevice->getVideoModeList()->getDesktopResolution();
		nulldevice->drop();
		params.Fullscreen = true;
	}
	device = irr::createDeviceEx(params);
	if(!device)
		return false;
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
	lpcstring = 0;
	always_chain = false;
	ignore_chain = false;
	chain_when_avail = false;
	is_building = false;
	showingcard = 0;
	menuHandler.prev_operation = 0;
	menuHandler.prev_sel = -1;
	memset(&dInfo, 0, sizeof(DuelInfo));
	memset(chatTiming, 0, sizeof(chatTiming));
	deckManager.LoadLFList();
	driver = device->getVideoDriver();
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	imageManager.SetDevice(device);
	if(!imageManager.Initial())
		return false;
	LoadExpansionDB();
	if(!dataManager.LoadDB("cards.cdb"))
		return false;
	if(!dataManager.LoadStrings("strings.conf"))
		return false;
	dataManager.LoadStrings("./expansions/strings.conf");
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 16);
	adFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 12);
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 48);
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.textfont, gameConf.textfontsize);
	textFont = guiFont;
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
	wchar_t strbuf[256];
	myswprintf(strbuf, L"EDOPro Version:%X.0%X.%X", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
	wMainMenu = env->addWindow(rect<s32>(370, 200, 650, 415), false, strbuf);
	wMainMenu->getCloseButton()->setVisible(false);
	btnLanMode = env->addButton(rect<s32>(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200));
	btnSingleMode = env->addButton(rect<s32>(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201));
	btnReplayMode = env->addButton(rect<s32>(10, 100, 270, 130), wMainMenu, BUTTON_REPLAY_MODE, dataManager.GetSysString(1202));
//	btnTestMode = env->addButton(rect<s32>(10, 135, 270, 165), wMainMenu, BUTTON_TEST_MODE, dataManager.GetSysString(1203));
	btnDeckEdit = env->addButton(rect<s32>(10, 135, 270, 165), wMainMenu, BUTTON_DECK_EDIT, dataManager.GetSysString(1204));
	btnModeExit = env->addButton(rect<s32>(10, 170, 270, 200), wMainMenu, BUTTON_MODE_EXIT, dataManager.GetSysString(1210));
	//lan mode
	wLanWindow = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1200));
	wLanWindow->getCloseButton()->setVisible(false);
	wLanWindow->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1220), rect<s32>(10, 30, 220, 50), false, false, wLanWindow);
	ebNickName = env->addEditBox(gameConf.nickname, rect<s32>(110, 25, 450, 50), true, wLanWindow);
	ebNickName->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	lstHostList = env->addListBox(rect<s32>(10, 60, 570, 320), wLanWindow, LISTBOX_LAN_HOST, true);
	lstHostList->setItemHeight(18);
	btnLanRefresh = env->addButton(rect<s32>(240, 325, 340, 350), wLanWindow, BUTTON_LAN_REFRESH, dataManager.GetSysString(1217));
	env->addStaticText(dataManager.GetSysString(1221), rect<s32>(10, 360, 220, 380), false, false, wLanWindow);
	ebJoinHost = env->addEditBox(gameConf.lasthost, rect<s32>(110, 355, 350, 380), true, wLanWindow);
	ebJoinHost->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(gameConf.lastport, rect<s32>(360, 355, 420, 380), true, wLanWindow);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1222), rect<s32>(10, 390, 220, 410), false, false, wLanWindow);
	ebJoinPass = env->addEditBox(gameConf.roompass, rect<s32>(110, 385, 420, 410), true, wLanWindow);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnJoinHost = env->addButton(rect<s32>(460, 355, 570, 380), wLanWindow, BUTTON_JOIN_HOST, dataManager.GetSysString(1223));
	btnJoinCancel = env->addButton(rect<s32>(460, 385, 570, 410), wLanWindow, BUTTON_JOIN_CANCEL, dataManager.GetSysString(1212));
	btnCreateHost = env->addButton(rect<s32>(460, 25, 570, 50), wLanWindow, BUTTON_CREATE_HOST, dataManager.GetSysString(1224));
	//create host
	wCreateHost = env->addWindow(rect<s32>(320, 100, 700, 520), false, dataManager.GetSysString(1224));
	wCreateHost->getCloseButton()->setVisible(false);
	wCreateHost->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1226), rect<s32>(20, 30, 220, 50), false, false, wCreateHost);
	cbLFlist = env->addComboBox(rect<s32>(140, 25, 300, 50), wCreateHost);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbLFlist->addItem(deckManager._lfList[i].listName, deckManager._lfList[i].hash);
	env->addStaticText(dataManager.GetSysString(1225), rect<s32>(20, 60, 220, 80), false, false, wCreateHost);
	cbRule = env->addComboBox(rect<s32>(140, 55, 300, 80), wCreateHost);
	cbRule->addItem(dataManager.GetSysString(1240));
	cbRule->addItem(dataManager.GetSysString(1241));
	cbRule->addItem(dataManager.GetSysString(1242));
	cbRule->addItem(dataManager.GetSysString(1243));
	env->addStaticText(dataManager.GetSysString(1227), rect<s32>(20, 90, 220, 110), false, false, wCreateHost);
	cbMatchMode = env->addComboBox(rect<s32>(140, 85, 300, 110), wCreateHost);
	cbMatchMode->addItem(dataManager.GetSysString(1244));
	cbMatchMode->addItem(dataManager.GetSysString(1245));
	cbMatchMode->addItem(dataManager.GetSysString(1246));
	cbMatchMode->addItem(dataManager.GetSysString(1247));
	env->addStaticText(dataManager.GetSysString(1237), rect<s32>(20, 120, 320, 140), false, false, wCreateHost);
	myswprintf(strbuf, L"%d", 180);
	ebTimeLimit = env->addEditBox(strbuf, rect<s32>(140, 115, 220, 140), true, wCreateHost);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1228), rect<s32>(20, 150, 320, 170), false, false, wCreateHost);
	btnRuleCards = env->addButton(rect<s32>(260, 330, 370, 350), wCreateHost, BUTTON_RULE_CARDS, dataManager.GetSysString(1625));
	wRules = env->addWindow(rect<s32>(630, 100, 1000, 310), false, dataManager.strBuffer);
	wRules->getCloseButton()->setVisible(false);
	wRules->setDrawTitlebar(false);
	wRules->setDraggable(true);
	wRules->setVisible(false);
	btnRulesOK = env->addButton(rect<s32>(135, 175, 235, 200), wRules, BUTTON_RULE_OK, dataManager.GetSysString(1211));
	for(int i = 0; i < 13; ++i)
		chkRules[i] = env->addCheckBox(false, recti(10 + (i % 2) * 150, 10 + (i / 2) * 20, 200 + (i % 2) * 120, 30 + (i / 2) * 20), wRules, CHECKBOX_EXTRA_RULE, dataManager.GetSysString(1132 + i));
	mainGame->extra_rules = 0;
	env->addStaticText(dataManager.GetSysString(1236), rect<s32>(20, 180, 220, 200), false, false, wCreateHost);
	cbDuelRule = env->addComboBox(rect<s32>(140, 175, 300, 200), wCreateHost, COMBOBOX_DUEL_RULE);
	cbDuelRule->addItem(dataManager.GetSysString(1260));
	cbDuelRule->addItem(dataManager.GetSysString(1261));
	cbDuelRule->addItem(dataManager.GetSysString(1262));
	cbDuelRule->addItem(dataManager.GetSysString(1263));
	cbDuelRule->setSelected(DEFAULT_DUEL_RULE - 1);
	btnCustomRule = env->addButton(rect<s32>(305, 175, 370, 200), wCreateHost, BUTTON_CUSTOM_RULE, dataManager.GetSysString(1626));
	wCustomRules = env->addWindow(rect<s32>(700, 100, 910, 410), false, dataManager.strBuffer);
	wCustomRules->getCloseButton()->setVisible(false);
	wCustomRules->setDrawTitlebar(false);
	wCustomRules->setDraggable(true);
	wCustomRules->setVisible(false);
	int spacing = 0;
	env->addStaticText(dataManager.GetSysString(1629), rect<s32>(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	spacing++;
	for(int i = 0; i < 6; ++i, ++spacing)
		chkCustomRules[i] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 390 + i, dataManager.GetSysString(1631 + i));
	env->addStaticText(dataManager.GetSysString(1628), rect<s32>(10, 10 + spacing * 20, 200, 30 + spacing * 20), false, false, wCustomRules);
	myswprintf(strbuf, dataManager.GetSysString(1627), dataManager.GetSysString(1056));
	spacing++;
	chkTypeLimit[0] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, strbuf);
	myswprintf(strbuf, dataManager.GetSysString(1627), dataManager.GetSysString(1063));
	spacing++;
	chkTypeLimit[1] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, strbuf);
	myswprintf(strbuf, dataManager.GetSysString(1627), dataManager.GetSysString(1073));
	spacing++;
	chkTypeLimit[2] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, strbuf);
	myswprintf(strbuf, dataManager.GetSysString(1627), dataManager.GetSysString(1074));
	spacing++;
	chkTypeLimit[3] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, -1, strbuf);
	myswprintf(strbuf, dataManager.GetSysString(1627), dataManager.GetSysString(1076));
	spacing++;
	chkTypeLimit[4] = env->addCheckBox(false, recti(10, 10 + spacing * 20, 200, 30 + spacing * 20), wCustomRules, 353 + spacing, strbuf);
	btnCustomRulesOK = env->addButton(rect<s32>(55, 270, 155, 295), wCustomRules, BUTTON_CUSTOM_RULE_OK, dataManager.GetSysString(1211));
	forbiddentypes = MASTER_RULE_4_FORB;
	duel_param = MASTER_RULE_4;
	chkNoCheckDeck = env->addCheckBox(false, rect<s32>(20, 210, 170, 230), wCreateHost, -1, dataManager.GetSysString(1229));
	chkNoShuffleDeck = env->addCheckBox(false, rect<s32>(180, 210, 360, 230), wCreateHost, -1, dataManager.GetSysString(1230));
	env->addStaticText(dataManager.GetSysString(1231), rect<s32>(20, 240, 320, 260), false, false, wCreateHost);
	myswprintf(strbuf, L"%d", 8000);
	ebStartLP = env->addEditBox(strbuf, rect<s32>(140, 235, 220, 260), true, wCreateHost);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1232), rect<s32>(20, 270, 320, 290), false, false, wCreateHost);
	myswprintf(strbuf, L"%d", 5);
	ebStartHand = env->addEditBox(strbuf, rect<s32>(140, 265, 220, 290), true, wCreateHost);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1233), rect<s32>(20, 300, 320, 320), false, false, wCreateHost);
	myswprintf(strbuf, L"%d", 1);
	ebDrawCount = env->addEditBox(strbuf, rect<s32>(140, 295, 220, 320), true, wCreateHost);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1234), rect<s32>(10, 360, 220, 380), false, false, wCreateHost);
	ebServerName = env->addEditBox(gameConf.gamename, rect<s32>(110, 355, 250, 380), true, wCreateHost);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1235), rect<s32>(10, 390, 220, 410), false, false, wCreateHost);
	ebServerPass = env->addEditBox(L"", rect<s32>(110, 385, 250, 410), true, wCreateHost);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnHostConfirm = env->addButton(rect<s32>(260, 355, 370, 380), wCreateHost, BUTTON_HOST_CONFIRM, dataManager.GetSysString(1211));
	btnHostCancel = env->addButton(rect<s32>(260, 385, 370, 410), wCreateHost, BUTTON_HOST_CANCEL, dataManager.GetSysString(1212));
	//host(single)
	wHostPrepare = env->addWindow(rect<s32>(270, 120, 750, 440), false, dataManager.GetSysString(1250));
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	wHostPrepare2 = env->addWindow(rect<s32>(750, 120, 950, 440), false, dataManager.GetSysString(1625));
	wHostPrepare2->getCloseButton()->setVisible(false);
	wHostPrepare2->setVisible(false);
	stHostPrepRule2 = env->addStaticText(L"", rect<s32>(10, 30, 460, 350), false, true, wHostPrepare2);
	btnHostPrepDuelist = env->addButton(rect<s32>(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, dataManager.GetSysString(1251));
	for(int i = 0; i < 6; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	btnHostPrepOB = env->addButton(rect<s32>(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252));
	myswprintf(dataManager.strBuffer, L"%ls%d", dataManager.GetSysString(1253), 0);
	stHostPrepOB = env->addStaticText(dataManager.strBuffer, rect<s32>(10, 210, 270, 230), false, false, wHostPrepare);
	stHostPrepRule = env->addStaticText(L"", rect<s32>(280, 30, 460, 230), false, true, wHostPrepare);
	stDeckSelect = env->addStaticText(dataManager.GetSysString(1254), rect<s32>(10, 235, 110, 255), false, false, wHostPrepare);
	cbDeckSelect = env->addComboBox(rect<s32>(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	cbDeckSelect2 = env->addComboBox(rect<s32>(280, 230, 430, 255), wHostPrepare);
	cbDeckSelect2->setMaxSelectionRows(10);
	btnHostPrepReady = env->addButton(rect<s32>(170, 180, 270, 205), wHostPrepare, BUTTON_HP_READY, dataManager.GetSysString(1218));
	btnHostPrepNotReady = env->addButton(rect<s32>(170, 180, 270, 205), wHostPrepare, BUTTON_HP_NOTREADY, dataManager.GetSysString(1219));
	btnHostPrepNotReady->setVisible(false);
	btnHostPrepStart = env->addButton(rect<s32>(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, dataManager.GetSysString(1215));
	btnHostPrepCancel = env->addButton(rect<s32>(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, dataManager.GetSysString(1210));
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
	irr::gui::IGUITab* tabInfo = wInfos->addTab(dataManager.GetSysString(1270));
	stName = env->addStaticText(L"", rect<s32>(10, 10, 287, 32), true, false, tabInfo, -1, false);
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stInfo = env->addStaticText(L"", rect<s32>(15, 37, 296, 60), false, true, tabInfo, -1, false);
	stInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stDataInfo = env->addStaticText(L"", rect<s32>(15, 60, 296, 83), false, true, tabInfo, -1, false);
	stDataInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stSetName = env->addStaticText(L"", rect<s32>(15, 83, 296, 106), false, true, tabInfo, -1, false);
	stSetName->setOverrideColor(SColor(255, 0, 0, 255));
	stText = env->addStaticText(L"", rect<s32>(15, 106, 287, 324), false, true, tabInfo, -1, false);
	scrCardText = env->addScrollBar(false, rect<s32>(267, 106, 287, 324), tabInfo, SCROLL_CARDTEXT);
	scrCardText->setLargeStep(1);
	scrCardText->setSmallStep(1);
	scrCardText->setVisible(false);
	//log
	irr::gui::IGUITab* tabLog =  wInfos->addTab(dataManager.GetSysString(1271));
	lstLog = env->addListBox(rect<s32>(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(18);
	btnClearLog = env->addButton(rect<s32>(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, dataManager.GetSysString(1272));
	//system
	irr::gui::IGUITab* tabSystem = wInfos->addTab(dataManager.GetSysString(1273));
	chkMAutoPos = env->addCheckBox(false, rect<s32>(20, 20, 280, 45), tabSystem, -1, dataManager.GetSysString(1274));
	chkMAutoPos->setChecked(gameConf.chkMAutoPos != 0);
	chkSTAutoPos = env->addCheckBox(false, rect<s32>(20, 50, 280, 75), tabSystem, -1, dataManager.GetSysString(1278));
	chkSTAutoPos->setChecked(gameConf.chkSTAutoPos != 0);
	chkRandomPos = env->addCheckBox(false, rect<s32>(40, 80, 300, 105), tabSystem, -1, dataManager.GetSysString(1275));
	chkRandomPos->setChecked(gameConf.chkRandomPos != 0);
	chkAutoChain = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, -1, dataManager.GetSysString(1276));
	chkAutoChain->setChecked(gameConf.chkAutoChain != 0);
	chkWaitChain = env->addCheckBox(false, rect<s32>(20, 140, 280, 165), tabSystem, -1, dataManager.GetSysString(1277));
	chkWaitChain->setChecked(gameConf.chkWaitChain != 0);
	chkHideHintButton = env->addCheckBox(false, rect<s32>(20, 170, 280, 195), tabSystem, -1, dataManager.GetSysString(1355));
	chkHideHintButton->setChecked(gameConf.chkHideHintButton != 0);
	chkIgnore1 = env->addCheckBox(false, rect<s32>(20, 200, 280, 225), tabSystem, -1, dataManager.GetSysString(1290));
	chkIgnore1->setChecked(gameConf.chkIgnore1 != 0);
	chkIgnore2 = env->addCheckBox(false, rect<s32>(20, 230, 280, 255), tabSystem, -1, dataManager.GetSysString(1291));
	chkIgnore2->setChecked(gameConf.chkIgnore2 != 0);
	chkEnableSound = env->addCheckBox(gameConf.enablesound, rect<s32>(140, 260, 300, 285), tabSystem, -1, dataManager.GetSysString(2046));
	chkEnableSound->setChecked(gameConf.enablesound);
	chkEnableMusic = env->addCheckBox(gameConf.enablemusic, rect<s32>(20, 260, 140, 285), tabSystem, CHECKBOX_ENABLE_MUSIC, dataManager.GetSysString(2047));
	chkEnableMusic->setChecked(gameConf.enablemusic);
	stVolume = env->addStaticText(L"Volume", rect<s32>(20, 290, 80, 310), false, true, tabSystem, -1, false);
	srcVolume = env->addScrollBar(true, rect<s32>(85, 295, 280, 310), tabSystem, SCROLL_VOLUME);
	srcVolume->setMax(100);
	srcVolume->setMin(0);
	srcVolume->setPos(gameConf.volume * 100);
	srcVolume->setLargeStep(1);
	srcVolume->setSmallStep(1);
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
	btnFirst = env->addButton(rect<s32>(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, dataManager.GetSysString(100));
	btnSecond = env->addButton(rect<s32>(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, dataManager.GetSysString(101));
	//message (310)
	wMessage = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(1216));
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wMessage, -1, false);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(rect<s32>(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, dataManager.GetSysString(1211));
	//auto fade message (310)
	wACMessage = env->addWindow(rect<s32>(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = env->addStaticText(L"", rect<s32>(0, 0, 350, 60), true, true, wACMessage, -1, true);
	stACMessage->setBackgroundColor(0xc0c0c0ff);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(560));
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wQuery, -1, false);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(rect<s32>(100, 105, 150, 130), wQuery, BUTTON_YES, dataManager.GetSysString(1213));
	btnNo = env->addButton(rect<s32>(200, 105, 250, 130), wQuery, BUTTON_NO, dataManager.GetSysString(1214));
	//options (310)
	wOptions = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wOptions, -1, false);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(rect<s32>(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, dataManager.GetSysString(1211));
	btnOptionp = env->addButton(rect<s32>(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, L"<<<");
	btnOptionn = env->addButton(rect<s32>(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, L">>>");
	//pos select
	wPosSelect = env->addWindow(rect<s32>(340, 200, 935, 410), false, dataManager.GetSysString(561));
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
	btnSelectOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, dataManager.GetSysString(1211));
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
	btnDisplayOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, dataManager.GetSysString(1211));
	//announce number
	wANNumber = env->addWindow(rect<s32>(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(rect<s32>(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(rect<s32>(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, dataManager.GetSysString(1211));
	//announce card
	wANCard = env->addWindow(rect<s32>(430, 170, 840, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", rect<s32>(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(rect<s32>(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(rect<s32>(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, dataManager.GetSysString(1211));
	//announce attribute
	wANAttribute = env->addWindow(rect<s32>(500, 200, 830, 285), false, dataManager.GetSysString(562));
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, dataManager.FormatAttribute(filter));
	//announce race
	wANRace = env->addWindow(rect<s32>(480, 200, 850, 410), false, dataManager.GetSysString(563));
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 25; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, dataManager.FormatRace(filter));
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
	btnActivate = env->addButton(rect<s32>(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1150));
	btnSummon = env->addButton(rect<s32>(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, dataManager.GetSysString(1151));
	btnSPSummon = env->addButton(rect<s32>(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, dataManager.GetSysString(1152));
	btnMSet = env->addButton(rect<s32>(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, dataManager.GetSysString(1153));
	btnSSet = env->addButton(rect<s32>(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, dataManager.GetSysString(1153));
	btnRepos = env->addButton(rect<s32>(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, dataManager.GetSysString(1154));
	btnAttack = env->addButton(rect<s32>(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, dataManager.GetSysString(1157));
	btnShowList = env->addButton(rect<s32>(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, dataManager.GetSysString(1158));
	btnOperation = env->addButton(rect<s32>(1, 169, 99, 189), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1161));
	btnReset = env->addButton(rect<s32>(1, 190, 99, 210), wCmdMenu, BUTTON_CMD_RESET, dataManager.GetSysString(1162));
	//deck edit
	wDeckEdit = env->addStaticText(L"", rect<s32>(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	stBanlist = env->addStaticText(dataManager.GetSysString(1300), rect<s32>(10, 9, 100, 29), false, false, wDeckEdit);
	cbDBLFList = env->addComboBox(rect<s32>(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	stDeck = env->addStaticText(dataManager.GetSysString(1301), rect<s32>(10, 39, 100, 59), false, false, wDeckEdit);
	cbDBDecks = env->addComboBox(rect<s32>(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbDBLFList->addItem(deckManager._lfList[i].listName);
	btnSaveDeck = env->addButton(rect<s32>(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, dataManager.GetSysString(1302));
	ebDeckname = env->addEditBox(L"", rect<s32>(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECK_NAME);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(rect<s32>(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, dataManager.GetSysString(1303));
	btnDeleteDeck = env->addButton(rect<s32>(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, dataManager.GetSysString(1308));
	btnShuffleDeck = env->addButton(rect<s32>(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307));
	btnSortDeck = env->addButton(rect<s32>(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, dataManager.GetSysString(1305));
	btnClearDeck = env->addButton(rect<s32>(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, dataManager.GetSysString(1304));
	btnSideOK = env->addButton(rect<s32>(510, 40, 820, 80), 0, BUTTON_SIDE_OK, dataManager.GetSysString(1334));
	btnSideOK->setVisible(false);
	btnSideShuffle = env->addButton(rect<s32>(310, 100, 370, 130), 0, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307));
	btnSideShuffle->setVisible(false);
	btnSideSort = env->addButton(rect<s32>(375, 100, 435, 130), 0, BUTTON_SORT_DECK, dataManager.GetSysString(1305));
	btnSideSort->setVisible(false);
	btnSideReload = env->addButton(rect<s32>(440, 100, 500, 130), 0, BUTTON_SIDE_RELOAD, dataManager.GetSysString(1309));
	btnSideReload->setVisible(false);
	//
	scrFilter = env->addScrollBar(false, recti(999, 161, 1019, 629), 0, SCROLL_FILTER);
	scrFilter->setLargeStep(10);
	scrFilter->setSmallStep(1);
	scrFilter->setVisible(false);
	//sort type
	wSort = env->addStaticText(L"", rect<s32>(930, 132, 1020, 156), true, false, 0, -1, true);
	cbSortType = env->addComboBox(rect<s32>(10, 2, 85, 22), wSort, COMBOBOX_SORTTYPE);
	cbSortType->setMaxSelectionRows(10);
	for(int i = 1370; i <= 1373; i++)
		cbSortType->addItem(dataManager.GetSysString(i));
	wSort->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", rect<s32>(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	stCategory = env->addStaticText(dataManager.GetSysString(1311), rect<s32>(10, 5, 70, 25), false, false, wFilter);
	cbCardType = env->addComboBox(rect<s32>(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(dataManager.GetSysString(1310));
	cbCardType->addItem(dataManager.GetSysString(1312));
	cbCardType->addItem(dataManager.GetSysString(1313));
	cbCardType->addItem(dataManager.GetSysString(1314));
	cbCardType2 = env->addComboBox(rect<s32>(130, 3, 190, 23), wFilter, COMBOBOX_SECONDTYPE);
	cbCardType2->setMaxSelectionRows(20);
	cbCardType2->addItem(dataManager.GetSysString(1310), 0);
	chkAnime = env->addCheckBox(false, recti(10, 96, 150, 118), wFilter, CHECKBOX_SHOW_ANIME, dataManager.GetSysString(1999));
	chkAnime->setChecked(gameConf.chkAnime != 0);
	stLimit = env->addStaticText(dataManager.GetSysString(1315), rect<s32>(205, 5, 280, 25), false, false, wFilter);
	cbLimit = env->addComboBox(rect<s32>(260, 3, 390, 23), wFilter, COMBOBOX_OTHER_FILT);
	cbLimit->setMaxSelectionRows(10);
	cbLimit->addItem(dataManager.GetSysString(1310));
	cbLimit->addItem(dataManager.GetSysString(1316));
	cbLimit->addItem(dataManager.GetSysString(1317));
	cbLimit->addItem(dataManager.GetSysString(1318));
	cbLimit->addItem(dataManager.GetSysString(1240));
	cbLimit->addItem(dataManager.GetSysString(1241));
	cbLimit->addItem(dataManager.GetSysString(1242));
	if(chkAnime->isChecked()) {
		cbLimit->addItem(dataManager.GetSysString(1243));
		cbLimit->addItem(L"Illegal");
		cbLimit->addItem(L"VG");
		cbLimit->addItem(L"Custom");
	}
	stAttribute = env->addStaticText(dataManager.GetSysString(1319), rect<s32>(10, 28, 70, 48), false, false, wFilter);
	cbAttribute = env->addComboBox(rect<s32>(60, 26, 190, 46), wFilter, COMBOBOX_OTHER_FILT);
	cbAttribute->setMaxSelectionRows(10);
	cbAttribute->addItem(dataManager.GetSysString(1310), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(dataManager.FormatAttribute(filter), filter);
	stRace = env->addStaticText(dataManager.GetSysString(1321), rect<s32>(10, 51, 70, 71), false, false, wFilter);
	cbRace = env->addComboBox(rect<s32>(60, 49, 190, 69), wFilter, COMBOBOX_OTHER_FILT);
	cbRace->setMaxSelectionRows(10);
	cbRace->addItem(dataManager.GetSysString(1310), 0);
	for(int filter = 0x1; filter != 0x2000000; filter <<= 1)
		cbRace->addItem(dataManager.FormatRace(filter), filter);
	stAttack = env->addStaticText(dataManager.GetSysString(1322), rect<s32>(205, 28, 280, 48), false, false, wFilter);
	ebAttack = env->addEditBox(L"", rect<s32>(260, 26, 340, 46), true, wFilter);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stDefense = env->addStaticText(dataManager.GetSysString(1323), rect<s32>(205, 51, 280, 71), false, false, wFilter);
	ebDefense = env->addEditBox(L"", rect<s32>(260, 49, 340, 69), true, wFilter);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stStar = env->addStaticText(dataManager.GetSysString(1324), rect<s32>(10, 74, 80, 94), false, false, wFilter);
	ebStar = env->addEditBox(L"", rect<s32>(60, 72, 100, 92), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stScale = env->addStaticText(dataManager.GetSysString(1336), rect<s32>(110, 74, 150, 94), false, false, wFilter);
	ebScale = env->addEditBox(L"", rect<s32>(150, 72, 190, 92), true, wFilter);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stSearch = env->addStaticText(dataManager.GetSysString(1325), rect<s32>(205, 74, 280, 94), false, false, wFilter);
	ebCardName = env->addEditBox(L"", rect<s32>(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(rect<s32>(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, dataManager.GetSysString(1326));
	btnStartFilter = env->addButton(rect<s32>(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, dataManager.GetSysString(1327));
	btnClearFilter = env->addButton(rect<s32>(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, dataManager.GetSysString(1304));
	wCategories = env->addWindow(rect<s32>(450, 60, 1000, 270), false, dataManager.strBuffer);
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(rect<s32>(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, dataManager.GetSysString(1211));
	for(int i = 0; i < 32; ++i)
		chkCategory[i] = env->addCheckBox(false, recti(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, dataManager.GetSysString(1100 + i));
	btnMarksFilter = env->addButton(rect<s32>(155, 96, 240, 118), wFilter, BUTTON_MARKS_FILTER, dataManager.GetSysString(1374));
	wLinkMarks = env->addWindow(rect<s32>(700, 30, 820, 150), false, dataManager.strBuffer);
	wLinkMarks->getCloseButton()->setVisible(false);
	wLinkMarks->setDrawTitlebar(false);
	wLinkMarks->setDraggable(false);
	wLinkMarks->setVisible(false);
	btnMarksOK = env->addButton(recti(45, 45, 75, 75), wLinkMarks, BUTTON_MARKERS_OK, dataManager.GetSysString(1211));
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
	wReplay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1202));
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = env->addListBox(rect<s32>(10, 30, 350, 400), wReplay, LISTBOX_REPLAY_LIST, true);
	lstReplayList->setItemHeight(18);
	btnLoadReplay = env->addButton(rect<s32>(470, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, dataManager.GetSysString(1348));
	btnDeleteReplay = env->addButton(rect<s32>(360, 355, 460, 380), wReplay, BUTTON_DELETE_REPLAY, dataManager.GetSysString(1361));
	btnRenameReplay = env->addButton(rect<s32>(360, 385, 460, 410), wReplay, BUTTON_RENAME_REPLAY, dataManager.GetSysString(1362));
	btnReplayCancel = env->addButton(rect<s32>(470, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, dataManager.GetSysString(1347));
	env->addStaticText(dataManager.GetSysString(1349), rect<s32>(360, 30, 570, 50), false, true, wReplay);
	stReplayInfo = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wReplay);
	chkYrp = env->addCheckBox(false, recti(360, 250, 560, 270), wReplay, -1, dataManager.GetSysString(1356));
	env->addStaticText(dataManager.GetSysString(1353), rect<s32>(360, 275, 570, 295), false, true, wReplay);
	ebRepStartTurn = env->addEditBox(L"", rect<s32>(360, 300, 460, 320), true, wReplay, -1);
	ebRepStartTurn->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//single play window
	wSinglePlay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1201));
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = env->addListBox(rect<s32>(10, 30, 350, 400), wSinglePlay, LISTBOX_SINGLEPLAY_LIST, true);
	lstSinglePlayList->setItemHeight(18);
	btnLoadSinglePlay = env->addButton(rect<s32>(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, dataManager.GetSysString(1211));
	btnSinglePlayCancel = env->addButton(rect<s32>(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, dataManager.GetSysString(1210));
	env->addStaticText(dataManager.GetSysString(1352), rect<s32>(360, 30, 570, 50), false, true, wSinglePlay);
	stSinglePlayInfo = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wSinglePlay);
	//replay save
	wReplaySave = env->addWindow(rect<s32>(510, 200, 820, 320), false, dataManager.GetSysString(1340));
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1342), rect<s32>(20, 25, 290, 45), false, false, wReplaySave);
	ebRSName =  env->addEditBox(L"", rect<s32>(20, 50, 290, 70), true, wReplaySave, EDITBOX_REPLAY_NAME);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(rect<s32>(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, dataManager.GetSysString(1341));
	btnRSNo = env->addButton(rect<s32>(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, dataManager.GetSysString(1212));
	//replay control
	wReplayControl = env->addStaticText(L"", rect<s32>(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, dataManager.GetSysString(1343));
	btnReplayPause = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, dataManager.GetSysString(1344));
	btnReplayStep = env->addButton(rect<s32>(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_STEP, dataManager.GetSysString(1345));
	btnReplayUndo = env->addButton(rect<s32>(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_UNDO, dataManager.GetSysString(1360));
	btnReplaySwap = env->addButton(rect<s32>(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346));
	btnReplayExit = env->addButton(rect<s32>(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, dataManager.GetSysString(1347));
	//chat
	wChat = env->addWindow(rect<s32>(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", rect<s32>(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	//swap
	btnSpectatorSwap = env->addButton(rect<s32>(205, 100, 295, 135), 0, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346));
	btnSpectatorSwap->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(rect<s32>(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, dataManager.GetSysString(1292));
	btnChainAlways = env->addButton(rect<s32>(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, dataManager.GetSysString(1293));
	btnChainWhenAvail = env->addButton(rect<s32>(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, dataManager.GetSysString(1294));
	btnChainIgnore->setIsPushButton(true);
	btnChainAlways->setIsPushButton(true);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	//shuffle
	btnShuffle = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, dataManager.GetSysString(1307));
	btnShuffle->setVisible(false);
	//cancel or finish
	btnCancelOrFinish = env->addButton(rect<s32>(205, 230, 295, 265), 0, BUTTON_CANCEL_OR_FINISH, dataManager.GetSysString(1295));
	btnCancelOrFinish->setVisible(false);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(rect<s32>(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	//tip
	stTip = env->addStaticText(L"", rect<s32>(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setBackgroundColor(0xc0ffffff);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
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

	utils.initUtils();

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
		if (window_size != size) {
			window_size = size;
			OnResize();
		}
		linePatternD3D = (linePatternD3D + 1) % 30;
		linePatternGL = (linePatternGL << 1) | (linePatternGL >> 15);
		atkframe += 0.1f;
		atkdy = (float)sin(atkframe);
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		gMutex.Lock();
		if(dInfo.isStarted || dInfo.isReplaySkiping) {
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
		gMutex.Unlock();
		if(signalFrame > 0) {
			signalFrame--;
			if(!signalFrame)
				frameSignal.Set();
		}
		if(waitFrame >= 0) {
			waitFrame++;
			if(waitFrame % 90 == 0) {
				stHintMsg->setText(dataManager.GetSysString(1390));
			} else if(waitFrame % 90 == 30) {
				stHintMsg->setText(dataManager.GetSysString(1391));
			} else if(waitFrame % 90 == 60) {
				stHintMsg->setText(dataManager.GetSysString(1392));
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
			myswprintf(cap, L"EDOPro FPS: %d", fps);
			device->setWindowCaption(cap);
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
void Game::InitStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, u32 cHeight, irr::gui::CGUITTFont* font, const wchar_t* text) {
	SetStaticText(pControl, cWidth - 10, font, text);
	if(font->getDimension(dataManager.strBuffer).Height <= cHeight) {
		scrCardText->setVisible(false);
		if(env->hasFocus(scrCardText))
			env->removeFocus(scrCardText);
		return;
	}
	const auto& tsize = scrCardText->getRelativePosition();
	SetStaticText(pControl, cWidth - tsize.getWidth() - 10, font, text);
	u32 fontheight = font->getDimension(L"A").Height + font->getKerningHeight();
	u32 step = (font->getDimension(dataManager.strBuffer).Height - cHeight) / fontheight + 1;
	scrCardText->setVisible(true);
	scrCardText->setMin(0);
	scrCardText->setMax(step);
	scrCardText->setPos(0);
}
void Game::SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, const wchar_t* text, u32 pos) {
	int pbuffer = 0, lsnz = 0;
	u32 _width = 0, _height = 0, s = font->getCharDimension(L' ').Width;
	wchar_t prev = 0, temp[4096] = L"";
	for (size_t i = 0; text[i] != 0 && i < wcslen(text); ++i) {
		wchar_t c = text[i];
		u32 w = font->getCharDimension(c).Width + font->getKerningWidth(c, prev);
		prev = c;
		if (c == L' ') {
			lsnz = pbuffer;
			if (_width + s > cWidth) {
				temp[pbuffer++] = L'\n';
				_width = 0;
			}
			else {
				temp[pbuffer++] = L' ';
				_width += s;
			}
		} else if(c == L'\n') {
			temp[pbuffer++] = L'\n';
			_width = 0;
		} else {
			if((_width += w) > cWidth) {
				if(lsnz) {
					wchar_t old = temp[lsnz];
					temp[lsnz] = L'\n';
					_width = 0;
					for (int j = lsnz + 1; j < i; j++) {
						_width += font->getCharDimension(text[j]).Width;
					}
					if(_width > cWidth)
						temp[lsnz] = old;
				}
				if(_width > cWidth) {
					temp[pbuffer++] = L'\n';
					_width = w;
				}
			}
			temp[pbuffer++] = c;
		}
	}
	pbuffer = 0;
	for (size_t i = 0; temp[i] != 0 && i < wcslen(temp); ++i) {
		wchar_t c = temp[i];
		if (c == L'\n') {
			_height++;
			if(_height == pos) {
				pbuffer = 0;
				continue;
			}
		}
		dataManager.strBuffer[pbuffer++] = c;
	}
	dataManager.strBuffer[pbuffer] = 0;
	pControl->setText(dataManager.strBuffer);
}
void Game::LoadExpansionDB() {
#ifdef _WIN32
	char fpath[1000];
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./expansions/*.cdb", &fdataw);
	if(fh != INVALID_HANDLE_VALUE) {
		do {
			if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				char fname[780];
				BufferIO::EncodeUTF8(fdataw.cFileName, fname);
				sprintf(fpath, "./expansions/%s", fname);
				dataManager.LoadDB(fpath);
			}
		} while(FindNextFileW(fh, &fdataw));
		FindClose(fh);
	}
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./expansions/")) != NULL) {
		while((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".cdb") != 0)
				continue;
			char filepath[1000];
			sprintf(filepath, "./expansions/%s", dirp->d_name);
			dataManager.LoadDB(filepath);
		}
		closedir(dir);
	}
#endif
}
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./deck/*.ydk", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			wchar_t* pf = fdataw.cFileName;
			while(*pf) pf++;
			while(*pf != L'.') pf--;
			*pf = 0;
			cbDeck->addItem(fdataw.cFileName);
		}
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./deck/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".ydk") != 0)
			continue;
		dirp->d_name[len - 4] = 0;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		cbDeck->addItem(wname);
	}
	closedir(dir);
#endif
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(!wcscmp(cbDeck->getItem(i), gameConf.lastdeck)) {
			cbDeck->setSelected(i);
			break;
		}
	}
}
void Game::RefreshReplay() {
	lstReplayList->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./replay/*.yrp", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && Replay::CheckReplay(fdataw.cFileName)) {
			lstReplayList->addItem(fdataw.cFileName);
		}
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./replay/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".yrp") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		if(Replay::CheckReplay(wname))
			lstReplayList->addItem(wname);
	}
	closedir(dir);
#endif
}
void Game::RefreshSingleplay() {
	lstSinglePlayList->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./single/*.lua", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			lstSinglePlayList->addItem(fdataw.cFileName);
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./single/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".lua") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		lstSinglePlayList->addItem(wname);
	}
	closedir(dir);
#endif
}
void Game::RefreshBGMList() {
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./sound/BGM/*.mp3", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			BGMList.push_back(fdataw.cFileName);
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./sound/BGM/*.mp3")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".mp3") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		BGMList.push_back(wname);
	}
	closedir(dir);
#endif
}
void Game::LoadConfig() {
	FILE* fp = fopen("system.conf", "r");
	if(!fp)
		return;
	char linebuf[256];
	char strbuf[32];
	char valbuf[256];
	wchar_t wstr[256];
	gameConf.antialias = 0;
	gameConf.fullscreen = false;
	gameConf.serverport = 7911;
	gameConf.textfontsize = 12;
	gameConf.nickname[0] = 0;
	gameConf.gamename[0] = 0;
	gameConf.lastdeck[0] = 0;
	gameConf.numfont[0] = 0;
	gameConf.textfont[0] = 0;
	gameConf.lasthost[0] = 0;
	gameConf.lastport[0] = 0;
	gameConf.roompass[0] = 0;
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
	gameConf.chkAnime = 0;
	while(fgets(linebuf, 256, fp)) {
		sscanf(linebuf, "%s = %s", strbuf, valbuf);
		if(!strcmp(strbuf, "antialias")) {
			gameConf.antialias = atoi(valbuf);
		} else if(!strcmp(strbuf, "use_d3d")) {
			gameConf.use_d3d = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf, "fullscreen")) {
			gameConf.fullscreen = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf, "errorlog")) {
			enable_log = atoi(valbuf);
		} else if(!strcmp(strbuf, "textfont")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			int textfontsize;
			sscanf(linebuf, "%s = %s %d", strbuf, valbuf, &textfontsize);
			gameConf.textfontsize = textfontsize;
			BufferIO::CopyWStr(wstr, gameConf.textfont, 256);
		} else if(!strcmp(strbuf, "numfont")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.numfont, 256);
		} else if(!strcmp(strbuf, "serverport")) {
			gameConf.serverport = atoi(valbuf);
		} else if(!strcmp(strbuf, "lasthost")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lasthost, 100);
		} else if(!strcmp(strbuf, "lastport")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastport, 20);
		} else if(!strcmp(strbuf, "roompass")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.roompass, 20);
		} else if(!strcmp(strbuf, "game_version")) {
			PRO_VERSION = atoi(valbuf);
		} else if(!strcmp(strbuf, "automonsterpos")) {
			gameConf.chkMAutoPos = atoi(valbuf);
		} else if(!strcmp(strbuf, "autospellpos")) {
			gameConf.chkSTAutoPos = atoi(valbuf);
		} else if(!strcmp(strbuf, "randompos")) {
			gameConf.chkRandomPos = atoi(valbuf);
		} else if(!strcmp(strbuf, "autochain")) {
			gameConf.chkAutoChain = atoi(valbuf);
		} else if(!strcmp(strbuf, "waitchain")) {
			gameConf.chkWaitChain = atoi(valbuf);
		} else if(!strcmp(strbuf, "mute_opponent")) {
			gameConf.chkIgnore1 = atoi(valbuf);
		} else if(!strcmp(strbuf, "mute_spectators")) {
			gameConf.chkIgnore2 = atoi(valbuf);
		} else if(!strcmp(strbuf, "hide_setname")) {
			gameConf.chkHideSetname = atoi(valbuf);
		} else if(!strcmp(strbuf, "hide_hint_button")) {
			gameConf.chkHideHintButton = atoi(valbuf);
		} else if(!strcmp(strbuf, "draw_field_spell")) {
			gameConf.draw_field_spell = atoi(valbuf);
		} else if(!strcmp(strbuf, "show_anime")) {
			gameConf.chkAnime = atoi(valbuf);
		} else if(!strcmp(strbuf, "enable_sound")) {
 			gameConf.enablesound = atoi(valbuf) > 0;
		} else if (!strcmp(strbuf, "skin_index")) {
			gameConf.skin_index = atoi(valbuf);
 		} else if(!strcmp(strbuf, "volume")) {
 			gameConf.volume = atof(valbuf) / 100;
 		} else if(!strcmp(strbuf, "enable_music")) {
 			gameConf.enablemusic = atoi(valbuf) > 0;
		} else {
			// options allowing multiple words
			sscanf(linebuf, "%s = %240[^\n]", strbuf, valbuf);
			if (!strcmp(strbuf, "nickname")) {
				BufferIO::DecodeUTF8(valbuf, wstr);
				BufferIO::CopyWStr(wstr, gameConf.nickname, 20);
			} else if (!strcmp(strbuf, "gamename")) {
				BufferIO::DecodeUTF8(valbuf, wstr);
				BufferIO::CopyWStr(wstr, gameConf.gamename, 20);
			} else if (!strcmp(strbuf, "lastdeck")) {
				BufferIO::DecodeUTF8(valbuf, wstr);
				BufferIO::CopyWStr(wstr, gameConf.lastdeck, 64);
			}
		}
	}
	fclose(fp);
}
void Game::SaveConfig() {
	FILE* fp = fopen("system.conf", "w");
	fprintf(fp, "#config file\n#nickname & gamename should be less than 20 characters\n");
	char linebuf[256];
	fprintf(fp, "use_d3d = %d\n", gameConf.use_d3d ? 1 : 0);
	fprintf(fp, "fullscreen = %d\n", gameConf.fullscreen ? 1 : 0);
	fprintf(fp, "antialias = %d\n", gameConf.antialias);
	fprintf(fp, "errorlog = %d\n", enable_log);
	BufferIO::CopyWStr(ebNickName->getText(), gameConf.nickname, 20);
	BufferIO::EncodeUTF8(gameConf.nickname, linebuf);
	fprintf(fp, "nickname = %s\n", linebuf);
	BufferIO::EncodeUTF8(gameConf.gamename, linebuf);
	fprintf(fp, "gamename = %s\n", linebuf);
	BufferIO::EncodeUTF8(gameConf.lastdeck, linebuf);
	fprintf(fp, "lastdeck = %s\n", linebuf);
	BufferIO::EncodeUTF8(gameConf.textfont, linebuf);
	fprintf(fp, "textfont = %s %d\n", linebuf, gameConf.textfontsize);
	BufferIO::EncodeUTF8(gameConf.numfont, linebuf);
	fprintf(fp, "numfont = %s\n", linebuf);
	fprintf(fp, "serverport = %d\n", gameConf.serverport);
	BufferIO::EncodeUTF8(gameConf.lasthost, linebuf);
	fprintf(fp, "lasthost = %s\n", linebuf);
	BufferIO::EncodeUTF8(gameConf.lastport, linebuf);
	fprintf(fp, "lastport = %s\n", linebuf);
	fprintf(fp, "game_version = %d\n", PRO_VERSION);
	//settings
	fprintf(fp, "automonsterpos = %d\n", ((chkMAutoPos->isChecked()) ? 1 : 0));
	fprintf(fp, "autospellpos = %d\n", ((chkSTAutoPos->isChecked()) ? 1 : 0));
	fprintf(fp, "randompos = %d\n", ((chkRandomPos->isChecked()) ? 1 : 0));
	fprintf(fp, "autochain = %d\n", ((chkAutoChain->isChecked()) ? 1 : 0));
	fprintf(fp, "waitchain = %d\n", ((chkWaitChain->isChecked()) ? 1 : 0));
	fprintf(fp, "mute_opponent = %d\n", ((chkIgnore1->isChecked()) ? 1 : 0));
	fprintf(fp, "mute_spectators = %d\n", ((chkIgnore2->isChecked()) ? 1 : 0));
	fprintf(fp, "hide_setname = %d\n", ((gameConf.chkHideSetname) ? 1 : 0));
	fprintf(fp, "hide_hint_button = %d\n", ((chkHideHintButton->isChecked()) ? 1 : 0));
	fprintf(fp, "draw_field_spell = %d\n", gameConf.draw_field_spell);
	fprintf(fp, "show_anime = %d\n", ((chkAnime->isChecked()) ? 1 : 0));
	fprintf(fp, "skin_index = %d\n", gameConf.skin_index);
	fprintf(fp, "enable_sound = %d\n", ((chkEnableSound->isChecked()) ? 1 : 0));
	fprintf(fp, "enable_music = %d\n", ((chkEnableMusic->isChecked()) ? 1 : 0));
	fprintf(fp, "#Volume of sound and music, between 0 and 100\n");
	int vol = gameConf.volume * 100;
	if (vol < 0) vol = 0; else if (vol > 100) vol = 100;
	fprintf(fp, "volume = %d\n", vol);
	fclose(fp);
}
bool Game::PlayChant(unsigned int code) {
	char sound[1000];
	sprintf(sound, "./sound/chants/%d.wav", code);
	FILE *file = fopen(sound, "r");
	if(file) {
		fclose(file);
		if (!engineSound->isCurrentlyPlaying(sound))
			PlaySoundEffect(sound);
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
		static char strBuffer[1024];
		if(is_playing && !engineMusic->isCurrentlyPlaying(strBuffer))
			is_playing = false;
		if(!is_playing) {
			int count = BGMList.size();
			int bgm = rand() % count;
			auto name = BGMList[bgm].c_str();
			wchar_t fname[256];
			myswprintf(fname, L"./sound/BGM/%ls", name);
			BufferIO::EncodeUTF8(fname, strBuffer);
		}
		if(!engineMusic->isCurrentlyPlaying(strBuffer)) {
			engineMusic->stopAllSounds();
			engineMusic->play2D(strBuffer, true);
			engineMusic->setSoundVolume(gameConf.volume);
			is_playing = true;
		}
	}
}
void Game::ShowCardInfo(int code, bool resize) {
	if (showingcard == code && !resize)
		return;
	showingcard = code;
	CardData cd;
	wchar_t formatBuffer[256];
	if(!dataManager.GetData(code, &cd))
		memset(&cd, 0, sizeof(CardData));
	imgCard->setImage(imageManager.GetTexture(code, true));
	imgCard->setScaleImage(true);
	if(cd.alias != 0 && (cd.alias - code < 10 || code - cd.alias < 10))
		myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(cd.alias), cd.alias);
	else myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(code), code);
	stName->setText(formatBuffer);
	stSetName->setText(L"");
	if(!gameConf.chkHideSetname) {
		unsigned long long sc = cd.setcode;
		if(cd.alias) {
			auto aptr = dataManager._datas.find(cd.alias);
			if(aptr != dataManager._datas.end())
				sc = aptr->second.setcode;
		}
		if(sc) {
			myswprintf(formatBuffer, L"%ls%ls", dataManager.GetSysString(1329), dataManager.FormatSetName(sc));
			SetStaticText(stSetName, (296 * window_size.Width / 1024) - 15, textFont, formatBuffer, 0);
		}
	}
	if(cd.type & TYPE_MONSTER) {
		myswprintf(formatBuffer, L"[%ls] %ls/%ls", dataManager.FormatType(cd.type), dataManager.FormatRace(cd.race), dataManager.FormatAttribute(cd.attribute));
		SetStaticText(stInfo, (296 * window_size.Width / 1024) - 15, textFont, formatBuffer, 0);
		if(cd.type & TYPE_LINK){
			if (cd.attack < 0)
				myswprintf(formatBuffer, L"?/Link %d	", cd.level);
			else
				myswprintf(formatBuffer, L"%d/Link %d	", cd.attack, cd.level);
			wcscat(formatBuffer, dataManager.FormatLinkMarker(cd.link_marker));
		}
		else {
			wchar_t* form = L"\u2605";
			if(cd.type & TYPE_XYZ) form = L"\u2606";
			myswprintf(formatBuffer, L"[%ls%d] ", form, cd.level);
			wchar_t adBuffer[16];
			if (cd.attack < 0 && cd.defense < 0)
				myswprintf(adBuffer, L"?/?");
			else if (cd.attack < 0)
				myswprintf(adBuffer, L"?/%d", cd.defense);
			else if (cd.defense < 0)
				myswprintf(adBuffer, L"%d/?", cd.attack);
			else
				myswprintf(adBuffer, L"%d/%d", cd.attack, cd.defense);
			wcscat(formatBuffer, adBuffer);
		}
		if(cd.type & TYPE_PENDULUM) {
			wchar_t scaleBuffer[16];
			myswprintf(scaleBuffer, L"   %d/%d", cd.lscale, cd.rscale);
			wcscat(formatBuffer, scaleBuffer);
		}
		SetStaticText(stDataInfo, (296 * window_size.Width / 1024) - 15, textFont, formatBuffer, 0);
	} else {
		myswprintf(formatBuffer, L"[%ls]", dataManager.FormatType(cd.type));
		SetStaticText(stInfo, (296 * window_size.Width / 1024) - 15, textFont, formatBuffer, 0);
		stDataInfo->setText(L"");
	}
	int offset = 37;
	stInfo->setRelativePosition(rect<s32>(15, offset, (296 * window_size.Width / 1024), offset + textFont->getDimension(stInfo->getText()).Height));
	offset += textFont->getDimension(stInfo->getText()).Height;
	if(wcscmp(stDataInfo->getText(), L"")) {
		stDataInfo->setRelativePosition(rect<s32>(15, offset, 296 * window_size.Width / 1024, offset + textFont->getDimension(stDataInfo->getText()).Height));
		offset += textFont->getDimension(stDataInfo->getText()).Height;
	}
	if(wcscmp(stSetName->getText(), L"")) {
		stSetName->setRelativePosition(rect<s32>(15, offset, 296 * window_size.Width / 1024, offset + textFont->getDimension(stSetName->getText()).Height));
		offset += textFont->getDimension(stSetName->getText()).Height;
	}
	stText->setRelativePosition(rect<s32>(15, offset, 287 * window_size.Width / 1024, 324 * window_size.Height / 640));
	scrCardText->setRelativePosition(rect<s32>(267 * window_size.Width / 1024, offset, 287 * window_size.Width / 1024, 324 * window_size.Height / 640));
	showingtext = dataManager.GetText(code);
	const auto& tsize = stText->getRelativePosition();
	InitStaticText(stText, tsize.getWidth(), tsize.getHeight(), textFont, showingtext);
}
void Game::AddChatMsg(wchar_t* msg, int player) {
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
		chatMsg[0].append(L": ");
		break;
	case 1: //client 1
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[0]);
		chatMsg[0].append(L": ");
		break;
	case 2: //host 2
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.hostname[1]);
		chatMsg[0].append(L": ");
		break;
	case 3: //client 2
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[1]);
		chatMsg[0].append(L": ");
		break;
	case 4: //host 3
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.hostname[2]);
		chatMsg[0].append(L": ");
		break;
	case 5: //client 3
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(dInfo.clientname[2]);
		chatMsg[0].append(L": ");
		break;
	case 7: //local name
		chatMsg[0].append(ebNickName->getText());
		chatMsg[0].append(L": ");
		break;
	case 8: //system custom message, no prefix.
		PlaySoundEffect("./sound/chatmessage.wav");
		chatMsg[0].append(L"[System]: ");
		break;
	case 9: //error message
		chatMsg[0].append(L"[Script Error]: ");
		break;
	default: //from watcher or unknown
		if(player < 11 || player > 19)
			chatMsg[0].append(L"[---]: ");
	}
	chatMsg[0].append(msg);
}
void Game::ClearChatMsg() {
	for(int i = 7; i >= 0; --i) {
		chatTiming[i] = 0;
	}
}
void Game::AddDebugMsg(char* msg)
{
	if (enable_log & 0x1) {
		wchar_t wbuf[1024];
		BufferIO::DecodeUTF8(msg, wbuf);
		AddChatMsg(wbuf, 9);
	}
	if (enable_log & 0x2) {
		FILE* fp = fopen("error.log", "at");
		if (!fp)
			return;
		time_t nowtime = time(NULL);
		struct tm *localedtime = localtime(&nowtime);
		char timebuf[40];
		strftime(timebuf, 40, "%Y-%m-%d %H:%M:%S", localedtime);
		fprintf(fp, "[%s][Script Error]: %s\n", timebuf, msg);
		fclose(fp);
	}
}
void Game::ClearTextures() {
	matManager.mCard.setTexture(0, 0);
	imgCard->setImage(imageManager.tCover[0]);
	scrCardText->setVisible(false);
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
	showingcard = 0;
	scrCardText->setVisible(false);
	closeDoneSignal.Set();
}
int Game::LocalPlayer(int player) {
	return dInfo.isFirst ? player : 1 - player;
}
const wchar_t* Game::LocalName(int local_player) {
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
	cbDuelRule->addItem(dataManager.GetSysString(1260));
	cbDuelRule->addItem(dataManager.GetSysString(1261));
	cbDuelRule->addItem(dataManager.GetSysString(1262));
	cbDuelRule->addItem(dataManager.GetSysString(1263));
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
		cbDuelRule->addItem(dataManager.GetSysString(1630));
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
	wOptions->setRelativePosition(ResizeWin(490, 200, 840, 340));
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
	const auto& tsize = stText->getRelativePosition();
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

	imageManager.ClearTexture();
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2)
{
	x = x * window_size.Width / 1024;
	y = y * window_size.Height / 640;
	x2 = x2 * window_size.Width / 1024;
	y2 = y2 * window_size.Height / 640;
	return recti(x, y, x2, y2);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2)
{
	x = x * window_size.Width / 1024 + dx;
	y = y * window_size.Height / 640 + dy;
	x2 = x2 * window_size.Width / 1024 + dx2;
	y2 = y2 * window_size.Height / 640 + dy2;
	return recti(x, y, x2, y2);
}
position2di Game::Resize(s32 x, s32 y, bool reverse)
{
	if (reverse)
	{
		x = x * 1024 / window_size.Width;
		y = y * 640 / window_size.Height;
	}
	else
	{
		x = x * window_size.Width / 1024;
		y = y * window_size.Height / 640;
	}
	return position2di(x, y);
}
recti Game::ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat)
{
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	if (chat)
	{
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
recti Game::ResizeElem(s32 x, s32 y, s32 x2, s32 y2)
{
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	x = (x + sx / 2 - 100) * window_size.Width / 1024 - sx / 2 + 100;
	y = y * window_size.Height / 640;
	x2 = sx + x;
	y2 = sy + y;
	return recti(x, y, x2, y2);
}
void Game::ValidateName(irr::gui::IGUIEditBox* box) {
	stringw text = box->getText();
	wchar_t filtered[256];
	int j = 0;
	for (int i = 0; text[i]; i++)
		if (!(text[i] == L'<' || text[i] == L'>' || text[i] == L':' || text[i] == L'"' || text[i] == L'/' || text[i] == L'\\' || text[i] == L'|' || text[i] == L'?' || text[i] == L'*' || text[i] == L'<')) {
			filtered[j] = text[i];
			j++;
		}
	filtered[j] = 0;
	box->setText(filtered);
}

}
