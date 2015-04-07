--ローズ・ウィッチ
function c23087070.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c23087070.condition)
	c:RegisterEffect(e1)
end
function c23087070.condition(e,c)
	return c:IsRace(RACE_PLANT)
end
