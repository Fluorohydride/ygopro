--限界竜シュヴァルツシルト
function c6930746.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c6930746.spcon)
	c:RegisterEffect(e1)
end
function c6930746.filter(c)
	return c:IsFaceup() and c:IsAttackAbove(2000)
end
function c6930746.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c6930746.filter,tp,0,LOCATION_MZONE,1,nil)
end
