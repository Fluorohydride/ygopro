--エレメントの泉
function c94425169.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--LP up
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_DELAY)
	e2:SetCode(EVENT_TO_HAND)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c94425169.condition)
	e2:SetOperation(c94425169.operation)
	c:RegisterEffect(e2)
end
function c94425169.filter(c)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:IsType(TYPE_MONSTER)
end
function c94425169.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c94425169.filter,1,nil)
end
function c94425169.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Recover(tp,500,REASON_EFFECT)
end
