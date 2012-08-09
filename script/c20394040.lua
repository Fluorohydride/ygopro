--バーバリアン1号
function c20394040.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c20394040.value)
	c:RegisterEffect(e1)
end
function c20394040.filter(c)
	return c:IsFaceup() and c:IsCode(40453765)
end
function c20394040.value(e,c)
	return Duel.GetMatchingGroupCount(c20394040.filter,c:GetControler(),LOCATION_MZONE,0,nil)*500
end
