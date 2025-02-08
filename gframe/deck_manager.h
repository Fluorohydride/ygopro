#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include "config.h"
#include "data_manager.h"
#include <unordered_map>
#include <vector>
#include <sstream>

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

struct LFList {
	unsigned int hash{};
	std::wstring listName;
	std::unordered_map<int, int> content;
};
struct Deck {
	std::vector<code_pointer> main;
	std::vector<code_pointer> extra;
	std::vector<code_pointer> side;
	Deck() {}
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

class DeckManager {
public:
	Deck current_deck;
	std::vector<LFList> _lfList;

#ifndef YGOPRO_SERVER_MODE
	static char deckBuffer[0x10000];
#endif

	void LoadLFListSingle(const char* path);
	void LoadLFList();
	const wchar_t* GetLFListName(int lfhash);
	const std::unordered_map<int, int>* GetLFListContent(int lfhash);
	unsigned int CheckDeck(Deck& deck, int lfhash, int rule);
	int LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, bool is_packlist = false);
	int LoadDeck(Deck& deck, std::istringstream& deckStream, bool is_packlist = false);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
#ifndef YGOPRO_SERVER_MODE
	void GetCategoryPath(wchar_t* ret, int index, const wchar_t* text);
	void GetDeckFile(wchar_t* ret, irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck);
	FILE* OpenDeckFile(const wchar_t* file, const char* mode);
	IReadFile* OpenDeckReader(const wchar_t* file);
	bool LoadCurrentDeck(const wchar_t* file, bool is_packlist = false);
	bool LoadCurrentDeck(irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck);
	bool SaveDeck(Deck& deck, const wchar_t* file);
	bool DeleteDeck(const wchar_t* file);
	bool CreateCategory(const wchar_t* name);
	bool RenameCategory(const wchar_t* oldname, const wchar_t* newname);
	bool DeleteCategory(const wchar_t* name);
	bool SaveDeckBuffer(const int deckbuf[], const wchar_t* name);
#endif //YGOPRO_SERVER_MODE
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
