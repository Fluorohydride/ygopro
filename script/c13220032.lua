--ヴァイロン・チャージャー
function c13220032.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsAttribute,ATTRIBUTE_LIGHT))
	e1:SetValue(c13220032.atkval)
	c:RegisterEffect(e1)
end
function c13220032.atkval(e,c)
	return e:GetHandler():GetEquipCount()*300
end
