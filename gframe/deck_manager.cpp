#include "deck_manager.h"
#include "data_manager.h"
#include "network.h"
#include "game.h"

namespace ygo {

char DeckManager::deckBuffer[0x10000]{};
DeckManager deckManager;

void DeckManager::LoadLFListSingle(const char* path) {
	LFList* cur = nullptr;
	FILE* fp = fopen(path, "r");
	char linebuf[256]{};
	wchar_t strBuffer[256]{};
	if(fp) {
		while(fgets(linebuf, 256, fp)) {
			if(linebuf[0] == '#')
				continue;
			if(linebuf[0] == '!') {
				int sa = BufferIO::DecodeUTF8(&linebuf[1], strBuffer);
				while(strBuffer[sa - 1] == L'\r' || strBuffer[sa - 1] == L'\n' )
					sa--;
				strBuffer[sa] = 0;
				LFList newlist;
				_lfList.push_back(newlist);
				cur = &_lfList[_lfList.size() - 1];
				cur->listName = strBuffer;
				cur->hash = 0x7dfcee6a;
				continue;
			}
			if(linebuf[0] == 0)
				continue;
			int code = 0;
			int count = -1;
			if (sscanf(linebuf, "%d %d", &code, &count) != 2)
				continue;
			if (code <= 0 || code > 0xfffffff)
				continue;
			if (count < 0 || count > 2)
				continue;
			if (!cur)
				continue;
			cur->content[code] = count;
			cur->hash = cur->hash ^ ((code << 18) | (code >> 14)) ^ ((code << (27 + count)) | (code >> (5 - count)));
		}
		fclose(fp);
	}
}
void DeckManager::LoadLFList() {
	LoadLFListSingle("expansions/lflist.conf");
	LoadLFListSingle("lflist.conf");
	LFList nolimit;
	nolimit.listName = L"N/A";
	nolimit.hash = 0;
	_lfList.push_back(nolimit);
}
const wchar_t* DeckManager::GetLFListName(int lfhash) {
	auto lit = std::find_if(_lfList.begin(), _lfList.end(), [lfhash](const ygo::LFList& list) {
		return list.hash == lfhash;
	});
	if(lit != _lfList.end())
		return lit->listName.c_str();
	return dataManager.unknown_string;
}
const std::unordered_map<int, int>* DeckManager::GetLFListContent(int lfhash) {
	auto lit = std::find_if(_lfList.begin(), _lfList.end(), [lfhash](const ygo::LFList& list) {
		return list.hash == lfhash;
	});
	if(lit != _lfList.end())
		return &lit->content;
	return nullptr;
}
static int checkAvail(int ot, int avail) {
	if((ot & avail) == avail)
		return 0;
	if((ot & AVAIL_OCG) && !(avail == AVAIL_OCG))
		return DECKERROR_OCGONLY;
	if((ot & AVAIL_TCG) && !(avail == AVAIL_TCG))
		return DECKERROR_TCGONLY;
	return DECKERROR_NOTAVAIL;
}
int DeckManager::CheckDeck(Deck& deck, int lfhash, int rule) {
	std::unordered_map<int, int> ccount;
	auto list = GetLFListContent(lfhash);
	if(!list)
		return 0;
	int dc = 0;
	if(deck.main.size() < DECK_MIN_SIZE || deck.main.size() > DECK_MAX_SIZE)
		return (DECKERROR_MAINCOUNT << 28) + deck.main.size();
	if(deck.extra.size() > EXTRA_MAX_SIZE)
		return (DECKERROR_EXTRACOUNT << 28) + deck.extra.size();
	if(deck.side.size() > SIDE_MAX_SIZE)
		return (DECKERROR_SIDECOUNT << 28) + deck.side.size();
	const int rule_map[6] = { AVAIL_OCG, AVAIL_TCG, AVAIL_SC, AVAIL_CUSTOM, AVAIL_OCGTCG, 0 };
	int avail = rule_map[rule];
	for(size_t i = 0; i < deck.main.size(); ++i) {
		code_pointer cit = deck.main[i];
		int gameruleDeckError = checkAvail(cit->second.ot, avail);
		if(gameruleDeckError)
			return (gameruleDeckError << 28) + cit->first;
		if(cit->second.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_TOKEN | TYPE_LINK))
			return (DECKERROR_EXTRACOUNT << 28);
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		if(dc > 3)
			return (DECKERROR_CARDCOUNT << 28) + cit->first;
		auto it = list->find(code);
		if(it != list->end() && dc > it->second)
			return (DECKERROR_LFLIST << 28) + cit->first;
	}
	for(size_t i = 0; i < deck.extra.size(); ++i) {
		code_pointer cit = deck.extra[i];
		int gameruleDeckError = checkAvail(cit->second.ot, avail);
		if(gameruleDeckError)
			return (gameruleDeckError << 28) + cit->first;
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		if(dc > 3)
			return (DECKERROR_CARDCOUNT << 28) + cit->first;
		auto it = list->find(code);
		if(it != list->end() && dc > it->second)
			return (DECKERROR_LFLIST << 28) + cit->first;
	}
	for(size_t i = 0; i < deck.side.size(); ++i) {
		code_pointer cit = deck.side[i];
		int gameruleDeckError = checkAvail(cit->second.ot, avail);
		if(gameruleDeckError)
			return (gameruleDeckError << 28) + cit->first;
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		if(dc > 3)
			return (DECKERROR_CARDCOUNT << 28) + cit->first;
		auto it = list->find(code);
		if(it != list->end() && dc > it->second)
			return (DECKERROR_LFLIST << 28) + cit->first;
	}
	return 0;
}
int DeckManager::LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec, bool is_packlist) {
	deck.clear();
	int code;
	int errorcode = 0;
	CardData cd;
	for(int i = 0; i < mainc; ++i) {
		code = dbuf[i];
		if(!dataManager.GetData(code, &cd)) {
			errorcode = code;
			continue;
		}
		if(cd.type & TYPE_TOKEN)
			continue;
		if(is_packlist) {
			deck.main.push_back(dataManager.GetCodePointer(code));
			continue;
		}
		if (cd.type & TYPES_EXTRA_DECK) {
			if ((int)deck.extra.size() < EXTRA_MAX_SIZE)
				deck.extra.push_back(dataManager.GetCodePointer(code));
		}
		else {
			if ((int)deck.main.size() < DECK_MAX_SIZE)
				deck.main.push_back(dataManager.GetCodePointer(code));
		}
	}
	for(int i = 0; i < sidec; ++i) {
		code = dbuf[mainc + i];
		if(!dataManager.GetData(code, &cd)) {
			errorcode = code;
			continue;
		}
		if(cd.type & TYPE_TOKEN)
			continue;
		if(deck.side.size() < SIDE_MAX_SIZE)
			deck.side.push_back(dataManager.GetCodePointer(code));
	}
	return errorcode;
}
bool DeckManager::LoadSide(Deck& deck, int* dbuf, int mainc, int sidec) {
	std::unordered_map<int, int> pcount;
	std::unordered_map<int, int> ncount;
	for(size_t i = 0; i < deck.main.size(); ++i)
		pcount[deck.main[i]->first]++;
	for(size_t i = 0; i < deck.extra.size(); ++i)
		pcount[deck.extra[i]->first]++;
	for(size_t i = 0; i < deck.side.size(); ++i)
		pcount[deck.side[i]->first]++;
	Deck ndeck;
	LoadDeck(ndeck, dbuf, mainc, sidec);
	if(ndeck.main.size() != deck.main.size() || ndeck.extra.size() != deck.extra.size())
		return false;
	for(size_t i = 0; i < ndeck.main.size(); ++i)
		ncount[ndeck.main[i]->first]++;
	for(size_t i = 0; i < ndeck.extra.size(); ++i)
		ncount[ndeck.extra[i]->first]++;
	for(size_t i = 0; i < ndeck.side.size(); ++i)
		ncount[ndeck.side[i]->first]++;
	for(auto cdit = ncount.begin(); cdit != ncount.end(); ++cdit)
		if(cdit->second != pcount[cdit->first])
			return false;
	deck = ndeck;
	return true;
}
void DeckManager::GetCategoryPath(wchar_t* ret, int index, const wchar_t* text) {
	wchar_t catepath[256];
	switch(index) {
	case 0:
		myswprintf(catepath, L"./pack");
		break;
	case 1:
		myswprintf(catepath, mainGame->gameConf.bot_deck_path);
		break;
	case -1:
	case 2:
	case 3:
		myswprintf(catepath, L"./deck");
		break;
	default:
		myswprintf(catepath, L"./deck/%ls", text);
	}
	BufferIO::CopyWStr(catepath, ret, 256);
}
void DeckManager::GetDeckFile(wchar_t* ret, irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck) {
	wchar_t filepath[256];
	wchar_t catepath[256];
	const wchar_t* deckname = cbDeck->getItem(cbDeck->getSelected());
	if(deckname != NULL) {
		GetCategoryPath(catepath, cbCategory->getSelected(), cbCategory->getText());
		myswprintf(filepath, L"%ls/%ls.ydk", catepath, deckname);
		BufferIO::CopyWStr(filepath, ret, 256);
	}
	else {
		BufferIO::CopyWStr(L"", ret, 256);
	}
}
bool DeckManager::LoadDeck(irr::gui::IGUIComboBox* cbCategory, irr::gui::IGUIComboBox* cbDeck) {
	wchar_t filepath[256];
	GetDeckFile(filepath, cbCategory, cbDeck);
	bool is_packlist = cbCategory->getSelected() == 0;
	bool res = LoadDeck(filepath, is_packlist);
	if(res && mainGame->is_building)
		mainGame->deckBuilder.RefreshPackListScroll();
	if (!res)
		current_deck.clear();
	return res;
}
FILE* DeckManager::OpenDeckFile(const wchar_t* file, const char* mode) {
#ifdef WIN32
	wchar_t wmode[20]{};
	BufferIO::CopyWStr(mode, wmode, sizeof(wmode) / sizeof(wchar_t));
	FILE* fp = _wfopen(file, wmode);
#else
	char file2[256];
	BufferIO::EncodeUTF8(file, file2);
	FILE* fp = fopen(file2, mode);
#endif
	return fp;
}
IReadFile* DeckManager::OpenDeckReader(const wchar_t* file) {
#ifdef WIN32
	IReadFile* reader = dataManager.FileSystem->createAndOpenFile(file);
#else
	char file2[256];
	BufferIO::EncodeUTF8(file, file2);
	IReadFile* reader = dataManager.FileSystem->createAndOpenFile(file2);
#endif
	return reader;
}
bool DeckManager::LoadDeck(const wchar_t* file, bool is_packlist) {
	IReadFile* reader = OpenDeckReader(file);
	if(!reader) {
		wchar_t localfile[64];
		myswprintf(localfile, L"./deck/%ls.ydk", file);
		reader = OpenDeckReader(localfile);
	}
	if(!reader && !mywcsncasecmp(file, L"./pack", 6)) {
		wchar_t zipfile[64];
		myswprintf(zipfile, L"%ls", file + 2);
		reader = OpenDeckReader(zipfile);
	}
	if(!reader)
		return false;
	auto size = reader->getSize();
	if(size >= (int)sizeof deckBuffer) {
		reader->drop();
		return false;
	}
	memset(deckBuffer, 0, sizeof(deckBuffer));
	reader->read(deckBuffer, size);
	reader->drop();
	std::istringstream deckStream(deckBuffer);
	return LoadDeck(current_deck, deckStream, is_packlist);
}
bool DeckManager::LoadDeck(Deck& deck, std::istringstream& deckStream, bool is_packlist) {
	int sp = 0, ct = 0, mainc = 0, sidec = 0, code;
	int cardlist[300]{};
	bool is_side = false;
	std::string linebuf;
	while(std::getline(deckStream, linebuf, '\n') && ct < 300) {
		if(linebuf[0] == '!') {
			is_side = true;
			continue;
		}
		if(linebuf[0] < '0' || linebuf[0] > '9')
			continue;
		sp = 0;
		while(linebuf[sp] >= '0' && linebuf[sp] <= '9')
			sp++;
		linebuf[sp] = 0;
		code = std::stoi(linebuf);
		cardlist[ct++] = code;
		if(is_side)
			sidec++;
		else
			mainc++;
	}
	LoadDeck(current_deck, cardlist, mainc, sidec, is_packlist);
	return true; // the above LoadDeck has return value but we ignore it here for now
}
bool DeckManager::SaveDeck(Deck& deck, const wchar_t* file) {
	if(!FileSystem::IsDirExists(L"./deck") && !FileSystem::MakeDir(L"./deck"))
		return false;
	FILE* fp = OpenDeckFile(file, "w");
	if(!fp)
		return false;
	fprintf(fp, "#created by ...\n#main\n");
	for(size_t i = 0; i < deck.main.size(); ++i)
		fprintf(fp, "%d\n", deck.main[i]->first);
	fprintf(fp, "#extra\n");
	for(size_t i = 0; i < deck.extra.size(); ++i)
		fprintf(fp, "%d\n", deck.extra[i]->first);
	fprintf(fp, "!side\n");
	for(size_t i = 0; i < deck.side.size(); ++i)
		fprintf(fp, "%d\n", deck.side[i]->first);
	fclose(fp);
	return true;
}
bool DeckManager::DeleteDeck(const wchar_t* file) {
#ifdef WIN32
	BOOL result = DeleteFileW(file);
	return !!result;
#else
	char filefn[256];
	BufferIO::EncodeUTF8(file, filefn);
	int result = unlink(filefn);
	return result == 0;
#endif
}
bool DeckManager::CreateCategory(const wchar_t* name) {
	if(!FileSystem::IsDirExists(L"./deck") && !FileSystem::MakeDir(L"./deck"))
		return false;
	if(name[0] == 0)
		return false;
	wchar_t localname[256];
	myswprintf(localname, L"./deck/%ls", name);
	return FileSystem::MakeDir(localname);
}
bool DeckManager::RenameCategory(const wchar_t* oldname, const wchar_t* newname) {
	if(!FileSystem::IsDirExists(L"./deck") && !FileSystem::MakeDir(L"./deck"))
		return false;
	if(newname[0] == 0)
		return false;
	wchar_t oldlocalname[256];
	wchar_t newlocalname[256];
	myswprintf(oldlocalname, L"./deck/%ls", oldname);
	myswprintf(newlocalname, L"./deck/%ls", newname);
	return FileSystem::Rename(oldlocalname, newlocalname);
}
bool DeckManager::DeleteCategory(const wchar_t* name) {
	wchar_t localname[256];
	myswprintf(localname, L"./deck/%ls", name);
	if(!FileSystem::IsDirExists(localname))
		return false;
	return FileSystem::DeleteDir(localname);
}
}
