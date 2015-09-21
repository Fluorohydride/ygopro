--魂の綱
function c37383714.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c37383714.condition)
	e1:SetCost(c37383714.cost)
	e1:SetTarget(c37383714.target)
	e1:SetOperation(c37383714.activate)
	c:RegisterEffect(e1)
end
function c37383714.cfilter(c,tp)
	return c:IsReason(REASON_EFFECT) and c:IsReason(REASON_DESTROY) and c:IsType(TYPE_MONSTER)
		and c:GetPreviousControler()==tp and c:IsPreviousLocation(LOCATION_MZONE)
end
function c37383714.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c37383714.cfilter,1,nil,tp)
end
function c37383714.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c37383714.spfilter(c,e,tp)
	return c:GetLevel()==4 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c37383714.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c37383714.spfilter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c37383714.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c37383714.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
