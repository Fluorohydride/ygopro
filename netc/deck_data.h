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
    struct FilterCondition;
    
	struct DeckData {
        
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > main_deck;
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > extra_deck;
		std::vector<std::tuple<CardData*, CardTextureInfo*, int> > side_deck;
        std::unordered_map<unsigned int, unsigned int> counts;
        
        void Clear();
        void Sort();
        void Shuffle();
        void CalCount();
        bool LoadFromFile(const wxString& file);
        bool LoadFromString(const wxString& deck);
        void SaveToFile(const wxString& file);
        wxString SaveToString();
        
        bool InsertCard(unsigned int code, unsigned int pos, unsigned int index = -1, bool strict = true);
        bool RemoveCard(unsigned int pos, unsigned int index);
        
        unsigned int mcount = 0;
        unsigned int scount = 0;
        unsigned int tcount = 0;
        unsigned int syncount = 0;
        unsigned int xyzcount = 0;
        unsigned int fuscount = 0;

        static bool deck_sort(const std::tuple<CardData*, CardTextureInfo*, int>& c1, const std::tuple<CardData*, CardTextureInfo*, int>& c2);
        static bool deck_sort_limit(const std::tuple<CardData*, CardTextureInfo*, int>& c1, const std::tuple<CardData*, CardTextureInfo*, int>& c2);
	};

    struct LimitRegulation {
        unsigned int hash = 0;
		wxString name;
		std::unordered_map<unsigned int, unsigned int> counts;
        
        unsigned int get_hash();
        unsigned int check_deck(DeckData& deck, unsigned int pool_flag);
	};
    
	class LimitRegulationMgr {
	public:
		void LoadLimitRegulation(const wxString& file);
		void SetLimitRegulation(unsigned int id);
        void SetLimitRegulation(LimitRegulation* lr);
        void GetDeckCardLimitCount(DeckData& deck);
        unsigned int GetCardLimitCount(unsigned int code);
        unsigned int CheckCurrentList(unsigned int pool);
        inline std::vector<LimitRegulation>& GetLimitRegulations() { return limit_regulations; }
        std::vector<CardData*> FilterCard(unsigned int limit, const FilterCondition& fc, const wxString& fs);
        void LoadCurrentListToDeck(DeckData& deck, int limit);
        
	private:
		LimitRegulation* current_list = nullptr;
        std::vector<LimitRegulation> limit_regulations;
	};


	extern LimitRegulationMgr limitRegulationMgr;

}

#endif
