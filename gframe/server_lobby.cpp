#include <nlohmann/json.hpp>
#include <IGUITable.h>
#include <IGUIEditBox.h>
#include <IGUIComboBox.h>
#include <IGUIButton.h>
#include <IGUICheckBox.h>
#include <IGUIWindow.h>
#include <ICursorControl.h>
#include "server_lobby.h"
#include <fmt/format.h>
#include <curl/curl.h>
#include "utils.h"
#include "data_manager.h"
#include "game.h"
#include "duelclient.h"
#include "logging.h"
#include "utils_gui.h"
#include "custom_skin_enum.h"
#include "game_config.h"

namespace ygo {

std::vector<RoomInfo> ServerLobby::roomsVector;
std::vector<ServerInfo> ServerLobby::serversVector;
std::atomic_bool ServerLobby::is_refreshing{ false };
std::atomic_bool ServerLobby::has_refreshed{ false };

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
	auto& rooms = roomsVector;

	std::wstring searchText(Utils::ToUpperNoAccents<std::wstring>(mainGame->ebRoomName->getText()));

	int searchRules = mainGame->cbFilterRule->getSelected();
	int searchBanlist = mainGame->cbFilterBanlist->getSelected();
	int bestOf = 0;
	int team1 = 0;
	int team2 = 0;
	try {
		bestOf = std::stoi(mainGame->ebOnlineBestOf->getText());
		team1 = std::stoi(mainGame->ebOnlineTeam1->getText());
		team2 = std::stoi(mainGame->ebOnlineTeam2->getText());
	}
	catch(...) {}

	bool doFilter = searchText.size() || searchRules > 0 || searchBanlist > 0 || bestOf || team1 || team2 || mainGame->btnFilterRelayMode->isPressed();

	const auto& normal_room = skin::ROOMLIST_NORMAL_ROOM_VAL;
	const auto& custom_room = skin::ROOMLIST_CUSTOM_ROOM_VAL;
	const auto& started_room = skin::ROOMLIST_STARTED_ROOM_VAL;
	const bool show_password_checked = mainGame->chkShowPassword->isChecked();
	const bool show_started_checked = mainGame->chkShowActiveRooms->isChecked();
	for(auto& room : rooms) {
		if((room.locked && !show_password_checked) || (room.started && !show_started_checked)) {
			continue;
		}
		if(doFilter) {
			if(searchText.size()) {
				bool res = Utils::ContainsSubstring(room.description, searchText, true);
				for(auto& name : room.players) {
					res = res || Utils::ContainsSubstring(name, searchText, true);
				}
				if(!res)
					continue;
			}
			if(bestOf && room.info.best_of != bestOf)
				continue;
			if(team1 && room.info.team1 != team1)
				continue;
			if(team2 && room.info.team2 != team2)
				continue;
			if(searchRules > 0) {
				if(searchRules != room.info.rule + 1)
					continue;
			}
			/*add mutex for banlist access*/
			if(searchBanlist > 0) {
				if(room.info.lflist != gdeckManager->_lfList[searchBanlist - 1].hash) {
					continue;
				}
			}
			if(mainGame->btnFilterRelayMode->isPressed() && !(room.info.duel_flag_low & DUEL_RELAY))
				continue;
		}
		std::wstring banlist;

		for(auto& list : gdeckManager->_lfList) {
			if(list.hash == room.info.lflist) {
				banlist = list.listName;
				break;
			}
		}

		irr::gui::IGUITable* roomListTable = mainGame->roomListTable;
		int index = roomListTable->getRowCount();

		roomListTable->addRow(index);
		roomListTable->setCellData(index, 0, room.locked ? (void*)1 : nullptr);
		roomListTable->setCellData(index, 1, &room);
		roomListTable->setCellText(index, 1, gDataManager->GetSysString(room.info.rule + 1900).data());
		roomListTable->setCellText(index, 2, fmt::format(L"[{}vs{}]{}{}", room.info.team1, room.info.team2,
			(room.info.best_of > 1) ? fmt::format(L" (best of {})", room.info.best_of) : L"",
			(room.info.duel_flag_low & DUEL_RELAY) ? L" (Relay)" : L"").data());
		int rule;
		auto duel_flag = (((uint64_t)room.info.duel_flag_low) | ((uint64_t)room.info.duel_flag_high) << 32);
		mainGame->GetMasterRule(duel_flag & ~(DUEL_RELAY | DUEL_TCG_SEGOC_NONPUBLIC), room.info.forbiddentypes, &rule);
		if(rule == 6) {
			if(duel_flag == DUEL_MODE_GOAT) {
				roomListTable->setCellText(index, 3, "GOAT");
			} else if(duel_flag == DUEL_MODE_RUSH) {
				roomListTable->setCellText(index, 3, "Rush");
			} else if(duel_flag == DUEL_MODE_SPEED) {
				roomListTable->setCellText(index, 3, "Speed");
			} else
				roomListTable->setCellText(index, 3, "Custom");
		} else
			roomListTable->setCellText(index, 3, fmt::format(L"{}MR {}", 
															 (duel_flag & DUEL_TCG_SEGOC_NONPUBLIC) ? L"TCG " : L"",
															 (rule == 0) ? 3 : rule).data());
		roomListTable->setCellText(index, 4, (banlist.size()) ? banlist.data() : L"???");
		std::wstring players;
		for(auto& player : room.players) {
			players.append(player).append(L", ");
		}
		if(players.size())
			players.resize(players.size() - 2);
		roomListTable->setCellText(index, 5, players.data());
		roomListTable->setCellText(index, 6, room.description.data());
		roomListTable->setCellText(index, 7, room.started ? gDataManager->GetSysString(1986).data() : gDataManager->GetSysString(1987).data());

		irr::video::SColor color;
		if(room.started)
			color = started_room;
		else if(rule == 5 && !room.info.no_check_deck && !room.info.no_shuffle_deck && room.info.start_lp == 8000 && room.info.start_hand == 5 && room.info.draw_count == 1)
			color = normal_room;
		else
			color = custom_room;
		roomListTable->setCellColor(index, 0, color);
		roomListTable->setCellColor(index, 1, color);
		roomListTable->setCellColor(index, 2, color);
		roomListTable->setCellColor(index, 3, color);
		roomListTable->setCellColor(index, 4, color);
		roomListTable->setCellColor(index, 5, color);
		roomListTable->setCellColor(index, 6, color);
		roomListTable->setCellColor(index, 7, color);
	}
	mainGame->roomListTable->setActiveColumn(mainGame->roomListTable->getActiveColumn(), true);
	if(!roomsVector.empty() && mainGame->chkShowActiveRooms->isChecked()) {
		mainGame->roomListTable->setActiveColumn(7, true);
		mainGame->roomListTable->orderRows(-1, irr::gui::EGOM_DESCENDING);
	}

	GUIUtils::ChangeCursor(mainGame->device, irr::gui::ECI_NORMAL);
	mainGame->btnLanRefresh2->setEnabled(true);
	mainGame->serverChoice->setEnabled(true);
	mainGame->roomListTable->setVisible(true);
	has_refreshed = false;
}
void ServerLobby::GetRoomsThread() {
	Utils::SetThreadName("RoomlistFetch");
	auto selected = mainGame->serverChoice->getSelected();
	if (selected < 0) return;
	ServerInfo serverInfo = serversVector[selected];

	GUIUtils::ChangeCursor(mainGame->device, irr::gui::ECI_WAIT);
	mainGame->btnLanRefresh2->setEnabled(false);
	mainGame->serverChoice->setEnabled(false);
	mainGame->roomListTable->setVisible(false);

	std::string retrieved_data;
	CURL* curl_handle = curl_easy_init();
	//if(mainGame->chkShowActiveRooms->isChecked()) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}/api/getrooms", BufferIO::EncodeUTF8(serverInfo.roomaddress), serverInfo.roomlistport).data());
	/*} else {
		curl_easy_setopt(curl_handle, CURLOPT_URL, fmt::format("http://{}:{}/api/getrooms", BufferIO::EncodeUTF8(serverInfo.roomaddress), serverInfo.roomlistport).data());
	}*/
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 7L);
	curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 15L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &retrieved_data);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, ygo::Utils::GetUserAgent().data());

	curl_easy_setopt(curl_handle, CURLOPT_NOPROXY, "*"); 
	curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 0);
	if(gGameConfig->ssl_certificate_path.size() && Utils::FileExists(Utils::ToPathString(gGameConfig->ssl_certificate_path)))
		curl_easy_setopt(curl_handle, CURLOPT_CAINFO, gGameConfig->ssl_certificate_path.data());
#ifdef _WIN32
	else
		curl_easy_setopt(curl_handle, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
#endif

	const auto res = curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	if(res != CURLE_OK) {
		//error
		mainGame->PopupMessage(gDataManager->GetSysString(2037));
		GUIUtils::ChangeCursor(mainGame->device, irr::gui::ECI_NORMAL);
		mainGame->btnLanRefresh2->setEnabled(true);
		mainGame->serverChoice->setEnabled(true);
		mainGame->roomListTable->setVisible(true);
		is_refreshing = false;
		return;
	}

	if(retrieved_data == "[server busy]") {
		mainGame->PopupMessage(gDataManager->GetSysString(2031));
	} else {
		roomsVector.clear();
		try {
			nlohmann::json j = nlohmann::json::parse(retrieved_data);
			if (j.size()) {
#define GET(field, type) obj[field].get<type>()
				for (auto& obj : j["rooms"]) {
					RoomInfo room;
					room.id = GET("roomid", int);
					room.name = BufferIO::DecodeUTF8(obj["roomname"].get_ref<std::string&>());
					room.description = BufferIO::DecodeUTF8(obj["roomnotes"].get_ref<std::string&>());
					room.locked = GET("needpass", bool);
					room.started = obj["istart"].get_ref<std::string&>() == "start";
					room.info.mode = GET("roommode", int);
					room.info.team1 = GET("team1", int);
					room.info.team2 = GET("team2", int);
					room.info.best_of = GET("best_of", int);
					const auto flag = GET("duel_flag", uint64_t);
					room.info.duel_flag_low = flag & 0xffffffff;
					room.info.duel_flag_high = (flag >> 32) & 0xffffffff;
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
					for (auto& obj2 : obj["users"])
						room.players.push_back(BufferIO::DecodeUTF8(obj2["name"].get_ref<std::string&>()));

					roomsVector.push_back(std::move(room));
				}
			}
		}
		catch (const std::exception& e) {
			ErrorLog(fmt::format("Exception occurred parsing server rooms: {}", e.what()));
		}
	}
	has_refreshed = true;
	is_refreshing = false;
}
void ServerLobby::RefreshRooms() {
	if(is_refreshing)
		return;
	is_refreshing = true;
	mainGame->roomListTable->clearRows();
	std::thread(GetRoomsThread).detach();
}
bool ServerLobby::HasRefreshedRooms() {
	return has_refreshed;
}
void ServerLobby::JoinServer(bool host) {
	mainGame->ebNickName->setText(mainGame->ebNickNameOnline->getText());
	auto selected = mainGame->serverChoice->getSelected();
	if (selected < 0) return;
	std::pair<uint32_t, uint16_t> serverinfo;
	try {
		const ServerInfo& server = serversVector[selected];
		serverinfo = DuelClient::ResolveServer(server.address, server.duelport);
	}
	catch(std::exception& e) { ErrorLog(fmt::format("Exception occurred: {}", e.what())); }
	if(host) {
		if(!DuelClient::StartClient(serverinfo.first, serverinfo.second))
			return;
	} else {
		//client
		auto room = static_cast<RoomInfo*>(mainGame->roomListTable->getCellData(mainGame->roomListTable->getSelected(), 1));
		if(!room)
			return;
		if(room->locked) {
			if(!mainGame->wRoomPassword->isVisible()) {
				mainGame->wRoomPassword->setVisible(true);
				return;
			}
			auto text = mainGame->ebRPName->getText();
			if(*text == L'\0')
				return;
			mainGame->wRoomPassword->setVisible(false);
			mainGame->dInfo.secret.pass = BufferIO::EncodeUTF8(text);
		} else
			mainGame->dInfo.secret.pass.clear();
		if(!DuelClient::StartClient(serverinfo.first, serverinfo.second, room->id, false))
			return;
	}
}


}
