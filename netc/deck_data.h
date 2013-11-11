#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "wx/string.h"
#include <map>
#include <vector>

namespace ygopro
{
	struct CardData;

	struct DeckData {
		std::vector<CardData*> main_deck;
		std::vector<CardData*> extra_deck;
		std::vector<CardData*> side_dack;
        
        void sort();
        void shuffle();
        wxString get_deck_string();
        void load_from_string(const wxString& deck);
	};

    struct BanListData {
        unsigned int hash;
		wxString name;
		std::map<unsigned int, unsigned int> counts;
        
        BanListData(): hash(0) {}
        unsigned int get_hash();
        unsigned int check_deck(DeckData& deck, unsigned int pool_flag);
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
