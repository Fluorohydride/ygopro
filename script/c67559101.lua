--先史遺産マッドゴーレム・シャコウキ
function c67559101.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c67559101.target)
	c:RegisterEffect(e1)
end
function c67559101.target(e,c)
	return c:IsSetCard(0x70)
end
