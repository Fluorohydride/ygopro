--シャインナイト
function c86952477.initial_effect(c)
	--lvchange
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CHANGE_LEVEL)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c86952477.lvcon)
	e1:SetValue(4)
	c:RegisterEffect(e1)
end
function c86952477.lvcon(e)
	return e:GetHandler():IsDefencePos()
end
