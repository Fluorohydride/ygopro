#ifndef _DUEL_COMMAND_H_
#define _DUEL_COMMAND_H_

namespace ygopro
{
    
    struct FieldBlock;
    struct FieldCard;
    class DuelScene;
    
    class DuelCommand {
    public:
        virtual bool Handle(std::shared_ptr<DuelScene> pscene) = 0;
        
    };
    
    class DuelCommandWait : public DuelCommand {
    public:
        DuelCommandWait(double tm);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
        
    protected:
        double end_time;
    };
    
    class DuelCommandMsg : public DuelCommand {
    public:
        DuelCommandMsg(uint8_t msg_type, BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
        
    protected:
        uint8_t msg_type = 0;
        BufferUtil* reader = nullptr;
    };
    
}

#endif
