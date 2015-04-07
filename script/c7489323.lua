--ミリス・レディエント
function c7489323.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetTarget(c7489323.tg1)
	e1:SetValue(500)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetTarget(c7489323.tg2)
	e2:SetValue(-400)
	c:RegisterEffect(e2)
end
function c7489323.tg1(e,c)
	return c:IsAttribute(ATTRIBUTE_EARTH)
end
function c7489323.tg2(e,c)
	return c:IsAttribute(ATTRIBUTE_WIND)
end
