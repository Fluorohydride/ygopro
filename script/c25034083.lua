--デコイロイド
function c25034083.initial_effect(c)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetTarget(c25034083.attg)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
end
function c25034083.attg(e,c)
	return not c:IsCode(25034083)
end
