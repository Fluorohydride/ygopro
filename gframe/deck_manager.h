#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include <unordered_map>
#include <vector>
#include <sstream>
#include "data_manager.h"

namespace ygo {
	constexpr int DECK_MAX_SIZE = 60;
	constexpr int DECK_MIN_SIZE = 40;
	constexpr int EXTRA_MAX_SIZE = 15;
	constexpr int SIDE_MAX_SIZE = 15;
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

	static char deckBuffer[0x10000];

	void LoadLFListSingle(const char* path);
	void LoadLFList();
	const wchar_t* GetLFListName(int lfhash);
	const std::unordered_map<int, int>* GetLFListContent(int lfhash);
	unsigned int CheckDeck(Deck& deck, int lfhash, int rule);
	int LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, bool is_packlist = false);
	int LoadDeck(Deck& deck, std::istringstream& deckStream, bool is_packlist = false);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
	void GetCategoryPath(wchar_t* ret, int index, const wchar_t* text);
	void GetDeckFile(wchar_t* ret, int category_index, const wchar_t* category_name, const wchar_t* deckname);
	FILE* OpenDeckFile(const wchar_t* file, const char* mode);
	irr::io::IReadFile* OpenDeckReader(const wchar_t* file);
	bool LoadCurrentDeck(const wchar_t* file, bool is_packlist = false);
	bool LoadCurrentDeck(int category_index, const wchar_t* category_name, const wchar_t* deckname);
	bool SaveDeck(Deck& deck, const wchar_t* file);
	bool DeleteDeck(const wchar_t* file);
	bool CreateCategory(const wchar_t* name);
	bool RenameCategory(const wchar_t* oldname, const wchar_t* newname);
	bool DeleteCategory(const wchar_t* name);
	bool SaveDeckBuffer(const int deckbuf[], const wchar_t* name);
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
