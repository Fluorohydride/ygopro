--冥界流傀儡術
function c40703393.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c40703393.target)
	e1:SetOperation(c40703393.activate)
	c:RegisterEffect(e1)
end
function c40703393.spfilter(c,e,tp,ft,rg)
	local lv=c:GetLevel()
	return lv>0 and c:IsRace(RACE_FIEND) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
		and rg:CheckWithSumEqual(Card.GetLevel,lv,ft,99)
end
function c40703393.rmfilter(c)
	return c:GetLevel()>0 and c:IsAbleToRemove()
end
function c40703393.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then ft=-ft+1 else ft=1 end
	if chk==0 then
		local rg=Duel.GetMatchingGroup(c40703393.rmfilter,tp,LOCATION_MZONE,0,nil)
		return Duel.IsExistingTarget(c40703393.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp,ft,rg)
	end
	local rg=Duel.GetMatchingGroup(c40703393.rmfilter,tp,LOCATION_MZONE,0,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c40703393.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp,ft,rg)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
end
function c40703393.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then ft=-ft+1 else ft=1 end
	if not tc:IsRelateToEffect(e) or not tc:IsCanBeSpecialSummoned(e,0,tp,false,false) then return end
	local rg=Duel.GetMatchingGroup(c40703393.rmfilter,tp,LOCATION_MZONE,0,nil)
	local lv=tc:GetLevel()
	if rg:CheckWithSumEqual(Card.GetLevel,lv,ft,99) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local rm=rg:SelectWithSumEqual(tp,Card.GetLevel,lv,ft,99)
		Duel.Remove(rm,POS_FACEUP,REASON_EFFECT)
		Duel.BreakEffect()
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
