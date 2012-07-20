--メンタルマスター
function c96782886.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(96782886,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c96782886.cost)
	e1:SetTarget(c96782886.target)
	e1:SetOperation(c96782886.operation)
	c:RegisterEffect(e1)
end
function c96782886.costfilter(c)
	return c:IsRace(RACE_PSYCHO) and c:GetCode()~=96782886
end
function c96782886.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) and Duel.CheckReleaseGroup(tp,c96782886.costfilter,1,nil) end
	Duel.PayLPCost(tp,800)
	local sg=Duel.SelectReleaseGroup(tp,c96782886.costfilter,1,1,nil)
	Duel.Release(sg,REASON_COST)
end
function c96782886.filter(c,e,tp)
	return c:IsRace(RACE_PSYCHO) and c:IsLevelBelow(4) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c96782886.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c96782886.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c96782886.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c96782886.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)	end
end
