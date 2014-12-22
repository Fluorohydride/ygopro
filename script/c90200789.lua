--忍法 超変化の術
function c90200789.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetTarget(c90200789.target)
	e1:SetOperation(c90200789.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c90200789.desop)
	c:RegisterEffect(e2)
end
function c90200789.filter1(c,tp,slv)
	local lv1=c:GetLevel()
	return c:IsFaceup() and c:IsSetCard(0x2b) and lv1>0
		and Duel.IsExistingTarget(c90200789.filter2,tp,0,LOCATION_MZONE,1,nil,lv1,slv)
end
function c90200789.filter2(c,lv1,slv)
	local lv2=c:GetLevel()
	return c:IsFaceup() and lv2>0 and lv1+lv2>=slv
end
function c90200789.spfilter(c,e,tp,lv)
	return c:IsRace(RACE_DRAGON+RACE_DINOSAUR+RACE_SEASERPENT) and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and (not lv or c:IsLevelBelow(lv))
end
function c90200789.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local sg=Duel.GetMatchingGroup(c90200789.spfilter,tp,LOCATION_DECK,0,nil,e,tp)
	if chk==0 then
		if sg:GetCount()==0 then return false end
		local mg,mlv=sg:GetMinGroup(Card.GetLevel)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
			and Duel.IsExistingTarget(c90200789.filter1,tp,LOCATION_MZONE,0,1,nil,tp,mlv)
	end
	local mg,mlv=sg:GetMinGroup(Card.GetLevel)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g1=Duel.SelectTarget(tp,c90200789.filter1,tp,LOCATION_MZONE,0,1,1,nil,tp,mlv)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g2=Duel.SelectTarget(tp,c90200789.filter2,tp,0,LOCATION_MZONE,1,1,nil,g1:GetFirst():GetLevel(),mlv)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g1,2,0,0)
end
function c90200789.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tg=g:Filter(Card.IsRelateToEffect,nil,e)
	if tg:GetCount()==0 then return end
	Duel.SendtoGrave(tg,REASON_EFFECT)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=tg:GetFirst()
	local lv=0
	if tc:IsLocation(LOCATION_GRAVE) then lv=lv+tc:GetLevel() end
	tc=tg:GetNext()
	if tc and tc:IsLocation(LOCATION_GRAVE) then lv=lv+tc:GetLevel() end
	if lv==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c90200789.spfilter,tp,LOCATION_DECK,0,1,1,nil,e,tp,lv)
	local tc=g:GetFirst()
	if tc then
		Duel.BreakEffect()
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		c:SetCardTarget(tc)
	end
end
function c90200789.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	if tc and tc:IsLocation(LOCATION_MZONE) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
