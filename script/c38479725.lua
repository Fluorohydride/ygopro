--トロイホース
function c38479725.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c38479725.condition)
	c:RegisterEffect(e1)
end
function c38479725.condition(e,c)
	return c:IsAttribute(ATTRIBUTE_EARTH)
end
