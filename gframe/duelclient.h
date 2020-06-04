#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include "config.h"
#include <vector>
#include <set>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "random_fwd.h"
#include "replay.h"

namespace ygo {

class DuelClient {
private:
	static unsigned int connect_state;
	static std::vector<unsigned char> response_buf;
	static unsigned int watching;
	static unsigned char selftype;
	static bool is_host;
	static event_base* client_base;
	static bufferevent* client_bev;
	static std::vector<uint8_t> duel_client_read;
	static std::vector<uint8_t> duel_client_write;
	static bool is_closing;
	static uint64_t select_hint;
	static std::wstring event_string;
	static bool is_swapping;
public:
	static randengine rnd;
	static unsigned int temp_ip;
	static unsigned short temp_port;
	static unsigned short temp_ver;
	static bool try_needed;
	static bool is_local_host;

	static std::pair<unsigned int, unsigned short> ResolveServer(const std::wstring& address, const std::wstring& port);
	static std::pair<unsigned int, unsigned short> ResolveServer(const std::wstring& address, int port);
	static bool StartClient(unsigned int ip, unsigned short port, unsigned int gameid = 0, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(char* data, unsigned int len);
	static bool CheckReady();
	static void SetPlayersCount();
	static std::pair<int, int> GetPlayersCount();
	static ReplayStream replay_stream;
	static Replay last_replay;
	static int ClientAnalyze(char* msg, unsigned int len);
	static int GetSpectatorsCount() {
		return watching;
	};
	static void SwapField();
	static bool IsConnected() {
		return !!connect_state;
	};
	static void SetResponseI(int respI);
	static void SetResponseB(void* respB, unsigned int len);
	static void SendResponse();
	static void SendPacketToServer(unsigned char proto) {
		duel_client_write.clear();
		BufferIO::insert_value<int16_t>(duel_client_write, 1);
		BufferIO::insert_value<int8_t>(duel_client_write, proto);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}
	template<typename ST>
	static void SendPacketToServer(unsigned char proto, ST& st) {
		duel_client_write.clear();
		BufferIO::insert_value<int16_t>(duel_client_write, 1 + sizeof(ST));
		BufferIO::insert_value<int8_t>(duel_client_write, proto);
		BufferIO::insert_value<ST>(duel_client_write, st);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}
	static void SendBufferToServer(unsigned char proto, void* buffer, size_t len) {
		duel_client_write.clear();
		BufferIO::insert_value<int16_t>(duel_client_write, 1 + len);
		BufferIO::insert_value<int8_t>(duel_client_write, proto);
		BufferIO::insert_data(duel_client_write, buffer, len);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}

	static void ReplayPrompt(bool need_header = false);
	
protected:
	static bool is_refreshing;
	static int match_kill;
	static event* resp_event;
	static std::set<unsigned int> remotes;
public:
	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};

}

#endif //DUELCLIENT_H
