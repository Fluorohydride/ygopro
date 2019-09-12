#ifndef DISCORD_WRAPPER_H
#define DISCORD_WRAPPER_H

#ifdef DISCORD_APP_ID
#include "discord_rpc.h"
#endif
#include <string>
#include "utils.h"
class DiscordWrapper {
public:
	enum PresenceType {
		MENU,
		DUEL,
		DUEL_STARTED,
		REPLAY,
		PUZZLE,
		DECK,
		DECK_SIDING,
		CLEAR
	};
	DiscordWrapper();
	~DiscordWrapper();
	bool Initialize(path_string workingDir);
	void UpdatePresence(PresenceType type);
	void Check();
private:
	bool wasLaunched;
#ifdef DISCORD_APP_ID
	static void OnReady(const DiscordUser* connectedUser);
	static void OnDisconnected(int errcode, const char* message);
	static void OnError(int errcode, const char* message);
	static void OnJoin(const char* secret);
	static void OnSpectate(const char* secret);
	static void OnJoinRequest(const DiscordUser* request);
#endif
};

#endif //DISCORD_WRAPPER_H
