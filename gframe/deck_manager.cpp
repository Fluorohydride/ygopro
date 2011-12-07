#include "deck_manager.h"
#include "data_manager.h"
#include "game.h"
#include <algorithm>

namespace ygo {

void DeckManager::LoadLFList() {
	LFList cur;
	std::unordered_map<int, int>* curMap = 0;
	FILE* fp = fopen("lflist.conf", "r");
	char linebuf[256];
	wchar_t strBuffer[256];
	if(fp) {
		fseek(fp, 0, SEEK_END);
		size_t fsize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fgets(linebuf, 256, fp);
		while(ftell(fp) < fsize) {
			fgets(linebuf, 256, fp);
			if(linebuf[0] == '#')
				continue;
			int p = 0, sa = 0, code, count;
			if(linebuf[0] == '!') {
				sa = DataManager::DecodeUTF8((const char*)(&linebuf[1]), strBuffer);
				while(strBuffer[sa - 1] == L'\r' || strBuffer[sa - 1] == L'\n' ) sa--;
				memcpy(cur.listName, (const void*)strBuffer, 40);
				cur.listName[sa] = 0;
				curMap = new std::unordered_map<int, int>;
				cur.content = curMap;
				_lfList.push_back(cur);
				continue;
			}
			while(linebuf[p] != ' ' && linebuf[p] != '\t' && linebuf[p] != 0) p++;
			if(linebuf[p] == 0)
				continue;
			linebuf[p++] = 0;
			sa = p;
			code = atoi(linebuf);
			if(code == 0)
				continue;
			while(linebuf[p] == ' ' || linebuf[p] == '\t') p++;
			while(linebuf[p] != ' ' && linebuf[p] != '\t' && linebuf[p] != 0) p++;
			linebuf[p] = 0;
			count = atoi(&linebuf[sa]);
			(*curMap)[code] = count;
		}
		fclose(fp);
	}
	myswprintf(cur.listName, L"无限制");
	cur.content = new std::unordered_map<int, int>;
	_lfList.push_back(cur);
}
bool DeckManager::CheckLFList(Deck& deck, int lfindex) {
	std::unordered_map<int, int> ccount;
	std::unordered_map<int, int>* list = _lfList[lfindex].content;
	int dc = 0, dec = 0;
	if(deck.main.size() < 40 || deck.main.size() > 60 || deck.extra.size() > 15 || deck.side.size() > 15)
		return false;
	for(int i = 0; i < deck.main.size(); ++i) {
		ccount[deck.main[i]->first]++;
		dc = ccount[deck.main[i]->first];
		auto it = list->find(deck.main[i]->first);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return false;
	}
	for(int i = 0; i < deck.extra.size(); ++i) {
		ccount[deck.extra[i]->first]++;
		dc = ccount[deck.extra[i]->first];
		auto it = list->find(deck.extra[i]->first);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return false;
	}
	for(int i = 0; i < deck.side.size(); ++i) {
		ccount[deck.side[i]->first]++;
		dc = ccount[deck.side[i]->first];
		auto it = list->find(deck.side[i]->first);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return false;
	}
	return true;
}
void DeckManager::LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec) {
	deck.main.clear();
	deck.extra.clear();
	deck.side.clear();
	int code;
	CardData cd;
	for(int i = 0; i < mainc; ++i) {
		code = dbuf[i];
		if(!mainGame->dataManager.GetData(code, &cd))
			continue;
		if(cd.type & TYPE_TOKEN)
			continue;
		else if(cd.type & 0x802040 && deck.extra.size() < 15) {
			deck.extra.push_back(mainGame->dataManager.GetCodePointer(code));
		} else if(deck.main.size() < 60) {
			deck.main.push_back(mainGame->dataManager.GetCodePointer(code));
		}
	}
	for(int i = 0; i < sidec; ++i) {
		code = dbuf[mainc + i];
		if(!mainGame->dataManager.GetData(code, &cd))
			continue;
		if(cd.type & TYPE_TOKEN)
			continue;
		if(deck.side.size() < 15)
			deck.side.push_back(mainGame->dataManager.GetCodePointer(code));
	}
}

bool DeckManager::LoadDeck(const wchar_t* file) {
	int sp = 0, ct = 0, mainc = 0, sidec = 0, code;
	wchar_t deck[64];
	myswprintf(deck, L"./deck/%ls.ydk", file);
	int cardlist[128];
	bool is_side = false;
#ifdef WIN32
	FILE* fp = _wfopen(deck, L"r");
#else
	char deckfn[256];
	DataManager::EncodeUTF8(deck, deckfn);
	FILE* fp = fopen(deckfn, "r");
#endif
	if(!fp)
		return false;
	char linebuf[256];
	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fgets(linebuf, 256, fp);
	while(ftell(fp) < fsize && ct < 128) {
		fgets(linebuf, 256, fp);
		if(linebuf[0] == '!') {
			is_side = true;
			continue;
		}
		if(linebuf[0] < '0' || linebuf[0] > '9')
			continue;
		sp = 0;
		while(linebuf[sp] >= '0' && linebuf[sp] <= '9') sp++;
		linebuf[sp] = 0;
		code = atoi(linebuf);
		cardlist[ct++] = code;
		if(is_side) sidec++;
		else mainc++;
	}
	fclose(fp);
	LoadDeck(deckhost, cardlist, mainc, sidec);
	return true;
}
void DeckManager::SaveDeck(Deck& deck, const wchar_t* name) {
	wchar_t file[64];
	myswprintf(file, L"./deck/%ls.ydk", name);
#ifdef WIN32
	FILE* fp = _wfopen(file, L"w");
#else
	char filefn[256];
	DataManager::EncodeUTF8(file, filefn);
	FILE* fp = fopen(filefn, "w");
#endif
	if(!fp)
		return;
	fprintf(fp, "#created by ...\n#main\n");
	for(int i = 0; i < deck.main.size(); ++i)
		fprintf(fp, "%d\n", deck.main[i]->first);
	fprintf(fp, "#extra\n");
	for(int i = 0; i < deck.extra.size(); ++i)
		fprintf(fp, "%d\n", deck.extra[i]->first);
	fprintf(fp, "!side\n");
	for(int i = 0; i < deck.side.size(); ++i)
		fprintf(fp, "%d\n", deck.side[i]->first);
	fclose(fp);
}
}
