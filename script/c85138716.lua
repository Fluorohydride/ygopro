--レスキューラビット
function c85138716.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_DECK)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(85138716,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c85138716.spcost)
	e1:SetTarget(c85138716.sptg)
	e1:SetOperation(c85138716.spop)
	c:RegisterEffect(e1)
end
function c85138716.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,85138716)==0 and e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
	Duel.RegisterFlagEffect(tp,85138716,RESET_PHASE+PHASE_END,0,1)
end
function c85138716.filter(c,e,tp)
	return c:IsType(TYPE_NORMAL) and c:GetLevel()<=4 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c85138716.filter2(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c85138716.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c85138716.filter,tp,LOCATION_DECK,0,nil,e,tp)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and g:IsExists(c85138716.filter2,1,nil,g)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,tp,LOCATION_DECK)
end
function c85138716.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2 then return end
	local g=Duel.GetMatchingGroup(c85138716.filter,tp,LOCATION_DECK,0,nil,e,tp)
	local dg=g:Filter(c85138716.filter2,nil,g)
	if dg:GetCount()>=1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=dg:Select(tp,1,1,nil)
		local tc1=sg:GetFirst()
		dg:RemoveCard(tc1)
		local tc2=dg:Filter(Card.IsCode,nil,tc1:GetCode()):GetFirst()
		Duel.SpecialSummonStep(tc1,0,tp,tp,false,false,POS_FACEUP)
		Duel.SpecialSummonStep(tc2,0,tp,tp,false,false,POS_FACEUP)
		tc1:RegisterFlagEffect(85138716,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		tc2:RegisterFlagEffect(85138716,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		Duel.SpecialSummonComplete()
		sg:AddCard(tc2)
		sg:KeepAlive()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetReset(RESET_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetLabelObject(sg)
		e1:SetOperation(c85138716.desop)
		Duel.RegisterEffect(e1,tp)
	end
end
function c85138716.desfilter(c)
	return c:GetFlagEffect(85138716)>0
end
function c85138716.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tg=g:Filter(c85138716.desfilter,nil)
	g:DeleteGroup()
	Duel.Destroy(tg,REASON_EFFECT)
end
