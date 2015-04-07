--カイザー・シーホース
function c17444133.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c17444133.condition)
	c:RegisterEffect(e1)
end
function c17444133.condition(e,c)
	return c:IsAttribute(ATTRIBUTE_LIGHT)
end
