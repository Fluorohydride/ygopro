--デコイロイド
function c25034083.initial_effect(c)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e1:SetValue(c25034083.atlimit)
	c:RegisterEffect(e1)
end
function c25034083.atlimit(e,c)
	return not c:IsCode(25034083) and c:IsFaceup()
end
