#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include "packet_handler.h"
#include "event2/event.h"
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#define TC_FLAG_EXTERNAL_BASE         0x1

class TcpClient : public PacketStream {
    
protected:
    event_base* loop_base = nullptr;
    bufferevent* client_bev = nullptr;
    event* timer_event = nullptr;
    unsigned short flag = 0;
    unsigned short connection_status = 0;
    unsigned int ipaddr = 0;
    unsigned short port = 0;
    
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
    
    void Connect(const char* ip, unsigned short pt, unsigned int timeout_ms) {
        ipaddr = inet_addr(ip);
        port = pt;
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
            timeval timeout = {(long)(timeout_ms / 1000), (int)(timeout_ms % 1000)};
            timer_event = event_new(loop_base, 0, EV_TIMEOUT, TimeoutCallback, this);
            event_add(timer_event, &timeout);
        }
    }
    
    void PullEvent() {
        event_base_loop(loop_base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
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
    
    virtual void OnConnected() = 0;
    virtual void OnConnectError() = 0;
    virtual void OnConnectTimeOut() = 0;
    virtual void OnDisconnected() = 0;
    
    virtual void SendPacket(unsigned short proto, void* buffer, unsigned short len) {
        unsigned short buf[2];
        buf[0] = 2 + len;
        buf[1] = proto;
        bufferevent_write(client_bev, buf, 4);
        bufferevent_write(client_bev, buffer, len);
    }
    
    virtual void SendPacket(PacketWriter& pkt) {
        unsigned short len = pkt.PacketSize();
        bufferevent_write(client_bev, &len, 2);
        bufferevent_write(client_bev, pkt.PacketBuffer(), pkt.PacketSize());
    }
    
    // default packet format:
    // |  packet_size |              packet               |
    // |              |  proto(16 bit)  |       data      |
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
                HandlePacket(*(unsigned short*)&buffer[2], &buffer[4], packet_len - 2);
            len -= packet_len + 2;
        }
    }
    
    // handle a single packet
    virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size) = 0;
    
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
            bufferevent_enable(client->client_bev, EV_READ | EV_WRITE);
            client->OnConnected();
        } else if(events & BEV_EVENT_EOF) {
            client->OnDisconnected();
            client->Shutdown();
        } else if(events & BEV_EVENT_ERROR) {
            client->OnConnectError();
            client->Shutdown();
        }
    }
    
    static void TimeoutCallback(evutil_socket_t fd, short events, void* arg) {
        TcpClient* client = (TcpClient*)arg;
        client->OnConnectTimeOut();
        client->timer_event = nullptr;
        if(client->client_bev) {
            bufferevent_free(client->client_bev);
            client->client_bev = nullptr;
        }
    }
    
};

class TcpClientSeed : public TcpClient {
public:
    virtual void SendPacket(unsigned short proto, void* buffer, unsigned short len) {
        unsigned short buf[4];
        buf[0] = 2 + len;
        *(unsigned int*)&buf[1] = response_seed;
        buf[3] = proto;
        bufferevent_write(client_bev, buf, 8);
        bufferevent_write(client_bev, buffer, len);
        response_seed = (response_seed * 0x1fd45cc2 + 0xdee89c3a) ^ 0xe7262ca9;
    }
    
    virtual void SendPacket(PacketWriter& pkt) {
        unsigned short len = pkt.PacketSize();
        bufferevent_write(client_bev, &len, 2);
        bufferevent_write(client_bev, &response_seed, 4);
        bufferevent_write(client_bev, pkt.PacketBuffer(), pkt.PacketSize());
        response_seed = (response_seed * 0x1fd45cc2 + 0xdee89c3a) ^ 0xe7262ca9;
    }
protected:
    unsigned int response_seed = 0;
};

#endif
