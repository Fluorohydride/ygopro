--バスター・モード
function c80280737.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE+TIMING_END_PHASE)
	e1:SetCost(c80280737.cost)
	e1:SetTarget(c80280737.target)
	e1:SetOperation(c80280737.activate)
	c:RegisterEffect(e1)
end
c80280737.list={[44508094]=61257789,[70902743]=77336644,[6021033]=1764972,
				[31924889]=14553285,[23693634]=38898779,[95526884]=37169670}
function c80280737.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c80280737.filter1(c,e,tp)
	local code=c:GetCode()
	local tcode=c80280737.list[code]
	return tcode and c:IsType(TYPE_SYNCHRO) and Duel.IsExistingMatchingCard(c80280737.filter2,tp,LOCATION_DECK,0,1,nil,tcode,e,tp)
end
function c80280737.filter2(c,tcode,e,tp)
	return c:IsCode(tcode) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c80280737.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.CheckReleaseGroup(tp,c80280737.filter1,1,nil,e,tp)
	end
	local rg=Duel.SelectReleaseGroup(tp,c80280737.filter1,1,1,nil,e,tp)
	e:SetLabel(rg:GetFirst():GetCode())
	Duel.Release(rg,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c80280737.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local code=e:GetLabel()
	local tcode=c80280737.list[code]
	local tc=Duel.GetFirstMatchingCard(c80280737.filter2,tp,LOCATION_DECK,0,nil,tcode,e,tp)
	if tc and Duel.SpecialSummon(tc,0,tp,tp,true,false,POS_FACEUP_ATTACK)>0 then
		tc:CompleteProcedure()
	end
end
