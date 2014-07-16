--レジェンド・オブ・ハート
function c89397517.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,89397517+EFFECT_COUNT_CODE_OATH)
	e1:SetCost(c89397517.cost)
	e1:SetTarget(c89397517.target)
	e1:SetOperation(c89397517.activate)
	c:RegisterEffect(e1)
end
function c89397517.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,2000) and Duel.CheckReleaseGroup(tp,Card.IsRace,1,nil,RACE_WARRIOR) end
	Duel.PayLPCost(tp,2000)
	local sg=Duel.SelectReleaseGroup(tp,Card.IsRace,1,1,nil,RACE_WARRIOR)
	Duel.Release(sg,REASON_COST)
end
function c89397517.rmfilter(c)
	return c:IsSetCard(0xa1) and c:IsType(TYPE_SPELL) and c:IsAbleToRemove()
end
function c89397517.spfilter(c,e,tp)
	return c:IsSetCard(0xa0) and c:IsType(TYPE_MONSTER) and c:IsCanBeSpecialSummoned(e,0,tp,true,true)
end
function c89397517.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c89397517.rmfilter,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,nil)
		and Duel.IsExistingMatchingCard(c89397517.spfilter,tp,LOCATION_HAND+LOCATION_GRAVE+LOCATION_DECK,0,1,nil,e,tp) end
	local g=Duel.GetMatchingGroup(c89397517.rmfilter,tp,LOCATION_HAND+LOCATION_GRAVE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND+LOCATION_GRAVE+LOCATION_DECK)
end
function c89397517.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 then return end
	local rmg=Duel.GetMatchingGroup(c89397517.rmfilter,tp,LOCATION_HAND+LOCATION_GRAVE,0,nil)
	local rmct=rmg:GetClassCount(Card.GetCode)
	local spg=Duel.GetMatchingGroup(c89397517.spfilter,tp,LOCATION_HAND+LOCATION_GRAVE+LOCATION_DECK,0,nil,e,tp)
	local spct=spg:GetClassCount(Card.GetCode)
	local ct=math.min(3,ft,spct,rmct)
	if ct==0 then return end
	local g=Group.CreateGroup()
	repeat
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local tc=rmg:Select(tp,1,1,nil):GetFirst()
		rmg:Remove(Card.IsCode,nil,tc:GetCode())
		g:AddCard(tc)
		ct=ct-1
	until ct<1 or not Duel.SelectYesNo(tp,aux.Stringid(89397517,1))
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	ct=g:FilterCount(Card.IsLocation,nil,LOCATION_REMOVED)
	while ct>0 do
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local tc=spg:Select(tp,1,1,nil):GetFirst()
		spg:Remove(Card.IsCode,nil,tc:GetCode())
		Duel.SpecialSummonStep(tc,0,tp,tp,true,true,POS_FACEUP)
		tc:CompleteProcedure()
		ct=ct-1
	end
	Duel.SpecialSummonComplete()
end
