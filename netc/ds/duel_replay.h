//#ifndef _DUEL_REPLAY_H_
//#define _DUEL_REPLAY_H_
//
//#include "../../ocgcore/ocgapi.h"
//
//#include "duel_scene_handler.h"
//
//#define REPLAY_COMPRESSED   0x1
//#define REPLAY_TAG          0x2
//#define REPLAY_RAWDATA      0x4
//#define REPLAY_DECDATA      0x8
//
//#define REPLAY_RPY1         0x31707279 // "yrp1"
//#define REPLAY_RPY2         0x32707279 // "yrp2"
//
//namespace ygopro
//{
//
//    struct ReplayHeader {
//        uint32_t id;
//        uint32_t version;
//        uint32_t flag;
//        uint32_t seed;
//        uint32_t data_size;
//        uint32_t hash;
//        uint8_t props[8];
//    };
//    
//    struct ReplayHeader2 {
//        uint32_t id;
//        uint32_t version;
//        uint32_t flag;
//        uint32_t raw_size;
//        uint32_t raw_size_d;
//        uint32_t dec_size;
//        uint32_t dec_size_d;
//        uint8_t props[8];
//        uint32_t inner_id;
//        uint32_t reserved[2];
//    };
//    
//    class DuelProtoReplay : public DuelProtoHandler, public ygoAdapter {
//    public:
//        DuelProtoReplay(const std::string& file);
//        ~DuelProtoReplay();
//        virtual void BeginProto();
//        virtual void EndProto();
//        virtual bool IsEnd();
//        void ReplayLoop();
//        
//        virtual byte* ReadScript(const char* script_name, int32_t* len);
//        virtual uint32 ReadCard(uint32 code, card_data* data);
//        virtual uint32 HandleMessage(void* msg, uint32 message_type);
//        
//    protected:
//        TextFile script_file;
//        uint8_t* rep_data = nullptr;
//        size_t rep_size = 0;
//        uint32_t seed = 0;
//        uint32_t flag = 0;
//        std::shared_ptr<duelAdapter> replay_duel;
//        bool replay_end = false;
//    };
//    
//}
//
//#endif
