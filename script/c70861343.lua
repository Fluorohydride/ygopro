--忍法 変化の術
function c70861343.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0,TIMING_DRAW_PHASE)
	e1:SetTarget(c70861343.target)
	e1:SetOperation(c70861343.operation)
	c:RegisterEffect(e1)
	--Destroy
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c70861343.desop)
	c:RegisterEffect(e2)
end
function c70861343.cfilter(c,e,tp)
	local lv=c:GetLevel()
	return c:IsFaceup() and lv>0 and c:IsSetCard(0x2b)
		and Duel.IsExistingMatchingCard(c70861343.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,nil,lv+3,e,tp)
end
function c70861343.filter(c,lv,e,tp)
	return c:IsLevelBelow(lv) and c:IsRace(RACE_BEAST+RACE_WINDBEAST+RACE_INSECT) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c70861343.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.CheckReleaseGroup(tp,c70861343.cfilter,1,nil,e,tp) end
	local rg=Duel.SelectReleaseGroup(tp,c70861343.cfilter,1,1,nil,e,tp)
	e:SetLabel(rg:GetFirst():GetLevel()+3)
	Duel.Release(rg,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_DECK)
end
function c70861343.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local c=e:GetHandler()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c70861343.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,1,nil,e:GetLabel(),e,tp)
	local tc=g:GetFirst()
	if tc then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		c:SetCardTarget(tc)
	end
end
function c70861343.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetHandler():GetFirstCardTarget()
	if tc and tc:IsLocation(LOCATION_MZONE) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
