--原始太陽ヘリオス
function c54493213.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetValue(c54493213.value)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_SET_DEFENCE)
	c:RegisterEffect(e2)
end
function c54493213.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER)
end
function c54493213.value(e,c)
	return Duel.GetMatchingGroupCount(c54493213.filter,c:GetControler(),LOCATION_REMOVED,LOCATION_REMOVED,nil)*100
end
