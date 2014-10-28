#include "../../common/common.h"

#include "../scene_mgr.h"

#include "duel_scene.h"
#include "duel_command.h"

namespace ygopro
{
    
    DuelCommandWait::DuelCommandWait(double tm) {
        end_time = SceneMgr::Get().GetGameTime() + tm;
    }
    
    bool DuelCommandWait::Handle(std::shared_ptr<DuelScene> pscene) {
        return SceneMgr::Get().GetGameTime() > end_time;
    }
    
    DuelMessageInit::DuelMessageInit(unsigned char* pdata, unsigned int len) {
        init_infos.resize(len);
        memcpy(&init_infos[0], pdata, len);
    }
    
    bool DuelMessageInit::Handle(std::shared_ptr<DuelScene> pscene) {
        BufferUtil reader(&init_infos[0]);
        for(int i = 0; i < 2; ++i) {
            int hp = reader.Read<int>();
            for(int i = 0; i < 5; ++i) {
                unsigned char exist_card = reader.Read<unsigned char>();
                if(exist_card) {
                    unsigned char pos = reader.Read<unsigned char>();
                    unsigned char sz = reader.Read<unsigned char>();
                }
            }
            for(int i = 0; i < 8; ++i) {
                unsigned char exist_card = reader.Read<unsigned char>();
                if(exist_card) {
                    unsigned char pos = reader.Read<unsigned char>();
                }
            }
            unsigned char main_sz = reader.Read<unsigned char>();
            unsigned char hand_sz = reader.Read<unsigned char>();
            unsigned char grave_sz = reader.Read<unsigned char>();
            unsigned char remove_sz = reader.Read<unsigned char>();
            unsigned char extra_sz = reader.Read<unsigned char>();
        }
        return true;
    }
    
    DuelMessageMove::DuelMessageMove(unsigned char* pdata, unsigned int len) {
        memcpy(&move_infos[0], pdata, 8);
    }
    
    bool DuelMessageMove::Handle(std::shared_ptr<DuelScene> pscene) {
        auto moving_card = pscene->GetCard(move_infos[0], move_infos[1], move_infos[2], move_infos[3]);
        if(moving_card)
            pscene->MoveCard(moving_card, move_infos[4], move_infos[5], move_infos[6], move_infos[7], false, 1.0);
        return true;
    }
    
}
