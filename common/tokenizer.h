#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

template<typename CHARTYPE = char>
class Tokenizer {
public:
    typedef typename std::basic_string<CHARTYPE, std::char_traits<CHARTYPE>, std::allocator<CHARTYPE>> string_type;
    typedef typename std::vector<string_type>::iterator token_iterator;
    typedef typename std::vector<string_type>::const_iterator const_token_iterator;
    
    Tokenizer(const string_type& str, const string_type& tok, bool ignore_empty = true) {
        sep_strings.clear();
        if(str.empty())
            return;
        auto begin = str.begin();
        auto end = str.end();
        while (true) {
            auto findit = begin;
            while (findit != end && tok.find(*findit) == string_type::npos)
                ++findit;
            auto findend = findit;
            if(!ignore_empty || (findend != begin))
                sep_strings.push_back(string_type(begin, findend));
            begin = findit;
            if(begin == end)
                break;
            ++begin;
        }
    }
    
    inline const_token_iterator begin() const { return sep_strings.begin(); }
    inline const_token_iterator end() const { return sep_strings.end(); }
    inline token_iterator begin() { return sep_strings.begin(); }
    inline token_iterator end() { return sep_strings.end(); }
    inline size_t size() { return sep_strings.size(); }
    inline std::string& operator[](size_t idx) { return sep_strings[idx]; }

private:
    std::vector<string_type> sep_strings;
};

#endif
