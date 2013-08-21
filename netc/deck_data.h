#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "wx/string.h"
#include <map>
#include <vector>

namespace ygopro
{
	struct CardData;

	struct BanListData {
		wxString name;
		unsigned int hash;
		std::map<unsigned int, unsigned int> counts;
	};

	struct DeckData {
		std::vector<CardData*> main_deck;
		std::vector<CardData*> extra_deck;
		std::vector<CardData*> side_dack;
	};

	class DeckMgr {
	public:
		void LoadBanLists(const char* file);
		void SetBanLists(unsigned int hash);

	private:
		BanListData* current_list;

	};

	extern DeckMgr deckMgr;

}

#endif
