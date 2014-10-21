#include "../../common/common.h"

#include "../gui_extra.h"
#include "duel_network.h"

namespace ygopro
{
    
    DuelProtoTcp::DuelProtoTcp() {
        server_ip = static_cast<std::wstring>(commonCfg["server_ip"]);
        server_port = commonCfg["server_port"];
        server_timeout = commonCfg["server_timeout"];
    }
    
    void DuelProtoTcp::BeginProto() {
        //Connect(To<std::string>(server_ip).c_str(), server_port, server_timeout);
    }
    
    void DuelProtoTcp::GetProto() {
        PullEvent();
    }
    
    bool DuelProtoTcp::ProtoEnd() {
        return proto_ended;
    }
    
    void DuelProtoTcp::OnConnected() {
        
    }
    
    void DuelProtoTcp::OnConnectError() {
        MessageBox::ShowOK(L"", L"connection error.", [this]() {
            std::cout << "end" << std::endl;
            proto_ended = true;
        });
    }
    
    void DuelProtoTcp::OnConnectTimeOut() {
        MessageBox::ShowOK(L"", L"connection timeout.", [this]() {
            std::cout << "end" << std::endl;
            proto_ended = true;
        });
    }
    
    void DuelProtoTcp::OnDisconnected() {
        
    }
    
    void DuelProtoTcp::HandlePacket(unsigned short proto, unsigned char data[], unsigned int size) {
        
    }
    
}
