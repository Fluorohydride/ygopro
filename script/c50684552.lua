--ワンダーガレージ
function c50684552.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(50684552,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c50684552.spcon)
	e1:SetTarget(c50684552.sptg)
	e1:SetOperation(c50684552.spop)
	c:RegisterEffect(e1)
end
function c50684552.spcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_DESTROY)~=0
		and e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
		and e:GetHandler():IsPreviousPosition(POS_FACEDOWN)
end
function c50684552.spfilter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsSetCard(0x16) and c:IsRace(RACE_MACHINE) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c50684552.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c50684552.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c50684552.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c50684552.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
	if g:GetCount()~=0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
