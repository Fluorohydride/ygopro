--トラファスフィア
function c72144675.initial_effect(c)
	--tribute limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_TRIBUTE_LIMIT)
	e1:SetValue(c72144675.tlimit)
	c:RegisterEffect(e1)
	--immune
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_IMMUNE_EFFECT)
	e2:SetValue(c72144675.efilter)
	c:RegisterEffect(e2)
end
function c72144675.tlimit(e,c)
	return not c:IsRace(RACE_WINDBEAST)
end
function c72144675.efilter(e,te)
	return te:IsActiveType(TYPE_TRAP)
end
