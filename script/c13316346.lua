--起動兵士デッドリボルバー
function c13316346.initial_effect(c)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c13316346.atkcon)
	e1:SetValue(2000)
	c:RegisterEffect(e1)
end
function c13316346.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x51)
end
function c13316346.atkcon(e)
	return Duel.IsExistingMatchingCard(c13316346.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
