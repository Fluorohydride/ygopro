#include "discord_wrapper.h"
#include "discord_register.h"
#include <chrono>
#include <cstdio>
#include "game.h"
#include "duelclient.h"

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
	char buff[PATH_MAX];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
	std::string filename;
	if (len != -1) {
		buff[len] = '\0';
		filename = ygo::Utils::GetFileName(buff);
	}
	std::string param = fmt::format("bash -c \"cd {}; ./{} from_discord\"", workingDir, filename);
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
	std::string presenceState;
	switch(presence) {
		case MENU: {
			discordPresence.details = "In menu";
			break;
		}
		case IN_LOBBY:
		case DUEL:
		case DUEL_STARTED:
		case DECK_SIDING: {
			if(presence == IN_LOBBY) {
				discordPresence.details = "Hosting a Duel";
				auto count = ygo::DuelClient::GetPlayersCount();
				discordPresence.partySize = count.first + count.second;
				discordPresence.partyMax = ygo::mainGame->dInfo.team1 + ygo::mainGame->dInfo.team2;
				discordPresence.joinSecret = "join";
			} else {
				if(presence == DECK_SIDING)
					discordPresence.details = "Side decking";
				else
					discordPresence.details = "Dueling";
				discordPresence.spectateSecret = "look";
			}
			if(((ygo::mainGame->dInfo.team1 + ygo::mainGame->dInfo.team2) > 2) || ygo::mainGame->dInfo.isRelay)
				presenceState = fmt::format("{}: {} vs {}", ygo::mainGame->dInfo.isRelay ? "Relay" : "Tag", ygo::mainGame->dInfo.team1, ygo::mainGame->dInfo.team2).c_str();
			else
				presenceState = "1 vs 1";
			if(ygo::mainGame->dInfo.best_of) {
				presenceState += fmt::format(" (best of {})", ygo::mainGame->dInfo.best_of);
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
	//discordPresence.smallImageKey = "game-icon";
	discordPresence.partyId = "party1234";
	/*discordPresence.partySize = 1;
	discordPresence.partyMax = 2;*/
	/*discordPresence.matchSecret = "xyzzy";*/
	//discordPresence.joinSecret = "join";
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
