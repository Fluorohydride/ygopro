--天下人 紫炎
function c41589166.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_IMMUNE_EFFECT)
	e1:SetValue(c41589166.efilter)
	c:RegisterEffect(e1)
end
function c41589166.efilter(e,te)
	return te:IsActiveType(TYPE_TRAP)
end
