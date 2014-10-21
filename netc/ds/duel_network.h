#ifndef _DUEL_NETWORK_H_
#define _DUEL_NETWORK_H_

#include "../../common/tcpclient.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    class DuelProtoTcp : public DuelProtoHandler, public TcpClientSeed {
    public:
        DuelProtoTcp();
        virtual void BeginProto();
        virtual void GetProto();
        virtual bool ProtoEnd();
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size);
        
    protected:
        std::wstring server_ip;
        unsigned int server_port = 0;
        unsigned int server_timeout = 0;
        bool proto_ended = false;
    };
    
}

#endif
