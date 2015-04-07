--武神器－チカヘシ
function c80555062.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c80555062.target)
	e1:SetCondition(c80555062.con)
	e1:SetValue(1)
	c:RegisterEffect(e1)
end
function c80555062.con(e)
	return e:GetHandler():IsDefencePos()
end
function c80555062.target(e,c)
	return c~=e:GetHandler() and c:IsSetCard(0x88)
end
