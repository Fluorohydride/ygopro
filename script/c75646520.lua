--金属探知器
function c75646520.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_CHAINING)
	e1:SetCondition(c75646520.condition)
	e1:SetOperation(c75646520.activate)
	c:RegisterEffect(e1)
end
function c75646520.condition(e,tp,eg,ep,ev,re,r,rp)
	return re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetActiveType()==TYPE_CONTINUOUS+TYPE_TRAP
end
function c75646520.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	--disable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
	e1:SetTarget(c75646520.distarget)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c75646520.distarget(e,c)
	return c:GetType()==TYPE_CONTINUOUS+TYPE_TRAP
end
