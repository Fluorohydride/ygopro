--暴君の威圧
function c4638410.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c4638410.cost)
	c:RegisterEffect(e1)
	--immune
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_IMMUNE_EFFECT)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c4638410.etarget)
	e2:SetValue(c4638410.efilter)
	c:RegisterEffect(e2)
end
function c4638410.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,1,nil) end
	local rg=Duel.SelectReleaseGroup(tp,nil,1,1,nil)
	Duel.Release(rg,REASON_COST)
end
function c4638410.etarget(e,c)
	return c:GetOwner()==e:GetHandlerPlayer()
end
function c4638410.efilter(e,te)
	return te:IsActiveType(TYPE_TRAP) and te:GetOwner()~=e:GetOwner()
end
