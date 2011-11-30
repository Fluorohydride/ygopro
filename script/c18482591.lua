--氷結界の破術師
function c18482591.initial_effect(c)
	--cannot activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_TRIGGER)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c18482591.con)
	e1:SetTarget(c18482591.targets)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_SZONE,LOCATION_HAND+LOCATION_SZONE)
	c:RegisterEffect(e1)
end
function c18482591.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x2f)
end
function c18482591.con(e)
	return Duel.IsExistingMatchingCard(c18482591.filter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c18482591.targets(e,c)
	if not c:IsType(TYPE_SPELL) then return false end
	if c:IsLocation(LOCATION_HAND) then return true end
	if c:IsFaceup() then return false end
	return Duel.GetTurnCount()-c:GetTurnID()<2
end
