#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include "config.h"
#include "client_card.h"
#include <unordered_map>
#include <vector>

namespace ygo {

struct LFList {
	unsigned int hash;
	wchar_t listName[20];
	std::unordered_map<int, int>* content;
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

	void LoadLFList();
	wchar_t* GetLFListName(int lfhash);
	int CheckLFList(Deck& deck, int lfhash, bool allow_ocg, bool allow_tcg);
	void LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadDeck(const wchar_t* file);
	bool SaveDeck(Deck& deck, const wchar_t* name);
	bool DeleteDeck(Deck& deck, const wchar_t* name);
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
