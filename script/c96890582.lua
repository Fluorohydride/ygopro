--秒殺の暗殺者
function c96890582.initial_effect(c)
	--atkdown
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c96890582.val)
	c:RegisterEffect(e1)
	--defdown
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
end
function c96890582.val(e,c)
	return Duel.GetFieldGroupCount(c:GetControler(),LOCATION_HAND,0)*(-400)
end
