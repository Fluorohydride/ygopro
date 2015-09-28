#include "utils/common.h"

#include "../config.h"

#include "duel_network.h"

namespace ygopro
{
    
    void DuelNetwork::Init() {
        server_ip = commonCfg["server_ip"].to_string();
        server_port = (uint32_t)commonCfg["server_port"].to_integer();
        server_timeout = (uint32_t)commonCfg["server_timeout"].to_integer();
        SetConnectionHandler(this);
        //Connect(To<std::string>(server_ip).c_str(), server_port, server_timeout);
    }
    
    void DuelNetwork::Uninit() {
        
    }
    
    void DuelNetwork::OnConnected() {
        
    }
    
    void DuelNetwork::OnConnectError() {

    }
    
    void DuelNetwork::OnConnectTimeOut() {

    }
    
    void DuelNetwork::OnDisconnected() {
        
    }
    
    void DuelNetwork::HandlePacket(uint16_t proto, uint8_t data[], size_t sz) {
        
    }
    
    void DuelNetwork::RegisterPacketHandler(status_callback scb, network_callback ncb, void* obj) {
        
    }
    
}
