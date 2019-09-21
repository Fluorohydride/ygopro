#ifndef SERVERLOBBY_H
#define SERVERLOBBY_H

#include "config.h"

namespace ygo {
	class ServerLobby {
		public:
			static void RefreshRooms();
			static int GetRoomsThread();
			static void FillOnlineRooms();
		private:
	};
	//extern ServerLobby serverLobby;
}
#endif //SERVERLOBBY_H