--ヘル・ツイン・コップ
function c86137485.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_FIEND),aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--chain attack
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(86137485,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(c86137485.atcon1)
	e1:SetOperation(c86137485.atop1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(86137485,0))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetCondition(c86137485.atcon2)
	e2:SetOperation(c86137485.atop2)
	c:RegisterEffect(e2)
end
function c86137485.atcon1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return Duel.GetTurnPlayer()==tp and bc:IsLocation(LOCATION_GRAVE) and bc:IsType(TYPE_MONSTER)
		and c:IsChainAttackable() and c:IsStatus(STATUS_OPPO_BATTLE)
end
function c86137485.atop1(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(800)
	e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
	Duel.ChainAttack()
end
function c86137485.atcon2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return Duel.GetTurnPlayer()~=tp and c:IsRelateToBattle() and c:IsStatus(STATUS_OPPO_BATTLE) 
		and bc:IsLocation(LOCATION_GRAVE) and bc:IsType(TYPE_MONSTER)
end
function c86137485.atop2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(800)
	e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
end
