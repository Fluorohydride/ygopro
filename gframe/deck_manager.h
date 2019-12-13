#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include "utils.h"
#include "client_card.h"
#include <unordered_map>
#include <vector>

namespace ygo {

struct LFList {
	unsigned int hash;
	std::wstring listName;
	std::unordered_map<int, int> content;
	bool whitelist;
};
struct Deck {
	std::vector<CardDataC*> main;
	std::vector<CardDataC*> extra;
	std::vector<CardDataC*> side;
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
private:
	int null_lflist_index = -1;
public:
	Deck current_deck;
	Deck pre_deck;
	std::vector<LFList> _lfList;

	bool LoadLFListSingle(const path_string& path);
	bool LoadLFListFolder(path_string path);
	void LoadLFList();
	void RefreshLFList();
	std::wstring GetLFListName(int lfhash);
	int CheckDeck(Deck& deck, int lfhash, bool allow_ocg, bool allow_tcg, bool allow_prerelease, bool doubled, int forbiddentypes = 0);
	int TypeCount(std::vector<CardDataC*> cards, int type);
	int LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, int mainc2 = 0, int sidec2 = 0);
	int LoadDeck(Deck& deck, std::vector<int> mainlist, std::vector<int> sidelist);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadDeck(const path_string& file, Deck* deck = nullptr);
	bool LoadDeckDouble(const path_string& file, const path_string& file2, Deck* deck = nullptr);
	bool SaveDeck(Deck& deck, const path_string& name);
	bool SaveDeck(const path_string& name, std::vector<int> mainlist, std::vector<int>extralist, std::vector<int> sidelist);
	bool DeleteDeck(Deck& deck, const path_string& name);
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
