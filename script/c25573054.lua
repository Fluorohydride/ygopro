--進化する翼
function c25573054.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_BATTLE_START)
	e1:SetCost(c25573054.cost)
	e1:SetTarget(c25573054.target)
	e1:SetOperation(c25573054.activate)
	c:RegisterEffect(e1)
end
function c25573054.tgfilter(c)
	return c:IsCode(57116033) and c:IsAbleToGraveAsCost()
end
function c25573054.spfilter(c,e,tp)
	return c:IsCode(98585345) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c25573054.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local sg=Duel.GetMatchingGroup(c25573054.spfilter,tp,LOCATION_DECK+LOCATION_HAND,0,nil,e,tp)
	local hg=Duel.GetMatchingGroup(Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,e:GetHandler())
	if chk==0 then
		if sg:GetCount()==0 then return false end
		local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
		if ft<-1 then return false end
		if ft==0 then
			if not Duel.IsExistingMatchingCard(c25573054.tgfilter,tp,LOCATION_MZONE,0,1,nil) then return false end
		else
			if not Duel.IsExistingMatchingCard(c25573054.tgfilter,tp,LOCATION_ONFIELD,0,1,nil) then return false end
		end
		if sg:IsExists(Card.IsLocation,1,nil,LOCATION_DECK) then
			return hg:GetCount()>1
		else
			return hg:GetCount()>2
		end
	end
	local cg=nil
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		cg=Duel.SelectMatchingCard(tp,c25573054.tgfilter,tp,LOCATION_MZONE,0,1,1,nil)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		cg=Duel.SelectMatchingCard(tp,c25573054.tgfilter,tp,LOCATION_ONFIELD,0,1,1,nil)
	end
	local ct=sg:GetCount()
	if ct>2 or sg:IsExists(Card.IsLocation,1,nil,LOCATION_DECK) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=hg:Select(tp,2,2,nil)
		cg:Merge(g)
	elseif ct==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=hg:Select(tp,2,2,sg:GetFirst())
		cg:Merge(g)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g1=hg:Select(tp,1,1,nil)
		if sg:IsContains(g1:GetFirst()) then
			hg:Sub(sg)
		end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g2=hg:Select(tp,1,1,g1:GetFirst())
		cg:Merge(g1)
		cg:Merge(g2)
	end
	Duel.SendtoGrave(cg,REASON_COST)
end
function c25573054.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,LOCATION_DECK+LOCATION_HAND)
end
function c25573054.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c25573054.spfilter,tp,LOCATION_DECK+LOCATION_HAND,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,true,false,POS_FACEUP)
		tc:CompleteProcedure()
	end
end
