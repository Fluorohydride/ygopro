#include "server_lobby.h"
#include "data_manager.h"
#include "game.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace ygo {

struct MemoryStruct {
	char *memory;
	size_t size;
};
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

std::wstring hex_to_wstring(std::string s) {
	std::wstring result;
	if(s.length() % 4 == 0) {
		for(int i = 0; i < s.length(); i+=4) {
			std::string s2 = s.substr(i,4);
			wchar_t resultC =  (wchar_t)strtoul(s2.c_str(),NULL,16);
			result += resultC;
		}
	}
	return result;
}

void RoomInfoFromNameSet(const wchar_t* roomName, std::wstring* room, std::wstring* rule, std::wstring* duel_rule, std::wstring* mode, std::wstring* players, wchar_t* duel_rule_val, wchar_t* nocheckdeck_val,wchar_t* noshuffle_val,int* life,int* hand,int* draw) {
	wchar_t ruleC='0';
	wchar_t typeC='0';
	wchar_t bufName[100];
	int result = swscanf(roomName, L"%c%c%c%c%c%i,%i,%i,%ls",&ruleC,&typeC,duel_rule_val,nocheckdeck_val,noshuffle_val,life,hand,draw,bufName);

	if (duel_rule_val[0] == '1') {
		*duel_rule = L"MR1";
	} else if (duel_rule_val[0] == '2') {
		*duel_rule = L"MR2";
	} else if (duel_rule_val[0] == '3') {
		*duel_rule = L"MR3";
	} else if (duel_rule_val[0] == '4') {
		*duel_rule = L"MR4";
	}

	if(result >= 8) {
		if(ruleC=='0') {
			*rule = L"[OCG]";
		} else if(ruleC=='1') {
			*rule = L"[TCG]";
		} else if(ruleC=='2') {
			*rule = L"[TCG/OCG]";
		} else if(ruleC=='3') {
			*rule = L"[None]";
		} else if(ruleC=='4') {
			*rule = L"[Anime]";
		} else if(ruleC=='5') {
			*rule = L"[Turbo]";
		} else {
			*rule = L"[?]";
		}
		if(typeC=='0') {
			*mode = L"[Single]";
		} else if(typeC=='1') {
			*mode = L"[Match]";
		} else if(typeC=='2') {
			*mode = L"[Tag]";
		} else if(typeC=='3') {
			*mode = L"[1vs2]";
		} else {
			*mode = L"[?]";
		}
		if(result == 9) {
			*room = bufName;
		} else {
			*room = roomName;
		}
	} else if(wcslen(roomName) >= 3) {
		if(roomName[1] == '#') {
			if(roomName[0] == 'T') {
				//tag
				*rule = L"[OCG]";
				*mode = L"[Tag]";
				*room = roomName;
			} else if(roomName[0] == 'M') {
				//match
				*rule = L"[OCG]";
				*mode = L"[Match]";
				*room = roomName;
			} else {
				*rule = L"[OCG]";
				*mode = L"[Single]";
				*room = roomName;
			}
		} else if(roomName[2] == '#' && roomName[1] == 'M' && roomName[0] == 'P') {
			//match
			*rule = L"[OCG]";
			*mode = L"[Match]";
			*room = roomName;
		} else {
			*rule = L"[OCG]";
			*mode = L"[Single]";
			*room = roomName;
		}
	} else if(wcslen(roomName) >= 2) {
		if(roomName[1] == '#') {
			if(roomName[0] == 'T') {
				//tag
				*rule = L"[OCG]";
				*mode = L"[Tag]";
				*room = roomName;
			} else if(roomName[0] == 'M') {
				//match
				*rule = L"[OCG]";
				*mode = L"[Match]";
				*room = roomName;
			} else {
				*rule = L"[OCG]";
				*mode = L"[Single]";
				*room = roomName;
			}
		} else {
			*rule = L"[OCG]";
			*mode = L"[Single]";
			*room = roomName;
		}
	} else {
		*rule = L"[OCG]";
		*mode = L"[Single]";
		*room = roomName;
	}	
}

void ServerLobby::FillOnlineRooms() {
	ServerInfo s = mainGame->serversVector[mainGame->serverChoice->getSelected()];
	std::vector<RoomInfo> rv = mainGame->roomsVector;

	wchar_t searchText[20];
	BufferIO::CopyWStr(mainGame->ebRoomName->getText(), searchText, 20);	
	int searchRules = mainGame->cbFilterRule->getSelected();
	int searchMatchMode = mainGame->cbFilterMatchMode->getSelected();
	int searchBanlist = mainGame->cbFilterBanlist->getSelected();

	bool doFilter = wcslen(searchText) > 0 || searchRules > 0 || searchMatchMode > 0 || searchBanlist > 0;

	SColor normal = SColor(255, 0, 0, 0);
	SColor red = SColor(255, 255, 100, 100);
	SColor color = normal;
	bool show_password_checked = mainGame->chkShowPassword->isChecked();

	for(u32 z=0;z < rv.size(); z++) {
		if(rv[z].passwordNeeded && !show_password_checked) {
			continue;
		}

		std::wstring c_banlist;
		std::wstring c_room;
		std::wstring c_rule;
		std::wstring c_duel_rule; //master rule string
		std::wstring c_mode;
		std::wstring c_players;
		wchar_t duel_rule = 48; //master rule
		wchar_t nocheckdeck = 48;
		wchar_t noshuffle = 48;
		int life, hand, draw;		
		RoomInfoFromNameSet(rv[z].roomname.c_str(), &c_room, &c_rule, &c_duel_rule, &c_mode, &c_players, &duel_rule, &nocheckdeck, &noshuffle, &life, &hand, &draw);

		if(doFilter) {
			bool foundPlayer = false;
			bool foundDesc = false;

			if(wcslen(searchText) > 0) {
				wchar_t playerSearch[20];
				BufferIO::CopyWStr(rv[z].players[0].c_str(), playerSearch, 20);
#ifdef WIN32
				if(wcsstr(wcslwr(playerSearch), wcslwr(searchText))) {
#else
				make_string_lower(playerSearch);
				make_string_lower(searchText);
				if(wcsstr(playerSearch, searchText)) {
#endif
					foundPlayer = true;
				}				
				wchar_t descSearch[100];
				BufferIO::CopyWStr(rv[z].roomdescription.c_str(), descSearch, 100);
				if(wcslen(descSearch) > 0) {
#ifdef WIN32
					if(wcsstr(wcslwr(descSearch), wcslwr(searchText))) {					
#else
					make_string_lower(descSearch);
					if(wcsstr(descSearch, searchText)) {
#endif					
						foundDesc = true;
					}
				}

				if(foundPlayer == false && foundDesc == false) {
					continue;
				}
			}
			if(searchMatchMode > 0) {
				if(searchMatchMode == 1 && c_mode.compare(L"[Single]")) {
					continue;
				} else if(searchMatchMode == 2 && c_mode.compare(L"[Match]")) {
					continue;
				} else if(searchMatchMode == 3 && c_mode.compare(L"[Tag]")) {
					continue;
				} else if(searchMatchMode == 4 && c_mode.compare(L"[1vs2]")) {
					continue;
				}
			}
			if(searchRules > 0) {
				if(searchRules == 1 && c_rule.compare(L"[OCG]")) {
					continue;
				} else if(searchRules == 2 && c_rule.compare(L"[TCG]")) {
					continue;
				} else if(searchRules == 3 && c_rule.compare(L"[TCG/OCG]")) {
					continue;
				} else if(searchRules == 4 && c_rule.compare(L"[None]")) {
					continue;
				} else if(searchRules == 5 && c_rule.compare(L"[Anime]")) {
					continue;
				} else if(searchRules == 6 && c_rule.compare(L"[Turbo]")) {
					continue;
				}
			}
			if(searchBanlist > 0) {
				int searchingForBanlist = searchBanlist - 1;
				if(rv[z].banlist != searchingForBanlist) {
					continue;
				}
			}
		}
		
		int playerCount = 0;
		for(int y=0; y<=3; y++) {
			int playerlen = wcslen(rv[z].players[y].c_str());
			if(playerlen > 0) {
				if(playerCount > 0) {
					c_players.append(L", ");
				}
				c_players.append(rv[z].players[y]);
				playerCount++;
			}
		}
			
		unsigned short banlist = rv[z].banlist;
		if(banlist < deckManager._lfList.size()) {
			c_banlist = fmt::format(L"[{}]", deckManager._lfList[banlist].listName);
		} else {
			c_banlist = fmt::format(L"[{}]", deckManager._lfList[banlist].listName);
		}

		irr::gui::IGUITable* roomListTable = mainGame->roomListTable;
		int index = roomListTable->getRowCount();

		if(rv[z].passwordNeeded) {
			roomListTable->addRow(index);
			roomListTable->setCellText(index,0,"textures/roombrowser/lock.png");
			//roomListTable->setCellData(index, 0, "textures/roombrowser/lock.png");
			roomListTable->setCellText(index,1,c_rule.c_str());
			roomListTable->setCellText(index,2,c_mode.c_str());
			roomListTable->setCellText(index,3,c_duel_rule.c_str());//master rule
			roomListTable->setCellText(index,4,c_banlist.c_str());
			roomListTable->setCellText(index,5,c_players.c_str());
			roomListTable->setCellText(index,6,rv[z].roomdescription.c_str());
			roomListTable->setCellText(index,7,rv[z].isStarted ? dataManager.GetSysString(1986).c_str() : dataManager.GetSysString(1987).c_str());
		} else {
			roomListTable->addRow(index);
			roomListTable->setCellText(index,0,L"");
			roomListTable->setCellText(index,1,c_rule.c_str());
			roomListTable->setCellText(index,2,c_mode.c_str());
			roomListTable->setCellText(index,3,c_duel_rule.c_str());//master rule
			roomListTable->setCellText(index,4,c_banlist.c_str());
			roomListTable->setCellText(index,5,c_players.c_str());
			roomListTable->setCellText(index,6,rv[z].roomdescription.c_str());
			roomListTable->setCellText(index,7,rv[z].isStarted ? dataManager.GetSysString(1986).c_str() : dataManager.GetSysString(1987).c_str());
		}
		if(rv[z].isStarted) {
			color = SColor(100,211,211,211);
			roomListTable->setCellColor(index, 0, color);
			roomListTable->setCellColor(index, 1, color);
			roomListTable->setCellColor(index, 2, color);
			roomListTable->setCellColor(index, 3, color);
			roomListTable->setCellColor(index, 4, color);
			roomListTable->setCellColor(index, 5, color);
		} else {
			if(duel_rule==L'4' && nocheckdeck==L'0' && noshuffle==L'0' && life==8000 && hand==5 && draw==1) {
				color = SColor(255,255,255,255);
				roomListTable->setCellColor(index, 0, color);
				roomListTable->setCellColor(index, 1, color);
				roomListTable->setCellColor(index, 2, color);
				roomListTable->setCellColor(index, 3, color);
				roomListTable->setCellColor(index, 4, color);
				roomListTable->setCellColor(index, 5, color);
			} else {
				color = red;
				roomListTable->setCellColor(index, 0, color);
				roomListTable->setCellColor(index, 1, color);
				roomListTable->setCellColor(index, 2, color);
				roomListTable->setCellColor(index, 3, color);
				roomListTable->setCellColor(index, 4, color);
				roomListTable->setCellColor(index, 5, color);
			}
		}

		roomListTable->setCellData(index, 1, (void*)z);
	}
}
int ServerLobby::GetRoomsThread() {
	Utils::changeCursor(ECI_WAIT);
	mainGame->btnLanRefresh2->setEnabled(false);
	mainGame->serverChoice->setEnabled(false);
	mainGame->roomListTable->setVisible(false);

	ServerInfo serverInfo = mainGame->serversVector[mainGame->serverChoice->getSelected()];

	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;
	chunk.memory = (char*)malloc(1);
	chunk.size = 0;
	curl_handle = curl_easy_init();
	if(mainGame->chkShowActiveRooms->isChecked()) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}/?full=1", serverInfo.roomaddress, serverInfo.roomport).c_str());
	} else {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}", serverInfo.roomaddress, serverInfo.roomport).c_str());
	}
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "YGOPRO");

	curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "*"); 
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0); 

	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) {
		//error
		mainGame->env->addMessageBox(L"Error 05", dataManager.GetSysString(2037).c_str());
		curl_easy_cleanup(curl_handle);
		Utils::changeCursor(ECI_NORMAL);
		mainGame->btnLanRefresh2->setEnabled(true);
		mainGame->serverChoice->setEnabled(true);
		mainGame->roomListTable->setVisible(true);
		return 0;
	}

	curl_easy_cleanup(curl_handle);
	
	std::string server_rooms_string(chunk.memory);
	if(server_rooms_string == "[server busy]") {
		mainGame->env->addMessageBox(L"Error 04", dataManager.GetSysString(2031).c_str());
	} else {
		mainGame->roomsVector.clear();

		nlohmann::json j = nlohmann::json::parse(server_rooms_string);
		if(j.size()) {
			for(auto& obj : j["rooms"].get<std::vector<nlohmann::json>>()) {
				RoomInfo roomInfo;
				roomInfo.roomid = std::stoi(obj["roomid"].get<std::string>());
				roomInfo.roomname = hex_to_wstring(obj["roomname"].get<std::string>());
				roomInfo.roomdescription = hex_to_wstring(obj["desc"].get<std::string>());
				roomInfo.passwordNeeded = obj["needpass"].get<std::string>().compare("true") == 0;
				roomInfo.isStarted = obj["istart"].get<std::string>().compare("active") == 0;
				roomInfo.banlist = std::stoi(obj["banlist"].get<std::string>());
				int i = 0;
				for(auto& obj2 : obj["users"].get<std::vector<nlohmann::json>>()) {
					roomInfo.players[i] = hex_to_wstring(obj2["name"].get<std::string>());
					i++;
				}
				mainGame->roomsVector.push_back(roomInfo);
			}
		}
	}

	if(chunk.memory)
		free(chunk.memory);

	if(mainGame->roomsVector.size() == 0) {
		mainGame->env->addMessageBox(dataManager.GetSysString(2032).c_str(), dataManager.GetSysString(2033).c_str());
	} else {
		FillOnlineRooms();
		if(mainGame->chkShowActiveRooms->isChecked()) {
			mainGame->roomListTable->setActiveColumnOrdering(EGOM_DESCENDING);
			mainGame->roomListTable->setActiveColumn(7, true);
		}
	}

	Utils::changeCursor(ECI_NORMAL);
	mainGame->btnLanRefresh2->setEnabled(true);
	mainGame->serverChoice->setEnabled(true);
	mainGame->roomListTable->setVisible(true);
	return 0;
}
void ServerLobby::RefreshRooms() {
	std::thread(GetRoomsThread).detach();
}

}