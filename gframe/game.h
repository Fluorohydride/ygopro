#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "client_field.h"
#include "deck_con.h"
#include "menu_handler.h"
#include <unordered_map>
#include <vector>
#include <list>
#include "CGUISkinSystem/CGUISkinSystem.h"
#include "CGUICustomText/CGUICustomText.h"
#include "CGUIFileSelectListBox/CGUIFileSelectListBox.h"
#include "CGUICustomContextMenu/CGUICustomContextMenu.h"
#include "CGUICustomContextMenu/CGUICustomMenu.h"
#include "CProgressBar/CProgressBar.h"
#include "deck_manager.h"
#include "repo_manager.h"

namespace ygo {

struct Config {
	bool use_d3d;
	bool use_vsync;
	int max_fps;
	int game_version;
	bool fullscreen;
	unsigned short antialias;
	std::wstring serverport;
	unsigned char textfontsize;
	std::wstring lasthost;
	std::wstring lastport;
	std::wstring nickname;
	std::wstring gamename;
	std::wstring lastdeck;
	std::wstring textfont;
	std::wstring numfont;
	std::wstring roompass;
	//settings
	int chkMAutoPos;
	int chkSTAutoPos;
	int chkRandomPos;
	int chkAutoChain;
	int chkWaitChain;
	int chkIgnore1;
	int chkIgnore2;
	int chkHideSetname;
	int chkHideHintButton;
	int draw_field_spell;
	int quick_animation;

	int chkAnime;
	bool enablesound;
	double volume;
	bool enablemusic;
	int skin_index;
};

struct DuelInfo {
	bool isStarted;
	bool isReplay;
	bool isOldReplay;
	bool isCatchingUp;
	bool isFirst;
	bool isRelay;
	bool isSingleMode;
	bool lua64;
	bool is_shuffling;
	int current_player[2];
	int lp[2];
	int startlp;
	int duel_field;
	int extraval;
	int turn;
	short curMsg;
	int team1;
	int team2;
	std::vector<std::wstring> clientname;
	std::vector<std::wstring> hostname;
	std::wstring strLP[2];
	std::wstring vic_string;
	unsigned char player_type;
	unsigned char time_player;
	unsigned short time_limit;
	unsigned short time_left[2];
	bool isReplaySwapped;
};

struct FadingUnit {
	bool signalAction;
	bool isFadein;
	float fadingFrame;
	int autoFadeoutFrame;
	irr::gui::IGUIElement* guiFading;
	irr::core::recti fadingSize;
	irr::core::vector2di fadingUL;
	irr::core::vector2di fadingLR;
	irr::core::vector2di fadingDest;
};

struct Fieldmatrix {
	float x, y, z, atan;
};

class Game {

public:
	bool Initialize();
	void MainLoop();
	void ToggleFullscreen();
	void BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
	void LoadExpansionDB();
	void RefreshDeck(irr::gui::IGUIComboBox* cbDeck);
	void RefreshReplay();
	void RefreshSingleplay();
	void DrawSelectionLine(irr::video::S3DVertex* vec, bool strip, int width, float* cv);
	void DrawSelectionLine(irr::gui::IGUIElement* element, int width, irr::video::SColor color);
	void DrawBackGround();
	void DrawLinkedZones(ClientCard* pcard);
	bool CheckMutual(ClientCard* pcard, int mark);
	void DrawCards();
	void DrawCard(ClientCard* pcard);
	void DrawMisc();
	void DrawStatus(ClientCard* pcard);
	void DrawPendScale(ClientCard* pcard);
	void DrawStackIndicator(const std::wstring& text, S3DVertex* v, bool opponent);
	void ConvertCoords(float x, float y, int* x1, int* y1);
	void DrawGUI();
	void DrawSpec();
	void DrawBackImage(irr::video::ITexture* texture);
	void ShowElement(irr::gui::IGUIElement* element, int autoframe = 0);
	void HideElement(irr::gui::IGUIElement* element, bool set_action = false);
	void PopupElement(irr::gui::IGUIElement* element, int hideframe = 0);
	void WaitFrameSignal(int frame);
	void DrawThumb(CardDataC* cp, position2di pos, LFList* lflist, bool drag = false, recti* cliprect = nullptr, bool loadimage = true);
	void DrawDeckBd();
	void LoadConfig();
	void SaveConfig();
	void LoadPicUrls();
	void AddGithubRepositoryStatusWindow(const RepoManager::GitRepo& repo);
	void LoadGithubRepositories();
	void ShowCardInfo(int code, bool resize = false);
	void ClearCardInfo(int player = 0);
	void AddChatMsg(const std::wstring& msg, int player);
	void AddLog(const std::wstring& msg, int param = 0);
	void ClearChatMsg();
	void AddDebugMsg(const std::string& msg);
	void ErrorLog(const std::string& msg);
	void ClearTextures();
	void CloseDuelWindow();
	void PopupMessage(const std::wstring& text);

	int LocalPlayer(int player);
	std::wstring LocalName(int local_player);
	void UpdateDuelParam();
	void UpdateExtraRules();
	int GetMasterRule(uint32 param, uint32 forbidden, int* truerule = 0);
	void SetPhaseButtons();
	void SetMesageWindow();

	bool HasFocus(EGUI_ELEMENT_TYPE type) const {
		irr::gui::IGUIElement* focus = env->getFocus();
		return focus && focus->hasType(type);
	}

	void OnResize();
	recti Resize(s32 x, s32 y, s32 x2, s32 y2);
	recti Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2);
	vector2d<s32> Resize(s32 x, s32 y, bool reverse = false);
	recti ResizeElem(s32 x, s32 y, s32 x2, s32 y2);
	recti ResizeWinFromCenter(s32 x, s32 y, s32 x2, s32 y2);
	recti ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat = false);
	void SetCentered(irr::gui::IGUIElement* elem);
	void ValidateName(irr::gui::IGUIElement* box);
	template<typename T>
	static std::vector<T> TokenizeString(T input, const T& token);
	static std::wstring StringtoUpper(std::wstring input);
	static bool CompareStrings(std::wstring input, const std::vector<std::wstring>& tokens, bool transform_input = false, bool transform_token = false);
	static bool CompareStrings(std::wstring input, std::wstring second_term, bool transform_input = false, bool transform_term = false);
	std::wstring ReadPuzzleMessage(const std::wstring& script_name);
	void* SetupDuel(uint32 seed);
	std::vector<unsigned char> LoadScript(const std::string& script_name);
	std::vector<unsigned char> PreLoadScript(void* pduel, const std::string& script_name);
	static byte* ScriptReader(const char* script_name, int* slen);
	static int MessageHandler(void* fduel, int type);

	std::mutex gMutex;
	std::mutex analyzeMutex;
	Signal frameSignal;
	Signal actionSignal;
	Signal replaySignal;
	std::mutex closeSignal;
	Signal closeDoneSignal;
	Config gameConf;
	DuelInfo dInfo;
#ifdef YGOPRO_BUILD_DLL
	void* ocgcore;
#endif
	bool coreloaded;
	bool is_fullscreen;
	core::dimension2d<u32> unresized_screen_size;
	std::list<FadingUnit> fadingList;
	std::vector<int> logParam;
	std::wstring chatMsg[8];
	struct RepoGui {
		std::string path;
		IProgressBar* progress1;
		IProgressBar* progress2;
		irr::gui::IGUIButton* history_button1;
		irr::gui::IGUIButton* history_button2;
		std::wstring commit_history_full;
		std::wstring commit_history_partial;
	};
	std::map<std::string, RepoGui> repoInfoGui;

	uint32 delta_time;
	uint32 delta_frames;
	int hideChatTimer;
	bool hideChat;
	float chatTiming[8];
	int chatType[8];
	unsigned short linePatternD3D;
	unsigned short linePatternGL;
	float waitFrame;
	int signalFrame;
	int saveReplay;
	int showcard;
	int showcardcode;
	float showcarddif;
	float showcardp;
	bool is_attacking;
	float attack_sv;
	irr::core::vector3df atk_r;
	irr::core::vector3df atk_t;
	float atkdy;
	int lpframe;
	float lpd;
	int lpplayer;
	int lpccolor;
	int lpcalpha;
	std::wstring lpcstring;
	bool always_chain;
	bool ignore_chain;
	bool chain_when_avail;

	bool is_building;
	bool is_siding;
	int forbiddentypes;
	unsigned short extra_rules;
	uint32 duel_param;
	uint32 showingcard;
	bool cardimagetextureloading;

	irr::core::dimension2d<irr::u32> window_size;

	CGUISkinSystem *skinSystem;

	ClientField dField;
	DeckBuilder deckBuilder;
	MenuHandler menuHandler;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::scene::ICameraSceneNode* camera;
	io::IFileSystem* filesystem;
	void PopulateResourcesDirectories();
	std::vector<path_string> field_dirs;
	std::vector<path_string> pic_dirs;
	std::vector<path_string> script_dirs;
	std::vector<std::wstring> cores_to_load;
	std::mutex popupCheck;
	std::wstring queued_msg;
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
	irr::gui::IGUIStaticText* stCardListTip;
	//infos
	irr::gui::IGUITabControl* wInfos;
	irr::gui::IGUIStaticText* stName;
	irr::gui::IGUIStaticText* stInfo;
	irr::gui::IGUIStaticText* stDataInfo;
	irr::gui::IGUIStaticText* stSetName;
	irr::gui::IGUIStaticText* stText;
	irr::gui::IGUIStaticText* stVolume;
	irr::gui::IGUIListBox* lstLog;
	irr::gui::IGUIListBox* lstChat;
	irr::gui::IGUIButton* btnClearLog;
	irr::gui::IGUIButton* btnClearChat;
	irr::gui::IGUIButton* btnSaveLog;
	irr::gui::IGUITab* tabRepositories;
	irr::gui::IGUIContextMenu* mTabRepositories;
	irr::gui::IGUICheckBox* chkMAutoPos;
	irr::gui::IGUICheckBox* chkSTAutoPos;
	irr::gui::IGUICheckBox* chkRandomPos;
	irr::gui::IGUICheckBox* chkAutoChain;
	irr::gui::IGUICheckBox* chkWaitChain;
	irr::gui::IGUICheckBox* chkQuickAnimation;
	irr::gui::IGUICheckBox* chkHideSetname;
	irr::gui::IGUICheckBox* chkHideHintButton;
	irr::gui::IGUICheckBox* chkEnableSound;
	irr::gui::IGUICheckBox* chkEnableMusic;
	irr::gui::IGUIScrollBar* srcVolume;
	//main menu
	irr::gui::IGUIWindow* wMainMenu;
	irr::gui::IGUIWindow* wCommitsLog;
	irr::gui::IGUIContextMenu* mTopMenu;
	irr::gui::IGUIContextMenu* mRepositoriesInfo;
	irr::gui::IGUIButton* btnLanMode;
	irr::gui::IGUIButton* btnSingleMode;
	irr::gui::IGUIButton* btnReplayMode;
	irr::gui::IGUIButton* btnTestMode;
	irr::gui::IGUIButton* btnDeckEdit;
	irr::gui::IGUIButton* btnModeExit;
	irr::gui::IGUIButton* btnCommitLogExit;
	irr::gui::IGUIStaticText* stCommitLog;
	//lan
	irr::gui::IGUIWindow* wLanWindow;
	irr::gui::IGUIEditBox* ebNickName;
	irr::gui::IGUIListBox* lstHostList;
	irr::gui::IGUIButton* btnLanRefresh;
	irr::gui::IGUIEditBox* ebJoinHost;
	irr::gui::IGUIEditBox* ebJoinPort;
	irr::gui::IGUIEditBox* ebJoinPass;
	irr::gui::IGUIButton* btnJoinHost;
	irr::gui::IGUIButton* btnJoinCancel;
	irr::gui::IGUIButton* btnCreateHost;
	//create host
	irr::gui::IGUIWindow* wCreateHost;
	irr::gui::IGUIComboBox* cbLFlist;
	irr::gui::IGUIComboBox* cbMatchMode;
	irr::gui::IGUIComboBox* cbRule;
	irr::gui::IGUIEditBox* ebTimeLimit;
	irr::gui::IGUIEditBox* ebStartLP;
	irr::gui::IGUIEditBox* ebStartHand;
	irr::gui::IGUIEditBox* ebDrawCount;
	irr::gui::IGUIEditBox* ebServerName;
	irr::gui::IGUIEditBox* ebServerPass;
	irr::gui::IGUIButton* btnRuleCards;
	irr::gui::IGUIWindow* wRules;
	irr::gui::IGUICheckBox* chkRules[13];
	irr::gui::IGUIButton* btnRulesOK;
	irr::gui::IGUIComboBox* cbDuelRule;
	irr::gui::IGUIButton* btnCustomRule;
	irr::gui::IGUICheckBox* chkCustomRules[6];
	irr::gui::IGUICheckBox* chkTypeLimit[5];
	irr::gui::IGUIWindow* wCustomRules;
	irr::gui::IGUIButton* btnCustomRulesOK;
	irr::gui::IGUICheckBox* chkNoCheckDeck;
	irr::gui::IGUICheckBox* chkNoShuffleDeck;
	irr::gui::IGUIButton* btnHostConfirm;
	irr::gui::IGUIButton* btnHostCancel;
	irr::gui::IGUIEditBox* ebHostPort;
	//host panel
	irr::gui::IGUIWindow* wHostPrepare;
	irr::gui::IGUIWindow* wHostPrepare2;
	irr::gui::IGUIStaticText* stHostCardRule;
	irr::gui::IGUIButton* btnHostPrepDuelist;
	irr::gui::IGUIButton* btnHostPrepOB;
	irr::gui::IGUIStaticText* stHostPrepDuelist[6];
	irr::gui::IGUICheckBox* chkHostPrepReady[6];
	irr::gui::IGUIButton* btnHostPrepKick[6];
	irr::gui::IGUIComboBox* cbDeckSelect;
	irr::gui::IGUIComboBox* cbDeckSelect2;
	irr::gui::IGUIStaticText* stHostPrepRule;
	irr::gui::IGUIStaticText* stHostPrepRule2;
	irr::gui::IGUIStaticText* stHostPrepOB;
	irr::gui::IGUIStaticText* stDeckSelect;
	irr::gui::IGUIButton* btnHostPrepReady;
	irr::gui::IGUIButton* btnHostPrepNotReady;
	irr::gui::IGUIButton* btnHostPrepStart;
	irr::gui::IGUIButton* btnHostPrepCancel;
	//replay
	irr::gui::IGUIWindow* wReplay;
	irr::gui::CGUIFileSelectListBox* lstReplayList;
	irr::gui::IGUIStaticText* stReplayInfo;
	irr::gui::IGUICheckBox* chkYrp;
	irr::gui::IGUIButton* btnLoadReplay;
	irr::gui::IGUIButton* btnDeleteReplay;
	irr::gui::IGUIButton* btnRenameReplay;
	irr::gui::IGUIButton* btnReplayCancel;
	irr::gui::IGUIButton* btnExportDeck;
	irr::gui::IGUIEditBox* ebRepStartTurn;
	//single play
	irr::gui::IGUIWindow* wSinglePlay;
	irr::gui::CGUIFileSelectListBox* lstSinglePlayList;
	irr::gui::IGUIStaticText* stSinglePlayInfo;
	irr::gui::IGUIButton* btnLoadSinglePlay;
	irr::gui::IGUIButton* btnSinglePlayCancel;
	//hand
	irr::gui::IGUIWindow* wHand;
	irr::gui::IGUIButton* btnHand[3];
	//
	irr::gui::IGUIWindow* wFTSelect;
	irr::gui::IGUIButton* btnFirst;
	irr::gui::IGUIButton* btnSecond;
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
	irr::gui::IGUIButton* btnOption[5];
	irr::gui::IGUIScrollBar *scrOption;
	//pos selection
	irr::gui::IGUIWindow* wPosSelect;
	irr::gui::CGUIImageButton* btnPSAU;
	irr::gui::CGUIImageButton* btnPSAD;
	irr::gui::CGUIImageButton* btnPSDU;
	irr::gui::CGUIImageButton* btnPSDD;
	//card selection
	irr::gui::IGUIWindow* wCardSelect;
	irr::gui::CGUIImageButton* btnCardSelect[5];
	irr::gui::IGUIStaticText *stCardPos[5];
	irr::gui::IGUIScrollBar *scrCardList;
	irr::gui::IGUIButton* btnSelectOK;
	//card display
	irr::gui::IGUIWindow* wCardDisplay;
	irr::gui::CGUIImageButton* btnCardDisplay[5];
	irr::gui::IGUIStaticText *stDisplayPos[5];
	irr::gui::IGUIScrollBar *scrDisplayList;
	irr::gui::IGUIButton* btnDisplayOK;
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
	irr::gui::IGUICheckBox* chkRace[25];
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
	irr::gui::IGUIButton* btnOperation;
	irr::gui::IGUIButton* btnReset;
	irr::gui::IGUIButton* btnShuffle;
	//chat window
	irr::gui::IGUIWindow* wChat;
	irr::gui::IGUIListBox* lstChatLog;
	irr::gui::IGUIEditBox* ebChatInput;
	irr::gui::IGUICheckBox* chkIgnore1;
	irr::gui::IGUICheckBox* chkIgnore2;
	//phase button
	irr::gui::IGUIStaticText* wPhase;
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
	irr::gui::IGUIButton* btnShuffleDeck;
	irr::gui::IGUIButton* btnSaveDeck;
	irr::gui::IGUIButton* btnDeleteDeck;
	irr::gui::IGUIButton* btnSaveDeckAs;
	irr::gui::IGUIButton* btnSideOK;
	irr::gui::IGUIButton* btnSideShuffle;
	irr::gui::IGUIButton* btnSideSort;
	irr::gui::IGUIButton* btnSideReload;
	irr::gui::IGUIEditBox* ebDeckname;
	irr::gui::IGUIStaticText* stBanlist;
	irr::gui::IGUIStaticText* stDeck;
	irr::gui::IGUIStaticText* stCategory;
	irr::gui::IGUIStaticText* stLimit;
	irr::gui::IGUIStaticText* stAttribute;
	irr::gui::IGUIStaticText* stRace;
	irr::gui::IGUIStaticText* stAttack;
	irr::gui::IGUIStaticText* stDefense;
	irr::gui::IGUIStaticText* stStar;
	irr::gui::IGUIStaticText* stSearch;
	irr::gui::IGUIStaticText* stScale;
	//filter
	irr::gui::IGUIStaticText* wFilter;
	irr::gui::IGUIScrollBar* scrFilter;
	irr::gui::IGUIComboBox* cbCardType;
	irr::gui::IGUIComboBox* cbCardType2;
	irr::gui::IGUIComboBox* cbRace;
	irr::gui::IGUIComboBox* cbAttribute;
	irr::gui::IGUIComboBox* cbLimit;
	irr::gui::IGUIEditBox* ebStar;
	irr::gui::IGUIEditBox* ebScale;
	irr::gui::IGUIEditBox* ebAttack;
	irr::gui::IGUIEditBox* ebDefense;
	irr::gui::IGUIEditBox* ebCardName;
	irr::gui::IGUIButton* btnEffectFilter;
	irr::gui::IGUIButton* btnStartFilter;
	irr::gui::IGUIButton* btnClearFilter;
	irr::gui::IGUIWindow* wCategories;
	irr::gui::IGUICheckBox* chkCategory[32];
	irr::gui::IGUIButton* btnCategoryOK;
	irr::gui::IGUIButton* btnMarksFilter;
	irr::gui::IGUIWindow* wLinkMarks;
	irr::gui::IGUIButton* btnMark[8];
	irr::gui::IGUIButton* btnMarksOK;
	irr::gui::IGUICheckBox* chkAnime;
	//sort type
	irr::gui::IGUIStaticText* wSort;
	irr::gui::IGUIComboBox* cbSortType;
	//replay save
	irr::gui::IGUIWindow* wReplaySave;
	irr::gui::IGUIEditBox* ebRSName;
	irr::gui::IGUIButton* btnRSYes;
	irr::gui::IGUIButton* btnRSNo;
	//replay control
	irr::gui::IGUIStaticText* wReplayControl;
	irr::gui::IGUIButton* btnReplayStart;
	irr::gui::IGUIButton* btnReplayPause;
	irr::gui::IGUIButton* btnReplayStep;
	irr::gui::IGUIButton* btnReplayUndo;
	irr::gui::IGUIButton* btnReplayExit;
	irr::gui::IGUIButton* btnReplaySwap;
	//surrender/leave
	irr::gui::IGUIButton* btnLeaveGame;
	//swap
	irr::gui::IGUIButton* btnSpectatorSwap;
	//chain control
	irr::gui::IGUIButton* btnChainIgnore;
	irr::gui::IGUIButton* btnChainAlways;
	irr::gui::IGUIButton* btnChainWhenAvail;

	//cancel or finish
	irr::gui::IGUIButton* btnCancelOrFinish;

	Fieldmatrix board;
};

extern Game* mainGame;

template<typename T>
inline std::vector<T> Game::TokenizeString(T input, const T & token) {
	std::vector<T> res;
	std::size_t pos;
	while((pos = input.find(token)) != T::npos) {
		if(pos != 0)
			res.push_back(input.substr(0, pos));
		input = input.substr(pos + 1);
	}
	if(input.size())
		res.push_back(input);
	return res;
}

}

#define FIELD_X			4.2f	
#define FIELD_Y			8.0f
#define FIELD_Z			7.8f

#define CARD_IMG_WIDTH		177
#define CARD_IMG_HEIGHT		254
#define CARD_THUMB_WIDTH	44
#define CARD_THUMB_HEIGHT	64

#define UEVENT_EXIT			0x1
#define UEVENT_TOWINDOW		0x2

#define COMMAND_ACTIVATE	0x0001
#define COMMAND_SUMMON		0x0002
#define COMMAND_SPSUMMON	0x0004
#define COMMAND_MSET		0x0008
#define COMMAND_SSET		0x0010
#define COMMAND_REPOS		0x0020
#define COMMAND_ATTACK		0x0040
#define COMMAND_LIST		0x0080
#define COMMAND_OPERATION	0x0100
#define COMMAND_RESET		0x0200

#define POSITION_HINT		0x8000

#define BUTTON_LAN_MODE				100
#define BUTTON_SINGLE_MODE			101
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
#define BUTTON_LAN_REFRESH			116
#define BUTTON_RULE_CARDS			117
#define BUTTON_RULE_OK				118
#define BUTTON_CUSTOM_RULE			119
#define BUTTON_HP_DUELIST			120
#define BUTTON_HP_OBSERVER			121
#define BUTTON_HP_START				122
#define BUTTON_HP_CANCEL			123
#define BUTTON_HP_KICK				124
#define CHECKBOX_HP_READY			125
#define BUTTON_HP_READY				126
#define BUTTON_HP_NOTREADY			127
#define COMBOBOX_DUEL_RULE			128
#define BUTTON_CUSTOM_RULE_OK		129
#define LISTBOX_REPLAY_LIST			130
#define BUTTON_LOAD_REPLAY			131
#define BUTTON_CANCEL_REPLAY		132
#define BUTTON_DELETE_REPLAY		133
#define BUTTON_RENAME_REPLAY		134
#define BUTTON_EXPORT_DECK			135
#define EDITBOX_CHAT				140
#define EDITBOX_PORT_BOX			141
#define BUTTON_MSG_OK				200
#define BUTTON_YES					201
#define BUTTON_NO					202
#define BUTTON_HAND1				205
#define BUTTON_HAND2				206
#define BUTTON_HAND3				207
#define BUTTON_FIRST				208
#define BUTTON_SECOND				209
#define BUTTON_POS_AU				210
#define BUTTON_POS_AD				211
#define BUTTON_POS_DU				212
#define BUTTON_POS_DD				213
#define BUTTON_OPTION_PREV			220
#define BUTTON_OPTION_NEXT			221
#define BUTTON_OPTION_OK			222
#define BUTTON_OPTION_0				223
#define BUTTON_OPTION_1				224
#define BUTTON_OPTION_2				225
#define BUTTON_OPTION_3				226
#define BUTTON_OPTION_4				227
#define SCROLL_OPTION_SELECT		228
#define BUTTON_CARD_0				230
#define BUTTON_CARD_1				231
#define BUTTON_CARD_2				232
#define BUTTON_CARD_3				233
#define BUTTON_CARD_4				234
#define SCROLL_CARD_SELECT			235
#define BUTTON_CARD_SEL_OK			236
#define TEXT_CARD_LIST_TIP			237
#define BUTTON_CMD_ACTIVATE			240
#define BUTTON_CMD_SUMMON			241
#define BUTTON_CMD_SPSUMMON			242
#define BUTTON_CMD_MSET				243
#define BUTTON_CMD_SSET				244
#define BUTTON_CMD_REPOS			245
#define BUTTON_CMD_ATTACK			246
#define BUTTON_CMD_SHOWLIST			247
#define BUTTON_CMD_SHUFFLE			248
#define BUTTON_CMD_RESET			249
#define BUTTON_ANNUMBER_OK			250
#define BUTTON_ANCARD_OK			251
#define EDITBOX_ANCARD				252
#define LISTBOX_ANCARD				253
#define CHECK_ATTRIBUTE				254
#define CHECK_RACE					255
#define BUTTON_BP					260
#define BUTTON_M2					261
#define BUTTON_EP					262
#define BUTTON_LEAVE_GAME			263
#define BUTTON_CHAIN_IGNORE			264
#define BUTTON_CHAIN_ALWAYS			265
#define BUTTON_CHAIN_WHENAVAIL		266
#define BUTTON_CANCEL_OR_FINISH		267
#define BUTTON_CLEAR_LOG			270
#define LISTBOX_LOG					271
#define BUTTON_CLEAR_CHAT			272
#define BUTTON_REPO_CHANGELOG		273
#define BUTTON_REPO_CHANGELOG_EXIT	274
#define BUTTON_DISPLAY_0			290
#define BUTTON_DISPLAY_1			291
#define BUTTON_DISPLAY_2			292
#define BUTTON_DISPLAY_3			293
#define BUTTON_DISPLAY_4			294
#define SCROLL_CARD_DISPLAY			295
#define BUTTON_CARD_DISP_OK			296
#define EDITBOX_DECK_NAME			297
#define BUTTON_CATEGORY_OK			300
#define COMBOBOX_DBLFLIST			301
#define COMBOBOX_DBDECKS			302
#define BUTTON_CLEAR_DECK			303
#define BUTTON_SAVE_DECK			304
#define BUTTON_SAVE_DECK_AS			305
#define BUTTON_DELETE_DECK			306
#define BUTTON_SIDE_RELOAD			307
#define BUTTON_SORT_DECK			308
#define BUTTON_SIDE_OK				309
#define BUTTON_SHUFFLE_DECK			310
#define COMBOBOX_MAINTYPE			311
#define COMBOBOX_SECONDTYPE			312
#define BUTTON_EFFECT_FILTER		313
#define BUTTON_START_FILTER			314
#define SCROLL_FILTER				315
#define EDITBOX_KEYWORD				316
#define BUTTON_CLEAR_FILTER			317
#define COMBOBOX_OTHER_FILT			319
#define BUTTON_REPLAY_START			320
#define BUTTON_REPLAY_PAUSE			321
#define BUTTON_REPLAY_STEP			322
#define BUTTON_REPLAY_UNDO			323
#define BUTTON_REPLAY_EXIT			324
#define BUTTON_REPLAY_SWAP			325
#define EDITBOX_REPLAY_NAME			326
#define BUTTON_REPLAY_SAVE			330
#define BUTTON_REPLAY_CANCEL		331
#define LISTBOX_SINGLEPLAY_LIST		350
#define BUTTON_LOAD_SINGLEPLAY		351
#define BUTTON_CANCEL_SINGLEPLAY	352
#define CHECKBOX_EXTRA_RULE			353
#define CHECKBOX_ENABLE_MUSIC		366
#define SCROLL_VOLUME				367
#define CHECKBOX_SHOW_ANIME			368
#define CHECKBOX_QUICK_ANIMATION	369
#define COMBOBOX_SORTTYPE			370

#define BUTTON_MARKS_FILTER			380
#define BUTTON_MARKERS_OK			381

#define CHECKBOX_OBSOLETE			390
#define CHECKBOX_DRAW				391
#define CHECKBOX_FIELD				392
#define CHECKBOX_PZONE				393
#define CHECKBOX_SEPARATE_PZONE		394
#define CHECKBOX_EMZONE				395

#define DEFAULT_DUEL_RULE			4

#define CARD_ARTWORK_VERSIONS_OFFSET	10

#define DECK_SEARCH_SCROLL_STEP		100
#endif // GAME_H
