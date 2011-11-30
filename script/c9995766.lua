--宮廷のしきたり
function c9995766.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetCondition(c9995766.condition)
	c:RegisterEffect(e1)
	--indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_INDESTRUCTABLE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_ONFIELD,LOCATION_ONFIELD)
	e2:SetTarget(c9995766.infilter)
	c:RegisterEffect(e2)
end
function c9995766.afilter(c)
	return c:IsFaceup() and c:GetCode()==9995766
end
function c9995766.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c9995766.afilter,tp,LOCATION_SZONE,0,1,nil)
end
function c9995766.infilter(e,c)
	return bit.band(c:GetType(),0x20004)==0x20004 and c:GetCode()~=9995766
end
