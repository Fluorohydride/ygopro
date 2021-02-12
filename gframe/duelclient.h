#ifndef DUELCLIENT_H
#define DUELCLIENT_H

#include "config.h"
#include <vector>
#include <deque>
#include <set>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <fmt/format.h>
#include "network.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "random_fwd.h"
#include "replay.h"

namespace ygo {

class DuelClient {
private:
	static uint32_t connect_state;
	static std::vector<uint8_t> response_buf;
	static uint32_t watching;
	static uint8_t selftype;
	static bool is_host;
	static event_base* client_base;
	static bufferevent* client_bev;
	static std::vector<uint8_t> duel_client_read;
	static std::vector<uint8_t> duel_client_write;
	static bool is_closing;
	static uint64_t select_hint;
	static std::wstring event_string;
	static bool is_swapping;
	static bool stop_threads;
	static std::deque<std::vector<uint8_t>> to_analyze;
	static std::mutex analyzeMutex;
	static std::mutex to_analyze_mutex;
	static std::thread parsing_thread;
	static std::thread client_thread;
	static std::condition_variable cv;
public:
	static randengine rnd;
	static uint32_t temp_ip;
	static uint16_t temp_port;
	static uint16_t temp_ver;
	static bool try_needed;
	static bool is_local_host;
	static std::atomic<bool> answered;

	static inline std::pair<uint32_t, uint16_t> ResolveServer(epro::wstringview address, epro::wstringview port) {
		return ResolveServer(BufferIO::EncodeUTF8s(address), BufferIO::EncodeUTF8s(port));
	}
	static std::pair<uint32_t, uint16_t> ResolveServer(epro::stringview address, epro::stringview port);
	static inline std::pair<uint32_t, uint16_t> ResolveServer(epro::wstringview address, int port) {
		return ResolveServer(BufferIO::EncodeUTF8s(address), fmt::to_string(port));
	}
	static bool StartClient(uint32_t ip, uint16_t port, uint32_t gameid = 0, bool create_game = true);
	static void ConnectTimeout(evutil_socket_t fd, short events, void* arg);
	static void StopClient(bool is_exiting = false);
	static void ClientRead(bufferevent* bev, void* ctx);
	static void ClientEvent(bufferevent *bev, short events, void *ctx);
	static int ClientThread();
	static void HandleSTOCPacketLan(char* data, uint32_t len);
	static void HandleSTOCPacketLan2(char* data, uint32_t len);
	static void ParserThread();
	static bool CheckReady();
	static std::pair<uint32_t, uint32_t> GetPlayersCount();
	static ReplayStream replay_stream;
	static Replay last_replay;
	static int ClientAnalyze(char* msg, uint32_t len);
	static int GetSpectatorsCount() {
		return watching;
	};
	static void SwapField();
	static bool IsConnected() {
		return !!connect_state;
	};
	static void SetResponseI(int respI);
	static void SetResponseB(void* respB, uint32_t len);
	static void SendResponse();
	static void SendPacketToServer(uint8_t proto) {
		if(!client_bev)
			return;
		duel_client_write.clear();
		BufferIO::insert_value<uint16_t>(duel_client_write, 1);
		BufferIO::insert_value<uint8_t>(duel_client_write, proto);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}
	template<typename ST>
	static void SendPacketToServer(uint8_t proto, ST& st) {
		if(!client_bev)
			return;
		duel_client_write.clear();
		BufferIO::insert_value<uint16_t>(duel_client_write, 1 + sizeof(ST));
		BufferIO::insert_value<uint8_t>(duel_client_write, proto);
		BufferIO::insert_value<ST>(duel_client_write, st);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}
	static void SendBufferToServer(uint8_t proto, void* buffer, size_t len) {
		if(!client_bev)
			return;
		duel_client_write.clear();
		BufferIO::insert_value<uint16_t>(duel_client_write, (uint16_t)(1 + len));
		BufferIO::insert_value<uint8_t>(duel_client_write, proto);
		BufferIO::insert_data(duel_client_write, buffer, len);
		bufferevent_write(client_bev, duel_client_write.data(), duel_client_write.size());
	}

	static void ReplayPrompt(bool need_header = false);
	
protected:
	static bool is_refreshing;
	static int match_kill;
	static event* resp_event;
	static std::set<uint32_t> remotes;
public:
	static std::vector<HostPacket> hosts;
	static void BeginRefreshHost();
	static int RefreshThread(event_base* broadev);
	static void BroadcastReply(evutil_socket_t fd, short events, void* arg);
};

}

#endif //DUELCLIENT_H
