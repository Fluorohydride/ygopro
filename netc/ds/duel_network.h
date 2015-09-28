#ifndef _DUEL_NETWORK_H_
#define _DUEL_NETWORK_H_

#include "utils/tcpclient.h"

namespace ygopro
{
    enum class NetworkStatus {
        Connected = 1,
        ConnectError = 2,
        ConnectTimeout = 3,
        Disconnected = 4,
    };
    
    class DuelNetwork : public TcpClientSeed, public ConnectionHandler {
    public:
        using status_callback = std::function<void(NetworkStatus)>;
        using network_callback = std::function<void(uint16_t, uint8_t*, size_t)>;
        void Init();
        void Uninit();
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(uint16_t proto, uint8_t data[], size_t sz);
        
        void RegisterPacketHandler(status_callback scb, network_callback ncb, void* obj);
        
    protected:
        std::string server_ip;
        uint32_t server_port = 0;
        uint32_t server_timeout = 0;
    };
    
}

#endif
