#ifdef DISCORD_APP_ID
#include <iostream>
#include <chrono>
#include <nlohmann/json.hpp>
#include <IGUIElement.h>
#include <IrrlichtDevice.h>
#include <IGUIButton.h>
#include <IGUIEditBox.h>
#include <IGUIStaticText.h>
#include <IGUITabControl.h>
#include <IGUIWindow.h>
#include "discord_register.h"
#include "discord_rpc.h"
#include "game.h"
#include "duelclient.h"
#include "logging.h"
#endif
#include "discord_wrapper.h"

DiscordWrapper::DiscordWrapper(): connected(false){
}

bool DiscordWrapper::Initialize(path_string workingDir) {
#ifdef DISCORD_APP_ID
#if defined(_WIN32) || defined(__linux__)
#ifdef _WIN32
	path_string param = fmt::format(EPRO_TEXT("\\\"{}\\\" from_discord \\\"{}\\\""), ygo::Utils::GetExePath(), workingDir);
#elif defined(__linux__)
	std::string param = fmt::format("bash -c \"cd \\\"{}\\\"; \\\"{}\\\" from_discord\"", workingDir, ygo::Utils::GetExePath());
#endif //_WIN32
	Discord_Register(DISCORD_APP_ID, ygo::Utils::ToUTF8IfNeeded(param).c_str());
#else
	RegisterURL(DISCORD_APP_ID);
#endif //_WIN32
	return true;
#else
	return false;
#endif //DISCORD_APP_ID
}

void DiscordWrapper::UpdatePresence(PresenceType type) {
#ifdef DISCORD_APP_ID
	static int64_t start = 0;
	static PresenceType presence = CLEAR;
	static int previous_gameid = 0;
	static bool running = false;
	if(type == INITIALIZE && !running) {
		Connect();
		running = true;
		return;
	}
	if((type == TERMINATE || type == DISCONNECT) && running) {
		if(type == TERMINATE) {
			DiscordEventHandlers handlers = {};
			handlers.ready = nullptr;
			handlers.disconnected = nullptr;
			handlers.errored = nullptr;
			handlers.joinGame = nullptr;
			handlers.spectateGame = nullptr;
			handlers.joinRequest = nullptr;
			handlers.payload = nullptr;
			Discord_UpdateHandlers(&handlers);
		}
		Disconnect();
		running = false;
		return;
	}
	if(!running)
		return;
	PresenceType previous = presence;
	presence = type;
	if(previous != presence)
		start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(type == CLEAR) {
		Discord_ClearPresence();
		return;
	}
	DiscordRichPresence discordPresence = {};
	std::string presenceState;
	std::string partyid;
	switch(presence) {
		case MENU: {
			discordPresence.details = "In menu";
			break;
		}
		case IN_LOBBY:
		case DUEL:
		case DUEL_STARTED:
		case DECK_SIDING: {
			auto count = ygo::DuelClient::GetPlayersCount();
			discordPresence.partySize = std::max(1, count.first + count.second);
			discordPresence.partyMax = ygo::mainGame->dInfo.team1 + ygo::mainGame->dInfo.team2 + ygo::DuelClient::GetSpectatorsCount() + 1;
			if(presence == IN_LOBBY) {
				discordPresence.details = "Hosting a Duel";
			} else {
				if(presence == DECK_SIDING)
					discordPresence.details = "Side decking";
				else
					discordPresence.details = "Dueling";
			}
			if(((ygo::mainGame->dInfo.team1 + ygo::mainGame->dInfo.team2) > 2) || ygo::mainGame->dInfo.isRelay)
				presenceState = fmt::format("{}: {} vs {}", ygo::mainGame->dInfo.isRelay ? "Relay" : "Tag", ygo::mainGame->dInfo.team1, ygo::mainGame->dInfo.team2).c_str();
			else
				presenceState = "1 vs 1";
			if(ygo::mainGame->dInfo.best_of) {
				presenceState += fmt::format(" (best of {})", ygo::mainGame->dInfo.best_of);
			}
			if(ygo::mainGame->dInfo.secret.game_id) {
				partyid = fmt::format("{}{}", ygo::mainGame->dInfo.secret.game_id, ygo::mainGame->dInfo.secret.server_address);
				discordPresence.joinSecret = CreateSecret(previous_gameid != ygo::mainGame->dInfo.secret.game_id).c_str();
				previous_gameid = ygo::mainGame->dInfo.secret.game_id;
			}
			break;
		}
		case REPLAY: {
			discordPresence.details = "Watching a replay";
			break;
		}
		case PUZZLE: {
			discordPresence.details = "Playing a puzzle";
			break;
		}
		case HAND_TEST: {
			discordPresence.details = "Testing hands";
			break;
		}
		case DECK: {
			discordPresence.details = "Editing a deck";
			break;
		}
		case CLEAR:
			break;
		default:
			break;
	}
	discordPresence.state = presenceState.c_str();
	discordPresence.startTimestamp = start;
	discordPresence.largeImageKey = "game-icon";
	discordPresence.partyId = partyid.c_str();
	Discord_UpdatePresence(&discordPresence);
#endif
}

void DiscordWrapper::Check() {
#ifdef DISCORD_APP_ID
	Discord_RunCallbacks();
#endif
}

void DiscordWrapper::Connect() {
#ifdef DISCORD_APP_ID
	DiscordEventHandlers handlers = {};
	handlers.ready = OnReady;
	handlers.disconnected = OnDisconnected;
	handlers.errored = OnError;
	handlers.joinGame = OnJoin;
	handlers.spectateGame = OnSpectate;
	handlers.joinRequest = OnJoinRequest;
	handlers.payload = &(*ygo::mainGame);
	Discord_Initialize(DISCORD_APP_ID, &handlers, 0, nullptr);
#endif
}

void DiscordWrapper::Disconnect() {
#ifdef DISCORD_APP_ID
	Discord_ClearPresence();
	Discord_Shutdown();
#endif
}

#ifdef DISCORD_APP_ID
std::string& DiscordWrapper::CreateSecret(bool update) const {
	static std::string string;
	if(!update)
		return string;
	auto& secret = ygo::mainGame->dInfo.secret;
	/*using fmt over nlohmann::json for an overall memory improvement as making a json object and converting that to string would be way slower than creating it with a formatted string*/
	string = fmt::format("{{\"id\": {},\"addr\" : {},\"port\" : {},\"pass\" : \"{}\" }}", secret.game_id, secret.server_address, secret.server_port, secret.pass.c_str());
	return string;
}
void DiscordWrapper::OnReady(const DiscordUser* connectedUser, void* payload) {
	printf("Discord: Connected to user %s#%s - %s\n",
		   connectedUser->username,
		   connectedUser->discriminator,
		   connectedUser->userId);
	std::cout << "ready at " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
	std::cout << "ready at (ms) " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
	static_cast<ygo::Game*>(payload)->discord.connected = true;
}

void DiscordWrapper::OnDisconnected(int errcode, const char* message, void* payload) {
	printf("Discord: Disconnected, error code: %d - %s\n",
		   errcode,
		   message);
	static_cast<ygo::Game*>(payload)->discord.connected = false;
}

void DiscordWrapper::OnError(int errcode, const char * message, void* payload) {
}

void DiscordWrapper::OnJoin(const char* secret, void* payload) {
	auto game = static_cast<ygo::Game*>(payload);
	if((game->is_building && game->is_siding) || game->dInfo.isInDuel || game->dInfo.isInLobby || game->dInfo.isReplay || game->wHostPrepare->isVisible())
		return;
	auto& host = ygo::mainGame->dInfo.secret;
	try {
		nlohmann::json json = nlohmann::json::parse(secret);
		host.game_id = json["id"].get<int>();
		host.server_address = json["addr"].get<int>();
		host.server_port = json["port"].get<int>();
		host.pass = json["pass"].get<std::string>();
	}
	catch(std::exception& e) {
		ygo::ErrorLog(std::string("Exception occurred: ") + e.what());
		return;
	}
	game->isHostingOnline = true;
	if(ygo::DuelClient::StartClient(host.server_address, host.server_port, host.game_id, false)) {
#define HIDE_AND_CHECK(obj) if(obj->isVisible()) game->HideElement(obj);
		if(game->is_building)
			game->deckBuilder.Terminate(false);
		HIDE_AND_CHECK(game->wMainMenu)
		HIDE_AND_CHECK(game->wLanWindow)
		HIDE_AND_CHECK(game->wCreateHost)
		HIDE_AND_CHECK(game->wReplay)
		HIDE_AND_CHECK(game->wSinglePlay)
		HIDE_AND_CHECK(game->wDeckEdit)
		HIDE_AND_CHECK(game->wRules)
		HIDE_AND_CHECK(game->wCustomRulesL)
		HIDE_AND_CHECK(game->wCustomRulesR)
		HIDE_AND_CHECK(game->wRoomListPlaceholder)
		HIDE_AND_CHECK(game->wCardImg)
		HIDE_AND_CHECK(game->wInfos)
		HIDE_AND_CHECK(game->btnLeaveGame)
		HIDE_AND_CHECK(game->wReplaySave)
		game->device->setEventReceiver(&game->menuHandler);
#undef HIDE_AND_CHECK
	}
}

void DiscordWrapper::OnSpectate(const char * secret, void* payload) {
}

void DiscordWrapper::OnJoinRequest(const DiscordUser* request, void* payload) {
	printf("%s", request->userId);
	Discord_Respond(request->userId, DISCORD_REPLY_YES);
}
#endif
