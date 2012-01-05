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
};

class DeckManager {
public:
	Deck deckhost;
	Deck deckclient;
	std::vector<LFList> _lfList;

	void LoadLFList();
	bool CheckLFList(Deck& deck, int lfindex);
	void LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadDeck(const wchar_t* file);
	void SaveDeck(Deck& deck, const wchar_t* name);
};

}

#endif //DECKMANAGER_H
