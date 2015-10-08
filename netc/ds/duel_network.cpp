#include "utils/common.h"

#include "../config.h"

#include "duel_network.h"

namespace ygopro
{
    
    void DuelNetwork::Init(int16_t max_proto_count) {
        server_ip = commonCfg["server_ip"].to_string();
        server_port = (int32_t)commonCfg["server_port"].to_integer();
        server_timeout = (int32_t)commonCfg["server_timeout"].to_integer();
        retry_interval = (int32_t)commonCfg["retry_interval"].to_integer();
        SetConnectionHandler(this);
        proto_handlers.resize(max_proto_count);
    }
    
    void DuelNetwork::Uninit() {
        proto_handlers.clear();
        objs.clear();
    }
    
    void DuelNetwork::Begin() {
        //Connect(To<std::string>(server_ip).c_str(), server_port, server_timeout);
    }
    
    void DuelNetwork::End() {
        
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
        if(proto >= (uint32_t)proto_handlers.size())
            return;
        auto& plist = proto_handlers[proto];
        for(auto iter = plist.begin(); iter != plist.end();) {
            auto cur = iter++;
            if(cur->pending_remove)
                plist.erase(cur);
            else
                cur->network_cb(proto, data, sz);
        }
    }
    
    void DuelNetwork::RegisterStatusHandler(status_callback scb, void* obj) {
        
    }
    
    void DuelNetwork::RegisterPacketHandler(uint16_t proto, network_callback ncb, void* obj) {
        if(proto >= (uint32_t)proto_handlers.size())
            return;
        proto_handlers[proto].emplace_back();
        auto& pt = proto_handlers[proto].back();
        pt.network_cb = ncb;
        pt.object = obj;
        objs[obj].insert(proto);
    }
    
    void DuelNetwork::RemovePacketHandler(void* obj) {
        auto iter = objs.find(obj);
        if(iter == objs.end())
            return;
        for(auto proto : iter->second) {
            for(auto& hinfo : proto_handlers[proto]) {
                if(hinfo.object == obj)
                    hinfo.pending_remove = true;
            }
        }
        objs.erase(iter);
    }
    
}
