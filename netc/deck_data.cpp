#include "card_data.h"
#include "deck_data.h"

#include "wx/wfstream.h"
#include "wx/txtstrm.h"
#include "wx/tokenzr.h"
#include "../common/hash.h"
#include <algorithm>

namespace ygopro
{
    
    LimitRegulationMgr limitRegulationMgr;
    
    void DeckData::Clear() {
        main_deck.clear();
        extra_deck.clear();
        side_deck.clear();
        counts.clear();
        mcount = 0;
        scount = 0;
        tcount = 0;
        syncount = 0;
        xyzcount = 0;
        fuscount = 0;
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
            if(side)
                side_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            else {
                if(ptr->type & 0x802040) {
                    extra_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                    if(ptr->type & 0x800000)
                        xyzcount++;
                    else if(ptr->type & 0x2000)
                        syncount++;
                    else
                        fuscount++;
                } else {
                    main_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                    if(ptr->type & 0x1)
                        mcount++;
                    else if(ptr->type & 0x2)
                        scount++;
                    else
                        tcount++;
                }
            }
            counts[(unsigned int)code]++;
        }
        limitRegulationMgr.GetDeckCardLimitCount(*this);
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
            unsigned int packed_data = (unsigned int)base64_dec_table[(unsigned char)deck[i + 4]];
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 3]]) << 6;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 2]]) << 12;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 1]]) << 18;
            packed_data |= ((unsigned int)base64_dec_table[(unsigned char)deck[i + 0]]) << 24;
            unsigned int code = packed_data & 0x7ffffff;
            unsigned int count = (packed_data >> 27) & 0x3;
            CardData* ptr = dataMgr[code];
            if(ptr == nullptr || (ptr->type & 0x4000))
                continue;
            if(packed_data & 0x20000000) {
                for(unsigned int j = 0; j < count; ++j)
                    side_deck.push_back(std::make_tuple(ptr, nullptr, 0));
            } else {
                if(ptr->type & 0x802040) {
                    for(unsigned int j = 0; j < count; ++j) {
                        extra_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                        if(ptr->type & 0x800000)
                            xyzcount++;
                        else if(ptr->type & 0x2000)
                            syncount++;
                        else
                            fuscount++;
                    }
                } else {
                    for(unsigned int j = 0; j < count; ++j) {
                        main_deck.push_back(std::make_tuple(ptr, nullptr, 0));
                        if(ptr->type & 0x1)
                            mcount++;
                        else if(ptr->type & 0x2)
                            scount++;
                        else
                            tcount++;
                    }
                }
            }
            counts[code]++;
        }
        limitRegulationMgr.GetDeckCardLimitCount(*this);
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
            deck_pstr[4] = base64_chars[packed_data & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[0] = base64_chars[(packed_data >> 24) & 0x3f];
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
            deck_pstr[4] = base64_chars[packed_data & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[0] = base64_chars[(packed_data >> 24) & 0x3f];
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
            deck_pstr[4] = base64_chars[packed_data & 0x3f];
            deck_pstr[3] = base64_chars[(packed_data >> 6) & 0x3f];
            deck_pstr[2] = base64_chars[(packed_data >> 12) & 0x3f];
            deck_pstr[1] = base64_chars[(packed_data >> 18) & 0x3f];
            deck_pstr[0] = base64_chars[(packed_data >> 24) & 0x3f];
            deck_pstr += 5;
        }
        return deck_string;
    }
    
    bool DeckData::InsertCard(unsigned int code, unsigned int pos, unsigned int index, bool strict) {
        CardData* cd = dataMgr[code];
        if(cd == nullptr || (cd->type & 0x4000))
            return false;
        unsigned int limit = limitRegulationMgr.GetCardLimitCount(code);
        if(counts[code] >= limit)
            return false;
        auto tp = std::make_tuple(cd, nullptr, limit);
        if(pos == 1 || pos == 2) {
            if(cd->type & 0x802040) {
                if(strict && pos == 1)
                    return false;
                if(index >= extra_deck.size())
                    extra_deck.push_back(tp);
                else
                    extra_deck.insert(extra_deck.begin() + index, tp);
                if(cd->type & 0x800000)
                    xyzcount++;
                else if(cd->type & 0x2000)
                    syncount++;
                else
                    fuscount++;
            } else {
                if(strict && pos == 2)
                    return false;
                if(index >= main_deck.size())
                    main_deck.push_back(tp);
                else
                    main_deck.insert(main_deck.begin() + index, tp);
                if(cd->type & 0x1)
                    mcount++;
                else if(cd->type & 0x2)
                    scount++;
                else
                    tcount++;
            }
        } else if(pos == 3) {
            if(index >= side_deck.size())
                side_deck.push_back(tp);
            else
                side_deck.insert(side_deck.begin() + index, tp);
        }
        counts[code]++;
        return true;
    }
    
    bool DeckData::RemoveCard(unsigned int pos, unsigned int index) {
        if(pos == 0)
            return false;
        if(pos == 1) {
            if(index >= main_deck.size())
                return false;
            auto& ecd = main_deck[index];
            CardData* cd = std::get<0>(ecd);
            if(cd->type & 0x1)
                mcount--;
            else if(cd->type & 0x2)
                scount--;
            else
                tcount--;
            main_deck.erase(main_deck.begin() + index);
            counts[cd->code]--;
        } else if(pos == 2) {
            if(index >= extra_deck.size())
                return false;
            auto& ecd = extra_deck[index];
            CardData* cd = std::get<0>(ecd);
            if(cd->type & 0x800000)
                xyzcount--;
            else if(cd->type & 0x2000)
                syncount--;
            else
                fuscount--;
            extra_deck.erase(extra_deck.begin() + index);
            counts[cd->code]--;
        } else if(pos == 3) {
            if(index >= side_deck.size())
                return false;
            auto& ecd = side_deck[index];
            CardData* cd = std::get<0>(ecd);
            side_deck.erase(side_deck.begin() + index);
            counts[cd->code]--;
        }
        return true;
    }
    
    bool DeckData::MoveCard(unsigned int pos1, unsigned int index1, unsigned int pos2, unsigned int index2) {
        if(pos1 == pos2 && index1 == index2)
            return false;
        if(pos1 == 0 || pos2 == 0)
            return false;
        if(pos1 == pos2) {
            if(pos1 == 1) {
                if(index1 >= main_deck.size() || index2 >= main_deck.size())
                    return false;
                auto data = main_deck[index1];
                if(index1 > index2) {
                    for(size_t i = index1; i != index2; --i)
                        main_deck[i] = main_deck[i - 1];
                } else {
                    for(size_t i = index1; i != index2; ++i)
                        main_deck[i] = main_deck[i + 1];
                }
                main_deck[index2] = data;
            } else if(pos1 == 2) {
                if(index1 >= extra_deck.size() || index2 >= extra_deck.size())
                    return false;
                auto data = extra_deck[index1];
                if(index1 > index2) {
                    for(size_t i = index1; i != index2; --i)
                        extra_deck[i] = extra_deck[i - 1];
                } else {
                    for(size_t i = index1; i != index2; ++i)
                        extra_deck[i] = extra_deck[i + 1];
                }
                extra_deck[index2] = data;
            } else {
                if(index1 >= side_deck.size() || index2 >= side_deck.size())
                    return false;
                auto data = side_deck[index1];
                if(index1 > index2) {
                    for(size_t i = index1; i != index2; --i)
                        side_deck[i] = side_deck[i - 1];
                } else {
                    for(size_t i = index1; i != index2; ++i)
                        side_deck[i] = side_deck[i + 1];
                }
                side_deck[index2] = data;
            }
        } else {
            if(pos1 == 1) {
                if(pos2 != 3)
                    return false;
                auto& ecd = main_deck[index1];
                CardData* cd = std::get<0>(ecd);
                if(cd->type & 0x1)
                    mcount--;
                else if(cd->type & 0x2)
                    scount--;
                else
                    tcount--;
                if(index2 >= side_deck.size())
                    side_deck.push_back(ecd);
                else
                    side_deck.insert(side_deck.begin() + index2, ecd);
                main_deck.erase(main_deck.begin() + index1);
            } else if(pos1 == 2) {
                if(pos2 != 3)
                    return false;
                auto& ecd = extra_deck[index1];
                CardData* cd = std::get<0>(ecd);
                if(cd->type & 0x800000)
                    xyzcount--;
                else if(cd->type & 0x2000)
                    syncount--;
                else
                    fuscount--;
                if(index2 >= side_deck.size())
                    side_deck.push_back(ecd);
                else
                    side_deck.insert(side_deck.begin() + index2, ecd);
                extra_deck.erase(extra_deck.begin() + index1);
            } else if(pos2 == 1) {
                auto& ecd = side_deck[index1];
                CardData* cd = std::get<0>(ecd);
                if(cd->type & 0x802040)
                    return false;
                if(cd->type & 0x1)
                    mcount++;
                else if(cd->type & 0x2)
                    scount++;
                else
                    tcount++;
                if(index2 >= main_deck.size())
                    main_deck.push_back(ecd);
                else
                    main_deck.insert(main_deck.begin() + index2, ecd);
                side_deck.erase(side_deck.begin() + index1);
            } else {
                auto& ecd = side_deck[index1];
                CardData* cd = std::get<0>(ecd);
                if(!(cd->type & 0x802040))
                    return false;
                if(cd->type & 0x800000)
                    xyzcount++;
                else if(cd->type & 0x2000)
                    syncount++;
                else
                    fuscount++;
                if(index2 >= extra_deck.size())
                    extra_deck.push_back(ecd);
                else
                extra_deck.insert(extra_deck.begin() + index2, ecd);
                side_deck.erase(side_deck.begin() + index1);
            }
        }
        return true;
    }
    
    bool DeckData::deck_sort(const std::tuple<CardData*, CardTextureInfo*, int>& c1, const std::tuple<CardData*, CardTextureInfo*, int>& c2) {
        CardData* p1 = std::get<0>(c1);
        CardData* p2 = std::get<0>(c2);
        return CardData::card_sort(p1, p2);
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
        limit_regulations.resize(limit_regulations.size() + 1);
        plist = &(*limit_regulations.rbegin());
        plist->name = wxT("-- No limit --");
        current_list = &limit_regulations[0];
    }
    
    void LimitRegulationMgr::SetLimitRegulation(unsigned int id) {
		if (id >= limit_regulations.size())
			return;
		current_list = &limit_regulations[id];
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
    
    void LimitRegulationMgr::FilterCard(unsigned int limit, const FilterCondition& fc, const wxString& fs, std::vector<CardData*>& result) {
        for(auto& iter : current_list->counts) {
            if(iter.second != limit)
                continue;
            CardData* cd = dataMgr[iter.first];
            if(cd && cd->CheckCondition(fc, fs))
                result.push_back(cd);
        }
        if(result.size())
            std::sort(result.begin(), result.end(), CardData::card_sort);
    }
    
}
