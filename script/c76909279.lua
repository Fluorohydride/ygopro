--激昂のミノタウルス
function c76909279.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c76909279.target)
	c:RegisterEffect(e1)
end
function c76909279.target(e,c)
	return c:IsRace(RACE_BEASTWARRIOR+RACE_WINDBEAST+RACE_BEAST)
end
