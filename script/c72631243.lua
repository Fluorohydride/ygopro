--エナジー・ブレイブ
function c72631243.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c72631243.indtg)
	e1:SetValue(1)
	c:RegisterEffect(e1)
end
function c72631243.indtg(e,c)
	return c:IsDualState()
end
