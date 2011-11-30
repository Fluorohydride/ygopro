--魔轟神ウルストス
function c73040500.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetCondition(c73040500.con)
	e1:SetTarget(c73040500.tg)
	e1:SetValue(400)
	c:RegisterEffect(e1)
end
function c73040500.con(e)
	return Duel.GetFieldGroupCount(e:GetHandler():GetControler(),LOCATION_HAND,0)<=2
end
function c73040500.tg(e,c)
	return c:IsFaceup() and c:IsSetCard(0x35)
end
