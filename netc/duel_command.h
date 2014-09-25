#ifndef _DUEL_COMMAND_H_
#define _DUEL_COMMAND_H_

namespace ygopro
{
    
    class DuelScene;
    
    struct DuelCommand {
        virtual bool Handle(DuelScene* pscene) = 0;
    };
    
    struct DuelCommandWait : public DuelCommand {
        DuelCommandWait(double tm);
        virtual bool Handle(DuelScene* pscene);

        double end_time;
    };
    
    struct DuelCommandMove : public DuelCommand {
        DuelCommandMove(unsigned int f, unsigned int t);
        virtual bool Handle(DuelScene* pscene);
        
        unsigned char from[4];
        unsigned char to[4];
    };
}

#endif
