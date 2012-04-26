--コンタクト
function c16616620.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c16616620.target)
	e1:SetOperation(c16616620.activate)
	c:RegisterEffect(e1)
end
c16616620.list={[42682609]=17955766,[43751755]=43237273,[17363041]=54959865,
				[29246354]=17732278,[16241441]=89621922,[42239546]=80344569}
function c16616620.filter1(c,e,tp)
	local code=c:GetCode()
	local tcode=c16616620.list[code]
	return tcode and Duel.IsExistingMatchingCard(c16616620.filter2,tp,LOCATION_HAND+LOCATION_DECK,0,1,nil,tcode,e,tp)
end
function c16616620.filter2(c,tcode,e,tp)
	return c:IsCode(tcode) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c16616620.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c16616620.filter1,tp,LOCATION_MZONE,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_DECK)
end
function c16616620.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsSetCard,tp,LOCATION_MZONE,0,nil,0x1e)
	if g:GetCount()==0 then return end
	Duel.SendtoGrave(g,REASON_EFFECT)
	local sg=Group.CreateGroup()
	local tc=g:GetFirst()
	while tc do
		local code=tc:GetCode()
		local tcode=c16616620.list[code]
		local tg=Duel.GetMatchingGroup(c16616620.filter2,tp,LOCATION_HAND+LOCATION_DECK,0,nil,tcode,e,tp)
		sg:Merge(tg)
		tc=g:GetNext()
	end
	if sg:GetCount()>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local spg=sg:Select(tp,1,1,nil)
		Duel.SpecialSummon(spg,0,tp,tp,false,false,POS_FACEUP)
	end
end
