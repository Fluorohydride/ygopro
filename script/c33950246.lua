--王宮の号令
function c33950246.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--disable effect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_ACTIVATING)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c33950246.disop)
	c:RegisterEffect(e2)
end
function c33950246.disop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsActiveType(TYPE_FLIP) then
		Duel.NegateActivation(ev)
	end
end
