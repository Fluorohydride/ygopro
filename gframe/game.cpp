#include "config.h"
#include "game.h"
#include "network.h"

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

namespace ygo {

Game* mainGame;

Game::Game() {
}
Game::~Game() {
}
bool Game::Initialize() {
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = 2;
	params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<u32>(1024, 640);
	device = irr::createDeviceEx(params);
	if(!device)
		return false;
	linePattern = 0x0f0f;
	waitFrame = 0;
	fadingFrame = 0;
	autoFadeoutFrame = 0;
	signalFrame = 0;
	showcard = 0;
	is_attacking = false;
	lpframe = 0;
	lpcstring = 0;
	always_chain = false;
	ignore_chain = false;
	is_building = false;
	dField.device = device;
	deckBuilder.device = device;
	memset(&dInfo, 0, sizeof(DuelInfo));
	netManager.local_addr = NetManager::GetLocalAddress();
	netManager.send_buffer_ptr = &netManager.send_buf[2];
	deckManager.LoadLFList();
	driver = device->getVideoDriver();
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	vdata = driver->getExposedVideoData();
	ime.Init(vdata);
	imageManager.SetDevice(device);
	if(!dataManager.LoadDates("cards.cdb"))
		return false;
	if(!imageManager.Initial())
		return false;
	env = device->getGUIEnvironment();
	smgr = device->getSceneManager();
	device->setWindowCaption(L"[---]");
	device->setResizable(false);
	myswprintf(dataManager.strBuffer, L"模式选择(当前IP:%d.%d.%d.%d  版本:0x%X)", netManager.local_addr & 0xff, (netManager.local_addr >> 8) & 0xff,
	           (netManager.local_addr >> 16) & 0xff, (netManager.local_addr >> 24) & 0xff, PROTO_VERSION);
	wModeSelection = env->addWindow(rect<s32>(270, 100, 750, 490), false, dataManager.strBuffer);
	wModeSelection->getCloseButton()->setVisible(false);
	wModes = env->addTabControl(rect<s32>(5, 60, 475, 350), wModeSelection, false, true, TAB_MODES);
	irr::gui::IGUITab* tabLanS = wModes->addTab(L"建立主机");
	irr::gui::IGUITab* tabLanC = wModes->addTab(L"加入游戏");
	irr::gui::IGUITab* tabReplay = wModes->addTab(L"观看录像");
	chkNoCheckDeck = env->addCheckBox(false, rect<s32>(10, 10, 210, 30), tabLanS, -1, L"不检查卡组");
	chkNoShuffleDeck = env->addCheckBox(false, rect<s32>(10, 35, 210, 55), tabLanS, -1, L"开局不洗卡组");
	chkNoShufflePlayer = env->addCheckBox(false, rect<s32>(10, 60, 210, 80), tabLanS, -1, L"主机固定先攻");
	chkAttackFT = env->addCheckBox(false, rect<s32>(10, 85, 210, 105), tabLanS, -1, L"第一回合可以攻击");
	chkNoChainHint = env->addCheckBox(false, rect<s32>(10, 110, 210, 130), tabLanS, -1, L"没有可用连锁时不等待");
	env->addStaticText(L"起始ＬＰ：", rect<s32>(200, 10, 300, 30), false, false, tabLanS);
	myswprintf(dataManager.strBuffer, L"%d", 8000);
	ebStartLP = env->addEditBox(dataManager.strBuffer, rect<s32>(310, 10, 390, 30), true, tabLanS);
	ebStartLP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"起始手牌：", rect<s32>(200, 35, 300, 55), false, false, tabLanS);
	myswprintf(dataManager.strBuffer, L"%d", 5);
	ebStartHand = env->addEditBox(dataManager.strBuffer, rect<s32>(310, 35, 390, 55), true, tabLanS);
	ebStartHand->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"每回合抽卡：", rect<s32>(200, 60, 300, 80), false, false, tabLanS);
	myswprintf(dataManager.strBuffer, L"%d", 1);
	ebDrawCount = env->addEditBox(dataManager.strBuffer, rect<s32>(310, 60, 390, 80), true, tabLanS);
	ebDrawCount->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"禁限卡表：", rect<s32>(200, 85, 300, 105), false, false, tabLanS);
	cbLFlist = env->addComboBox(rect<s32>(310, 85, 450, 105), tabLanS);
	for(int i = 0; i < deckManager._lfList.size(); ++i)
		cbLFlist->addItem(deckManager._lfList[i].listName);
	//env->addStaticText(L"决斗模式：", rect<s32>(200, 110, 300, 130), false, false, tabLanS);
	cbMatchMode = env->addComboBox(rect<s32>(310, 110, 450, 130), tabLanS);
	cbMatchMode->addItem(L"单局模式");
	cbMatchMode->addItem(L"比赛模式");
	cbMatchMode->setVisible(false);
	//env->addStaticText(L"回合时间：", rect<s32>(200, 135, 300, 155), false, false, tabLanS);
	cbTurnTime = env->addComboBox(rect<s32>(310, 135, 450, 155), tabLanS);
	cbTurnTime->addItem(L"无限制");
	cbTurnTime->addItem(L"５分钟");
	cbTurnTime->addItem(L"３分钟");
	cbTurnTime->addItem(L"１分钟");
	cbTurnTime->setVisible(false);
	env->addStaticText(L"游戏名：", rect<s32>(10, 205, 100, 225), false, false, tabLanS);
	ebServerName = env->addEditBox(L"Game", rect<s32>(100, 205, 240, 225), true, tabLanS);
	ebServerName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"密码：", rect<s32>(10, 230, 100, 250), false, false, tabLanS);
	ebServerPass = env->addEditBox(L"", rect<s32>(100, 230, 240, 250), true, tabLanS);
	ebServerPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnLanStartServer = env->addButton(rect<s32>(350, 195, 460, 220), tabLanS, BUTTON_LAN_START_SERVER, L"建立主机");
	btnLanCancelServer = env->addButton(rect<s32>(350, 225, 460, 250), tabLanS, BUTTON_LAN_CANCEL_SERVER, L"取消主机");
	btnLanCancelServer->setEnabled(false);
	lstServerList = env->addListBox(rect<s32>(10, 10, 460, 136), tabLanC, LISTBOX_SERVER_LIST, true);
	lstServerList->setItemHeight(18);
	btnRefreshList = env->addButton(rect<s32>(180, 145, 280, 170), tabLanC, BUTTON_LAN_REFRESH, L"刷新");
	chkStOnly = env->addCheckBox(true, rect<s32>(10, 180, 220, 200), tabLanC, -1, L"只连接标准模式的主机");
	env->addStaticText(L"主机地址：", rect<s32>(10, 205, 120, 225), false, false, tabLanC);
	ebJoinIP = env->addEditBox(L"", rect<s32>(100, 205, 240, 225), true, tabLanC);
	ebJoinIP->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	ebJoinPort = env->addEditBox(L"", rect<s32>(245, 205, 305, 225), true, tabLanC);
	ebJoinPort->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"游戏密码：", rect<s32>(10, 230, 120, 250), false, false, tabLanC);
	ebJoinPass = env->addEditBox(L"", rect<s32>(100, 230, 240, 250), true, tabLanC);
	ebJoinPass->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnLanConnect = env->addButton(rect<s32>(350, 225, 460, 250), tabLanC, BUTTON_LAN_CONNECT, L"加入游戏");
	lstReplayList = env->addListBox(rect<s32>(10, 10, 460, 190), tabReplay, LISTBOX_REPLAY_LIST, true);
	lstReplayList->setItemHeight(18);
	btnLoadReplay = env->addButton(rect<s32>(180, 200, 280, 225), tabReplay, BUTTON_LOAD_REPLAY, L"载入录像");
	env->addStaticText(L"昵称：", rect<s32>(10, 30, 90, 50), false, false, wModeSelection);
	ebUsername = env->addEditBox(L"", rect<s32>(80, 25, 260, 50), true, wModeSelection);
	ebUsername->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"卡组：", rect<s32>(270, 30, 350, 50), false, false, wModeSelection);
	cbDeckSel = env->addComboBox(rect<s32>(320, 25, 470, 50), wModeSelection, -1);
	RefreshDeck(cbDeckSel);
	btnDeckEdit = env->addButton(rect<s32>(410, 55, 470, 80), wModeSelection, BUTTON_DECK_EDIT, L"编辑");
	stModeStatus = env->addStaticText(L"", rect<s32>(20, 360, 350, 380), false, false, wModeSelection);
	btnModeExit = env->addButton(rect<s32>(380, 355, 470, 380), wModeSelection, BUTTON_MODE_EXIT, L"退出");
	//img
	wCardImg = env->addStaticText(L"", rect<s32>(1, 1, 199, 273), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(0xc0c0c0c0);
	wCardImg->setVisible(false);
	imgCard = env->addImage(rect<s32>(9, 9, 187, 262), wCardImg);
	imgCard->setUseAlphaChannel(true);
	//phase
	btnDP = env->addButton(rect<s32>(475, 310, 525, 330), 0, -1, L"ＤＰ");
	btnDP->setEnabled(false);
	btnDP->setPressed(true);
	btnDP->setVisible(false);
	btnSP = env->addButton(rect<s32>(540, 310, 590, 330), 0, -1, L"ＳＰ");
	btnSP->setEnabled(false);
	btnSP->setPressed(true);
	btnSP->setVisible(false);
	btnM1 = env->addButton(rect<s32>(605, 310, 655, 330), 0, -1, L"Ｍ１");
	btnM1->setEnabled(false);
	btnM1->setPressed(true);
	btnM1->setVisible(false);
	btnBP = env->addButton(rect<s32>(670, 310, 720, 330), 0, BUTTON_BP, L"ＢＰ");
	btnBP->setVisible(false);
	btnM2 = env->addButton(rect<s32>(735, 310, 785, 330), 0, BUTTON_M2, L"Ｍ２");
	btnM2->setVisible(false);
	btnEP = env->addButton(rect<s32>(800, 310, 850, 330), 0, BUTTON_EP, L"ＥＰ");
	btnEP->setVisible(false);
	//tab
	wInfos = env->addTabControl(rect<s32>(1, 275, 301, 639), 0, true);
	wInfos->setVisible(false);
	//info
	irr::gui::IGUITab* tabInfo = wInfos->addTab(L"卡片信息");
	stName = env->addStaticText(L"", rect<s32>(10, 10, 287, 32), true, false, tabInfo, -1, false);
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stInfo = env->addStaticText(L"", rect<s32>(15, 37, 296, 60), false, true, tabInfo, -1, false);
	stInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stDataInfo = env->addStaticText(L"", rect<s32>(15, 60, 296, 83), false, true, tabInfo, -1, false);
	stDataInfo->setOverrideColor(SColor(255, 0, 0, 255));
	stText = env->addStaticText(L"", rect<s32>(15, 83, 296, 324), false, true, tabInfo, -1, false);
	//log
	irr::gui::IGUITab* tabLog =  wInfos->addTab(L"消息记录");
	lstLog = env->addListBox(rect<s32>(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(18);
	btnClearLog = env->addButton(rect<s32>(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, L"清除记录");
	btnSaveLog = env->addButton(rect<s32>(40, 300, 140, 325), tabLog, BUTTON_SAVE_LOG, L"保存记录");
	btnSaveLog->setVisible(false);
	//system
	irr::gui::IGUITab* tabSystem = wInfos->addTab(L"系统设定");
	chkAutoPos = env->addCheckBox(false, rect<s32>(20, 20, 280, 45), tabSystem, -1, L"自动选择卡片位置");
	chkAutoPos->setChecked(true);
	chkRandomPos = env->addCheckBox(false, rect<s32>(40, 50, 300, 75), tabSystem, -1, L"↑随机选择位置");
	chkAutoChain = env->addCheckBox(false, rect<s32>(20, 80, 280, 105), tabSystem, -1, L"自动排列连锁顺序");
	chkAutoChain->setChecked(true);
	chkWaitChain = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, -1, L"没有可连锁的卡时延迟回应");
	//message (310)
	wMessage = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"消息");
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wMessage, -1, false);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(rect<s32>(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, L"确定");
	//auto fade message (310)
	wACMessage = env->addWindow(rect<s32>(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = env->addStaticText(L"", rect<s32>(0, 0, 350, 60), true, true, wACMessage, -1, true);
	stACMessage->setBackgroundColor(0xc0c0c0ff);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"请选择：");
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wQuery, -1, false);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(rect<s32>(100, 105, 150, 130), wQuery, BUTTON_YES, L"是");
	btnNo = env->addButton(rect<s32>(200, 105, 250, 130), wQuery, BUTTON_NO, L"否");
	//options (310)
	wOptions = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wOptions, -1, false);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(rect<s32>(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, L"确定");
	btnOptionp = env->addButton(rect<s32>(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, L"<<<");
	btnOptionn = env->addButton(rect<s32>(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, L">>>");
	//pos select
	wPosSelect = env->addWindow(rect<s32>(340, 200, 935, 410), false, L"请选择表示形式");
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	btnPSAU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(10, 45, 150, 185), wPosSelect, BUTTON_POS_AU);
	btnPSAU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(155, 45, 295, 185), wPosSelect, BUTTON_POS_AD);
	btnPSAD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD->setImage(imageManager.tCover, rect<s32>(0, 0, 177, 254));
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDD->setImageRotation(270);
	btnPSDD->setImage(imageManager.tCover, rect<s32>(0, 0, 177, 254));
	//announce number
	//card select
	wCardSelect = env->addWindow(rect<s32>(320, 100, 1000, 400), false, L"请选择卡");
	wCardSelect->getCloseButton()->setVisible(false);
	wCardSelect->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stCardPos[i] = env->addStaticText(L"", rect<s32>(40 + 125 * i, 30, 139 + 125 * i, 50), true, false, wCardSelect, -1, true);
		stCardPos[i]->setBackgroundColor(0xffffffff);
		stCardPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardSelect[i] = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(30 + 125 * i, 55, 150 + 125 * i, 225), wCardSelect, BUTTON_CARD_0 + i);
		btnCardSelect[i]->setImageScale(core::vector2df(0.6f, 0.6f));
	}
	scrCardList = env->addScrollBar(true, rect<s32>(30, 235, 650, 255), wCardSelect, SCROLL_CARD_SELECT);
	btnSelectOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, L"确定");
	//announce number
	wANNumber = env->addWindow(rect<s32>(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(rect<s32>(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(rect<s32>(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, L"确定");
	//announce card
	wANCard = env->addWindow(rect<s32>(570, 200, 760, 295), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", rect<s32>(20, 30, 170, 50), true, wANCard, -1);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANCardOK = env->addButton(rect<s32>(60, 60, 130, 85), wANCard, BUTTON_ANCARD_OK, L"确定");
	//announce attribute
	wANAttribute = env->addWindow(rect<s32>(500, 200, 830, 285), false, L"请选择要宣言的属性：");
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, DataManager::FormatAttribute(filter));
	//announce attribute
	wANRace = env->addWindow(rect<s32>(480, 200, 850, 385), false, L"请选择要宣言的种族：");
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 22; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, DataManager::FormatRace(filter));
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
	btnActivate = env->addButton(rect<s32>(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, L"发动");
	btnSummon = env->addButton(rect<s32>(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, L"召唤");
	btnSPSummon = env->addButton(rect<s32>(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, L"特殊召唤");
	btnMSet = env->addButton(rect<s32>(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, L"放置");
	btnSSet = env->addButton(rect<s32>(1, 85, 99, 105), wCmdMenu, BUTTON_CMD_SSET, L"放置");
	btnRepos = env->addButton(rect<s32>(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, L"反转召唤");
	btnAttack = env->addButton(rect<s32>(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, L"攻击");
	btnShowList = env->addButton(rect<s32>(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, L"查看列表");
	//deck edit
	wDeckEdit = env->addStaticText(L"", rect<s32>(309, 8, 605, 130), true, false, 0, -1, true);
	wDeckEdit->setVisible(false);
	env->addStaticText(L"禁限卡表：", rect<s32>(10, 9, 100, 29), false, false, wDeckEdit);
	cbDBLFList = env->addComboBox(rect<s32>(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
	env->addStaticText(L"卡组列表：", rect<s32>(10, 39, 100, 59), false, false, wDeckEdit);
	cbDBDecks = env->addComboBox(rect<s32>(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
	for(int i = 0; i < deckManager._lfList.size(); ++i)
		cbDBLFList->addItem(deckManager._lfList[i].listName);
	for(int i = 0; i < cbDeckSel->getItemCount(); ++i)
		cbDBDecks->addItem(cbDeckSel->getItem(i));
	btnSaveDeck = env->addButton(rect<s32>(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, L"保存");
	ebDeckname = env->addEditBox(L"", rect<s32>(80, 65, 220, 90), true, wDeckEdit, -1);
	ebDeckname->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnSaveDeckAs = env->addButton(rect<s32>(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, L"另存");
	btnClearDeck = env->addButton(rect<s32>(225, 95, 290, 116), wDeckEdit, BUTTON_CLEAR_DECK, L"清空");
	btnSortDeck = env->addButton(rect<s32>(155, 95, 220, 116), wDeckEdit, BUTTON_SORT_DECK, L"排序");
	btnDBExit = env->addButton(rect<s32>(10, 95, 90, 116), wDeckEdit, BUTTON_DBEXIT, L"退出编辑");
	//filters
	wFilter = env->addStaticText(L"", rect<s32>(610, 8, 1020, 130), true, false, 0, -1, true);
	wFilter->setVisible(false);
	env->addStaticText(L"卡种：", rect<s32>(10, 5, 70, 25), false, false, wFilter);
	cbCardType = env->addComboBox(rect<s32>(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
	cbCardType->addItem(L"(无)");
	cbCardType->addItem(L"怪兽");
	cbCardType->addItem(L"魔法");
	cbCardType->addItem(L"陷阱");
	cbCardType2 = env->addComboBox(rect<s32>(130, 3, 190, 23), wFilter, -1);
	cbCardType2->addItem(L"(无)", 0);
	env->addStaticText(L"系列：", rect<s32>(210, 5, 280, 25), false, false, wFilter);
	cbCardClass = env->addComboBox(rect<s32>(260, 3, 390, 23), wFilter, -1);
	cbCardClass->addItem(L"(无)", 0);
	for(auto ssit = dataManager._seriesStrings.begin(); ssit != dataManager._seriesStrings.end(); ++ssit)
		cbCardClass->addItem(ssit->second, ssit->first);
	env->addStaticText(L"属性：", rect<s32>(10, 28, 70, 48), false, false, wFilter);
	cbAttribute = env->addComboBox(rect<s32>(60, 26, 190, 46), wFilter, -1);
	cbAttribute->addItem(L"(无)", 0);
	for(int filter = 0x1; filter != 0x80; filter <<= 1)
		cbAttribute->addItem(DataManager::FormatAttribute(filter), filter);
	env->addStaticText(L"种族：", rect<s32>(210, 28, 280, 48), false, false, wFilter);
	cbRace = env->addComboBox(rect<s32>(260, 26, 390, 46), wFilter, -1);
	cbRace->addItem(L"(无)", 0);
	for(int filter = 0x1; filter != 0x400000; filter <<= 1)
		cbRace->addItem(DataManager::FormatRace(filter), filter);
	env->addStaticText(L"攻击：", rect<s32>(10, 51, 70, 71), false, false, wFilter);
	ebAttack = env->addEditBox(L"", rect<s32>(60, 49, 190, 69), true, wFilter);
	ebAttack->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"守备：", rect<s32>(10, 74, 70, 94), false, false, wFilter);
	ebDefence = env->addEditBox(L"", rect<s32>(60, 72, 190, 92), true, wFilter);
	ebDefence->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"限制：", rect<s32>(210, 51, 280, 71), false, false, wFilter);
	cbLimit = env->addComboBox(rect<s32>(260, 49, 340, 69), wFilter, -1);
	cbLimit->addItem(L"(无)");
	cbLimit->addItem(L"禁止");
	cbLimit->addItem(L"限制");
	cbLimit->addItem(L"准限制");
	env->addStaticText(L"星数：", rect<s32>(210, 74, 280, 94), false, false, wFilter);
	ebStar = env->addEditBox(L"", rect<s32>(260, 72, 340, 92), true, wFilter);
	ebStar->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	env->addStaticText(L"密码：", rect<s32>(10, 97, 70, 117), false, false, wFilter);
	ebCardCode = env->addEditBox(L"", rect<s32>(60, 95, 150, 115), true, wFilter);
	ebCardCode->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnCodeDir = env->addButton(rect<s32>(155, 95, 190, 115), wFilter, BUTTON_GETCODE, L"<--");
	btnEffectFilter = env->addButton(rect<s32>(345, 49, 390, 92), wFilter, BUTTON_EFFECT_FILTER, L"效果");
	btnStartFilter = env->addButton(rect<s32>(210, 96, 260, 118), wFilter, BUTTON_START_FILTER, L"搜索");
	btnResultFilter = env->addButton(rect<s32>(290, 96, 390, 118), wFilter, BUTTON_RESULT_FILTER, L"结果中搜索");
	wCategories = env->addWindow(rect<s32>(630, 80, 1000, 290), false, dataManager.strBuffer);
	wCategories->getCloseButton()->setVisible(false);
	wCategories->setDrawTitlebar(false);
	wCategories->setDraggable(false);
	wCategories->setVisible(false);
	btnCategoryOK = env->addButton(rect<s32>(135, 175, 235, 200), wCategories, BUTTON_CATEGORY_OK, L"确定");
	for(int i = 0; i < 32; ++i)
		chkCategory[i] = env->addCheckBox(false, recti(10 + (i % 4) * 90, 10 + (i / 4) * 20, 100 + (i % 4) * 90, 30 + (i / 4) * 20), wCategories, -1, DataManager::effect_strings[i]);
	scrFilter = env->addScrollBar(false, recti(999, 161, 1019, 629), 0, SCROLL_FILTER);
	scrFilter->setLargeStep(10);
	scrFilter->setSmallStep(1);
	scrFilter->setVisible(false);
	//replay save
	//yes/no (310)
	wReplaySave = env->addWindow(rect<s32>(510, 200, 820, 320), false, L"是否要保存Replay？");
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
	env->addStaticText(L"Replay文件：", rect<s32>(20, 25, 290, 45), false, false, wReplaySave);
	ebRSName =  env->addEditBox(L"", rect<s32>(20, 50, 290, 70), true, wReplaySave, -1);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(rect<s32>(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, L"保存");
	btnRSNo = env->addButton(rect<s32>(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, L"取消");
	//replay control
	wReplay = env->addStaticText(L"", rect<s32>(205, 143, 295, 273), true, false, 0, -1, true);
	wReplay->setVisible(false);
	btnReplayStart = env->addButton(rect<s32>(5, 5, 85, 25), wReplay, BUTTON_REPLAY_START, L"播放");
	btnReplayPause = env->addButton(rect<s32>(5, 30, 85, 50), wReplay, BUTTON_REPLAY_PAUSE, L"暂停");
	btnReplayStep = env->addButton(rect<s32>(5, 55, 85, 75), wReplay, BUTTON_REPLAY_STEP, L"下一步");
	btnReplaySwap = env->addButton(rect<s32>(5, 80, 85, 100), wReplay, BUTTON_REPLAY_SWAP, L"切换视角");
	btnReplayExit = env->addButton(rect<s32>(5, 105, 85, 125), wReplay, BUTTON_REPLAY_EXIT, L"退出");
	device->setEventReceiver(&dField);
	LoadConfig();
	env->getSkin()->setFont(guiFont);
	for (u32 i = 0; i < EGDC_COUNT; ++i) {
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(224);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
	return true;
}
void Game::MainLoop() {
	int fps = 0;
	wchar_t cap[256];
	gTimer.Reset();
	float st, ed;
	is_closing = false;
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, -0.81f, 0.44f, -0.42f, 0.42f, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);
	mProjection.buildCameraLookAtMatrixLH(vector3df(3.95f, 7.0f, 9.0f), vector3df(3.95f, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	irr::scene::ILightSceneNode* light = smgr->addLightSceneNode(0, vector3df(0, 0, 100));
	light->getLightData().AmbientColor = SColorf(1.0f, 1.0f, 1.0f);
	light->getLightData().DiffuseColor = SColorf(0.0f, 0.0f, 0.0f);
	float atkframe = 0.1f;
	while(device->run()) {
		linePattern = (linePattern << 1) | (linePattern >> 15);
		atkframe += 0.1f;
		atkdy = (float)sin(atkframe);
		st = gTimer.GetElapsedTime();
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		if(imageManager.tBackGround)
			driver->draw2DImage(imageManager.tBackGround, recti(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getSize().Width, imageManager.tBackGround->getSize().Height));
		gMutex.Lock();
		if(dInfo.isStarted) {
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();
		} else if(is_building) {
			DrawDeckBd();
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
				stHintMsg->setText(L"等待对方行动中...");
			} else if(waitFrame % 90 == 30) {
				stHintMsg->setText(L" 等待对方行动中....");
			} else if(waitFrame % 90 == 60) {
				stHintMsg->setText(L"  等待对方行动中.....");
			}
		}
		driver->endScene();
		fps++;
		ed = gTimer.GetElapsedTime();
		if(ed - st < 16900) {
			gTimer.Wait(16900 + st - ed);
		}
		if(ed >= 1000000.0f) {
			myswprintf(cap, L"FPS: %d", fps);
			device->setWindowCaption(cap);
			fps = 0;
			gTimer.Reset();
		}
	}
	is_closing = true;
	shutdown(netManager.sBHost, SD_BOTH);
	closesocket(netManager.sBHost);
	shutdown(netManager.sListen, SD_BOTH);
	closesocket(netManager.sListen);
	shutdown(netManager.sRemote, SD_BOTH);
	closesocket(netManager.sRemote);
	device->drop();
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
void Game::SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, wchar_t* text) {
	int pbuffer = 0;
	int _width = 0, w = 0;
	for(int i = 0; text[i] != 0 && i < 1023; ++i) {
		w = font->getCharDimension(text[i]).Width;
		if(text[i] == L'\n')
			_width = 0;
		else if(_width > 0 && _width + w > cWidth) {
			dataManager.strBuffer[pbuffer++] = L'\n';
			_width = 0;
		}
		_width += w;
		dataManager.strBuffer[pbuffer++] = text[i];
	}
	dataManager.strBuffer[pbuffer] = 0;
	pControl->setText(dataManager.strBuffer);
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
		DataManager::DecodeUTF8(dirp->d_name, wname);
		cbDeck->addItem(wname);
	}
#endif
}
void Game::RefreshReplay() {
	lstReplayList->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./replay/*.yrp", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && lastReplay.CheckReplay(fdataw.cFileName)) {
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
		DataManager::DecodeUTF8(dirp->d_name, wname);
		if(lastReplay.CheckReplay(dirp->d_name));
		lstReplayList->addItem(wname);
	}
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
	int value;
	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	while(ftell(fp) < fsize) {
		fgets(linebuf, 256, fp);
		sscanf(linebuf, "%s = %s", strbuf, valbuf);
		if(!strcmp(strbuf, "nickname")) {
			DataManager::DecodeUTF8(valbuf, wstr);
			ebUsername->setText(wstr);
		} else if(!strcmp(strbuf, "gamename")) {
			DataManager::DecodeUTF8(valbuf, wstr);
			ebServerName->setText(wstr);
		} else if(!strcmp(strbuf, "textfont")) {
			guiFont = irr::gui::CGUITTFont::createTTFont(env, valbuf, 14);
			textFont = guiFont;
		} else if(!strcmp(strbuf, "numfont")) {
			numFont = irr::gui::CGUITTFont::createTTFont(env, valbuf, 16);
			adFont = irr::gui::CGUITTFont::createTTFont(env, valbuf, 12);
			lpcFont = irr::gui::CGUITTFont::createTTFont(env, valbuf, 48);
		} else if(!strcmp(strbuf, "servport")) {
			netManager.serv_port = atoi(valbuf);
		} else if(!strcmp(strbuf, "defaultip")) {
			DataManager::DecodeUTF8(valbuf, wstr);
			ebJoinIP->setText(wstr);
		} else if(!strcmp(strbuf, "defaultport")) {
			DataManager::DecodeUTF8(valbuf, wstr);
			ebJoinPort->setText(wstr);
		}
	}
	fclose(fp);
}
void Game::SaveConfig() {

}
void Game::ShowCardInfo(int code) {
	CardData cd;
	wchar_t formatBuffer[256];
	dataManager.GetData(code, &cd);
	imgCard->setImage(imageManager.GetTexture(code));
	myswprintf(formatBuffer, L"%ls[%d]", dataManager.GetName(code), code);
	stName->setText(formatBuffer);
	if(cd.type & TYPE_MONSTER) {
		myswprintf(formatBuffer, L"[%ls] %ls/%ls", DataManager::FormatType(cd.type), DataManager::FormatRace(cd.race), DataManager::FormatAttribute(cd.attribute));
		stInfo->setText(formatBuffer);
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
		stDataInfo->setText(formatBuffer);
		stText->setRelativePosition(irr::core::position2di(15, 83));
	} else {
		myswprintf(formatBuffer, L"[%ls]", DataManager::FormatType(cd.type));
		stInfo->setText(formatBuffer);
		stDataInfo->setText(L"");
		stText->setRelativePosition(irr::core::position2di(15, 60));
	}
	SetStaticText(stText, 270, textFont, (wchar_t*)dataManager.GetText(code));
}

}
