#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include "config.h"
#include "network.h"
#include <atomic>

namespace ygo {
struct ServerInfo {
	std::wstring name;
	std::wstring address;
	std::wstring roomaddress;
	int duelport;
	int roomlistport;
};
struct RoomInfo {
	std::wstring name;
	int id;
	bool started;
	bool locked;
	std::vector<std::wstring> players;
	std::wstring description;
	HostInfo info;
};
class ServerLobby {
public:
	static void RefreshRooms();
	static int GetRoomsThread();
	static void FillOnlineRooms();
	static void JoinServer(bool host);
	static std::atomic_bool is_refreshing;
};
//extern ServerLobby serverLobby;
}
#endif //SERVERLOBBY_H