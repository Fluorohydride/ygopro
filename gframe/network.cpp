#include "network.h"

namespace ygo {

/*
int NetManager::GetLocalAddress() {
	char hname[256];
	gethostname(hname, 256);
	hostent* host = gethostbyname(hname);
	if(!host)
		return 0;
	int i = 0;
	for(i = 0; i < 8; ++i) {
		local_addr[i] = 0;
		if(host->h_addr_list[i] == 0)
			break;
		local_addr[i] = *(unsigned int*)host->h_addr_list[i];
	}
	return i;
}
int NetManager::BroadcastServer(void* np) {
	NetManager* net = (NetManager*)np;
	SOCKADDR_IN sockTo;
	sockTo.sin_family = AF_INET;
	sockTo.sin_port = htons(7912);
	int recvLen = recvfrom(net->sBHost, (char*)&net->hReq, sizeof(HostRequest), 0, 0, 0);
	while(recvLen != 0 && recvLen != SOCKET_ERROR) {
		if(recvLen == sizeof(HostRequest) && net->hReq.identifier == NETWORK_CLIENT_ID) {
			sockTo.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			sendto(net->sBHost, (const char*)&net->hInfo, sizeof(HostInfo), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
		}
		recvLen = recvfrom(net->sBHost, (char*)&net->hReq, sizeof(HostRequest), 0, 0, 0);
	}
	net->is_creating_host = false;
	shutdown(net->sBHost, SD_BOTH);
	closesocket(net->sBHost);
	return 0;
}
int NetManager::BroadcastClient(void* np) {
	NetManager* net = (NetManager*)np;
	SOCKADDR_IN sockTo;
	sockTo.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	sockTo.sin_family = AF_INET;
	sockTo.sin_port = htons(7913);
	fd_set fds;
	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(net->sBClient, &fds);
	sendto(net->sBClient, (const char*)&net->hReq, sizeof(HostRequest), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
	mainGame->is_refreshing = true;
	int result = select(0, &fds, 0, 0, &tv);
	std::set<int> addrset;
	net->hosts.clear();
	SOCKADDR_IN sockFrom;
	int sz = sizeof(SOCKADDR_IN);
	if(mainGame->is_closing)
		return 0;
	wchar_t tbuf[256];
	const wchar_t* mode;
	std::vector<HostInfo>::iterator it;
	mainGame->gMutex.Lock();
	for(it = net->hosts.begin(); it != net->hosts.end(); ++it) {
		if(!it->no_check_deck && !it->no_shuffle_deck && it->start_lp == 8000 && it->start_hand == 5 && it->draw_count == 1)
			mode = L"标准设定";
		else mode = L"自定义设定";
	}
	mainGame->gMutex.Unlock();
	mainGame->is_refreshing = false;
	closesocket(net->sBClient);
	return 0;
}
*/
}
