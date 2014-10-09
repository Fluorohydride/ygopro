#ifndef _DUEL_COMMAND_H_
#define _DUEL_COMMAND_H_

namespace ygopro
{
    
    struct FieldBlock;
    struct FieldCard;
    class DuelScene;
    
    class DuelCommand {
    public:
        virtual bool Handle(DuelScene* pscene) = 0;
    };
    
    class DuelCommandWait : public DuelCommand {
    public:
        DuelCommandWait(double tm);
        virtual bool Handle(DuelScene* pscene);
        
    protected:
        double end_time;
    };
    
    class DuelMessage : public DuelCommand {
    public:
        DuelMessage(unsigned char* data, unsigned int len) {
            pdata = data;
            length = len;
        }
        
    protected:
        unsigned char* pdata;
        int length;
    };
    
    class DuelMessageMove : public DuelMessage {
    public:
        using DuelMessage::DuelMessage;
        virtual bool Handle(DuelScene* pscene);
    };
}

#endif
