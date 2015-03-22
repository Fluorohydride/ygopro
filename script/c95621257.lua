--フレムベル・マジカル
function c95621257.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c95621257.atkcon)
	e1:SetValue(400)
	c:RegisterEffect(e1)
end
function c95621257.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x1)
end
function c95621257.atkcon(e)
	return Duel.IsExistingMatchingCard(c95621257.filter,e:GetHandler():GetControler(),LOCATION_MZONE,0,1,nil)
end
