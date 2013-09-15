--暴君の暴言
function c76721030.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c76721030.cost)
	c:RegisterEffect(e1)
	--cannot trigger
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_TRIGGER)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE+LOCATION_HAND,LOCATION_MZONE+LOCATION_HAND)
	e2:SetTarget(c76721030.etarget)
	c:RegisterEffect(e2)
end
function c76721030.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,2,nil) end
	local rg=Duel.SelectReleaseGroup(tp,nil,2,2,nil)
	Duel.Release(rg,REASON_COST)
end
function c76721030.etarget(e,c)
	return c:IsType(TYPE_MONSTER)
end
