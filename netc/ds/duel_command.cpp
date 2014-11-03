#include "../../common/common.h"

#include "../../ocgcore/common.h"

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

    DuelMsgRetry::DuelMsgRetry(BufferUtil& reader) {
        
    }

    bool DuelMsgRetry::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgHint::DuelMsgHint(BufferUtil& reader) {
        hint_type = reader.Read<unsigned char>();
        reader.Skip(1);
        hint_data = reader.Read<unsigned int>();
    }
    
    bool DuelMsgHint::Handle(std::shared_ptr<DuelScene> pscene) {
        switch (hint_type) {
            case HINT_EVENT:
                break;
            case HINT_MESSAGE:
                break;
            case HINT_SELECTMSG:
                break;
            case HINT_OPSELECTED:
                break;
            case HINT_EFFECT:
                break;
            case HINT_RACE:
                break;
            case HINT_ATTRIB:
                break;
            case HINT_CODE:
                break;
            case HINT_NUMBER:
                break;
            case HINT_CARD:
                break;
            default:
                break;
        }
        return true;
    }
    
    DuelMsgStart::DuelMsgStart(BufferUtil& reader) {
        
    }
    
    bool DuelMsgStart::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgWin::DuelMsgWin(BufferUtil& reader) {
        win_player = reader.Read<unsigned char>();
        win_reason = reader.Read<unsigned char>();
    }
    
    bool DuelMsgWin::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgUpdateCard::DuelMsgUpdateCard(BufferUtil& reader) {
        unsigned int len = reader.Read<unsigned int>();
        update_buffer.resize(len);
        reader.Read(&update_buffer[0], len);
    }
    
    bool DuelMsgUpdateCard::Handle(std::shared_ptr<DuelScene> pscene) {
        BufferUtil reader(&update_buffer[0], update_buffer.size());
        return true;
    }
    
    DuelMsgSelBattleCmd::DuelMsgSelBattleCmd(BufferUtil& reader) {
        int playerid = reader.Read<unsigned char>();
        int act_sz = reader.Read<unsigned char>();
        for(int i = 0; i < act_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
            unsigned int desc = reader.Read<unsigned int>();
        }
        int atk_sz = reader.Read<unsigned char>();
        for(int i = 0; i < act_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
            unsigned int param = reader.Read<unsigned char>();
        }
        bool can_m2 = reader.Read<unsigned char>() == 1;
        bool can_ep = reader.Read<unsigned char>() == 1;
    }
    
    bool DuelMsgSelBattleCmd::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelMainCmd::DuelMsgSelMainCmd(BufferUtil& reader) {
        int playerid = reader.Read<unsigned char>();
        int summon_sz = reader.Read<unsigned char>();
        for(int i = 0; i < summon_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
        }
        int spsummon_sz = reader.Read<unsigned char>();
        for(int i = 0; i < spsummon_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
        }
        int repos_sz = reader.Read<unsigned char>();
        for(int i = 0; i < repos_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
        }
        int mset_sz = reader.Read<unsigned char>();
        for(int i = 0; i < mset_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
        }
        int sset_sz = reader.Read<unsigned char>();
        for(int i = 0; i < sset_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
        }
        int act_sz = reader.Read<unsigned char>();
        for(int i = 0; i < act_sz; ++i) {
            unsigned int code = reader.Read<unsigned int>();
            unsigned int con = reader.Read<unsigned char>();
            unsigned int loc = reader.Read<unsigned char>();
            unsigned int seq = reader.Read<unsigned char>();
            unsigned int desc = reader.Read<unsigned int>();
        }
        bool can_bp = reader.Read<unsigned char>() == 1;
        bool can_ep = reader.Read<unsigned char>() == 1;
    }
    
    bool DuelMsgSelMainCmd::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelEffectYN::DuelMsgSelEffectYN(BufferUtil& reader) {
        int playerid = reader.Read<unsigned char>();
        unsigned int code = reader.Read<unsigned int>();
        unsigned int info_loc = reader.Read<unsigned int>();
    }
    
    bool DuelMsgSelEffectYN::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelYesNo::DuelMsgSelYesNo(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelYesNo::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelOption::DuelMsgSelOption(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelOption::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelCard::DuelMsgSelCard(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelCard::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelChain::DuelMsgSelChain(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelChain::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelPlace::DuelMsgSelPlace(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelPlace::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelPosition::DuelMsgSelPosition(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelPosition::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelTribute::DuelMsgSelTribute(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelTribute::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSortChain::DuelMsgSortChain(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSortChain::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelCounter::DuelMsgSelCounter(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelCounter::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelSum::DuelMsgSelSum(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelSum::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSelField::DuelMsgSelField(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSelField::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSortCard::DuelMsgSortCard(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSortCard::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgConfirmDeck::DuelMsgConfirmDeck(BufferUtil& reader) {
        
    }
    
    bool DuelMsgConfirmDeck::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgConfirmCard::DuelMsgConfirmCard(BufferUtil& reader) {
        
    }
    
    bool DuelMsgConfirmCard::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgShuffleDeck::DuelMsgShuffleDeck(BufferUtil& reader) {
        
    }
    
    bool DuelMsgShuffleDeck::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgShuffleHand::DuelMsgShuffleHand(BufferUtil& reader) {
        
    }
    
    bool DuelMsgShuffleHand::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgRefreshDeck::DuelMsgRefreshDeck(BufferUtil& reader) {
        
    }
    
    bool DuelMsgRefreshDeck::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSwapGraveDeck::DuelMsgSwapGraveDeck(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSwapGraveDeck::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgShuffleSetCard::DuelMsgShuffleSetCard(BufferUtil& reader) {
        
    }
    
    bool DuelMsgShuffleSetCard::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgReverseDeck::DuelMsgReverseDeck(BufferUtil& reader) {
        
    }
    
    bool DuelMsgReverseDeck::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDeckTop::DuelMsgDeckTop(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDeckTop::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgNewTurn::DuelMsgNewTurn(BufferUtil& reader) {
        
    }
    
    bool DuelMsgNewTurn::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgNewPhase::DuelMsgNewPhase(BufferUtil& reader) {
        
    }
    
    bool DuelMsgNewPhase::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgMove::DuelMsgMove(BufferUtil& reader) {
        reader.Read(&move_infos[0], 8);
    }
    
    bool DuelMsgMove::Handle(std::shared_ptr<DuelScene> pscene) {
        auto moving_card = pscene->GetCard(move_infos[0], move_infos[1], move_infos[2], move_infos[3]);
        if(moving_card)
            pscene->MoveCard(moving_card, move_infos[4], move_infos[5], move_infos[6], move_infos[7], false, 1.0);
        return true;
    }
    
    DuelMsgPosChange::DuelMsgPosChange(BufferUtil& reader) {
        
    }
    
    bool DuelMsgPosChange::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSet::DuelMsgSet(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSet::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSwap::DuelMsgSwap(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSwap::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgFieldDisable::DuelMsgFieldDisable(BufferUtil& reader) {
        
    }
    
    bool DuelMsgFieldDisable::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSummoning::DuelMsgSummoning(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSummoning::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSummoned::DuelMsgSummoned(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSummoned::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSPSummoning::DuelMsgSPSummoning(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSPSummoning::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgSPSummoned::DuelMsgSPSummoned(BufferUtil& reader) {
        
    }
    
    bool DuelMsgSPSummoned::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgFlipSummoning::DuelMsgFlipSummoning(BufferUtil& reader) {
        
    }
    
    bool DuelMsgFlipSummoning::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgFlipSummoned::DuelMsgFlipSummoned(BufferUtil& reader) {
        
    }
    
    bool DuelMsgFlipSummoned::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChaining::DuelMsgChaining(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChaining::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChained::DuelMsgChained(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChained::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChainSolving::DuelMsgChainSolving(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChainSolving::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChainSolved::DuelMsgChainSolved(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChainSolved::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChainEnd::DuelMsgChainEnd(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChainEnd::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChainNegated::DuelMsgChainNegated(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChainNegated::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgChainDisabled::DuelMsgChainDisabled(BufferUtil& reader) {
        
    }
    
    bool DuelMsgChainDisabled::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCardSelected::DuelMsgCardSelected(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCardSelected::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgRandomSelected::DuelMsgRandomSelected(BufferUtil& reader) {
        
    }
    
    bool DuelMsgRandomSelected::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgBecomeTarget::DuelMsgBecomeTarget(BufferUtil& reader) {
        
    }
    
    bool DuelMsgBecomeTarget::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDraw::DuelMsgDraw(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDraw::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDamage::DuelMsgDamage(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDamage::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgRecover::DuelMsgRecover(BufferUtil& reader) {
        
    }
    
    bool DuelMsgRecover::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgEquip::DuelMsgEquip(BufferUtil& reader) {
        
    }
    
    bool DuelMsgEquip::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgLPUpdate::DuelMsgLPUpdate(BufferUtil& reader) {
        
    }
    
    bool DuelMsgLPUpdate::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgUnequip::DuelMsgUnequip(BufferUtil& reader) {
        
    }
    
    bool DuelMsgUnequip::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCardTarget::DuelMsgCardTarget(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCardTarget::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCancelTarget::DuelMsgCancelTarget(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCancelTarget::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgLPCost::DuelMsgLPCost(BufferUtil& reader) {
        
    }
    
    bool DuelMsgLPCost::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgAddCounter::DuelMsgAddCounter(BufferUtil& reader) {
        
    }
    
    bool DuelMsgAddCounter::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgRemoveCounter::DuelMsgRemoveCounter(BufferUtil& reader) {
        
    }
    
    bool DuelMsgRemoveCounter::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgAttack::DuelMsgAttack(BufferUtil& reader) {
        
    }
    
    bool DuelMsgAttack::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgBattle::DuelMsgBattle(BufferUtil& reader) {
        
    }
    
    bool DuelMsgBattle::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgAttackNegated::DuelMsgAttackNegated(BufferUtil& reader) {
        
    }
    
    bool DuelMsgAttackNegated::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDamageStepStart::DuelMsgDamageStepStart(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDamageStepStart::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDamageStepEnd::DuelMsgDamageStepEnd(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDamageStepEnd::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgMissEffect::DuelMsgMissEffect(BufferUtil& reader) {
        
    }
    
    bool DuelMsgMissEffect::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgBeChainTarget::DuelMsgBeChainTarget(BufferUtil& reader) {
        
    }
    
    bool DuelMsgBeChainTarget::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCreateRelation::DuelMsgCreateRelation(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCreateRelation::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgReleaseRelation::DuelMsgReleaseRelation(BufferUtil& reader) {
        
    }
    
    bool DuelMsgReleaseRelation::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgTossCoin::DuelMsgTossCoin(BufferUtil& reader) {
        
    }
    
    bool DuelMsgTossCoin::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgTossDice::DuelMsgTossDice(BufferUtil& reader) {
        
    }
    
    bool DuelMsgTossDice::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDeclearRace::DuelMsgDeclearRace(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDeclearRace::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDeclearAttrib::DuelMsgDeclearAttrib(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDeclearAttrib::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDeclearCard::DuelMsgDeclearCard(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDeclearCard::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgDeclearNumber::DuelMsgDeclearNumber(BufferUtil& reader) {
        
    }
    
    bool DuelMsgDeclearNumber::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCardHint::DuelMsgCardHint(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCardHint::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgTagSwap::DuelMsgTagSwap(BufferUtil& reader) {
        
    }
    
    bool DuelMsgTagSwap::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgLoad::DuelMsgLoad(BufferUtil& reader) {
        pinfos.resize(2);
        for(int p = 0; p < 2; ++p) {
            pinfos[p].hp = reader.Read<int>();
            for(int i = 0; i < 5; ++i) {
                unsigned int exist_card = reader.Read<unsigned char>();
                if(exist_card) {
                    unsigned int pos = reader.Read<unsigned char>();
                    unsigned int sz = reader.Read<unsigned char>();
                    pinfos[p].minfo[i] = pos | (sz << 8);
                } else
                    pinfos[p].minfo[i] = 0;
            }
            for(int i = 0; i < 8; ++i) {
                unsigned int exist_card = reader.Read<unsigned char>();
                if(exist_card)
                    pinfos[p].sinfo[i] = reader.Read<unsigned char>();
                else
                    pinfos[p].sinfo[i] = 0;
            }
            pinfos[p].main_size = reader.Read<unsigned char>();
            pinfos[p].hand_size = reader.Read<unsigned char>();
            pinfos[p].grave_size = reader.Read<unsigned char>();
            pinfos[p].remove_size = reader.Read<unsigned char>();
            pinfos[p].extra_size = reader.Read<unsigned char>();
        }
        int chainsz = reader.Read<unsigned char>();
        cinfos.resize(chainsz);
        for(int i = 0; i < chainsz; ++i) {
            cinfos[i].code = reader.Read<unsigned int>();
            cinfos[i].card_con = reader.Read<unsigned char>();
            cinfos[i].card_loc = reader.Read<unsigned char>();
            cinfos[i].card_seq = reader.Read<unsigned char>();
            cinfos[i].card_pos = reader.Read<unsigned char>();
            cinfos[i].trig_con = reader.Read<unsigned char>();
            cinfos[i].trig_loc = reader.Read<unsigned char>();
            cinfos[i].trig_seq = reader.Read<unsigned char>();
            cinfos[i].desc = reader.Read<unsigned int>();
        }
    }
    
    bool DuelMsgLoad::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgAIName::DuelMsgAIName(BufferUtil& reader) {
        
    }
    
    bool DuelMsgAIName::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgShowHint::DuelMsgShowHint(BufferUtil& reader) {
        
    }
    
    bool DuelMsgShowHint::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgMatchKill::DuelMsgMatchKill(BufferUtil& reader) {
        
    }
    
    bool DuelMsgMatchKill::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
    DuelMsgCustomMsg::DuelMsgCustomMsg(BufferUtil& reader) {
        
    }
    
    bool DuelMsgCustomMsg::Handle(std::shared_ptr<DuelScene> pscene) {
        return true;
    }
    
}
