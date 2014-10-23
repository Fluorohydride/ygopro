#ifndef _DUEL_REPLAY_H_
#define _DUEL_REPLAY_H_

#include "../../ocgcore/ocgapi.h"

#include "duel_scene_handler.h"

#define REPLAY_COMPRESSED   0x1
#define REPLAY_TAG          0x2
#define REPLAY_RAWDATA      0x4
#define REPLAY_DECDATA      0x8

#define REPLAY_RPY1         0x31707279 // "yrp1"
#define REPLAY_RPY2         0x32707279 // "yrp2"

namespace ygopro
{

    struct ReplayHeader {
        unsigned int id;
        unsigned int version;
        unsigned int flag;
        unsigned int seed;
        unsigned int data_size;
        unsigned int hash;
        unsigned char props[8];
    };
    
    struct ReplayHeader2 {
        unsigned int id;
        unsigned int version;
        unsigned int flag;
        unsigned int raw_size;
        unsigned int raw_size_d;
        unsigned int dec_size;
        unsigned int dec_size_d;
        unsigned char props[8];
        unsigned int inner_id;
        unsigned int reserved[2];
    };
    
    class DuelProtoReplay : public DuelProtoHandler, public ygoAdapter {
    public:
        DuelProtoReplay(const std::string& file);
        ~DuelProtoReplay();
        virtual void BeginProto();
        virtual void GetProto();
        virtual bool ProtoEnd();
        
        virtual byte* ReadScript(const char* script_name, int* len);
        virtual uint32 ReadCard(uint32 code, card_data* data);
        virtual uint32 HandleMessage(void* msg, uint32 message_type);
        
    protected:
        unsigned char* rep_data = nullptr;
        size_t rep_size = 0;
        unsigned int seed = 0;
        unsigned int flag = 0;
        std::shared_ptr<duelAdapter> replay_duel;
    };
    
}

#endif
