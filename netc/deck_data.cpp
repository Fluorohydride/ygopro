#include "deck_data.h"
#include "../common/hash.h"

namespace ygopro
{
    
    DeckMgr deckMgr;
    
    unsigned int BanListData::get_hash() {
        if(hash)
            return hash;
        Hash32_SHA1 out;
        for(auto iter : counts) {
            out.Append(&iter.first, sizeof(iter.first));
            out.Append(&iter.second, sizeof(iter.second));
        }
        hash = out.GetHash();
        return hash;
    }
    
    unsigned int BanListData::check_deck(DeckData& deck, unsigned int pool_type) {
        return 0;
    }
    
}
