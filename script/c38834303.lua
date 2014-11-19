--カウンタークリーナー
function c38834303.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c38834303.cost)
	e1:SetTarget(c38834303.target)
	e1:SetOperation(c38834303.activate)
	c:RegisterEffect(e1)
end
function c38834303.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500) end
	Duel.PayLPCost(tp,500)
end
function c38834303.filter(c)
	return c:GetCounter(0)~=0
end
function c38834303.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c38834303.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
end
function c38834303.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(c38834303.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	local tc=sg:GetFirst()
	while tc do
		tc:RemoveCounter(tp,0,0,0)
		tc=sg:GetNext()
	end
end
