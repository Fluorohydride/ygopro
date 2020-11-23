#ifndef DISCORD_WRAPPER_H
#define DISCORD_WRAPPER_H

#include <string>
#include <cstdint>

class DiscordWrapper {
public:
	struct DiscordSecret {
		uint32_t game_id;
		uint32_t server_address;
		uint16_t server_port;
		std::string pass;
	};
	enum PresenceType {
		MENU,
		IN_LOBBY,
		DUEL,
		DUEL_STARTED,
		REPLAY,
		PUZZLE,
		HAND_TEST,
		DECK,
		DECK_SIDING,
		CLEAR,
		INITIALIZE,
		DISCONNECT,
		TERMINATE
	};
	bool Initialize();
	void UpdatePresence(PresenceType type);
	bool connected{ false };
	void Check();
private:
	int64_t start{ 0 };
	int previous_gameid{ 0 };
	bool running{ false };
	char secret_buf[128];
	PresenceType presence{ CLEAR };
	static void Connect();
	static void Disconnect();
};

#endif //DISCORD_WRAPPER_H
