#include "deck_manager.h"
#include "data_manager.h"
#include "network.h"
#include "game.h"
#include <algorithm>
#include <fstream>

namespace ygo {

DeckManager deckManager;

bool DeckManager::LoadLFListSingle(const path_string& path) {
	bool loaded = false;
	std::ifstream infile(path, std::ifstream::in);
	if(!infile.is_open())
		return loaded;
	LFList lflist;
	lflist.hash = 0;
	std::string str;
	while(std::getline(infile, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if(str.empty() || str[0] == '#')
			continue;
		if(str[0] == '!') {
			if(lflist.hash)
				_lfList.push_back(lflist);
			lflist.listName = BufferIO::DecodeUTF8s(str.substr(1));
			lflist.content.clear();
			lflist.hash = 0x7dfcee6a;
			lflist.whitelist = false;
			loaded = true;
			continue;
		}
		const std::string key("$whitelist");
		if(str.substr(0, key.size()) == key) {
			lflist.whitelist = true;
		}
		if(!lflist.hash)
			continue;
		pos = str.find(" ");
		if(pos == std::string::npos)
			continue;
		int code = std::stoi(str.substr(0, pos));
		if(!code)
			continue;
		str = str.substr(pos + 1);
		str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
		pos = str.find(" ");
		if(pos == std::string::npos)
			continue;
		int count = std::stoi(str.substr(0, pos));
		lflist.content[code] = count;
		lflist.hash = lflist.hash ^ ((code << 18) | (code >> 14)) ^ ((code << (27 + count)) | (code >> (5 - count)));
	}
	if(lflist.hash)
		_lfList.push_back(lflist);
	infile.close();
	return loaded;
}
bool DeckManager::LoadLFListFolder(path_string path) {
	path = Utils::NormalizePath(path);
	bool loaded = false;
	auto lflists = Utils::FindfolderFiles(path, std::vector<path_string>({ TEXT("conf") }));
	for (const auto& lflist : lflists) {
		loaded = LoadLFListSingle(path + lflist);
	}
	return loaded;
}
void DeckManager::LoadLFList() {
	LoadLFListSingle(TEXT("./expansions/lflist.conf"));
	LoadLFListSingle(TEXT("./lflist.conf"));
	LoadLFListFolder(TEXT("./lflists/"));
	LFList nolimit;
	nolimit.listName = L"N/A";
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
	}
}
std::wstring DeckManager::GetLFListName(int lfhash) {
	auto it = std::find_if(_lfList.begin(), _lfList.end(), [lfhash](LFList list){return list.hash == (unsigned int)lfhash; });
	if(it != _lfList.end())
		return (*it).listName.c_str();
	return dataManager.unknown_string;
}
int DeckManager::TypeCount(std::vector<CardDataC*> cards, int type) {
	int count = 0;
	for(auto card : cards) {
		if(card->type & type)
			count++;
	}
	return count;
}
inline int CheckCards(const std::vector<CardDataC *> &cards, LFList* curlist, std::unordered_map<int, int>* list,
					  bool allow_ocg, bool allow_tcg, bool allow_prerelease,
					  std::unordered_map<int, int> &ccount,
					  std::function<int(CardDataC*)> additionalCheck = [](CardDataC*){ return 0; }) {
	for (const auto cit : cards) {
		if (!allow_ocg && (cit->ot == 0x1))
			return (DECKERROR_OCGONLY << 28) + cit->code;
		if (!allow_tcg && (cit->ot == 0x2))
			return (DECKERROR_TCGONLY << 28) + cit->code;
		int additional = additionalCheck(cit);
		if (additional) {
			return additional;
		}
		int code = cit->alias ? cit->alias : cit->code;
		ccount[code]++;
		int dc = ccount[code];
		if (dc > 3)
			return (DECKERROR_CARDCOUNT << 28) + cit->code;
		auto it = list->find(cit->code);
		if (it == list->end())
			it = list->find(code);
		if ((it != list->end() && dc > it->second) || (curlist->whitelist && it == list->end()))
			return (DECKERROR_LFLIST << 28) + cit->code;
	}
	return 0;
}
int DeckManager::CheckDeck(Deck& deck, int lfhash, bool allow_ocg, bool allow_tcg, bool doubled, int forbiddentypes) {
	std::unordered_map<int, int> ccount;
	LFList* curlist = nullptr;
	for(auto& list : _lfList) {
		if(list.hash == (unsigned int)lfhash) {
			curlist = &list;
			break;
		}
	}
	if(!curlist)
		return 0;
	auto list = &curlist->content;
	if(TypeCount(deck.main, forbiddentypes) > 0 || TypeCount(deck.extra, forbiddentypes) > 0 || TypeCount(deck.side, forbiddentypes) > 0)
		return (DECKERROR_FORBTYPE << 28);
	bool speed = mainGame->dInfo.extraval & 0x1;
	if(doubled){
		if(speed){
			if(deck.main.size() < 40 || deck.main.size() > 60)
				return (DECKERROR_MAINCOUNT << 28) + deck.main.size();
			if(deck.extra.size() > 10)
				return (DECKERROR_EXTRACOUNT << 28) + deck.extra.size();
			if(deck.side.size())
				return (DECKERROR_SIDECOUNT << 28) + deck.side.size();
		} else {
			if(deck.main.size() != 100)
				return (DECKERROR_MAINCOUNT << 28) + deck.main.size();
			if(deck.extra.size() > 30)
				return (DECKERROR_EXTRACOUNT << 28) + deck.extra.size();
			if(deck.side.size() > 30)
				return (DECKERROR_SIDECOUNT << 28) + deck.side.size();
		}
	} else {
		if(speed){
			if(deck.main.size() < 20 || deck.main.size() > 30)
				return (DECKERROR_MAINCOUNT << 28) + deck.main.size();
			if(deck.extra.size() > 5)
				return (DECKERROR_EXTRACOUNT << 28) + deck.extra.size();
			if(deck.side.size())
				return (DECKERROR_SIDECOUNT << 28) + deck.side.size();
		} else {
			if(deck.main.size() < 40 || deck.main.size() > 60)
				return (DECKERROR_MAINCOUNT << 28) + deck.main.size();
			if(deck.extra.size() > 15)
				return (DECKERROR_EXTRACOUNT << 28) + deck.extra.size();
			if(deck.side.size() > 15)
				return (DECKERROR_SIDECOUNT << 28) + deck.side.size();
		}
	}
	int currentCheck = CheckCards(deck.main, curlist, list, allow_ocg, allow_tcg, true, ccount, [](CardDataC* cit) {
		if ((cit->type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_TOKEN)) || (cit->type & TYPE_LINK && cit->type & TYPE_MONSTER))
			return (DECKERROR_EXTRACOUNT << 28);
		return 0;
	});
	if (currentCheck) return currentCheck;
	currentCheck = CheckCards(deck.extra, curlist, list, allow_ocg, allow_tcg, true, ccount);
	if (currentCheck) return currentCheck;
	return CheckCards(deck.side, curlist, list, allow_ocg, allow_tcg, true, ccount);
}
int DeckManager::LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, int mainc2, int sidec2) {
	std::vector<int> mainvect;
	std::vector<int> sidevect;
	mainvect.insert(mainvect.end(), dbuf, dbuf + mainc);
	dbuf += mainc;
	sidevect.insert(sidevect.end(), dbuf, dbuf + sidec);
	dbuf += sidec;
	mainvect.insert(mainvect.end(), dbuf, dbuf + mainc2);
	dbuf += mainc2;
	sidevect.insert(sidevect.end(), dbuf, dbuf + sidec2);
	return LoadDeck(deck, mainvect, sidevect);
}
int DeckManager::LoadDeck(Deck& deck, std::vector<int> mainlist, std::vector<int> sidelist) {
	deck.clear();
	int errorcode = 0;
	CardData cd;
	for(auto code : mainlist) {
		if(!dataManager.GetData(code, &cd)) {
			errorcode = code;
			continue;
		}
		if(cd.type & TYPE_TOKEN)
			continue;
		else if((cd.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ) || (cd.type & TYPE_LINK && cd.type & TYPE_MONSTER))) {
			deck.extra.push_back(dataManager.GetCardData(code));
		} else {
			deck.main.push_back(dataManager.GetCardData(code));
		}
	}
	for(auto code : sidelist) {
		if(!dataManager.GetData(code, &cd)) {
			errorcode = code;
			continue;
		}
		if(cd.type & TYPE_TOKEN)
			continue;
		deck.side.push_back(dataManager.GetCardData(code));	//verified by GetData()
	}
	return errorcode;
}
bool LoadCardList(const path_string& name, std::vector<int>* mainlist = nullptr, std::vector<int>* sidelist = nullptr, int* retmainc = nullptr, int* retsidec = nullptr) {
	std::ifstream deck(name, std::ifstream::in);
	if(!deck.is_open())
		return false;
	std::vector<int> res;
	std::string str;
	bool is_side = false;
	int sidec = 0;
	while(std::getline(deck, str)) {
		auto pos = str.find_first_of("\n\r");
		if(str.size() && pos != std::string::npos)
			str = str.substr(0, pos);
		if(str.empty() || str[0] == '#')
			continue;
		if(str[0] == '!') {
			is_side = true;
			continue;
		}
		if(str.find_first_of("0123456789") != std::string::npos) {
			int code = 0;
			try {
				code = std::stoi(str);
			} catch (...){
				continue;
			}
			res.push_back(code);
			if(is_side) {
				if(sidelist)
					sidelist->push_back(code);
				sidec++;
			} else {
				if(mainlist)
					mainlist->push_back(code);
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
bool DeckManager::LoadSide(Deck& deck, int* dbuf, int mainc, int sidec) {
	std::map<int, int> pcount;
	std::map<int, int> ncount;
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
bool DeckManager::LoadDeck(const path_string& file, Deck* deck) {
	std::vector<int> mainlist;
	std::vector<int> sidelist;
	if(!LoadCardList(fmt::format(TEXT("./deck/{}.ydk"), file.c_str()), &mainlist, &sidelist)) {
		if(!LoadCardList(file, &mainlist, &sidelist))
			return false;
	}
	if(deck)
		LoadDeck(*deck, mainlist, sidelist);
	else
		LoadDeck(current_deck, mainlist, sidelist);
	return true;
}
bool DeckManager::LoadDeckDouble(const path_string& file, const path_string& file2, Deck* deck) {
	std::vector<int> mainlist;
	std::vector<int> sidelist;
	LoadCardList(fmt::format(TEXT("./deck/{}.ydk"), file.c_str()), &mainlist, &sidelist);
	LoadCardList(fmt::format(TEXT("./deck/{}.ydk"), file2.c_str()), &mainlist, &sidelist);
	if(deck)
		LoadDeck(*deck, mainlist, sidelist);
	else
		LoadDeck(current_deck, mainlist, sidelist);
	return true;
}
bool DeckManager::SaveDeck(Deck& deck, const path_string& name) {
	std::ofstream deckfile(fmt::format(TEXT("./deck/{}.ydk"), name.c_str()), std::ofstream::out);
	if(!deckfile.is_open())
		return false;
	deckfile << "#created by " << BufferIO::EncodeUTF8s(mainGame->ebNickName->getText()) << "\n#main\n";
	for(auto card : deck.main)
		deckfile << std::to_string(card->code) << "\n";
	deckfile << "#extra\n";
	for(auto card : deck.extra)
		deckfile << std::to_string(card->code) << "\n";
	deckfile << "!side\n";
	for(auto card : deck.side)
		deckfile << std::to_string(card->code) << "\n";
	deckfile.close();
	return true;
}
bool DeckManager::SaveDeck(const path_string& name, std::vector<int> mainlist, std::vector<int> extralist, std::vector<int> sidelist) {
	std::ofstream deckfile(fmt::format(TEXT("./deck/{}.ydk"), name.c_str()), std::ofstream::out);
	if(!deckfile.is_open())
		return false;
	deckfile << "#created by " << BufferIO::EncodeUTF8s(mainGame->ebNickName->getText()) << "\n#main\n";
	for(auto card : mainlist)
		deckfile << std::to_string(card) << "\n";
	deckfile << "#extra\n";
	for(auto card : extralist)
		deckfile << std::to_string(card) << "\n";
	deckfile << "!side\n";
	for(auto card : sidelist)
		deckfile << std::to_string(card) << "\n";
	deckfile.close();
	return true;
}
bool DeckManager::DeleteDeck(Deck& deck, const path_string& name) {
	return Utils::Deletefile(fmt::format(TEXT("./deck/{}.ydk"), name.c_str()));
}
}
