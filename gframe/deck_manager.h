#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include <unordered_map>
#include <vector>
#include <sstream>
#include "data_manager.h"

#ifndef YGOPRO_MAX_DECK
#define YGOPRO_MAX_DECK					60
#endif

#ifndef YGOPRO_MIN_DECK
#define YGOPRO_MIN_DECK					40
#endif

#ifndef YGOPRO_MAX_EXTRA
#define YGOPRO_MAX_EXTRA					15
#endif

#ifndef YGOPRO_MAX_SIDE
#define YGOPRO_MAX_SIDE					15
#endif

namespace ygo {

constexpr int DECK_MAX_SIZE = YGOPRO_MAX_DECK;
constexpr int DECK_MIN_SIZE = YGOPRO_MIN_DECK;
constexpr int EXTRA_MAX_SIZE = YGOPRO_MAX_EXTRA;
constexpr int SIDE_MAX_SIZE = YGOPRO_MAX_SIDE;
constexpr int PACK_MAX_SIZE = 1000;

constexpr int MAINC_MAX = 250;	// the limit of card_state
constexpr int SIDEC_MAX = MAINC_MAX;

constexpr int DECK_CATEGORY_PACK = 0;
constexpr int DECK_CATEGORY_BOT = 1;
constexpr int DECK_CATEGORY_NONE = 2;
constexpr int DECK_CATEGORY_SEPARATOR = 3;
constexpr int DECK_CATEGORY_CUSTOM = 4;

struct LFList {
	unsigned int hash{};
	std::wstring listName;
	std::unordered_map<uint32_t, int> content;
};
struct Deck {
	std::vector<code_pointer> main;
	std::vector<code_pointer> extra;
	std::vector<code_pointer> side;
	Deck() = default;
	Deck(const Deck& ndeck) {
		main = ndeck.main;
		extra = ndeck.extra;
		side = ndeck.side;
	}
	void clear() {
		main.clear();
		extra.clear();
		side.clear();
	}
};

struct DeckArray {
	std::vector<uint32_t> main;
	std::vector<uint32_t> extra;
	std::vector<uint32_t> side;
};

class DeckManager {
public:
	Deck current_deck;
	std::vector<LFList> _lfList;

	static constexpr int MAX_YDK_SIZE = 0x10000;

	void LoadLFListSingle(const char* path);
	void LoadLFList();
	const wchar_t* GetLFListName(unsigned int lfhash);
	const LFList* GetLFList(unsigned int lfhash);
	unsigned int CheckDeck(const Deck& deck, unsigned int lfhash, int rule);
#ifndef YGOPRO_SERVER_MODE
	bool LoadCurrentDeck(const wchar_t* file, bool is_packlist = false);
	bool LoadCurrentDeck(int category_index, const wchar_t* category_name, const wchar_t* deckname);
	bool LoadCurrentDeck(std::istringstream& deckStream, bool is_packlist = false);
#endif //YGOPRO_SERVER_MODE

	static uint32_t LoadDeck(Deck& deck, uint32_t dbuf[], int mainc, int sidec, bool is_packlist = false);
	static uint32_t LoadDeckFromStream(Deck& deck, std::istringstream& deckStream, bool is_packlist = false);
	static bool LoadSide(Deck& deck, uint32_t dbuf[], int mainc, int sidec);
#ifndef YGOPRO_SERVER_MODE
	static void GetCategoryPath(wchar_t* ret, int index, const wchar_t* text);
	static void GetDeckFile(wchar_t* ret, int category_index, const wchar_t* category_name, const wchar_t* deckname);
	static FILE* OpenDeckFile(const wchar_t* file, const char* mode);
	static irr::io::IReadFile* OpenDeckReader(const wchar_t* file);
	static bool SaveDeck(const Deck& deck, const wchar_t* file);
	static void SaveDeck(const Deck& deck, std::stringstream& deckStream);
	static bool DeleteDeck(const wchar_t* file);
	static bool CreateCategory(const wchar_t* name);
	static bool RenameCategory(const wchar_t* oldname, const wchar_t* newname);
	static bool DeleteCategory(const wchar_t* name);
	static bool SaveDeckArray(const DeckArray& deck, const wchar_t* name);
#endif //YGOPRO_SERVER_MODE
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
