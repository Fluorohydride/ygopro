#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include "config.h"
#include "client_card.h"
#include <unordered_map>
#include <vector>

namespace ygo {

struct LFList {
	unsigned int hash;
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

	void LoadLFListSingle(const char* path);
	void LoadLFList();
	const wchar_t* GetLFListName(int lfhash);
	const std::unordered_map<int, int>* GetLFListContent(int lfhash);
	int CheckDeck(Deck& deck, int lfhash, bool allow_ocg, bool allow_tcg);
	int LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec);
	bool LoadSide(Deck& deck, int* dbuf, int mainc, int sidec);
	void GetCategoryPath(wchar_t* ret, int index, const wchar_t* text);
	void GetDeckFile(wchar_t* ret, irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck);
	bool LoadDeck(irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck);
	FILE* OpenDeckFile(const wchar_t * file, const char * mode);
	bool LoadDeck(const wchar_t* file);
	bool SaveDeck(Deck& deck, const wchar_t* file);
	bool DeleteDeck(const wchar_t* file);
	bool CreateCategory(const wchar_t* name);
	bool RenameCategory(const wchar_t* oldname, const wchar_t* newname);
	bool DeleteCategory(const wchar_t* name);
};

extern DeckManager deckManager;

}

#endif //DECKMANAGER_H
