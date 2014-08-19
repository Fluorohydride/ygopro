--幻奏の音女セレナ
function c42029847.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c42029847.condition)
	c:RegisterEffect(e1)
	--extra summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetOperation(c42029847.regop)
	c:RegisterEffect(e2)
end
function c42029847.condition(e,c)
	return c:IsRace(RACE_FAIRY)
end
function c42029847.regop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFlagEffect(tp,42029847)~=0 then return end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x9b))
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
	Duel.RegisterFlagEffect(tp,42029847,RESET_PHASE+PHASE_END,0,1)
end
