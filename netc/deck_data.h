#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "wx/string.h"
#include <unordered_map>
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
    struct CardTextureInfo;
    
	struct DeckData {
        DeckData(): mcount(0), scount(0), tcount(0), syncount(0), xyzcount(0), fuscount(0) {}
        
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > main_deck;
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > extra_deck;
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > side_deck;
        std::unordered_map<unsigned int, unsigned int> counts;
        
        void Clear();
        void Sort();
        void Shuffle();
        bool LoadFromFile(const wxString& file);
        bool LoadFromString(const wxString& deck);
        void SaveToFile(const wxString& file);
        wxString SaveToString();
        
        bool InsertCard(unsigned int code, unsigned int pos, unsigned int index = -1);
        bool RemoveCard(unsigned int pos, unsigned int index);
        bool MoveCard(unsigned int pos1, unsigned int index1, unsigned int pos2, unsigned int index2);
        
        unsigned int mcount;
        unsigned int scount;
        unsigned int tcount;
        unsigned int syncount;
        unsigned int xyzcount;
        unsigned int fuscount;

        static bool deck_sort(const std::tuple<CardData*, CardTextureInfo*, int>& c1, const std::tuple<CardData*, CardTextureInfo*, int>& c2);
	};

    struct LimitRegulation {
        unsigned int hash;
		wxString name;
		std::unordered_map<unsigned int, unsigned int> counts;
        
        LimitRegulation(): hash(0) {}
        unsigned int get_hash();
        unsigned int check_deck(DeckData& deck, unsigned int pool_flag);
	};
    
	class LimitRegulationMgr {
	public:
        LimitRegulationMgr(): current_list(nullptr) {}
		void LoadLimitRegulation(const wxString& file);
		void SetLimitRegulation(unsigned int id);
        void SetLimitRegulation(LimitRegulation* lr);
        void GetDeckCardLimitCount(DeckData& deck);
        unsigned int GetCardLimitCount(unsigned int code);
        unsigned int CheckCurrentList(unsigned int pool);
        inline std::vector<LimitRegulation>& GetLimitRegulations() { return limit_regulations; }
        
	private:
		LimitRegulation* current_list;
        std::vector<LimitRegulation> limit_regulations;
	};


	extern LimitRegulationMgr limitRegulationMgr;

}

#endif
