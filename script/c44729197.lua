--スチームロイド
function c44729197.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetOperation(c44729197.operation)
	c:RegisterEffect(e1)
end
function c44729197.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetAttackTarget()==nil then return end
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	if c==Duel.GetAttacker() then
		e1:SetValue(500)
	else e1:SetValue(-500) end
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE)
	c:RegisterEffect(e1)
end
