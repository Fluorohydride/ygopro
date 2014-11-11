#include "../../common/common.h"

#include "duel_network.h"

namespace ygopro
{
    
    DuelProtoTcp::DuelProtoTcp() {
        server_ip = static_cast<std::wstring>(commonCfg["server_ip"]);
        server_port = commonCfg["server_port"];
        server_timeout = commonCfg["server_timeout"];
    }
    
    void DuelProtoTcp::BeginProto() {
        SetConnectionHandler(this);
        //Connect(To<std::string>(server_ip).c_str(), server_port, server_timeout);
    }
    
    void DuelProtoTcp::EndProto() {
        
    }
    
    void DuelProtoTcp::OnConnected() {
        
    }
    
    void DuelProtoTcp::OnConnectError() {

    }
    
    void DuelProtoTcp::OnConnectTimeOut() {

    }
    
    void DuelProtoTcp::OnDisconnected() {
        
    }
    
    void DuelProtoTcp::HandlePacket(uint16_t proto, uint8_t data[], size_t sz) {
        
    }
    
}
