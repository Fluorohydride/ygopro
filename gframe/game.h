#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "data_manager.h"
#include "image_manager.h"
#include "materials.h"
#include "client_card.h"
#include "client_field.h"
#include "deck_con.h"
#include "menu_handler.h"
#include "network.h"
#include "deck_manager.h"
#include "replay.h"
#include <string>
#include "../ocgcore/mtrandom.h"
#include <unordered_map>

namespace ygo {

struct Config {
	unsigned short antialias;
	unsigned short serverport;
	wchar_t lastip[20];
	wchar_t lastport[10];
	wchar_t nickname[20];
	wchar_t gamename[20];
	wchar_t lastdeck[64];
	wchar_t textfont[256];
	wchar_t numfont[256];
};

struct DuelInfo {
	bool isStarted;
	bool is_shuffling;
	bool netError;
	bool is_host_player[2];
	bool is_first_turn;
	bool is_local_host;
	bool is_responsed;
	long pDuel;
	int resPlayer;
	int responseI;
	char responseB[64];
	int lp[2];
	int engFlag;
	int engLen;
	int engType;
	int msgLen;
	int msgType;
	int turn;
	short curMsg;
	short preMsg;
	int selectHint;
	wchar_t hostname[20];
	wchar_t clientname[20];
	wchar_t pass[20];
	wchar_t strLP[2][16];
	wchar_t strTurn[8];
	wchar_t strEvent[64];
};

class Game {

public:
	Game();
	~Game();

	bool Initialize();
	void MainLoop();
	void BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
	void SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, wchar_t* text);
	void RefreshDeck(irr::gui::IGUIComboBox* cbDeck);
	void RefreshReplay();
	void DrawSelectionLine(irr::video::S3DVertex* vec, bool strip, int width, float* cv);
	void DrawBackGround();
	void DrawCards();
	void DrawCard(ClientCard* pcard);
	void DrawMisc();
	void DrawGUI();
	void DrawSpec();
	void ShowElement(irr::gui::IGUIElement* element, int autoframe = 0);
	void HideElement(irr::gui::IGUIElement* element, bool set_action = false, irr::gui::IGUIElement* next = 0);
	void PopupElement(irr::gui::IGUIElement* element, int hideframe = 0);
	void WaitFrameSignal(int frame);
	void DrawThumb(int code, position2di pos, std::unordered_map<int, int>* lflist);
	void DrawDeckBd();
	void LoadConfig();
	void SaveConfig();
	void ShowCardInfo(int code);
	
	int LocalPlayer(int player);
	const wchar_t* LocalName(int local_player);
	bool SendByte(int player, char val);
	bool SendGameMessage(int player, char* buf, int len);
	bool WaitforResponse(int player);
	void SetResponseI();
	void SetResponseB(int len);
	bool RefreshMzone(int player, int flag = 0x181fff, int use_cache = 1);
	bool RefreshSzone(int player, int flag = 0x181fff, int use_cache = 1);
	bool RefreshHand(int player, int flag = 0x181fff, int use_cache = 1);
	bool RefreshGrave(int player, int flag = 0x181fff, int use_cache = 1);
	bool RefreshExtra(int player, int flag = 0x181fff, int use_cache = 1);
	bool RefreshSingle(int player, int location, int sequence, int flag = 0x181fff);
	void ReplayRefresh(int flag = 0x181fff);
	void ReplayRefreshHand(int player, int flag = 0x181fff);
	void ReplayRefreshGrave(int player, int flag = 0x181fff);
	void ReplayRefreshSingle(int player, int location, int sequence, int flag = 0x181fff);

	static int CardReader(int, void*);
	static int MessageHandler(long, int);
	static int EngineThread(void*);
	static void Proceed(void*);
	static void Analyze(void*, char*);
	static int RecvThread(void*);
	static int GameThread(void*);
	static bool SolveMessage(void*, char*, int);
	static int ReplayThread(void* pd);
	static bool AnalyzeReplay(void*, char*);

	//
	char msgBuffer[0x1000];
	char queryBuffer[0x1000];

	bool is_closing;
	bool is_refreshing;
	mtrandom rnd;
	Timer gTimer;
	Mutex gMutex;
	Mutex gBuffer;
	Signal frameSignal;
	Signal localMessage;
	Signal localResponse;
	Signal localAction;
	Config gameConf;
	NetManager netManager;
	DataManager dataManager;
	ImageManager imageManager;
	DeckManager deckManager;
	Materials matManager;
	Replay lastReplay;

	std::vector<int> logParam;
	unsigned short linePattern;
	int waitFrame;
	int signalFrame;
	bool isFadein;
	bool signalAction;
	irr::gui::IGUIElement* guiFading;
	irr::gui::IGUIElement* guiNext;
	int fadingFrame;
	irr::core::recti fadingSize;
	irr::core::vector2di fadingUL;
	irr::core::vector2di fadingLR;
	irr::core::vector2di fadingDiff;
	int autoFadeoutFrame;
	int showcard;
	int showcardcode;
	int showcarddif;
	int showcardp;
	int is_attacking;
	int attack_sv;
	irr::core::vector3df atk_r;
	irr::core::vector3df atk_t;
	float atkdy;
	int lpframe;
	int lpd;
	int lpplayer;
	int lpccolor;
	wchar_t* lpcstring;
	bool always_chain;
	bool ignore_chain;

	bool is_building;

	DuelInfo dInfo;
	ClientField dField;
	DeckBuilder deckBuilder;
	MenuHandler menuHandler;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::scene::ICameraSceneNode* camera;
	//GUI
	irr::gui::IGUIEnvironment* env;
	irr::gui::CGUITTFont* guiFont;
	irr::gui::CGUITTFont* textFont;
	irr::gui::CGUITTFont* numFont;
	irr::gui::CGUITTFont* adFont;
	irr::gui::CGUITTFont* lpcFont;
	std::map<irr::gui::CGUIImageButton*, int> imageLoading;
	//card image
	irr::gui::IGUIStaticText* wCardImg;
	irr::gui::IGUIImage* imgCard;
	//hint text
	irr::gui::IGUIStaticText* stHintMsg;
	irr::gui::IGUIStaticText* stTip;
	//infos
	irr::gui::IGUITabControl* wInfos;
	irr::gui::IGUIStaticText* stName;
	irr::gui::IGUIStaticText* stInfo;
	irr::gui::IGUIStaticText* stDataInfo;
	irr::gui::IGUIStaticText* stText;
	irr::gui::IGUICheckBox* chkAutoPos;
	irr::gui::IGUICheckBox* chkRandomPos;
	irr::gui::IGUICheckBox* chkAutoChain;
	irr::gui::IGUICheckBox* chkWaitChain;
	irr::gui::IGUIListBox* lstLog;
	irr::gui::IGUIButton* btnClearLog;
	irr::gui::IGUIButton* btnSaveLog;
	//main menu
	irr::gui::IGUIWindow* wMainMenu;
	irr::gui::IGUIButton* btnLanMode;
	irr::gui::IGUIButton* btnServerMode;
	irr::gui::IGUIButton* btnReplayMode;
	irr::gui::IGUIButton* btnTestMode;
	irr::gui::IGUIButton* btnDeckEdit;
	irr::gui::IGUIButton* btnModeExit;
	irr::gui::IGUIWindow* wLanWindow;
	irr::gui::IGUIEditBox* ebNickName;
	irr::gui::IGUIListBox* lstHostList;
	irr::gui::IGUIEditBox* ebJoinIP;
	irr::gui::IGUIEditBox* ebJoinPort;
	irr::gui::IGUIEditBox* ebJoinPass;
	irr::gui::IGUIButton* btnJoinHost;
	irr::gui::IGUIButton* btnJoinCancel;
	irr::gui::IGUIButton* btnCreateHost;
	irr::gui::IGUIWindow* wHostWindow;
	irr::gui::IGUIComboBox* cbLFlist;
	irr::gui::IGUIComboBox* cbMatchMode;
	irr::gui::IGUIComboBox* cbRule;
	irr::gui::IGUIEditBox* ebStartLP;
	irr::gui::IGUIEditBox* ebStartHand;
	irr::gui::IGUIEditBox* ebDrawCount;
	irr::gui::IGUIEditBox* ebServerName;
	irr::gui::IGUIEditBox* ebServerPass;
	irr::gui::IGUICheckBox* chkNoCheckDeck;
	irr::gui::IGUICheckBox* chkNoShuffleDeck;
	irr::gui::IGUIButton* btnHostConfirm;
	irr::gui::IGUIButton* btnHostCancel;
	irr::gui::IGUIListBox* lstReplayList;
	irr::gui::IGUIButton* btnLoadReplay;
	irr::gui::IGUIEditBox* ebUsername;
	irr::gui::IGUIComboBox* cbDeckSel;
	irr::gui::IGUIStaticText* stModeStatus;
	irr::gui::IGUIComboBox* cbIPList;
	//message
	irr::gui::IGUIWindow* wMessage;
	irr::gui::IGUIStaticText* stMessage;
	irr::gui::IGUIButton* btnMsgOK;
	//auto close message
	irr::gui::IGUIWindow* wACMessage;
	irr::gui::IGUIStaticText* stACMessage;
	//yes/no
	irr::gui::IGUIWindow* wQuery;
	irr::gui::IGUIStaticText* stQMessage;
	irr::gui::IGUIButton* btnYes;
	irr::gui::IGUIButton* btnNo;
	//options
	irr::gui::IGUIWindow* wOptions;
	irr::gui::IGUIStaticText* stOptions;
	irr::gui::IGUIButton* btnOptionp;
	irr::gui::IGUIButton* btnOptionn;
	irr::gui::IGUIButton* btnOptionOK;
	//pos selection
	irr::gui::IGUIWindow* wPosSelect;
	irr::gui::CGUIImageButton* btnPSAU;
	irr::gui::CGUIImageButton* btnPSAD;
	irr::gui::CGUIImageButton* btnPSDU;
	irr::gui::CGUIImageButton* btnPSDD;
	irr::gui::IGUIWindow* wSelectYesNo;
	irr::gui::IGUIWindow* wSelectOption;
	//card selection
	irr::gui::IGUIWindow* wCardSelect;
	irr::gui::CGUIImageButton* btnCardSelect[5];
	irr::gui::IGUIStaticText *stCardPos[5];
	irr::gui::IGUIScrollBar *scrCardList;
	irr::gui::IGUIButton* btnSelectOK;
	//announce number
	irr::gui::IGUIWindow* wANNumber;
	irr::gui::IGUIComboBox* cbANNumber;
	irr::gui::IGUIButton* btnANNumberOK;
	//announce card
	irr::gui::IGUIWindow* wANCard;
	irr::gui::IGUIEditBox* ebANCard;
	irr::gui::IGUIListBox* lstANCard;
	irr::gui::IGUIButton* btnANCardOK;
	//announce attribute
	irr::gui::IGUIWindow* wANAttribute;
	irr::gui::IGUICheckBox* chkAttribute[7];
	//announce race
	irr::gui::IGUIWindow* wANRace;
	irr::gui::IGUICheckBox* chkRace[22];
	//cmd menu
	irr::gui::IGUIWindow* wCmdMenu;
	irr::gui::IGUIButton* btnActivate;
	irr::gui::IGUIButton* btnSummon;
	irr::gui::IGUIButton* btnSPSummon;
	irr::gui::IGUIButton* btnMSet;
	irr::gui::IGUIButton* btnSSet;
	irr::gui::IGUIButton* btnRepos;
	irr::gui::IGUIButton* btnAttack;
	irr::gui::IGUIButton* btnShowList;
	//phase button
	irr::gui::IGUIButton* btnDP;
	irr::gui::IGUIButton* btnSP;
	irr::gui::IGUIButton* btnM1;
	irr::gui::IGUIButton* btnBP;
	irr::gui::IGUIButton* btnM2;
	irr::gui::IGUIButton* btnEP;
	//deck edit
	irr::gui::IGUIStaticText* wDeckEdit;
	irr::gui::IGUIComboBox* cbDBLFList;
	irr::gui::IGUIComboBox* cbDBDecks;
	irr::gui::IGUIButton* btnClearDeck;
	irr::gui::IGUIButton* btnSortDeck;
	irr::gui::IGUIButton* btnSaveDeck;
	irr::gui::IGUIButton* btnSaveDeckAs;
	irr::gui::IGUIButton* btnDBExit;
	irr::gui::IGUIEditBox* ebDeckname;
	//filter
	irr::gui::IGUIStaticText* wFilter;
	irr::gui::IGUIScrollBar* scrFilter;
	irr::gui::IGUIComboBox* cbCardType;
	irr::gui::IGUIComboBox* cbCardType2;
	irr::gui::IGUIComboBox* cbRace;
	irr::gui::IGUIComboBox* cbAttribute;
	irr::gui::IGUIComboBox* cbLimit;
	irr::gui::IGUIEditBox* ebStar;
	irr::gui::IGUIEditBox* ebAttack;
	irr::gui::IGUIEditBox* ebDefence;
	irr::gui::IGUIEditBox* ebCardName;
	irr::gui::IGUIButton* btnEffectFilter;
	irr::gui::IGUIButton* btnStartFilter;
	irr::gui::IGUIButton* btnResultFilter;
	irr::gui::IGUIWindow* wCategories;
	irr::gui::IGUICheckBox* chkCategory[32];
	irr::gui::IGUIButton* btnCategoryOK;
	//replay save
	irr::gui::IGUIWindow* wReplaySave;
	irr::gui::IGUIEditBox* ebRSName;
	irr::gui::IGUIButton* btnRSYes;
	irr::gui::IGUIButton* btnRSNo;
	//replay control
	irr::gui::IGUIStaticText* wReplay;
	irr::gui::IGUIButton* btnReplayStart;
	irr::gui::IGUIButton* btnReplayPause;
	irr::gui::IGUIButton* btnReplayStep;
	irr::gui::IGUIButton* btnReplayExit;
	irr::gui::IGUIButton* btnReplaySwap;

};

extern Game* mainGame;

}

#define COMMAND_ACTIVATE	0x0001
#define COMMAND_SUMMON		0x0002
#define COMMAND_SPSUMMON	0x0004
#define COMMAND_MSET		0x0008
#define COMMAND_SSET		0x0010
#define COMMAND_REPOS		0x0020
#define COMMAND_ATTACK		0x0040
#define COMMAND_LIST		0x0080

#define BUTTON_LAN_MODE				100
#define BUTTON_SERVER_MODE			101
#define BUTTON_REPLAY_MODE			102
#define BUTTON_TEST_MODE			103
#define BUTTON_DECK_EDIT			104
#define BUTTON_MODE_EXIT			105
#define LISTBOX_LAN_HOST			110
#define BUTTON_JOIN_HOST			111
#define BUTTON_JOIN_CANCEL			112
#define BUTTON_CREATE_HOST			113
#define BUTTON_HOST_CONFIRM			114
#define BUTTON_HOST_CANCEL			115
#define BUTTON_MSG_OK				150
#define BUTTON_YES					151
#define BUTTON_NO					152
#define BUTTON_POS_AU				160
#define BUTTON_POS_AD				161
#define BUTTON_POS_DU				162
#define BUTTON_POS_DD				163
#define BUTTON_OPTION_PREV			170
#define BUTTON_OPTION_NEXT			171
#define BUTTON_OPTION_OK			172
#define BUTTON_CARD_0				180
#define BUTTON_CARD_1				181
#define BUTTON_CARD_2				182
#define BUTTON_CARD_3				183
#define BUTTON_CARD_4				184
#define SCROLL_CARD_SELECT			185
#define BUTTON_CARD_SEL_OK			186
#define BUTTON_CMD_ACTIVATE			190
#define BUTTON_CMD_SUMMON			191
#define BUTTON_CMD_SPSUMMON			192
#define BUTTON_CMD_MSET				193
#define BUTTON_CMD_SSET				194
#define BUTTON_CMD_REPOS			195
#define BUTTON_CMD_ATTACK			196
#define BUTTON_CMD_SHOWLIST			197
#define BUTTON_ANNUMBER_OK			200
#define BUTTON_ANCARD_OK			201
#define EDITBOX_ANCARD				202
#define LISTBOX_ANCARD				203
#define CHECK_ATTRIBUTE				204
#define CHECK_RACE					205
#define BUTTON_BP					210
#define BUTTON_M2					211
#define BUTTON_EP					212
#define BUTTON_CLEAR_LOG			220
#define LISTBOX_LOG					221
#define BUTTON_CATEGORY_OK			250
#define COMBOBOX_DBLFLIST			251
#define COMBOBOX_DBDECKS			252
#define BUTTON_CLEAR_DECK			253
#define BUTTON_SAVE_DECK			254
#define BUTTON_SAVE_DECK_AS			255
#define BUTTON_DBEXIT				256
#define BUTTON_SORT_DECK			257
#define COMBOBOX_MAINTYPE			260
#define BUTTON_EFFECT_FILTER		261
#define BUTTON_START_FILTER			262
#define BUTTON_RESULT_FILTER		263
#define SCROLL_FILTER				264
#define SCROLL_KEYWORD				265
#define BUTTON_REPLAY_START			270
#define BUTTON_REPLAY_PAUSE			271
#define BUTTON_REPLAY_STEP			272
#define BUTTON_REPLAY_EXIT			273
#define BUTTON_REPLAY_SWAP			274
#define BUTTON_REPLAY_SAVE			280
#define BUTTON_REPLAY_CANCEL		281
#endif // GAME_H
