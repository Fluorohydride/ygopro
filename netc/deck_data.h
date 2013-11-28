#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "wx/string.h"
#include <map>
#include <vector>
#include <tuple>

#define DECK_NOERROR        0
#define DECK_EXCEED_COUNT   0x10000000
#define DECK_POOL_MISMATCH  0x20000000
#define DECK_UNKNOWN_CARD   0x30000000
#define DECK_COUNT_ERROR    0x40000000
#define DECK_SIDE_COUNT     0x50000000
#define DECK_SIDE_ERROR     0x60000000

namespace ygopro
{

    struct CardData;
    struct TextureInfo;
    
	struct DeckData {
		std::vector<std::tuple<CardData*, TextureInfo*, int> > main_deck;
		std::vector<std::tuple<CardData*, TextureInfo*, int> > extra_deck;
		std::vector<std::tuple<CardData*, TextureInfo*, int> > side_deck;
        
        void Clear();
        void Sort();
        void Shuffle();
        bool LoadFromFile(const wxString& file);
        bool LoadFromString(const wxString& deck);
        void SaveToFile(const wxString& file);
        wxString SaveToString();
        
        static bool deck_sort(const std::tuple<CardData*, TextureInfo*, int>& c1, const std::tuple<CardData*, TextureInfo*, int>& c2);
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
        DeckMgr(): current_list(nullptr) {}
		void LoadBanLists(const wxString& file);
		void SetBanLists(unsigned int hash);
        unsigned int CheckCurrentList(unsigned int pool);
        void GetDeckCardLimitCount(DeckData& deck);
        unsigned int GetCardLimitCount(unsigned int code);
        
	private:
		BanListData* current_list;
        std::vector<BanListData> banlists;
	};


	extern DeckMgr deckMgr;

}

#endif
