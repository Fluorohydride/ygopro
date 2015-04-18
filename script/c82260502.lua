--ヒエラコスフィンクス
function c82260502.initial_effect(c)
	--at limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetValue(c82260502.atlimit)
	c:RegisterEffect(e1)
end
function c82260502.atlimit(e,c)
	return c:IsPosition(POS_FACEDOWN_DEFENCE)
end
