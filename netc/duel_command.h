#ifndef _DUEL_COMMAND_H_
#define _DUEL_COMMAND_H_

namespace ygopro
{
    
    struct DuelCommand {
        virtual void Handle() = 0;
        double wait_time = 0.0;
        std::function<void ()> cb;
    };
    
}

#endif
