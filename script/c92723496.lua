--竜宮のツガイ
function c92723496.initial_effect(c)
	--spsummon
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1,92723496)
	e3:SetCost(c92723496.spcost)
	e3:SetTarget(c92723496.sptg)
	e3:SetOperation(c92723496.spop)
	c:RegisterEffect(e3)
end
function c92723496.costfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsDiscardable()
end
function c92723496.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c92723496.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c92723496.costfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c92723496.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsRace(RACE_WYRM) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c92723496.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c92723496.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c92723496.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c92723496.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
