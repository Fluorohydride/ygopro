#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

#include "utils/singleton.h"

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
    struct FilterCondition;
    class BuilderCard;
    
    struct DeckCardData {
        DeckCardData(CardData* d, uint32_t l): data(d), limit(l) {}
        CardData* data;
        uint32_t limit;
        std::shared_ptr<BuilderCard> builder_card = nullptr;
    };
    
	struct DeckData {

		std::vector<std::shared_ptr<DeckCardData>> main_deck;
		std::vector<std::shared_ptr<DeckCardData>> extra_deck;
		std::vector<std::shared_ptr<DeckCardData>> side_deck;
        std::unordered_map<uint32_t, uint32_t> counts;
        
        void Clear();
        void Sort();
        void Shuffle();
        void CalCount();
        bool LoadFromFile(const std::string& file);
        bool LoadFromString(const std::string& deck);
        void SaveToFile(const std::string& file);
        std::string SaveToString();
        
        std::shared_ptr<DeckCardData> InsertCard(uint32_t pos, uint32_t index, uint32_t code, bool checkc);
        bool RemoveCard(uint32_t pos, uint32_t index);
        
        uint32_t mcount = 0;
        uint32_t scount = 0;
        uint32_t tcount = 0;
        uint32_t syncount = 0;
        uint32_t xyzcount = 0;
        uint32_t fuscount = 0;

        static bool deck_sort(const std::shared_ptr<DeckCardData>& c1, const std::shared_ptr<DeckCardData>& c2);
        static bool deck_sort_limit(const std::shared_ptr<DeckCardData>& c1, const std::shared_ptr<DeckCardData>& c2);
	};

    struct LimitRegulation {
        uint32_t hash = 0;
		std::wstring name;
		std::unordered_map<uint32_t, uint32_t> counts;
        
        uint32_t get_hash();
        uint32_t check_deck(DeckData& deck, uint32_t pool_flag);
	};
    
	class LimitRegulationMgr : public Singleton<LimitRegulationMgr> {
	public:
		void LoadLimitRegulation(const std::string& file, const std::wstring& default_name);
		void SetLimitRegulation(uint32_t id);
        void SetLimitRegulation(LimitRegulation* lr);
        void GetDeckCardLimitCount(DeckData& deck);
        uint32_t GetCardLimitCount(uint32_t code);
        uint32_t CheckCurrentList(uint32_t pool);
        inline std::vector<LimitRegulation>& GetLimitRegulations() { return limit_regulations; }
        std::vector<CardData*> FilterCard(uint32_t limit, const FilterCondition& fc);
        void LoadCurrentListToDeck(DeckData& deck, int32_t limit);
        
	private:
		LimitRegulation* current_list = nullptr;
        std::vector<LimitRegulation> limit_regulations;
	};

}

#endif
