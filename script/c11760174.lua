--言語道断侍
function c11760174.initial_effect(c)
	--act limit
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(11760174,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_REPEAT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c11760174.cost)
	e1:SetOperation(c11760174.operation)
	c:RegisterEffect(e1)
end
function c11760174.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
end
function c11760174.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EFFECT_CANNOT_ACTIVATE)
	e1:SetTargetRange(1,1)
	e1:SetValue(c11760174.aclimit)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c11760174.aclimit(e,re,tp)
	return re:IsActiveType(TYPE_SPELL+TYPE_TRAP)
end
