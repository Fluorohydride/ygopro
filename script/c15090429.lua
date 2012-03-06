--暴風小僧
function c15090429.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c15090429.condition)
	c:RegisterEffect(e1)
end
function c15090429.condition(e,c)
	return c:IsAttribute(ATTRIBUTE_WIND)
end
