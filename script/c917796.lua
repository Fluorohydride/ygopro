--ガガガタッグ
function c917796.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c917796.cost)
	e1:SetTarget(c917796.target)
	e1:SetOperation(c917796.activate)
	c:RegisterEffect(e1)
end
function c917796.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,917796)==0 end
	Duel.RegisterFlagEffect(tp,917796,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c917796.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x54)
end
function c917796.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c917796.filter,tp,LOCATION_MZONE,0,1,nil) end
end
function c917796.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(c917796.filter,tp,LOCATION_MZONE,0,nil)
	local atk=sg:GetCount()*500
	local c=e:GetHandler()
	local tc=sg:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(atk)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_STANDBY,2)
		tc:RegisterEffect(e1)
		tc=sg:GetNext()
	end
end
