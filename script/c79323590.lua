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
	e2:SetTarget(c79323590.actarget)
	e2:SetCost(c79323590.costchk)
	e2:SetOperation(c79323590.costop)
	c:RegisterEffect(e2)
	--summon cost
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_SUMMON_COST)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_HAND,LOCATION_HAND)
	e3:SetCondition(c79323590.costcon)
	e3:SetCost(c79323590.costchk)
	e3:SetOperation(c79323590.costop)
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
	--accumulate
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_FIELD)
	e7:SetCode(0x10000000+79323590)
	e7:SetRange(LOCATION_SZONE)
	e7:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e7:SetTargetRange(1,1)
	c:RegisterEffect(e7)
end
function c79323590.actarget(e,te,tp)
	return te:GetHandler():IsLocation(LOCATION_HAND)
end
function c79323590.costcon(e)
	c79323590[0]=false
	return true
end
function c79323590.costchk(e,te_or_c,tp)
	return Duel.CheckLPCost(tp,500)
end
function c79323590.costop(e,tp,eg,ep,ev,re,r,rp)
	if c79323590[0] then return end
	Duel.PayLPCost(tp,Duel.GetFlagEffect(tp,79323590)*500)
	c79323590[0]=true
end
