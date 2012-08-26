--死の演算盤
function c77910045.initial_effect(c)
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
	e2:SetOperation(c77910045.operation)
	c:RegisterEffect(e2)
end
function c77910045.filter(c,tp)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:IsControler(tp) and c:IsType(TYPE_MONSTER)
end
function c77910045.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=eg:FilterCount(c77910045.filter,nil,tp)
	if ct>0 then
		Duel.Damage(tp,500*ct,REASON_EFFECT)
	end
	ct=eg:FilterCount(c77910045.filter,nil,1-tp)
	if ct>0 then
		Duel.Damage(1-tp,500*ct,REASON_EFFECT)
	end
end
