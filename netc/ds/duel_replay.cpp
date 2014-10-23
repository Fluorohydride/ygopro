#include "../../common/common.h"
#include "../../buildin/LzmaSDK.h"

#include "duel_replay.h"

namespace ygopro
{
    
    DuelProtoReplay::DuelProtoReplay(const std::string& file) {
        std::ifstream rep_file(file, std::ios::in | std::ios::binary);
        rep_file.seekg(std::ios::end);
        size_t comp_size = (size_t)rep_file.tellg() - sizeof(ReplayHeader);
        rep_file.seekg(std::ios::beg);
        ReplayHeader rep_header;
        rep_file.read((char*)&rep_header, sizeof(ReplayHeader));
        rep_size = rep_header.data_size;
        rep_data = new unsigned char[rep_size];
        seed = rep_header.seed;
        flag = rep_header.flag;
        if(rep_header.flag & REPLAY_COMPRESSED) {
            unsigned char* comp_data = new unsigned char[comp_size];
            rep_file.read((char*)comp_data, comp_size);
            rep_size = rep_header.data_size;
            if(LzmaUncompress(rep_data, &rep_size, comp_data, &comp_size, rep_header.props, 5) != SZ_OK) {
                delete[] rep_data;
                rep_data = nullptr;
                rep_size = 0;
                return;
            }
            delete[] comp_data;
        } else {
            rep_file.read((char*)rep_data, comp_size);
        }
    }
    
    DuelProtoReplay::~DuelProtoReplay() {
        if(rep_data)
            delete[] rep_data;
    }
    
    void DuelProtoReplay::BeginProto() {
        replay_duel = std::make_shared<duelAdapter>(seed);
        replay_duel->set_adapter(this);
        BufferUtil rep_reader(rep_data);
        if(flag & REPLAY_TAG) {
            rep_reader.Skip(160);
//            cur_replay.ReadData(mainGame->dInfo.hostname, 40);
//            cur_replay.ReadData(mainGame->dInfo.hostname_tag, 40);
//            cur_replay.ReadData(mainGame->dInfo.clientname_tag, 40);
//            cur_replay.ReadData(mainGame->dInfo.clientname, 40);
        } else {
            rep_reader.Skip(80);
//            cur_replay.ReadData(mainGame->dInfo.hostname, 40);
//            cur_replay.ReadData(mainGame->dInfo.clientname, 40);
        }
        int start_lp = rep_reader.Read<uint32>();
        int start_hand = rep_reader.Read<uint32>();
        int draw_count = rep_reader.Read<uint32>();
        int opt = rep_reader.Read<uint32>();
        replay_duel->set_player_info(0, start_lp, start_hand, draw_count);
        replay_duel->set_player_info(1, start_lp, start_hand, draw_count);
        if(!(opt & DUEL_TAG_MODE)) {
            int main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 0, 0, LOCATION_DECK, 0, 0);
            int extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 0, 0, LOCATION_EXTRA, 0, 0);
            //mainGame->dField.Initial(0, main, extra);
            main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 1, 1, LOCATION_DECK, 0, 0);
            extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 1, 1, LOCATION_EXTRA, 0, 0);
            //mainGame->dField.Initial(1, main, extra);
        } else {
            int main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 0, 0, LOCATION_DECK, 0, 0);
            int extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 0, 0, LOCATION_EXTRA, 0, 0);
            //mainGame->dField.Initial(0, main, extra);
            main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_tag_card(rep_reader.Read<uint32>(), 0, LOCATION_DECK);
            extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_tag_card(rep_reader.Read<uint32>(), 0, LOCATION_EXTRA);
            main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 1, 1, LOCATION_DECK, 0, 0);
            extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_card(rep_reader.Read<uint32>(), 1, 1, LOCATION_EXTRA, 0, 0);
            //mainGame->dField.Initial(1, main, extra);
            main = rep_reader.Read<uint32>();
            for(int i = 0; i < main; ++i)
                replay_duel->new_tag_card(rep_reader.Read<uint32>(), 1, LOCATION_DECK);
            extra = rep_reader.Read<uint32>();
            for(int i = 0; i < extra; ++i)
                replay_duel->new_tag_card(rep_reader.Read<uint32>(), 1, LOCATION_EXTRA);
        }
        replay_duel->start_duel(opt);
    }
    
    void DuelProtoReplay::GetProto() {
        
    }
    
    bool DuelProtoReplay::ProtoEnd() {
        return false;
    }
    
    byte* DuelProtoReplay::ReadScript(const char* script_name, int* len) {
        return nullptr;
    }
    
    uint32 DuelProtoReplay::ReadCard(uint32 code, card_data* data) {
        return 0;
    }
    
    uint32 DuelProtoReplay::HandleMessage(void* msg, uint32 message_type) {
        return 0;
    }
    
}
