#include "network.h"
#include "game.h"

namespace ygo {

const unsigned short PROTO_VERSION = 0x1020;

bool NetManager::CreateHost(int ipindex) {
	wchar_t* pstr;
	int wp;
	sBHost = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBHost == INVALID_SOCKET)
		return false;
	BOOL opt = TRUE;
	setsockopt(sBHost, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
	SOCKADDR_IN local;
	if(ipindex == -1)
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		local.sin_addr.s_addr = local_addr[ipindex];
	local.sin_family = AF_INET;
	local.sin_port = htons(7913);
	if(::bind(sBHost, (sockaddr*)&local, sizeof(sockaddr)) == SOCKET_ERROR) {
		closesocket(sBHost);
		return false;
	}
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sListen == INVALID_SOCKET) {
		closesocket(sBHost);
		return false;
	}
	local.sin_port = htons(serv_port);
	if(::bind(sListen, (sockaddr*)&local, sizeof(sockaddr)) == SOCKET_ERROR) {
		closesocket(sBHost);
		closesocket(sListen);
		return false;
	}
	is_creating_host = true;
	Thread::NewThread(BroadcastServer, this);
	Thread::NewThread(ListenThread, this);
	return true;
}
bool NetManager::CancelHost() {
	if(!is_creating_host)
		return false;
	closesocket(sBHost);
	closesocket(sListen);
	is_creating_host = false;
}
bool NetManager::RefreshHost(int ipindex) {
	sBClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBClient == INVALID_SOCKET)
		return false;
	BOOL opt = TRUE;
	setsockopt(sBClient, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
	SOCKADDR_IN local;
	if(ipindex == -1)
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		local.sin_addr.s_addr = local_addr[ipindex];
	local.sin_family = AF_INET;
	local.sin_port = htons(7912);
	hReq.identifier = NETWORK_CLIENT_ID;
	if(::bind(sBClient, (sockaddr*)&local, sizeof(sockaddr)) == SOCKET_ERROR) {
		closesocket(sBClient);
		return false;
	}
	Thread::NewThread(BroadcastClient, this);
	return true;
}
bool NetManager::JoinHost() {
	sRemote = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sRemote == INVALID_SOCKET) {
		closesocket(sRemote);
		return false;
	}
	char ip[20];
	int i = 0;
	wchar_t* pstr = (wchar_t *)mainGame->ebJoinIP->getText();
	while(*pstr && i < 16) {
		ip[i++] = *pstr;
		*pstr++;
	}
	ip[i] = 0;
	remote_addr = inet_addr(ip);
	remote_port = _wtoi(mainGame->ebJoinPort->getText());
	Thread::NewThread(JoinThread, (void*)this);
	return true;
}
bool NetManager::SendtoRemote(char* buf, int len) {
	int result = send(sRemote, buf, len, 0);
	if( result == SOCKET_ERROR)
		return false;
	return true;
}
bool NetManager::WaitClientResponse() {
	bool retry = false;
	do {
		int result = recv(sRemote, recv_buf, 2048, 0);
		if(result == 0) {
			if(!mainGame->is_closing) {
				mainGame->gMutex.Lock();
				mainGame->stACMessage->setText(L"与对方的连接中断");
				mainGame->dInfo.netError = true;
				mainGame->gMutex.Unlock();
			}
			return false;
		}
		if(result == SOCKET_ERROR) {
			if(!mainGame->is_closing) {
				mainGame->gMutex.Lock();
				mainGame->stACMessage->setText(L"网络连接发生错误");
				mainGame->dInfo.netError = true;
				mainGame->gMutex.Unlock();
				return false;
			}
		}
		char* pbuf = recv_buf;
		int type = NetManager::ReadInt8(pbuf);
		mainGame->dInfo.is_responsed = false;
		if(type == 1) {
			mainGame->dInfo.responseI = NetManager::ReadInt32(pbuf);
			mainGame->SetResponseI();
		} else if(type == 2) {
			int len = NetManager::ReadInt8(pbuf);
			for(int i = 0; i < len; ++i)
				mainGame->dInfo.responseB[i] = NetManager::ReadInt8(pbuf);
			mainGame->SetResponseB(len);
		} else {
			mainGame->SendByte(mainGame->dInfo.resPlayer, 1);
			retry = true;
		}
	} while(retry);
	return true;
}
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
int NetManager::ListenThread(void* np) {
	NetManager* net = (NetManager*)np;
	if(listen(net->sListen, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(net->sListen);
		return 0;
	}
	int recvlen, index;
	char* pbuf, *psbuf;
	net->sRemote = accept(net->sListen, 0, 0);
	while(net->sRemote != SOCKET_ERROR) {
		recvlen = recv(net->sRemote, net->recv_buf, 4096, 0);
		if(recvlen == SOCKET_ERROR) {
			closesocket(net->sRemote);
			net->sRemote = accept(net->sListen, 0, 0);
			continue;
		}
		//check deck
		pbuf = net->recv_buf;
		int ver = ReadInt16(pbuf);
		if(ver != PROTO_VERSION) {
			psbuf = net->send_buf;
			WriteInt8(psbuf, 0x1);
			WriteInt16(psbuf, PROTO_VERSION);
			send(net->sRemote, net->send_buf, 3, 0);
			closesocket(net->sRemote);
			net->sRemote = accept(net->sListen, 0, 0);
			continue;
		}
		int mode = ReadInt16(pbuf);
		if(mode == 1 && (net->hInfo.no_check_deck || net->hInfo.no_shuffle_deck || net->hInfo.start_lp != 8000 || net->hInfo.start_hand != 5 || net->hInfo.draw_count != 1 )) {
			psbuf = net->send_buf;
			WriteInt8(psbuf, 0x4);
			send(net->sRemote, net->send_buf, 1, 0);
			closesocket(net->sRemote);
			net->sRemote = accept(net->sListen, 0, 0);
			continue;
		}
		wchar_t cn = ReadInt16(pbuf);
		int off = 0;
		while(cn != 0 && off < 19) {
			mainGame->dInfo.pass[off++] = cn;
			cn = ReadInt16(pbuf);
		}
		mainGame->dInfo.pass[off] = 0;
		if(wcscmp(mainGame->dInfo.pass, mainGame->ebServerPass->getText())) {
			psbuf = net->send_buf;
			WriteInt8(psbuf, 0x3);
			send(net->sRemote, net->send_buf, 1, 0);
			closesocket(net->sRemote);
			net->sRemote = accept(net->sListen, 0, 0);
			continue;
		}
		cn = ReadInt16(pbuf);
		off = 0;
		while(cn != 0 && off < 19) {
			mainGame->dInfo.clientname[off++] = cn;
			cn = ReadInt16(pbuf);
		}
		mainGame->dInfo.clientname[off] = 0;
		int maincount = ReadInt16(pbuf);
		int sidecount = ReadInt16(pbuf);
		mainGame->deckManager.LoadDeck(mainGame->deckManager.deckclient, (int*)pbuf, maincount, sidecount);
		psbuf = net->send_buf;
		WriteInt8(psbuf, 0);
		int li = 0;
//		while(ln[li] && li < 19) {
//			mainGame->dInfo.hostname[li] = ln[li];
//			WriteInt16(psbuf, ln[li++]);
//		}
		mainGame->dInfo.hostname[li] = 0;
		WriteInt16(psbuf, 0);
		send(net->sRemote, net->send_buf, 3 + li * 2, 0);
		mainGame->gMutex.Lock();
		mainGame->imgCard->setImage(mainGame->imageManager.tCover);
		mainGame->wCardImg->setVisible(true);
		mainGame->wInfos->setVisible(true);
		mainGame->stName->setText(L"");
		mainGame->stInfo->setText(L"");
		mainGame->stDataInfo->setText(L"");
		mainGame->stText->setText(L"");
		mainGame->dInfo.engLen = 0;
		mainGame->dInfo.msgLen = 0;
		mainGame->dField.Clear();

		mainGame->gMutex.Unlock();
		mainGame->WaitFrameSignal(10);
		closesocket(net->sBHost);
		net->is_creating_host = false;
		Thread::NewThread(Game::EngineThread, &mainGame->dInfo);
		Thread::NewThread(Game::GameThread, &mainGame->dInfo);
		break;
	}
	return 0;
}
int NetManager::JoinThread(void* adr) {
	SOCKADDR_IN server;
	NetManager* pnet = (NetManager*)adr;
	server.sin_addr.s_addr = pnet->remote_addr;
	server.sin_family = AF_INET;
	server.sin_port = htons(pnet->remote_port);
	if(connect(pnet->sRemote, (sockaddr*)&server, sizeof(sockaddr)) == SOCKET_ERROR) {
		closesocket(pnet->sRemote);
		if(!mainGame->is_closing) {
//			mainGame->stModeStatus->setText(L"无法连接至主机");
		}
		return 0;
	}
	char* pbuf = pnet->send_buf;
	NetManager::WriteInt16(pbuf, PROTO_VERSION);
	NetManager::WriteInt16(pbuf, 1);
	const wchar_t* pname = mainGame->ebJoinPass->getText();
	int i = 0;
	while(pname[i] != 0 && i < 19)
		NetManager::WriteInt16(pbuf, pname[i++]);
	NetManager::WriteInt16(pbuf, 0);
	i = 0;
//	pname = mainGame->ebUsername->getText();
	while(pname[i] != 0 && i < 19) {
		mainGame->dInfo.hostname[i] = pname[i];
		NetManager::WriteInt16(pbuf, pname[i++]);
	}
	NetManager::WriteInt16(pbuf, 0);
	mainGame->dInfo.hostname[i] = 0;
	NetManager::WriteInt16(pbuf, mainGame->deckManager.deckhost.main.size() + mainGame->deckManager.deckhost.extra.size());
	NetManager::WriteInt16(pbuf, mainGame->deckManager.deckhost.side.size());
	for(int i = 0; i < mainGame->deckManager.deckhost.main.size(); ++i)
		NetManager::WriteInt32(pbuf, mainGame->deckManager.deckhost.main[i]->first);
	for(int i = 0; i < mainGame->deckManager.deckhost.extra.size(); ++i)
		NetManager::WriteInt32(pbuf, mainGame->deckManager.deckhost.extra[i]->first);
	for(int i = 0; i < mainGame->deckManager.deckhost.side.size(); ++i)
		NetManager::WriteInt32(pbuf, mainGame->deckManager.deckhost.side[i]->first);
	pnet->SendtoRemote(pnet->send_buf, pbuf - pnet->send_buf);
	int result = recv(pnet->sRemote, pnet->recv_buf, 4096, 0);
	if(result == SOCKET_ERROR || pnet->recv_buf[0] != 0) {
		closesocket(pnet->sRemote);
	}
	wchar_t* pn = (wchar_t*)&pnet->recv_buf[1];
	int pi = 0;
	while(pn[pi] && pi < 19) {
		mainGame->dInfo.clientname[pi] = pn[pi];
		pi++;
	}
	mainGame->dInfo.clientname[pi] = 0;
	mainGame->gMutex.Lock();
	mainGame->imgCard->setImage(mainGame->imageManager.tCover);
	mainGame->wCardImg->setVisible(true);
	mainGame->wInfos->setVisible(true);
	mainGame->stName->setText(L"");
	mainGame->stInfo->setText(L"");
	mainGame->stDataInfo->setText(L"");
	mainGame->stText->setText(L"");
//	mainGame->stModeStatus->setText(L"");
	mainGame->dInfo.engLen = 0;
	mainGame->dInfo.msgLen = 0;
	mainGame->dInfo.is_local_host = false;
	mainGame->dField.Clear();
	mainGame->gMutex.Unlock();
	mainGame->WaitFrameSignal(10);
	Thread::NewThread(Game::RecvThread, &mainGame->dInfo);
	Thread::NewThread(Game::GameThread, &mainGame->dInfo);
}
}
