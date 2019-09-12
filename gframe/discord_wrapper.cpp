#include "discord_wrapper.h"
#include "discord_register.h"
#include <chrono>
#include <cstdio>
#include "game.h"

DiscordWrapper::DiscordWrapper(): wasLaunched(false) {
}

DiscordWrapper::~DiscordWrapper() {
#ifdef DISCORD_APP_ID
	Discord_Shutdown();
#endif
}

bool DiscordWrapper::Initialize(path_string workingDir) {
#ifdef DISCORD_APP_ID
	DiscordEventHandlers handlers = {};
	handlers.ready = OnReady;
	handlers.disconnected = OnDisconnected;
	handlers.errored = OnError;
	handlers.joinGame = OnJoin;
	handlers.spectateGame = OnSpectate;
	handlers.joinRequest = OnJoinRequest;
#ifdef _WIN32
	TCHAR exepath[MAX_PATH];
	GetModuleFileName(nullptr, exepath, MAX_PATH);
	std::wstring param = exepath + std::wstring(L" from_discord ") + workingDir;
#elif defined(__linux__)
	std::string param = workingDir + "/run.sh from_discord " + workingDir;
#else
	std::string param = "open " + workingDir + "/ygopro.app --args from_discord";
#endif
	Discord_Register(DISCORD_APP_ID, ygo::Utils::ToUTF8IfNeeded(param).c_str());
	Discord_Initialize(DISCORD_APP_ID, &handlers, 0, nullptr);
#endif //_WIN32
	return true;
}

void DiscordWrapper::UpdatePresence(PresenceType type) {
#ifdef DISCORD_APP_ID
	static int64_t start = 0;
	static PresenceType presence = CLEAR;
	PresenceType previous = presence;
	presence = type;
	if(previous != presence)
		start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(type == CLEAR) {
		Discord_ClearPresence();
		return;
	}
	DiscordRichPresence discordPresence = {};
	switch(presence) {
		case DiscordWrapper::MENU: {
			discordPresence.details = "In menu";
			break;
		}
		case DiscordWrapper::DUEL:
		case DiscordWrapper::DUEL_STARTED: {
			discordPresence.details = "In duel";
			break;
		}
		/*case DiscordWrapper::DUEL_STARTED: {
			discordPresence.details = "In duel";
			break;
		}*/
		case DiscordWrapper::REPLAY: {
			discordPresence.details = "Watching a replay";
			break;
		}
		case DiscordWrapper::PUZZLE: {
			discordPresence.details = "Playing a puzzle";
			break;
		}
		case DiscordWrapper::DECK: {
			discordPresence.details = "Editing a deck";
			break;
		}
		case DiscordWrapper::DECK_SIDING: {
			discordPresence.details = "Side decking";
			break;
		}
		case DiscordWrapper::CLEAR:
			break;
		default:
			break;
	}
	discordPresence.state = "Edopro FTW";
	discordPresence.startTimestamp = start;
	discordPresence.largeImageKey = "game-icon";
	discordPresence.smallImageKey = "game-icon";
	discordPresence.partyId = "party1234";
	discordPresence.partySize = 1;
	discordPresence.partyMax = 2;
	/*discordPresence.matchSecret = "xyzzy";*/
	discordPresence.joinSecret = "join";
	discordPresence.spectateSecret = "look";
	discordPresence.instance = 0;
	Discord_UpdatePresence(&discordPresence);
#endif
}

void DiscordWrapper::Check() {
#ifdef DISCORD_APP_ID
	Discord_RunCallbacks();
#endif
}

#ifdef DISCORD_APP_ID
void DiscordWrapper::OnReady(const DiscordUser* connectedUser) {
// 	wasLaunched = true;
}

void DiscordWrapper::OnDisconnected(int errcode, const char * message) {
}

void DiscordWrapper::OnError(int errcode, const char * message) {
}

void DiscordWrapper::OnJoin(const char* secret) {
}

void DiscordWrapper::OnSpectate(const char * secret) {
}

void DiscordWrapper::OnJoinRequest(const DiscordUser* request) {
	printf("%s", request->userId);
	Discord_Respond(request->userId, DISCORD_REPLY_YES);
}
#endif
