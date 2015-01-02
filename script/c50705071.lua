--メタル・デビルゾア
function c50705071.initial_effect(c)
	c:EnableReviveLimit()
	--spsummon proc
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_DECK)
	e1:SetCondition(c50705071.spcon)
	e1:SetOperation(c50705071.spop)
	c:RegisterEffect(e1)
end
function c50705071.spfilter(c)
	return c:IsCode(24311372) and c:GetEquipGroup():IsExists(Card.IsCode,1,nil,68540058)
end
function c50705071.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.CheckReleaseGroup(tp,c50705071.spfilter,1,nil)
end
function c50705071.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.SelectReleaseGroup(tp,c50705071.spfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
	Duel.ShuffleDeck(tp)
end
