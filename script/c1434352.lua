--カオス・ネクロマンサー
function c1434352.initial_effect(c)
	--attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c1434352.atkval)
	c:RegisterEffect(e1)
end
function c1434352.atkval(e,c)
	return Duel.GetMatchingGroupCount(Card.IsType,c:GetControler(),LOCATION_GRAVE,0,nil,TYPE_MONSTER)*300
end
