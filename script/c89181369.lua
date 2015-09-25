--星屑のきらめき
function c89181369.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c89181369.target)
	e1:SetOperation(c89181369.activate)
	c:RegisterEffect(e1)
end
function c89181369.spfilter(c,e,tp,rg)
	if not c:IsType(TYPE_SYNCHRO) or not c:IsRace(RACE_DRAGON) or not c:IsCanBeSpecialSummoned(e,0,tp,false,false) then return false end
	if rg:IsContains(c) then
		rg:RemoveCard(c)
		result=rg:CheckWithSumEqual(Card.GetLevel,c:GetLevel(),1,99)
		rg:AddCard(c)
	else
		result=rg:CheckWithSumEqual(Card.GetLevel,c:GetLevel(),1,99)
	end
	return result
end
function c89181369.rmfilter(c)
	return c:GetLevel()>0 and c:IsAbleToRemove()
end
function c89181369.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return false end
		local rg=Duel.GetMatchingGroup(c89181369.rmfilter,tp,LOCATION_GRAVE,0,nil)
		return Duel.IsExistingTarget(c89181369.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp,rg)
	end
	local rg=Duel.GetMatchingGroup(c89181369.rmfilter,tp,LOCATION_GRAVE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c89181369.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp,rg)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c89181369.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) or Duel.GetLocationCount(tp,LOCATION_MZONE)<=0
		or not tc:IsCanBeSpecialSummoned(e,0,tp,false,false) then return end
	local rg=Duel.GetMatchingGroup(c89181369.rmfilter,tp,LOCATION_GRAVE,0,nil)
	rg:RemoveCard(tc)
	if rg:CheckWithSumEqual(Card.GetLevel,tc:GetLevel(),1,99) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local rm=rg:SelectWithSumEqual(tp,Card.GetLevel,tc:GetLevel(),1,99)
		Duel.Remove(rm,POS_FACEUP,REASON_EFFECT)
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
