--ウミノタウルス
function c30464153.initial_effect(c)
	--pierce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_PIERCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c30464153.target)
	c:RegisterEffect(e1)
end
function c30464153.target(e,c)
	return c:IsRace(RACE_FISH+RACE_SEASERPENT+RACE_AQUA)
end
