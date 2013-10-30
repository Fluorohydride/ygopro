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
        
        unsigned int get_hash();
	};

	struct DeckData {
		std::vector<CardData*> main_deck;
		std::vector<CardData*> extra_deck;
		std::vector<CardData*> side_dack;
        
        void sort();
        void shuffle();
        wxString get_deck_string();
        void load_from_string();
	};

	class DeckMgr {
	public:
		void LoadBanLists(const wxString& file);
		void SetBanLists(unsigned int hash);

	private:
		BanListData* current_list;

	};

	extern DeckMgr deckMgr;

}

#endif
