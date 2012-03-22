--魔力枯渇
function c95451366.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c95451366.target)
	e1:SetOperation(c95451366.activate)
	c:RegisterEffect(e1)
end
function c95451366.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsCanRemoveCounter(tp,1,1,0x3001,1,REASON_EFFECT) end
end
function c95451366.filter(c)
	return c:IsFaceup() and c:GetCounter(0x3001)~=0
end
function c95451366.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c95451366.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	local tc=g:GetFirst()
	while tc do 
		local cc=tc:GetCounter(0x3001)
		tc:RemoveCounter(tp,0x3001,cc,REASON_EFFECT)
		tc=g:GetNext()
	end
end
