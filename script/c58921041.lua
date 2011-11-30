--魔封じの芳香
function c58921041.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--cannot activate
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_TRIGGER)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_HAND+LOCATION_SZONE, LOCATION_HAND+LOCATION_SZONE)
	e2:SetTarget(c58921041.targets)
	c:RegisterEffect(e2)
end
function c58921041.targets(e,c)
	if not c:IsType(TYPE_SPELL) then return false end
	if c:IsLocation(LOCATION_HAND) then return true end
	if c:IsFaceup() then return false end
	return Duel.GetTurnCount()-c:GetTurnID()<2
end
