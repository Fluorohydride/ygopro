--ナチュル・アントジョー
function c99150062.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(99150062,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c99150062.spcon)
	e1:SetTarget(c99150062.sptg)
	e1:SetOperation(c99150062.spop)
	c:RegisterEffect(e1)
end
function c99150062.cfilter(c,tp)
	return c:GetSummonPlayer()~=tp
end
function c99150062.spcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c99150062.cfilter,1,nil,tp)
end
function c99150062.filter(c,e,tp)
	return c:IsSetCard(0x2a) and c:GetLevel()<=3 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c99150062.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c99150062.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c99150062.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c99150062.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
	Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
end
