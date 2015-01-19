--マハー・ヴァイロ
function c93013676.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c93013676.val)
	c:RegisterEffect(e1)
end
function c93013676.val(e,c)
	return c:GetEquipCount()*500
end
