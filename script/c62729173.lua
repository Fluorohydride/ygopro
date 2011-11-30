--インヴェルズ万能態
function c62729173.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c62729173.condition)
	c:RegisterEffect(e1)
end
function c62729173.condition(e,c)
	return c:IsSetCard(0x100a)
end
