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
    
    // MSG_RETRY
    class DuelMsgRetry : public DuelCommand {
    public:
        DuelMsgRetry(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_HINT
    class DuelMsgHint : public DuelCommand {
    public:
        DuelMsgHint(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        int hint_type = 0;
        int hint_data = 0;
    };
    
    // MSG_START
    class DuelMsgStart : public DuelCommand {
    public:
        DuelMsgStart(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_WIN
    class DuelMsgWin : public DuelCommand {
    public:
        DuelMsgWin(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        int win_player = 0;
        int win_reason = 0;
    };
    
    // MSG_UPDATE_CARD
    class DuelMsgUpdateCard : public DuelCommand {
    public:
        DuelMsgUpdateCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        std::vector<unsigned char> update_buffer;
    };
    
    // MSG_SELECT_BATTLECMD
    class DuelMsgSelBattleCmd : public DuelCommand {
    public:
        DuelMsgSelBattleCmd(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_MAINCMD
    class DuelMsgSelMainCmd : public DuelCommand {
    public:
        DuelMsgSelMainCmd(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_EFFECTYN
    class DuelMsgSelEffectYN : public DuelCommand {
    public:
        DuelMsgSelEffectYN(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_YESNO
    class DuelMsgSelYesNo : public DuelCommand {
    public:
        DuelMsgSelYesNo(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_OPTION
    class DuelMsgSelOption : public DuelCommand {
    public:
        DuelMsgSelOption(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_CARD
    class DuelMsgSelCard : public DuelCommand {
    public:
        DuelMsgSelCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_CHAIN
    class DuelMsgSelChain : public DuelCommand {
    public:
        DuelMsgSelChain(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_PLACE
    class DuelMsgSelPlace : public DuelCommand {
    public:
        DuelMsgSelPlace(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_POSITION
    class DuelMsgSelPosition : public DuelCommand {
    public:
        DuelMsgSelPosition(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_TRIBUTE
    class DuelMsgSelTribute : public DuelCommand {
    public:
        DuelMsgSelTribute(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SORT_CHAIN
    class DuelMsgSortChain : public DuelCommand {
    public:
        DuelMsgSortChain(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_COUNTER
    class DuelMsgSelCounter : public DuelCommand {
    public:
        DuelMsgSelCounter(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_SUM
    class DuelMsgSelSum : public DuelCommand {
    public:
        DuelMsgSelSum(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SELECT_DISFIELD
    class DuelMsgSelField : public DuelCommand {
    public:
        DuelMsgSelField(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SORT_CARD
    class DuelMsgSortCard : public DuelCommand {
    public:
        DuelMsgSortCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CONFIRM_DECKTOP
    class DuelMsgConfirmDeck : public DuelCommand {
    public:
        DuelMsgConfirmDeck(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CONFIRM_CARDS
    class DuelMsgConfirmCard : public DuelCommand {
    public:
        DuelMsgConfirmCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SHUFFLE_DECK
    class DuelMsgShuffleDeck : public DuelCommand {
    public:
        DuelMsgShuffleDeck(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SHUFFLE_HAND
    class DuelMsgShuffleHand : public DuelCommand {
    public:
        DuelMsgShuffleHand(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_REFRESH_DECK
    class DuelMsgRefreshDeck : public DuelCommand {
    public:
        DuelMsgRefreshDeck(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SWAP_GRAVE_DECK
    class DuelMsgSwapGraveDeck : public DuelCommand {
    public:
        DuelMsgSwapGraveDeck(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SHUFFLE_SET_CARD
    class DuelMsgShuffleSetCard : public DuelCommand {
    public:
        DuelMsgShuffleSetCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_REVERSE_DECK
    class DuelMsgReverseDeck : public DuelCommand {
    public:
        DuelMsgReverseDeck(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DECK_TOP
    class DuelMsgDeckTop : public DuelCommand {
    public:
        DuelMsgDeckTop(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_NEW_TURN
    class DuelMsgNewTurn : public DuelCommand {
    public:
        DuelMsgNewTurn(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_NEW_PHASE
    class DuelMsgNewPhase : public DuelCommand {
    public:
        DuelMsgNewPhase(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_MOVE
    class DuelMsgMove : public DuelCommand {
    public:
        DuelMsgMove(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        std::array<unsigned char, 8> move_infos;
    };
    
    // MSG_POS_CHANGE
    class DuelMsgPosChange : public DuelCommand {
    public:
        DuelMsgPosChange(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SET
    class DuelMsgSet : public DuelCommand {
    public:
        DuelMsgSet(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SWAP
    class DuelMsgSwap : public DuelCommand {
    public:
        DuelMsgSwap(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_FIELD_DISABLED
    class DuelMsgFieldDisable : public DuelCommand {
    public:
        DuelMsgFieldDisable(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SUMMONING
    class DuelMsgSummoning : public DuelCommand {
    public:
        DuelMsgSummoning(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SUMMONED
    class DuelMsgSummoned : public DuelCommand {
    public:
        DuelMsgSummoned(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SPSUMMONING
    class DuelMsgSPSummoning : public DuelCommand {
    public:
        DuelMsgSPSummoning(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SPSUMMONED
    class DuelMsgSPSummoned : public DuelCommand {
    public:
        DuelMsgSPSummoned(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_FLIPSUMMONING
    class DuelMsgFlipSummoning : public DuelCommand {
    public:
        DuelMsgFlipSummoning(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_FLIPSUMMONED
    class DuelMsgFlipSummoned : public DuelCommand {
    public:
        DuelMsgFlipSummoned(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAINING
    class DuelMsgChaining : public DuelCommand {
    public:
        DuelMsgChaining(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAINED
    class DuelMsgChained : public DuelCommand {
    public:
        DuelMsgChained(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAIN_SOLVING
    class DuelMsgChainSolving : public DuelCommand {
    public:
        DuelMsgChainSolving(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAIN_SOLVED
    class DuelMsgChainSolved : public DuelCommand {
    public:
        DuelMsgChainSolved(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAIN_END
    class DuelMsgChainEnd : public DuelCommand {
    public:
        DuelMsgChainEnd(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAIN_NEGATED
    class DuelMsgChainNegated : public DuelCommand {
    public:
        DuelMsgChainNegated(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CHAIN_DISABLED
    class DuelMsgChainDisabled : public DuelCommand {
    public:
        DuelMsgChainDisabled(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CARD_SELECTED
    class DuelMsgCardSelected : public DuelCommand {
    public:
        DuelMsgCardSelected(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_RANDOM_SELECTED
    class DuelMsgRandomSelected : public DuelCommand {
    public:
        DuelMsgRandomSelected(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_BECOME_TARGET
    class DuelMsgBecomeTarget : public DuelCommand {
    public:
        DuelMsgBecomeTarget(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DRAW
    class DuelMsgDraw : public DuelCommand {
    public:
        DuelMsgDraw(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DAMAGE
    class DuelMsgDamage : public DuelCommand {
    public:
        DuelMsgDamage(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_RECOVER
    class DuelMsgRecover : public DuelCommand {
    public:
        DuelMsgRecover(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_EQUIP
    class DuelMsgEquip : public DuelCommand {
    public:
        DuelMsgEquip(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_LPUPDATE
    class DuelMsgLPUpdate : public DuelCommand {
    public:
        DuelMsgLPUpdate(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_UNEQUIP
    class DuelMsgUnequip : public DuelCommand {
    public:
        DuelMsgUnequip(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CARD_TARGET
    class DuelMsgCardTarget : public DuelCommand {
    public:
        DuelMsgCardTarget(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CANCEL_TARGET
    class DuelMsgCancelTarget : public DuelCommand {
    public:
        DuelMsgCancelTarget(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_PAY_LPCOST
    class DuelMsgLPCost : public DuelCommand {
    public:
        DuelMsgLPCost(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_ADD_COUNTER
    class DuelMsgAddCounter : public DuelCommand {
    public:
        DuelMsgAddCounter(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_REMOVE_COUNTER
    class DuelMsgRemoveCounter : public DuelCommand {
    public:
        DuelMsgRemoveCounter(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_ATTACK
    class DuelMsgAttack : public DuelCommand {
    public:
        DuelMsgAttack(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_BATTLE
    class DuelMsgBattle : public DuelCommand {
    public:
        DuelMsgBattle(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_ATTACK_NEGATED
    class DuelMsgAttackNegated : public DuelCommand {
    public:
        DuelMsgAttackNegated(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DAMAGE_STEP_START
    class DuelMsgDamageStepStart : public DuelCommand {
    public:
        DuelMsgDamageStepStart(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DAMAGE_STEP_END
    class DuelMsgDamageStepEnd : public DuelCommand {
    public:
        DuelMsgDamageStepEnd(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_MISSED_EFFECT
    class DuelMsgMissEffect : public DuelCommand {
    public:
        DuelMsgMissEffect(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_BE_CHAIN_TARGET
    class DuelMsgBeChainTarget : public DuelCommand {
    public:
        DuelMsgBeChainTarget(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CREATE_RELATION
    class DuelMsgCreateRelation : public DuelCommand {
    public:
        DuelMsgCreateRelation(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_RELEASE_RELATION
    class DuelMsgReleaseRelation : public DuelCommand {
    public:
        DuelMsgReleaseRelation(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_TOSS_COIN
    class DuelMsgTossCoin : public DuelCommand {
    public:
        DuelMsgTossCoin(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_TOSS_DICE
    class DuelMsgTossDice : public DuelCommand {
    public:
        DuelMsgTossDice(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DECLEAR_RACE
    class DuelMsgDeclearRace: public DuelCommand {
    public:
        DuelMsgDeclearRace(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DECLEAR_ATTRIB
    class DuelMsgDeclearAttrib : public DuelCommand {
    public:
        DuelMsgDeclearAttrib(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DECLEAR_CARD
    class DuelMsgDeclearCard : public DuelCommand {
    public:
        DuelMsgDeclearCard(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_DECLEAR_NUMBER
    class DuelMsgDeclearNumber : public DuelCommand {
    public:
        DuelMsgDeclearNumber(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CARD_HINT
    class DuelMsgCardHint : public DuelCommand {
    public:
        DuelMsgCardHint(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_TAG_SWAP
    class DuelMsgTagSwap: public DuelCommand {
    public:
        DuelMsgTagSwap(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_RELOAD_FIELD
    class DuelMsgLoad : public DuelCommand {
        struct InitPlayerInfo {
            unsigned int hp;
            std::array<int, 5> minfo;
            std::array<int, 8> sinfo;
            unsigned int main_size;
            unsigned int hand_size;
            unsigned int grave_size;
            unsigned int remove_size;
            unsigned int extra_size;
        };
        
        struct InitChainInfo {
            unsigned int code;
            unsigned int desc;
            unsigned char card_con;
            unsigned char card_loc;
            unsigned char card_seq;
            unsigned char card_pos;
            unsigned char trig_con;
            unsigned char trig_loc;
            unsigned char trig_seq;
        };
        
    public:
        DuelMsgLoad(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
        std::vector<InitPlayerInfo> pinfos;
        std::vector<InitChainInfo> cinfos;
    };
    
    // MSG_AI_NAME
    class DuelMsgAIName: public DuelCommand {
    public:
        DuelMsgAIName(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_SHOW_HINT
    class DuelMsgShowHint: public DuelCommand {
    public:
        DuelMsgShowHint(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_MATCH_KILL
    class DuelMsgMatchKill: public DuelCommand {
    public:
        DuelMsgMatchKill(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
    
    // MSG_CUSTOM_MSG
    class DuelMsgCustomMsg: public DuelCommand {
    public:
        DuelMsgCustomMsg(BufferUtil& reader);
        virtual bool Handle(std::shared_ptr<DuelScene> pscene);
    protected:
    };
}

#endif
