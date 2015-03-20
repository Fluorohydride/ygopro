--ヒエラコスフィンクス
function c82260502.initial_effect(c)
	--at limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetTarget(c82260502.atlimit)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
end
function c82260502.atlimit(e,c)
	return c:IsPosition(POS_FACEDOWN_DEFENCE)
end
