#ifndef _DUEL_REPLAY_H_
#define _DUEL_REPLAY_H_

#include "duel_scene_handler.h"

namespace ygopro
{
    
    class DuelProtoReplay : public DuelProtoHandler {
    public:
        DuelProtoReplay();
        virtual void BeginProto();
        virtual void GetProto();
        virtual bool ProtoEnd();
        
    protected:
        
    };
    
}

#endif
