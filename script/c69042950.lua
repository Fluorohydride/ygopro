--バグ・ロード
function c69042950.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c69042950.target)
	e1:SetOperation(c69042950.activate)
	c:RegisterEffect(e1)
end
function c69042950.mfilter(c,clv)
	return c:IsFaceup() and c:GetLevel()==clv
end
function c69042950.mfilter2(c)
	return c:IsFaceup() and c:IsLevelBelow(4)
end
function c69042950.spfilter(c,e,tp)
	local lv=c:GetLevel()
	return lv>0 and lv<=4 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and Duel.IsExistingMatchingCard(c69042950.mfilter,tp,LOCATION_MZONE,0,1,nil,lv)
end
function c69042950.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c69042950.spfilter,tp,LOCATION_HAND,0,1,nil,e,tp)
		and Duel.IsExistingMatchingCard(c69042950.mfilter2,tp,0,LOCATION_MZONE,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c69042950.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c69042950.spfilter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
		if g:GetCount()~=0 then
			Duel.SpecialSummonStep(g:GetFirst(),0,tp,tp,false,false,POS_FACEUP)
		end
	end
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c69042950.spfilter,1-tp,LOCATION_HAND,0,1,nil,e,1-tp)
		and Duel.SelectYesNo(1-tp,aux.Stringid(69042950,0)) then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(1-tp,c69042950.spfilter,1-tp,LOCATION_HAND,0,1,1,nil,e,1-tp)
		if g:GetCount()~=0 then
			Duel.SpecialSummonStep(g:GetFirst(),0,1-tp,1-tp,false,false,POS_FACEUP)
		end
	end
	Duel.SpecialSummonComplete()
end
