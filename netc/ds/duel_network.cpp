#include "../../common/common.h"

#include "../gui_extra.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    void DuelSceneHandler::OnConnected() {
        
    }
    
    void DuelSceneHandler::OnConnectError() {
        MessageBox::ShowOK(L"", L"connection error.", [this]() {
            std::cout << "end" << std::endl;
        });
    }
    
    void DuelSceneHandler::OnConnectTimeOut() {
        MessageBox::ShowOK(L"", L"connection timeout.", [this]() {
            std::cout << "end" << std::endl;
        });
    }
    
    void DuelSceneHandler::OnDisconnected() {
        
    }
    
    void DuelSceneHandler::HandlePacket(unsigned short proto, unsigned char data[], unsigned int size) {
        
    }
    
}
