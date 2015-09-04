--ガーディアン・オブ・オーダー
function c71799173.initial_effect(c)
	c:SetUniqueOnField(1,0,71799173)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c71799173.spcon)
	c:RegisterEffect(e1)
end
function c71799173.spfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c71799173.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and	Duel.IsExistingMatchingCard(c71799173.spfilter,c:GetControler(),LOCATION_MZONE,0,2,nil)
end
