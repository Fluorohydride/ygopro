#include "card_data.h"
#include "deck_data.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "../common/hash.h"

namespace ygopro
{
    
    DeckMgr deckMgr;
    
    void DeckData::sort() {
        
    }
    
    void DeckData::shuffle() {
        
    }
    
    bool DeckData::load_from_file(const wxString& file) {
        wxFileInputStream deck_file(file);
        if(!deck_file.IsOk())
            return false;
        wxTextInputStream ts(deck_file);
        wxString line = ts.ReadLine();
        while(line.length() > 0) {
            if(line[0] == wxT('#'))
                continue;
        }
        return true;
    }
    
    void DeckData::save_to_file(const wxString& file) {
        
    }
    
    bool DeckData::load_from_string(const wxString& deck) {
        return true;
    }
    
    wxString DeckData::save_to_string() {
        return "";
    }
    
    bool DeckData::deck_sort(const std::pair<CardData*, void*>& c1, const std::pair<CardData*, void*>& c2) {
        CardData* p1 = c1.first;
        CardData* p2 = c2.first;
        if((p1->type & 0x7) != (p2->type & 0x7))
            return (p1->type & 0x7) < (p2->type & 0x7);
        if((p1->type & 0x7) == 1) {
            int type1 = (p1->type & 0x8020c0) ? (p1->type & 0x8020c1) : (p1->type & 0x31);
            int type2 = (p2->type & 0x8020c0) ? (p2->type & 0x8020c1) : (p2->type & 0x31);
            if(type1 != type2)
                return type1 < type2;
            if(p1->level != p2->level)
                return p1->level > p2->level;
            if(p1->attack != p2->attack)
                return p1->attack > p2->attack;
            if(p1->defence != p2->defence)
                return p1->defence > p2->defence;
            else return p1->code < p2->code;
        }
        if((p1->type & 0xfffffff8) != (p2->type & 0xfffffff8))
            return (p1->type & 0xfffffff8) < (p2->type & 0xfffffff8);
        return p1->code < p2->code;
    }
    
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
