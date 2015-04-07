--サイバーダーク・インパクト！
function c80033124.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c80033124.target)
	e1:SetOperation(c80033124.activate)
	c:RegisterEffect(e1)
end
function c80033124.ffilter0(c)
	local code=c:GetCode()
	return (code==41230939 or code==77625948 or code==3019642) and c:IsAbleToDeck() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c80033124.ffilter(c,e)
	local code=c:GetCode()
	return (code==41230939 or code==77625948 or code==3019642) and c:IsAbleToDeck()
		and not c:IsImmuneToEffect(e) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c80033124.spfilter(c,e,tp)
	return c:IsCode(40418351) and c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_FUSION,tp,false,false)
end
function c80033124.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if not Duel.IsExistingMatchingCard(c80033124.spfilter,tp,LOCATION_EXTRA,0,1,nil,e,tp) then return false end
		local g=Duel.GetMatchingGroup(c80033124.ffilter0,tp,0x1e,0,nil)
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 and not g:IsExists(Card.IsLocation,1,nil,LOCATION_MZONE) then return false end
		return g:IsExists(Card.IsCode,1,nil,41230939)
			and g:IsExists(Card.IsCode,1,nil,77625948)
			and g:IsExists(Card.IsCode,1,nil,3019642)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c80033124.cfilter(c)
	return c:IsLocation(LOCATION_HAND) or (c:IsOnField() and c:IsFacedown())
end
function c80033124.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c80033124.ffilter,tp,0x1e,0,nil,e)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if (ft<=0 and not g:IsExists(Card.IsLocation,1,nil,LOCATION_MZONE))
		or not g:IsExists(Card.IsCode,1,nil,41230939)
		or not g:IsExists(Card.IsCode,1,nil,77625948)
		or not g:IsExists(Card.IsCode,1,nil,3019642) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	if ft<=0 then g1=g:FilterSelect(tp,Card.IsLocation,1,1,nil,LOCATION_MZONE)
	else g1=g:Select(tp,1,1,nil) end
	g:Remove(Card.IsCode,nil,g1:GetFirst():GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g2=g:Select(tp,1,1,nil)
	g:Remove(Card.IsCode,nil,g2:GetFirst():GetCode())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g3=g:Select(tp,1,1,nil)
	g1:Merge(g2)
	g1:Merge(g3)
	local cg=g1:Filter(c80033124.cfilter,nil)
	if cg:GetCount()>0 then
		Duel.ConfirmCards(1-tp,cg)
		Duel.ShuffleHand(tp)
	end
	Duel.SendtoDeck(g1,nil,2,REASON_EFFECT)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=Duel.SelectMatchingCard(tp,c80033124.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp)
	Duel.SpecialSummon(sg,SUMMON_TYPE_FUSION,tp,tp,false,false,POS_FACEUP)
	sg:GetFirst():CompleteProcedure()
end
