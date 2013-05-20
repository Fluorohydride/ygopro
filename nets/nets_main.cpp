#include "../common/common.h"
#include "nets_impl.h"
#include "clan_mgr.h"
#include "player_mgr.h"
#include "ticket.h"
#include "dbadapter.h"

Random globalRandom;
ygopro::DBAdapter ygopro::dbAdapter;
unsigned int proto_version = 0x2000;

int main(int argc, char* argv[])
{
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif

	globalRandom.Initialise((unsigned int)time(0));
	ygopro::dbAdapter.Connect("127.0.0.1");
	ygopro::clanMgr.LoadClans();
	ygopro::playerMgr.LoadPlayers();
	ygopro::ticketMgr.LoadTicketInfos();
	ygopro::playerServer.Run(8848);

#ifdef _WIN32
	WSACleanup();
#endif
	return 0;
}
