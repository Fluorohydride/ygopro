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
    
    class DuelMessageInit : public DuelCommand {
    public:
        DuelMessageInit(unsigned char* data, unsigned int len);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        std::vector<unsigned char> init_infos;
    };
    
    class DuelMessageMove : public DuelCommand {
    public:
        DuelMessageMove(unsigned char* data, unsigned int len);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        std::array<unsigned char, 8> move_infos;
    };
}

#endif
