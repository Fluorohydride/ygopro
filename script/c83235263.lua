--ディノインフィニティ
function c83235263.initial_effect(c)
	--base attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c83235263.atkval)
	c:RegisterEffect(e1)
end
function c83235263.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_DINOSAUR)
end
function c83235263.atkval(e,c)
	return Duel.GetMatchingGroupCount(c83235263.filter,c:GetControler(),LOCATION_REMOVED,0,nil)*1000
end
