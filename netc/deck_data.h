#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "wx/string.h"
#include <map>
#include <vector>

namespace ygopro
{

    struct CardData;
    
	struct DeckData {
		std::vector<std::pair<CardData*, void*> > main_deck;
		std::vector<std::pair<CardData*, void*> > extra_deck;
		std::vector<std::pair<CardData*, void*> > side_deck;
        
        void sort();
        void shuffle();
        bool load_from_file(const wxString& file);
        void save_to_file(const wxString& file);
        bool load_from_string(const wxString& deck);
        wxString save_to_string();
        
        bool deck_sort(const std::pair<CardData*, void*>& c1, const std::pair<CardData*, void*>& c2);
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
