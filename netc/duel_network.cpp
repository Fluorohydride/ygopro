#include "../common/common.h"

#include "gui_extra.h"
#include "duel_scene.h"

namespace ygopro
{
    
    void DuelScene::OnConnected() {
        
    }
    
    void DuelScene::OnConnectError() {
        MessageBox::ShowOK(L"", L"connection error.", [this]() {
            std::cout << "end" << std::endl;
        });
    }
    
    void DuelScene::OnConnectTimeOut() {
        MessageBox::ShowOK(L"", L"connection timeout.", [this]() {
            std::cout << "end" << std::endl;
        });
    }
    
    void DuelScene::OnDisconnected() {
        
    }
    
    void DuelScene::HandlePacket(unsigned short proto, unsigned char data[], unsigned int size) {
        
    }
    
}
