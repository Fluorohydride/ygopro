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
    
    using status_callback = std::function<void(NetworkStatus)>;
    using network_callback = std::function<void(uint16_t, uint8_t*, size_t)>;
    
    struct SHandlerInfo {
        network_callback network_cb;
        void* object = nullptr;
        bool pending_remove = false;
    };
    
    class DuelNetwork : public TcpClientSeed, public ConnectionHandler {
    public:
        void Init(int16_t max_proto_count);
        void Uninit();
        void Begin();
        void End();
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(uint16_t proto, uint8_t data[], size_t sz);
        
        void RegisterStatusHandler(status_callback scb, void* obj);
        void RegisterPacketHandler(uint16_t proto, network_callback ncb, void* obj);
        void RemovePacketHandler(void* obj);
        
    protected:
        std::string server_ip;
        int32_t server_port = 0;
        int32_t server_timeout = 0;
        int32_t retry_interval = 0;
        std::vector<std::list<SHandlerInfo>> proto_handlers;
        std::map<void*, std::set<int32_t>> objs;
    };
    
}

#endif
