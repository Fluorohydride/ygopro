#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include <unordered_map>
#include <vector>
#include "network.h"
#include "text_types.h"
#include "client_card.h"

namespace ygo {

struct LFList {
	unsigned int hash;
	std::wstring listName;
	std::unordered_map<uint32_t, int> content;
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
enum class DuelAllowedCards {
	ALLOWED_CARDS_OCG_ONLY,
	ALLOWED_CARDS_TCG_ONLY,
	ALLOWED_CARDS_OCG_TCG,
	ALLOWED_CARDS_WITH_PRERELEASE,
	ALLOWED_CARDS_ANY
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
	LFList* GetLFList(int lfhash);
	std::wstring GetLFListName(int lfhash);
	DeckError CheckDeck(Deck& deck, int lfhash, DuelAllowedCards allowedCards, bool doubled, int forbiddentypes = 0);
	int TypeCount(std::vector<CardDataC*> cards, int type);
	int LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, int mainc2 = 0, int sidec2 = 0);
	int LoadDeck(Deck& deck, std::vector<int> mainlist, std::vector<int> sidelist);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadDeck(const path_string& file, Deck* deck = nullptr);
	bool LoadDeckDouble(const path_string& file, const path_string& file2, Deck* deck = nullptr);
	bool SaveDeck(Deck& deck, const path_string& name);
	bool SaveDeck(const path_string& name, std::vector<int> mainlist, std::vector<int>extralist, std::vector<int> sidelist);
	const wchar_t* ExportDeckBase64(Deck& deck);
	const wchar_t* ExportDeckCardNames(Deck deck);
	void ImportDeckBase64(Deck& deck, const wchar_t* buffer);
	bool DeleteDeck(Deck& deck, const path_string& name);
	bool RenameDeck(const path_string& oldname, const path_string& newname);
};

extern DeckManager* gdeckManager;

}

#endif //DECKMANAGER_H
