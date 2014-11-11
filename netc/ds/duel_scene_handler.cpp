#include "../../common/common.h"

#include "../../ocgcore/common.h"

#include "../sungui.h"
#include "duel_command.h"
#include "duel_scene_handler.h"

namespace ygopro
{
    
    DuelSceneHandler::DuelSceneHandler(std::shared_ptr<DuelScene> pscene) {
        duel_scene = pscene;
    }
    
    bool DuelSceneHandler::UpdateEvent() {
        auto pscene = duel_scene.lock();
        return false;
    }
    
    void DuelSceneHandler::BeginHandler() {
        proto_handler->BeginProto();
    }
    
    void DuelProtoHandler::ProcessMsg(uint32_t sz) {
        BufferUtil reader(&msg_buffer[0], sz);
        while(!reader.IsEnd()) {
            uint8_t msg_type = reader.Read<uint8_t>();
            MessageToCmd(msg_type, reader);
        }
    }

    int32_t DuelProtoHandler::MessageToCmd(uint8_t msg_type, BufferUtil& reader) {
        switch(msg_type) {
            case MSG_RETRY:
                PushCommand(std::make_shared<DuelMsgRetry>(reader));
                break;
            case MSG_HINT:
                PushCommand(std::make_shared<DuelMsgHint>(reader));
                break;
            case MSG_START:
                PushCommand(std::make_shared<DuelMsgStart>(reader));
                break;
            case MSG_WIN:
                PushCommand(std::make_shared<DuelMsgWin>(reader));
                break;
            case MSG_UPDATE_CARD:
                PushCommand(std::make_shared<DuelMsgUpdateCard>(reader));
                break;
            case MSG_SELECT_BATTLECMD:
                PushCommand(std::make_shared<DuelMsgSelBattleCmd>(reader));
                break;
            case MSG_SELECT_MAINCMD:
                PushCommand(std::make_shared<DuelMsgSelMainCmd>(reader));
                break;
            case MSG_SELECT_EFFECTYN:
                PushCommand(std::make_shared<DuelMsgSelEffectYN>(reader));
                break;
            case MSG_SELECT_YESNO:
                PushCommand(std::make_shared<DuelMsgSelYesNo>(reader));
                break;
            case MSG_SELECT_OPTION:
                PushCommand(std::make_shared<DuelMsgSelOption>(reader));
                break;
            case MSG_SELECT_CARD:
                PushCommand(std::make_shared<DuelMsgSelCard>(reader));
                break;
            case MSG_SELECT_CHAIN:
                PushCommand(std::make_shared<DuelMsgSelChain>(reader));
                break;
            case MSG_SELECT_PLACE:
                PushCommand(std::make_shared<DuelMsgSelPlace>(reader));
                break;
            case MSG_SELECT_POSITION:
                PushCommand(std::make_shared<DuelMsgSelPosition>(reader));
                break;
            case MSG_SELECT_TRIBUTE:
                PushCommand(std::make_shared<DuelMsgSelTribute>(reader));
                break;
            case MSG_SORT_CHAIN:
                PushCommand(std::make_shared<DuelMsgSortChain>(reader));
                break;
            case MSG_SELECT_COUNTER:
                PushCommand(std::make_shared<DuelMsgSelCounter>(reader));
                break;
            case MSG_SELECT_SUM:
                PushCommand(std::make_shared<DuelMsgSelSum>(reader));
                break;
            case MSG_SELECT_DISFIELD:
                PushCommand(std::make_shared<DuelMsgSelField>(reader));
                break;
            case MSG_SORT_CARD:
                PushCommand(std::make_shared<DuelMsgSortCard>(reader));
                break;
            case MSG_CONFIRM_DECKTOP:
                PushCommand(std::make_shared<DuelMsgConfirmDeck>(reader));
                break;
            case MSG_CONFIRM_CARDS:
                PushCommand(std::make_shared<DuelMsgConfirmCard>(reader));
                break;
            case MSG_SHUFFLE_DECK:
                PushCommand(std::make_shared<DuelMsgShuffleDeck>(reader));
                break;
            case MSG_SHUFFLE_HAND:
                PushCommand(std::make_shared<DuelMsgShuffleHand>(reader));
                break;
            case MSG_REFRESH_DECK:
                PushCommand(std::make_shared<DuelMsgRefreshDeck>(reader));
                break;
            case MSG_SWAP_GRAVE_DECK:
                PushCommand(std::make_shared<DuelMsgSwapGraveDeck>(reader));
                break;
            case MSG_SHUFFLE_SET_CARD:
                PushCommand(std::make_shared<DuelMsgShuffleSetCard>(reader));
                break;
            case MSG_REVERSE_DECK:
                PushCommand(std::make_shared<DuelMsgReverseDeck>(reader));
                break;
            case MSG_DECK_TOP:
                PushCommand(std::make_shared<DuelMsgDeckTop>(reader));
                break;
            case MSG_NEW_TURN:
                PushCommand(std::make_shared<DuelMsgNewTurn>(reader));
                break;
            case MSG_NEW_PHASE:
                PushCommand(std::make_shared<DuelMsgNewPhase>(reader));
                break;
            case MSG_MOVE:
                PushCommand(std::make_shared<DuelMsgMove>(reader));
                break;
            case MSG_POS_CHANGE:
                PushCommand(std::make_shared<DuelMsgPosChange>(reader));
                break;
            case MSG_SET:
                PushCommand(std::make_shared<DuelMsgSet>(reader));
                break;
            case MSG_SWAP:
                PushCommand(std::make_shared<DuelMsgSwap>(reader));
                break;
            case MSG_FIELD_DISABLED:
                PushCommand(std::make_shared<DuelMsgFieldDisable>(reader));
                break;
            case MSG_SUMMONING:
                PushCommand(std::make_shared<DuelMsgSummoning>(reader));
                break;
            case MSG_SUMMONED:
                PushCommand(std::make_shared<DuelMsgSummoned>(reader));
                break;
            case MSG_SPSUMMONING:
                PushCommand(std::make_shared<DuelMsgSPSummoning>(reader));
                break;
            case MSG_SPSUMMONED:
                PushCommand(std::make_shared<DuelMsgSPSummoned>(reader));
                break;
            case MSG_FLIPSUMMONING:
                PushCommand(std::make_shared<DuelMsgFlipSummoning>(reader));
                break;
            case MSG_FLIPSUMMONED:
                PushCommand(std::make_shared<DuelMsgFlipSummoned>(reader));
                break;
            case MSG_CHAINING:
                PushCommand(std::make_shared<DuelMsgChaining>(reader));
                break;
            case MSG_CHAINED:
                PushCommand(std::make_shared<DuelMsgChained>(reader));
                break;
            case MSG_CHAIN_SOLVING:
                PushCommand(std::make_shared<DuelMsgChainSolving>(reader));
                break;
            case MSG_CHAIN_SOLVED:
                PushCommand(std::make_shared<DuelMsgChainSolved>(reader));
                break;
            case MSG_CHAIN_END:
                PushCommand(std::make_shared<DuelMsgChainEnd>(reader));
                break;
            case MSG_CHAIN_NEGATED:
                PushCommand(std::make_shared<DuelMsgChainNegated>(reader));
                break;
            case MSG_CHAIN_DISABLED:
                PushCommand(std::make_shared<DuelMsgChainDisabled>(reader));
                break;
            case MSG_CARD_SELECTED:
                PushCommand(std::make_shared<DuelMsgCardSelected>(reader));
                break;
            case MSG_RANDOM_SELECTED:
                PushCommand(std::make_shared<DuelMsgRandomSelected>(reader));
                break;
            case MSG_BECOME_TARGET:
                PushCommand(std::make_shared<DuelMsgBecomeTarget>(reader));
                break;
            case MSG_DRAW:
                PushCommand(std::make_shared<DuelMsgDraw>(reader));
                break;
            case MSG_DAMAGE:
                PushCommand(std::make_shared<DuelMsgDamage>(reader));
                break;
            case MSG_RECOVER:
                PushCommand(std::make_shared<DuelMsgRecover>(reader));
                break;
            case MSG_EQUIP:
                PushCommand(std::make_shared<DuelMsgEquip>(reader));
                break;
            case MSG_LPUPDATE:
                PushCommand(std::make_shared<DuelMsgLPUpdate>(reader));
                break;
            case MSG_UNEQUIP:
                PushCommand(std::make_shared<DuelMsgUnequip>(reader));
                break;
            case MSG_CARD_TARGET:
                PushCommand(std::make_shared<DuelMsgCardTarget>(reader));
                break;
            case MSG_CANCEL_TARGET:
                PushCommand(std::make_shared<DuelMsgCancelTarget>(reader));
                break;
            case MSG_PAY_LPCOST:
                PushCommand(std::make_shared<DuelMsgLPCost>(reader));
                break;
            case MSG_ADD_COUNTER:
                PushCommand(std::make_shared<DuelMsgAddCounter>(reader));
                break;
            case MSG_REMOVE_COUNTER:
                PushCommand(std::make_shared<DuelMsgRemoveCounter>(reader));
                break;
            case MSG_ATTACK:
                PushCommand(std::make_shared<DuelMsgAttack>(reader));
                break;
            case MSG_BATTLE:
                PushCommand(std::make_shared<DuelMsgBattle>(reader));
                break;
            case MSG_ATTACK_NEGATED:
                PushCommand(std::make_shared<DuelMsgAttackNegated>(reader));
                break;
            case MSG_DAMAGE_STEP_START:
                PushCommand(std::make_shared<DuelMsgDamageStepStart>(reader));
                break;
            case MSG_DAMAGE_STEP_END:
                PushCommand(std::make_shared<DuelMsgDamageStepEnd>(reader));
                break;
            case MSG_MISSED_EFFECT:
                PushCommand(std::make_shared<DuelMsgMissEffect>(reader));
                break;
            case MSG_BE_CHAIN_TARGET:
                PushCommand(std::make_shared<DuelMsgBeChainTarget>(reader));
                break;
            case MSG_CREATE_RELATION:
                PushCommand(std::make_shared<DuelMsgCreateRelation>(reader));
                break;
            case MSG_RELEASE_RELATION:
                PushCommand(std::make_shared<DuelMsgReleaseRelation>(reader));
                break;
            case MSG_TOSS_COIN:
                PushCommand(std::make_shared<DuelMsgTossCoin>(reader));
                break;
            case MSG_TOSS_DICE:
                PushCommand(std::make_shared<DuelMsgTossDice>(reader));
                break;
            case MSG_DECLEAR_RACE:
                PushCommand(std::make_shared<DuelMsgDeclearRace>(reader));
                break;
            case MSG_DECLEAR_ATTRIB:
                PushCommand(std::make_shared<DuelMsgDeclearAttrib>(reader));
                break;
            case MSG_DECLEAR_CARD:
                PushCommand(std::make_shared<DuelMsgDeclearCard>(reader));
                break;
            case MSG_DECLEAR_NUMBER:
                PushCommand(std::make_shared<DuelMsgDeclearNumber>(reader));
                break;
            case MSG_CARD_HINT:
                PushCommand(std::make_shared<DuelMsgCardHint>(reader));
                break;
            case MSG_TAG_SWAP:
                PushCommand(std::make_shared<DuelMsgTagSwap>(reader));
                break;
            case MSG_RELOAD_FIELD:
                PushCommand(std::make_shared<DuelMsgLoad>(reader));
                break;
            case MSG_AI_NAME:
                PushCommand(std::make_shared<DuelMsgAIName>(reader));
                break;
            case MSG_SHOW_HINT:
                PushCommand(std::make_shared<DuelMsgShowHint>(reader));
                break;
            case MSG_MATCH_KILL:
                PushCommand(std::make_shared<DuelMsgMatchKill>(reader));
                break;
            case MSG_CUSTOM_MSG:
                PushCommand(std::make_shared<DuelMsgCustomMsg>(reader));
                break;
            default:
                break;
        }
        return 0;
    }
    
}