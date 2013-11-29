#include "card_data.h"
#include "deck_data.h"

#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include "../common/hash.h"
#include <algorithm>

namespace ygopro
{
    
    LimitRegulationMgr limitRegulationMgr;
    
    void DeckData::Clear() {
        main_deck.clear();
        extra_deck.clear();
        side_deck.clear();
    }
    
    void DeckData::Sort() {
        std::sort(main_deck.begin(), main_deck.end(), deck_sort);
        std::sort(extra_deck.begin(), extra_deck.end(), deck_sort);
        std::sort(side_deck.begin(), side_deck.end(), deck_sort);
    }
    
    void DeckData::Shuffle() {
        std::random_shuffle(main_deck.begin(), main_deck.end());
        std::random_shuffle(extra_deck.begin(), extra_deck.end());
        std::random_shuffle(side_deck.begin(), side_deck.end());
    }
    
    bool DeckData::LoadFromFile(const wxString& file) {
        wxFileInputStream deck_file(file);
        if(!deck_file.IsOk())
            return false;
        main_deck.clear();
        extra_deck.clear();
        side_deck.clear();
        wxTextInputStream ts(deck_file);
        bool side = false;
        unsigned long code;
        while(!deck_file.Eof()) {
            wxString line = ts.ReadLine();
            if(line.IsEmpty() || line[0] == wxT('#'))
                continue;
            if(line[0] == wxT('!')) {
                side = true;
                continue;
            }
            line.ToULong(&code);
            CardData* ptr = dataMgr[(unsigned int)code];
            if(ptr == nullptr || (ptr->type & 0x4000))
                continue;
            if(side) {
                side_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            } else {
                if(ptr->type & 0x802040)
                    extra_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                else
                    main_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            }
        }
        return true;
    }
    
    bool DeckData::LoadFromString(const wxString& deck) {
        static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        static unsigned char base64_dec_table[256] = {0};
        static bool base64_dec_init = false;
        if(!base64_dec_init) {
            base64_dec_init = true;
            for(int i = 0; i < 64; ++i)
                base64_dec_table[(int)base64_chars[i]] = i;
        }
        if(deck.size() % 5)
            return false;
        for(size_t i = 0; i < deck.size(); i += 5) {
            unsigned int packed_data = (unsigned int)base64_dec_table[(unsigned char)deck[i]];
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 1]]) << 6;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 2]]) << 12;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 3]]) << 18;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 4]]) << 24;
            unsigned int code = packed_data & 0x7ffffff;
            unsigned int count = (packed_data >> 27) & 0x3;
            CardData* ptr = dataMgr[code];
            if(ptr == nullptr || (ptr->type & 0x4000))
                continue;
            if(packed_data & 0x20000000) {
                for(unsigned int j = 0; j < count; ++j)
                    side_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            } else {
                if(ptr->type & 0x802040)
                    for(unsigned int j = 0; j < count; ++j)
                        extra_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                else
                    for(unsigned int j = 0; j < count; ++j)
                        main_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            }
        }
        return true;
    }
    
    void DeckData::SaveToFile(const wxString& file) {
        wxFileOutputStream deck_file(file);
        if(!deck_file.IsOk())
            return;
        wxTextOutputStream ts(deck_file);
        ts << "#Created by ygopro deck editor." << endl << "#main" << endl;
        for(auto& cd : main_deck)
            ts << std::get<0>(cd)->code << " #" << std::get<0>(cd)->name << endl;
        for(auto& cd : extra_deck)
            ts << std::get<0>(cd)->code << " #" << std::get<0>(cd)->name << endl;
        if(side_deck.size()) {
            ts << "!side" << endl;
            for(auto& cd : side_deck)
                ts << std::get<0>(cd)->code << " #" << std::get<0>(cd)->name << endl;
        }
    }
    
    wxString DeckData::SaveToString() {
        static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        Sort();
        char deck_string[1024] = {0};
        char* deck_pstr = deck_string;
        for(size_t i = 0; i < main_deck.size(); ++i) {
            unsigned int count = 1;
            unsigned int code = std::get<0>(main_deck[i])->code;
            while(i < main_deck.size() - 1 && code == std::get<0>(main_deck[i + 1])->code) {
                count++;
                i++;
            }
            unsigned int packed_data = (code & 0x7ffffff) | (count << 27);
            deck_pstr[0] = base64_chars[packed_data & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[4] = base64_chars[(packed_data >> 24) & 0x3f];
            deck_pstr += 5;
        }
        for(size_t i = 0; i < extra_deck.size(); ++i) {
            unsigned int count = 1;
            unsigned int code = std::get<0>(extra_deck[i])->code;
            while(i < extra_deck.size() - 1 && code == std::get<0>(extra_deck[i + 1])->code) {
                count++;
                i++;
            }
            unsigned int packed_data = (code & 0x7ffffff) | (count << 27);
            deck_pstr[0] = base64_chars[packed_data & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[4] = base64_chars[(packed_data >> 24) & 0x3f];
            deck_pstr += 5;
        }
        for(size_t i = 0; i < side_deck.size(); ++i) {
            unsigned int count = 1;
            unsigned int code = std::get<0>(side_deck[i])->code;
            while(i < side_deck.size() - 1 && code == std::get<0>(side_deck[i + 1])->code) {
                count++;
                i++;
            }
            unsigned int packed_data = (code & 0x7ffffff) | (count << 27) | 0x20000000;
            deck_pstr[0] = base64_chars[packed_data & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[4] = base64_chars[(packed_data >> 24) & 0x3f];
            deck_pstr += 5;
        }
        return deck_string;
    }
    
    bool DeckData::deck_sort(const std::tuple<CardData*, CardTextureInfo*, int>& c1, const std::tuple<CardData*, CardTextureInfo*, int>& c2) {
        CardData* p1 = std::get<0>(c1);
        CardData* p2 = std::get<0>(c2);
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
    
    unsigned int LimitRegulation::get_hash() {
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
    
    unsigned int LimitRegulation::check_deck(DeckData& deck, unsigned int pool_type) {
        return 0;
    }
    
    void LimitRegulationMgr::LoadLimitRegulation(const wxString& file) {
        wxFileInputStream ban_file(file);
        if(!ban_file.IsOk())
            return;
        wxTextInputStream ts(ban_file);
        unsigned long code, count;
        LimitRegulation* plist = nullptr;
        while(!ban_file.Eof()) {
            wxString line = ts.ReadLine();
            if(line.IsEmpty() || line[0] == wxT('#'))
                continue;
            if(line[0] == wxT('!')) {
                limit_regulations.resize(limit_regulations.size() + 1);
                plist = &(*limit_regulations.rbegin());
                plist->name = line.Right(line.Length() - 1);
                continue;
            }
            if(plist == nullptr)
                continue;
            wxStringTokenizer tk(line, wxT(" \t\r\n"), wxTOKEN_STRTOK);
            tk.GetNextToken().ToULong(&code);
            tk.GetNextToken().ToULong(&count);
            if(code == 0)
                continue;
            plist->counts[(unsigned int)code] = (unsigned int)count;
        }
        if(limit_regulations.size() > 0)
            current_list = &limit_regulations[0];
    }
    
    void LimitRegulationMgr::SetLimitRegulation(unsigned int hash) {
        for(auto& lr : limit_regulations) {
            if(lr.get_hash() == hash) {
                current_list = &lr;
                return;
            }
        }
    }
    
    void LimitRegulationMgr::SetLimitRegulation(LimitRegulation* lr) {
        if(lr)
            current_list = lr;
    }
    
    unsigned int LimitRegulationMgr::CheckCurrentList(unsigned int pool) {
        if(!current_list)
            return DECK_NOERROR;
        return DECK_NOERROR;
    }
    
    void LimitRegulationMgr::GetDeckCardLimitCount(DeckData& deck) {
        if(!current_list) {
            for(auto& cd : deck.main_deck)
                std::get<2>(cd) = 3;
            for(auto& cd : deck.extra_deck)
                std::get<2>(cd) = 3;
            for(auto& cd : deck.side_deck)
                std::get<2>(cd) = 3;
            return;
        }
        for(auto& cd : deck.main_deck) {
            unsigned int code = std::get<0>(cd)->code;
            auto iter = current_list->counts.find(code);
            if(iter == current_list->counts.end())
                std::get<2>(cd) = 3;
            else
                std::get<2>(cd) = iter->second;
        }
        for(auto& cd : deck.extra_deck) {
            unsigned int code = std::get<0>(cd)->code;
            auto iter = current_list->counts.find(code);
            if(iter == current_list->counts.end())
                std::get<2>(cd) = 3;
            else
                std::get<2>(cd) = iter->second;
        }
        for(auto& cd : deck.side_deck) {
            unsigned int code = std::get<0>(cd)->code;
            auto iter = current_list->counts.find(code);
            if(iter == current_list->counts.end())
                std::get<2>(cd) = 3;
            else
                std::get<2>(cd) = iter->second;
        }
    }
    
    unsigned int LimitRegulationMgr::GetCardLimitCount(unsigned int code) {
        if(!current_list)
            return 3;
        auto iter = current_list->counts.find(code);
        if(iter == current_list->counts.end())
            return 3;
        else
            return iter->second;
    }
}
