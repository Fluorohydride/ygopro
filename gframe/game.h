#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "data_manager.h"
#include "image_manager.h"
#include "materials.h"
#include "client_card.h"
#include "client_field.h"
#include "deck_con.h"
#include "network.h"
#include "deck_manager.h"
#include "replay.h"
#include <string>
#include "../ocgcore/mtrandom.h"
#include <unordered_map>

namespace ygo {

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
	void HideElement(irr::gui::IGUIElement* element, bool set_action = false);
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
	irr::gui::IGUIWindow* wModeSelection;
	irr::gui::IGUITabControl* wModes;
	irr::gui::IGUIButton* btnLanStartServer;
	irr::gui::IGUIButton* btnLanCancelServer;
	irr::gui::IGUIEditBox* ebStartLP;
	irr::gui::IGUIEditBox* ebStartHand;
	irr::gui::IGUIEditBox* ebDrawCount;
	irr::gui::IGUIEditBox* ebServerName;
	irr::gui::IGUIEditBox* ebServerPass;
	irr::gui::IGUIComboBox* cbLFlist;
	irr::gui::IGUIComboBox* cbMatchMode;
	irr::gui::IGUIComboBox* cbTurnTime;
	irr::gui::IGUICheckBox* chkNoCheckDeck;
	irr::gui::IGUICheckBox* chkNoShuffleDeck;
	irr::gui::IGUICheckBox* chkNoShufflePlayer;
	irr::gui::IGUICheckBox* chkAttackFT;
	irr::gui::IGUICheckBox* chkNoChainHint;
	irr::gui::IGUIListBox* lstServerList;
	irr::gui::IGUIButton* btnRefreshList;
	irr::gui::IGUIEditBox* ebJoinIP;
	irr::gui::IGUIEditBox* ebJoinPort;
	irr::gui::IGUIEditBox* ebJoinPass;
	irr::gui::IGUICheckBox* chkStOnly;
	irr::gui::IGUIButton* btnLanConnect;
	irr::gui::IGUIListBox* lstReplayList;
	irr::gui::IGUIButton* btnLoadReplay;
	irr::gui::IGUIEditBox* ebUsername;
	irr::gui::IGUIComboBox* cbDeckSel;
	irr::gui::IGUIButton* btnDeckEdit;
	irr::gui::IGUIStaticText* stModeStatus;
	irr::gui::IGUIButton* btnModeExit;
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

#define TAB_MODES					50
#define BUTTON_SAVE_LOG				100
#define BUTTON_CLEAR_LOG			101
#define LISTBOX_LOG					102
#define BUTTON_LAN_START_SERVER		110
#define BUTTON_LAN_CANCEL_SERVER	111
#define BUTTON_LAN_CONNECT			112
#define BUTTON_LAN_REFRESH			113
#define BUTTON_LOAD_REPLAY			114
#define BUTTON_DECK_EDIT			115
#define BUTTON_MODE_EXIT			116
#define LISTBOX_SERVER_LIST			117
#define LISTBOX_REPLAY_LIST			118
#define BUTTON_MSG_OK				120
#define BUTTON_YES					121
#define BUTTON_NO					122
#define BUTTON_POS_AU				130
#define BUTTON_POS_AD				131
#define BUTTON_POS_DU				132
#define BUTTON_POS_DD				133
#define BUTTON_OPTION_PREV			140
#define BUTTON_OPTION_NEXT			141
#define BUTTON_OPTION_OK			142
#define BUTTON_CARD_0				150
#define BUTTON_CARD_1				151
#define BUTTON_CARD_2				152
#define BUTTON_CARD_3				153
#define BUTTON_CARD_4				154
#define SCROLL_CARD_SELECT			155
#define BUTTON_CARD_SEL_OK			156
#define BUTTON_CMD_ACTIVATE			160
#define BUTTON_CMD_SUMMON			161
#define BUTTON_CMD_SPSUMMON			162
#define BUTTON_CMD_MSET				163
#define BUTTON_CMD_SSET				164
#define BUTTON_CMD_REPOS			165
#define BUTTON_CMD_ATTACK			166
#define BUTTON_CMD_SHOWLIST			167
#define BUTTON_ANNUMBER_OK			170
#define BUTTON_ANCARD_OK			171
#define EDITBOX_ANCARD				172
#define LISTBOX_ANCARD				173
#define CHECK_ATTRIBUTE				174
#define CHECK_RACE					175
#define BUTTON_BP					180
#define BUTTON_M2					181
#define BUTTON_EP					182
#define BUTTON_CATEGORY_OK			200
#define COMBOBOX_DBLFLIST			201
#define COMBOBOX_DBDECKS			202
#define BUTTON_CLEAR_DECK			203
#define BUTTON_SAVE_DECK			204
#define BUTTON_SAVE_DECK_AS			205
#define BUTTON_DBEXIT				206
#define BUTTON_SORT_DECK			207
#define COMBOBOX_MAINTYPE			210
#define BUTTON_EFFECT_FILTER		211
#define BUTTON_START_FILTER			212
#define BUTTON_RESULT_FILTER		213
#define SCROLL_FILTER				214
#define SCROLL_KEYWORD				215
#define BUTTON_REPLAY_START			220
#define BUTTON_REPLAY_PAUSE			221
#define BUTTON_REPLAY_STEP			222
#define BUTTON_REPLAY_EXIT			223
#define BUTTON_REPLAY_SWAP			224
#define BUTTON_REPLAY_SAVE			230
#define BUTTON_REPLAY_CANCEL		231
#endif // GAME_H
