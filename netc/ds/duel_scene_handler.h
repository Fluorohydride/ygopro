#ifndef _DUEL_SCENE_HANDLER_H_
#define _DUEL_SCENE_HANDLER_H_

#include "../scene_mgr.h"
#include "duel_command.h"

namespace ygopro
{
        
    class DuelScene;
    
    class DuelProtoHandler : public CommandList<DuelCommand> {
    public:
        virtual void BeginProto() = 0;
        virtual void GetProto() = 0;
        virtual bool ProtoEnd() = 0;
        virtual int MessageToCmd(unsigned int sz);
        
    protected:
        std::vector<unsigned char> msg_buffer;
    };
    
    class DuelSceneHandler : public SceneHandler, public Timer<double> {
    public:
        DuelSceneHandler(std::shared_ptr<DuelScene> pscene);
        virtual bool UpdateEvent();
        virtual void BeginHandler();
        template<typename T>
        void SetProtoHandler(T ph) {
            proto_handler = std::static_pointer_cast<DuelProtoHandler>(ph);
        }
        
    protected:
        std::weak_ptr<DuelScene> duel_scene;
        std::shared_ptr<DuelProtoHandler> proto_handler;
    };
    
}

#endif
