--サンライズ・ガードナー
function c57549932.initial_effect(c)
	aux.EnableDualAttribute(c)
	--change base defence
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(aux.IsDualState)
	e1:SetCode(EFFECT_SET_BASE_DEFENCE)
	e1:SetValue(2300)
	c:RegisterEffect(e1)
end
