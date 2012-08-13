--闇の芸術家
function c72520073.initial_effect(c)
	--defdown
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(72520073,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetCondition(c72520073.defcon)
	e1:SetOperation(c72520073.defop)
	c:RegisterEffect(e1)
end
function c72520073.defcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	return c==Duel.GetAttackTarget() and bc:IsAttribute(ATTRIBUTE_LIGHT)
end
function c72520073.defop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_DEFENCE)
		e1:SetValue(c:GetDefence()/2)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE)
		c:RegisterEffect(e1)
	end
end
