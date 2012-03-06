--ダブルコストン
function c44436472.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c44436472.condition)
	c:RegisterEffect(e1)
end
function c44436472.condition(e,c)
	return c:IsAttribute(ATTRIBUTE_DARK)
end
