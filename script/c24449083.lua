--コート·オブ·ジャスティス
function c24449083.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(24449083,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_SZONE)
	e1:SetCondition(c24449083.condition)
	e1:SetTarget(c24449083.target)
	e1:SetOperation(c24449083.operation)
	c:RegisterEffect(e1)
end
function c24449083.cfilter(c)
	return c:IsFaceup() and c:GetLevel()==1 and c:IsRace(RACE_FAIRY)
end
function c24449083.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c24449083.cfilter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.GetFlagEffect(tp,24449083)==0
end
function c24449083.filter(c,e,sp)
	return c:IsRace(RACE_FAIRY) and c:IsCanBeSpecialSummoned(e,0,sp,false,false)
end
function c24449083.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingTarget(c24449083.filter,tp,LOCATION_HAND,0,1,nil,e,tp)
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
	Duel.RegisterFlagEffect(tp,24449083,RESET_PHASE+PHASE_END,0,1)
end
function c24449083.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if not Duel.IsExistingMatchingCard(c24449083.cfilter,tp,LOCATION_MZONE,0,1,nil) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c24449083.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
