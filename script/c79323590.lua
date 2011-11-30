--魔力の枷
function c79323590.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--activate cost
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_ACTIVATE_COST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,1)
	e2:SetCost(c79323590.accost)
	e2:SetValue(c79323590.acval)
	e2:SetOperation(c79323590.acop)
	c:RegisterEffect(e2)
	--summon cost
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_SUMMON_COST)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_HAND,LOCATION_HAND)
	e3:SetCost(c79323590.ccost)
	e3:SetValue(1)
	e3:SetOperation(c79323590.acop)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_SPSUMMON_COST)
	c:RegisterEffect(e4)
	--set cost
	local e5=e3:Clone()
	e5:SetCode(EFFECT_MSET_COST)
	c:RegisterEffect(e5)
	local e6=e3:Clone()
	e6:SetCode(EFFECT_SSET_COST)
	c:RegisterEffect(e6)
end
function c79323590.accost(e,te,tp)
	return Duel.CheckLPCost(tp,500)
end
function c79323590.ccost(e,c,tp)
	return Duel.CheckLPCost(tp,500)
end
function c79323590.acval(e,te,tp)
	return te:GetHandler():GetLocation()==LOCATION_HAND
end
function c79323590.acop(e,tp,eg,ep,ev,re,r,rp)
	Duel.PayLPCost(tp,500)
end
