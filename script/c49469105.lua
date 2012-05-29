--融合破棄
function c49469105.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c49469105.cost)
	e1:SetTarget(c49469105.target)
	e1:SetOperation(c49469105.operation)
	c:RegisterEffect(e1)
end
function c49469105.cfilter(c)
	return c:IsCode(24094653) and c:IsDiscardable() and c:IsAbleToGraveAsCost()
end
function c49469105.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c49469105.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c49469105.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c49469105.filter2(c,fc)
	local fd=c:GetCode()
	for i=1,fc.material_count do
		if fd==fc.material[i] then return true end
	end
	return false
end
function c49469105.filter1(c,g)
	local ct=c.material_count
	return ct~=nil and g:IsExists(c49469105.filter2,1,nil,c)
end
function c49469105.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c49469105.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local g=Duel.GetMatchingGroup(c49469105.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c49469105.filter1,tp,LOCATION_EXTRA,0,1,nil,g) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c49469105.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local g=Duel.GetMatchingGroup(c49469105.spfilter,tp,LOCATION_HAND,0,nil,e,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local exg=Duel.SelectMatchingCard(tp,c49469105.filter1,tp,LOCATION_EXTRA,0,1,1,nil,g)
	if exg:GetCount()>0 then
		Duel.SendtoGrave(exg,REASON_EFFECT)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=g:FilterSelect(tp,c49469105.filter2,1,1,nil,exg:GetFirst())
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetOperation(c49469105.tgop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		sg:GetFirst():RegisterEffect(e1,true)
	end
end
function c49469105.tgop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
end
