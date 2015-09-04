--アマゾネス・スカウト
function c71209500.initial_effect(c)
	--effects
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(71209500,0))
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0+TIMING_DRAW_PHASE)
	e1:SetCost(c71209500.efcost)
	e1:SetTarget(c71209500.eftg)
	e1:SetOperation(c71209500.efop)
	c:RegisterEffect(e1)
end
function c71209500.efcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c71209500.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x4)
end
function c71209500.eftg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c71209500.filter,tp,LOCATION_MZONE,0,1,e:GetHandler()) end
end
function c71209500.efop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetMatchingGroup(c71209500.filter,tp,LOCATION_MZONE,0,nil)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		e1:SetValue(c71209500.efilter)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
		e2:SetValue(1)
		e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e2)
		tc=g:GetNext()
	end
end
function c71209500.efilter(e,re,rp)
	return re:IsActiveType(TYPE_MONSTER) and aux.tgval(e,re,rp)
end