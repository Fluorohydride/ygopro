#ifndef _DUEL_NETWORK_H_
#define _DUEL_NETWORK_H_

#include "../../common/tcpclient.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    class DuelProtoTcp : public DuelProtoHandler, public TcpClientSeed, public ConnectionHandler {
    public:
        DuelProtoTcp();
        virtual void BeginProto();
        virtual void GetProto();
        virtual bool ProtoEnd();
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(uint16_t proto, uint8_t data[], size_t sz);
        
    protected:
        std::wstring server_ip;
        uint32_t server_port = 0;
        uint32_t server_timeout = 0;
        bool proto_ended = false;
    };
    
}

#endif
