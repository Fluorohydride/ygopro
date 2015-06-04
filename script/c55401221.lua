--電池メン－単一型
function c55401221.initial_effect(c)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetValue(c55401221.atlimit)
	c:RegisterEffect(e1)
end
function c55401221.atlimit(e,c)
	return c:IsFaceup() and c:IsRace(RACE_THUNDER) and c:GetCode()~=55401221
end
