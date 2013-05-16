#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include "common.h"
#include "packet_handler.h"
#include "event2/event.h"
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#define TC_CONNECT_ESTABLISHED		1
#define TC_CONNECT_ERROR			2
#define TC_CONNECT_TIMEOUT			3
#define TC_CONNECT_DISCONNECTED		4
#define TC_CONNECT_RECONNECTED		5
#define TC_CONNECT_REC_ERROR		6
#define TC_CONNECT_REC_TIMEOUT		7

#define TC_FLAG_EXTERNAL_BASE		0x1

class TcpClient : public PacketStream {

protected:
	event_base* loop_base;
	bufferevent* client_bev;
	event* timer_event;
	unsigned short flag;
	unsigned short connection_status;
	unsigned int ipaddr;
	unsigned short port;

public:
	TcpClient(event_base* external_base = nullptr) {
		flag = 0;
		if(external_base) {
			loop_base = external_base;
			flag |= TC_FLAG_EXTERNAL_BASE;
		} else
			loop_base = event_base_new();
		client_bev = nullptr;
		timer_event = nullptr;
	}

	virtual ~TcpClient() {
		if(!TC_FLAG_EXTERNAL_BASE)
			event_base_free(loop_base);
	}

	void Connect(const char* ip, unsigned short port, unsigned int timeout_ms) {
		ipaddr = inet_addr(ip);
		this->port = port;
		sockaddr_in sin;
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = ipaddr;
		sin.sin_port = htons(port);
		client_bev = bufferevent_socket_new(loop_base, -1, BEV_OPT_CLOSE_ON_FREE);
		bufferevent_setcb(client_bev, ReadCallback, nullptr, EventCallback, (void*)this);
		if (bufferevent_socket_connect(client_bev, (sockaddr*)&sin, sizeof(sin)) < 0) {
			bufferevent_free(client_bev);
			return;
		}
		if(timeout_ms) {
			timeval timeout = {timeout_ms / 1000, timeout_ms % 1000};
			timer_event = event_new(loop_base, 0, EV_TIMEOUT, TimeoutCallback, this);
			event_add(timer_event, &timeout);
		}
	}

	void CheckNetwork() {
		event_base_loop(loop_base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}

	bool IsConnected() {
		return (connection_status == TC_CONNECT_ESTABLISHED) || (connection_status == TC_CONNECT_RECONNECTED);
	}

	void Shutdown() {
		if(timer_event) {
			event_del(timer_event);
			timer_event = nullptr;
		}
		if(client_bev) {
			bufferevent_free(client_bev);
			client_bev = nullptr;
		}
	}

	virtual void ConnectEvent() = 0;

	virtual void SendPacket(unsigned char proto, void* buffer, size_t len) {
		unsigned char buf[3];
		*(unsigned short*)buf = 1 + len;
		buf[2] = proto;
		bufferevent_write(client_bev, buf, 3);
		bufferevent_write(client_bev, buffer, len);
	}

	virtual void SendPacket(PacketWriter& pkt) {
		unsigned char buf[2];
		*(unsigned short*)buf = pkt.PacketSize();
		bufferevent_write(client_bev, buf, 2);
		bufferevent_write(client_bev, pkt.PacketBuffer(), pkt.PacketSize());
	}

	// default packet format: 
	// |  packet_size |              packet               |
	// |              |   proto(8bit)   |       data      |
	// |    16 bits   |          packet_size bits         |
	virtual void HandleData(bufferevent* bev) {
		evbuffer* input = bufferevent_get_input(bev);
		size_t len = evbuffer_get_length(input);
		unsigned short packet_len = 0;
		while(len >= 2) {
			evbuffer_copyout(input, &packet_len, 2);
			if(len < (size_t)(packet_len + 2))
				return;
			unsigned char buffer[2048];
			evbuffer_remove(input, buffer, packet_len + 2);
			if(packet_len)
				HandlePacket(buffer[2], &buffer[3], packet_len - 1);
			len -= packet_len + 2;
		}
	}

	// handle a single packet
	virtual void HandlePacket(unsigned char proto, unsigned char data[], unsigned int size) = 0;

	static void ReadCallback(bufferevent* bev, void* ctx) {
		TcpClient* client = (TcpClient*)ctx;
		client->HandleData(bev);
	}

	static void EventCallback(bufferevent *bev, short events, void *ctx) {
		TcpClient* client = (TcpClient*)ctx;
		if(events & BEV_EVENT_CONNECTED) {
			if(client->timer_event) {
				event_del(client->timer_event);
				client->timer_event = nullptr;
			}
			bufferevent_enable(client->client_bev, EV_READ);
			client->connection_status = TC_CONNECT_ESTABLISHED;
			client->ConnectEvent();
		} else if(events & BEV_EVENT_EOF) {
			client->connection_status = TC_CONNECT_DISCONNECTED;
			client->ConnectEvent();
			client->Shutdown();
		} else if(events & BEV_EVENT_ERROR) {
			client->connection_status = TC_CONNECT_ERROR;
			client->ConnectEvent();
			client->Shutdown();
		}
	}

	static void TimeoutCallback(evutil_socket_t fd, short events, void* arg) {
		TcpClient* client = (TcpClient*)arg;
		client->connection_status = TC_CONNECT_TIMEOUT;
		client->ConnectEvent();
		client->timer_event = nullptr;
		if(client->client_bev) {
			bufferevent_free(client->client_bev);
			client->client_bev = nullptr;
		}
	}

};

#endif
