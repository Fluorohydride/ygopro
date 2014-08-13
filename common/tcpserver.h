#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "packet_handler.h"
#include "event2/event.h"
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <unordered_set>

class TcpServer;

class ClientConnection: public PacketStream {
    
    friend class TcpServer;
    
protected:
    TcpServer* server = nullptr;
    bufferevent* bev = nullptr;
    void* object = nullptr;
    unsigned int address = 0;
    
public:
    
    virtual ~ClientConnection() {}
    
    // connection event
    virtual void Connected() {}
    virtual void Disconnected() {}
    
    virtual void SendPacket(unsigned short proto, void* buffer, unsigned short len) {
        unsigned short buf[4];
        buf[0] = 2 + len;
        buf[1] = proto;
        bufferevent_write(bev, buf, 4);
        bufferevent_write(bev, buffer, len);
    }
    
    virtual void SendPacket(PacketWriter& pkt) {
        unsigned short len = pkt.PacketSize();
        bufferevent_write(bev, &len, 2);
        bufferevent_write(bev, pkt.PacketBuffer(), len);
    }
    
    // default packet format:
    // |  packet_size |              packet               |
    // |              |  proto(16 bits)  |      data      |
    // |    16 bits   |          packet_size bits         |
    // packet_size must less then 1022
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
    
};

class ClientConnectionSeed: public ClientConnection {
    
protected:
    unsigned int response_seed = 0;
    
public:
    // packet format:
    // |  packet_size |      response_seed     |              packet               |
    // |              |                        |  proto(16 bit)  |       data      |
    // |    16 bits   |         32 bits        |          packet_size bits         |
    // packet_size must less then 1024
    virtual void HandleData(bufferevent* bev) {
        evbuffer* input = bufferevent_get_input(bev);
        size_t len = evbuffer_get_length(input);
        unsigned short packet_len = 0;
        while(len >= 6) {
            evbuffer_copyout(input, &packet_len, 2);
            if(len < (size_t)(packet_len + 6))
                return;
            unsigned char buffer[2048];
            evbuffer_remove(input, buffer, packet_len + 6);
            unsigned int resp_seed = *(unsigned int*)&buffer[2];
            if(packet_len && (resp_seed == response_seed)) {
                HandlePacket(*(unsigned short*)&buffer[6], &buffer[8], packet_len - 2);
                response_seed = (resp_seed * 0x1fd45cc2 + 0xdee89c3a) ^ 0xe7262ca9;
            }
            len -= packet_len + 6;
        }
    }
};

class TcpServer {
    
protected:
    event_base* loop_base = nullptr;
    evconnlistener* listener = nullptr;
    std::unordered_set<ClientConnection*> connections;
    
public:
    TcpServer() {
        loop_base = event_base_new();
    }
    
    ~TcpServer() {
        event_base_free(loop_base);
    }
    
    void Run(unsigned short port) {
        sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
        sin.sin_port = htons(port);
        listener = evconnlistener_new_bind(loop_base, AcceptCallback, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, (sockaddr*)&sin, sizeof(sin));
        if(!listener)
            return;
        evconnlistener_set_error_cb(listener, AcceptErrorCallback);
        event_base_dispatch(loop_base);
        for(auto bit : connections) {
            bufferevent_disable(bit->bev, EV_READ);
            bufferevent_free(bit->bev);
        }
        connections.clear();
        evconnlistener_free(listener);
        listener = 0;
    }
    
    void DisconnectClient(ClientConnection* conh) {
        if(conh->server != this)
            return;
        conh->Disconnected();
        bufferevent_disable(conh->bev, EV_READ);
        bufferevent_free(conh->bev);
        connections.erase(conh);
        delete conh;
    }
    
    virtual ClientConnection* NewConnection() = 0;
    
    static void AcceptCallback(evconnlistener* listener, evutil_socket_t fd, sockaddr* address, int socklen, void* ctx) {
        TcpServer* server = (TcpServer*)ctx;
        ClientConnection* conh = server->NewConnection();
        conh->address = ((sockaddr_in*)address)->sin_addr.s_addr;
        conh->server = server;
        conh->bev = bufferevent_socket_new(server->loop_base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(conh->bev, ReadCallback, nullptr, EventCallback, conh);
        bufferevent_enable(conh->bev, EV_READ);
        conh->Connected();
    }
    
    static void AcceptErrorCallback(evconnlistener* listener, void* ctx) {
        TcpServer* server = (TcpServer*)ctx;
        event_base_loopexit(server->loop_base, 0);
    }
    
    static void ReadCallback(bufferevent* bev, void* ctx) {
        ClientConnection* conh = (ClientConnection*) ctx;
        conh->HandleData(bev);
    }
    
    static void EventCallback(bufferevent *bev, short events, void *ctx) {
        ClientConnection* conh = (ClientConnection*) ctx;
        if(events & BEV_EVENT_EOF) {
            conh->server->DisconnectClient(conh);
        } else if(events & BEV_EVENT_ERROR) {
            conh->server->DisconnectClient(conh);
        } else if(events & BEV_EVENT_TIMEOUT) {
            conh->server->DisconnectClient(conh);
        }
    }
    
};

#endif
