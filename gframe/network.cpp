#include "network.h"
#include "game.h"

namespace ygo {

const unsigned short PROTO_VERSION = 0x1016;

bool NetManager::CreateHost() {
	wchar_t* pstr;
	int wp;
	hInfo.identifier = NETWORK_SERVER_ID;
	hInfo.version = PROTO_VERSION;
	hInfo.address = mainGame->netManager.local_addr;
	for(wp = 0, pstr = (wchar_t*)mainGame->ebServerName->getText(); wp < 19 && pstr[wp]; ++wp)
		hInfo.name[wp] = pstr[wp];
	hInfo.port = serv_port;
	hInfo.name[wp] = 0;
	hInfo.no_check_deck = mainGame->chkNoCheckDeck->isChecked();
	hInfo.no_shuffle_deck = mainGame->chkNoShuffleDeck->isChecked();
	hInfo.no_shuffle_player = mainGame->chkNoShufflePlayer->isChecked();
	hInfo.attack_ft = mainGame->chkAttackFT->isChecked();
	hInfo.no_chain_hint = mainGame->chkNoChainHint->isChecked();
	hInfo.start_lp = _wtoi(mainGame->ebStartLP->getText());
	hInfo.start_hand = _wtoi(mainGame->ebStartHand->getText());
	hInfo.draw_count = _wtoi(mainGame->ebDrawCount->getText());
	hInfo.is_match = mainGame->cbMatchMode->getSelected() == 0 ? false : true;
	hInfo.lfindex = mainGame->cbLFlist->getSelected();
	hInfo.time_limit = mainGame->cbTurnTime->getSelected();
	for(wp = 0, pstr = (wchar_t*)mainGame->cbLFlist->getItem(mainGame->cbLFlist->getSelected()); wp < 19 && pstr[wp]; ++wp)
		hInfo.lflist[wp] = pstr[wp];
	hInfo.lflist[wp] = 0;
	sBHost = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBHost == INVALID_SOCKET)
		return false;
	BOOL opt = TRUE;
	setsockopt(sBHost, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
	SOCKADDR_IN local;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
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
bool NetManager::RefreshHost() {
	sBClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sBClient == INVALID_SOCKET)
		return false;
	BOOL opt = TRUE;
	setsockopt(sBClient, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(BOOL));
	SOCKADDR_IN local;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
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
	return *(int*)host->h_addr_list[0];
}
int NetManager::BroadcastServer(void* np) {
	NetManager* net = (NetManager*)np;
	SOCKADDR_IN sockTo;
	sockTo.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	sockTo.sin_family = AF_INET;
	sockTo.sin_port = htons(7912);
	int recvLen = recvfrom(net->sBHost, (char*)&net->hReq, sizeof(HostRequest), 0, 0, 0);
	while(recvLen != 0 && recvLen != SOCKET_ERROR) {
		if(recvLen == sizeof(HostRequest) && net->hReq.identifier == NETWORK_CLIENT_ID)
			sendto(net->sBHost, (const char*)&net->hInfo, sizeof(HostInfo), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
		recvLen = recvfrom(net->sBHost, (char*)&net->hReq, sizeof(HostRequest), 0, 0, 0);
	}
	net->is_creating_host = false;
	shutdown(net->sBHost, SD_BOTH);
	closesocket(net->sBHost);
	if(!mainGame->is_closing) {
		mainGame->gMutex.Lock();
		mainGame->btnLanStartServer->setEnabled(true);
		mainGame->btnLanCancelServer->setEnabled(false);
		mainGame->btnLanConnect->setEnabled(true);
		mainGame->btnRefreshList->setEnabled(true);
		mainGame->btnLoadReplay->setEnabled(true);
		mainGame->btnDeckEdit->setEnabled(true);
		mainGame->gMutex.Unlock();
	}
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
	tv.tv_sec = 0;
	tv.tv_usec = 500000;
	FD_ZERO(&fds);
	FD_SET(net->sBClient, &fds);
	sendto(net->sBClient, (const char*)&net->hReq, sizeof(HostRequest), 0, (sockaddr*)&sockTo, sizeof(sockaddr));
	mainGame->lstServerList->clear();
	mainGame->is_refreshing = true;
	int result = select(0, &fds, 0, 0, &tv);
	std::set<int> addrset;
	net->hosts.clear();
	while(result != 0 && result != SOCKET_ERROR) {
		int recvLen = recvfrom(net->sBClient, (char*)&net->hInfo, sizeof(HostInfo), 0, 0, 0);
		if(recvLen == sizeof(HostInfo) && net->hInfo.identifier == NETWORK_SERVER_ID
		        && net->hInfo.version == PROTO_VERSION && addrset.find(net->hInfo.address) == addrset.end()) {
			net->hosts.push_back(net->hInfo);
		}
		result = select(0, &fds, 0, 0, &tv);
	}
	if(mainGame->is_closing)
		return 0;
	wchar_t tbuf[256];
	const wchar_t* mode;
	std::vector<HostInfo>::iterator it;
	mainGame->gMutex.Lock();
	for(it = net->hosts.begin(); it != net->hosts.end(); ++it) {
		if(!it->no_check_deck && !it->no_shuffle_deck && !it->no_shuffle_deck && !it->attack_ft && !it->no_chain_hint
		        && it->start_lp == 8000 && it->start_hand == 5 && it->draw_count == 1)
			mode = L"标准设定";
		else mode = L"自定义设定";
		myswprintf(tbuf, L"[%ls][%ls]%ls", mode, it->lflist, it->name);
		mainGame->lstServerList->addItem(tbuf);
	}
	mainGame->btnLanStartServer->setEnabled(true);
	mainGame->btnLanConnect->setEnabled(true);
	mainGame->btnRefreshList->setEnabled(true);
	mainGame->btnLoadReplay->setEnabled(true);
	mainGame->btnDeckEdit->setEnabled(true);
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
		if(mode == 1 && (net->hInfo.no_chain_hint || net->hInfo.no_check_deck || net->hInfo.no_shuffle_deck || net->hInfo.no_shuffle_player || net->hInfo.attack_ft
		                 || net->hInfo.start_lp != 8000 || net->hInfo.start_hand != 5 || net->hInfo.draw_count != 1 )) {
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
		if(!net->hInfo.no_check_deck && !mainGame->deckManager.CheckLFList(mainGame->deckManager.deckclient, net->hInfo.lfindex)) {
			psbuf = net->send_buf;
			WriteInt8(psbuf, 0x2);
			send(net->sRemote, net->send_buf, 1, 0);
			closesocket(net->sRemote);
			net->sRemote = accept(net->sListen, 0, 0);
			continue;
		}
		psbuf = net->send_buf;
		WriteInt8(psbuf, 0);
		const wchar_t* ln = mainGame->ebUsername->getText();
		int li = 0;
		while(ln[li] && li < 19) {
			mainGame->dInfo.hostname[li] = ln[li];
			WriteInt16(psbuf, ln[li++]);
		}
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
		mainGame->stModeStatus->setText(L"");
		mainGame->dInfo.engLen = 0;
		mainGame->dInfo.msgLen = 0;
		mainGame->dField.Clear();
		mainGame->HideElement(mainGame->wModeSelection);
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
			mainGame->btnLanStartServer->setEnabled(true);
			mainGame->btnLanConnect->setEnabled(true);
			mainGame->btnRefreshList->setEnabled(true);
			mainGame->btnLoadReplay->setEnabled(true);
			mainGame->btnDeckEdit->setEnabled(true);
			mainGame->stModeStatus->setText(L"无法连接至主机");
		}
		return 0;
	}
	char* pbuf = pnet->send_buf;
	NetManager::WriteInt16(pbuf, PROTO_VERSION);
	NetManager::WriteInt16(pbuf, mainGame->chkStOnly->isChecked() ? 1 : 0);
	const wchar_t* pname = mainGame->ebJoinPass->getText();
	int i = 0;
	while(pname[i] != 0 && i < 19)
		NetManager::WriteInt16(pbuf, pname[i++]);
	NetManager::WriteInt16(pbuf, 0);
	i = 0;
	pname = mainGame->ebUsername->getText();
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
		if(!mainGame->is_closing) {
			mainGame->btnLanStartServer->setEnabled(true);
			mainGame->btnLanConnect->setEnabled(true);
			mainGame->btnRefreshList->setEnabled(true);
			mainGame->btnLoadReplay->setEnabled(true);
			mainGame->btnDeckEdit->setEnabled(true);
			if(result == SOCKET_ERROR)
				mainGame->stModeStatus->setText(L"网络连接发生错误");
			else if(pnet->recv_buf[0] == 0x1) {
				wchar_t errorbuf[32];
				myswprintf(errorbuf, L"当前版本(0x%X)与主机版本(0x%X)不匹配", PROTO_VERSION, (int)(*(short*)&pnet->recv_buf[1]));
				mainGame->stModeStatus->setText(errorbuf);
			} else if(pnet->recv_buf[0] == 0x2)
				mainGame->stModeStatus->setText(L"无效卡组或者卡组不符合禁卡表规范");
			else if(pnet->recv_buf[0] == 0x3)
				mainGame->stModeStatus->setText(L"密码错误");
			else if(pnet->recv_buf[0] == 0x4)
				mainGame->stModeStatus->setText(L"主机非标准对战模式，拒绝连接");
			else mainGame->stModeStatus->setText(L"未知错误");
		}
		return 0;
	}
	mainGame->stModeStatus->setText(L"");
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
	mainGame->lstServerList->clear();
	mainGame->stModeStatus->setText(L"");
	mainGame->dInfo.engLen = 0;
	mainGame->dInfo.msgLen = 0;
	mainGame->dInfo.is_local_host = false;
	mainGame->dField.Clear();
	mainGame->btnLanStartServer->setEnabled(true);
	mainGame->btnLanConnect->setEnabled(true);
	mainGame->btnRefreshList->setEnabled(true);
	mainGame->btnLoadReplay->setEnabled(true);
	mainGame->btnDeckEdit->setEnabled(true);
	mainGame->HideElement(mainGame->wModeSelection);
	mainGame->gMutex.Unlock();
	mainGame->WaitFrameSignal(10);
	Thread::NewThread(Game::RecvThread, &mainGame->dInfo);
	Thread::NewThread(Game::GameThread, &mainGame->dInfo);
}
}
