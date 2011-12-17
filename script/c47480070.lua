--アマゾネスの聖戦士
function c47480070.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c47480070.val)
	c:RegisterEffect(e1)
end
function c47480070.val(e,c)
	return Duel.GetMatchingGroupCount(c47480070.filter,c:GetControler(),LOCATION_MZONE,0,nil)*100
end
function c47480070.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x4)
end
