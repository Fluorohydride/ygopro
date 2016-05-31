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

#ifndef _WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

const unsigned short PRO_VERSION = 0x1339;

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
	device = irr::createDeviceEx(params);
	if(!device)
		return false;
	linePattern = 0x0f0f;
	waitFrame = 0;
	signalFrame = 0;
	showcard = 0;
	is_attacking = false;
	lpframe = 0;
	lpcstring = 0;
	always_chain = false;
	ignore_chain = false;
	is_building = false;
	memset(&dInfo, 0, sizeof(DuelInfo));
	memset(chatTiming, 0, sizeof(chatTiming));
	deckManager.LoadLFList();
	driver = device->getVideoDriver();
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	imageManager.SetDevice(device);
	if(!imageManager.Initial())
		return false;
	if(!dataManager.LoadDB("cards.cdb"))
		return false;
	if(!dataManager.LoadStrings("strings.conf"))
		return false;
	RefreshExpansionDB();
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 16);
	adFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 12);
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 48);
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.textfont, gameConf.textfontsize);
	textFont = guiFont;
	smgr = device->getSceneManager();
	device->setWindowCaption(L"[---]");
	device->setResizable(false);
	//main menu
	wchar_t strbuf[256];
	myswprintf(strbuf, L"YGOPro Version:%X.0%X.%X", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
	wMainMenu = env->addWindow(rect<s32>(370, 200, 650, 415), false, strbuf);
	wMainMenu->getCloseButton()->setVisible(false);
	btnLanMode = env->addButton(rect<s32>(10, 30, 270, 60), wMainMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200));
	btnServerMode = env->addButton(rect<s32>(10, 65, 270, 95), wMainMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201));
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
	ebJoinIP = env->addEditBox(gameConf.lastip, rect<s32>(110, 355, 250, 380), true, wLanWindow);
	ebJoinIP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(gameConf.lastport, rect<s32>(260, 355, 320, 380), true, wLanWindow);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1222), rect<s32>(10, 390, 220, 410), false, false, wLanWindow);
	ebJoinPass = env->addEditBox(gameConf.roompass, rect<s32>(110, 385, 320, 410), true, wLanWindow);
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
	env->addStaticText(dataManager.GetSysString(1237), rect<s32>(20, 120, 320, 140), false, false, wCreateHost);
	myswprintf(strbuf, L"%d", 180);
	ebTimeLimit = env->addEditBox(strbuf, rect<s32>(140, 115, 220, 140), true, wCreateHost);
	ebTimeLimit->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1228), rect<s32>(20, 150, 320, 170), false, false, wCreateHost);
	chkEnablePriority = env->addCheckBox(false, rect<s32>(20, 180, 360, 200), wCreateHost, -1, dataManager.GetSysString(1236));
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
	btnHostPrepDuelist = env->addButton(rect<s32>(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, dataManager.GetSysString(1251));
	for(int i = 0; i < 2; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250, 65 + i * 25, 270, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	for(int i = 2; i < 4; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 75 + i * 25, 240, 95 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 75 + i * 25, 30, 95 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250, 75 + i * 25, 270, 95 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	btnHostPrepOB = env->addButton(rect<s32>(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252));
	myswprintf(dataManager.strBuffer, L"%ls%d", dataManager.GetSysString(1253), 0);
	stHostPrepOB = env->addStaticText(dataManager.strBuffer, rect<s32>(10, 210, 270, 230), false, false, wHostPrepare);
	stHostPrepRule = env->addStaticText(L"", rect<s32>(280, 30, 460, 230), false, true, wHostPrepare);
	env->addStaticText(dataManager.GetSysString(1254), rect<s32>(10, 235, 110, 255), false, false, wHostPrepare);
	cbDeckSelect = env->addComboBox(rect<s32>(120, 230, 270, 255), wHostPrepare);
	cbDeckSelect->setMaxSelectionRows(10);
	btnHostPrepStart = env->addButton(rect<s32>(230, 280, 340, 305), wHostPrepare, BUTTON_HP_START, dataManager.GetSysString(1215));
	btnHostPrepCancel = env->addButton(rect<s32>(350, 280, 460, 305), wHostPrepare, BUTTON_HP_CANCEL, dataManager.GetSysString(1212));
	//img
	wCardImg = env->addStaticText(L"", rect<s32>(1, 1, 199, 273), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(0xc0c0c0c0);
	wCardImg->setVisible(false);
	imgCard = env->addImage(rect<s32>(9, 9, 187, 262), wCardImg);
	imgCard->setUseAlphaChannel(true);
	//phase
	wPhase = env->addStaticText(L"", rect<s32>(480, 310, 855, 330));
	wPhase->setVisible(false);
	btnDP = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, -1, L"\xff24\xff30");
	btnDP->setEnabled(false);
	btnDP->setPressed(true);
	btnDP->setVisible(false);
	btnSP = env->addButton(rect<s32>(65, 0, 115, 20), wPhase, -1, L"\xff33\xff30");
	btnSP->setEnabled(false);
	btnSP->setPressed(true);
	btnSP->setVisible(false);
	btnM1 = env->addButton(rect<s32>(130, 0, 180, 20), wPhase, -1, L"\xff2d\xff11");
	btnM1->setEnabled(false);
	btnM1->setPressed(true);
	btnM1->setVisible(false);
	btnBP = env->addButton(rect<s32>(195, 0, 245, 20), wPhase, BUTTON_BP, L"\xff22\xff30");
	btnBP->setVisible(false);
	btnM2 = env->addButton(rect<s32>(260, 0, 310, 20), wPhase, BUTTON_M2, L"\xff2d\xff12");
	btnM2->setVisible(false);
	btnEP = env->addButton(rect<s32>(325, 0, 375, 20), wPhase, BUTTON_EP, L"\xff25\xff30");
	btnEP->setVisible(false);
	btnShuffle = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, dataManager.GetSysString(1307));
	btnShuffle->setVisible(false);
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
	chkAutoPos = env->addCheckBox(false, rect<s32>(20, 20, 280, 45), tabSystem, -1, dataManager.GetSysString(1274));
	chkAutoPos->setChecked(gameConf.chkAutoPos != 0);
	chkRandomPos = env->addCheckBox(false, rect<s32>(40, 50, 300, 75), tabSystem, -1, dataManager.GetSysString(1275));
	chkRandomPos->setChecked(gameConf.chkRandomPos != 0);
	chkAutoChain = env->addCheckBox(false, rect<s32>(20, 80, 280, 105), tabSystem, -1, dataManager.GetSysString(1276));
	chkAutoChain->setChecked(gameConf.chkAutoChain != 0);
	chkWaitChain = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, -1, dataManager.GetSysString(1277));
	chkWaitChain->setChecked(gameConf.chkWaitChain != 0);
	chkIgnore1 = env->addCheckBox(false, rect<s32>(20, 170, 280, 195), tabSystem, -1, dataManager.GetSysString(1290));
	chkIgnore1->setChecked(gameConf.chkIgnore1 != 0);
	chkIgnore2 = env->addCheckBox(false, rect<s32>(20, 200, 280, 225), tabSystem, -1, dataManager.GetSysString(1291));
	chkIgnore2->setChecked(gameConf.chkIgnore2 != 0);
	chkHideSetname = env->addCheckBox(false, rect<s32>(20, 260, 280, 285), tabSystem, -1, dataManager.GetSysString(1354));
	chkHideSetname->setChecked(gameConf.chkHideSetname != 0);
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
	btnPSAD->setImage(imageManager.tCover[0], rect<s32>(0, 0, 177, 254));
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDD->setImageRotation(270);
	btnPSDD->setImage(imageManager.tCover[0], rect<s32>(0, 0, 177, 254));
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
	wANCard = env->addWindow(rect<s32>(560, 170, 770, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", rect<s32>(20, 25, 190, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(rect<s32>(20, 50, 190, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(rect<s32>(60, 165, 150, 190), wANCard, BUTTON_ANCARD_OK, dataManager.GetSysString(1211));
	//announce attribute
	wANAttribute = env->addWindow(rect<s32>(500, 200, 830, 285), false, dataManager.GetSysString(562));
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, dataManager.FormatAttribute(filter));
	//announce attribute
	wANRace = env->addWindow(rect<s32>(480, 200, 850, 385), false, dataManager.GetSysString(563));
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 24; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, dataManager.FormatRace(filter));
	//selection hint
	stHintMsg = env->addStaticText(L"", rect<s32>(500, 60, 820, 90), true, false, 0, -1, false);
	stHintMsg->setBackgroundColor(0xc0ffffff);
	stHintMsg->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHintMsg->setVisible(false);
	stTip = env->addStaticText(L"", rect<s32>(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setBackgroundColor(0xc0ffffff);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
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
	//deck edit
	wDeckEdit = env->addStaticText(L"", rect<s32>(309, 5, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1300), rect<s32>(10, 9, 100, 29), false, false, wDeckEdit);
	cbDBLFList = env->addComboBox(rect<s32>(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	cbDBLFList->setMaxSelectionRows(10);
	env->addStaticText(dataManager.GetSysString(1301), rect<s32>(10, 39, 100, 59), false, false, wDeckEdit);
	cbDBDecks = env->addComboBox(rect<s32>(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	cbDBDecks->setMaxSelectionRows(15);
	for(unsigned int i = 0; i < deckManager._lfList.size(); ++i)
		cbDBLFList->addItem(deckManager._lfList[i].listName);
	btnSaveDeck = env->addButton(rect<s32>(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, dataManager.GetSysString(1302));
	ebDeckname = env->addEditBox(L"", rect<s32>(80, 65, 220, 90), true, wDeckEdit, -1);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(rect<s32>(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, dataManager.GetSysString(1303));
	btnDeleteDeck = env->addButton(rect<s32>(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, dataManager.GetSysString(1308));
	btnShuffleDeck = env->addButton(rect<s32>(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307));
	btnSortDeck = env->addButton(rect<s32>(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, dataManager.GetSysString(1305));
	btnClearDeck = env->addButton(rect<s32>(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, dataManager.GetSysString(1304));
	btnSideOK = env->addButton(rect<s32>(510, 40, 820, 80), 0, BUTTON_SIDE_OK, dataManager.GetSysString(1334));
	btnSideOK->setVisible(false);
	//filters
	wFilter = env->addStaticText(L"", rect<s32>(610, 5, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1311), rect<s32>(10, 25 / 6 + 2, 70, 22 + 25 / 6), false, false, wFilter);
	cbCardType = env->addComboBox(rect<s32>(60, 25 / 6, 120, 20 + 25 / 6), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(dataManager.GetSysString(1310));
	cbCardType->addItem(dataManager.GetSysString(1312));
	cbCardType->addItem(dataManager.GetSysString(1313));
	cbCardType->addItem(dataManager.GetSysString(1314));
	cbCardType2 = env->addComboBox(rect<s32>(125, 25 / 6, 200, 20 + 25 / 6), wFilter, -1);
	cbCardType2->setMaxSelectionRows(10);
	cbCardType2->addItem(dataManager.GetSysString(1310), 0);
	env->addStaticText(dataManager.GetSysString(1315), rect<s32>(205, 2 + 25 / 6, 280, 22 + 25 / 6), false, false, wFilter);
	cbLimit = env->addComboBox(rect<s32>(260, 25 / 6, 390, 20 + 25 / 6), wFilter, -1);
	cbLimit->setMaxSelectionRows(10);
	cbLimit->addItem(dataManager.GetSysString(1310));
	cbLimit->addItem(dataManager.GetSysString(1316));
	cbLimit->addItem(dataManager.GetSysString(1317));
	cbLimit->addItem(dataManager.GetSysString(1318));
	cbLimit->addItem(dataManager.GetSysString(1240));
	cbLimit->addItem(dataManager.GetSysString(1241));
	env->addStaticText(dataManager.GetSysString(1319), rect<s32>(10, 22 + 50 / 6, 70, 42 + 50 / 6), false, false, wFilter);
	cbAttribute = env->addComboBox(rect<s32>(60, 20 + 50 / 6, 190, 40 + 50 / 6), wFilter, -1);
	cbAttribute->setMaxSelectionRows(10);
	cbAttribute->addItem(dataManager.GetSysString(1310), 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(dataManager.FormatAttribute(filter), filter);
	env->addStaticText(dataManager.GetSysString(1321), rect<s32>(10, 42 + 75 / 6, 70, 62 + 75 / 6), false, false, wFilter);
	cbRace = env->addComboBox(rect<s32>(60, 40 + 75 / 6, 190, 60 + 75 / 6), wFilter, -1);
	cbRace->setMaxSelectionRows(10);
	cbRace->addItem(dataManager.GetSysString(1310), 0);
	for(int filter = 0x1; filter != 0x1000000; filter <<= 1)
		cbRace->addItem(dataManager.FormatRace(filter), filter);
	env->addStaticText(dataManager.GetSysString(1322), rect<s32>(205, 22 + 50 / 6, 280, 42 + 50 / 6), false, false, wFilter);
	ebAttack = env->addEditBox(L"", rect<s32>(260, 20 + 50 / 6, 340, 40 + 50 / 6), true, wFilter);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1323), rect<s32>(205, 42 + 75 / 6, 280, 62 + 75 / 6), false, false, wFilter);
	ebDefense = env->addEditBox(L"", rect<s32>(260, 40 + 75 / 6, 340, 60 + 75 / 6), true, wFilter);
	ebDefense->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1324), rect<s32>(10, 62 + 100 / 6, 80, 82 + 100 / 6), false, false, wFilter);
	ebStar = env->addEditBox(L"", rect<s32>(60, 60 + 100 / 6, 190, 80 + 100 / 6), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1336), rect<s32>(10, 82 + 125 / 6, 80, 102 + 125 / 6), false, false, wFilter);
	ebScale = env->addEditBox(L"", rect<s32>(60, 80 + 125 / 6, 190, 100 + 125 / 6), true, wFilter);
	ebScale->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(dataManager.GetSysString(1325), rect<s32>(205, 62 + 100 / 6, 280, 82 + 100 / 6), false, false, wFilter);
	ebCardName = env->addEditBox(L"", rect<s32>(260, 60 + 100 / 6, 390, 80 + 100 / 6), true, wFilter, EDITBOX_KEYWORD);
	ebCardName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnEffectFilter = env->addButton(rect<s32>(345, 20 + 50 / 6, 390, 60 + 75 / 6), wFilter, BUTTON_EFFECT_FILTER, dataManager.GetSysString(1326));
	btnStartFilter = env->addButton(rect<s32>(205, 80 + 125 / 6, 390, 100 + 125 / 6), wFilter, BUTTON_START_FILTER, dataManager.GetSysString(1327));
	if(mainGame->gameConf.separate_clear_button) {
		btnStartFilter->setRelativePosition(rect<s32>(260, 80 + 125 / 6, 390, 100 + 125 / 6));
		btnClearFilter = env->addButton(rect<s32>(205, 80 + 125 / 6, 255, 100 + 125 / 6), wFilter, BUTTON_CLEAR_FILTER, dataManager.GetSysString(1304));
	}
	wCategories = env->addWindow(rect<s32>(630, 60, 1000, 270), false, dataManager.strBuffer);
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(rect<s32>(135, 175, 235, 200), wCategories, BUTTON_CATEGORY_OK, dataManager.GetSysString(1211));
	for(int i = 0; i < 32; ++i)
		chkCategory[i] = env->addCheckBox(false, recti(10 + (i % 4) * 90, 10 + (i / 4) * 20, 100 + (i % 4) * 90, 30 + (i / 4) * 20), wCategories, -1, dataManager.GetSysString(1100 + i));
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
	//replay window
	wReplay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1202));
	wReplay->getCloseButton()->setVisible(false);
	wReplay->setVisible(false);
	lstReplayList = env->addListBox(rect<s32>(10, 30, 350, 400), wReplay, LISTBOX_REPLAY_LIST, true);
	lstReplayList->setItemHeight(18);
	btnLoadReplay = env->addButton(rect<s32>(460, 355, 570, 380), wReplay, BUTTON_LOAD_REPLAY, dataManager.GetSysString(1348));
	btnReplayCancel = env->addButton(rect<s32>(460, 385, 570, 410), wReplay, BUTTON_CANCEL_REPLAY, dataManager.GetSysString(1347));
	env->addStaticText(dataManager.GetSysString(1349), rect<s32>(360, 30, 570, 50), false, true, wReplay);
	stReplayInfo = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wReplay);
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
	ebRSName =  env->addEditBox(L"", rect<s32>(20, 50, 290, 70), true, wReplaySave, -1);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(rect<s32>(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, dataManager.GetSysString(1341));
	btnRSNo = env->addButton(rect<s32>(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, dataManager.GetSysString(1212));
	//replay control
	wReplayControl = env->addStaticText(L"", rect<s32>(205, 118, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, dataManager.GetSysString(1343));
	btnReplayPause = env->addButton(rect<s32>(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_PAUSE, dataManager.GetSysString(1344));
	btnReplayStep = env->addButton(rect<s32>(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_STEP, dataManager.GetSysString(1345));
	btnReplayUndo = env->addButton(rect<s32>(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_UNDO, dataManager.GetSysString(1360));
	btnReplaySwap = env->addButton(rect<s32>(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346));
	btnReplayExit = env->addButton(rect<s32>(5, 130, 85, 150), wReplayControl, BUTTON_REPLAY_EXIT, dataManager.GetSysString(1347));
	//chat
	wChat = env->addWindow(rect<s32>(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", rect<s32>(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	//
	btnLeaveGame = env->addButton(rect<s32>(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	device->setEventReceiver(&menuHandler);
	LoadConfig();
	env->getSkin()->setFont(guiFont);
	env->setFocus(wMainMenu);
	for (u32 i = 0; i < EGDC_COUNT; ++i) {
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(224);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
	hideChat = false;
	hideChatTimer = 0;
	return true;
}
void Game::MainLoop() {
	wchar_t cap[256];
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, -0.90f, 0.45f, -0.42f, 0.42f, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);

	mProjection.buildCameraLookAtMatrixLH(vector3df(4.2f, 8.0f, 7.8f), vector3df(4.2f, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
	irr::ITimer* timer = device->getTimer();
	timer->setTime(0);
	int fps = 0;
	int cur_time = 0;
	while(device->run()) {
		if(gameConf.use_d3d)
			linePattern = (linePattern + 1) % 30;
		else
			linePattern = (linePattern << 1) | (linePattern >> 15);
		atkframe += 0.1f;
		atkdy = (float)sin(atkframe);
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		if(imageManager.tBackGround)
			driver->draw2DImage(imageManager.tBackGround, recti(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getOriginalSize().Width, imageManager.tBackGround->getOriginalSize().Height));
		gMutex.Lock();
		if(dInfo.isStarted) {
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();
		} else if(is_building) {
			if(imageManager.tBackGround_deck)
				driver->draw2DImage(imageManager.tBackGround_deck, recti(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getOriginalSize().Width, imageManager.tBackGround->getOriginalSize().Height));
			DrawDeckBd();
		} else {
			if(imageManager.tBackGround_menu)
				driver->draw2DImage(imageManager.tBackGround_menu, recti(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getOriginalSize().Width, imageManager.tBackGround->getOriginalSize().Height));
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
		if(!device->isWindowActive())
			ignore_chain = false;
		fps++;
		cur_time = timer->getTime();
		if(cur_time < fps * 17 - 20)
#ifdef _WIN32
			Sleep(20);
#else
			usleep(20000);
#endif
		if(cur_time >= 1000) {
			myswprintf(cap, L"FPS: %d", fps);
			device->setWindowCaption(cap);
			fps = 0;
			cur_time -= 1000;
			timer->setTime(0);
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
	}
	DuelClient::StopClient(true);
	if(mainGame->dInfo.isSingleMode)
		SingleMode::StopPlay(true);
#ifdef _WIN32
	Sleep(500);
#else
	usleep(500000);
#endif
	SaveConfig();
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
	SetStaticText(pControl, cWidth, font, text);
	if(font->getDimension(dataManager.strBuffer).Height <= cHeight) {
		scrCardText->setVisible(false);
		return;
	}
	SetStaticText(pControl, cWidth-25, font, text);
	u32 fontheight = font->getDimension(L"A").Height + font->getKerningHeight();
	u32 step = (font->getDimension(dataManager.strBuffer).Height - cHeight) / fontheight + 1;
	scrCardText->setVisible(true);
	scrCardText->setMin(0);
	scrCardText->setMax(step);
	scrCardText->setPos(0);
}
void Game::SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, const wchar_t* text, u32 pos) {
	int pbuffer = 0;
	u32 _width = 0, _height = 0;
	for(size_t i = 0; text[i] != 0 && i < wcslen(text); ++i) {
		u32 w = font->getCharDimension(text[i]).Width;
		if(text[i] == L'\n') {
			dataManager.strBuffer[pbuffer++] = L'\n';
			_width = 0;
			_height++;
			if(_height == pos)
				pbuffer = 0;
			continue;
		} else if(_width > 0 && _width + w > cWidth) {
			dataManager.strBuffer[pbuffer++] = L'\n';
			_width = 0;
			_height++;
			if(_height == pos)
				pbuffer = 0;
		}
		_width += w;
		dataManager.strBuffer[pbuffer++] = text[i];
	}
	dataManager.strBuffer[pbuffer] = 0;
	pControl->setText(dataManager.strBuffer);
}
void Game::RefreshExpansionDB() {
#ifdef _WIN32
	char fpath[1000];
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./expansions/*.cdb", &fdataw);
	if(fh != INVALID_HANDLE_VALUE) {
		do {
			if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				sprintf(fpath, "./expansions/%ls", fdataw.cFileName);
				dataManager.LoadDB(fpath);
			}
		} while(FindNextFileW(fh, &fdataw));
		FindClose(fh);
	}
#else
	DIR * dir;
	struct dirent * dirp;
	const char *foldername = "./expansions/";
	if((dir = opendir(foldername)) != NULL) {
		while((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".cdb") != 0)
				continue;
			char *filepath = (char *)malloc(sizeof(char)*(len + strlen(foldername)));
			strncpy(filepath, foldername, strlen(foldername) + 1);
			strncat(filepath, dirp->d_name, len);
			std::cout << "Found file " << filepath << std::endl;
			if(!dataManager.LoadDB(filepath))
				std::cout << "Error loading file" << std::endl;
			free(filepath);
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
void Game::LoadConfig() {
	FILE* fp = fopen("system.conf", "r");
	if(!fp)
		return;
	char linebuf[256];
	char strbuf[32];
	char valbuf[256];
	wchar_t wstr[256];
	gameConf.antialias = 0;
	gameConf.serverport = 7911;
	gameConf.textfontsize = 12;
	gameConf.nickname[0] = 0;
	gameConf.gamename[0] = 0;
	gameConf.lastdeck[0] = 0;
	gameConf.numfont[0] = 0;
	gameConf.textfont[0] = 0;
	gameConf.lastip[0] = 0;
	gameConf.lastport[0] = 0;
	gameConf.roompass[0] = 0;
	//settings
	gameConf.chkAutoPos = 1;
	gameConf.chkRandomPos = 0;
	gameConf.chkAutoChain = 0;
	gameConf.chkWaitChain = 0;
	gameConf.chkIgnore1 = 0;
	gameConf.chkIgnore2 = 0;
	gameConf.chkHideSetname = 0;
	gameConf.control_mode = 0;
	gameConf.draw_field_spell = 1;
	gameConf.separate_clear_button = 1;
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	while(ftell(fp) < fsize) {
		fgets(linebuf, 256, fp);
		sscanf(linebuf, "%s = %s", strbuf, valbuf);
		if(!strcmp(strbuf, "antialias")) {
			gameConf.antialias = atoi(valbuf);
		} else if(!strcmp(strbuf, "use_d3d")) {
			gameConf.use_d3d = atoi(valbuf) > 0;
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
		} else if(!strcmp(strbuf, "lastip")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastip, 20);
		} else if(!strcmp(strbuf, "lastport")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastport, 20);
		} else if(!strcmp(strbuf, "roompass")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.roompass, 20);
		} else if(!strcmp(strbuf, "autopos")) {
			gameConf.chkAutoPos = atoi(valbuf);
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
		} else if(!strcmp(strbuf, "control_mode")) {
			gameConf.control_mode = atoi(valbuf);
		} else if(!strcmp(strbuf, "draw_field_spell")) {
			gameConf.draw_field_spell = atoi(valbuf);
		} else if(!strcmp(strbuf, "separate_clear_button")) {
			gameConf.separate_clear_button = atoi(valbuf);
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
	BufferIO::EncodeUTF8(gameConf.lastip, linebuf);
	fprintf(fp, "lastip = %s\n", linebuf);
	BufferIO::EncodeUTF8(gameConf.lastport, linebuf);
	fprintf(fp, "lastport = %s\n", linebuf);
	//settings
	fprintf(fp, "autopos = %d\n", ((mainGame->chkAutoPos->isChecked()) ? 1 : 0));
	fprintf(fp, "randompos = %d\n", ((mainGame->chkRandomPos->isChecked()) ? 1 : 0));
	fprintf(fp, "autochain = %d\n", ((mainGame->chkAutoChain->isChecked()) ? 1 : 0));
	fprintf(fp, "waitchain = %d\n", ((mainGame->chkWaitChain->isChecked()) ? 1 : 0));
	fprintf(fp, "mute_opponent = %d\n", ((mainGame->chkIgnore1->isChecked()) ? 1 : 0));
	fprintf(fp, "mute_spectators = %d\n", ((mainGame->chkIgnore2->isChecked()) ? 1 : 0));
	fprintf(fp, "hide_setname = %d\n", ((mainGame->chkHideSetname->isChecked()) ? 1 : 0));
	fprintf(fp, "#control_mode = 0: Key A/S/R. control_mode = 1: MouseLeft/MouseRight/F9\n");
	fprintf(fp, "control_mode = %d\n", gameConf.control_mode);
	fprintf(fp, "draw_field_spell = %d\n", gameConf.draw_field_spell);
	fprintf(fp, "separate_clear_button = %d\n", gameConf.separate_clear_button);
	fclose(fp);
}
void Game::ShowCardInfo(int code) {
	CardData cd;
	wchar_t formatBuffer[256];
	if(!dataManager.GetData(code, &cd))
		memset(&cd, 0, sizeof(CardData));
	imgCard->setImage(imageManager.GetTexture(code));
	imgCard->setScaleImage(true);
	if(cd.alias != 0 && (cd.alias - code < 10 || code - cd.alias < 10))
		myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(cd.alias), cd.alias);
	else myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(code), code);
	stName->setText(formatBuffer);
	int offset = 0;
	if(!mainGame->chkHideSetname->isChecked()) {
		unsigned long long sc = cd.setcode;
		if(cd.alias) {
			auto aptr = dataManager._datas.find(cd.alias);
			if(aptr != dataManager._datas.end())
				sc = aptr->second.setcode;
		}
		if(sc) {
			offset = 23;
			myswprintf(formatBuffer, L"%ls%ls", dataManager.GetSysString(1329), dataManager.FormatSetName(sc));
			stSetName->setText(formatBuffer);
		} else
			stSetName->setText(L"");
	} else {
		stSetName->setText(L"");
	}
	if(cd.type & TYPE_MONSTER) {
		myswprintf(formatBuffer, L"[%ls] %ls/%ls", dataManager.FormatType(cd.type), dataManager.FormatRace(cd.race), dataManager.FormatAttribute(cd.attribute));
		stInfo->setText(formatBuffer);
		int form = 0x2605;
		if(cd.type & TYPE_XYZ) ++form;
		myswprintf(formatBuffer, L"[%c%d] ", form, cd.level);
		wchar_t adBuffer[16];
		if(cd.attack < 0 && cd.defense < 0)
			myswprintf(adBuffer, L"?/?");
		else if(cd.attack < 0)
			myswprintf(adBuffer, L"?/%d", cd.defense);
		else if(cd.defense < 0)
			myswprintf(adBuffer, L"%d/?", cd.attack);
		else
			myswprintf(adBuffer, L"%d/%d", cd.attack, cd.defense);
		wcscat(formatBuffer, adBuffer);
		if(cd.type & TYPE_PENDULUM) {
			wchar_t scaleBuffer[16];
			myswprintf(scaleBuffer, L"   %d/%d", cd.lscale, cd.rscale);
			wcscat(formatBuffer, scaleBuffer);
		}
		stDataInfo->setText(formatBuffer);
		stSetName->setRelativePosition(rect<s32>(15, 83, 296, 106));
		stText->setRelativePosition(rect<s32>(15, 83 + offset, 287, 324));
		scrCardText->setRelativePosition(rect<s32>(267, 83 + offset, 287, 324));
	} else {
		myswprintf(formatBuffer, L"[%ls]", dataManager.FormatType(cd.type));
		stInfo->setText(formatBuffer);
		stDataInfo->setText(L"");
		stSetName->setRelativePosition(rect<s32>(15, 60, 296, 83));
		stText->setRelativePosition(rect<s32>(15, 60 + offset, 287, 324));
		scrCardText->setRelativePosition(rect<s32>(267, 60 + offset, 287, 324));
	}
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
	case 0: //from host
		chatMsg[0].append(dInfo.hostname);
		chatMsg[0].append(L": ");
		break;
	case 1: //from client
		chatMsg[0].append(dInfo.clientname);
		chatMsg[0].append(L": ");
		break;
	case 2: //host tag
		chatMsg[0].append(dInfo.hostname_tag);
		chatMsg[0].append(L": ");
		break;
	case 3: //client tag
		chatMsg[0].append(dInfo.clientname_tag);
		chatMsg[0].append(L": ");
		break;
	case 7: //local name
		chatMsg[0].append(mainGame->ebNickName->getText());
		chatMsg[0].append(L": ");
		break;
	case 8: //system custom message, no prefix.
		chatMsg[0].append(L"[System]: ");
		break;
	case 9: //error message
		chatMsg[0].append(L"[Script error:] ");
		break;
	default: //from watcher or unknown
		if(player < 11 || player > 19)
			chatMsg[0].append(L"[---]: ");
	}
	chatMsg[0].append(msg);
}
void Game::ClearTextures() {
	matManager.mCard.setTexture(0, 0);
	mainGame->imgCard->setImage(0);
	mainGame->btnPSAU->setImage();
	mainGame->btnPSDU->setImage();
	for(int i=0; i<=4; ++i) {
		mainGame->btnCardSelect[i]->setImage();
		mainGame->btnCardDisplay[i]->setImage();
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
	wChat->setVisible(false);
	lstLog->clear();
	logParam.clear();
	lstHostList->clear();
	DuelClient::hosts.clear();
	ClearTextures();
	closeDoneSignal.Set();
}
int Game::LocalPlayer(int player) {
	return dInfo.isFirst ? player : 1 - player;
}
const wchar_t* Game::LocalName(int local_player) {
	return local_player == 0 ? dInfo.hostname : dInfo.clientname;
}

}
