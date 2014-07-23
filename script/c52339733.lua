--竜胆ブルーム
function c52339733.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_PRE_DAMAGE_CALCULATE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c52339733.atkcon)
	e1:SetOperation(c52339733.atkop)
	c:RegisterEffect(e1)
end
function c52339733.atkcon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	return d and a:GetControler()~=d:GetControler()
end
function c52339733.atkop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if a:IsFaceup() and a:IsRelateToBattle() and d:IsFaceup() and d:IsRelateToBattle() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetValue(a:GetDefence())
		e1:SetReset(RESET_PHASE+RESET_DAMAGE_CAL)
		a:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetValue(d:GetDefence())
		d:RegisterEffect(e2)
	end
end
