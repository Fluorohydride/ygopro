#ifndef _DECK_DATA_H_
#define _DECK_DATA_H_

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
    
    struct DeckCardExtraData {
        virtual ~DeckCardExtraData() = default;
    };
    
    struct DeckCardData {
        DeckCardData(CardData* d, unsigned int l): data(d), limit(l) {}
        CardData* data;
        unsigned int limit;
        std::shared_ptr<DeckCardExtraData> extra;
    };
    
	struct DeckData {
        
		std::vector<std::shared_ptr<DeckCardData>> main_deck;
		std::vector<std::shared_ptr<DeckCardData>> extra_deck;
		std::vector<std::shared_ptr<DeckCardData>> side_deck;
        std::unordered_map<unsigned int, unsigned int> counts;
        
        void Clear();
        void Sort();
        void Shuffle();
        void CalCount();
        bool LoadFromFile(const std::wstring& file);
        bool LoadFromString(const std::string& deck);
        void SaveToFile(const std::wstring& file);
        std::string SaveToString();
        
        std::shared_ptr<DeckCardData> InsertCard(unsigned int pos, unsigned int index, unsigned int code, bool checkc);
        bool RemoveCard(unsigned int pos, unsigned int index);
        
        unsigned int mcount = 0;
        unsigned int scount = 0;
        unsigned int tcount = 0;
        unsigned int syncount = 0;
        unsigned int xyzcount = 0;
        unsigned int fuscount = 0;

        static bool deck_sort(const std::shared_ptr<DeckCardData>& c1, const std::shared_ptr<DeckCardData>& c2);
        static bool deck_sort_limit(const std::shared_ptr<DeckCardData>& c1, const std::shared_ptr<DeckCardData>& c2);
	};

    struct LimitRegulation {
        unsigned int hash = 0;
		std::wstring name;
		std::unordered_map<unsigned int, unsigned int> counts;
        
        unsigned int get_hash();
        unsigned int check_deck(DeckData& deck, unsigned int pool_flag);
	};
    
	class LimitRegulationMgr : public Singleton<LimitRegulationMgr> {
	public:
		void LoadLimitRegulation(const std::wstring& file, const std::wstring& default_name);
		void SetLimitRegulation(unsigned int id);
        void SetLimitRegulation(LimitRegulation* lr);
        void GetDeckCardLimitCount(DeckData& deck);
        unsigned int GetCardLimitCount(unsigned int code);
        unsigned int CheckCurrentList(unsigned int pool);
        inline std::vector<LimitRegulation>& GetLimitRegulations() { return limit_regulations; }
        std::vector<CardData*> FilterCard(unsigned int limit, const FilterCondition& fc);
        void LoadCurrentListToDeck(DeckData& deck, int limit);
        
	private:
		LimitRegulation* current_list = nullptr;
        std::vector<LimitRegulation> limit_regulations;
	};

}

#endif
