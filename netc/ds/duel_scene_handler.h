#ifndef _DUEL_INPUT_HANDLER_H_
#define _DUEL_INPUT_HANDLER_H_

#include "../../common/tcpclient.h"
#include "../scene_mgr.h"

namespace ygopro
{
        
    class DuelScene;
    class DuelCommand;
    
    class DuelSceneHandler : public SceneHandler, public Timer<double>, public TcpClientSeed {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual void UpdateEvent();
        virtual void BeginHandler();
        
        virtual void OnConnected();
        virtual void OnConnectError();
        virtual void OnConnectTimeOut();
        virtual void OnDisconnected();
        virtual void HandlePacket(unsigned short proto, unsigned char data[], unsigned int size);
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        CommandList<DuelCommand> duel_commands;
    };
    
}

#endif
