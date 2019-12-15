#include "server_lobby.h"
#include "data_manager.h"
#include "game.h"
#include "duelclient.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace ygo {

std::atomic_bool ServerLobby::is_refreshing{ false };

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	auto buff = static_cast<std::string*>(userp);
	size_t prev_size = buff->size();
	buff->resize(prev_size + realsize);
	memcpy((char*)buff->data() + prev_size, contents, realsize);
	return realsize;
}

void ServerLobby::FillOnlineRooms() {
	mainGame->roomListTable->clearRows();
	std::vector<RoomInfo>& rooms = mainGame->roomsVector;

	std::wstring searchText(Game::StringtoUpper(mainGame->ebRoomName->getText()));

	int searchRules = mainGame->cbFilterRule->getSelected();
	int searchBanlist = mainGame->cbFilterBanlist->getSelected();
	int bestOf = std::stoi(mainGame->ebOnlineBestOf->getText());
	int team1 = std::stoi(mainGame->ebOnlineTeam1->getText());
	int team2 = std::stoi(mainGame->ebOnlineTeam2->getText());

	bool doFilter = searchText.size() || searchRules > 0 || searchBanlist > 0 || bestOf || team1 || team2 || mainGame->btnFilterRelayMode->isPressed();

	SColor normal = SColor(255, 0, 0, 0);
	SColor red = SColor(255, 255, 100, 100);
	SColor color = normal;
	bool show_password_checked = mainGame->chkShowPassword->isChecked();
	bool show_started_checked = mainGame->chkShowActiveRooms->isChecked();
	for(auto& room : rooms) {
		if((room.locked && !show_password_checked) || (room.started && !show_started_checked)) {
			continue;
		}
		if(doFilter) {
			if(searchText.size()) {
				bool res = false;
				for(auto& name : room.players) {
					res = res || Game::CompareStrings(name, searchText, true, false);
				}
				if(!res && (room.description.size() && !Game::CompareStrings(room.description, searchText, true, false)))
					continue;
			}
			if(bestOf && room.info.best_of != bestOf)
				continue;
			if(team1 && room.info.team1 != team1)
				continue;
			if(team2 && room.info.team2 != team2)
				continue;
			if(searchRules > 0) {
				if(searchRules != room.info.rule - 1)
					continue;
			}
			/*add mutex for banlist access*/
			if(searchBanlist > 0) {
				if(room.info.lflist != deckManager._lfList[searchBanlist - 1].hash) {
					continue;
				}
			}
			if(mainGame->btnFilterRelayMode->isPressed() && !(room.info.duel_flag & DUEL_RELAY))
				continue;
		}
		std::wstring banlist;

		for(auto& list : deckManager._lfList) {
			if(list.hash == room.info.lflist) {
				banlist = list.listName;
				break;
			}
		}

		irr::gui::IGUITable* roomListTable = mainGame->roomListTable;
		int index = roomListTable->getRowCount();

		roomListTable->addRow(index);
		roomListTable->setCellData(index, 0, room.locked ? (void*)1 : nullptr);
		roomListTable->setCellData(index, 1, (void*)&room);
		roomListTable->setCellText(index, 1, dataManager.GetSysString(room.info.rule + 1900).c_str());
		roomListTable->setCellText(index, 2, fmt::format(L"[{}vs{}]{}{}", room.info.team1, room.info.team2,
			(room.info.best_of > 1) ? fmt::format(L" (best of {})", room.info.best_of).c_str() : L"",
			(room.info.duel_flag & DUEL_RELAY) ? L" (Relay)" : L"").c_str());
		int rule;
		mainGame->GetMasterRule(room.info.duel_flag & ~DUEL_RELAY, room.info.forbiddentypes, &rule);
		if(rule == 5)
			roomListTable->setCellText(index, 3, "Custom");
		else
			roomListTable->setCellText(index, 3, fmt::format(L"MR {}", (rule == 0) ? 3 : rule).c_str());
		roomListTable->setCellText(index, 4, (banlist.size()) ? banlist.c_str() : L"???");
		std::wstring players;
		for(auto& player : room.players) {
			players.append(player).append(L", ");
		}
		if(players.size())
			players.resize(players.size() - 2);
		roomListTable->setCellText(index, 5, players.c_str());
		roomListTable->setCellText(index, 6, room.description.c_str());
		roomListTable->setCellText(index, 7, room.started ? dataManager.GetSysString(1986).c_str() : dataManager.GetSysString(1987).c_str());


		if(room.started)
			color = SColor(100, 211, 211, 211);
		else if(rule == 4 && !room.info.no_check_deck && !room.info.no_shuffle_deck && room.info.start_lp == 8000 && room.info.start_hand == 5 && room.info.draw_count == 1)
			color = SColor(255, 255, 255, 255);
		else
			color = red;
		roomListTable->setCellColor(index, 0, color);
		roomListTable->setCellColor(index, 1, color);
		roomListTable->setCellColor(index, 2, color);
		roomListTable->setCellColor(index, 3, color);
		roomListTable->setCellColor(index, 4, color);
		roomListTable->setCellColor(index, 5, color);
		roomListTable->setCellColor(index, 6, color);
		roomListTable->setCellColor(index, 7, color);
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

	std::string retrieved_data;
	curl_handle = curl_easy_init();
	if(mainGame->chkShowActiveRooms->isChecked()) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}/api/getrooms", BufferIO::EncodeUTF8s(serverInfo.roomaddress), serverInfo.roomlistport).c_str());
	} else {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}/api/getrooms", BufferIO::EncodeUTF8s(serverInfo.roomaddress), serverInfo.roomlistport).c_str());
	}
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 7L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&retrieved_data);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "YGOPRO");

	curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "*"); 
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0); 

	res = curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	if(res != CURLE_OK) {
		//error
		mainGame->PopupMessage(dataManager.GetSysString(2037), L"Error 05");
		Utils::changeCursor(ECI_NORMAL);
		mainGame->btnLanRefresh2->setEnabled(true);
		mainGame->serverChoice->setEnabled(true);
		mainGame->roomListTable->setVisible(true);
		is_refreshing = false;
		return 0;
	}

	if(retrieved_data == "[server busy]") {
		mainGame->PopupMessage(dataManager.GetSysString(2031), L"Error 04");
	} else {
		mainGame->roomsVector.clear();
		nlohmann::json j = nlohmann::json::parse(retrieved_data);
		if(j.size()) {
#define GET(field, type) obj[field].get<type>()
			try {
				for(auto& obj : j["rooms"].get<std::vector<nlohmann::json>>()) {
					RoomInfo room;
					room.id = GET("roomid", int);
					room.name = BufferIO::DecodeUTF8s(GET("roomname", std::string));
					room.description = BufferIO::DecodeUTF8s(GET("roomnotes", std::string));
					room.locked = GET("needpass", bool);
					room.started = GET("istart", std::string) == "start";
					room.info.mode = GET("roommode", int);
					room.info.team1 = GET("team1", int);
					room.info.team2 = GET("team2", int);
					room.info.best_of = GET("best_of", int);
					room.info.duel_flag = GET("duel_flag", int);
					room.info.forbiddentypes = GET("forbidden_types", int);
					room.info.extra_rules = GET("extra_rules", int);
					room.info.start_lp = GET("start_lp", int);
					room.info.start_hand = GET("start_hand", int);
					room.info.draw_count = GET("draw_count", int);
					room.info.time_limit = GET("time_limit", int);
					room.info.rule = GET("rule", int);
					room.info.no_check_deck = GET("no_check", bool);
					room.info.no_shuffle_deck = GET("no_shuffle", bool);
					room.info.lflist = GET("banlist_hash", int);
#undef GET
					for(auto& obj2 : obj["users"].get<std::vector<nlohmann::json>>()) {
						room.players.push_back(BufferIO::DecodeUTF8s(obj2["name"].get<std::string>()));
					}
					mainGame->roomsVector.push_back(std::move(room));
				}
			}
			catch(...) {

			}
		}
	}
	if(mainGame->roomsVector.empty()) {
		mainGame->PopupMessage(dataManager.GetSysString(2033), dataManager.GetSysString(2032));
	} else {
		FillOnlineRooms();
		if(mainGame->chkShowActiveRooms->isChecked()) {
			mainGame->roomListTable->setActiveColumn(7, true);
			mainGame->roomListTable->orderRows(-1, EGOM_DESCENDING);
		}
	}

	Utils::changeCursor(ECI_NORMAL);
	mainGame->btnLanRefresh2->setEnabled(true);
	mainGame->serverChoice->setEnabled(true);
	mainGame->roomListTable->setVisible(true);
	is_refreshing = false;
	return 0;
}
void ServerLobby::RefreshRooms() {
	if(is_refreshing)
		return;
	is_refreshing = true;
	mainGame->roomListTable->clearRows();
	std::thread(GetRoomsThread).detach();
}
void ServerLobby::JoinServer(bool host) {
	mainGame->ebNickName->setText(mainGame->ebNickNameOnline->getText());
	std::vector<RoomInfo> rv = mainGame->roomsVector;
	ServerInfo server = mainGame->serversVector[mainGame->serverChoice->getSelected()];
	try {
		auto serverinfo = DuelClient::ResolveServer(server.address, server.duelport);
		if(host) {
			if(!DuelClient::StartClient(serverinfo.first, serverinfo.second)) {
				return;
			}
		} else {
			//client
			RoomInfo* room = static_cast<RoomInfo*>(mainGame->roomListTable->getCellData(mainGame->roomListTable->getSelected(), 1));
			if(room->locked) {
				if(!mainGame->wRoomPassword->isVisible()) {
					mainGame->wRoomPassword->setVisible(true);
					return;
				} else {
					if(wcslen(mainGame->ebRPName->getText()) == 0) {
						return;
					}
				}
			}
			mainGame->dInfo.secret.pass = BufferIO::EncodeUTF8s(mainGame->ebRPName->getText());
			if(!DuelClient::StartClient(serverinfo.first, serverinfo.second, room->id, false)) {
				return;
			}
		}
	}
	catch(std::exception& e) {
		mainGame->ErrorLog(std::string("Exception ocurred: ") + e.what());
	}
}


}