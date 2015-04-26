--氷結界の御庭番
function c27527047.initial_effect(c)
	--untargetable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x2f))
	e1:SetValue(c27527047.tgval)
	c:RegisterEffect(e1)
end
function c27527047.tgval(e,re,rp)
	local tp=e:GetHandler():GetControler()
	return tp~=rp and re:GetHandler():IsType(TYPE_MONSTER) and aux.tgval(e,re,rp)
end
