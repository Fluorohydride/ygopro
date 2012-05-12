--死霊の誘い
function c98139712.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c98139712.operation)
	c:RegisterEffect(e2)
end
function c98139712.filter1(c,tp)
	return c:GetOwner()==1-tp
end
function c98139712.filter2(c,tp)
	return c:GetOwner()==tp
end
function c98139712.operation(e,tp,eg,ep,ev,re,r,rp)
	local d1=eg:FilterCount(c98139712.filter1,nil,tp)*300
	local d2=eg:FilterCount(c98139712.filter2,nil,tp)*300
	Duel.Damage(1-tp,d1,REASON_EFFECT)
	Duel.Damage(tp,d2,REASON_EFFECT)
end
