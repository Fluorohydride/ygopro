//#ifndef _DUEL_COMMAND_H_
//#define _DUEL_COMMAND_H_
//
//namespace ygopro
//{
//    
//    struct FieldBlock;
//    struct FieldCard;
//    class DuelScene;
//    
//    class DuelCmd {
//    public:
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene) = 0;
//        
//    };
//    
//    class DuelCmdWait : public DuelCmd {
//    public:
//        DuelCmdWait(double tm);
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
//        
//    protected:
//        double end_time;
//    };
//    
//    class DuelCmdMsg : public DuelCmd {
//    public:
//        DuelCmdMsg(uint8_t msg_type, BufferUtil& reader);
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
//        
//    protected:
//        uint8_t msg_type = 0;
//        BufferUtil* reader = nullptr;
//    };
//    
//    class DuelCmdRefreshPos : public DuelCmd {
//    public:
//        DuelCmdRefreshPos(std::shared_ptr<FieldCard> ptr, bool update, float tm);
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
//    protected:
//        std::shared_ptr<FieldCard> pcard;
//        bool update = false;
//        float update_tm = 0.0f;
//    };
//    
//    class DuelCmdMove : public DuelCmd {
//    public:
//        DuelCmdMove(std::shared_ptr<FieldCard> ptr, uint32_t code, int32_t con, int32_t loc, int32_t seq, int32_t subs, uint32_t reason);
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
//    protected:
//        std::shared_ptr<FieldCard> pcard;
//        uint32_t code;
//        int32_t con;
//        int32_t loc;
//        int32_t seq;
//        int32_t subs;
//        uint32_t reason;
//    };
//    
//    class DuelCmdDraw : public DuelCmd {
//    public:
//        DuelCmdDraw(uint32_t pl, uint32_t data);
//        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
//    protected:
//        uint32_t playerid;
//        uint32_t data;
//    };
//}
//
//#endif
