--紋章獣エアレー
function c82315772.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c82315772.spcon)
	c:RegisterEffect(e1)
end
function c82315772.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x76)
end
function c82315772.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0 and
		Duel.IsExistingMatchingCard(c82315772.filter,c:GetControler(),LOCATION_MZONE,0,2,nil)
end
