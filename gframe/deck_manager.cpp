#include <algorithm>
#include <fstream>
#include <fmt/format.h>
#include <zlib.h>
#include "network.h"
#include "deck_manager.h"
#include "data_manager.h"
#include "game.h"
#include <IGUIEditBox.h>
#include "Base64.h"
#include "utils.h"
#include "client_card.h"

namespace ygo {
CardDataC* DeckManager::GetDummyOrMappedCardData(uint32_t code) {
	if(!load_dummies)
		return GetMappedCardData(code);
	auto it = dummy_entries.find(code);
	if(it != dummy_entries.end())
		return it->second;
	CardDataC* tmp = new CardDataC();
	tmp->code = 0;
	tmp->alias = code;
	dummy_entries[code] = tmp;
	return tmp;
}
CardDataC* DeckManager::GetMappedCardData(uint32_t code) {
	auto it = mapped_ids.find(code);
	if(it != mapped_ids.end())
		return gDataManager->GetCardData(it->second);
	return nullptr;
}
void DeckManager::ClearDummies() {
	for(auto& card : dummy_entries) {
		delete card.second;
	}
	dummy_entries.clear();
}
bool DeckManager::LoadLFListSingle(const epro::path_string& path) {
	static const char key[] = "$whitelist";
	bool loaded = false;
	std::ifstream infile(path, std::ifstream::in);
	if(!infile.is_open())
		return loaded;
	LFList lflist;
	lflist.hash = 0;
	std::string str;
	while(std::getline(infile, str)) {
		auto pos = str.find('\r');
		if(str.size() && pos != std::string::npos)
			str.erase(pos);
		if(str.empty() || str[0] == '#')
			continue;
		if(str[0] == '!') {
			if(lflist.hash)
				_lfList.push_back(lflist);
			lflist.listName = BufferIO::DecodeUTF8({ str.data() + 1, str.size() - 1 });
			lflist.content.clear();
			lflist.hash = 0x7dfcee6a;
			lflist.whitelist = false;
			loaded = true;
			continue;
		}
		if(str.rfind(key, 0) == 0)
			lflist.whitelist = true;
		if(!lflist.hash)
			continue;
		pos = str.find(" ");
		if(pos == std::string::npos)
			continue;
		uint32_t code = 0;
		try { code = std::stoul(str.substr(0, pos)); }
		catch(...){}
		if(!code)
			continue;
		str.erase(0, pos + 1);
		str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
		pos = str.find(" ");
		if(pos == std::string::npos)
			continue;
		int count = 0;
		try { count = std::stoi(str.substr(0, pos)); }
		catch(...) { continue; }
		lflist.content[code] = count;
		lflist.hash = lflist.hash ^ ((code << 18) | (code >> 14)) ^ ((code << (27 + count)) | (code >> (5 - count)));
	}
	if(lflist.hash)
		_lfList.push_back(lflist);
	infile.close();
	return loaded;
}
bool DeckManager::LoadLFListFolder(epro::path_string path) {
	path = Utils::NormalizePath(path);
	bool loaded = false;
	auto lflists = Utils::FindFiles(path, { EPRO_TEXT("conf") });
	for (const auto& lflist : lflists) {
		loaded = LoadLFListSingle(path + lflist);
	}
	return loaded;
}
void DeckManager::LoadLFList() {
	LoadLFListSingle(EPRO_TEXT("./expansions/lflist.conf"));
	LoadLFListSingle(EPRO_TEXT("./lflist.conf"));
	LoadLFListFolder(EPRO_TEXT("./lflists/"));
	LFList nolimit;
	nolimit.listName = L"N/A"; // N/A
	nolimit.hash = 0;
	nolimit.content.clear();
	nolimit.whitelist = false;
	_lfList.push_back(nolimit);
	null_lflist_index = _lfList.size() - 1;
}
//moves the "N/A" lflist at the bottom of the vector
void DeckManager::RefreshLFList() {
	if(null_lflist_index != -1 && null_lflist_index != _lfList.size() -1) {
		auto it = _lfList.begin() + null_lflist_index;
		std::rotate(it, it + 1, _lfList.end());
		null_lflist_index = _lfList.size() - 1;
	}
}
void DeckManager::RefreshDeck(Deck& deck) {
	for(auto& list : { &deck.main, &deck.extra, &deck.side }) {
		for(auto& card : *list) {
			if(card->code == 0 && card->alias) {
				if(auto cd = gDataManager->GetCardData(card->alias)) {
					card = cd;
				}
			}
		}
	}
}
LFList* DeckManager::GetLFList(uint32_t lfhash) {
	auto it = std::find_if(_lfList.begin(), _lfList.end(), [lfhash](LFList list) {return list.hash == lfhash; });
	return it != _lfList.end() ? &*it : nullptr;
}
epro::wstringview DeckManager::GetLFListName(uint32_t lfhash) {
	auto lflist = GetLFList(lfhash);
	if(lflist)
		return lflist->listName;
	return gDataManager->unknown_string;
}
int DeckManager::TypeCount(std::vector<CardDataC*> cards, uint32_t type) {
	int count = 0;
	for(auto card : cards) {
		if(card->type & type)
			count++;
	}
	return count;
}
inline DeckError CheckCards(const std::vector<CardDataC*> &cards, LFList* curlist, banlist_content_t* list,
					  DuelAllowedCards allowedCards,
					  banlist_content_t &ccount,
					  DeckError(*additionalCheck)(CardDataC*) = nullptr) {
	DeckError ret{ DeckError::NONE };
	for (const auto cit : cards) {
		ret.code = cit->code;
		switch (allowedCards) {
#define CHECK_UNOFFICIAL(cit) if (cit->ot > 0x3) return ret.type = DeckError::UNOFFICIALCARD, ret;
		case DuelAllowedCards::ALLOWED_CARDS_OCG_ONLY:
			CHECK_UNOFFICIAL(cit);
			if (!(cit->ot & 0x1))
				return ret.type = DeckError::TCGONLY, ret;
			break;
		case DuelAllowedCards::ALLOWED_CARDS_TCG_ONLY:
			CHECK_UNOFFICIAL(cit);
			if (!(cit->ot & 0x2))
				return ret.type = DeckError::OCGONLY, ret;
			break;
		case DuelAllowedCards::ALLOWED_CARDS_OCG_TCG:
			CHECK_UNOFFICIAL(cit);
			break;
#undef CHECK_UNOFFICIAL
		case DuelAllowedCards::ALLOWED_CARDS_WITH_PRERELEASE:
			if (cit->ot & 0x1 || cit->ot & 0x2 || cit->ot & 0x100)
				break;
			return ret.type = DeckError::UNOFFICIALCARD, ret;
		case DuelAllowedCards::ALLOWED_CARDS_ANY:
		default:
			break;
		}
		DeckError additional = additionalCheck ? additionalCheck(cit) : DeckError{ DeckError::NONE };
		if (additional.type) {
			return additional;
		}
		uint32_t code = cit->alias ? cit->alias : cit->code;
		ccount[code]++;
		int dc = ccount[code];
		if (dc > 3)
			return ret.type = DeckError::CARDCOUNT, ret;
		auto it = list->find(cit->code);
		if (it == list->end())
			it = list->find(code);
		if ((it != list->end() && dc > it->second) || (curlist->whitelist && it == list->end()))
			return ret.type = DeckError::LFLIST, ret;
	}
	return { DeckError::NONE };
}
DeckError DeckManager::CheckDeck(Deck& deck, uint32_t lfhash, DuelAllowedCards allowedCards, bool doubled, uint32_t forbiddentypes) {
	banlist_content_t ccount;
	LFList* curlist = nullptr;
	for(auto& list : _lfList) {
		if(list.hash == lfhash) {
			curlist = &list;
			break;
		}
	}
	DeckError ret{ DeckError::NONE };
	if(!curlist)
		return ret;
	auto list = &curlist->content;
	if(TypeCount(deck.main, forbiddentypes) > 0 || TypeCount(deck.extra, forbiddentypes) > 0 || TypeCount(deck.side, forbiddentypes) > 0)
		return ret.type = DeckError::FORBTYPE, ret;
	bool speed = mainGame->extra_rules & DECK_LIMIT_20;
	size_t minmain = 40, maxmain = 60, maxextra = 15, maxside = 15;
	if(doubled){
		if(speed){
			maxextra = 10;
			maxside = 12;
		} else {
			minmain = maxmain = 100;
			maxextra = 30;
			maxside = 30;
		}
	} else {
		if(speed){
			minmain = 20;
			maxmain = 30;
			maxextra = 5;
			maxside = 6;
		}
	}
	if(deck.main.size() < minmain || deck.main.size() > maxmain) {
		ret.type = DeckError::MAINCOUNT;
		ret.count.current = deck.main.size();
		ret.count.minimum = minmain;
		ret.count.maximum = maxmain;
	} else if(deck.extra.size() > maxextra) {
		ret.type = DeckError::EXTRACOUNT;
		ret.count.current = deck.extra.size();
		ret.count.minimum = 0;
		ret.count.maximum = maxextra;
	} else if(deck.side.size() > maxside) {
		ret.type = DeckError::SIDECOUNT;
		ret.count.current = deck.side.size();
		ret.count.minimum = 0;
		ret.count.maximum = maxside;
	}
	if(ret.type)
		return ret;
	ret = CheckCards(deck.main, curlist, list, allowedCards, ccount, [](CardDataC* cit)->DeckError {
		if ((cit->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)) || (cit->type & TYPE_LINK && cit->type & TYPE_MONSTER))
			return { DeckError::EXTRACOUNT };
		return { DeckError::NONE };
	});
	if (ret.type) return ret;
	ret = CheckCards(deck.extra, curlist, list, allowedCards , ccount, [](CardDataC* cit)->DeckError {
		if (!(cit->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ)) && !(cit->type & TYPE_LINK && cit->type & TYPE_MONSTER))
			return { DeckError::EXTRACOUNT };
		return { DeckError::NONE };
	});
	if (ret.type) return ret;
	return CheckCards(deck.side, curlist, list, allowedCards, ccount);
}
uint32_t DeckManager::LoadDeck(Deck& deck, uint32_t* dbuf, uint32_t mainc, uint32_t sidec, uint32_t mainc2, uint32_t sidec2) {
	cardlist_type mainvect(mainc + mainc2);
	cardlist_type sidevect(sidec + sidec2);
	auto copy = [&dbuf](uint32_t* vec, uint32_t count) {
		if(count > 0) {
			memcpy(vec, dbuf, count * sizeof(uint32_t));
			dbuf += count;
		}
	};
	copy(mainvect.data(), mainc);
	copy(sidevect.data(), sidec);
	copy(mainvect.data() + mainc, mainc2);
	copy(sidevect.data() + sidec, sidec2);
	return LoadDeck(deck, mainvect, sidevect);
}
uint32_t DeckManager::LoadDeck(Deck& deck, const cardlist_type& mainlist, const cardlist_type& sidelist, const cardlist_type* extralist) {
	deck.clear();
	uint32_t errorcode = 0;
	CardDataC* cd = nullptr;
	const bool loadalways = !!extralist;
	for(auto code : mainlist) {
		if(!(cd = gDataManager->GetCardData(code))) {
			if(!loadalways) {
				errorcode = code;
				continue;
			}
			cd = gdeckManager->GetDummyOrMappedCardData(code);
		}
		if(!cd || cd->type & TYPE_TOKEN)
			continue;
		else if(!extralist && (cd->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ) || (cd->type & TYPE_LINK && cd->type & TYPE_MONSTER))) {
			deck.extra.push_back(cd);
		} else {
			deck.main.push_back(cd);
		}
	}
	if(extralist) {
		for(auto code : *extralist) {
			if(!(cd = gDataManager->GetCardData(code))) {
				if(!loadalways) {
					errorcode = code;
					continue;
				}
				cd = gdeckManager->GetDummyOrMappedCardData(code);
			}
			if(!cd || cd->type & TYPE_TOKEN)
				continue;
			deck.extra.push_back(cd);
		}
	}
	for(auto code : sidelist) {
		if(!(cd = gDataManager->GetCardData(code))) {
			if(!loadalways) {
				errorcode = code;
				continue;
			}
			cd = gdeckManager->GetDummyOrMappedCardData(code);
		}
		if(!cd || cd->type & TYPE_TOKEN)
			continue;
		deck.side.push_back(cd);
	}
	return errorcode;
}
bool LoadCardList(const epro::path_string& name, cardlist_type* mainlist = nullptr, cardlist_type* extralist = nullptr, cardlist_type* sidelist = nullptr, uint32_t* retmainc = nullptr, uint32_t* retsidec = nullptr) {
	std::ifstream deck(name, std::ifstream::in);
	if(!deck.is_open())
		return false;
	cardlist_type res;
	std::string str;
	bool is_side = false;
	bool is_extra = false;
	uint32_t sidec = 0;
	while(std::getline(deck, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str.erase(pos);
		if(str.empty())
			continue;
		if(str[0] == '#') {
			if(!extralist || str != "#extra")
				continue;
			is_extra = true;
		}
		if(str[0] == '!') {
			is_side = true;
			continue;
		}
		if(str.find_first_of("0123456789") != std::string::npos) {
			uint32_t code = 0;
			try { code = static_cast<uint32_t>(std::stoul(str)); }
			catch (...) { continue; }
			res.push_back(code);
			if(is_side) {
				if(sidelist)
					sidelist->push_back(code);
				sidec++;
			} else {
				if(mainlist && !is_extra)
					mainlist->push_back(code);
				if(extralist && is_extra)
					extralist->push_back(code);
			}
		}
	}
	deck.close();
	if(retmainc)
		*retmainc = res.size() - sidec;
	if(retsidec)
		*retsidec = sidec;
	return true;
}
bool DeckManager::LoadSide(Deck& deck, uint32_t* dbuf, uint32_t mainc, uint32_t sidec) {
	std::map<uint32_t, int> pcount;
	std::map<uint32_t, int> ncount;
	for(auto& card: deck.main)
		pcount[card->code]++;
	for(auto& card : deck.extra)
		pcount[card->code]++;
	for(auto& card : deck.side)
		pcount[card->code]++;
	Deck ndeck;
	LoadDeck(ndeck, dbuf, mainc, sidec);
	if(ndeck.main.size() != deck.main.size() || ndeck.extra.size() != deck.extra.size())
		return false;
	for(auto& card : ndeck.main)
		ncount[card->code]++;
	for(auto& card : ndeck.extra)
		ncount[card->code]++;
	for(auto& card : ndeck.side)
		ncount[card->code]++;
	if(!std::equal(pcount.begin(), pcount.end(), ncount.begin()))
		return false;
	deck = ndeck;
	return true;
}
bool DeckManager::LoadDeck(epro::path_stringview file, Deck* deck, bool separated) {
	cardlist_type mainlist;
	cardlist_type sidelist;
	cardlist_type extralist;
	if(!LoadCardList(fmt::format(EPRO_TEXT("./deck/{}.ydk"), file), &mainlist, separated ? &extralist : nullptr, &sidelist)) {
		if(!LoadCardList({ file.data(), file.size() }, &mainlist, separated ? &extralist : nullptr, &sidelist))
			return false;
	}
	if(deck)
		LoadDeck(*deck, mainlist, sidelist, separated ? &extralist : nullptr);
	else
		LoadDeck(current_deck, mainlist, sidelist, separated ? &extralist : nullptr);
	return true;
}
bool DeckManager::LoadDeckDouble(epro::path_stringview file, epro::path_stringview file2, Deck* deck) {
	cardlist_type mainlist;
	cardlist_type sidelist;
	LoadCardList(fmt::format(EPRO_TEXT("./deck/{}.ydk"), file), &mainlist, nullptr, &sidelist);
	LoadCardList(fmt::format(EPRO_TEXT("./deck/{}.ydk"), file2), &mainlist, nullptr, &sidelist);
	if(deck)
		LoadDeck(*deck, mainlist, sidelist);
	else
		LoadDeck(current_deck, mainlist, sidelist);
	return true;
}
bool DeckManager::SaveDeck(Deck& deck, epro::path_stringview name) {
	std::ofstream deckfile(fmt::format(EPRO_TEXT("./deck/{}.ydk"), name), std::ofstream::out);
	if(!deckfile.is_open())
		return false;
	deckfile << "#created by " << BufferIO::EncodeUTF8(mainGame->ebNickName->getText()) << "\n#main\n";
	for(auto card : deck.main)
		deckfile << fmt::to_string(card->code) << "\n";
	deckfile << "#extra\n";
	for(auto card : deck.extra)
		deckfile << fmt::to_string(card->code) << "\n";
	deckfile << "!side\n";
	for(auto card : deck.side)
		deckfile << fmt::to_string(card->code) << "\n";
	deckfile.close();
	return true;
}
bool DeckManager::SaveDeck(epro::path_stringview name, const cardlist_type& mainlist, const cardlist_type& extralist, const cardlist_type& sidelist) {
	std::ofstream deckfile(fmt::format(EPRO_TEXT("./deck/{}.ydk"), name), std::ofstream::out);
	if(!deckfile.is_open())
		return false;
	deckfile << "#created by " << BufferIO::EncodeUTF8(mainGame->ebNickName->getText()) << "\n#main\n";
	for(auto card : mainlist)
		deckfile << fmt::to_string(card) << "\n";
	deckfile << "#extra\n";
	for(auto card : extralist)
		deckfile << fmt::to_string(card) << "\n";
	deckfile << "!side\n";
	for(auto card : sidelist)
		deckfile << fmt::to_string(card) << "\n";
	deckfile.close();
	return true;
}
const wchar_t* DeckManager::ExportDeckBase64(Deck& deck) {
	static std::wstring res;
	auto decktobuf = [](const auto& src) {
		static cardlist_type cards;
		cards.resize(src.size());
		for(size_t i = 0; i < src.size(); i++) {
			cards[i] = src[i]->code;
		}
		return base64_encode((uint8_t*)cards.data(), cards.size() * sizeof(cardlist_type::value_type));
	};
	res = fmt::format(L"ydke://{}!{}!{}!", decktobuf(deck.main), decktobuf(deck.extra), decktobuf(deck.side));
	return res.data();
}
const wchar_t* DeckManager::ExportDeckCardNames(Deck deck) {
	static std::wstring res;
	res.clear();
	std::sort(deck.main.begin(), deck.main.end(), DataManager::deck_sort_lv);
	std::sort(deck.extra.begin(), deck.extra.end(), DataManager::deck_sort_lv);
	std::sort(deck.side.begin(), deck.side.end(), DataManager::deck_sort_lv);
	auto serialize = [](const auto& list) {
		uint32_t prev = 0;
		uint32_t count = 0;
		for(const auto& card : list) {
			auto code = card->code;
			if(card->alias && abs((int)(card->alias - card->code)) < 10) {
				code = card->alias;
			}
			if(!prev) {
				prev = code;
				count = 1;
			} else if(prev && code != prev) {
				res.append(fmt::format(L"{} x{}\n", gDataManager->GetName(prev), count));
				count = 1;
				prev = code;
			} else {
				count++;
			}
		}
		if(prev)
			res.append(fmt::format(L"{} x{}\n", gDataManager->GetName(prev), count));
	};
	bool prev = false;
	if(deck.main.size()) {
		res.append(L"Main Deck:\n");
		serialize(deck.main);
		prev = true;
	}
	if(deck.extra.size()) {
		if(prev)
			res.append(L"\n\n");
		res.append(L"Extra Deck:\n");
		serialize(deck.extra);
		prev = true;
	}
	if(deck.side.size()) {
		if(prev)
			res.append(L"\n\n");
		res.append(L"Side Deck:\n");
		serialize(deck.side);
	}
	return res.data();
}
cardlist_type BufferToCardlist(const std::vector<uint8_t>& input) {
	cardlist_type vect(input.size() / 4);
	memcpy(vect.data(), input.data(), input.size());
	return vect;
}
void DeckManager::ImportDeckBase64(Deck& deck, const wchar_t* buffer) {
	buffer += (sizeof(L"ydke://") / sizeof(wchar_t)) - 1;
	size_t delimiters[3];
	int delim = 0;
	for(int i = 0; delim < 3 && buffer[i]; i++) {
		if(buffer[i] == L'!') {
			delimiters[delim++] = i;
		}
	}
	if(delim != 3)
		return;
	const auto mainlist = BufferToCardlist(base64_decode(buffer, delimiters[0]));
	const auto extralist = BufferToCardlist(base64_decode(buffer + delimiters[0] + 1, delimiters[1] - delimiters[0]));
	const auto sidelist = BufferToCardlist(base64_decode(buffer + delimiters[1] + 1, delimiters[2] - delimiters[1]));
	LoadDeck(deck, mainlist, sidelist, &extralist);
}
template<size_t N>
uint32_t gzinflate(const std::vector<uint8_t>& in, uint8_t(&buffer)[N]) {
	if(in.empty())
		return 0;
	z_stream z{};

	if(inflateInit2(&z, -MAX_WBITS) != Z_OK)
		return 0;

	z.next_in = (decltype(z.next_in))in.data();
	z.avail_in = in.size();

	z.next_out = buffer;
	z.avail_out = N;

	if(inflate(&z, Z_SYNC_FLUSH) < 0 || inflateEnd(&z) != Z_OK)
		return 0;

	return N - z.avail_out;
}

static constexpr size_t BufferSize(size_t mainc, size_t sidec) {
	return (2 * sizeof(uint8_t)) + ((mainc + sidec) * sizeof(uint32_t));
}

bool DeckManager::ImportDeckBase64Omega(Deck& deck, epro::wstringview buffer) {
	constexpr size_t max_main = 60 + 15;
	constexpr size_t max_side = 15;
	constexpr size_t max_size = BufferSize(max_main, max_side);
	uint8_t out_buf[max_size];
	const auto size = gzinflate(base64_decode(buffer, false, true), out_buf);
	if(size < 6) //counts and at least 1 card
		return false;
	const uint8_t mainc = out_buf[0];
	if(mainc > max_main)
		return false;
	const uint8_t sidec = out_buf[1];
	if(sidec > max_side)
		return false;
	if(size < BufferSize(mainc, sidec))
		return false;
	LoadDeck(deck, reinterpret_cast<uint32_t*>(out_buf + 2), mainc, sidec);
	return true;
}
bool DeckManager::DeleteDeck(Deck& deck, epro::path_stringview name) {
	return Utils::FileDelete(fmt::format(EPRO_TEXT("./deck/{}.ydk"), name));
}
bool DeckManager::RenameDeck(epro::path_stringview oldname, epro::path_stringview newname) {
	return Utils::FileMove(fmt::format(EPRO_TEXT("./deck/{}.ydk"), oldname), fmt::format(EPRO_TEXT("./deck/{}.ydk"), newname));
}
}
