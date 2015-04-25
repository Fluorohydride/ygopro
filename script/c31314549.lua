--RR－シンギング・レイニアス
function c31314549.initial_effect(c)
	--special summon rule
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetRange(LOCATION_HAND)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetCountLimit(1,31314549)
	e1:SetCondition(c31314549.spcon)
	c:RegisterEffect(e1)
end
function c31314549.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c31314549.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c31314549.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
