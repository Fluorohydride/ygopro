--ハンディキャップマッチ！
function c67223587.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c67223587.condition)
	e1:SetTarget(c67223587.target)
	e1:SetOperation(c67223587.activate)
	c:RegisterEffect(e1)
end
function c67223587.cfilter(c,tp)
	return c:IsFaceup() and c:GetSummonPlayer()==tp and c:IsSetCard(0x19)
end
function c67223587.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c67223587.cfilter,1,nil,tp)
end
function c67223587.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsSetCard(0x19) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c67223587.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c67223587.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,0,tp,LOCATION_HAND+LOCATION_DECK)
end
function c67223587.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c67223587.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,1,nil,e,tp)
	if g:GetCount()>0 then
		Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
	end
end
