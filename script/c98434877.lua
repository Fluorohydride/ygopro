--水魔神－スーガ
function c98434877.initial_effect(c)
	--atkdown
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(98434877,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e1:SetCountLimit(1)
	e1:SetCondition(c98434877.condition)
	e1:SetTarget(c98434877.target)
	e1:SetOperation(c98434877.operation)
	c:RegisterEffect(e1)
end
function c98434877.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetAttackTarget()==e:GetHandler()
end
function c98434877.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetAttacker():IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(Duel.GetAttacker())
end
function c98434877.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
		e1:SetValue(0)
		tc:RegisterEffect(e1)
	end
end
