--Ms.JUDGE
function c86767655.initial_effect(c)
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_CHAIN_SOLVING)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,86767655)
	e1:SetCondition(c86767655.discon)
	e1:SetOperation(c86767655.disop)
	c:RegisterEffect(e1)
end
function c86767655.discon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and Duel.IsChainDisablable(ev)
end
function c86767655.disop(e,tp,eg,ep,ev,re,r,rp)
	local c1,c2=Duel.TossCoin(tp,2)
	if c1+c2==2 then
		Duel.NegateEffect(ev)
	end
end
