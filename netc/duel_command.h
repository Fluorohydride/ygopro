#ifndef _DUEL_COMMAND_H_
#define _DUEL_COMMAND_H_

namespace ygopro
{
    
    struct FieldBlock;
    struct FieldCard;
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
        DuelCommandMove(std::shared_ptr<FieldCard> pcard, int side, int loc, int seq, int pos);
        virtual bool Handle(DuelScene* pscene);
        
        std::shared_ptr<FieldCard> moving_card;
        unsigned int to[4];
    };
}

#endif
