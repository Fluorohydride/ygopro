--インヴェルズの歩哨
function c99214782.initial_effect(c)
	--cannot trigger
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_TRIGGER)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c99214782.condition)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c99214782.target)
	c:RegisterEffect(e1)
end
function c99214782.condition(e)
	return e:GetHandler():IsAttackPos()
end
function c99214782.target(e,c)
	return c:GetLevel()>=5 and bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)~=0
end
