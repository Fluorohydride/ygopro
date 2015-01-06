--イグザリオン・ユニバース
function c63749102.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(63749102,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetHintTiming(0,TIMING_BATTLE_PHASE)
	e1:SetCondition(c63749102.atkcon)
	e1:SetOperation(c63749102.atkop)
	c:RegisterEffect(e1)
end
function c63749102.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentChain()==0 and e:GetHandler()==Duel.GetAttacker() and Duel.GetAttackTarget() and Duel.GetAttackTarget():IsDefencePos()
end
function c63749102.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(-400)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_PIERCE)
		c:RegisterEffect(e2)
	end
end
