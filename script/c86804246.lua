--スーパーバグマン
function c86804246.initial_effect(c)
	c:EnableReviveLimit()
	c:SetUniqueOnField(1,1,86804246)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
	e1:SetRange(LOCATION_HAND)
	e1:SetTargetRange(POS_FACEUP_DEFENCE,0)
	e1:SetCondition(c86804246.spcon)
	e1:SetOperation(c86804246.spop)
	c:RegisterEffect(e1)
	--swap ad
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SWAP_AD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c86804246.adfilter)
	c:RegisterEffect(e2)
end
function c86804246.spfilter(c,code)
	return c:IsCode(code) and c:IsAbleToRemoveAsCost()
end
function c86804246.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c86804246.spfilter,tp,LOCATION_GRAVE,0,1,nil,87526784)
		and Duel.IsExistingMatchingCard(c86804246.spfilter,tp,LOCATION_GRAVE,0,1,nil,23915499)
		and Duel.IsExistingMatchingCard(c86804246.spfilter,tp,LOCATION_GRAVE,0,1,nil,50319138)
end
function c86804246.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g1=Duel.SelectMatchingCard(tp,c86804246.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,87526784)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g2=Duel.SelectMatchingCard(tp,c86804246.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,23915499)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g3=Duel.SelectMatchingCard(tp,c86804246.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,50319138)
	g1:Merge(g2)
	g1:Merge(g3)
	Duel.Remove(g1,POS_FACEUP,REASON_COST)
end
function c86804246.adfilter(e,c)
	return c:IsPosition(POS_FACEUP_ATTACK)
end
