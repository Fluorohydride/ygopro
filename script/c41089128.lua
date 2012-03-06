--炎を支配する者
function c41089128.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c41089128.condition)
	c:RegisterEffect(e1)
end
function c41089128.condition(e,c)
	return c:IsAttribute(ATTRIBUTE_FIRE)
end
