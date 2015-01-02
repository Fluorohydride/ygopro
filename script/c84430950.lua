--重装武者－ベン・ケイ
function c84430950.initial_effect(c)
	--multi attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetValue(c84430950.val)
	c:RegisterEffect(e1)
end
function c84430950.val(e,c)
	return c:GetEquipCount()
end
